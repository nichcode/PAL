
// ==================================================
// Includes
// ==================================================

#include "pal/pal_input.h"

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

// ==================================================
// Typedefs, enums and structs
// ==================================================

#define PAL_INPUT_CLASS L"PALInputClass"

typedef struct {
    bool scancodeState[PAL_SCANCODE_MAX];
    bool keycodeState[PAL_KEYCODE_MAX];
    PalScancode scancodes[512];
    PalKeycode keycodes[256];
} Keyboard;

typedef struct {
    Int32 x;
    Int32 y;
    Int32 dx;
    Int32 dy;
    Int32 WheelX;
    Int32 WheelY;
    bool state[PAL_MOUSE_BUTTON_MAX];
} Mouse;

typedef struct {
    bool initialized;
    bool autoFocusWindow;
    const PalAllocator* allocator;
    PalEventDriver* eventDriver;
    HINSTANCE instance;
    HWND activeWindow;
    HWND hiddenWindow;
} InputWin32;

static BYTE s_RawBuffer[4096] = {};
static Mouse s_Mouse = {};
static Keyboard s_Keyboard = {};
static InputWin32 s_Input = {};

// ==================================================
// Internal API
// ==================================================

static void createKeycodeTable() {

    // Letters
    s_Keyboard.keycodes['A'] = PAL_KEYCODE_A;
    s_Keyboard.keycodes['B'] = PAL_KEYCODE_B;
    s_Keyboard.keycodes['C'] = PAL_KEYCODE_C;
    s_Keyboard.keycodes['D'] = PAL_KEYCODE_D;
    s_Keyboard.keycodes['E'] = PAL_KEYCODE_E;
    s_Keyboard.keycodes['F'] = PAL_KEYCODE_F;
    s_Keyboard.keycodes['G'] = PAL_KEYCODE_G;
    s_Keyboard.keycodes['H'] = PAL_KEYCODE_H;
    s_Keyboard.keycodes['I'] = PAL_KEYCODE_I;
    s_Keyboard.keycodes['J'] = PAL_KEYCODE_J;
    s_Keyboard.keycodes['K'] = PAL_KEYCODE_K;
    s_Keyboard.keycodes['L'] = PAL_KEYCODE_L;
    s_Keyboard.keycodes['M'] = PAL_KEYCODE_M;
    s_Keyboard.keycodes['N'] = PAL_KEYCODE_N;
    s_Keyboard.keycodes['O'] = PAL_KEYCODE_O;
    s_Keyboard.keycodes['P'] = PAL_KEYCODE_P;
    s_Keyboard.keycodes['Q'] = PAL_KEYCODE_Q;
    s_Keyboard.keycodes['R'] = PAL_KEYCODE_R;
    s_Keyboard.keycodes['S'] = PAL_KEYCODE_S;
    s_Keyboard.keycodes['T'] = PAL_KEYCODE_T;
    s_Keyboard.keycodes['U'] = PAL_KEYCODE_U;
    s_Keyboard.keycodes['V'] = PAL_KEYCODE_V;
    s_Keyboard.keycodes['W'] = PAL_KEYCODE_W;
    s_Keyboard.keycodes['X'] = PAL_KEYCODE_X;
    s_Keyboard.keycodes['Y'] = PAL_KEYCODE_Y;
    s_Keyboard.keycodes['Z'] = PAL_KEYCODE_Z;

    // Numbers (top row)
    s_Keyboard.keycodes['0'] = PAL_KEYCODE_0;
    s_Keyboard.keycodes['1'] = PAL_KEYCODE_1;
    s_Keyboard.keycodes['2'] = PAL_KEYCODE_2;
    s_Keyboard.keycodes['3'] = PAL_KEYCODE_3;
    s_Keyboard.keycodes['4'] = PAL_KEYCODE_4;
    s_Keyboard.keycodes['5'] = PAL_KEYCODE_5;
    s_Keyboard.keycodes['6'] = PAL_KEYCODE_6;
    s_Keyboard.keycodes['7'] = PAL_KEYCODE_7;
    s_Keyboard.keycodes['8'] = PAL_KEYCODE_8;
    s_Keyboard.keycodes['9'] = PAL_KEYCODE_9;

    // Function
    s_Keyboard.keycodes[VK_F1] = PAL_KEYCODE_F1;
    s_Keyboard.keycodes[VK_F2] = PAL_KEYCODE_F2;
    s_Keyboard.keycodes[VK_F3] = PAL_KEYCODE_F3;
    s_Keyboard.keycodes[VK_F4] = PAL_KEYCODE_F4;
    s_Keyboard.keycodes[VK_F5] = PAL_KEYCODE_F5;
    s_Keyboard.keycodes[VK_F6] = PAL_KEYCODE_F6;
    s_Keyboard.keycodes[VK_F7] = PAL_KEYCODE_F7;
    s_Keyboard.keycodes[VK_F8] = PAL_KEYCODE_F8;
    s_Keyboard.keycodes[VK_F9] = PAL_KEYCODE_F9;
    s_Keyboard.keycodes[VK_F10] = PAL_KEYCODE_F10;
    s_Keyboard.keycodes[VK_F11] = PAL_KEYCODE_F11;
    s_Keyboard.keycodes[VK_F12] = PAL_KEYCODE_F12;

    // Control
    s_Keyboard.keycodes[VK_ESCAPE] = PAL_KEYCODE_ESCAPE;
    s_Keyboard.keycodes[VK_RETURN] = PAL_KEYCODE_ENTER;
    s_Keyboard.keycodes[VK_TAB] = PAL_KEYCODE_TAB;
    s_Keyboard.keycodes[VK_BACK] = PAL_KEYCODE_BACKSPACE;
    s_Keyboard.keycodes[VK_SPACE] = PAL_KEYCODE_SPACE;
    s_Keyboard.keycodes[VK_CAPITAL] = PAL_KEYCODE_CAPSLOCK;
    s_Keyboard.keycodes[VK_NUMLOCK] = PAL_KEYCODE_NUMLOCK;
    s_Keyboard.keycodes[VK_SCROLL] = PAL_KEYCODE_SCROLLLOCK;
    s_Keyboard.keycodes[VK_SHIFT] = PAL_KEYCODE_LSHIFT;  // left shift
    s_Keyboard.keycodes[VK_RSHIFT] = PAL_KEYCODE_RSHIFT;
    s_Keyboard.keycodes[VK_CONTROL] = PAL_KEYCODE_LCTRL; // left control
    s_Keyboard.keycodes[VK_RCONTROL] = PAL_KEYCODE_RCTRL;
    s_Keyboard.keycodes[VK_MENU] = PAL_KEYCODE_LALT; // left Alt
    s_Keyboard.keycodes[VK_RMENU] = PAL_KEYCODE_RALT;

    // Arrows
    s_Keyboard.keycodes[VK_LEFT] = PAL_KEYCODE_LEFT;
    s_Keyboard.keycodes[VK_RIGHT] = PAL_KEYCODE_RIGHT;
    s_Keyboard.keycodes[VK_UP] = PAL_KEYCODE_UP;
    s_Keyboard.keycodes[VK_DOWN] = PAL_KEYCODE_DOWN;

    // Navigation
    s_Keyboard.keycodes[VK_INSERT] = PAL_KEYCODE_INSERT;
    s_Keyboard.keycodes[VK_DELETE] = PAL_KEYCODE_DELETE;
    s_Keyboard.keycodes[VK_HOME] = PAL_KEYCODE_HOME;
    s_Keyboard.keycodes[VK_END] = PAL_KEYCODE_END;
    s_Keyboard.keycodes[VK_PRIOR] = PAL_KEYCODE_PAGEUP;
    s_Keyboard.keycodes[VK_NEXT] = PAL_KEYCODE_PAGEDOWN;

    // Keypad
    s_Keyboard.keycodes[VK_NUMPAD0] = PAL_KEYCODE_KP_0;
    s_Keyboard.keycodes[VK_NUMPAD1] = PAL_KEYCODE_KP_1;
    s_Keyboard.keycodes[VK_NUMPAD2] = PAL_KEYCODE_KP_2;
    s_Keyboard.keycodes[VK_NUMPAD3] = PAL_KEYCODE_KP_3;
    s_Keyboard.keycodes[VK_NUMPAD4] = PAL_KEYCODE_KP_4;
    s_Keyboard.keycodes[VK_NUMPAD5] = PAL_KEYCODE_KP_5;
    s_Keyboard.keycodes[VK_NUMPAD6] = PAL_KEYCODE_KP_6;
    s_Keyboard.keycodes[VK_NUMPAD7] = PAL_KEYCODE_KP_7;
    s_Keyboard.keycodes[VK_NUMPAD8] = PAL_KEYCODE_KP_8;
    s_Keyboard.keycodes[VK_NUMPAD9] = PAL_KEYCODE_KP_9;
    
    s_Keyboard.keycodes[VK_ADD] = PAL_KEYCODE_KP_ADD;
    s_Keyboard.keycodes[VK_SUBTRACT] = PAL_KEYCODE_KP_SUBTRACT;
    s_Keyboard.keycodes[VK_MULTIPLY] = PAL_KEYCODE_KP_MULTIPLY;
    s_Keyboard.keycodes[VK_DIVIDE] = PAL_KEYCODE_KP_DIVIDE;
    s_Keyboard.keycodes[VK_DECIMAL] = PAL_KEYCODE_KP_DECIMAL;

    // Misc
    s_Keyboard.keycodes[VK_SNAPSHOT] = PAL_KEYCODE_PRINTSCREEN;
    s_Keyboard.keycodes[VK_PAUSE] = PAL_KEYCODE_PAUSE;    
    s_Keyboard.keycodes[VK_APPS] = PAL_KEYCODE_MENU;
    s_Keyboard.keycodes[VK_OEM_7] = PAL_KEYCODE_APOSTROPHE;
    s_Keyboard.keycodes[VK_OEM_5] = PAL_KEYCODE_BACKSLASH;
    s_Keyboard.keycodes[VK_OEM_COMMA] = PAL_KEYCODE_COMMA;
    s_Keyboard.keycodes[VK_OEM_PLUS] = PAL_KEYCODE_EQUAL;
    s_Keyboard.keycodes[VK_OEM_3] = PAL_KEYCODE_GRAVEACCENT;
    s_Keyboard.keycodes[VK_OEM_MINUS] = PAL_KEYCODE_SUBTRACT;
    s_Keyboard.keycodes[VK_OEM_PERIOD] = PAL_KEYCODE_PERIOD;
    s_Keyboard.keycodes[VK_OEM_1] = PAL_KEYCODE_SEMICOLON;
    s_Keyboard.keycodes[VK_OEM_2] = PAL_KEYCODE_SLASH;
    s_Keyboard.keycodes[VK_OEM_4] = PAL_KEYCODE_LBRACKET;
    s_Keyboard.keycodes[VK_OEM_6] = PAL_KEYCODE_RBRACKET;
    s_Keyboard.keycodes[VK_LWIN] = PAL_KEYCODE_LSUPER;
    s_Keyboard.keycodes[VK_RWIN] = PAL_KEYCODE_RSUPER;
}

static void createScancodeTable() {

    // Scancodes are made from OR'ed (scancode | extended)
    // Letters
    s_Keyboard.scancodes[0x01E] = PAL_SCANCODE_A;
    s_Keyboard.scancodes[0x030] = PAL_SCANCODE_B;
    s_Keyboard.scancodes[0x02E] = PAL_SCANCODE_C;
    s_Keyboard.scancodes[0x020] = PAL_SCANCODE_D;
    s_Keyboard.scancodes[0x012] = PAL_SCANCODE_E;
    s_Keyboard.scancodes[0x021] = PAL_SCANCODE_F;
    s_Keyboard.scancodes[0x022] = PAL_SCANCODE_G;
    s_Keyboard.scancodes[0x023] = PAL_SCANCODE_H;
    s_Keyboard.scancodes[0x017] = PAL_SCANCODE_I;
    s_Keyboard.scancodes[0x024] = PAL_SCANCODE_J;
    s_Keyboard.scancodes[0x025] = PAL_SCANCODE_K;
    s_Keyboard.scancodes[0x026] = PAL_SCANCODE_L;
    s_Keyboard.scancodes[0x032] = PAL_SCANCODE_M;
    s_Keyboard.scancodes[0x031] = PAL_SCANCODE_N;
    s_Keyboard.scancodes[0x018] = PAL_SCANCODE_O;
    s_Keyboard.scancodes[0x019] = PAL_SCANCODE_P;
    s_Keyboard.scancodes[0x010] = PAL_SCANCODE_Q;
    s_Keyboard.scancodes[0x013] = PAL_SCANCODE_R;
    s_Keyboard.scancodes[0x01F] = PAL_SCANCODE_S;
    s_Keyboard.scancodes[0x014] = PAL_SCANCODE_T;
    s_Keyboard.scancodes[0x016] = PAL_SCANCODE_U;
    s_Keyboard.scancodes[0x02F] = PAL_SCANCODE_V;
    s_Keyboard.scancodes[0x011] = PAL_SCANCODE_W;
    s_Keyboard.scancodes[0x02D] = PAL_SCANCODE_X;
    s_Keyboard.scancodes[0x015] = PAL_SCANCODE_Y;
    s_Keyboard.scancodes[0x02C] = PAL_SCANCODE_Z;

    // Numbers (top row)
    s_Keyboard.scancodes[0x00B] = PAL_SCANCODE_0;
    s_Keyboard.scancodes[0x002] = PAL_SCANCODE_1;
    s_Keyboard.scancodes[0x003] = PAL_SCANCODE_2;
    s_Keyboard.scancodes[0x004] = PAL_SCANCODE_3;
    s_Keyboard.scancodes[0x005] = PAL_SCANCODE_4;
    s_Keyboard.scancodes[0x006] = PAL_SCANCODE_5;
    s_Keyboard.scancodes[0x007] = PAL_SCANCODE_6;
    s_Keyboard.scancodes[0x008] = PAL_SCANCODE_7;
    s_Keyboard.scancodes[0x009] = PAL_SCANCODE_8;
    s_Keyboard.scancodes[0x00A] = PAL_SCANCODE_9;

    // Function
    s_Keyboard.scancodes[0x03B] = PAL_SCANCODE_F1;
    s_Keyboard.scancodes[0x03C] = PAL_SCANCODE_F2;
    s_Keyboard.scancodes[0x03D] = PAL_SCANCODE_F3;
    s_Keyboard.scancodes[0x03E] = PAL_SCANCODE_F4;
    s_Keyboard.scancodes[0x03F] = PAL_SCANCODE_F5;
    s_Keyboard.scancodes[0x040] = PAL_SCANCODE_F6;
    s_Keyboard.scancodes[0x041] = PAL_SCANCODE_F7;
    s_Keyboard.scancodes[0x042] = PAL_SCANCODE_F8;
    s_Keyboard.scancodes[0x043] = PAL_SCANCODE_F9;
    s_Keyboard.scancodes[0x044] = PAL_SCANCODE_F10;
    s_Keyboard.scancodes[0x057] = PAL_SCANCODE_F11;
    s_Keyboard.scancodes[0x058] = PAL_SCANCODE_F12;

    // Control
    s_Keyboard.scancodes[0x001] = PAL_SCANCODE_ESCAPE;
    s_Keyboard.scancodes[0x01C] = PAL_SCANCODE_ENTER;
    s_Keyboard.scancodes[0x00F] = PAL_SCANCODE_TAB;
    s_Keyboard.scancodes[0x00E] = PAL_SCANCODE_BACKSPACE;
    s_Keyboard.scancodes[0x039] = PAL_SCANCODE_SPACE;
    s_Keyboard.scancodes[0x03A] = PAL_SCANCODE_CAPSLOCK;
    s_Keyboard.scancodes[0x145] = PAL_SCANCODE_NUMLOCK;
    s_Keyboard.scancodes[0x046] = PAL_SCANCODE_SCROLLLOCK;
    s_Keyboard.scancodes[0x02A] = PAL_SCANCODE_LSHIFT;
    s_Keyboard.scancodes[0x036] = PAL_SCANCODE_RSHIFT;
    s_Keyboard.scancodes[0x01D] = PAL_SCANCODE_LCTRL;
    s_Keyboard.scancodes[0x11D] = PAL_SCANCODE_RCTRL;
    s_Keyboard.scancodes[0x038] = PAL_SCANCODE_LALT;
    s_Keyboard.scancodes[0x138] = PAL_SCANCODE_RALT;

    // Arrows
    s_Keyboard.scancodes[0x14B] = PAL_SCANCODE_LEFT;
    s_Keyboard.scancodes[0x14D] = PAL_SCANCODE_RIGHT;
    s_Keyboard.scancodes[0x148] = PAL_SCANCODE_UP;
    s_Keyboard.scancodes[0x150] = PAL_SCANCODE_DOWN;

    // Navigation
    s_Keyboard.scancodes[0x152] = PAL_SCANCODE_INSERT;
    s_Keyboard.scancodes[0x153] = PAL_SCANCODE_DELETE;
    s_Keyboard.scancodes[0x147] = PAL_SCANCODE_HOME;
    s_Keyboard.scancodes[0x14F] = PAL_SCANCODE_END;
    s_Keyboard.scancodes[0x149] = PAL_SCANCODE_PAGEUP;
    s_Keyboard.scancodes[0x151] = PAL_SCANCODE_PAGEDOWN;

    // Keypad
    s_Keyboard.scancodes[0x052] = PAL_SCANCODE_KP_0;
    s_Keyboard.scancodes[0x04F] = PAL_SCANCODE_KP_1;
    s_Keyboard.scancodes[0x050] = PAL_SCANCODE_KP_2;
    s_Keyboard.scancodes[0x051] = PAL_SCANCODE_KP_3;
    s_Keyboard.scancodes[0x04B] = PAL_SCANCODE_KP_4;
    s_Keyboard.scancodes[0x04C] = PAL_SCANCODE_KP_5;
    s_Keyboard.scancodes[0x04D] = PAL_SCANCODE_KP_6;
    s_Keyboard.scancodes[0x047] = PAL_SCANCODE_KP_7;
    s_Keyboard.scancodes[0x048] = PAL_SCANCODE_KP_8;
    s_Keyboard.scancodes[0x049] = PAL_SCANCODE_KP_9;
    s_Keyboard.scancodes[0x11C] = PAL_SCANCODE_KP_ENTER;
    s_Keyboard.scancodes[0x04E] = PAL_SCANCODE_KP_ADD;
    s_Keyboard.scancodes[0x04A] = PAL_SCANCODE_KP_SUBTRACT;
    s_Keyboard.scancodes[0x037] = PAL_SCANCODE_KP_MULTIPLY;
    s_Keyboard.scancodes[0x135] = PAL_SCANCODE_KP_DIVIDE;
    s_Keyboard.scancodes[0x053] = PAL_SCANCODE_KP_DECIMAL;
    s_Keyboard.scancodes[0x059] = PAL_SCANCODE_KP_EQUAL;

    // Misc
    s_Keyboard.scancodes[0x137] = PAL_SCANCODE_PRINTSCREEN;
    s_Keyboard.scancodes[0x146] = PAL_SCANCODE_PAUSE;    
    s_Keyboard.scancodes[0x045] = PAL_SCANCODE_PAUSE;
    s_Keyboard.scancodes[0x15D] = PAL_SCANCODE_MENU;
    s_Keyboard.scancodes[0x028] = PAL_SCANCODE_APOSTROPHE;
    s_Keyboard.scancodes[0x02B] = PAL_SCANCODE_BACKSLASH;
    s_Keyboard.scancodes[0x033] = PAL_SCANCODE_COMMA;
    s_Keyboard.scancodes[0x00D] = PAL_SCANCODE_EQUAL;
    s_Keyboard.scancodes[0x029] = PAL_SCANCODE_GRAVEACCENT;
    s_Keyboard.scancodes[0x00C] = PAL_SCANCODE_SUBTRACT;
    s_Keyboard.scancodes[0x034] = PAL_SCANCODE_PERIOD;
    s_Keyboard.scancodes[0x027] = PAL_SCANCODE_SEMICOLON;
    s_Keyboard.scancodes[0x035] = PAL_SCANCODE_SLASH;
    s_Keyboard.scancodes[0x01A] = PAL_SCANCODE_LBRACKET;
    s_Keyboard.scancodes[0x01B] = PAL_SCANCODE_RBRACKET;
    s_Keyboard.scancodes[0x15B] = PAL_SCANCODE_LSUPER;
    s_Keyboard.scancodes[0x15C] = PAL_SCANCODE_RSUPER;
}

static void handleKeyboardInput(
    RAWKEYBOARD* keyboard) {
    
    PalScancode scancode = PAL_SCANCODE_UNKNOWN;
    PalKeycode key = PAL_KEYCODE_UNKNOWN;
    bool extended = (keyboard->Flags & RI_KEY_E0) != 0;

    // scancode
    if (!extended && keyboard->MakeCode == 0x045) {
        scancode = PAL_SCANCODE_NUMLOCK;

    } else if (keyboard->MakeCode == 0x2A || keyboard->MakeCode == 0xAA) {
        return;

    } else {
        Uint16 index = keyboard->MakeCode | (extended << 8);
        scancode = s_Keyboard.scancodes[index];
    }

    // keycode
    if (extended && keyboard->VKey == VK_RETURN) {
        key = PAL_KEYCODE_KP_ENTER;

    } else if (extended && keyboard->VKey == VK_OEM_PLUS) {
        key = PAL_KEYCODE_KP_EQUAL;

    } else if (extended && keyboard->VKey == VK_CONTROL) {
        key = PAL_KEYCODE_RCTRL;

    } else if (extended && keyboard->VKey == VK_MENU) {
        key = PAL_KEYCODE_RALT;

    } else if (scancode == PAL_SCANCODE_RSHIFT && keyboard->VKey == VK_SHIFT) {
        key = PAL_KEYCODE_RSHIFT;

    } else {
        key = s_Keyboard.keycodes[keyboard->VKey];
    }

    bool isKeyDown = !(keyboard->Flags & RI_KEY_BREAK);
    bool repeat = s_Keyboard.keycodeState[key];

    if (keyboard->Flags & RI_KEY_BREAK) {
        s_Keyboard.keycodeState[key] = false;
        s_Keyboard.scancodeState[scancode] = false;
        
    } else {
        s_Keyboard.keycodeState[key] = true;
        s_Keyboard.scancodeState[scancode] = true;  
    }
}

static void handleMouseInput( 
    RAWMOUSE* mouse) {

    USHORT flags = mouse->usButtonFlags;
    s_Mouse.dx += mouse->lLastX;
    s_Mouse.dy += mouse->lLastY;
    
    // get button
    PalMouseButton button;
    if (flags & RI_MOUSE_LEFT_BUTTON_DOWN ||
        flags & RI_MOUSE_LEFT_BUTTON_UP) {
        button = PAL_MOUSE_BUTTON_LEFT;

    } else if (flags & RI_MOUSE_RIGHT_BUTTON_DOWN ||
        flags & RI_MOUSE_RIGHT_BUTTON_UP) {
        button = PAL_MOUSE_BUTTON_RIGHT;

    } else if (flags & RI_MOUSE_MIDDLE_BUTTON_DOWN ||
        flags & RI_MOUSE_MIDDLE_BUTTON_UP) {
        button = PAL_MOUSE_BUTTON_MIDDLE;

    } else if (flags & RI_MOUSE_BUTTON_4_DOWN ||
        flags & RI_MOUSE_BUTTON_4_UP) {
        button = PAL_MOUSE_BUTTON_X1;

    } else if (flags & RI_MOUSE_BUTTON_5_DOWN ||
        flags & RI_MOUSE_BUTTON_5_UP) {
        button = PAL_MOUSE_BUTTON_X2;
    }

    if (flags & RI_MOUSE_LEFT_BUTTON_DOWN ||
        flags & RI_MOUSE_RIGHT_BUTTON_DOWN ||
        flags & RI_MOUSE_MIDDLE_BUTTON_DOWN ||
        flags & RI_MOUSE_BUTTON_4_DOWN ||
        flags & RI_MOUSE_BUTTON_5_DOWN) {
        s_Mouse.state[button] = true;

    } else if (flags & RI_MOUSE_LEFT_BUTTON_UP ||
        flags & RI_MOUSE_RIGHT_BUTTON_UP ||
        flags & RI_MOUSE_MIDDLE_BUTTON_UP ||
        flags & RI_MOUSE_BUTTON_4_UP ||
        flags & RI_MOUSE_BUTTON_5_UP) {
        s_Mouse.state[button] = false;
    }
}

LRESULT CALLBACK inputProc(
    HWND hwnd, 
    UINT msg, 
    WPARAM wParam, 
    LPARAM lParam) {
    
    switch (msg) {
        case WM_INPUT: {
            // for consistent API flow, we won't allow getting inputs without setting input window
            if (!s_Input.activeWindow) {
                break;
            }

            UINT size = sizeof(s_RawBuffer);
            UINT result = GetRawInputData(
                (HRAWINPUT)lParam, 
                RID_INPUT, 
                s_RawBuffer, 
                &size, 
                sizeof(RAWINPUTHEADER)
            );

            if (result == (UINT) -1) {
                break;
            }

            RAWINPUT* raw = (RAWINPUT*)s_RawBuffer;
            if (raw->header.dwType == RIM_TYPEKEYBOARD) {
                handleKeyboardInput(&raw->data.keyboard);

            } else if (raw->header.dwType == RIM_TYPEMOUSE) {
                handleMouseInput(&raw->data.mouse);
            }

            break;
        }
    }
    
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ==================================================
// Public API
// ==================================================

PalResult PAL_CALL palInitInput(
    const PalAllocator *allocator,
    PalEventDriver* eventDriver,
    bool autoFocusWindow) {

    if (s_Input.initialized) {
        return PAL_RESULT_SUCCESS;
    }

    if (allocator && (allocator->allocate || allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    // get the instance
    s_Input.instance = GetModuleHandleW(nullptr);

    // register class
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = inputProc;
    wc.hInstance = s_Input.instance;
    wc.lpszClassName = PAL_INPUT_CLASS;

    // since we check every input carefully, the only error we can get is access denied
    if (!RegisterClassExW(&wc)) {
        return PAL_RESULT_ACCESS_DENIED;
    }

    // create hidden window
    s_Input.hiddenWindow = CreateWindowExW(
        0,
        PAL_INPUT_CLASS,
        L"InputWindow",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        nullptr,
        nullptr,
        s_Input.instance,
        nullptr
    );

    if (!s_Input.hiddenWindow) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // register raw input
    RAWINPUTDEVICE rid = {};
    rid.dwFlags = RIDEV_INPUTSINK;
    rid.hwndTarget = s_Input.hiddenWindow;
    rid.usUsage = 0x06; 
    rid.usUsagePage = 0x01;

    // keyboards
    if (!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE))) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // mice
    rid.usUsage = 0x02; 
    if (!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE))) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // create mapping table
    createKeycodeTable();
    createScancodeTable();

    s_Input.allocator = allocator;
    s_Input.eventDriver = eventDriver;
    s_Input.initialized = true;
    s_Input.autoFocusWindow = autoFocusWindow;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownInput() {

    if (!s_Input.initialized) {
        return;
    }

    DestroyWindow(s_Input.hiddenWindow);
    UnregisterClassW(PAL_INPUT_CLASS, s_Input.instance);
    s_Input.initialized = false;
}

void PAL_CALL palUpdateInput() {

    if (!s_Input.initialized || !s_Input.activeWindow) {
        return;
    }
    
    MSG msg;
    while (PeekMessageA(&msg, s_Input.hiddenWindow, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

const bool* PAL_CALL palGetKeycodeState() {

    if (!s_Input.initialized) {
        return nullptr;
    }
    return s_Keyboard.keycodeState;
}

const bool* PAL_CALL palGetScancodeState() {

    if (!s_Input.initialized) {
        return nullptr;
    }
    return s_Keyboard.scancodeState;
}

const bool* PAL_CALL palGetMouseState() {

    if (!s_Input.initialized) {
        return nullptr;
    }
    return s_Mouse.state;
}

void PAL_CALL palGetMousePosition(
    Int32* x, 
    Int32* y) {
    
    if (!s_Input.initialized) {
        return;
    }

    if (x) {
        *x = s_Mouse.x;
    }

    if (y) {
        *y = s_Mouse.y;
    }
}

void PAL_CALL palGetMouseDelta(
    Int32* dx, 
    Int32* dy) {
    
    if (!s_Input.initialized) {
        return;
    }

    if (dx) {
        *dx = s_Mouse.dx;
    }

    if (dy) {
        *dy = s_Mouse.dy;
    }
}

void PAL_CALL palGetMouseWheelDelta(
    Int32* dx, 
    Int32* dy) {
    
    if (!s_Input.initialized) {
        return;
    }
    
    if (dx) {
        *dx = s_Mouse.WheelX;
    }

    if (dy) {
        *dy = s_Mouse.WheelY;
    }
}

void* PAL_CALL palGetInputWindow() {

    if (!s_Input.initialized) {
        return nullptr;
    }
    return (void*)s_Input.activeWindow;
}

void PAL_CALL palSetInputWindow( 
    void* windowHandle) {

    s_Input.activeWindow = (HWND)windowHandle;
}

void PAL_CALL palSetAutoFocusWindow( 
    bool enable) {
    
    s_Input.autoFocusWindow = enable;
}