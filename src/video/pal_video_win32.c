
// ==================================================
// Includes
// ==================================================

#include "pal/pal_video.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

// set unicode
#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include <windows.h>

// ==================================================
// Typedefs, enums and structs
// ==================================================

#define PAL_VIDEO_CLASS L"PALVideoClass"
#define PAL_VIDEO_PROP L"PALVideo"
#define WIN32_DPI 0
#define WIN32_DPI_AWARE 2
#define MAX_MODE_COUNT 128
#define NULL_ORIENTATION 5

typedef HRESULT (WINAPI* GetDpiForMonitorFn)(
    HMONITOR, 
    Int32, 
    UINT*, 
    UINT*);

typedef HRESULT (WINAPI* SetProcessAwarenessFn)(
    Int32);

typedef struct {
    bool initialized;
    PalVideoFeatures features;
    const PalAllocator* allocator;
    GetDpiForMonitorFn getDpiForMonitor;
    SetProcessAwarenessFn setProcessAwareness;
    HINSTANCE shcore;
    HINSTANCE instance;
} VideoWin32;

typedef struct {
    Int32 count;
    Int32 maxCount;
    PalDisplay** displays;
} DisplayData;

static VideoWin32 s_Video = {};

// ==================================================
// Internal API
// ==================================================

LRESULT CALLBACK videoProc(
    HWND hwnd, 
    UINT msg, 
    WPARAM wParam, 
    LPARAM lParam) {

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

BOOL CALLBACK enumMonitors(
    HMONITOR monitor, 
    HDC hdc, 
    LPRECT lRect, 
    LPARAM lParam) {
    
    DisplayData* data = (DisplayData*)lParam;
    if (data->displays) {
        if (data->count < data->maxCount) {
            data->displays[data->count] = (PalDisplay*)monitor;
        }
    }

    data->count++;
    return TRUE;
}

static inline PalOrientation orientationFromWin32(
    DWORD orientation) {
    
    switch (orientation) {
        case DMDO_DEFAULT: 
        return PAL_ORIENTATION_LANDSCAPE;

        case DMDO_90: 
        return PAL_ORIENTATION_PORTRAIT;

        case DMDO_180: 
        return PAL_ORIENTATION_LANDSCAPE_FLIPPED;

        case DMDO_270: 
        return PAL_ORIENTATION_PORTRAIT_FLIPPED;
    }
    return PAL_ORIENTATION_LANDSCAPE;
}

static inline DWORD orientationToin32(
    PalOrientation orientation) {

    switch (orientation) {
        case PAL_ORIENTATION_LANDSCAPE: 
        return DMDO_DEFAULT;

        case PAL_ORIENTATION_PORTRAIT: 
        return DMDO_90;

        case PAL_ORIENTATION_LANDSCAPE_FLIPPED: 
        return DMDO_180;

        case PAL_ORIENTATION_PORTRAIT_FLIPPED:
        return DMDO_270;
    }
    return NULL_ORIENTATION;
}

static PalResult setDisplayMode(
    PalDisplay* display,
    PalDisplayMode* mode,
    bool test) {
    
    if (!display || !mode) {
        return PAL_RESULT_NULL_POINTER;
    } 

    HMONITOR monitor = (HMONITOR)display;
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_DISPLAY;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    DWORD flags = DM_PELSWIDTH | DM_PELSHEIGHT;
    flags |= DM_DISPLAYFREQUENCY | DM_BITSPERPEL;

    DEVMODE devMode = {};
    devMode.dmSize = sizeof(DEVMODE);
    devMode.dmFields = flags;

    devMode.dmPelsWidth = mode->width;
    devMode.dmPelsHeight = mode->height;
    devMode.dmDisplayFrequency = mode->refreshRate;
    devMode.dmBitsPerPel = mode->bpp;

    DWORD settingsFlag = CDS_FULLSCREEN;
    if (test) {
        settingsFlag = CDS_TEST;
    }

    ULONG result = ChangeDisplaySettingsExW(
        mi.szDevice,
        &devMode,
        NULL,
        settingsFlag,
        NULL
    );

    if (result == DISP_CHANGE_SUCCESSFUL) {
        return PAL_RESULT_SUCCESS;
    } else {
        return PAL_RESULT_INVALID_DISPLAY_MODE;
    }
}

static inline bool compareDisplayMode(
    const PalDisplayMode* a, 
    const PalDisplayMode* b) {

    return 
        a->bpp == b->bpp               &&
        a->width == b->width           &&
        a->height == b->height         &&
        a->refreshRate == b->refreshRate;
}

static void addDisplayMode(
    PalDisplayMode* modes, 
    const PalDisplayMode* mode, 
    Int32* count) {

    // check if we have a duplicate mode
    for (Int32 i = 0; i < *count; i++) {
        PalDisplayMode* oldMode = &modes[i];
        if (compareDisplayMode(oldMode, mode)) {
            return; // discard it
        }
    }

    // new mode
    modes[*count] = *mode;
    *count += 1;
}

// ==================================================
// Public API
// ==================================================

PalResult PAL_CALL palInitVideo(
    const PalAllocator *allocator) {
    
    if (s_Video.initialized) {
        return PAL_RESULT_SUCCESS;
    }
    
    if (allocator && (!allocator->allocate || !allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    // get the instance
    s_Video.instance = GetModuleHandleW(nullptr);

    // register class
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.hCursor = LoadCursorW(s_Video.instance, IDC_ARROW);
    wc.hIcon = LoadIconW(s_Video.instance, IDI_APPLICATION);
    wc.hIconSm = LoadIconW(s_Video.instance, IDI_APPLICATION);
    wc.hInstance = s_Video.instance;
    wc.lpfnWndProc = videoProc;
    wc.lpszClassName = PAL_VIDEO_CLASS;
    wc.style = CS_OWNDC;

    // since we check every input carefully, the only error we can get is access denied
    if (!RegisterClassExW(&wc)) {
        return PAL_RESULT_ACCESS_DENIED;
    }

    // load shared libraries
    s_Video.shcore = LoadLibraryA("shcore.dll");
    if (s_Video.shcore) {
        s_Video.getDpiForMonitor = (GetDpiForMonitorFn)GetProcAddress(
            s_Video.shcore,
            "GetDpiForMonitor"
        );

        s_Video.setProcessAwareness = (SetProcessAwarenessFn)GetProcAddress(
            s_Video.shcore,
            "SetProcessDpiAwareness"
        );
    }

    // set features
    s_Video.features |= PAL_VIDEO_FEATURE_DISPLAY_ORIENTATION;
    s_Video.features |= PAL_VIDEO_FEATURE_BORDERLESS_WINDOW;
    s_Video.features |= PAL_VIDEO_FEATURE_DISPLAY_MODE_SWITCH;
    s_Video.features |= PAL_VIDEO_FEATURE_MULTI_DISPLAYS;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_RESIZING;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_POSITIONING;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_MINMAX;
    s_Video.features |= PAL_VIDEO_FEATURE_DISPLAY_GAMMA_CONTROL;
    s_Video.features |= PAL_VIDEO_FEATURE_CLIP_CURSOR;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY;

    if (s_Video.getDpiForMonitor && s_Video.setProcessAwareness) {
        s_Video.features |= PAL_VIDEO_FEATURE_HIGH_DPI;
    }

    s_Video.initialized = true;
    s_Video.allocator = allocator;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownVideo() {

    if (!s_Video.initialized) {
        return;
    }

    UnregisterClassW(PAL_VIDEO_CLASS, s_Video.instance);
    s_Video.initialized = false;
}

PalVideoFeatures PAL_CALL palGetVideoFeatures() {

    if (!s_Video.initialized) {
        return 0;
    }

    return s_Video.features;
}

// ==================================================
// Display
// ==================================================

PalResult PAL_CALL palEnumerateDisplays(
    Int32 *count,
    PalDisplay **displays) {
    
    if (!count) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (count == 0 && displays) {
        PAL_RESULT_INSUFFICIENT_BUFFER;
    }

    DisplayData data;
    data.count = 0;
    data.displays = displays;
    data.maxCount = displays ? *count : 0;
    EnumDisplayMonitors(nullptr, nullptr, enumMonitors, (LPARAM)&data);

    if (!displays) {
        *count = data.count;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetPrimaryDisplay(
    PalDisplay **outDisplay) {
    
    if (!outDisplay) {
        return PAL_RESULT_NULL_POINTER;
    }

    HMONITOR monitor = MonitorFromPoint((POINT){0, 0}, MONITOR_DEFAULTTOPRIMARY);
    if (!monitor) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    *outDisplay = (PalDisplay*)monitor;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetDisplayInfo(
    PalDisplay *display,
    PalDisplayInfo *info) {

    if (!display || !info) {
        return PAL_RESULT_NULL_POINTER;
    }

    HMONITOR monitor = (HMONITOR)display;
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_DISPLAY;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    info->x = mi.rcMonitor.left;
    info->y = mi.rcMonitor.top;
    info->width = mi.rcMonitor.right - mi.rcMonitor.left;
    info->height = mi.rcMonitor.bottom - mi.rcWork.top;

    // get name
    WideCharToMultiByte(CP_UTF8, 0, mi.szDevice, -1, info->name, 32, NULL, NULL);

    DEVMODE devMode = {};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    info->refreshRate = devMode.dmDisplayFrequency;   
    info->orientation = orientationFromWin32(devMode.dmDisplayOrientation);

    // get dpi scale
    Int32 dpiX, dpiY;
    if (s_Video.getDpiForMonitor || s_Video.setProcessAwareness) {
        s_Video.setProcessAwareness(WIN32_DPI_AWARE);
        s_Video.getDpiForMonitor(monitor, WIN32_DPI, &dpiX, &dpiY);
        
    } else {
        dpiX = 96;
    }    
    info->dpi = dpiX;

    // check for primary display
    HMONITOR primaryDisplay = MonitorFromPoint(
        (POINT){0, 0}, 
        MONITOR_DEFAULTTOPRIMARY
    );

    if (!primaryDisplay) {
        info->primary = false;
    }

    if (primaryDisplay == monitor) {
        info->primary = true;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palEnumerateDisplayModes(
    PalDisplay *display,
    Int32 *count,
    PalDisplayMode *modes) {
    
    if (!display || !count) {
        return PAL_RESULT_NULL_POINTER;
    }

    Int32 modeCount = 0;
    Int32 maxModes = 0;
    HMONITOR monitor = (HMONITOR)display;
    PalDisplayMode* displayModes = nullptr;

    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_DISPLAY;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    if (!modes) {
        // allocate and store tmp display modesand check for the interested fields.
        displayModes = palAllocate(
            s_Video.allocator, 
            sizeof(PalDisplayMode) * MAX_MODE_COUNT,
            0
        );

        if (!displayModes) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        memset(
            displayModes, 
            0,
            sizeof(PalDisplayMode) * MAX_MODE_COUNT
        );
        maxModes = MAX_MODE_COUNT;

    } else {
        displayModes = modes;
        maxModes = *count;
    }

    DEVMODEW dm = {};
    dm.dmSize = sizeof(DEVMODE);
    for (Int32 i = 0; EnumDisplaySettingsW(mi.szDevice, i, &dm); i++) {
        // Pal support up to 128 modes
        if (modeCount > maxModes) {
            break;
        }

        PalDisplayMode* mode = &displayModes[modeCount];
        mode->refreshRate = dm.dmDisplayFrequency;
        mode->width = dm.dmPelsWidth;
        mode->height = dm.dmPelsHeight;
        mode->bpp = dm.dmBitsPerPel;
        addDisplayMode(displayModes, mode, &modeCount);
    }

    if (!modes) {
        *count = modeCount;
        palFree(s_Video.allocator, displayModes);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetCurrentDisplayMode(
    PalDisplay *display,
    PalDisplayMode *mode) {
    
    if (!display || !mode) {
        return PAL_RESULT_NULL_POINTER;
    }

    HMONITOR monitor = (HMONITOR)display;
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_DISPLAY;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    DEVMODE devMode = {};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    mode->width = devMode.dmPelsWidth;
    mode->height = devMode.dmPelsHeight;
    mode->refreshRate = devMode.dmDisplayFrequency;
    mode->bpp = devMode.dmBitsPerPel;

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetDisplayMode(
    PalDisplay *display,
    PalDisplayMode *mode) {

    return setDisplayMode(display, mode, false);
}

PalResult PAL_CALL palValidateDisplayMode(
    PalDisplay *display,
    PalDisplayMode *mode) {
    
    return setDisplayMode(display, mode, true);
}

PalResult PAL_CALL palSetDisplayOrientation(
    PalDisplay *display,
    PalOrientation orientation) {
    
    if (!display) {
        return PAL_RESULT_NULL_POINTER;
    }

    DWORD win32Orientation = orientationToin32(orientation);
    if (orientation == NULL_ORIENTATION) {
        return PAL_RESULT_INVALID_ORIENTATION;
    }

    HMONITOR monitor = (HMONITOR)display;
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_DISPLAY;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    DEVMODE devMode = {};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    DWORD displayOrientation = devMode.dmDisplayOrientation;

    // only swap size if switching between landscape and portrait
    bool isDisplayLandscape = (displayOrientation == DMDO_DEFAULT || displayOrientation == DMDO_180);
    bool isLandscape = (win32Orientation == DMDO_DEFAULT || win32Orientation == DMDO_180);
    if (isDisplayLandscape != isLandscape) {
        DWORD tmp = devMode.dmPelsWidth;
        devMode.dmPelsWidth = devMode.dmPelsHeight;
        devMode.dmPelsHeight = tmp;
    }

    devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYORIENTATION;
    devMode.dmDisplayOrientation = win32Orientation;

    ULONG result = ChangeDisplaySettingsExW(
        mi.szDevice,
        &devMode,
        NULL,
        CDS_RESET,
        NULL
    );

    if (result == DISP_CHANGE_SUCCESSFUL) {
        return PAL_RESULT_SUCCESS;

    } else {
        return PAL_RESULT_INVALID_ORIENTATION;
    }
}