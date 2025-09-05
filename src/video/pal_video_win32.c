
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
#define WINDOW_NAME_SIZE 256

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
    PalEventDriver* eventDriver;
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

typedef struct {
    Uint32 style;
    Uint32 exStyle;
} WindowData;

static VideoWin32 s_Video = {};

// ==================================================
// Internal API
// ==================================================

LRESULT CALLBACK videoProc(
    HWND hwnd, 
    UINT msg, 
    WPARAM wParam, 
    LPARAM lParam) {
    
    switch (msg) {
        case WM_CLOSE: {
            // check if the user supplied an event driver
            if (s_Video.eventDriver) {
                // check if the user is interested in window close event
                PalEventDriver* driver = s_Video.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_WINDOW_CLOSE);
                if (mode != PAL_DISPATCH_NONE) {
                    // push a window close event
                    PalEvent event = {};
                    event.type = PAL_EVENT_WINDOW_CLOSE;
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);                
                } 
            }
            return 0;
        }

        case WM_SIZE: {

        }

        case WM_MOVE: {
            
        }

        case WM_SHOWWINDOW: {
            
        }

        case WM_SETFOCUS: {
            
        }

        case WM_KILLFOCUS: {
            
        }

        case WM_ENTERSIZEMOVE: {
            
        }

        case WM_EXITSIZEMOVE: {
            
        }

        case WM_DPICHANGED: {

        }

        case WM_DEVICECHANGE: {

        }
    }

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
    const PalAllocator *allocator,
    PalEventDriver* eventDriver) {
    
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
    s_Video.features |= PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW;
    s_Video.features |= PAL_VIDEO_FEATURE_TOOL_WINDOW;
    s_Video.features |= PAL_VIDEO_FEATURE_DISPLAY_MODE_SWITCH;
    s_Video.features |= PAL_VIDEO_FEATURE_MULTI_DISPLAYS;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_RESIZING;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_POSITIONING;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_MINMAX;
    s_Video.features |= PAL_VIDEO_FEATURE_NO_MAXIMIZEBOX;
    s_Video.features |= PAL_VIDEO_FEATURE_NO_MINIMIZEBOX;
    s_Video.features |= PAL_VIDEO_FEATURE_DISPLAY_GAMMA_CONTROL;
    s_Video.features |= PAL_VIDEO_FEATURE_CLIP_CURSOR;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY;

    if (s_Video.getDpiForMonitor && s_Video.setProcessAwareness) {
        s_Video.features |= PAL_VIDEO_FEATURE_HIGH_DPI;
        s_Video.setProcessAwareness(WIN32_DPI_AWARE);
    }

    s_Video.initialized = true;
    s_Video.allocator = allocator;
    s_Video.eventDriver = eventDriver;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownVideo() {

    if (!s_Video.initialized) {
        return;
    }

    UnregisterClassW(PAL_VIDEO_CLASS, s_Video.instance);
    s_Video.initialized = false;
}

void PAL_CALL palUpdateVideo() {

    if (!s_Video.initialized) {
        return;
    }
    
    MSG msg;
    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
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

    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
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

    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
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
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

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
    if (s_Video.getDpiForMonitor) {
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
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
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
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
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
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    return setDisplayMode(display, mode, false);
}

PalResult PAL_CALL palValidateDisplayMode(
    PalDisplay *display,
    PalDisplayMode *mode) {

    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    return setDisplayMode(display, mode, true);
}

PalResult PAL_CALL palSetDisplayOrientation(
    PalDisplay *display,
    PalOrientation orientation) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
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

// ==================================================
// Window
// ==================================================

PalResult PAL_CALL palCreateWindow(
    const PalWindowCreateInfo *info,
    PalWindow **outWindow) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!info || !outWindow) {
        return PAL_RESULT_NULL_POINTER;
    }

    HWND handle = nullptr;
    PalDisplay* display = nullptr;
    PalDisplayInfo displayInfo;

    Uint32 style = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED;
    Uint32 exStyle = 0;

    // no minimize box
    if (!(info->style & PAL_WINDOW_STYLE_NO_MINIMIZEBOX)) {
        style |= WS_MINIMIZEBOX;
    }

    // no maximize box
    if (!(info->style & PAL_WINDOW_STYLE_NO_MAXIMIZEBOX)) {
        style |= WS_MAXIMIZEBOX;
    }

    // resizable window
    if (info->style & PAL_WINDOW_STYLE_RESIZABLE) {
        style |= WS_THICKFRAME;

    } else {
        // not resizable. We remove the maximizebox even if user requested
        style &= ~WS_MAXIMIZEBOX;
    }

    // transparent window
    if (info->style & PAL_WINDOW_STYLE_TRANSPARENT) {
        exStyle |= WS_EX_LAYERED;
    }

    // tool window
    if (info->style & PAL_WINDOW_STYLE_TOOL) {
        exStyle |= WS_EX_TOOLWINDOW;
    }

    // topmost window
    if (info->style & PAL_WINDOW_STYLE_TOPMOST) {
        exStyle |= WS_EX_TOPMOST;
    }

    // get display
    if (info->display) {
        display = info->display;
    } else {
        // get primary display
        display = (PalDisplay*)MonitorFromPoint((POINT){0, 0}, MONITOR_DEFAULTTOPRIMARY);
        if (!display) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    // get display info
    PalResult result = palGetDisplayInfo(display, &displayInfo);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    } 

    // compose position. 
    Int32 x, y = 0;
    // the position and size must be scaled with the dpi before this call
    if (info->center) {
        x = displayInfo.x + (displayInfo.width - info->width) / 2;
        y = displayInfo.y + (displayInfo.height - info->height) / 2;

    } else {
        // we set 100 for each axix
        x = displayInfo.x + 100;
        y = displayInfo.y + 100;
    }

    // adjust the window size
    RECT rect = { 0, 0, 0, 0 };
    rect.right = info->width;
    rect.bottom = info->height;
    AdjustWindowRectEx(&rect, style, 0, exStyle);

    wchar_t buffer[WINDOW_NAME_SIZE];  
    MultiByteToWideChar(CP_UTF8, 0, info->title, -1, buffer, 256);

    // create the window
    handle = CreateWindowExW(
        exStyle,
        PAL_VIDEO_CLASS,
        buffer,
        style, 
        x,
        y,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr, 
        nullptr, 
        s_Video.instance,
        nullptr
    );

    if (!handle) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // show, maximize and minimize
    Int32 showFlag = SW_HIDE;
    // maximize
    if (info->showMaximized) {
        showFlag = SW_SHOWMAXIMIZED;
    }

    // minimized
    if (info->showMinimized) {
        showFlag = SW_SHOWMINIMIZED;
    }

    // shown
    if (info->show) {
        if (showFlag == SW_HIDE) {
            // change only if maximize and minimize are not set
            showFlag = SW_SHOW;
        }
    }

    ShowWindow(handle, showFlag);
    UpdateWindow(handle);

    if (info->style & PAL_WINDOW_STYLE_BORDERLESS) {
        // revert changes
        SetWindowLongPtrW(handle, GWL_STYLE, WS_POPUP);
        SetWindowLongPtrW(handle, GWL_EXSTYLE, WS_EX_APPWINDOW);

        // force a frame update
        SetWindowPos(
            handle,
            nullptr,
            0, 
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
        );
    }

    *outWindow = (PalWindow*)handle;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyWindow(
    PalWindow *window) {

    if (!window) {
        return;
    }
    DestroyWindow((HWND)window);
}

PalResult PAL_CALL palMinimizeWindow(
    PalWindow* window) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!ShowWindow((HWND)window, SW_MINIMIZE)) {
        // technically, this will fail if the window handle is invalid
        return PAL_RESULT_INVALID_WINDOW;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palMaximizeWindow(
    PalWindow* window) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!ShowWindow((HWND)window, SW_MAXIMIZE)) {
        // technically, this will fail if the window handle is invalid
        return PAL_RESULT_INVALID_WINDOW;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palRestoreWindow(
    PalWindow* window) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!ShowWindow((HWND)window, SW_RESTORE)) {
        // technically, this will fail if the window handle is invalid
        return PAL_RESULT_INVALID_WINDOW;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palShowWindow(
    PalWindow* window) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!ShowWindow((HWND)window, SW_SHOW)) {
        // technically, this will fail if the window handle is invalid
        return PAL_RESULT_INVALID_WINDOW;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palHideWindow(
    PalWindow* window) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!ShowWindow((HWND)window, SW_HIDE)) {
        // technically, this will fail if the window handle is invalid
        return PAL_RESULT_INVALID_WINDOW;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window || !info) {
        return PAL_RESULT_NULL_POINTER;
    }

    DWORD flags = 0;
    if (info->flags == PAL_FLASH_STOP) {
        flags = FLASHW_STOP;

    } else {
        if (info->flags & PAL_FLASH_CAPTION) {
            flags |= FLASHW_CAPTION;
        } if (info->flags & PAL_FLASH_TRAY) {
            flags |= FLASHW_TRAY;
            flags |= FLASHW_TIMERNOFG;
        }
    }

    FLASHWINFO flashInfo = {};
    flashInfo.cbSize = sizeof(FLASHWINFO);
    flashInfo.dwFlags = flags;
    flashInfo.dwTimeout = info->interval;
    flashInfo.hwnd = (HWND)window;
    flashInfo.uCount = info->count;

    bool success = FlashWindowEx(&flashInfo);
    if (!success) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_WINDOW;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    } 

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* outStyle) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window || !outStyle) {
        return PAL_RESULT_NULL_POINTER;
    }

    PalWindowStyle windowStyle = 0;
    DWORD style = GetWindowLongPtrW((HWND)window, GWL_STYLE);
    DWORD exStyle = GetWindowLongPtrW((HWND)window, GWL_EXSTYLE);

    if (!style) {
        // since there is no window without styles
        // we assume if we dont get any, the window handle is invalid
        return PAL_RESULT_INVALID_WINDOW;
    }
    
    // check if we can resize
    if (style & WS_THICKFRAME) {
        windowStyle |= PAL_WINDOW_STYLE_RESIZABLE;
    }

    // check if we are transparent
    if (exStyle & WS_EX_LAYERED) {
        windowStyle |= PAL_WINDOW_STYLE_TRANSPARENT;
    }

    // check if we are a topmost window
    if (exStyle & WS_EX_TOPMOST) {
        windowStyle |= PAL_WINDOW_STYLE_TOPMOST;
    }
    
    // check if we have a minimize box
    if (!(style & WS_MINIMIZEBOX)) {
        windowStyle |= PAL_WINDOW_STYLE_NO_MINIMIZEBOX;
    }

    // check if we have a maximize box
    if (!(style & WS_MAXIMIZEBOX)) {
        windowStyle |= PAL_WINDOW_STYLE_NO_MAXIMIZEBOX;
    }

    // check if its a tool window
    if (exStyle & WS_EX_TOOLWINDOW) {
        windowStyle |= PAL_WINDOW_STYLE_TOOL;
    }

    // we check borderless last since it will overwrite other styles
    if (style & WS_POPUP) {
        windowStyle |= PAL_WINDOW_STYLE_BORDERLESS;

        // we remove minimize and maximize box if set
        windowStyle &= ~PAL_WINDOW_STYLE_NO_MINIMIZEBOX;
        windowStyle &= ~PAL_WINDOW_STYLE_NO_MAXIMIZEBOX;
    }

    *outStyle = windowStyle;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetWindowDisplay(
    PalWindow* window, 
    PalDisplay** outDisplay) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window || !outDisplay) {
        return PAL_RESULT_NULL_POINTER;
    }

    HMONITOR monitor = MonitorFromWindow((HWND)window, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_WINDOW;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    *outDisplay = (PalDisplay*)monitor;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetWindowTitle(
    PalWindow* window,
    char** outTitle) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window || !outTitle) {
        return PAL_RESULT_NULL_POINTER;
    }

    wchar_t buffer[WINDOW_NAME_SIZE];
    if (GetWindowTextW((HWND)window, buffer, WINDOW_NAME_SIZE) == 0) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    // convert to UTF-8 encoding string and allocate a string buffer
    int len = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, nullptr, 0, nullptr, nullptr);
    char* string = palAllocate(s_Video.allocator, len + 1, 0);
    if (!string) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    WideCharToMultiByte(CP_UTF8, 0, buffer, -1, string, len, nullptr, nullptr);
    *outTitle = string;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetWindowPos(
    PalWindow* window, 
    Int32* x, 
    Int32* y) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    RECT rect;
    if (!GetWindowRect((HWND)window, &rect)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    if (x) {
        *x = rect.left;
    }

    if (y) {
        *y = rect.top;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetWindowClientPos(
    PalWindow* window, 
    Int32* x, 
    Int32* y) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    RECT rect;
    if (!GetClientRect((HWND)window, &rect)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    if (x) {
        *x = rect.left;
    }

    if (y) {
        *y = rect.top;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetWindowSize(
    PalWindow* window, 
    Uint32* width, 
    Uint32* height) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    RECT rect;
    if (!GetWindowRect((HWND)window, &rect)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    if (width) {
        *width = rect.right - rect.left;
    }

    if (height) {
        *height = rect.bottom - rect.top;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetWindowClientSize(
    PalWindow* window, 
    Uint32* width, 
    Uint32* height) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    RECT rect;
    if (!GetClientRect((HWND)window, &rect)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    if (width) {
        *width = rect.right - rect.left;
    }

    if (height) {
        *height = rect.bottom - rect.top;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetWindowState(
    PalWindow* window, 
    PalWindowState* state) {

    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window || !state) {
        return PAL_RESULT_NULL_POINTER;
    }

    WINDOWPLACEMENT wp = {};
    if (!GetWindowPlacement((HWND)window, &wp)) {
        return PAL_RESULT_INVALID_WINDOW;
    }
    
    if (wp.showCmd == SW_SHOWMINIMIZED || wp.showCmd == SW_MINIMIZE) {
        state->minimized = true;
        state->maximized = false;

    } else if (wp.showCmd == SW_SHOWMAXIMIZED || wp.showCmd == SW_MAXIMIZE) {
        state->maximized = true;
        state->minimized = false;
    }

    // check visibility
    state->visible = IsWindowVisible((HWND)window);
    return PAL_RESULT_SUCCESS;
}

PalWindow* PAL_CALL palGetFocusWindow() {

    if (!s_Video.initialized) {
        return nullptr;
    }
    return GetFocus();
}

PalWindow* PAL_CALL palGetForegroundWindow() {

    if (!s_Video.initialized) {
        return nullptr;
    }
    return GetForegroundWindow();
}

PalResult PAL_CALL palSetWindowOpacity(
    PalWindow* window,
    float opacity) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!SetLayeredWindowAttributes((HWND)window, 0, (BYTE)(opacity * 255), LWA_ALPHA)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_WINDOW;

        } else if (error == ERROR_INVALID_PARAMETER) {
            return PAL_RESULT_INVALID_PARAMETER;

        } else {
            // FIXME: check for child windows
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    // convert our style to win32 styles and exStyles
    DWORD win32Style = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED; // all windows have this styles
    DWORD exStyle = 0;

    // check for resizing
    if (style & PAL_WINDOW_STYLE_RESIZABLE) {
        win32Style |= WS_THICKFRAME;
    }

    // check for transparent window
    if (style & PAL_WINDOW_STYLE_TRANSPARENT) {
        exStyle |= WS_EX_LAYERED;
    }

    // check for topmost window
    if (style & PAL_WINDOW_STYLE_TOPMOST) {
        exStyle |= WS_EX_TOPMOST;
    }

    // check for no minimize box
    if (style & PAL_WINDOW_STYLE_NO_MINIMIZEBOX) {
        win32Style &= ~WS_MINIMIZEBOX;
    }

    // check for maximize box
    if (style & PAL_WINDOW_STYLE_NO_MAXIMIZEBOX) {
        win32Style &= ~WS_MAXIMIZEBOX;
    }

    // check for tool window
    if (style & PAL_WINDOW_STYLE_TOOL) {
        exStyle |= WS_EX_TOOLWINDOW;
    }

    // check for borderless window
    if (style & PAL_WINDOW_STYLE_BORDERLESS) {
        // revert the styles
        win32Style = WS_POPUP;
        exStyle = WS_EX_APPWINDOW;
    }

    HWND hwnd = (HWND)window;
    SetWindowLongPtrW(hwnd, GWL_STYLE, win32Style);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);

    // force a frame update
    bool success = SetWindowPos(
        hwnd,
        nullptr,
        0, 
        0,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
    );

    if (success) {
        return PAL_RESULT_SUCCESS;

    } else {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_WINDOW;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }
}

PalResult PAL_CALL palSetWindowTitle(
    PalWindow* window, 
    const char* title) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window || !title) {
        return PAL_RESULT_NULL_POINTER;
    }
    
    wchar_t buffer[WINDOW_NAME_SIZE];
    MultiByteToWideChar(CP_UTF8, 0, title, -1, buffer, 256);

    if (!SetWindowTextW((HWND)window, buffer)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetWindowPos(
    PalWindow* window, 
    Int32 x, 
    Int32 y) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }
    
    bool success = SetWindowPos(
        (HWND)window, 
        nullptr, 
        x, 
        y, 
        0, 
        0, 
        SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE
    );

    if (!success) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_WINDOW;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetWindowSize(
    PalWindow* window, 
    Uint32 width, 
    Uint32 height) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    bool success = SetWindowPos(
        (HWND)window,
        HWND_TOP, 
        0, 
        0, 
        width, 
        height,
        SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER
    );

    if (!success) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_WINDOW;

        } else if (error == ERROR_INVALID_PARAMETER) {
            return PAL_RESULT_INVALID_PARAMETER;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetFocusWindow(
    PalWindow* window) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!SetFocus((HWND)window)) {
        DWORD error = GetLastError();
        if (error ==ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_WINDOW;
        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetForegroundWindow(
    PalWindow* window) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!SetForegroundWindow((HWND)window)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_WINDOW;

        } else if (error == ERROR_ACCESS_DENIED) {
            return PAL_RESULT_ACCESS_DENIED;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }
    return PAL_RESULT_SUCCESS;

}