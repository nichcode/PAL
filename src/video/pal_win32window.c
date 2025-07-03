
#include "pal_pch.h"
#include "pal_video_c.h"

#define PAL_CHECK_WINDOW(window, ret) {              \
    if (!window) {                                   \
        palSetError(PAL_NULL_POINTER);               \
        return ret;                                  \
    }                                                \
} 

#define WIN32_CLASS L"PALClass"
#define WIN32_PROP L"PAL"

static HINSTANCE s_Instance;

void toWideString(wchar_t* buffer, const char* string);
LRESULT CALLBACK palWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef struct PalWindow
{
    HWND handle;
    const char* title;
    Uint32 id;

    Uint32 style;
    Uint32 exStyle;
    Uint32 width;
    Uint32 height;
    int x;
    int y;

    bool maximized;
    bool minimized;
    bool fullscreen;
    bool hidden;

} PalWindow;

PalWindow* _PCALL palCreateWindow(PalWindowDesc* desc)
{
    PAL_CHECK_VIDEO(PAL_NULL);
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

    Uint64 len = strlen(desc->title);
    wchar_t buffer[len + 1];
    palZeroMemory(buffer, len + 1);
    toWideString(buffer, desc->title);

    HWND handle = CreateWindowExW(
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
        s_Instance,
        PAL_NULL
    );

    if (!handle) {
        g_Video.allocator.free(window);
        return PAL_NULL;
    }

    window = g_Video.allocator.alloc(sizeof(PalWindow));
    palZeroMemory(window, sizeof(PalWindow));
    if (!window) {
        palSetError(PAL_OUT_OF_MEMORY);
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
    } else {
        window->hidden = PAL_TRUE;
    }

    ShowWindow(handle, showFlag);
    SetPropW(handle, WIN32_PROP, window);

    window->handle = handle;
    window->title = desc->title;
    window->style = style;
    window->exStyle = exStyle;
    window->width = desc->width;
    window->height = desc->height;
    window->x = x;
    window->y = y;

    // set windowID
    window->id = g_Video.nextWindowID;
    g_Video.nextWindowID++;
    palHashMapInsert(&g_Video.map, window->id, window);

    // fullscreen
    if (desc->flags & PAL_WINDOW_FULLSCREEN) {
        palSetWindowFullScreen(window, desc->displayIndex, PAL_TRUE);
    }

    return window;
}

void _PCALL palDestroyWindow(PalWindow* window)
{
    PAL_CHECK_WINDOW(window, );
    DestroyWindow(window->handle);
    palHashMapPop(&g_Video.map, window->id);
    g_Video.allocator.free(window);
}

void _PCALL palUpdateWindows()
{
    MSG msg;
    while (PeekMessageA(&msg, PAL_NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

void _PCALL palShowWindow(PalWindow* window)
{
    PAL_CHECK_WINDOW(window, );
    if (window->maximized) {
        ShowWindow(window->handle, SW_SHOWMAXIMIZED);

    } else {
        ShowWindow(window->handle, SW_SHOW);
    }
    window->hidden = PAL_FALSE;
}

void _PCALL palHideWindow(PalWindow* window)
{
    PAL_CHECK_WINDOW(window, );
    ShowWindow(window->handle, SW_HIDE);
    window->hidden = PAL_TRUE;
}

void _PCALL palCenterWindow(PalWindow* window, int displayIndex)
{
    PAL_CHECK_WINDOW(window, );
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

    window->x = x;
    window->y = y;
}

void _PCALL palMaximizeWindow(PalWindow* window)
{
    PAL_CHECK_WINDOW(window, );
    if (window->maximized) {
        return;
    }

    int displayIndex = palGetWindowDisplayIndex(window);
    palSetWindowFullScreen(window, displayIndex, PAL_FALSE);
    ShowWindow(window->handle, SW_SHOWMAXIMIZED);
    window->maximized = PAL_TRUE;
    window->minimized = PAL_FALSE;
}

void _PCALL palMinimizeWindow(PalWindow* window)
{
    PAL_CHECK_WINDOW(window, );
    if (window->minimized) {
        return;
    }

    int displayIndex = palGetWindowDisplayIndex(window);
    palSetWindowFullScreen(window, displayIndex, PAL_FALSE);
    ShowWindow(window->handle, SW_MINIMIZE);
    window->minimized = PAL_TRUE;
    window->maximized = PAL_FALSE;
}

const char* _PCALL palGetWindowTitle(PalWindow* window)
{
    PAL_CHECK_WINDOW(window, PAL_NULL);
    return window->title;
}

void _PCALL palGetWindowPos(PalWindow* window, int* x, int* y)
{
    PAL_CHECK_WINDOW(window, );
    if (x) {
        *x = window->x;
    }

    if (y) {
        *y = window->y;
    }
}

void _PCALL palGetWindowSize(PalWindow* window, Uint32* width, Uint32* height)
{
    PAL_CHECK_WINDOW(window, );
    if (width) {
        *width = window->width;
    }

    if (height) {
        *height = window->height;
    }
}

PalWindowID _PCALL palGetWindowID(PalWindow* window)
{
    PAL_CHECK_WINDOW(window, 0);
    return window->id;
}

int _PCALL palGetWindowDisplayIndex(PalWindow* window)
{
    PAL_CHECK_WINDOW(window, -1);
    HMONITOR monitor = MonitorFromWindow(window->handle, MONITOR_DEFAULTTONEAREST);
    if (!monitor) { 
        return -1; 
    }

    for (int i = 0; i < g_Video.displayCount; i++) {
        PalDisplay* display = &g_Video.displays[i];
        if (monitor == display->handle) {
            return i;
        }
    }
}

bool _PCALL palIsWindowMaximized(PalWindow* window)
{
    PAL_CHECK_WINDOW(window, PAL_FALSE);
    return window->maximized;
}

bool _PCALL palIsWindowMinimized(PalWindow* window)
{
    PAL_CHECK_WINDOW(window, PAL_FALSE);
    return window->minimized;
}

bool _PCALL palIsWindowHidden(PalWindow* window)
{
    PAL_CHECK_WINDOW(window, PAL_FALSE);
    return window->hidden;
}

bool _PCALL palIsWindowFullScreen(PalWindow* window)
{
    PAL_CHECK_WINDOW(window, PAL_FALSE);
    return window->fullscreen;
}

void _PCALL palSetWindowTitle(PalWindow* window, const char* title)
{
    PAL_CHECK_WINDOW(window, );
    Uint64 len = strlen(title);
    wchar_t buffer[len + 1];
    palZeroMemory(buffer, len + 1);

    toWideString(buffer, title);
    SetWindowTextW(window->handle, buffer);
    window->title = title;
}

void _PCALL palSetWindowPos(PalWindow* window, int x, int y)
{
    PAL_CHECK_WINDOW(window, );
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
    PAL_CHECK_WINDOW(window, );
    if (!width) {
        width = 1;
    }

    if (!height) {
        height = 1;
    }

    RECT rect = { 0, 0, 0, 0 };
    rect.right = width;
    rect.bottom = height;
    AdjustWindowRectEx(&rect, window->style, 0, window->exStyle);

    SetWindowPos(window->handle, HWND_TOP, 0, 0, 
        rect.right - rect.left, rect.bottom - rect.top,
        SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER
    );

    window->width = width;
    window->height = height;
}

void _PCALL palSetWindowFullScreen(PalWindow* window, int displayIndex, bool enable)
{
    PAL_CHECK_WINDOW(window, );
    if (window->fullscreen && enable) {
        // already fullscreen
        return;
    }

    if (!window->fullscreen && !enable) {
        // already windowed
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

    } else {
        // toggle windowed or maximized
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
    }
    window->fullscreen = enable;
}

bool palInitWindow()
{
    s_Instance = GetModuleHandleW(PAL_NULL);

    WNDCLASSEXW wc = {};
    wc.cbClsExtra = 0;
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.cbWndExtra = 0;
    wc.hbrBackground = NULL;
    wc.hCursor = LoadCursorW(s_Instance, IDC_ARROW);
    wc.hIcon = LoadIconW(s_Instance, IDI_APPLICATION);
    wc.hIconSm = LoadIconW(s_Instance, IDI_APPLICATION);
    wc.hInstance = s_Instance;
    wc.lpfnWndProc = palWindowProc;
    wc.lpszClassName = WIN32_CLASS;
    wc.lpszMenuName = NULL;
    wc.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClassExW(&wc)) {
        return PAL_FALSE;
    }

    return PAL_TRUE;
}

void palShutdownWindow()
{
    UnregisterClassW(WIN32_CLASS, s_Instance);
}

void toWideString(wchar_t* buffer, const char* string)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, string, -1, PAL_NULL, 0);
    if (len == 0) { return; }
    MultiByteToWideChar(CP_UTF8, 0, string, -1, buffer, len);
}

LRESULT CALLBACK palWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}