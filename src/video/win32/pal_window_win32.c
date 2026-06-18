
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef _WIN32
#include "pal_video_win32.h"
#include "pal_shared.h"

static WindowData* getFreeWindowData()
{
    for (int i = 0; i < s_Video.maxWindowData; ++i) {
        if (!s_Video.windowData[i].used) {
            s_Video.windowData[i].used = PAL_TRUE;
            return &s_Video.windowData[i];
        }
    }

    // resize the data array
    // It is rare for a user to create and manage
    // 32 windows at the same time
    WindowData* data = nullptr;
    int count = s_Video.maxWindowData * 2; // double the size
    int freeIndex = s_Video.maxWindowData + 1;
    data = palAllocate(s_Video.allocator, sizeof(WindowData) * count, 0);
    if (data) {
        memcpy(data, s_Video.windowData, s_Video.maxWindowData * sizeof(WindowData));

        palFree(s_Video.allocator, s_Video.windowData);
        s_Video.windowData = data;
        s_Video.maxWindowData = count;

        s_Video.windowData[freeIndex].used = PAL_TRUE;
        return &s_Video.windowData[freeIndex];
    }
    return nullptr;
}

PalResult PAL_CALL palCreateWindow(
    const PalWindowCreateInfo* info,
    PalWindow** outWindow)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!info || !outWindow) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    WindowData* data = getFreeWindowData();
    if (!data) {
        return palMakeResult(
            PAL_RESULT_OUT_OF_MEMORY, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    HWND handle = nullptr;
    PalMonitor* monitor = nullptr;
    PalMonitorInfo monitorInfo;

    uint32_t style = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED;
    uint32_t exStyle = 0;

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

    // get monitor
    if (info->monitor) {
        monitor = info->monitor;

    } else {
        // get primary monitor
        monitor = (PalMonitor*)MonitorFromPoint((POINT){0, 0}, MONITOR_DEFAULTTOPRIMARY);
        if (!monitor) {
            return palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                GetLastError());
        }
    }

    // get monitor info
    PalResult result = palGetMonitorInfo(monitor, &monitorInfo);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    // compose position.
    int32_t x, y = 0;
    // the position and size must be scaled with the dpi before this call
    if (info->center) {
        x = monitorInfo.x + (monitorInfo.width - info->width) / 2;
        y = monitorInfo.y + (monitorInfo.height - info->height) / 2;

    } else {
        // we set 100 for each axix
        x = monitorInfo.x + 100;
        y = monitorInfo.y + 100;
    }

    // adjust the window size
    RECT rect = {0, 0, 0, 0};
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
        nullptr);

    if (!handle) {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    // set the pixel format is set
    if (info->fbConfigId) {
        // clang-format off
        if (info->fbConfigBackend == PAL_CONFIG_BACKEND_EGL  ||
            info->fbConfigBackend == PAL_CONFIG_BACKEND_GLX) {
            return palMakeResult(
                PAL_RESULT_INVALID_ARGUMENT, 
                PAL_RESULT_SOURCE_WINDOWS, 
                GetLastError());
        }
        // clang-format on

        HDC hdc = GetDC(handle);
        // since we have the pixel format already
        // we ask the OS (platform) to fill the pfd struct for us from that
        // index
        PIXELFORMATDESCRIPTOR pfd;
        if (!s_Video.describePixelFormat(
                hdc,
                info->fbConfigId,
                sizeof(PIXELFORMATDESCRIPTOR),
                &pfd)) {
            return palMakeResult(
                PAL_RESULT_INVALID_ARGUMENT, 
                PAL_RESULT_SOURCE_WINDOWS, 
                GetLastError());
        }

        s_Video.setPixelFormat(hdc, info->fbConfigBackend, &pfd);
        ReleaseDC(handle, hdc);
    }

    // show, maximize and minimize
    int32_t showFlag = SW_HIDE;
    // maximize
    if (info->maximized) {
        showFlag = SW_MAXIMIZE;
        data->state = PAL_WINDOW_STATE_MAXIMIZED;
    }

    // minimized
    if (info->minimized) {
        showFlag = SW_MINIMIZE;
        data->state = PAL_WINDOW_STATE_MINIMIZED;
    }

    // shown
    if (info->show) {
        if (showFlag == SW_HIDE) {
            // change only if maximize and minimize are not set
            showFlag = SW_SHOW;
            data->state = PAL_WINDOW_STATE_RESTORED;
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
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }

    data->isAttached = PAL_FALSE;
    data->cursor = nullptr;
    data->wndProc = (LONG_PTR)videoProc;
    SetPropW(handle, PAL_VIDEO_PROP, data);
    *outWindow = (PalWindow*)handle;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyWindow(PalWindow* window)
{
    if (s_Video.initialized && window) {
        WindowData* data = (WindowData*)GetPropW((HWND)window, PAL_VIDEO_PROP);
        // destroy only PAL created window
        if (data->isAttached) {
            return;
        }

        DestroyWindow((HWND)window);
        data->used = PAL_FALSE;
    }
}

PalResult PAL_CALL palMinimizeWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!ShowWindow((HWND)window, SW_MINIMIZE)) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palMaximizeWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!ShowWindow((HWND)window, SW_MAXIMIZE)) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palRestoreWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!ShowWindow((HWND)window, SW_RESTORE)) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palShowWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!ShowWindow((HWND)window, SW_SHOW)) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palHideWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!ShowWindow((HWND)window, SW_HIDE)) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window || !info) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    DWORD flags = 0;
    if (info->flags == PAL_FLASH_STOP) {
        flags = FLASHW_STOP;

    } else {
        if (info->flags & PAL_FLASH_CAPTION) {
            flags |= FLASHW_CAPTION;
        }
        if (info->flags & PAL_FLASH_TRAY) {
            flags |= FLASHW_TRAY;
            flags |= FLASHW_TIMERNOFG;
        }
    }

    FLASHWINFO flashInfo = {0};
    flashInfo.cbSize = sizeof(FLASHWINFO);
    flashInfo.dwFlags = flags;
    flashInfo.dwTimeout = info->interval;
    flashInfo.hwnd = (HWND)window;
    flashInfo.uCount = info->count;

    PalBool success = FlashWindowEx(&flashInfo);
    if (!success) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return palMakeResult(
                PAL_RESULT_INVALID_HANDLE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);

        } else {
            return palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* outStyle)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window || !outStyle) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    PalWindowStyle windowStyle = 0;
    DWORD style = (DWORD)GetWindowLongPtrW((HWND)window, GWL_STYLE);
    DWORD exStyle = (DWORD)GetWindowLongPtrW((HWND)window, GWL_EXSTYLE);

    if (!style) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
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

PalResult PAL_CALL palGetWindowMonitor(
    PalWindow* window,
    PalMonitor** outMonitor)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window || !outMonitor) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    HMONITOR monitor = nullptr;
    monitor = MonitorFromWindow((HWND)window, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return palMakeResult(
                PAL_RESULT_INVALID_HANDLE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);

        } else {
            return palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);
        }
    }

    *outMonitor = (PalMonitor*)monitor;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetWindowTitle(
    PalWindow* window,
    uint64_t bufferSize,
    uint64_t* outSize,
    char* outBuffer)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window || !outBuffer) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    wchar_t buffer[WINDOW_NAME_SIZE];
    if (GetWindowTextW((HWND)window, buffer, WINDOW_NAME_SIZE) == 0) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    int len = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, nullptr, 0, 0, 0);
    if (outSize) {
        *outSize = len - 1;
    }

    // see if user provided a buffer and write to it
    if (outBuffer && bufferSize > 0) {
        int write = (int)bufferSize - 1;
        WideCharToMultiByte(CP_UTF8, 0, buffer, -1, outBuffer, write + 1, 0, 0);
        outBuffer[write < len - 1 ? write : len - 1] = '\0';
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetWindowPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    RECT rect;
    if (!GetWindowRect((HWND)window, &rect)) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
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
    uint32_t* width,
    uint32_t* height)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    RECT rect;
    if (!GetWindowRect((HWND)window, &rect)) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
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
    PalWindowState* outState)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window || !outState) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    WINDOWPLACEMENT wp = {0};
    if (!GetWindowPlacement((HWND)window, &wp)) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (wp.showCmd == SW_MINIMIZE) {
        *outState = PAL_WINDOW_STATE_MINIMIZED;

    } else if (wp.showCmd == SW_MAXIMIZE) {
        *outState = PAL_WINDOW_STATE_MAXIMIZED;

    } else if (wp.showCmd == SW_RESTORE || wp.showCmd == SW_NORMAL) {
        *outState = PAL_WINDOW_STATE_RESTORED;
    }

    return PAL_RESULT_SUCCESS;
}

PalBool PAL_CALL palIsWindowVisible(PalWindow* window)
{
    if (!s_Video.initialized || !window) {
        return PAL_FALSE;
    }

    return IsWindowVisible((HWND)window);
}

PalWindow* PAL_CALL palGetFocusWindow()
{
    if (!s_Video.initialized) {
        return nullptr;
    }
    return (PalWindow*)GetFocus();
}

PalResult PAL_CALL palGetWindowHandleInfo(
    PalWindow* window, 
    PalWindowHandleInfo* info)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window || !info) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    info->nativeDisplay = (void*)s_Video.instance;
    info->nativeWindow = (void*)window;
    info->nativeHandle1 = nullptr;
    info->nativeHandle2 = nullptr;
    info->nativeHandle3 = nullptr;

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetWindowOpacity(
    PalWindow* window,
    float opacity)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (opacity < 0.0f) {
        opacity = 0.0f;
    }

    if (opacity > 1.0f) {
        opacity = 1.0f;
    }

    PalBool ret = SetLayeredWindowAttributes((HWND)window, 0, (BYTE)(opacity * 255), LWA_ALPHA);
    if (!ret) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return palMakeResult(
                PAL_RESULT_INVALID_HANDLE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);

        } else if (error == ERROR_INVALID_PARAMETER) {
            return palMakeResult(
                PAL_RESULT_INVALID_ARGUMENT, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);

        } else {
            return palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    // convert our style to win32 styles and exStyles
    // all windows have this styles
    DWORD win32Style = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED;
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
    PalBool success = SetWindowPos(
        hwnd,
        nullptr,
        0,
        0,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    if (success) {
        return PAL_RESULT_SUCCESS;

    } else {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return palMakeResult(
                PAL_RESULT_INVALID_HANDLE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);

        } else {
            return palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);
        }
    }
}

PalResult PAL_CALL palSetWindowTitle(
    PalWindow* window,
    const char* title)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window || !title) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    wchar_t buffer[WINDOW_NAME_SIZE];
    MultiByteToWideChar(CP_UTF8, 0, title, -1, buffer, 256);

    if (!SetWindowTextW((HWND)window, buffer)) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetWindowPos(
    PalWindow* window,
    int32_t x,
    int32_t y)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    PalBool success =
        SetWindowPos((HWND)window, nullptr, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);

    if (!success) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return palMakeResult(
                PAL_RESULT_INVALID_HANDLE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);

        } else {
            return palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);
        }
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetWindowSize(
    PalWindow* window,
    uint32_t width,
    uint32_t height)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    PalBool success = SetWindowPos(
        (HWND)window,
        HWND_TOP,
        0,
        0,
        width,
        height,
        SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);

    if (!success) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return palMakeResult(
                PAL_RESULT_INVALID_HANDLE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);

        } else if (error == ERROR_INVALID_PARAMETER) {
            return palMakeResult(
                PAL_RESULT_INVALID_ARGUMENT, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);

        } else {
            return palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);
        }
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetFocusWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!SetActiveWindow((HWND)window)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return palMakeResult(
                PAL_RESULT_INVALID_HANDLE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);

        } else if (error == ERROR_ACCESS_DENIED) {
            return palMakeResult(
                PAL_RESULT_INVALID_OPERATION, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);

        } else {
            return palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);
        }
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palAttachWindow(
    void* windowHandle,
    PalWindow** outWindow)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!windowHandle || !outWindow) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    WindowData* data = getFreeWindowData();
    if (!data) {
        return palMakeResult(
            PAL_RESULT_OUT_OF_MEMORY, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    PalWindow* window = (PalWindow*)windowHandle;
    data->isAttached = PAL_TRUE;
    data->wndProc = SetWindowLongPtrW((HWND)windowHandle, GWLP_WNDPROC, (LONG_PTR)videoProc);

    // use default PAL video cursor
    // there is no way to get the cursor set on the native window
    data->cursor = nullptr;

    // get state
    palGetWindowState(window, &data->state);
    SetPropW((HWND)window, PAL_VIDEO_PROP, data);

    *outWindow = window;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palDetachWindow(
    PalWindow* window,
    void** outWindowHandle)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    WindowData* data = nullptr;
    data = (WindowData*)GetPropW((HWND)window, PAL_VIDEO_PROP);
    if (!data) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (data->isAttached == PAL_FALSE) {
        // window is owned by PAL
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    data->used = PAL_FALSE;
    SetWindowLongPtrW((HWND)window, GWLP_WNDPROC, data->wndProc);
    if (outWindowHandle) {
        *outWindowHandle = (void*)window;
    }

    return PAL_RESULT_SUCCESS;
}

#endif // _WIN32