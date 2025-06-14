
#include "PAL_pch.h"
#include "PAL_win32platform.h"

PAL_Window* PAL_CreateWindow(const char* title, Uint32 w, Uint32 h, Uint32 flags)
{
    PAL_CHECK_INIT(PAL_NULL)

    Uint32 style = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED;
    Uint32 exStyle = WS_EX_OVERLAPPEDWINDOW;

    if (flags & PAL_WINDOW_MINIMIZEBOX) {
        style |= WS_MINIMIZEBOX;
    }

    if (flags & PAL_WINDOW_RESIZABLE) {
        style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    }

    RECT rect = { 0, 0, 0, 0 };
    rect.right = w;
    rect.bottom = h;
    AdjustWindowRectEx(&rect, style, 0, exStyle);

    Uint32 width = rect.right - rect.left;
    Uint32 height = rect.bottom - rect.top;
    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;

    PAL_Window* window = (PAL_Window*)PAL_Alloc(sizeof(PAL_Window));
    if (!window) {
        PAL_SetError(PAL_ALLOCATION_ERROR, "Memory allocation failed in PAL_CreateWindow");
        return PAL_NULL;
    }

    wchar_t buffer[PAL_MESSAGE_SIZE] = {};
    PAL_ToWideStringUTF8Win32(buffer, title);

    window->handle = CreateWindowExW(
        exStyle,
        s_PALWin32.className,
        buffer,
        style, 
        x,
        y,
        width,
        height,
        PAL_NULL, 
        PAL_NULL, 
        s_PALWin32.instance,
        PAL_NULL
    );

    if (!window->handle) {
        PAL_SetErrorWin32("Win32: Failed to create window");
        PAL_Free(window);
        return PAL_NULL;
    }

    if (flags & PAL_WINDOW_CENTER) {
        // Get monitor
        window->monitor.handle = MonitorFromWindow(
            window->handle, 
            MONITOR_DEFAULTTONEAREST
        );

        if (window->monitor.handle) {
            MONITORINFO monitorInfo;
            monitorInfo.cbSize = sizeof(MONITORINFO);
            GetMonitorInfoW(window->monitor.handle, &monitorInfo);

            window->monitor.w = monitorInfo.rcMonitor.right;
            window->monitor.h = monitorInfo.rcMonitor.bottom;
            window->x = (window->monitor.w - w) / 2;
            window->y = (window->monitor.h - h) / 2;
            SetWindowPos(
                window->handle,
                0, window->x, window->y, 0, 0, 
                SWP_NOZORDER | SWP_NOSIZE
            );
        }
    }

    window->w = width;
    window->h = height;
    window->x = x;
    window->y = y;

    int showFlag = SW_HIDE;
    if (flags & PAL_WINDOW_MAXIMIZED) {
        showFlag = SW_SHOWMAXIMIZED;
    }

    if (flags & PAL_WINDOW_SHOWN) {
        if (showFlag == SW_HIDE) {
            showFlag = SW_SHOW;
        }
    }

    ShowWindow(window->handle, showFlag);
    SetPropW(window->handle, s_PALWin32.propName, window);

    return window;
}

void PAL_DestroyWindow(PAL_Window* window)
{
    if (!window) {
        PAL_SetError(PAL_INVALID_ERROR, "PAL_DestroyWindow expects a valid window");
        return;
    }

    DestroyWindow(window->handle);
    PAL_Free(window);
}