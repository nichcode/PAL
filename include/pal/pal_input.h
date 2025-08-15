
/**

Copyright (C) 2025 Nicholas Agbo

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

/**
 * @file pal_input.h
 *
 * Header file for input functions, macros, enum and structs
 *
 * @defgroup input
 */

#ifndef _PAL_INPUT_H
#define _PAL_INPUT_H

#include "pal_core.h"

/**
 * @struct PalInputDevice
 * @brief Opaque handle to an input device.
 *
 * @ingroup input
 */
typedef struct PalInputDevice PalInputDevice;

/**
 * @enum PalInputMask
 * @brief bitmask used for input device enumeration.
 * This is a bitmask enum and multiple flags can be OR'ed together using bitwise OR operator (`|`).
 * 
 * @code
 * PalInputMask inputMask = PAL_INPUT_MASK_KEYBOARD | PAL_INPUT_MASK_GAMEPAD.
 * @endcode
 * @note All orientation follow the format `PAL_INPUT_MASK_**` for consistency and API use.
 *
 * @ingroup input
 */
typedef enum PalInputMask {
    PAL_INPUT_MASK_KEYBOARD = PAL_BIT(0),         /** < Enumerate keyboard devices.*/
    PAL_INPUT_MASK_MOUSE = PAL_BIT(1),            /** < Enumerate mouse devices.*/
    PAL_INPUT_MASK_GAMEPAD = PAL_BIT(2),          /** < Enumerate gamepad devices.*/

    /** < Enumerate all input devices.*/
    PAL_INPUT_MASK_ALL = PAL_INPUT_MASK_KEYBOARD | PAL_INPUT_MASK_MOUSE | PAL_INPUT_MASK_GAMEPAD
} PalInputMask;

/**
 * @enum PalInputDeviceType
 * @brief input device types. This is not a bitmask enum.
 *
 * @note All input device types follow the format `PAL_INPUT_DEVICE_**` for consistency and API use.
 *
 * @ingroup input
 */
typedef enum PalInputDeviceType {
    PAL_INPUT_DEVICE_UNKNOWN,              /** < Invalid device.*/
    PAL_INPUT_DEVICE_KEYBOARD,             /** < Keyboard device.*/
    PAL_INPUT_DEVICE_MOUSE,                /** < Mouse device.*/
    PAL_INPUT_DEVICE_DUALSHOCK4,           /** < PS4 device.*/
    PAL_INPUT_DEVICE_DUALSENSE,            /** < Dualsense device.*/
    PAL_INPUT_DEVICE_XBOX_360,             /** < XBOX360 device.*/
    PAL_INPUT_DEVICE_XBOX_ONE,             /** < XBOX One device.*/
    PAL_INPUT_DEVICE_SWITCH_PRO,           /** < Nintento switch pro device.*/
    PAL_INPUT_DEVICE_GENERIC_GAMEPAD       /** < Generic USB gamepad device.*/
} PalInputDeviceType;

/**
 * @enum PalScancode
 * @brief scancodes (layout independent keys) of a keyboard device.
 *
 * @note All scancodes follow the format `PAL_SCANCODE_**` for consistency and API use.
 *
 * @ingroup input
 */
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

/**
 * @enum PalKey
 * @brief keycodes (layout aware keys) of a keyboard device.
 *
 * @note All keycodes follow the format `PAL_KEY_**` for consistency and API use.
 *
 * @ingroup input
 */
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

/**
 * @enum PalMouseButton
 * @brief mouse button codes of a mouse device.
 *
 * @note All mouse buttons follow the format `PAL_MOUSE_BUTTON_**` for consistency and API use.
 *
 * @ingroup input
 */
typedef enum PalMouseButton {
    PAL_MOUSE_BUTTON_UNKNOWN = 0,

    PAL_MOUSE_BUTTON_LEFT,
    PAL_MOUSE_BUTTON_RIGHT,
    PAL_MOUSE_BUTTON_MIDDLE,
    PAL_MOUSE_BUTTON_X1,
    PAL_MOUSE_BUTTON_X2,

    PAL_MOUSE_BUTTON_MAX
} PalMouseButton;

/**
 * @enum PalGamepadButton
 * @brief buttons of a gamepad device.
 *
 * @note All buttons follow the format `PAL_GAMEPAD_BUTTON_**` for consistency and API use.
 *
 * @ingroup input
 */
typedef enum PalGamepadButton {
    PAL_GAMEPAD_BUTTON_UNKNOWN = 0,

    PAL_GAMEPAD_BUTTON_A,                 /** < Same as cross.*/
    PAL_GAMEPAD_BUTTON_B,                 /** < Same as circle.*/
    PAL_GAMEPAD_BUTTON_X,                 /** < Same as square.*/
    PAL_GAMEPAD_BUTTON_Y,                 /** < Same as triangle.*/
    PAL_GAMEPAD_BUTTON_LEFT_BUMPER,       /** < Same as L1.*/
    PAL_GAMEPAD_BUTTON_RIGHT_BUMPER,      /** < Same as R1.*/
    PAL_GAMEPAD_BUTTON_BACK,              /** < Same as select or view.*/
    PAL_GAMEPAD_BUTTON_START,             /** < Same as menu or options.*/
    PAL_GAMEPAD_BUTTON_GUIDE,             /** < Same as xbox or ps home.*/
    PAL_GAMEPAD_BUTTON_LEFT_THUMB,        /** < Same as L3 (press left stick).*/
    PAL_GAMEPAD_BUTTON_RIGHT_THUMB,       /** < Same as R3 (press right stick).*/

    PAL_GAMEPAD_BUTTON_DPAD_UP,           
    PAL_GAMEPAD_BUTTON_DPAD_DOWN,
    PAL_GAMEPAD_BUTTON_DPAD_LEFT,
    PAL_GAMEPAD_BUTTON_DPAD_RIGHT,

    PAL_GAMEPAD_BUTTON_MAX,
} PalGamepadButton;

/**
 * @enum PalGamepadAxis
 * @brief axis of a gamepad device.
 *
 * @note All axis follow the format `PAL_GAMEPAD_AXIS_**` for consistency and API use.
 *
 * @ingroup input
 */
typedef enum PalGamepadAxis {
    PAL_GAMEPAD_AXIS_UNKNOWN = 0,

    PAL_GAMEPAD_AXIS_LEFT_X,              /** < Left stick X.*/
    PAL_GAMEPAD_AXIS_LEFT_Y,              /** < Left stick Y.*/
    PAL_GAMEPAD_AXIS_RIGHT_X,             /** < Right stick X.*/
    PAL_GAMEPAD_AXIS_RIGHT_Y,             /** < Right stick Y.*/

    PAL_GAMEPAD_AXIS_LEFT_TRIGGER,        /** < Same as L2 or LT.*/
    PAL_GAMEPAD_AXIS_RIGHT_TRIGGER,       /** < Same as R2 or RT.*/

    PAL_GAMEPAD_AXIS_MAX,
} PalGamepadAxis;

/**
 * @struct PalInputDeviceInfo
 * @brief Contains information about an input device.
 *
 * @sa palGetInputDeviceInfo()
 *
 * @ingroup input
 */
typedef struct PalInputDeviceInfo {
    char name[64];                  /** < Name o the device.*/
    char path[128];                 /** < Path of the device.*/
    Uint32 vendorID;                /** < Vendor ID of the device.*/
    Uint32 productID;               /** < Product ID of the device.*/
    PalInputDeviceType type;        /** < The device type. see `PalInputDeviceType`.*/
} PalInputDeviceInfo;

/**
 * @struct PalKeyboardState
 * @brief Information about a keyboard device state (keys and scancodes).
 *
 * @sa palGetKeyboardState()
 *
 * @ingroup input
 */
typedef struct PalKeyboardState {
    const bool* scancodes;           /** < Layout independent keys state. Must not be modified by user. see `PalScancode`.*/
    const bool* keys;                /** < Layout aware keys state. Must not be modified by user. see `PalKey`.*/
} PalKeyboardState;

/**
 * @struct PalMouseWheel
 * @brief Information about a mouse device scroll wheel.
 *
 * @sa PalMouseState()
 *
 * @ingroup input
 */
typedef struct PalMouseWheel {
    int x;                       /** < Horizontal scroll*/
    int y;                       /** < Vertical scroll*/
} PalMouseWheel;

/**
 * @struct PalMouseState
 * @brief Information about a mouse device state. (wheel and buttons).
 *
 * @sa palGetMouseState()
 *
 * @ingroup input
 */
typedef struct PalMouseState {
    const PalMouseWheel* wheel;                /** < Mouse wheel information.*/
    const bool* buttons;                       /** < Mouse buttons state.*/
} PalMouseState;

/**
 * @struct PalGamepadState
 * @brief Information about a gamepad device state (buttons and axes)).
 *
 * @sa palGetGamepadState()
 *
 * @ingroup input
 */
typedef struct PalGamepadState {
    const int* axes;                       /** < Normalized axes. (-1 to 1.0 for sticks, 0 to 1 for triggers).*/
    const bool* buttons;                   /** < gamepad buttons state.*/
} PalGamepadState;

/**
 * @brief Initializes the input system.
 * 
 * The input system must be initialized before enumerating connected input devices,
 * registering input devices to recieve input for them and other input related functionality. 
 * This must called before any input related function.
 * 
 * The input system must be shutdown with `palShutdownInput()` when no longer needed.
 * The `eventDriver` field in `info` must be valid to recieve input related events.
 *
 * @param[in] allocator Optional user provided allocator. Can be `nullptr` to use default.
 * @param[in] eventDriver Optional event driver to push events to. If `nullptr` no events will be pushed.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palShutdownInput()
 * @ingroup input
 */
_PAPI PalResult _PCALL palInitInput(
    PalAllocator *allocator,
    PalEventDriver *eventDriver);

/**
 * @brief Shutdown the input system.
 * 
 * The input system must be initialized before this call.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palInitInput()
 * @ingroup input
 */
_PAPI void _PCALL palShutdownInput();

/**
 * @brief Update the input system and all registered input devices.
 * 
 * @note This function must be called from the main thread.
 *
 * @sa palInitInput()
 * @ingroup input
 */
_PAPI void _PCALL palUpdateInput();

/**
 * @brief Returns a list of active and connected input devices.
 *
 * User must `allocate` statically or dynamically and pass the maximum capacity of the
 * allocated array as `count` and pass the array itself as `inputDevices`.
 * The user is responsible for the life time of the array.
 *
 * The `count` should be the number, not the size in bytes. Example:
 * 
 * @code
 * PalInputDevice* inputDevices[2];
 * @endcode
 * 
 * @code
 * Int32 count = 2;
 * @endcode
 *
 * you can set the `inputDevices` to nullptr and PAL will set the count of the connected displays to `count`.
 * If the `count` is zero or less than zero, the function returns `PAL_RESULT_INSUFFICIENT_BUFFER`.
 * If `count` is less than the connected input devices, PAL will write up to `count`.
 *
 * @param[in] mask Bitmask of input devices to enumerate.
 * @param[in] count Capacity of the `inputDevices` array.
 * @param[out] inputDevices User allocated array of PalInputDevice.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note Users must not free the display handles, they are managed by the OS.
 * Users are required to cache this, and call this function again if input devices are added or removed.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palInitInput(), PalInputDeviceInfo
 * @ingroup input
 */
_PAPI PalResult _PCALL palEnumerateInputDevices(
    PalInputMask mask,
    Int32* count,
    PalInputDevice** inputDevices);

/**
 * @brief Get information about an input device.
 *
 * This takes in a PalInputDeviceInfo struct and fills it.
 * Some of the fields are set to defaults if operation is not supported on the OS.
 * example: standard mouse and keyboard might not provide a vendor and product id,
 * so PAL will set it to `0`.
 *
 * @param[in] inputDevice The input device.
 * @param[out] info Pointer to the PalInputDeviceInfo struct to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 * Users are required to cache this.
 *
 * @sa palEnumerateInputDevices(), PalInputDeviceInfo
 * @ingroup input
 */
_PAPI PalResult _PCALL palGetInputDeviceInfo(
    PalInputDevice* inputDevice, 
    PalInputDeviceInfo* info);

/**
 * @brief Register an input device to recieve input.
 *
 * The input devices registered will be updated when `palUpdateInput()` is called.
 * So for maximum performance, 
 * register only input devices you need and unregister them with `palUnregisterInputDevice()` when no longer needed.
 *
 * @param[in] inputDevice The input device to register.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palEnumerateInputDevices(), palInitInput()
 * @sa palUnregisterInputDevice()
 * @ingroup input
 */
_PAPI PalResult _PCALL palRegisterInputDevice(PalInputDevice* inputDevice);

/**
 * @brief Unregister an input device using the input system.
 * 
 * This function stops the given input device from recieving input.
 * The input device must be registered before this call, 
 * or `PAL_RESULT_INPUT_DEVICE_NOT_REGISTERED` will be returned and this function will fail.
 * To recieve input for the input device again, call `palRegisterInputDevice()`.
 * 
 * @param[in] inputDevice The input device to unregister.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palEnumerateInputDevices(), palInitInput()
 * @sa palRegisterInputDevice()
 * @ingroup input
 */
_PAPI PalResult _PCALL palUnregisterInputDevice(PalInputDevice* inputDevice);

// TODO: docs
_PAPI void palGetMousePosition(Int32* x, Int32* y);

// TODO: docs
_PAPI void palGetMouseRelative(Int32* x, Int32* y);

/**
 * @brief Get the state of a registered keyboard device.
 * 
 * This is not per-keyboard device state, 
 * therefore the returned `state` should be used for all registered keyboards.
 * Since the OS (platform) merges all keyboards into a virtual keyboard,
 * the `state` returned will be a virtual keyboard (one state for all keyoard types).
 *  
 * A keyboard device must be registered before this call.
 * The difference between multiple keyboard devices will be the keys on the keybard.
 * Example: some keyboards might not have a numpad, therefore that key cannot be pressed on that keybard.
 * 
 * If a keyboard device is registered, the `state` will be updated at every call to `palUpdateInput()`.
 *
 * @param[out] state Pointer to the PalKeyboardState struct to fill.
 * 
 * @note This function is guaranteed not to fail if `state` is is valid.
 * @note This function is thread-safe if `state` is thread local.
 * If not, the user is responsible for synchronization.
 *
 * @sa palRegisterInputDevice(), PalKeyboardState
 * @ingroup input
 */
_PAPI void _PCALL palGetKeyboardState(
    PalInputDevice* keyboard, 
    PalKeyboardState* state);

/**
 * @brief Get the state of a registered mouse device.
 * 
 * This is not per-mouse device state, 
 * therefore the returned `state` should be used for all registered mice.
 * Since the OS (platform) merges all mice into a virtual mouse,
 * the `state` returned will be a virtual mouse (one state for all mouse types).
 *  
 * A mouse device must be registered before this call.
 * If a mouse device is registered, the `state` will be updated at every call to `palUpdateInput()`.
 *
 * @param[out] state Pointer to the PalKeyboardState struct to fill.
 * 
 * @note This function is guaranteed not to fail if `state` is is valid.
 * @note This function is thread-safe if `state` is thread local.
 * If not, the user is responsible for synchronization.
 *
 * @sa palRegisterInputDevice(), PalMouseState
 * @ingroup input
 */
_PAPI void _PCALL palGetMouseState(
    PalInputDevice* mouse,
    PalMouseState* state);

#endif // _PAL_INPUT_H