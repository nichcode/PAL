
#ifndef _PAL_INPUT_H
#define _PAL_INPUT_H

#include "pal_core.h"

PAL_DECLARE_HANDLE(PalInput);
PAL_DECLARE_HANDLE(PalInputDevice);
PAL_DECLARE_HANDLE(PalEventDriver);

typedef enum PalInputDeviceType {
    PAL_INPUT_DEVICE_UNKNOWN = 0,
    PAL_INPUT_DEVICE_KEYBOARD,
    PAL_INPUT_DEVICE_MOUSE,
    PAL_INPUT_DEVICE_DUALSHOCK4,
    PAL_INPUT_DEVICE_DUALSENSE,
    PAL_INPUT_DEVICE_XBOX_360,
    PAL_INPUT_DEVICE_XBOX_ONE,
    PAL_INPUT_DEVICE_SWITCH_PRO,
    PAL_INPUT_DEVICE_GAMEPAD,
    PAL_INPUT_DEVICE_HID
} PalInputDeviceType;

typedef enum PalScancode {
    PAL_SCANCODE_UNKNOWN = 0,

    // Letters
    
    PAL_SCANCODE_A,
    PAL_SCANCODE_B,
    PAL_SCANCODE_C,
    PAL_SCANCODE_D,
    PAL_SCANCODE_E,
    PAL_SCANCODE_F,
    PAL_SCANCODE_G,
    PAL_SCANCODE_H,
    PAL_SCANCODE_I,
    PAL_SCANCODE_J,
    PAL_SCANCODE_K,
    PAL_SCANCODE_L,
    PAL_SCANCODE_M,
    PAL_SCANCODE_N,
    PAL_SCANCODE_O,
    PAL_SCANCODE_P,
    PAL_SCANCODE_Q,
    PAL_SCANCODE_R,
    PAL_SCANCODE_S,
    PAL_SCANCODE_T,
    PAL_SCANCODE_U,
    PAL_SCANCODE_V,
    PAL_SCANCODE_W,
    PAL_SCANCODE_X,
    PAL_SCANCODE_Y,
    PAL_SCANCODE_Z,

    // Numbers (top row)
    
    PAL_SCANCODE_0,
    PAL_SCANCODE_1,
    PAL_SCANCODE_2,
    PAL_SCANCODE_3,
    PAL_SCANCODE_4,
    PAL_SCANCODE_5,
    PAL_SCANCODE_6,
    PAL_SCANCODE_7,
    PAL_SCANCODE_8,
    PAL_SCANCODE_9,

    // Function
    
    PAL_SCANCODE_F1,
    PAL_SCANCODE_F2,
    PAL_SCANCODE_F3,
    PAL_SCANCODE_F4,
    PAL_SCANCODE_F5,
    PAL_SCANCODE_F6,
    PAL_SCANCODE_F7,
    PAL_SCANCODE_F8,
    PAL_SCANCODE_F9,
    PAL_SCANCODE_F10,
    PAL_SCANCODE_F11,
    PAL_SCANCODE_F12,

    // Control
    
    PAL_SCANCODE_ESCAPE,
    PAL_SCANCODE_ENTER,
    PAL_SCANCODE_TAB,
    PAL_SCANCODE_BACKSPACE,
    PAL_SCANCODE_SPACE,
    PAL_SCANCODE_CAPSLOCK,
    PAL_SCANCODE_NUMLOCK,
    PAL_SCANCODE_SCROLLLOCK,
    PAL_SCANCODE_LSHIFT,
    PAL_SCANCODE_RSHIFT,
    PAL_SCANCODE_LCTRL,
    PAL_SCANCODE_RCTRL,
    PAL_SCANCODE_LALT,
    PAL_SCANCODE_RALT,

    // Arrows
    
    PAL_SCANCODE_LEFT,
    PAL_SCANCODE_RIGHT,
    PAL_SCANCODE_UP,
    PAL_SCANCODE_DOWN,

    // Navigation
    
    PAL_SCANCODE_INSERT,
    PAL_SCANCODE_DELETE,
    PAL_SCANCODE_HOME,
    PAL_SCANCODE_END,
    PAL_SCANCODE_PAGEUP,
    PAL_SCANCODE_PAGEDOWN,

    // Keypad
    
    PAL_SCANCODE_KP_0,
    PAL_SCANCODE_KP_1,
    PAL_SCANCODE_KP_2,
    PAL_SCANCODE_KP_3,
    PAL_SCANCODE_KP_4,
    PAL_SCANCODE_KP_5,
    PAL_SCANCODE_KP_6,
    PAL_SCANCODE_KP_7,
    PAL_SCANCODE_KP_8,
    PAL_SCANCODE_KP_9,
    PAL_SCANCODE_KP_ENTER,
    PAL_SCANCODE_KP_ADD,
    PAL_SCANCODE_KP_SUBTRACT,
    PAL_SCANCODE_KP_MULTIPLY,
    PAL_SCANCODE_KP_DIVIDE,
    PAL_SCANCODE_KP_DECIMAL,
    PAL_SCANCODE_KP_EQUAL,

    // Misc

    PAL_SCANCODE_PRINTSCREEN,
    PAL_SCANCODE_PAUSE,
    PAL_SCANCODE_MENU,
    PAL_SCANCODE_APOSTROPHE,
    PAL_SCANCODE_BACKSLASH,
    PAL_SCANCODE_COMMA,
    PAL_SCANCODE_EQUAL,
    PAL_SCANCODE_GRAVEACCENT,
    PAL_SCANCODE_SUBTRACT,
    PAL_SCANCODE_PERIOD,
    PAL_SCANCODE_SEMICOLON,
    PAL_SCANCODE_SLASH,
    PAL_SCANCODE_LBRACKET,
    PAL_SCANCODE_RBRACKET,
    PAL_SCANCODE_LSUPER,
    PAL_SCANCODE_RSUPER,

    PAL_SCANCODE_MAX
} PalScancode;

typedef enum PalKey {
    PAL_KEY_UNKNOWN = 0,

    // Letters
    
    PAL_KEY_A,
    PAL_KEY_B,
    PAL_KEY_C,
    PAL_KEY_D,
    PAL_KEY_E,
    PAL_KEY_F,
    PAL_KEY_G,
    PAL_KEY_H,
    PAL_KEY_I,
    PAL_KEY_J,
    PAL_KEY_K,
    PAL_KEY_L,
    PAL_KEY_M,
    PAL_KEY_N,
    PAL_KEY_O,
    PAL_KEY_P,
    PAL_KEY_Q,
    PAL_KEY_R,
    PAL_KEY_S,
    PAL_KEY_T,
    PAL_KEY_U,
    PAL_KEY_V,
    PAL_KEY_W,
    PAL_KEY_X,
    PAL_KEY_Y,
    PAL_KEY_Z,

    // Numbers (top row)
    
    PAL_KEY_0,
    PAL_KEY_1,
    PAL_KEY_2,
    PAL_KEY_3,
    PAL_KEY_4,
    PAL_KEY_5,
    PAL_KEY_6,
    PAL_KEY_7,
    PAL_KEY_8,
    PAL_KEY_9,

    // Function
    
    PAL_KEY_F1,
    PAL_KEY_F2,
    PAL_KEY_F3,
    PAL_KEY_F4,
    PAL_KEY_F5,
    PAL_KEY_F6,
    PAL_KEY_F7,
    PAL_KEY_F8,
    PAL_KEY_F9,
    PAL_KEY_F10,
    PAL_KEY_F11,
    PAL_KEY_F12,

    // Control
    
    PAL_KEY_ESCAPE,
    PAL_KEY_ENTER,
    PAL_KEY_TAB,
    PAL_KEY_BACKSPACE,
    PAL_KEY_SPACE,
    PAL_KEY_CAPSLOCK,
    PAL_KEY_NUMLOCK,
    PAL_KEY_SCROLLLOCK,
    PAL_KEY_LSHIFT,
    PAL_KEY_RSHIFT,
    PAL_KEY_LCTRL,
    PAL_KEY_RCTRL,
    PAL_KEY_LALT,
    PAL_KEY_RALT,

    // Arrows
    
    PAL_KEY_LEFT,
    PAL_KEY_RIGHT,
    PAL_KEY_UP,
    PAL_KEY_DOWN,

    // Navigation
    
    PAL_KEY_INSERT,
    PAL_KEY_DELETE,
    PAL_KEY_HOME,
    PAL_KEY_END,
    PAL_KEY_PAGEUP,
    PAL_KEY_PAGEDOWN,

    // Keypad
    
    PAL_KEY_KP_0,
    PAL_KEY_KP_1,
    PAL_KEY_KP_2,
    PAL_KEY_KP_3,
    PAL_KEY_KP_4,
    PAL_KEY_KP_5,
    PAL_KEY_KP_6,
    PAL_KEY_KP_7,
    PAL_KEY_KP_8,
    PAL_KEY_KP_9,
    PAL_KEY_KP_ENTER,
    PAL_KEY_KP_ADD,
    PAL_KEY_KP_SUBTRACT,
    PAL_KEY_KP_MULTIPLY,
    PAL_KEY_KP_DIVIDE,
    PAL_KEY_KP_DECIMAL,
    PAL_KEY_KP_EQUAL,

    // Misc

    PAL_KEY_PRINTSCREEN,
    PAL_KEY_PAUSE,
    PAL_KEY_MENU,
    PAL_KEY_APOSTROPHE,
    PAL_KEY_BACKSLASH,
    PAL_KEY_COMMA,
    PAL_KEY_EQUAL,
    PAL_KEY_GRAVEACCENT,
    PAL_KEY_SUBTRACT,
    PAL_KEY_PERIOD,
    PAL_KEY_SEMICOLON,
    PAL_KEY_SLASH,
    PAL_KEY_LBRACKET,
    PAL_KEY_RBRACKET,
    PAL_KEY_LSUPER,
    PAL_KEY_RSUPER,

    PAL_KEY_MAX
} PalKey;

typedef enum PalMouseButton {
    PAL_MOUSE_BUTTON_UNKNOWN = 0,

    PAL_MOUSE_BUTTON_LEFT,
    PAL_MOUSE_BUTTON_RIGHT,
    PAL_MOUSE_BUTTON_MIDDLE,
    PAL_MOUSE_BUTTON_X1,
    PAL_MOUSE_BUTTON_X2,

    PAL_MOUSE_BUTTON_MAX

} PalMouseButton;

typedef struct PalInputDeviceInfo {
    char name[64];
    char path[128];
    Uint32 vendorID;
    Uint32 productID;
    PalInputDeviceType type;
} PalInputDeviceInfo;

typedef struct PalInputConfig {
    PalAllocator* allocator;
    PalEventDriver eventDriver;
} PalInputConfig;

typedef struct PalKeyboardState {
    const bool* scancodes;
    const bool* keys;
} PalKeyboardState;

typedef struct PalMouseState {
    int x;
    int y;
    int dx;
    int dy;
    int wheelDeltaX;
    int wheelDeltaY;
    const bool* buttons;
} PalMouseState;

_PAPI PalResult _PCALL palCreateInput(
    PalInputConfig* config,
    PalInput* outInput);

_PAPI void _PCALL palDestroyInput(
    PalInput input);

_PAPI PalResult _PCALL palUpdateInput(
    PalInput input);

_PAPI PalResult _PCALL palEnumerateInputDevices(
    PalInput input,
    int* count,
    PalInputDevice* inputDevices);

_PAPI PalResult _PCALL palGetInputDeviceInfo(
    PalInputDevice inputDevice, 
    PalInputDeviceInfo* info);

_PAPI PalResult _PCALL palRegisterInputDevice(
    PalInput input,
    PalInputDevice inputDevice);

_PAPI PalResult _PCALL palGetScancodeName(
    PalInput input,
    PalScancode scancode,
    const char** outName);

_PAPI PalResult _PCALL palGetMouseButtonName(
    PalInput input,
    PalMouseButton button,
    const char** outName);

_PAPI PalResult _PCALL palGetKeyboardState(
    PalInput input,
    PalKeyboardState* state);

_PAPI PalResult _PCALL palGetMouseState(
    PalInput input,
    PalMouseState* state);

#endif // _PAL_INPUT_H