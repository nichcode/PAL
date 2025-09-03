
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