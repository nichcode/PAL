
#include "pal_pch.h"
#include "pal_win32platform.h"
#include "pal/pal_video.h"

PalWindow* _PCALL palCreateWindow(PalWindowDesc* desc)
{
    if (!s_Video.initialized) {
        palSetError(PAL_VIDEO_NOT_INITIALIZED);
        return PAL_NULL;
    }

    PalWindow* window = PAL_NULL;
    if (!desc) {
        palSetError(PAL_NULL_POINTER);
        return PAL_NULL;
    }

    Uint32 style = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED;
    Uint32 exStyle = WS_EX_OVERLAPPEDWINDOW;

    if (desc->flags & PAL_WINDOW_MINIMIZEBOX) {
        style |= WS_MINIMIZEBOX;
    }

    if (desc->flags & PAL_WINDOW_RESIZABLE) {
        style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    }

    RECT rect = { 0, 0, 0, 0 };
    rect.right = desc->width;
    rect.bottom = desc->height;
    AdjustWindowRectEx(&rect, style, 0, exStyle);

    // get display
    const PalDisplay* display = palGetDisplay(desc->displayIndex);
    if (!display) {
        palSetError(PAL_DISPLAY_ERROR);
        return PAL_NULL;
    }

    Uint32 width = rect.right - rect.left;
    Uint32 height = rect.bottom - rect.top;
    int x = 0;
    int y = 0;
    // we use desc size rather because we are only using the physical size of the display
    if (desc->flags & PAL_WINDOW_CENTER) {
        x = display->x + (display->width - desc->width) / 2;
        y = display->y + (display->height - desc->height) / 2;

    } else {
        // we set 100 for each axix
        x = display->x + 100;
        y = display->y + 100;
    }

    window = s_Video.allocator->alloc(sizeof(PalWindow));
    palZeroMemory(window, sizeof(PalWindow));
    if (!window) {
        palSetError(PAL_OUT_OF_MEMORY);
        return PAL_NULL;
    }

    wchar_t buffer[_PAL_MSG_SIZE] = {};
    palToWstrUTF8Win32(buffer, desc->title);
    window->hidden = PAL_TRUE;

    window->handle = CreateWindowExW(
        exStyle,
        WIN32_CLASS,
        buffer,
        style, 
        x,
        y,
        width,
        height,
        PAL_NULL, 
        PAL_NULL, 
        s_HInstance,
        PAL_NULL
    );

    if (!window->handle) {
        palSetError(PAL_PLATFORM_ERROR);
        s_Video.allocator->free(window);
        return PAL_NULL;
    }

    // show flag
    int showFlag = SW_HIDE;
    if (desc->flags & PAL_WINDOW_MAXIMIZED) {
        showFlag = SW_SHOWMAXIMIZED;
    }

    if (desc->flags & PAL_WINDOW_SHOWN) {
        if (showFlag == SW_HIDE) {
            showFlag = SW_SHOW;
        }
    }

    if (showFlag != SW_HIDE) {
        window->hidden = PAL_FALSE;
    }

    ShowWindow(window->handle, showFlag);
    SetPropW(window->handle, WIN32_PROP, window);

    window->title = desc->title;
    window->style = style;
    window->exStyle = exStyle;
    window->flags = desc->flags;
    window->width = desc->width;
    window->height = desc->height;
    window->x = x;
    window->y = y;

    // set windowID
    window->id = s_Video.nextWindowID;
    s_Video.nextWindowID++;
    _palHashMapInsert(
        &s_Video.windowHashMap,
        window->id,
        window
    );

    // fullscreen
    if (desc->flags & PAL_WINDOW_FULLSCREEN) {
        palSetWindowFullScreen(window, desc->displayIndex, PAL_TRUE);
    }

    return window;
}

void _PCALL palDestroyWindow(PalWindow* window)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return;
    }

    DestroyWindow(window->handle);
    _palHashMapPop(&s_Video.windowHashMap, window->id);
    s_Video.allocator->free(window);
    window = PAL_NULL;
}

void _PCALL palShowWindow(PalWindow* window)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return;
    }

    if (window->maximized) {
        ShowWindow(window->handle, SW_SHOWMAXIMIZED);

    } else {
        ShowWindow(window->handle, SW_SHOW);
    }

    window->hidden = PAL_FALSE;
}

void _PCALL palHideWindow(PalWindow* window)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return;
    }

    ShowWindow(window->handle, SW_HIDE);
    window->hidden = PAL_TRUE;
}

void _PCALL palCenterWindow(PalWindow* window, int displayIndex)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return;
    }

    if (window->fullscreen || window->maximized) {
        return;
    }

    // get display
    const PalDisplay* display = palGetDisplay(displayIndex);
    if (!display) {
        palSetError(PAL_DISPLAY_ERROR);
        return;
    }

    int x = display->x + (display->width - window->width) / 2;
    int y = display->y + (display->height - window->height) / 2;

    SetWindowPos(
        window->handle, NULL, x, y, 
        0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE
    );

    window->flags |= PAL_WINDOW_CENTER;
    window->x = x;
    window->y = y;
}

void _PCALL palMaximizeWindow(PalWindow* window)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return;
    }

    if (window->maximized) {
        return;
    }

    ShowWindow(window->handle, SW_SHOWMAXIMIZED);
    window->maximized = PAL_TRUE;
    window->minimized = PAL_FALSE;
}

void _PCALL palMinimizeWindow(PalWindow* window)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return;
    }

    if (window->minimized) {
        return;
    }

    ShowWindow(window->handle, SW_MINIMIZE);
    window->minimized = PAL_TRUE;
    window->maximized = PAL_FALSE;
}

void _PCALL palSetWindowFullScreen(PalWindow* window, int displayIndex, bool enable)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return;
    }

    if (window->fullscreen && enable) {
        return;
    }

    if (!window->fullscreen && !enable) {
        return;
    }

    const PalDisplay* display = palGetDisplay(displayIndex);
    if (!display) {
        palSetError(PAL_DISPLAY_ERROR);
        return;
    }

    if (enable) {
        // toggle fullscreen
        SetWindowLongPtrW(
            window->handle,
            GWL_STYLE,
            WS_POPUP | WS_VISIBLE
        );

        SetWindowLongPtrW(
            window->handle,
            GWL_EXSTYLE,
            WS_EX_APPWINDOW
        );

        SetWindowPos(
            window->handle,
            HWND_TOP,
            display->x,
            display->y,
            display->width,
            display->height,
            SWP_FRAMECHANGED | SWP_SHOWWINDOW
        );

        window->fullscreen = PAL_TRUE;

    } else {
        // set everything back to normal
        SetWindowLongPtrW(
            window->handle,
            GWL_STYLE,
            window->style
        );

        SetWindowLongPtrW(
            window->handle,
            GWL_EXSTYLE,
            window->exStyle
        );

        int showFlag = SWP_HIDEWINDOW;
        if (!window->hidden || !window->minimized) {
            showFlag = SWP_SHOWWINDOW;
        }

        SetWindowPos(
            window->handle,
            HWND_TOP,
            window->x,
            window->y,
            window->width,
            window->height,
            SWP_FRAMECHANGED | SWP_NOOWNERZORDER | showFlag
        );

        window->fullscreen = PAL_FALSE;
    }
}

const char* _PCALL palGetWindowTitle(PalWindow* window)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return PAL_NULL;
    }

    return window->title;
}

void _PCALL palGetWindowPos(PalWindow* window, int* x, int* y)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return;
    }

    if (x) {
        *x = window->x;
    }

    if (y) {
        *y = window->y;
    }
}

void _PCALL palGetWindowSize(PalWindow* window, Uint32* width, Uint32* height)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return;
    }

    if (width) {
        *width = window->width;
    }

    if (height) {
        *height = window->height;
    }
}

PalWindowFlags _PCALL palGetWindowFlags(PalWindow* window)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return 0;
    }
    return window->flags;
}

PalWindowID _PCALL palGetWindowID(PalWindow* window)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return 0;
    }
    return window->id;
}

int _PCALL palGetWindowDisplayIndex(PalWindow* window)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return -1;
    }

    HMONITOR monitor = MonitorFromWindow(window->handle, MONITOR_DEFAULTTONEAREST);
    if (!monitor) { 
        return -1; 
    }

    MONITORINFO info = {};
    info.cbSize = sizeof(MONITORINFO);
    GetMonitorInfoW(monitor, &info);

    for (int i = 0; i < s_Video.displayCount; i++) {
        PalDisplay* display = &s_Video.displays[i];
        if (monitor == display->handle) {
            return i;
        }
    }
}

void _PCALL palSetWindowTitle(PalWindow* window, const char* title)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return;
    }

    wchar_t buffer[_PAL_MSG_SIZE] = {};
    palToWstrUTF8Win32(buffer, title);
    SetWindowTextW(window->handle, buffer);
    window->title = title;
}

void _PCALL palSetWindowPos(PalWindow* window, int x, int y)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return;
    }

    if (!x && !y) {
        palSetError(PAL_INVALID_ARG);
        return;
    }

    RECT rect = { x, y, x, y };
    AdjustWindowRectEx(&rect, window->style, 0, window->exStyle);
    window->x = x;
    window->y = y;

    SetWindowPos(
        window->handle, NULL, rect.left, rect.top, 
        0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE
    );
}

void _PCALL palSetWindowSize(PalWindow* window, Uint32 width, Uint32 height)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return;
    }

    if (!width && !height) {
        palSetError(PAL_INVALID_ARG);
        return;
    }

    RECT rect = { 0, 0, 0, 0 };
    rect.right = width;
    rect.bottom = height;
    AdjustWindowRectEx(&rect, window->style, 0, window->exStyle);
    window->width = width;
    window->height = height;

    SetWindowPos(window->handle, HWND_TOP, 0, 0, 
        rect.right - rect.left, rect.bottom - rect.top,
        SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER
    );
}

bool _PCALL palIsWindowMaximized(PalWindow* window)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return PAL_FALSE;
    }
    return window->maximized;
}

bool _PCALL palIsWindowMinimized(PalWindow* window)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return PAL_FALSE;
    }
    return window->minimized;
}

bool _PCALL palIsWindowHidden(PalWindow* window)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return PAL_FALSE;
    }
    return window->hidden;
}

bool _PCALL palIsWindowFullScreen(PalWindow* window)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return PAL_FALSE;
    }
    return window->fullscreen;
}