
#include "PAL_pch.h"
#include "PAL/PAL_window.h"
#include "PAL/PAL_platform.h"

struct Callbacks
{
    PAL_WindowCloseFunc close;
    PAL_WindowKeyFunc key;
    PAL_WindowButtonFunc button;
    PAL_WindowMouseMovedFunc mouseMoved;
    PAL_WindowMouseScrolledFunc mouseScrolled;
    PAL_WindowResizeFunc resized;
    PAL_WindowMovedFunc moved;
    PAL_WindowFocusedFunc focused;
};

static Callbacks s_Callbacks;

void _CenterWindow(PAL_Window* window)
{
    MONITORINFO monitor_info;
    monitor_info.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(MonitorFromWindow((HWND)window->handle, MONITOR_DEFAULTTONEAREST), &monitor_info);
    u32 max_hwidth = monitor_info.rcMonitor.right;
    u32 max_hheight = monitor_info.rcMonitor.bottom;

    i32 x = (max_hwidth - window->width) / 2;
    i32 y = (max_hheight - window->height) / 2;
    window->x = x;
    window->y = y;
    SetWindowPos((HWND)window->handle, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void _MapKeys(PAL_Window* window)
{
    window->keycodes[0x01E] = PAL_Keys_A;
    window->keycodes[0x030] = PAL_Keys_B;
    window->keycodes[0x02E] = PAL_Keys_C;
    window->keycodes[0x020] = PAL_Keys_D;
    window->keycodes[0x012] = PAL_Keys_E;
    window->keycodes[0x021] = PAL_Keys_F;
    window->keycodes[0x022] = PAL_Keys_G;
    window->keycodes[0x023] = PAL_Keys_H;
    window->keycodes[0x017] = PAL_Keys_I;
    window->keycodes[0x024] = PAL_Keys_J;
    window->keycodes[0x025] = PAL_Keys_K;
    window->keycodes[0x026] = PAL_Keys_L;
    window->keycodes[0x032] = PAL_Keys_M;
    window->keycodes[0x031] = PAL_Keys_N;
    window->keycodes[0x018] = PAL_Keys_O;
    window->keycodes[0x019] = PAL_Keys_P;
    window->keycodes[0x010] = PAL_Keys_Q;
    window->keycodes[0x013] = PAL_Keys_R;
    window->keycodes[0x01F] = PAL_Keys_S;
    window->keycodes[0x014] = PAL_Keys_T;
    window->keycodes[0x016] = PAL_Keys_U;
    window->keycodes[0x02F] = PAL_Keys_V;
    window->keycodes[0x011] = PAL_Keys_W;
    window->keycodes[0x02D] = PAL_Keys_X;
    window->keycodes[0x015] = PAL_Keys_Y;
    window->keycodes[0x02C] = PAL_Keys_Z;

    window->keycodes[0x00B] = PAL_Keys_0;
    window->keycodes[0x002] = PAL_Keys_1;
    window->keycodes[0x003] = PAL_Keys_2;
    window->keycodes[0x004] = PAL_Keys_3;
    window->keycodes[0x005] = PAL_Keys_4;
    window->keycodes[0x006] = PAL_Keys_5;
    window->keycodes[0x007] = PAL_Keys_6;
    window->keycodes[0x008] = PAL_Keys_7;
    window->keycodes[0x009] = PAL_Keys_8;
    window->keycodes[0x00A] = PAL_Keys_9;

    window->keycodes[0x028] = PAL_Keys_Apostrophe;
    window->keycodes[0x02B] = PAL_Keys_Backslash;
    window->keycodes[0x033] = PAL_Keys_Comma;
    window->keycodes[0x00D] = PAL_Keys_Equal;
    window->keycodes[0x029] = PAL_Keys_GraveAccent;
    window->keycodes[0x01A] = PAL_Keys_LeftBracket;
    window->keycodes[0x00C] = PAL_Keys_Minus;
    window->keycodes[0x034] = PAL_Keys_Period;
    window->keycodes[0x01B] = PAL_Keys_RightBracket;
    window->keycodes[0x027] = PAL_Keys_SemiColon;
    window->keycodes[0x035] = PAL_Keys_Slash;

    window->keycodes[0x00E] = PAL_Keys_Backspace;
    window->keycodes[0x153] = PAL_Keys_Delete;
    window->keycodes[0x14F] = PAL_Keys_End;
    window->keycodes[0x01C] = PAL_Keys_Enter;
    window->keycodes[0x001] = PAL_Keys_Escape;
    window->keycodes[0x147] = PAL_Keys_Home;
    window->keycodes[0x152] = PAL_Keys_Insert;
    window->keycodes[0x15D] = PAL_Keys_Menu;
    window->keycodes[0x151] = PAL_Keys_PageDown;
    window->keycodes[0x149] = PAL_Keys_PageUp;
    window->keycodes[0x045] = PAL_Keys_Pause;
    window->keycodes[0x039] = PAL_Keys_Space;
    window->keycodes[0x00F] = PAL_Keys_Tab;
    window->keycodes[0x03A] = PAL_Keys_CapsLock;
    window->keycodes[0x145] = PAL_Keys_NumLock;
    window->keycodes[0x046] = PAL_Keys_ScrollLock;
    window->keycodes[0x03B] = PAL_Keys_F1;
    window->keycodes[0x03C] = PAL_Keys_F2;
    window->keycodes[0x03D] = PAL_Keys_F3;
    window->keycodes[0x03E] = PAL_Keys_F4;
    window->keycodes[0x03F] = PAL_Keys_F5;
    window->keycodes[0x040] = PAL_Keys_F6;
    window->keycodes[0x041] = PAL_Keys_F7;
    window->keycodes[0x042] = PAL_Keys_F8;
    window->keycodes[0x043] = PAL_Keys_F9;
    window->keycodes[0x044] = PAL_Keys_F10;
    window->keycodes[0x057] = PAL_Keys_F11;
    window->keycodes[0x058] = PAL_Keys_F12;

    window->keycodes[0x038] = PAL_Keys_LeftAlt;
    window->keycodes[0x01D] = PAL_Keys_LeftControl;
    window->keycodes[0x02A] = PAL_Keys_LeftShift;
    window->keycodes[0x15B] = PAL_Keys_LeftSuper;
    window->keycodes[0x137] = PAL_Keys_PAL_intScreen;
    window->keycodes[0x138] = PAL_Keys_RightAlt;
    window->keycodes[0x11D] = PAL_Keys_RightControl;
    window->keycodes[0x036] = PAL_Keys_RightShift;
    window->keycodes[0x15C] = PAL_Keys_RightSuper;
    window->keycodes[0x150] = PAL_Keys_Down;
    window->keycodes[0x14B] = PAL_Keys_Left;
    window->keycodes[0x14D] = PAL_Keys_Right;
    window->keycodes[0x148] = PAL_Keys_Up;

    window->keycodes[0x052] = PAL_Keys_P0;
    window->keycodes[0x04F] = PAL_Keys_P1;
    window->keycodes[0x050] = PAL_Keys_P2;
    window->keycodes[0x051] = PAL_Keys_P3;
    window->keycodes[0x04B] = PAL_Keys_P4;
    window->keycodes[0x04C] = PAL_Keys_P5;
    window->keycodes[0x04D] = PAL_Keys_P6;
    window->keycodes[0x047] = PAL_Keys_P7;
    window->keycodes[0x048] = PAL_Keys_P8;
    window->keycodes[0x049] = PAL_Keys_P9;
    window->keycodes[0x04E] = PAL_Keys_PAdd;
    window->keycodes[0x053] = PAL_Keys_PDecimal;
    window->keycodes[0x135] = PAL_Keys_PDivide;
    window->keycodes[0x11C] = PAL_Keys_PEnter;
    window->keycodes[0x059] = PAL_Keys_PEqual;
    window->keycodes[0x037] = PAL_Keys_PMultiply;
    window->keycodes[0x04A] = PAL_Keys_PSubtract;
}

void _ProcessKey(PAL_Window* window, u32 key, i32 scancode, u32 action)
{
    CHECK_ERR(key > 0 || key < PAL_Keys_Max, "Invalid key", return)
    CHECK_ERR(action == PAL_Actions_Release || action == PAL_Actions_Press, "Invalid action", return)
    b8 repeated = false;
    if (action == PAL_Actions_Release && window->keys[key] == PAL_Actions_Release) {
        return;
    }

    if (action == PAL_Actions_Press && window->keys[key] == PAL_Actions_Press) {
        repeated = true;
    }

    if (action == PAL_Actions_Press && window->keys[key] == PAL_Actions_Release) {
        window->keys[key] = PAL_Actions_Press;
    }
    else {
        window->keys[key] = action;
    }

    if (repeated) {
        action = PAL_Actions_Repeat;
    }

    if (s_Callbacks.key) {
        s_Callbacks.key(window, key, scancode, action);
    }
}

void _ProcessButton(PAL_Window* window, u16 button, u8 action)
{
    CHECK_ERR(button > 0 || button < PAL_Buttons_Max, "Invalid button", return)
    CHECK_ERR(action == PAL_Actions_Release || action == PAL_Actions_Press, "Invalid action", return)

    if (action == PAL_Actions_Press && window->buttons[button] == PAL_Actions_Release) {
        window->buttons[button] = PAL_Actions_Press;
    }
    else {
        window->buttons[button] = action;
    }

    if (s_Callbacks.button) {
        s_Callbacks.button(window, button, action);
    }
}

void _ProcessMouseMoved(PAL_Window* window, i32 x, i32 y)
{
    window->mousePos[0] = x;
    window->mousePos[1] = y;

    if (s_Callbacks.mouseMoved) {
        s_Callbacks.mouseMoved(window, x, y);
    }
}

void _ProcessFocus(PAL_Window* window, b8 focused)
{
    window->focused = focused;
    if (s_Callbacks.focused) {
        s_Callbacks.focused(window, focused);
    }

    if (focused == false) {
        // process keys
        for (u32 key = 0; key <= PAL_Keys_Max; key++) {
            if (window->keys[key] == PAL_Actions_Press) {
                const i32 scancode = window->scancodes[key];
                _ProcessKey(window, key, scancode, PAL_Actions_Release);
            }
        }

        // proces mouse buttons
        for (u32 button = 0; button <= PAL_Buttons_Max; button++) {
            if (window->buttons[button] == PAL_Actions_Press) {
                _ProcessButton(window, button, PAL_Actions_Release);
            }
        }
    }
}

PAL_Window* PAL_CreateWindow(const char* title, u32 width, u32 height, u32 flags)
{
    u32 style = WS_OVERLAPPEDWINDOW;
    u32 ex_style = WS_EX_APPWINDOW;

    RECT rect = { 0, 0, 0, 0 };
    rect.right = width;
    rect.bottom = height;
    AdjustWindowRectEx(&rect, style, 0, ex_style);
    wchar_t* wstr = PAL_ToWstring(title);

    PAL_Window* window = new PAL_Window();
    CHECK_ERR(window, "failed to create window", return nullptr)
    window->x = CW_USEDEFAULT;
    window->y = CW_USEDEFAULT;
    window->title = title;
    window->width = width;
    window->height = height;

    HWND handle = CreateWindowExW(
        ex_style, s_ClassName, wstr, style, window->x,
        window->y, rect.right - rect.left, rect.bottom - rect.top, 
        NULL, NULL, s_Instance,  NULL);

    CHECK_ERR(handle, "failed to create window handle", delete window; return nullptr)
    window->handle = handle;

    if (flags & PAL_WindowFlags_Center) {
        _CenterWindow(window);
    }

    if (flags & PAL_WindowFlags_Show) {
        UpdateWindow(handle);
        ShowWindow(handle, SW_NORMAL);
    }
    else {
        ShowWindow(handle, SW_HIDE);
    }

    SetPropW(handle, s_PropName, window);
    PAL_FreeWstring(wstr);
    _MapKeys(window);

    window->focused = true;
    window->context = nullptr;
    return window;
}

void PAL_DestroyWindow(PAL_Window* window)
{
    CHECK_ERR(window, "window is null", return)
    DestroyWindow((HWND)window->handle);
    delete window;
    window = nullptr;
}

void PAL_PullEvents()
{
    MSG msg;
    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    // from GLFW
    HWND handle = GetActiveWindow();
    if (handle) {

        PAL_Window* window = (PAL_Window*)GetProp(handle, s_PropName);
        if (window) {
            int i;
            const int keys[4][2] = {
                { VK_LSHIFT, PAL_Keys_LeftShift },
                { VK_RSHIFT, PAL_Keys_RightShift },
                { VK_LWIN, PAL_Keys_LeftSuper },
                { VK_RWIN, PAL_Keys_RightSuper }
            };

            for (i = 0; i < 4; i++) {
                const int vk = keys[i][0];
                const int key = keys[i][1];
                const int scancode = window->scancodes[key];

                if ((GetKeyState(vk) & 0x8000)) { continue; }
                if (window->keycodes[key] != PAL_Actions_Press) { continue; }

                _ProcessKey(window, key, scancode, PAL_Actions_Release);
            }
        }
    }
}

void PAL_HideWindow(PAL_Window* window)
{
    CHECK_ERR(window, "window is null", return)
    ShowWindow((HWND)window->handle, SW_HIDE);
}

void PAL_ShowWindow(PAL_Window* window)
{
    CHECK_ERR(window, "window is null", return)
    ShowWindow((HWND)window->handle, SW_SHOW);
}

void PAL_SetWindowTitle(PAL_Window* window, const char* title)
{
    CHECK_ERR(window, "window is null", return)
    window->title = title;
    wchar_t* wstr = PAL_ToWstring(title);
    SetWindowText((HWND)window->handle, wstr);
    PAL_FreeWstring(wstr);
}

void PAL_SetWindowSize(PAL_Window* window, u32 width, u32 height)
{
    CHECK_ERR(window, "window is null", return)
    CHECK_ERR(width > 0 && height > 0, "invalid Parameter", return)

    RECT rect = { 0, 0, 0, 0 };
    rect.right = width;
    rect.bottom = height;
    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, 0, WS_EX_OVERLAPPEDWINDOW);
    window->width = width;
    window->height = height;

    SetWindowPos((HWND)window->handle, HWND_TOP, 0, 0, 
        rect.right - rect.left, rect.bottom - rect.top,
        SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER
    );
}

void PAL_SetWindowPos(PAL_Window* window, i32 x, i32 y)
{
    CHECK_ERR(window, "window is null", return)
    RECT rect = { x, y, x, y };
    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, 0, WS_EX_OVERLAPPEDWINDOW);
    window->x = x;
    window->y = y;

    SetWindowPos(
        (HWND)window->handle, NULL, rect.left, rect.top, 
        0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE
    );
}

void PAL_SetWindowCloseCallback(PAL_WindowCloseFunc func)
{
    s_Callbacks.close = func;
}

void PAL_SetWindowKeyCallback(PAL_WindowKeyFunc func)
{
    s_Callbacks.key = func;
}

void PAL_SetWindowButtonCallback(PAL_WindowButtonFunc func)
{
    s_Callbacks.button = func;
}

void PAL_SetWindowMouseMovedCallback(PAL_WindowMouseMovedFunc func)
{
    s_Callbacks.mouseMoved = func;
}

void PAL_SetWindowMouseScrolledCallback(PAL_WindowMouseScrolledFunc func)
{
    s_Callbacks.mouseScrolled = func;
}

void PAL_SetWindowMovedCallback(PAL_WindowMovedFunc func)
{
    s_Callbacks.moved = func;
}

void PAL_SetWindowResizedCallback(PAL_WindowResizeFunc func)
{
    s_Callbacks.resized = func;
}

void PAL_SetWindowFocusedCallback(PAL_WindowFocusedFunc func)
{
    s_Callbacks.focused = func;
}

void PAL_WindowResetCallbacks()
{
    s_Callbacks.close = nullptr;
    s_Callbacks.key = nullptr;
    s_Callbacks.button = nullptr;
    s_Callbacks.mouseMoved = nullptr;
    s_Callbacks.mouseScrolled = nullptr;
    s_Callbacks.resized = nullptr;
    s_Callbacks.focused = nullptr;
}

b8 PAL_WindowShouldClose(PAL_Window* window)
{
    CHECK_ERR(window, "window is null", return false)
    return window->shouldClose;
}

b8 PAL_GetKeyState(PAL_Window* window, u32 key)
{
    CHECK_ERR(window, "window is null", return false)
    CHECK_ERR(key > 0 || key < PAL_Keys_Max, "Invalid key", return false)
    return window->keys[key] == PAL_Actions_Press;
}

b8 PAL_GetButtonState(PAL_Window* window, u32 button)
{
    CHECK_ERR(window, "window is null", return false)
    CHECK_ERR(button > 0 || button < PAL_Buttons_Max, "Invalid button", return false)
    return window->buttons[button] == PAL_Actions_Press;
}

const char* PAL_GetWindowTitle(PAL_Window* window)
{
    CHECK_ERR(window, "window is null", return nullptr)
    return window->title;
}

void* PAL_GetWindowHandle(PAL_Window* window)
{
    CHECK_ERR(window, "window is null", return nullptr)
    return window->handle;
}

u32 PAL_GetWindowWidth(PAL_Window* window)
{
    CHECK_ERR(window, "window is null", return 0)
    return window->width;
}

u32 PAL_GetWindowHeight(PAL_Window* window)
{
    CHECK_ERR(window, "window is null", return 0)
    return window->height;
}

i32 PAL_GetWindowPosX(PAL_Window* window)
{
    CHECK_ERR(window, "window is null", return 0)
    return window->x;
}

i32 PAL_GetWindowPosY(PAL_Window* window)
{
    CHECK_ERR(window, "window is null", return 0)
    return window->y;
}

LRESULT CALLBACK _Win32Proc(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param)
{
    PAL_Window* window = (PAL_Window*)GetPropW(hwnd, s_PropName);
	if (!window) { return DefWindowProcW(hwnd, msg, w_param, l_param); }

    switch (msg) {
        case WM_CLOSE: {
            window->shouldClose = true;
            if (s_Callbacks.close) {s_Callbacks.close(window); }
            return 0;
            break;
        } 

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            u32 key;
            u8 action;
            i32 scancode;

            // from GLFW
            action = (HIWORD(l_param) & KF_UP) ? PAL_Actions_Release : PAL_Actions_Press;
            scancode = (HIWORD(l_param) & (KF_EXTENDED | 0xff));
            if (!scancode) {
                // NOTE: Some synthetic key messages have a scancode of zero
                // HACK: Map the virtual key back to a usable scancode
                scancode = MapVirtualKeyW((UINT)w_param, MAPVK_VK_TO_VSC);
            }

            // HACK: Alt+PrtSc has a different scancode than just PrtSc
            if (scancode == 0x54) { scancode = 0x137; }

            // HACK: Ctrl+Pause has a different scancode than just Pause
            if (scancode == 0x146) { scancode = 0x45; }

            // HACK: CJK IME sets the extended bit for right Shift
            if (scancode == 0x136) { scancode = 0x36; }

            key = window->keycodes[scancode];

            // The Ctrl keys require special handling
            if (w_param == VK_CONTROL) {
                if (HIWORD(l_param) & KF_EXTENDED) {
                    // Right side keys have the extended key bit set
                    key = PAL_Keys_RightControl;
                }
                else {
                    // NOTE: Alt Gr sends Left Ctrl followed by Right Alt
                    // HACK: We only want one event for Alt Gr, so if we detect
                    //       this sequence we discard this Left Ctrl message now
                    //       and later report Right Alt normally
                    MSG next;
                    const DWORD time = GetMessageTime();

                    if (PeekMessageW(&next, NULL, 0, 0, PM_NOREMOVE)) {
                        if (next.message == WM_KEYDOWN ||
                            next.message == WM_SYSKEYDOWN ||
                            next.message == WM_KEYUP ||
                            next.message == WM_SYSKEYUP) {
                            if (next.wParam == VK_MENU &&
                                (HIWORD(next.lParam) & KF_EXTENDED) &&
                                next.time == time) {
                                // Next message is Right Alt down so discard this
                                break;
                            }
                        }
                    }

                    // This is a regular Left Ctrl message
                    key = PAL_Keys_LeftControl;
                }
            }
            else if (w_param == VK_PROCESSKEY) {
                // IME notifies that keys have been filtered by setting the
                // virtual key-code to VK_PROCESSKEY
                break;
            }

            if (action == PAL_Actions_Release && w_param == VK_SHIFT) {
                // HACK: Release both Shift keys on Shift up event, as when both
                //       are pressed the first release does not emit any event
                // NOTE: The other half of this is in _glfwPollEventsWin32
                _ProcessKey(window, PAL_Keys_LeftShift, scancode, action);
                _ProcessKey(window, PAL_Keys_RightShift, scancode, action);
            }
            else if (w_param == VK_SNAPSHOT) {
                // HACK: Key down is not reported for the Print Screen key
                _ProcessKey(window, key, scancode, PAL_Actions_Press);
                _ProcessKey(window, key, scancode, PAL_Actions_Release);
            }
            else {
                _ProcessKey(window, key, scancode, action);
            }

            break;
        }

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP: {
            u8 action;
            u16 button;

            if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP) {
                button = PAL_Buttons_Left;
            }

            else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP) {
                button = PAL_Buttons_Right;
            }

            else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP) {
                button = PAL_Buttons_Middle;
            }

            if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN ||
                msg == WM_MBUTTONDOWN || msg == WM_XBUTTONDOWN) {
                action = PAL_Actions_Press;
            }
            else {
                action = PAL_Actions_Release;
            }

            if (msg == WM_LBUTTONDOWN) {
                SetCapture(hwnd);
            }
            else if (msg == WM_LBUTTONUP) {
                ReleaseCapture();
            }

            _ProcessButton(window, button, action);
            if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONUP) { return true; }

            return 0;
            break;
        }

        case WM_MOUSEMOVE: {
            const int x = GET_X_LPARAM(l_param);
            const int y = GET_Y_LPARAM(l_param);

            if (window->focused) { _ProcessMouseMoved(window, x, y); }

            return 0;
            break;
        }

        case WM_MOUSEWHEEL: {
            if (s_Callbacks.mouseScrolled) {
                s_Callbacks.mouseScrolled(window, 0.0, (f32)HIWORD(w_param) / (f32)WHEEL_DELTA);
            }

            return 0;
            break;
        }

        case WM_MOVE: {
            i32 x = GET_X_LPARAM(l_param);
            i32 y = GET_Y_LPARAM(l_param);
            window->x = x;
            window->y = y;

            if (s_Callbacks.moved) {
                s_Callbacks.moved(window, x, y);
            }

            return 0;
            break;
        }

        case WM_SIZE: {
            const u32 width = (u32)LOWORD(l_param);
            const u32 height = (u32)HIWORD(l_param);

            if (width != window->width || height != window->height) {
                window->width = width;
                window->height = height;

                if (s_Callbacks.resized) {
                    s_Callbacks.resized(window, width, height);
                }
            }
            return 0;
            break;
        }

        case WM_SETFOCUS: {
            _ProcessFocus(window, true);
            return 0;
            break;
        }

        case WM_KILLFOCUS: {
            _ProcessFocus(window, false);
            return 0;
            break;
        }
    }

    return DefWindowProcW(hwnd, msg, w_param, l_param);
}