/**
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifdef _WIN32

#include "win32_helper.h"
#include "pal2/video/window.h"
#include <stdlib.h>

#define WINDOW_NAME_SIZE 256

/* =========================================================================
 * Helper
 * ========================================================================= */

static WindowData* getFreeWindowData(void)
{
    for (uint32_t i = 0; i < g_Win32.maxWindowData; ++i) {
        if (!g_Win32.windowData[i].used) {
            memset(&g_Win32.windowData[i], 0, sizeof(WindowData));
            g_Win32.windowData[i].used = PAL_TRUE;
            return &g_Win32.windowData[i];
        }
    }

    uint32_t oldCount = g_Win32.maxWindowData;
    uint32_t count = (oldCount > 0) ? (oldCount * 2) : 4;
    WindowData* data = palAllocate(g_Win32.allocator, sizeof(WindowData) * count, 0);
    if (data) {
        memset(data, 0, sizeof(WindowData) * count);
        if (g_Win32.windowData && oldCount > 0) {
            memcpy(data, g_Win32.windowData, oldCount * sizeof(WindowData));
            palFree(g_Win32.allocator, g_Win32.windowData);
        }

        g_Win32.windowData = data;
        g_Win32.maxWindowData = count;

        g_Win32.windowData[oldCount].used = PAL_TRUE;
        return &g_Win32.windowData[oldCount];
    }
    return nullptr;
}

LRESULT CALLBACK videoProc(
    HWND,
    UINT,
    WPARAM,
    LPARAM);

/* =========================================================================
 * API
 * ========================================================================= */

PalResult win32CreateWindow(
    const PalWindowCreateInfo* info, 
    PalWindow** window)
{
    WindowData* data = getFreeWindowData();
    if (!data) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    HWND handle = nullptr;
    PalMonitor* monitor = nullptr;
    PalMonitorInfo monitorInfo;

    uint32_t style = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED;
    uint32_t exStyle = 0;

    if (!(info->style & PAL_WINDOW_STYLE_NO_MINIMIZEBOX)) {
        style |= WS_MINIMIZEBOX;
    }

    if (!(info->style & PAL_WINDOW_STYLE_NO_MAXIMIZEBOX)) {
        style |= WS_MAXIMIZEBOX;
    }

    if (info->style & PAL_WINDOW_STYLE_RESIZABLE) {
        style |= WS_THICKFRAME;

    } else {
        // not resizable. We remove the maximizebox even if user requested
        style &= ~WS_MAXIMIZEBOX;
    }

    if (info->style & PAL_WINDOW_STYLE_TRANSPARENT) {
        exStyle |= WS_EX_LAYERED;
    }

    if (info->style & PAL_WINDOW_STYLE_TOOL) {
        exStyle |= WS_EX_TOOLWINDOW;
    }

    if (info->style & PAL_WINDOW_STYLE_TOPMOST) {
        exStyle |= WS_EX_TOPMOST;
    }

    if (info->monitor) {
        monitor = info->monitor;
    } else {
        monitor = (PalMonitor*)MonitorFromPoint((POINT){0, 0}, MONITOR_DEFAULTTOPRIMARY);
        if (!monitor) {
            return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, GetLastError());
        }
    }

    palGetMonitorInfo(monitor, &monitorInfo);
    int32_t x, y = 0;
    if (info->center) {
        x = monitorInfo.x + (monitorInfo.width - info->width) / 2;
        y = monitorInfo.y + (monitorInfo.height - info->height) / 2;
    } else {
        x = monitorInfo.x + 100;
        y = monitorInfo.y + 100;
    }

    RECT rect = {0, 0, 0, 0};
    rect.right = info->width;
    rect.bottom = info->height;
    AdjustWindowRectEx(&rect, style, 0, exStyle);

    wchar_t buffer[WINDOW_NAME_SIZE];
    MultiByteToWideChar(CP_UTF8, 0, info->title, -1, buffer, 256);

    handle = CreateWindowExW(
        exStyle,
        VIDEO_CLASS,
        buffer,
        style,
        x,
        y,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr,
        nullptr,
        g_Win32.instance,
        nullptr);

    if (!handle) {
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, GetLastError());
    }

    if (info->fbConfigIndex) {
        // clang-format off
        if (info->fbConfigBackend == PAL_FBCONFIG_BACKEND_EGL  ||
            info->fbConfigBackend == PAL_FBCONFIG_BACKEND_GLX) {
            return palMakeResult(
                PAL_RESULT_CODE_INVALID_ARGUMENT, 
                PAL_RESULT_SOURCE_WIN32, 
                GetLastError());
        }
        // clang-format on

        HDC hdc = GetDC(handle);

        // we ask the OS (platform) to fill the pfd struct for us from that index.
        PIXELFORMATDESCRIPTOR pfd;
        if (!g_Win32.describePixelFormat(hdc, info->fbConfigIndex, sizeof(PIXELFORMATDESCRIPTOR), &pfd)) {
            return palMakeResult(PAL_RESULT_CODE_INVALID_ARGUMENT, PAL_RESULT_SOURCE_WIN32, GetLastError());
        }

        g_Win32.setPixelFormat(hdc, info->fbConfigIndex, &pfd);
        ReleaseDC(handle, hdc);
    }

    int32_t showFlag = SW_HIDE;
    if (info->state == PAL_WINDOW_STATE_MAXIMIZED) {
        showFlag = SW_MAXIMIZE;
        data->state = PAL_WINDOW_STATE_MAXIMIZED;
    }

    if (info->state == PAL_WINDOW_STATE_MINIMIZED) {
        showFlag = SW_MINIMIZE;
        data->state = PAL_WINDOW_STATE_MINIMIZED;
    }

    if (info->show) {
        if (showFlag == SW_HIDE) {
            showFlag = SW_SHOW;
            data->state = PAL_WINDOW_STATE_RESTORED;
        }
    }

    ShowWindow(handle, showFlag);
    UpdateWindow(handle);

    if (info->style & PAL_WINDOW_STYLE_BORDERLESS) {
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
    SetPropW(handle, VIDEO_PROP, data);
    
    *window = (PalWindow*)handle;
    return PAL_RESULT_SUCCESS;
}

void win32DestroyWindow(PalWindow* window)
{
    WindowData* data = (WindowData*)GetPropW((HWND)window, VIDEO_PROP);
    if (data->isAttached) {
        return;
    }

    DestroyWindow((HWND)window);
    data->used = PAL_FALSE;
}

void win32MaximizeWindow(PalWindow* window)
{
    ShowWindow((HWND)window, SW_MINIMIZE);
}

void win32MinimizeWindow(PalWindow* window)
{
    ShowWindow((HWND)window, SW_MAXIMIZE);
}

void win32RestoreWindow(PalWindow* window)
{
    ShowWindow((HWND)window, SW_RESTORE);
}

void win32ShowWindow(PalWindow* window)
{
    ShowWindow((HWND)window, SW_SHOW);
}

void win32HideWindow(PalWindow* window)
{
    ShowWindow((HWND)window, SW_HIDE);
}

void win32FlashWindow(
    PalWindow* window, 
    const PalFlashInfo* info)
{
    DWORD flags = 0;
    if (info->flags == PAL_FLASH_FLAG_STOP) {
        flags = FLASHW_STOP;

    } else {
        if (info->flags & PAL_FLASH_FLAG_CAPTION) {
            flags |= FLASHW_CAPTION;
        }
        if (info->flags & PAL_FLASH_FLAG_TRAY) {
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

    FlashWindowEx(&flashInfo);
}

PalResult win32AttachWindow(
    void* windowHandle,
    PalWindow** window)
{
    WindowData* data = getFreeWindowData();
    if (!data) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    PalWindow* _window = (PalWindow*)windowHandle;
    data->isAttached = PAL_TRUE;
    data->wndProc = SetWindowLongPtrW((HWND)windowHandle, GWLP_WNDPROC, (LONG_PTR)videoProc);
    data->cursor = nullptr;

    palGetWindowState(_window, &data->state);
    SetPropW((HWND)_window, VIDEO_PROP, data);

    *window = _window;
    return PAL_RESULT_SUCCESS;
}

PalResult win32DetachWindow(
    PalWindow* window,
    void** windowHandle)
{
    WindowData* data = nullptr;
    data = (WindowData*)GetPropW((HWND)window, VIDEO_PROP);
    if (!data) {
        return palMakeResult(PAL_RESULT_CODE_INVALID_HANDLE, PAL_RESULT_SOURCE_WIN32, GetLastError());
    }

    if (data->isAttached == PAL_FALSE) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    data->used = PAL_FALSE;
    SetWindowLongPtrW((HWND)window, GWLP_WNDPROC, data->wndProc);
    if (windowHandle) {
        *windowHandle = (void*)window;
    }

    return PAL_RESULT_SUCCESS;
}
    
void win32GetWindowStyle(
    PalWindow* window, 
    PalWindowStyle* style)
{
    PalWindowStyle windowStyle = 0;
    DWORD _style = (DWORD)GetWindowLongPtrW((HWND)window, GWL_STYLE);
    DWORD exStyle = (DWORD)GetWindowLongPtrW((HWND)window, GWL_EXSTYLE);

    if (_style & WS_THICKFRAME) {
        windowStyle |= PAL_WINDOW_STYLE_RESIZABLE;
    }

    if (exStyle & WS_EX_LAYERED) {
        windowStyle |= PAL_WINDOW_STYLE_TRANSPARENT;
    }

    if (exStyle & WS_EX_TOPMOST) {
        windowStyle |= PAL_WINDOW_STYLE_TOPMOST;
    }

    if (!(_style & WS_MINIMIZEBOX)) {
        windowStyle |= PAL_WINDOW_STYLE_NO_MINIMIZEBOX;
    }

    if (!(_style & WS_MAXIMIZEBOX)) {
        windowStyle |= PAL_WINDOW_STYLE_NO_MAXIMIZEBOX;
    }

    if (exStyle & WS_EX_TOOLWINDOW) {
        windowStyle |= PAL_WINDOW_STYLE_TOOL;
    }

    if (_style & WS_POPUP) {
        windowStyle |= PAL_WINDOW_STYLE_BORDERLESS;
        windowStyle &= ~PAL_WINDOW_STYLE_NO_MINIMIZEBOX;
        windowStyle &= ~PAL_WINDOW_STYLE_NO_MAXIMIZEBOX;
    }

    *style = windowStyle;
}

void win32GetWindowMonitor(
    PalWindow* window, 
    PalMonitor** monitor)
{
    *monitor = (PalMonitor*)MonitorFromWindow((HWND)window, MONITOR_DEFAULTTONEAREST);
}

void win32GetWindowTitle(
    PalWindow* window, 
    uint64_t bufferSize, 
    uint64_t* size, 
    char* buffer)
{
    wchar_t tmp[WINDOW_NAME_SIZE];
    GetWindowTextW((HWND)window, tmp, WINDOW_NAME_SIZE);
    int len = WideCharToMultiByte(CP_UTF8, 0, tmp, -1, nullptr, 0, 0, 0);
    if (size) {
        *size = len - 1;
    }

    if (buffer && bufferSize > 0) {
        int write = (int)bufferSize - 1;
        WideCharToMultiByte(CP_UTF8, 0, tmp, -1, buffer, write + 1, 0, 0);
        buffer[write < len - 1 ? write : len - 1] = '\0';
    }
}

void win32GetWindowPos(
    PalWindow* window, 
    int32_t* x, 
    int32_t* y)
{
    //TODO: client
    RECT rect;
    GetWindowRect((HWND)window, &rect);

    if (x) {
        *x = rect.left;
    }

    if (y) {
        *y = rect.top;
    }
}

void win32GetWindowSize(
    PalWindow* window, 
    uint32_t* width, 
    uint32_t* height)
{
    //TODO: client
    RECT rect;
    GetWindowRect((HWND)window, &rect);

    if (width) {
        *width = rect.right - rect.left;
    }

    if (height) {
        *height = rect.bottom - rect.top;
    }
}

void win32GetWindowState(
    PalWindow* window, 
    PalWindowState* state)
{
    WINDOWPLACEMENT wp = {0};
    GetWindowPlacement((HWND)window, &wp);

    if (wp.showCmd == SW_MINIMIZE) {
        *state = PAL_WINDOW_STATE_MINIMIZED;

    } else if (wp.showCmd == SW_MAXIMIZE) {
        *state = PAL_WINDOW_STATE_MAXIMIZED;

    } else if (wp.showCmd == SW_RESTORE || wp.showCmd == SW_NORMAL) {
        *state = PAL_WINDOW_STATE_RESTORED;
    }
}

PalBool win32IsWindowVisible(PalWindow* window)
{
    return IsWindowVisible((HWND)window);
}

PalWindow* win32GetFocusWindow(void)
{
    return (PalWindow*)GetFocus();
}

void win32GetWindowHandleInfo(
    PalWindow* window, 
    PalWindowHandleInfo* info)
{
    info->nativeInstance = (void*)g_Win32.instance;
    info->nativeWindow = (void*)window;
    info->nativeHandle1 = nullptr;
    info->nativeHandle2 = nullptr;
    info->nativeHandle3 = nullptr;
}

void win32SetWindowOpacity(
    PalWindow* window, 
    float opacity)
{
    SetLayeredWindowAttributes((HWND)window, 0, (BYTE)(opacity * 255), LWA_ALPHA);
}

void win32SetWindowStyle(
    PalWindow* window, 
    PalWindowStyle style)
{
    DWORD win32Style = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED;
    DWORD exStyle = 0;

    if (style & PAL_WINDOW_STYLE_RESIZABLE) {
        win32Style |= WS_THICKFRAME;
    }

    if (style & PAL_WINDOW_STYLE_TRANSPARENT) {
        exStyle |= WS_EX_LAYERED;
    }

    if (style & PAL_WINDOW_STYLE_TOPMOST) {
        exStyle |= WS_EX_TOPMOST;
    }

    if (style & PAL_WINDOW_STYLE_NO_MINIMIZEBOX) {
        win32Style &= ~WS_MINIMIZEBOX;
    }

    if (style & PAL_WINDOW_STYLE_NO_MAXIMIZEBOX) {
        win32Style &= ~WS_MAXIMIZEBOX;
    }

    if (style & PAL_WINDOW_STYLE_TOOL) {
        exStyle |= WS_EX_TOOLWINDOW;
    }

    if (style & PAL_WINDOW_STYLE_BORDERLESS) {
        win32Style = WS_POPUP;
        exStyle = WS_EX_APPWINDOW;
    }

    HWND hwnd = (HWND)window;
    SetWindowLongPtrW(hwnd, GWL_STYLE, win32Style);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);

    // force a frame update
    SetWindowPos(
        hwnd,
        nullptr,
        0,
        0,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

void win32SetWindowTitle(
    PalWindow* window, 
    const char* title)
{
    wchar_t buffer[WINDOW_NAME_SIZE];
    MultiByteToWideChar(CP_UTF8, 0, title, -1, buffer, 256);
    SetWindowTextW((HWND)window, buffer);
}

void win32SetWindowPos(
    PalWindow* window, 
    int32_t x, 
    int32_t y)
{
    // TODO: get client
    SetWindowPos((HWND)window, nullptr, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
}

void win32SetWindowSize(
    PalWindow* window, 
    uint32_t width, 
    uint32_t height)
{
    // TODO: get client
    SetWindowPos(
        (HWND)window,
        HWND_TOP,
        0,
        0,
        width,
        height,
        SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);
}

void win32SetFocusWindow(PalWindow* window)
{
    SetActiveWindow((HWND)window);
}

#endif // _WIN32