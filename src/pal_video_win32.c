
/**

Copyright (C) 2025 Nicholas Agbo
  
This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:
  
1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required. 
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

#include "pal/pal_video.h"

#ifdef _WIN32
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
#endif // _WIN32

#define PAL_WIN32_VIDEO_CLASS L"PALVideoClass"
#define PAL_WIN32_VIDEO_PROP L"PALVideo"
#define WIN32_DPI 0
#define WIN32_DPI_AWARE 2
#define MAX_MODE_COUNT 128
#define NULL_ORIENTATION 5

typedef HRESULT (WINAPI* GetDpiForMonitorFn)(HMONITOR, int, UINT*, UINT*);
typedef HRESULT (WINAPI* SetProcessAwarenessFn)(int);

static GetDpiForMonitorFn s_GetDpiForMonitor;
static SetProcessAwarenessFn s_SetProcessAwareness;
static HINSTANCE s_Shcore;

typedef struct PalVideoSystem {
    PalAllocator* allocator;
    HINSTANCE instance;
    PalVideoFeatures features;
} PalVideoSystem;

typedef struct PalWindow {
    PalVideoSystem* system;
    HWND handle;
    const char* title;
    Uint32 width;
    Uint32 height;
    Uint32 style;
    Uint32 exStyle;
    int x;
    int y;
    bool hidden;
    bool fullscreen;
    bool maximized, minimized;
} PalWindow;

typedef struct DisplayData {
    PalDisplay** displays;
    int count;
    int maxCount;
} DisplayData;

static void getMonitorDPI(
    HMONITOR monitor, 
    int* dpi);

static PalDisplayOrientation orientationFromWin32(DWORD orientation);
static DWORD orientationToin32(PalDisplayOrientation orientation);

static PalResult setDisplayMode(
    PalDisplay* display,
    PalDisplayMode* mode,
    bool test);

static void addMode(
    PalDisplayMode* modes, 
    const PalDisplayMode* mode, 
    int* count);

static bool compareMode(
    const PalDisplayMode* a, 
    const PalDisplayMode* b);

BOOL CALLBACK enumMonitors(
    HMONITOR monitor, 
    HDC hdc, 
    LPRECT lRect, 
    LPARAM lParam);

LRESULT CALLBACK videoProc(
    HWND hwnd, 
    UINT msg, 
    WPARAM wParam, 
    LPARAM lParam);

// ==================================================
// Public API
// ==================================================

PalResult _PCALL palCreateVideoSystem(
    const PalVideoSystemCreateInfo* info,
    PalVideoSystem** outSystem) {

    PalVideoSystem* system;
    if (!info || !outSystem) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (info->allocator && 
    (!info->allocator->allocate || !info->allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    system = palAllocate(info->allocator, sizeof(PalVideoSystem), 0);
    if (!system) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(system, 0, sizeof(PalVideoSystem));
    if (info->allocator) {
        system->allocator = info->allocator;
    }

    // register class
    system->instance = GetModuleHandleW(nullptr);
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    if (!GetClassInfoExW(system->instance, PAL_WIN32_VIDEO_CLASS, &wc)) {
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hbrBackground = NULL;
        wc.hCursor = LoadCursorW(system->instance, IDC_ARROW);
        wc.hIcon = LoadIconW(system->instance, IDI_APPLICATION);
        wc.hIconSm = LoadIconW(system->instance, IDI_APPLICATION);
        wc.hInstance = system->instance;
        wc.lpfnWndProc = videoProc;
        wc.lpszClassName = PAL_WIN32_VIDEO_CLASS;
        wc.lpszMenuName = NULL;
        wc.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

        if (!RegisterClassExW(&wc)) {
            return PAL_RESULT_ACCESS_DENIED;
        }
    }

    // load shared libraries
    s_Shcore = LoadLibraryA("shcore.dll");
    if (s_Shcore) {
        s_GetDpiForMonitor = (GetDpiForMonitorFn)GetProcAddress(
            s_Shcore,
            "GetDpiForMonitor"
        );

        s_SetProcessAwareness = (SetProcessAwarenessFn)GetProcAddress(
            s_Shcore,
            "SetProcessDpiAwareness"
        );
    }

    // set features
    PalVideoFeatures features;
    features |= PAL_VIDEO_FEATURE_DISPLAY_ORIENTATION;
    features |= PAL_VIDEO_FEATURE_BORDERLESS_WINDOW;
    features |= PAL_VIDEO_FEATURE_DISPLAY_MODE_SWITCH;
    features |= PAL_VIDEO_FEATURE_MULTI_DISPLAYS;
    features |= PAL_VIDEO_FEATURE_WINDOW_RESIZING;
    features |= PAL_VIDEO_FEATURE_WINDOW_POSITIONING;
    features |= PAL_VIDEO_FEATURE_WINDOW_MINMAX;
    features |= PAL_VIDEO_FEATURE_DISPLAY_GAMMA_CONTROL;
    features |= PAL_VIDEO_FEATURE_CLIP_CURSOR;

    if (s_GetDpiForMonitor && s_SetProcessAwareness) {
        features |= PAL_VIDEO_FEATURE_HIGH_DPI;
    }

    system->features = features;
    *outSystem = system;
    return PAL_RESULT_SUCCESS;
}

void _PCALL palDestroyVideoSystem(
    PalVideoSystem* system) {

    if (!system || (system && !system->instance)) {
        return;
    }

    UnregisterClassW(PAL_WIN32_VIDEO_CLASS, system->instance);
    palFree(system->allocator, system);
}

PalVideoFeatures _PCALL palGetVideoFeatures(PalVideoSystem* system) {

    if (!system) {
        return 0;
    }
    return system->features;
}

PalResult _PCALL palUpdateVideo(
    PalVideoSystem* system) {

    if (!system) {
        return PAL_RESULT_NULL_POINTER;
    }
    
    MSG msg;
    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult _PCALL palEnumerateDisplays(
    PalVideoSystem* system,
    int* count,
    PalDisplay** displays) {

    if (!system || !count) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (count == 0) {
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

PalResult _PCALL palGetPrimaryDisplay(
    PalVideoSystem* system,
    PalDisplay** outDisplay) {

    if (!system || !outDisplay) {
        return PAL_RESULT_NULL_POINTER;
    }

    HMONITOR monitor = MonitorFromPoint((POINT){0, 0}, MONITOR_DEFAULTTOPRIMARY);
    *outDisplay = (PalDisplay*)monitor;
    return PAL_RESULT_SUCCESS;
}

PalResult _PCALL palGetDisplayInfo(
    PalDisplay* display, 
    PalDisplayInfo* info) {

    if (!display || !info) {
        return PAL_RESULT_NULL_POINTER;
    }

    HMONITOR monitor = (HMONITOR)display;
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        return PAL_RESULT_INVALID_DISPLAY; 
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
    int dpi;
    getMonitorDPI(monitor, &dpi);
    info->dpi = dpi;

    // check for primary display
    HMONITOR primaryDisplay = MonitorFromPoint(
        (POINT){0, 0}, 
        MONITOR_DEFAULTTOPRIMARY
    );

    if (primaryDisplay == monitor) {
        info->primary = true;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult _PCALL palEnumerateDisplayModes(
    PalDisplay* display,
    int* count,
    PalDisplayMode* modes) {

    if (!display || !count) {
        return PAL_RESULT_NULL_POINTER;
    }

    int modeCount = 0;
    int maxModes = 0;
    HMONITOR monitor = (HMONITOR)display;
    PalDisplayMode* displayModes = nullptr;

    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        return PAL_RESULT_INVALID_DISPLAY;
    }

    if (!modes) {
        // allocate and store tmp display modes
        // and check for the interested fields.
        displayModes = palAllocate(
            nullptr, 
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
    for (int i = 0; EnumDisplaySettingsW(mi.szDevice, i, &dm); i++) {
        // Pal support up to 128 modes
        if (modeCount > maxModes) {
            break;
        }

        PalDisplayMode* mode = &displayModes[modeCount];
        mode->refreshRate = dm.dmDisplayFrequency;
        mode->width = dm.dmPelsWidth;
        mode->height = dm.dmPelsHeight;
        mode->bpp = dm.dmBitsPerPel;
        addMode(displayModes, mode, &modeCount);
    }

    if (!modes) {
        *count = modeCount;
        palFree(nullptr, displayModes);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult _PCALL palGetCurrentDisplayMode(
    PalDisplay* display,
    PalDisplayMode* mode) {

    if (!display || !mode) {
        return PAL_RESULT_NULL_POINTER;
    }

    HMONITOR monitor = (HMONITOR)display;
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        return PAL_RESULT_INVALID_DISPLAY; 
    }

    DEVMODE devMode = {};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    mode->width = devMode.dmPelsWidth;
    mode->height = devMode.dmPelsHeight;
    mode->refreshRate = devMode.dmDisplayFrequency;
    mode->bpp = devMode.dmBitsPerPel;
}

PalResult _PCALL palSetDisplayMode(
    PalDisplay* display,
    PalDisplayMode* mode) {

    return setDisplayMode(display, mode, false);
}

PalResult _PCALL palValidateDisplayMode(
    PalDisplay* display,
    PalDisplayMode* mode) {

    return setDisplayMode(display, mode, true);
}

PalResult _PCALL palSetDisplayOrientation(
    PalDisplay* display,
    PalDisplayOrientation orientation) {

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
        return PAL_RESULT_INVALID_DISPLAY; 
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

PalResult _PCALL palCreateWindow(
    PalVideoSystem* system, 
    PalWindowCreateInfo* info,
    PalWindow** outWindow) {

    if (!system || !info || !outWindow) {
        return PAL_RESULT_NULL_POINTER;
    }

    PalWindow* window = nullptr;
    PalDisplay* display = nullptr;
    PalDisplayInfo displayInfo;
    bool hidden;

    PalVideoFeatures features = system->features;
    Uint32 style = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED;
    Uint32 exStyle = WS_EX_OVERLAPPEDWINDOW;

    if (info->flags & PAL_WINDOW_MINIMIZEBOX) {
        style |= WS_MINIMIZEBOX;
    }

    if (info->flags & PAL_WINDOW_RESIZABLE) {
        style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    }

    if (info->display) {
        display = info->display;
    } else {
        palGetPrimaryDisplay(system, &display);
    }

    // get display info
    PalResult result = palGetDisplayInfo(display, &displayInfo);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    Uint32 width = 0;
    Uint32 height = 0;
    int x = 0;
    int y = 0;
    
    if (info->flags & PAL_WINDOW_CENTER) {
        x = displayInfo.x + (displayInfo.width - info->width) / 2;
        y = displayInfo.y + (displayInfo.height - info->height) / 2;

    } else {
        // we set 100 for each axix
        x = displayInfo.x + 100;
        y = displayInfo.y + 100;
    }

    float scale;
    // set window size
    if (info->flags & PAL_WINDOW_ALLOW_HIGH_DPI) {
        // check for support
        if (features & PAL_VIDEO_FEATURE_HIGH_DPI) {
            scale = (float)displayInfo.dpi / 96.0f;
            width = (Uint32)((float)info->width * scale);
            height = (Uint32)((float)info->height * scale);
        } else {
            return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
        }

    } else {
        width = info->width;
        height = info->height;
    }

    RECT rect = { 0, 0, 0, 0 };
    rect.right = width;
    rect.bottom = height;
    AdjustWindowRectEx(&rect, style, 0, exStyle);

    wchar_t buffer[256] = {};    
    int len = MultiByteToWideChar(CP_UTF8, 0, info->title, -1, nullptr, 0);
    MultiByteToWideChar(CP_UTF8, 0, info->title, -1, buffer, len);

    HWND handle = CreateWindowExW(
        exStyle,
        PAL_WIN32_VIDEO_CLASS,
        buffer,
        style, 
        x,
        y,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr, 
        nullptr, 
        system->instance,
        nullptr
    );

    if (!handle) {
        return PAL_RESULT_VIDEO_DEVICE_NOT_FOUND;
    }

    window = palAllocate(system->allocator, sizeof(PalWindow), 0);
    if (!window) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(window, 0, sizeof(PalWindow));
    int showFlag = SW_HIDE;
    if (info->flags & PAL_WINDOW_MAXIMIZED) {
        showFlag = SW_SHOWMAXIMIZED;
    }

    if (info->flags & PAL_WINDOW_SHOWN) {
        if (showFlag == SW_HIDE) {
            showFlag = SW_SHOW;
        }
    }

    if (showFlag != SW_HIDE) {
        hidden = false;
    } else {
        hidden = true;
    }

    ShowWindow(handle, showFlag);
    SetPropW(handle, PAL_WIN32_VIDEO_PROP, window);

    window->system = system;
    window->handle = handle;
    window->title = info->title;
    window->style = style;
    window->exStyle = exStyle;
    window->width = info->width;
    window->height = info->height;
    window->x = x;
    window->y = y;
    window->hidden = hidden;

    if (info->flags & PAL_WINDOW_FULLSCREEN) {
        PalResult result = palSetWindowFullscreen(window, display, true);
        if (result != PAL_RESULT_SUCCESS) {
            DestroyWindow(window->handle);
            palFree(system->allocator, window);
            return result;
        }
    }

    *outWindow = window;
    return PAL_RESULT_SUCCESS;
}

void _PCALL palDestroyWindow(PalWindow* window) {

    if (!window || (window && !window->system)) {
        return;
    }

    DestroyWindow(window->handle);
    palFree(window->system->allocator, window);
}

PalResult _PCALL palSetWindowBorderless(
    PalWindow* window,
    bool enable) {

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (enable) {
        SetWindowLongPtrW(window->handle, GWL_STYLE, WS_POPUP | WS_VISIBLE);
        SetWindowLongPtrW(window->handle, GWL_EXSTYLE, WS_EX_APPWINDOW);
        
    } else {
        SetWindowLongPtrW(window->handle, GWL_STYLE, window->style);
        SetWindowLongPtrW(window->handle, GWL_EXSTYLE, window->exStyle);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult _PCALL palGetWindowDisplay(
    PalWindow* window, 
    PalDisplay** outDisplay) {

    if (!window || !outDisplay) {
        return PAL_RESULT_NULL_POINTER;
    }

    HMONITOR monitor = MonitorFromWindow(window->handle, MONITOR_DEFAULTTONEAREST);
    if (!monitor) { 
        return PAL_RESULT_INVALID_WINDOW; 
    }

    *outDisplay = (PalDisplay*)monitor;
    return PAL_RESULT_SUCCESS;
}

PalResult _PCALL palFitWindowToDisplay(
    PalWindow* window, 
    PalDisplay* display) {
    
    PalResult result;
    PalDisplayInfo info;
    PalDisplay* currentDisplay;
    
    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (display) {
        currentDisplay = display;

    } else {
        result = palGetWindowDisplay(window, &currentDisplay);
        if (result != PAL_RESULT_SUCCESS) {
            return result;
        }
    }

    result = palGetDisplayInfo(currentDisplay, &info);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    bool success = SetWindowPos(
        window->handle,
        HWND_TOP,
        info.x,
        info.y,
        info.width,
        info.height,
        SWP_FRAMECHANGED | SWP_SHOWWINDOW
    );

    if (!success) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult _PCALL palRestoreWindow(PalWindow* window) {

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    bool success = SetWindowPos(
        window->handle,
        HWND_TOP,
        window->x,
        window->y,
        window->width,
        window->height,
        SWP_FRAMECHANGED | SWP_SHOWWINDOW
    );

    if (!success) {
        PAL_RESULT_INVALID_WINDOW;
    }

    if (window->maximized || window->minimized) {
        ShowWindow(window->handle, SW_RESTORE);
        window->maximized = false;
        window->minimized = false;
        window->hidden = false;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult _PCALL palMaximizeWindow(PalWindow* window) {

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (window->maximized) {
        return PAL_RESULT_SUCCESS;
    }

    if (window->fullscreen) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    ShowWindow(window->handle, SW_SHOWMAXIMIZED);
    window->maximized = true;
    window->minimized = false;
    return PAL_RESULT_SUCCESS;
}

PalResult _PCALL palMinimizeWindow(PalWindow* window) {

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (window->minimized) {
        return PAL_RESULT_SUCCESS;
    }

    if (window->fullscreen) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    ShowWindow(window->handle, SW_MINIMIZE);
    window->minimized = true;
    window->maximized = false;
    return PAL_RESULT_SUCCESS;
}

PalResult _PCALL palSetWindowFullscreen(
    PalWindow* window, 
    PalDisplay* display,
    bool enable) {

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (window->fullscreen && enable) {
        // already fullscreen
        return PAL_RESULT_SUCCESS;
    }

    if (!window->fullscreen && !enable) {
        // already windowed
        return PAL_RESULT_SUCCESS;
    }

    PalResult result;
    if (enable) {
        result = palSetWindowBorderless(window, true);
        if (result != PAL_RESULT_SUCCESS) {
            return result;
        }

        result = palFitWindowToDisplay(window, display);
        if (result != PAL_RESULT_SUCCESS) {
            return result;
        }

    } else {
        result = palSetWindowBorderless(window, false);
        if (result != PAL_RESULT_SUCCESS) {
            return result;
        }

        result = palRestoreWindow(window);
        if (result != PAL_RESULT_SUCCESS) {
            return result;
        }
    }

    window->fullscreen = enable;
    return PAL_RESULT_SUCCESS;
}

void _PCALL palShowWindow(PalWindow* window) {

    if (!window) {
        return;
    }

    if (window->maximized) {
        ShowWindow(window->handle, SW_SHOWMAXIMIZED);

    } else {
        ShowWindow(window->handle, SW_SHOW);
    }
    window->hidden = false;
}

void _PCALL palHideWindow(PalWindow* window) {

    ShowWindow(window->handle, SW_HIDE);
    window->hidden = true;
}

PalResult _PCALL palCenterWindow(
    PalWindow* window, 
    PalDisplay* display) {

    PalResult result;
    PalDisplayInfo info;
    PalDisplay* currentDisplay;

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (window->fullscreen || window->maximized || window->minimized) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    if (display) {
        currentDisplay = display;

    } else {
        result = palGetWindowDisplay(window, &currentDisplay);
        if (result != PAL_RESULT_SUCCESS) {
            return result;
        }
    }

    result = palGetDisplayInfo(currentDisplay, &info);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    int x = info.x + (info.width - window->width) / 2;
    int y = info.y + (info.height - window->height) / 2;
    bool success = SetWindowPos(
        window->handle, 
        nullptr, 
        x, 
        y, 
        0, 
        0, 
        SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE
    );

    if (!success) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    window->x = x;
    window->y = y;
    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Internal API
// ==================================================

static void getMonitorDPI(
    HMONITOR monitor, 
    int* dpi) {

    if (!s_GetDpiForMonitor || !s_SetProcessAwareness) {
        *dpi = 96;
        return;
    }

    int dpiX, dpiY;
    s_SetProcessAwareness(WIN32_DPI_AWARE);
    s_GetDpiForMonitor(monitor, WIN32_DPI, &dpiX, &dpiY);
    *dpi = dpiX;
}

static PalDisplayOrientation orientationFromWin32(DWORD orientation) {

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
}

static DWORD orientationToin32(PalDisplayOrientation orientation) {

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
        return PAL_RESULT_INVALID_DISPLAY; 
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

static bool compareMode(
    const PalDisplayMode* a, 
    const PalDisplayMode* b) {

    return 
        a->bpp == b->bpp               &&
        a->width == b->width           &&
        a->height == b->height         &&
        a->refreshRate == b->refreshRate;
}

static void addMode(
    PalDisplayMode* modes, 
    const PalDisplayMode* mode, 
    int* count) {

    // check if we have a duplicate mode
    for (int i = 0; i < *count; i++) {
        PalDisplayMode* oldMode = &modes[i];
        if (compareMode(oldMode, mode)) {
            return;
        }
    }

    // new mode
    modes[*count] = *mode;
    *count += 1;
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

LRESULT CALLBACK videoProc(
    HWND hwnd, 
    UINT msg, 
    WPARAM wParam, 
    LPARAM lParam) {

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}