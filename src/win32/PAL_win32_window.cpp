
#include "PAL_pch.h"
#include "PAL_win32_platform.h"
#include "PAL/PAL.h"

#define WINDOW_ERROR(window, ret) if (!window) \
                    { PAL_ERROR(PAL_INVALID_POINTER, "Window is null or empty "); return ret; }

void PAL_CenterWindow(PAL_Window* window)
{
    MONITORINFO monitor_info;
    monitor_info.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(MonitorFromWindow(window->handle, MONITOR_DEFAULTTONEAREST), &monitor_info);
    u32 max_hwidth = monitor_info.rcMonitor.right;
    u32 max_hheight = monitor_info.rcMonitor.bottom;

    i32 x = (max_hwidth - window->width) / 2;
    i32 y = (max_hheight - window->height) / 2;
    window->x = x;
    window->y = y;
    i32 show_flags = SWP_HIDEWINDOW;
    if (window->flags & PAL_WINDOW_SHOWN) { show_flags = SWP_SHOWWINDOW; }
    SetWindowPos(window->handle, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | show_flags);
}

void PAL_MapKeys(PAL_Window* window)
{
    window->keycodes[0x01E] = PAL_KEY_A;
    window->keycodes[0x030] = PAL_KEY_B;
    window->keycodes[0x02E] = PAL_KEY_C;
    window->keycodes[0x020] = PAL_KEY_D;
    window->keycodes[0x012] = PAL_KEY_E;
    window->keycodes[0x021] = PAL_KEY_F;
    window->keycodes[0x022] = PAL_KEY_G;
    window->keycodes[0x023] = PAL_KEY_H;
    window->keycodes[0x017] = PAL_KEY_I;
    window->keycodes[0x024] = PAL_KEY_J;
    window->keycodes[0x025] = PAL_KEY_K;
    window->keycodes[0x026] = PAL_KEY_L;
    window->keycodes[0x032] = PAL_KEY_M;
    window->keycodes[0x031] = PAL_KEY_N;
    window->keycodes[0x018] = PAL_KEY_O;
    window->keycodes[0x019] = PAL_KEY_P;
    window->keycodes[0x010] = PAL_KEY_Q;
    window->keycodes[0x013] = PAL_KEY_R;
    window->keycodes[0x01F] = PAL_KEY_S;
    window->keycodes[0x014] = PAL_KEY_T;
    window->keycodes[0x016] = PAL_KEY_U;
    window->keycodes[0x02F] = PAL_KEY_V;
    window->keycodes[0x011] = PAL_KEY_W;
    window->keycodes[0x02D] = PAL_KEY_X;
    window->keycodes[0x015] = PAL_KEY_Y;
    window->keycodes[0x02C] = PAL_KEY_Z;

    window->keycodes[0x00B] = PAL_KEY_0;
    window->keycodes[0x002] = PAL_KEY_1;
    window->keycodes[0x003] = PAL_KEY_2;
    window->keycodes[0x004] = PAL_KEY_3;
    window->keycodes[0x005] = PAL_KEY_4;
    window->keycodes[0x006] = PAL_KEY_5;
    window->keycodes[0x007] = PAL_KEY_6;
    window->keycodes[0x008] = PAL_KEY_7;
    window->keycodes[0x009] = PAL_KEY_8;
    window->keycodes[0x00A] = PAL_KEY_9;

    window->keycodes[0x028] = PAL_KEY_APOSTROPHE;
    window->keycodes[0x02B] = PAL_KEY_BACK_SLASH;
    window->keycodes[0x033] = PAL_KEY_COMMA;
    window->keycodes[0x00D] = PAL_KEY_EQUAL;
    window->keycodes[0x029] = PAL_KEY_GRAVE_ACCENT;
    window->keycodes[0x01A] = PAL_KEY_LEFT_BRACKET;
    window->keycodes[0x00C] = PAL_KEY_MINUS;
    window->keycodes[0x034] = PAL_KEY_PERIOD;
    window->keycodes[0x01B] = PAL_KEY_RIGHT_BRACKET;
    window->keycodes[0x027] = PAL_KEY_SEMi_COLON;
    window->keycodes[0x035] = PAL_KEY_SLASH;

    window->keycodes[0x00E] = PAL_KEY_BACK_SPACE;
    window->keycodes[0x153] = PAL_KEY_DELETE;
    window->keycodes[0x14F] = PAL_KEY_END;
    window->keycodes[0x01C] = PAL_KEY_ENTER;
    window->keycodes[0x001] = PAL_KEY_ESCAPE;
    window->keycodes[0x147] = PAL_KEY_HOME;
    window->keycodes[0x152] = PAL_KEY_INSERT;
    window->keycodes[0x15D] = PAL_KEY_MENU;
    window->keycodes[0x151] = PAL_KEY_PAGE_DOWN;
    window->keycodes[0x149] = PAL_KEY_PAGE_UP;
    window->keycodes[0x045] = PAL_KEY_PAUSE;
    window->keycodes[0x039] = PAL_KEY_SPACE;
    window->keycodes[0x00F] = PAL_KEY_TAB;
    window->keycodes[0x03A] = PAL_KEY_CAPS_LOCK;
    window->keycodes[0x145] = PAL_KEY_NUM_LOCK;
    window->keycodes[0x046] = PAL_KEY_SCROLL_LOCK;
    window->keycodes[0x03B] = PAL_KEY_F1;
    window->keycodes[0x03C] = PAL_KEY_F2;
    window->keycodes[0x03D] = PAL_KEY_F3;
    window->keycodes[0x03E] = PAL_KEY_F4;
    window->keycodes[0x03F] = PAL_KEY_F5;
    window->keycodes[0x040] = PAL_KEY_F6;
    window->keycodes[0x041] = PAL_KEY_F7;
    window->keycodes[0x042] = PAL_KEY_F8;
    window->keycodes[0x043] = PAL_KEY_F9;
    window->keycodes[0x044] = PAL_KEY_F10;
    window->keycodes[0x057] = PAL_KEY_F11;
    window->keycodes[0x058] = PAL_KEY_F12;

    window->keycodes[0x038] = PAL_KEY_LEFT_ALT;
    window->keycodes[0x01D] = PAL_KEY_LEFT_CONTROL;
    window->keycodes[0x02A] = PAL_KEY_LEFT_SHIFT;
    window->keycodes[0x15B] = PAL_KEY_LEFT_SUPER;
    window->keycodes[0x138] = PAL_KEY_RIGHT_ALT;
    window->keycodes[0x11D] = PAL_KEY_RIGHT_CONTROL;
    window->keycodes[0x036] = PAL_KEY_RIGHT_SHIFT;
    window->keycodes[0x15C] = PAL_KEY_RIGHT_SUPER;
    window->keycodes[0x150] = PAL_KEY_DOWN;
    window->keycodes[0x14B] = PAL_KEY_LEFT;
    window->keycodes[0x14D] = PAL_KEY_RIGHT;
    window->keycodes[0x148] = PAL_KEY_UP;
}

void PAL_ProcessKey(PAL_Window* window, u32 key, i32 scancode, u32 action)
{
    if (!key >=0 || !key < PAL_KEY_MAX) {
        PAL_ERROR(PAL_INVALID_PARAMETER, "Invalid key");
        return;
    }

    if (!action >=0 || !action < PAL_ACTION_MAX) {
        PAL_ERROR(PAL_INVALID_PARAMETER, "Invalid action");
        return;
    }

    b8 repeated = false;
    if (action == PAL_RELEASE && window->keys[key] == PAL_RELEASE) {
        return;
    }

    if (action == PAL_PRESS && window->keys[key] == PAL_PRESS) {
        repeated = true;
    }

    if (action == PAL_PRESS && window->keys[key] == PAL_RELEASE) {
        window->keys[key] = PAL_PRESS;
    }
    else {
        window->keys[key] = action;
    }

    if (repeated) {
        action = PAL_REPEAT;
    }
}

PAL_Window* PAL_CreateWindow(const char* title, u32 width, u32 height, u32 flags)
{
    PAL_CHECK_INIT()

    if (!width || !height) {
        PAL_ERROR(PAL_INVALID_PARAMETER, "The width and height of the window cannot be less or equal to 0");
        return nullptr;
    }

    u32 style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
    u32 ex_style = WS_EX_APPWINDOW;

    if (flags & PAL_WINDOW_MINIMIZEBOX) {
        style |= WS_MINIMIZEBOX;
    }

    if (flags & PAL_WINDOW_RESIZABLE) {
        style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    }

    RECT rect = { 0, 0, 0, 0 };
    rect.right = width;
    rect.bottom = height;
    AdjustWindowRectEx(&rect, style, 0, ex_style);
    wchar_t* wstr = PAL_ToWideString(title);

    PAL_Window* window = new PAL_Window();
    if (!window) {
        PAL_ERROR(PAL_OUT_OF_MEMORY, "Failed to allocate memory for the window");
        return nullptr;
    }

    window->x = CW_USEDEFAULT;
    window->y = CW_USEDEFAULT;
    window->title = title;
    window->width = width;
    window->height = height;
    window->flags = flags;

    window->handle = CreateWindowExW(
        ex_style, 
        s_ClassName, 
        wstr, 
        style, 
        window->x,
        window->y, 
        rect.right - rect.left,
        rect.bottom - rect.top, 
        NULL, 
        NULL, 
        s_Instance,  
        NULL
    );

    if (!window->handle) {
        PAL_ERROR(PAL_PLATFORM_ERROR, "Failed to create win32 window");
        return nullptr;
    }

    i32 show_flag = SW_HIDE;
    if (flags & PAL_WINDOW_MAXIMIZE) { show_flag = SW_SHOWMAXIMIZED; }
    if (flags & PAL_WINDOW_CENTER) { PAL_CenterWindow(window); }

    if (flags & PAL_WINDOW_SHOWN) {
        if (show_flag == SW_HIDE) { show_flag = SW_NORMAL; }
        UpdateWindow(window->handle);
        ShowWindow(window->handle, show_flag); 
    }

    SetPropW(window->handle, s_PropName, window);
    delete[] wstr;

    PAL_MapKeys(window);
    window->focused = true;
    window->shouldClose = false;

    s_Data.windowCount++;
    if (s_Data.windowCount == 1) { s_Data.activeWindow = window; }
    return window;
}

void PAL_DestroyWindow(PAL_Window* window)
{
    WINDOW_ERROR(window,)
    if (s_Data.activeWindow == window) {s_Data.activeWindow = nullptr; }
    s_Data.windowCount--;
    DestroyWindow(window->handle);
    delete window;
    window = nullptr;
}

void PAL_PullEvents()
{
    PAL_CHECK_INIT()
    if (s_Data.windowCount <= 0) {
        PAL_ERROR(PAL_PLATFORM_ERROR, "No window was created. You need a window to call this function");
        return;
    }

    MSG msg;
    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    HWND handle = GetActiveWindow();
    if (handle) {

        PAL_Window* window = (PAL_Window*)GetProp(handle, s_PropName);
        if (window) {
            int i;
            const int keys[4][2] = {
                { VK_LSHIFT, PAL_KEY_LEFT_SHIFT },
                { VK_RSHIFT, PAL_KEY_RIGHT_SHIFT },
                { VK_LWIN, PAL_KEY_LEFT_SUPER },
                { VK_RWIN, PAL_KEY_RIGHT_SUPER }
            };

            for (i = 0; i < 4; i++) {
                const int vk = keys[i][0];
                const int key = keys[i][1];
                const int scancode = window->scancodes[key];

                if ((GetKeyState(vk) & 0x8000)) { continue; }
                if (window->keycodes[key] != PAL_PRESS) { continue; }

                PAL_ProcessKey(window, key, scancode, PAL_RELEASE);
            }
        }
    }
}

void PAL_HideWindow(PAL_Window* window)
{
    WINDOW_ERROR(window,)
    ShowWindow(window->handle, SW_HIDE);
}

void PAL_ShowWindow(PAL_Window* window)
{
    WINDOW_ERROR(window,)
    i32 show_flag = SW_NORMAL;
    window->hidden = false;
    if (window->maximized) {
        show_flag = SW_SHOWMAXIMIZED;
    }
    ShowWindow(window->handle, show_flag);
}

void PAL_MaximizeWindow(PAL_Window* window)
{
    WINDOW_ERROR(window,)
    window->minimized = false;
    window->maximized = true;
    ShowWindow(window->handle, SW_SHOWMAXIMIZED);
}

void PAL_MinimizeWindow(PAL_Window* window)
{
    WINDOW_ERROR(window,)
    window->minimized = true;
    window->maximized = false;
    ShowWindow(window->handle, SW_SHOWMINIMIZED);
}

void PAL_MakeWindowActive(PAL_Window* window)
{
    WINDOW_ERROR(window,)
    s_Data.activeWindow = window;
}

void PAL_SetWindowTitle(PAL_Window* window, const char* title)
{
    WINDOW_ERROR(window,)

    window->title = title;
    wchar_t* wstr = PAL_ToWideString(title);
    SetWindowText(window->handle, wstr);
    delete[] wstr;
}

void PAL_SetWindowPos(PAL_Window* window, i32 x, i32 y, b8 center)
{
    WINDOW_ERROR(window,)
    if (center) {
        PAL_CenterWindow(window);
    }
    else {
        RECT rect = { x, y, x, y };
        AdjustWindowRectEx(&rect, window->flags, 0, WS_EX_APPWINDOW);
        window->x = x;
        window->y = y;

        SetWindowPos(
            window->handle,
            NULL, 
            rect.left, 
            rect.top, 
            0, 
            0, 
            SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE
        );
    }
}

void PAL_SetWindowSize(PAL_Window* window, u32 width, u32 height)
{
    WINDOW_ERROR(window,)
    if (!width && !height) {
        PAL_ERROR(PAL_INVALID_PARAMETER, "Width and height cannot be 0");
        return;
    }

    RECT rect = { 0, 0, 0, 0 };
    rect.right = width;
    rect.bottom = height;
    AdjustWindowRectEx(&rect, window->flags, 0, WS_EX_APPWINDOW);
    window->width = width;
    window->height = height;

    SetWindowPos(
        window->handle, 
        HWND_TOP, 
        0, 
        0, 
        rect.right - rect.left, 
        rect.bottom - rect.top,
        SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER
    );
}

const char* PAL_GetWindowTitle(PAL_Window* window)
{
    WINDOW_ERROR(window, nullptr)
    return window->title;
}

void PAL_GetWindowSize(PAL_Window* window, u32* width, u32* height)
{
    WINDOW_ERROR(window,)
    if (width) {
        *width = window->width;
    }
    if (height) {
        *height = window->height;
    }
}

void PAL_GetWindowPos(PAL_Window* window, i32* x, i32* y)
{
    WINDOW_ERROR(window,)
    if (x) {
        *x = window->x;
    }
    if (y) {
        *y = window->y;
    }
}

void* PAL_GetWindowHandle(PAL_Window* window)
{
    WINDOW_ERROR(window, nullptr)
    return window->handle;
}

b8 PAL_WindowShouldClose(PAL_Window* window)
{
    WINDOW_ERROR(window, true)
    return window->shouldClose;
}

b8 PAL_WindowIsHidden(PAL_Window* window)
{
    WINDOW_ERROR(window, true)
    return window->hidden;
}

b8 PAL_WindowIsMaximized(PAL_Window* window)
{
    WINDOW_ERROR(window, false)
    return window->maximized;
}

b8 PAL_WindowIsMinimized(PAL_Window* window)
{
    WINDOW_ERROR(window, true)
    return window->minimized;
}

// input.h
b8 PAL_GetKeyState(u32 key)
{
    if (!s_Data.activeWindow) {
        PAL_ERROR(PAL_PLATFORM_ERROR, "No active window set for input");
        return false;
    }

    if (!key >=0 || !key < PAL_KEY_MAX) {
        PAL_ERROR(PAL_INVALID_PARAMETER, "Invalid key");
        return false;
    }

    return s_Data.activeWindow->keys[key] == PAL_PRESS;
}

b8 PAL_GetMouseButtonState(u32 mouse_button)
{
    if (!s_Data.activeWindow) {
        PAL_ERROR(PAL_PLATFORM_ERROR, "No active window set for input");
        return false;
    }

    if (!mouse_button >=0 || !mouse_button < PAL_MOUSE_BUTTON_MAX) {
        PAL_ERROR(PAL_INVALID_PARAMETER, "Invalid mouse button");
        return false;
    }

    return s_Data.activeWindow->mouseButtons[mouse_button] == PAL_PRESS;
}

LRESULT CALLBACK PAL_Win32Proc(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param)
{
    PAL_Window* window = (PAL_Window*)GetPropW(hwnd, s_PropName);
    if (!window) { return DefWindowProcW(hwnd, msg, w_param, l_param); }

    switch (msg) {
        case WM_CLOSE: {
            window->shouldClose = true;
            return 0;
            break;
        }
    }

    return DefWindowProcW(hwnd, msg, w_param, l_param);
}