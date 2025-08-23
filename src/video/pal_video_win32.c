
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
#include <windowsx.h>
#include <time.h>

#define PAL_WIN32_VIDEO_CLASS L"PALVideoClass"
#define PAL_WIN32_VIDEO_PROP L"PALVideo"
#define WIN32_DPI 0
#define WIN32_DPI_AWARE 2
#define MAX_MODE_COUNT 128
#define NULL_ORIENTATION 5

#define DBT_DEVNODES_CHANGED 0x0007

typedef HRESULT (WINAPI* GetDpiForMonitorFn)(HMONITOR, Int32, UINT*, UINT*);
typedef HRESULT (WINAPI* SetProcessAwarenessFn)(Int32);

typedef struct PendingEventWin32 {
    Uint64 sourceID;
    Uint32 width;
    Uint32 height;
    Int32 x;
    Int32 y;
    bool pendingResize;
    bool pendingMove;
} PendingEventWin32;

typedef struct VideoSystemWin32 {
    const PalAllocator* allocator;
    PalEventDriver* eventDriver;
    HINSTANCE shcore;
    HINSTANCE instance;
    GetDpiForMonitorFn getDpiForMonitor;
    SetProcessAwarenessFn setProcessAwareness;

    PalVideoFeatures features;
    bool initialized;
} VideoSystemWin32;

typedef struct PalWindow {
    HWND handle;
    const char* title;
    Uint64 id;
    Uint32 width;
    Uint32 height;
    Uint32 style;
    Uint32 exStyle;
    Int32 x;
    Int32 y;
    bool hidden;
    bool fullscreen;
    bool maximized, minimized;
    bool highDPI;
} PalWindow;

typedef struct DisplayData {
    PalDisplay** displays;
    Int32 count;
    Int32 maxCount;
} DisplayData;

static Uint32 s_Counter = 0;
static VideoSystemWin32 s_System = {};
static PendingEventWin32 s_Event = {};

static void getMonitorDPI(
    HMONITOR monitor, 
    Int32* dpi);

static PalDisplayOrientation orientationFromWin32(DWORD orientation);
static DWORD orientationToin32(PalDisplayOrientation orientation);

static PalResult setDisplayMode(
    PalDisplay* display,
    PalDisplayMode* mode,
    bool test);

static void addMode(
    PalDisplayMode* modes, 
    const PalDisplayMode* mode, 
    Int32* count);

static bool compareMode(
    const PalDisplayMode* a, 
    const PalDisplayMode* b);

static Uint64 generateWindowID();

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

PalResult _PCALL palInitVideo(
    const PalAllocator *allocator,
    PalEventDriver *eventDriver) {

    if (allocator && (!allocator->allocate || !allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    s_System.allocator = allocator;
    s_System.eventDriver = eventDriver;

    if (!s_System.initialized) {
        // register class
        s_System.instance = GetModuleHandleW(nullptr);
        WNDCLASSEXW wc = {0};
        wc.cbSize = sizeof(WNDCLASSEXW);
        if (!GetClassInfoExW(s_System.instance, PAL_WIN32_VIDEO_CLASS, &wc)) {
            wc.cbClsExtra = 0;
            wc.cbWndExtra = 0;
            wc.hbrBackground = NULL;
            wc.hCursor = LoadCursorW(s_System.instance, IDC_ARROW);
            wc.hIcon = LoadIconW(s_System.instance, IDI_APPLICATION);
            wc.hIconSm = LoadIconW(s_System.instance, IDI_APPLICATION);
            wc.hInstance = s_System.instance;
            wc.lpfnWndProc = videoProc;
            wc.lpszClassName = PAL_WIN32_VIDEO_CLASS;
            wc.lpszMenuName = NULL;
            wc.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

            if (!RegisterClassExW(&wc)) {
                return PAL_RESULT_ACCESS_DENIED;
            }
        }

        // load shared libraries
        s_System.shcore = LoadLibraryA("shcore.dll");
        if (s_System.shcore) {
            s_System.getDpiForMonitor = (GetDpiForMonitorFn)GetProcAddress(
                s_System.shcore,
                "GetDpiForMonitor"
            );

            s_System.setProcessAwareness = (SetProcessAwarenessFn)GetProcAddress(
                s_System.shcore,
                "SetProcessDpiAwareness"
            );
        }

        // set features
        s_System.features |= PAL_VIDEO_FEATURE_DISPLAY_ORIENTATION;
        s_System.features |= PAL_VIDEO_FEATURE_BORDERLESS_WINDOW;
        s_System.features |= PAL_VIDEO_FEATURE_DISPLAY_MODE_SWITCH;
        s_System.features |= PAL_VIDEO_FEATURE_MULTI_DISPLAYS;
        s_System.features |= PAL_VIDEO_FEATURE_WINDOW_RESIZING;
        s_System.features |= PAL_VIDEO_FEATURE_WINDOW_POSITIONING;
        s_System.features |= PAL_VIDEO_FEATURE_WINDOW_MINMAX;
        s_System.features |= PAL_VIDEO_FEATURE_DISPLAY_GAMMA_CONTROL;
        s_System.features |= PAL_VIDEO_FEATURE_CLIP_CURSOR;
        s_System.features |= PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION;
        s_System.features |= PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY;

        if (s_System.getDpiForMonitor && s_System.setProcessAwareness) {
            s_System.features |= PAL_VIDEO_FEATURE_HIGH_DPI;
        }

        s_System.initialized = true;
    }

    return PAL_RESULT_SUCCESS;
}

void _PCALL palShutdownVideo() {

    if (!s_System.initialized) {
        return;
    }
    UnregisterClassW(PAL_WIN32_VIDEO_CLASS, s_System.instance);
    s_System.initialized = false;
}

PalVideoFeatures _PCALL palGetVideoFeatures() {

    if (!s_System.initialized) {
        return 0;
    }
    return s_System.features;
}

void _PCALL palUpdateVideo() {

    if (!s_System.initialized) {
        return;
    }
    
    MSG msg;
    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    if (s_Event.pendingResize) {
        PalEvent event = {};
        event.data = palPackUint32(s_Event.width, s_Event.height);
        event.sourceID = s_Event.sourceID;
        event.type = PAL_EVENT_WINDOW_RESIZE;
        palPushEvent(s_System.eventDriver, &event);
        s_Event.pendingResize = false;

    } else if (s_Event.pendingMove) {
        PalEvent event = {};
        event.data = palPackInt32(s_Event.x, s_Event.y);
        event.sourceID = s_Event.sourceID;
        event.type = PAL_EVENT_WINDOW_MOVE;
        palPushEvent(s_System.eventDriver, &event);
        s_Event.pendingMove = false;
    }
}

PalResult _PCALL palEnumerateDisplays(
    Int32* count,
    PalDisplay** displays) {

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

PalResult _PCALL palGetPrimaryDisplay(
    PalDisplay** outDisplay) {

    if (!outDisplay) {
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
    Int32 dpi;
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
    Int32* count,
    PalDisplayMode* modes) {

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
    const PalWindowCreateInfo* info,
    PalWindow** outWindow) {

    if (!info || !outWindow) {
        return PAL_RESULT_NULL_POINTER;
    }

    PalWindow* window = nullptr;
    PalDisplay* display = nullptr;
    PalDisplayInfo displayInfo;
    bool hidden, highDPI;

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
        palGetPrimaryDisplay(&display);
    }

    // get display info
    PalResult result = palGetDisplayInfo(display, &displayInfo);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    Uint32 width = 0;
    Uint32 height = 0;
    Int32 x = 0;
    Int32 y = 0;
    
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
        if (s_System.features & PAL_VIDEO_FEATURE_HIGH_DPI) {
            scale = (float)displayInfo.dpi / 96.0f;
            width = (Uint32)((float)info->width * scale);
            height = (Uint32)((float)info->height * scale);
            highDPI = true;
        } else {
            return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
        }

    } else {
        width = info->width;
        height = info->height;
        highDPI = false;
    }

    RECT rect = { 0, 0, 0, 0 };
    rect.right = width;
    rect.bottom = height;
    AdjustWindowRectEx(&rect, style, 0, exStyle);

    wchar_t buffer[256] = {};    
    Int32 len = MultiByteToWideChar(CP_UTF8, 0, info->title, -1, nullptr, 0);
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
        s_System.instance,
        nullptr
    );

    if (!handle) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    window = palAllocate(s_System.allocator, sizeof(PalWindow), 0);
    if (!window) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(window, 0, sizeof(PalWindow));
    Int32 showFlag = SW_HIDE;
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
    UpdateWindow(window->handle);
    SetPropW(handle, PAL_WIN32_VIDEO_PROP, window);

    window->handle = handle;
    window->title = info->title;
    window->style = style;
    window->exStyle = exStyle;
    window->width = info->width;
    window->height = info->height;
    window->x = x;
    window->y = y;
    window->hidden = hidden;
    window->highDPI = highDPI;

    if (info->flags & PAL_WINDOW_FULLSCREEN) {
        PalResult result = palSetWindowFullscreen(window, display, true);
        if (result != PAL_RESULT_SUCCESS) {
            DestroyWindow(window->handle);
            palFree(s_System.allocator, window);
            return result;
        }
    }

    window->id = generateWindowID();
    *outWindow = window;
    return PAL_RESULT_SUCCESS;
}

void _PCALL palDestroyWindow(PalWindow* window) {

    if (!window) {
        return;
    }

    DestroyWindow(window->handle);
    palFree(s_System.allocator, window);
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

PalResult palFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info) {

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
    flashInfo.hwnd = window->handle;
    flashInfo.uCount = info->count;

    bool success = FlashWindowEx(&flashInfo);
    if (!success) {
        return PAL_RESULT_INVALID_WINDOW;
    } 

    return PAL_RESULT_SUCCESS;
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

    Int32 x = info.x + (info.width - window->width) / 2;
    Int32 y = info.y + (info.height - window->height) / 2;
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

PalResult _PCALL palSetWindowTitle(
    PalWindow* window, 
    const char* title) {

    if (!window || !title) {
        return PAL_RESULT_NULL_POINTER;
    }

    wchar_t buffer[256] = {};    
    Int32 len = MultiByteToWideChar(CP_UTF8, 0, title, -1, nullptr, 0);
    MultiByteToWideChar(CP_UTF8, 0, title, -1, buffer, len);

    SetWindowTextW(window->handle, buffer);
    window->title = title;
    return PAL_RESULT_SUCCESS;
}

PalResult _PCALL palSetWindowPos(
    PalWindow* window, 
    Int32 x, 
    Int32 y) {

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    RECT rect = { x, y, x, y };
    AdjustWindowRectEx(&rect, window->style, 0, window->exStyle);
    SetWindowPos(
        window->handle, 
        nullptr, 
        rect.left, 
        rect.top, 
        0, 
        0, 
        SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE
    );

    window->x = x;
    window->y = y;
    return PAL_RESULT_SUCCESS;
}

PalResult _PCALL palSetWindowSize(
    PalWindow* window, 
    Uint32 width, 
    Uint32 height) {

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!width || !height) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    RECT rect = { 0, 0, 0, 0 };
    rect.right = width;
    rect.bottom = height;
    AdjustWindowRectEx(&rect, window->style, 0, window->exStyle);
    SetWindowPos(
        window->handle, 
        HWND_TOP, 
        0, 
        0, 
        rect.right - rect.left, 
        rect.bottom - rect.top,
        SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER
    );

    window->width = width;
    window->height = height;
    return PAL_RESULT_SUCCESS;
}

const char* _PCALL palGetWindowTitle(PalWindow* window) {

    if (!window) {
        return nullptr;
    }
    return window->title;
}

void _PCALL palGetWindowPos(
    PalWindow* window, 
    Int32* x, 
    Int32* y) {

    if (!window) {
        return;
    }

    if (x) {
        *x = window->x;
    }

    if (y) {
        *y = window->y;
    }
}

void _PCALL palGetWindowSize(
    PalWindow* window, 
    Uint32* width, 
    Uint32* height) {

    if (!window) {
        return;
    }

    if (width) {
        *width = window->width;
    }

    if (height) {
        *height = window->height;
    }
}

Uint64 _PCALL palGetWindowID(PalWindow* window) {

    if (!window) {
        return 0;
    }
    return window->id;
}

void* _PCALL palGetWindowHandle(PalWindow* window) {

    if (!window) {
        return nullptr;
    }
    return window->handle;
}

bool _PCALL palIsWindowMaximized(PalWindow* window) {

    if (!window) {
        return false;
    }
    return window->maximized;
}

bool _PCALL palIsWindowMinimized(PalWindow* window) {

    if (!window) {
        return false;
    }
    return window->minimized;
}

bool _PCALL palIsWindowHidden(PalWindow* window) {

    if (!window) {
        return false;
    }
    return window->hidden;
}

bool _PCALL palIsWindowFullScreen(PalWindow* window) {

    if (!window) {
        return false;
    }
    return window->fullscreen;
}

// ==================================================
// Internal API
// ==================================================

static void getMonitorDPI(
    HMONITOR monitor, 
    Int32* dpi) {

    if (!s_System.getDpiForMonitor || !s_System.setProcessAwareness) {
        *dpi = 96;
        return;
    }

    Int32 dpiX, dpiY;
    s_System.setProcessAwareness(WIN32_DPI_AWARE);
    s_System.getDpiForMonitor(monitor, WIN32_DPI, &dpiX, &dpiY);
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
    Int32* count) {

    // check if we have a duplicate mode
    for (Int32 i = 0; i < *count; i++) {
        PalDisplayMode* oldMode = &modes[i];
        if (compareMode(oldMode, mode)) {
            return;
        }
    }

    // new mode
    modes[*count] = *mode;
    *count += 1;
}

static Uint64 generateWindowID() {

    Uint32 timeStamp = (Uint32)time(nullptr);
    Uint32 local = s_Counter++;
    return ((Uint64)timeStamp << 32) | local;
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

    PalWindow* window = (PalWindow*)GetPropW(hwnd, PAL_WIN32_VIDEO_PROP);
    if (!window) {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    switch (msg) {
        case WM_CLOSE: {
            if (s_System.eventDriver) {
                PalEventDriver* driver = s_System.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_WINDOW_CLOSE);
                if (mode != PAL_DISPATCH_NONE) {
                    PalEvent event = {};
                    event.type = PAL_EVENT_WINDOW_CLOSE;
                    event.sourceID = window->id;
                    palPushEvent(driver, &event);
                }
            }
            return 0;
        }

        case WM_SIZE: {
            const Uint32 width = (Uint32)LOWORD(lParam);
            const Uint32 height = (Uint32)HIWORD(lParam);
            window->width = width;
            window->height = height;

            if (s_System.eventDriver) {
                PalEventDriver* driver = s_System.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_WINDOW_RESIZE);

                if (mode == PAL_DISPATCH_CALLBACK) {
                    PalEvent event = {};
                    event.type = PAL_EVENT_WINDOW_RESIZE;
                    event.sourceID = window->id;
                    event.data = palPackUint32(width, height);
                    palPushEvent(driver, &event);

                } else if (mode == PAL_DISPATCH_POLL) {
                    s_Event.pendingResize = true;
                    s_Event.width = width;
                    s_Event.height = height;
                    s_Event.sourceID = window->id;
                }
            }
            return 0;
        }

        case WM_MOVE: {
            Int32 x = GET_X_LPARAM(lParam);
            Int32 y = GET_Y_LPARAM(lParam);
            window->x = x;
            window->y = y;

            if (s_System.eventDriver) {
                PalEventDriver* driver = s_System.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_WINDOW_MOVE);

                if (mode == PAL_DISPATCH_CALLBACK) {
                    PalEvent event = {};
                    event.type = PAL_EVENT_WINDOW_MOVE;
                    event.sourceID = window->id;
                    event.data = palPackInt32(x, y);
                    palPushEvent(driver, &event);

                } else if (mode == PAL_DISPATCH_POLL) {
                    s_Event.pendingMove = true;
                    s_Event.x = x;
                    s_Event.y = y;
                    s_Event.sourceID = window->id;
                }
            }
            return 0;
        }

        case WM_DPICHANGED: {
            UINT dpi = HIWORD(wParam);
            if (window->highDPI) {
                RECT* rect = (RECT*)lParam;

                Int32 x = rect->left;
                Int32 y = rect->top;
                Uint32 w = rect->right - rect->left;
                Uint32 h = rect->bottom - rect->top;

                SetWindowPos(
                    hwnd, 
                    nullptr, 
                    x, 
                    y, 
                    w, 
                    h,
                    SWP_NOZORDER | SWP_NOACTIVATE
                );

                window->x = x;
                window->y = y;
                window->width = w;
                window->height = h;
            }

            if (s_System.eventDriver) {
                PalEventDriver* driver = s_System.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_DPI_CHANGED);
                if (mode != PAL_DISPATCH_NONE) {
                    PalEvent event = {};
                    event.type = PAL_EVENT_DPI_CHANGED;
                    event.sourceID = window->id;
                    event.data = dpi;
                    palPushEvent(driver, &event);
                }
            }
            return 0;
        }

        case WM_DEVICECHANGE: {
            if (s_System.eventDriver) {
                PalEventDriver* driver = s_System.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_DISPLAYS_CHANGED);
                if (wParam == DBT_DEVNODES_CHANGED) {
                    if (mode != PAL_DISPATCH_NONE) {
                        PalEvent event = {};
                        event.type = PAL_EVENT_DISPLAYS_CHANGED;
                        event.sourceID = window->id;
                        palPushEvent(driver, &event);
                    }
                }
            }

            break;
        }

        case WM_LBUTTONDOWN: {
            SetCapture(hwnd);
            return 0;
            break;
        }

        case WM_LBUTTONUP:
        case WM_CAPTURECHANGED: {
            ReleaseCapture();
            return 0;
            break;
        }

        case WM_MOUSEMOVE: {
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);

            if (s_System.eventDriver) {
                PalEventDriver* driver = s_System.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_MOUSE_MOVE);

                if (mode == PAL_DISPATCH_NONE) {
                    PalEvent event = {};
                    event.type = PAL_EVENT_MOUSE_MOVE;
                    event.sourceID = window->id;
                    event.data = palPackInt32(x, y);
                    palPushEvent(driver, &event);
                }
            }
            return 0;
        }

        case WM_ERASEBKGND: {
            return 1;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}