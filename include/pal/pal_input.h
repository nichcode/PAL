
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
    PAL_SCANCODE_KP_PLUS,
    PAL_SCANCODE_KP_MINUS,
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
    PAL_SCANCODE_MINUS,
    PAL_SCANCODE_PERIOD,
    PAL_SCANCODE_SEMICOLON,
    PAL_SCANCODE_SLASH,
    PAL_SCANCODE_LBRACKET,
    PAL_SCANCODE_RBRACKET,
    PAL_SCANCODE_LSUPER,
    PAL_SCANCODE_RSUPER,

    PAL_SCANCODE_MAX
} PalScancode;

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

#endif // _PAL_INPUT_H