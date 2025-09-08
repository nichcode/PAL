
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
    WNDCLASSEXW wc = {0};
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