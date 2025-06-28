
#include "pal_pch.h"
#include "pal_win32.h"
#include "pal_video_c.h"

static int s_RegCount = 0;

bool palRegisterWindowClass()
{
    if (!s_RegCount) {
        s_Win32.hInstance = GetModuleHandleW(PAL_NULL);
        WNDCLASSEXW wc = {};
        wc.cbClsExtra = 0;
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.cbWndExtra = 0;
        wc.hbrBackground = NULL;
        wc.hCursor = LoadCursorW(s_Win32.hInstance, IDC_ARROW);
        wc.hIcon = LoadIconW(s_Win32.hInstance, IDI_APPLICATION);
        wc.hIconSm = LoadIconW(s_Win32.hInstance, IDI_APPLICATION);
        wc.hInstance = s_Win32.hInstance;
        wc.lpfnWndProc = palProcWin32;
        wc.lpszClassName = WIN32_CLASS;
        wc.lpszMenuName = NULL;
        wc.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

        ATOM success = RegisterClassExW(&wc);
        if (!success) {
            return PAL_FALSE;
        }
        
        s_RegCount++;
        return PAL_TRUE;
    }
}

void palUnregisterWindowClass()
{
    s_RegCount--;
    if (s_RegCount <= 0) {
        UnregisterClassW(WIN32_CLASS, s_Win32.hInstance);
    }
}

bool palLoadLibraries()
{
    // shcore.dll
    s_Win32.shcore = LoadLibraryA("shcore.dll");
    if (!s_Win32.shcore) {
        palSetError(PAL_PLATFORM_ERROR);
        return PAL_FALSE;
    }

    // ntdll.dll
    s_Win32.ntdll = GetModuleHandleA("ntdll.dll");

    // load function pointers
    s_Win32.getDpiForMonitor = (GetDpiForMonitorFn)GetProcAddress(
        s_Win32.shcore, 
        "GetDpiForMonitor"
    );

    s_Win32.setProcessAwareness = (SetProcessAwarenessFn)GetProcAddress(
        s_Win32.shcore, 
        "SetProcessDpiAwareness"
    );

    s_Video.displayCount = 0;
    EnumDisplayMonitors(PAL_NULL, PAL_NULL, palMonitorProcWin32, 0);
    return PAL_TRUE;
}

void palUnloadLibraries()
{
    for (int i = 0; i < s_Video.displayCount; i++) {
        PalDisplay* display = &s_Video.displays[i];
        s_Video.allocator.free(display->name);
        s_Video.allocator.free(display->modes);
    }
}

bool palCompareDisplayMode(const PalDisplayMode* a, const PalDisplayMode* b)
{
    int same = a->alphaBits == b->alphaBits &&
           a->redBits == b->redBits         &&
           a->greenBits == b->greenBits     &&
           a->blueBits == b->blueBits       &&
           a->alphaBits == b->alphaBits     &&
           a->width == b->width             &&
           a->height == b->height           &&
           a->refreshRate == b->refreshRate;

    if (same) {
        return PAL_TRUE;
    }
    return PAL_FALSE;
}

void palAddDisplayMode(const PalDisplayMode* mode)
{
    PalDisplay* display = &s_Video.displays[s_Video.displayCount];
    for (int i = 0; i < display->modeCount; i++) {
        if (palCompareDisplayMode(&display->modes[i], mode) == PAL_TRUE) {
            return;
        }
    }

    display->modes[display->modeCount] = *mode;
    display->modeCount++;
}

void palGetColorBits(PalDisplayMode* mode, int bpp)
{
    switch (bpp) {
        case 16: {
            mode->redBits = 5;
            mode->greenBits = 6;
            mode->blueBits = 5;
            mode->alphaBits = 0;
            return;
        }

        case 24: {
            mode->redBits = mode->greenBits = 8;
            mode->blueBits = 8;
            mode->alphaBits = 0;
            return;
        }

        case 32: {
            mode->redBits = mode->greenBits = 8;
            mode->blueBits = mode->alphaBits = 8;
            return;
        }
    }
    mode->redBits = mode->greenBits = 0;
    mode->blueBits = mode->alphaBits = 0;

}

void palGetDisplayDPI(PalDisplay* display)
{
    UINT dpiX, dpiY;
    if (s_Win32.setProcessAwareness) {
        s_Win32.setProcessAwareness(WIN32_DPI_AWARE);
    }

    if (s_Win32.getDpiForMonitor) {
        s_Win32.getDpiForMonitor(
            (HMONITOR)display->handle,
            WIN32_DPI,
            &dpiX,
            &dpiY
        );

        display->dpiScaleX = dpiX / 96.0f;
        display->dpiScaleY = dpiY / 96.0f;

    } else {
        display->dpiScaleX = 1.0f;
        display->dpiScaleY = 1.0f;
    }
}

BOOL CALLBACK palMonitorProcWin32(HMONITOR monitor, HDC, LPRECT, LPARAM)
{
    if (s_Video.displayCount >= PAL_MAX_DISPLAYS) { 
        return PAL_FALSE; 
    }

    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        return TRUE; 
    }

    PalDisplay* display = &s_Video.displays[s_Video.displayCount];
    display->index = s_Video.displayCount;
    display->handle = (void*)monitor;

    // size and pos
    display->x = mi.rcMonitor.left;
    display->y = mi.rcMonitor.top;
    display->width = mi.rcMonitor.right - mi.rcMonitor.left;
    display->height = mi.rcMonitor.bottom - mi.rcWork.top;

    display->name = s_Video.allocator.alloc(PAL_DISPLAY_NAME_SIZE);
    display->modes = s_Video.allocator.alloc(
        PAL_MAX_DISPLAY_MODES * sizeof(PalDisplayMode)
    );

    // change into a UTF8 string
    WideCharToMultiByte(CP_UTF8, 0, mi.szDevice, -1, display->name, 32, NULL, NULL);
    palGetDisplayDPI(display);

    // get modes
    DEVMODEW dm = {};
    dm.dmSize = sizeof(DEVMODE);
    int i = 0;
    for (i = 0; EnumDisplaySettingsW(mi.szDevice, i, &dm); i++) {
        if (i >= PAL_MAX_DISPLAY_MODES) {
            break;
        }

        PalDisplayMode mode;
        mode.refreshRate = dm.dmDisplayFrequency;
        mode.width = dm.dmPelsWidth;
        mode.height = dm.dmPelsHeight;
        palGetColorBits(&mode, dm.dmBitsPerPel);
        palAddDisplayMode(&mode);
    }

    display->modeCount--;
    display->refreshRate = display->modes[display->modeCount].refreshRate;
    s_Video.displayCount++;
    return PAL_TRUE;
}

// Window
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

    window = s_Video.allocator.alloc(sizeof(PalWindow));
    palZeroMemory(window, sizeof(PalWindow));
    if (!window) {
        palSetError(PAL_OUT_OF_MEMORY);
        return PAL_NULL;
    }

    wchar_t buffer[PAL_MSG_SIZE] = {};
    palToWstrUTF8Win32(buffer, desc->title);
    window->hidden = PAL_TRUE;

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
        s_Win32.hInstance,
        PAL_NULL
    );

    if (!handle) {
        palSetError(PAL_PLATFORM_ERROR);
        s_Video.allocator.free(window);
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

    ShowWindow(handle, showFlag);
    SetPropW(handle, WIN32_PROP, window);

    window->handle = handle;
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
    palHashMapInsert(&s_Video.map, window->id, window);

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

    DestroyWindow((HWND)window->handle);
    palHashMapPop(&s_Video.map, window->id);
    s_Video.allocator.free(window);
}

void _PCALL palShowWindow(PalWindow* window)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return;
    }

    if (window->maximized) {
        ShowWindow((HWND)window->handle, SW_SHOWMAXIMIZED);

    } else {
        ShowWindow((HWND)window->handle, SW_SHOW);
    }
    window->hidden = PAL_FALSE;
}

void _PCALL palHideWindow(PalWindow* window)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return;
    }

    ShowWindow((HWND)window->handle, SW_HIDE);
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
        (HWND)window->handle, NULL, x, y, 
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

    int displayIndex = palGetWindowDisplayIndex(window);
    palSetWindowFullScreen(window, displayIndex, PAL_FALSE);
    ShowWindow((HWND)window->handle, SW_SHOWMAXIMIZED);
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

    int displayIndex = palGetWindowDisplayIndex(window);
    palSetWindowFullScreen(window, displayIndex, PAL_FALSE);
    ShowWindow((HWND)window->handle, SW_MINIMIZE);
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
            (HWND)window->handle,
            GWL_STYLE,
            WS_POPUP | WS_VISIBLE
        );

        SetWindowLongPtrW(
            (HWND)window->handle,
            GWL_EXSTYLE,
            WS_EX_APPWINDOW
        );

        SetWindowPos(
            (HWND)window->handle,
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
            (HWND)window->handle,
            GWL_STYLE,
            window->style
        );

        SetWindowLongPtrW(
            (HWND)window->handle,
            GWL_EXSTYLE,
            window->exStyle
        );

        int showFlag = SWP_HIDEWINDOW;
        if (!window->hidden || !window->minimized) {
            showFlag = SWP_SHOWWINDOW;
        }

        SetWindowPos(
            (HWND)window->handle,
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

int _PCALL palGetWindowDisplayIndex(PalWindow* window)
{
    if (!window) {
        palSetError(PAL_NULL_POINTER);
        return -1;
    }

    HMONITOR monitor = MonitorFromWindow((HWND)window->handle, MONITOR_DEFAULTTONEAREST);
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

    wchar_t buffer[PAL_MSG_SIZE] = {};
    palToWstrUTF8Win32(buffer, title);
    SetWindowTextW((HWND)window->handle, buffer);
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
        (HWND)window->handle, NULL, rect.left, rect.top, 
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

    SetWindowPos((HWND)window->handle, HWND_TOP, 0, 0, 
        rect.right - rect.left, rect.bottom - rect.top,
        SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER
    );
}