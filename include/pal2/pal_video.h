/**
 * @file pal_video.h
 * @brief This is the header file for PAL Video API.
 *
 * It defines all the types and functions of the video module.
 *
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @defgroup pal_video Video Module
 * @{
 */

#ifndef PAL_VIDEO_H
#define PAL_VIDEO_H

#include "pal_core.h"

#define PAL_MONITOR_NAME_SIZE 32

/**
 * @defgroup video_features Video Features
 * @brief Video features.
 * 
 * @{
 */
#define PAL_VIDEO_FEATURE_HIGH_DPI (1ULL << 0)
#define PAL_VIDEO_FEATURE_MONITOR_SET_ORIENTATION (1ULL << 1)
#define PAL_VIDEO_FEATURE_MONITOR_GET_ORIENTATION (1ULL << 2)
#define PAL_VIDEO_FEATURE_BORDERLESS_WINDOW (1ULL << 3)
#define PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW (1ULL << 4)
#define PAL_VIDEO_FEATURE_TOOL_WINDOW (1ULL << 5)
#define PAL_VIDEO_FEATURE_MONITOR_SET_MODE (1ULL << 6)
#define PAL_VIDEO_FEATURE_MONITOR_GET_MODE (1ULL << 7)
#define PAL_VIDEO_FEATURE_MULTI_MONITORS (1ULL << 8)
#define PAL_VIDEO_FEATURE_WINDOW_SET_SIZE (1ULL << 9)
#define PAL_VIDEO_FEATURE_WINDOW_GET_SIZE (1ULL << 10)
#define PAL_VIDEO_FEATURE_WINDOW_SET_POS (1ULL << 11)
#define PAL_VIDEO_FEATURE_WINDOW_GET_POS (1ULL << 12)
#define PAL_VIDEO_FEATURE_WINDOW_SET_STATE (1ULL << 13)
#define PAL_VIDEO_FEATURE_WINDOW_GET_STATE (1ULL << 14)
#define PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY (1ULL << 15)
#define PAL_VIDEO_FEATURE_WINDOW_GET_VISIBILITY (1ULL << 16)
#define PAL_VIDEO_FEATURE_WINDOW_SET_TITLE (1ULL << 17)
#define PAL_VIDEO_FEATURE_WINDOW_GET_TITLE (1ULL << 18)
#define PAL_VIDEO_FEATURE_NO_MAXIMIZEBOX (1ULL << 19)
#define PAL_VIDEO_FEATURE_NO_MINIMIZEBOX (1ULL << 20)
#define PAL_VIDEO_FEATURE_CLIP_CURSOR (1ULL << 21)
#define PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION (1ULL << 22)
#define PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY (1ULL << 23)
#define PAL_VIDEO_FEATURE_WINDOW_FLASH_INTERVAL (1ULL << 24)
#define PAL_VIDEO_FEATURE_WINDOW_SET_INPUT_FOCUS (1ULL << 25)
#define PAL_VIDEO_FEATURE_WINDOW_GET_INPUT_FOCUS (1ULL << 26)
#define PAL_VIDEO_FEATURE_WINDOW_SET_STYLE (1ULL << 27)
#define PAL_VIDEO_FEATURE_WINDOW_GET_STYLE (1ULL << 28)
#define PAL_VIDEO_FEATURE_CURSOR_SET_POS (1ULL << 29)
#define PAL_VIDEO_FEATURE_CURSOR_GET_POS (1ULL << 30)
#define PAL_VIDEO_FEATURE_WINDOW_SET_ICON (1ULL << 31)
#define PAL_VIDEO_FEATURE_TOPMOST_WINDOW (1ULL << 32)
#define PAL_VIDEO_FEATURE_DECORATED_WINDOW (1ULL << 33)
#define PAL_VIDEO_FEATURE_CURSOR_SET_VISIBILITY (1ULL << 34)
#define PAL_VIDEO_FEATURE_WINDOW_GET_MONITOR (1ULL << 35)
#define PAL_VIDEO_FEATURE_MONITOR_GET_PRIMARY (1ULL << 36)
#define PAL_VIDEO_FEATURE_FOREIGN_WINDOWS (1ULL << 37)
#define PAL_VIDEO_FEATURE_MONITOR_VALIDATE_MODE (1ULL << 38)
#define PAL_VIDEO_FEATURE_WINDOW_SET_CURSOR (1ULL << 39)
/** @} */

/**
 * @defgroup orientations Monitor Orientations
 * @brief Monitor orientations
 * 
 * @{
 */
#define PAL_ORIENTATION_LANDSCAPE 0
#define PAL_ORIENTATION_PORTRAIT 1
#define PAL_ORIENTATION_LANDSCAPE_FLIPPED 2
#define PAL_ORIENTATION_PORTRAIT_FLIPPED 3
#define PAL_ORIENTATION_COUNT 4
/** @} */

/**
 * @defgroup window_styles Window Styles
 * @brief Window Styles.
 * 
 * @{
 */
#define PAL_WINDOW_STYLE_RESIZABLE (1U << 0)
#define PAL_WINDOW_STYLE_TRANSPARENT (1U << 1)
#define PAL_WINDOW_STYLE_TOPMOST (1U << 2)
#define PAL_WINDOW_STYLE_NO_MINIMIZEBOX (1U << 3)
#define PAL_WINDOW_STYLE_NO_MAXIMIZEBOX (1U << 4)
#define PAL_WINDOW_STYLE_TOOL (1U << 5)
#define PAL_WINDOW_STYLE_BORDERLESS (1U << 6)
/** @} */

/**
 * @defgroup window_states Window States
 * @brief Window States
 * 
 * @{
 */
#define PAL_WINDOW_STATE_NORMAL 0
#define PAL_WINDOW_STATE_MAXIMIZED 1
#define PAL_WINDOW_STATE_MINIMIZED 2
#define PAL_WINDOW_STATE_RESTORED 3
#define PAL_WINDOW_STATE_COUNT 4
/** @} */

/**
 * @defgroup flash_flags Flash Flags
 * @brief Flash Flash
 * 
 * @{
 */
#define PAL_FLASH_FLAG_STOP 0
#define PAL_FLASH_FLAG_CAPTION (1U << 0)
#define PAL_FLASH_FLAG_TRAY (1U << 1)
/** @} */

/**
 * @defgroup fbconfig_backend FBConfig Backend
 * @brief FBConfig Backend
 * 
 * @{
 */
#define PAL_FBCONFIG_BACKEND_PAL_OPENGL 0
#define PAL_FBCONFIG_BACKEND_EGL 1
#define PAL_FBCONFIG_BACKEND_GLX 2
#define PAL_FBCONFIG_BACKEND_WGL 3
#define PAL_FBCONFIG_BACKEND_COUNT 4
/** @} */

/**
 * @defgroup scancodes Keyboard Scancodes
 * @brief Keyboard scancodes
 * 
 * @{
 */
#define PAL_SCANCODE_UNKNOWN 0
#define PAL_SCANCODE_A 1
#define PAL_SCANCODE_B 2
#define PAL_SCANCODE_C 3
#define PAL_SCANCODE_D 4
#define PAL_SCANCODE_E 5
#define PAL_SCANCODE_F 6
#define PAL_SCANCODE_G 7
#define PAL_SCANCODE_H 8
#define PAL_SCANCODE_I 9
#define PAL_SCANCODE_J 10
#define PAL_SCANCODE_K 11
#define PAL_SCANCODE_L 12
#define PAL_SCANCODE_M 13
#define PAL_SCANCODE_N 14
#define PAL_SCANCODE_O 15
#define PAL_SCANCODE_P 16
#define PAL_SCANCODE_Q 17
#define PAL_SCANCODE_R 18
#define PAL_SCANCODE_S 19
#define PAL_SCANCODE_T 20
#define PAL_SCANCODE_U 21
#define PAL_SCANCODE_V 22
#define PAL_SCANCODE_W 23
#define PAL_SCANCODE_X 24
#define PAL_SCANCODE_Y 25
#define PAL_SCANCODE_Z 26
#define PAL_SCANCODE_0 27
#define PAL_SCANCODE_1 28
#define PAL_SCANCODE_2 29
#define PAL_SCANCODE_3 30
#define PAL_SCANCODE_4 31
#define PAL_SCANCODE_5 32
#define PAL_SCANCODE_6 33
#define PAL_SCANCODE_7 34
#define PAL_SCANCODE_8 35
#define PAL_SCANCODE_9 36
#define PAL_SCANCODE_F1 37
#define PAL_SCANCODE_F2 38
#define PAL_SCANCODE_F3 39
#define PAL_SCANCODE_F4 40
#define PAL_SCANCODE_F5 41
#define PAL_SCANCODE_F6 42
#define PAL_SCANCODE_F7 43
#define PAL_SCANCODE_F8 44
#define PAL_SCANCODE_F9 45
#define PAL_SCANCODE_F10 46
#define PAL_SCANCODE_F11 47
#define PAL_SCANCODE_F12 48
#define PAL_SCANCODE_ESCAPE 49
#define PAL_SCANCODE_ENTER 50
#define PAL_SCANCODE_TAB 51
#define PAL_SCANCODE_BACKSPACE 52
#define PAL_SCANCODE_SPACE 53
#define PAL_SCANCODE_CAPSLOCK 54
#define PAL_SCANCODE_NUMLOCK 55
#define PAL_SCANCODE_SCROLLLOCK 56
#define PAL_SCANCODE_LSHIFT 57
#define PAL_SCANCODE_RSHIFT 58
#define PAL_SCANCODE_LCTRL 59
#define PAL_SCANCODE_RCTRL 60
#define PAL_SCANCODE_LALT 61
#define PAL_SCANCODE_RALT 62
#define PAL_SCANCODE_LEFT 63
#define PAL_SCANCODE_RIGHT 64
#define PAL_SCANCODE_UP 65
#define PAL_SCANCODE_DOWN 66
#define PAL_SCANCODE_INSERT 67
#define PAL_SCANCODE_DELETE 68
#define PAL_SCANCODE_HOME 69
#define PAL_SCANCODE_END 70
#define PAL_SCANCODE_PAGEUP 71
#define PAL_SCANCODE_PAGEDOWN 72
#define PAL_SCANCODE_KP_0 73
#define PAL_SCANCODE_KP_1 74
#define PAL_SCANCODE_KP_2 75
#define PAL_SCANCODE_KP_3 76
#define PAL_SCANCODE_KP_4 77
#define PAL_SCANCODE_KP_5 78
#define PAL_SCANCODE_KP_6 79
#define PAL_SCANCODE_KP_7 80
#define PAL_SCANCODE_KP_8 81
#define PAL_SCANCODE_KP_9 82
#define PAL_SCANCODE_KP_ENTER 83
#define PAL_SCANCODE_KP_ADD 84
#define PAL_SCANCODE_KP_SUBTRACT 85
#define PAL_SCANCODE_KP_MULTIPLY 86
#define PAL_SCANCODE_KP_DIVIDE 87
#define PAL_SCANCODE_KP_DECIMAL 88
#define PAL_SCANCODE_KP_EQUAL 89
#define PAL_SCANCODE_PRINTSCREEN 90
#define PAL_SCANCODE_PAUSE 91
#define PAL_SCANCODE_MENU 92
#define PAL_SCANCODE_APOSTROPHE 93
#define PAL_SCANCODE_BACKSLASH 94
#define PAL_SCANCODE_COMMA 95
#define PAL_SCANCODE_EQUAL 96
#define PAL_SCANCODE_GRAVEACCENT 97
#define PAL_SCANCODE_SUBTRACT 98
#define PAL_SCANCODE_PERIOD 99
#define PAL_SCANCODE_SEMICOLON 100
#define PAL_SCANCODE_SLASH 101
#define PAL_SCANCODE_LBRACKET 102
#define PAL_SCANCODE_RBRACKET 103
#define PAL_SCANCODE_LSUPER 104
#define PAL_SCANCODE_RSUPER 105
#define PAL_SCANCODE_COUNT 106
/** @} */

/**
 * @defgroup keycodes Keyboard Keycodes
 * @brief Keyboard Keycodes
 * 
 * @{
 */
#define PAL_KEYCODE_UNKNOWN 0
#define PAL_KEYCODE_A 1
#define PAL_KEYCODE_B 2
#define PAL_KEYCODE_C 3
#define PAL_KEYCODE_D 4
#define PAL_KEYCODE_E 5
#define PAL_KEYCODE_F 6
#define PAL_KEYCODE_G 7
#define PAL_KEYCODE_H 8
#define PAL_KEYCODE_I 9
#define PAL_KEYCODE_J 10
#define PAL_KEYCODE_K 11
#define PAL_KEYCODE_L 12
#define PAL_KEYCODE_M 13
#define PAL_KEYCODE_N 14
#define PAL_KEYCODE_O 15
#define PAL_KEYCODE_P 16
#define PAL_KEYCODE_Q 17
#define PAL_KEYCODE_R 18
#define PAL_KEYCODE_S 19
#define PAL_KEYCODE_T 20
#define PAL_KEYCODE_U 21
#define PAL_KEYCODE_V 22
#define PAL_KEYCODE_W 23
#define PAL_KEYCODE_X 24
#define PAL_KEYCODE_Y 25
#define PAL_KEYCODE_Z 26
#define PAL_KEYCODE_0 27
#define PAL_KEYCODE_1 28
#define PAL_KEYCODE_2 29
#define PAL_KEYCODE_3 30
#define PAL_KEYCODE_4 31
#define PAL_KEYCODE_5 32
#define PAL_KEYCODE_6 33
#define PAL_KEYCODE_7 34
#define PAL_KEYCODE_8 35
#define PAL_KEYCODE_9 36
#define PAL_KEYCODE_F1 37
#define PAL_KEYCODE_F2 38
#define PAL_KEYCODE_F3 39
#define PAL_KEYCODE_F4 40
#define PAL_KEYCODE_F5 41
#define PAL_KEYCODE_F6 42
#define PAL_KEYCODE_F7 43
#define PAL_KEYCODE_F8 44
#define PAL_KEYCODE_F9 45
#define PAL_KEYCODE_F10 46
#define PAL_KEYCODE_F11 47
#define PAL_KEYCODE_F12 48
#define PAL_KEYCODE_ESCAPE 49
#define PAL_KEYCODE_ENTER 50
#define PAL_KEYCODE_TAB 51
#define PAL_KEYCODE_BACKSPACE 52
#define PAL_KEYCODE_SPACE 53
#define PAL_KEYCODE_CAPSLOCK 54
#define PAL_KEYCODE_NUMLOCK 55
#define PAL_KEYCODE_SCROLLLOCK 56
#define PAL_KEYCODE_LSHIFT 57
#define PAL_KEYCODE_RSHIFT 58
#define PAL_KEYCODE_LCTRL 59
#define PAL_KEYCODE_RCTRL 60
#define PAL_KEYCODE_LALT 61
#define PAL_KEYCODE_RALT 62
#define PAL_KEYCODE_LEFT 63
#define PAL_KEYCODE_RIGHT 64
#define PAL_KEYCODE_UP 65
#define PAL_KEYCODE_DOWN 66
#define PAL_KEYCODE_INSERT 67
#define PAL_KEYCODE_DELETE 68
#define PAL_KEYCODE_HOME 69
#define PAL_KEYCODE_END 70
#define PAL_KEYCODE_PAGEUP 71
#define PAL_KEYCODE_PAGEDOWN 72
#define PAL_KEYCODE_KP_0 73
#define PAL_KEYCODE_KP_1 74
#define PAL_KEYCODE_KP_2 75
#define PAL_KEYCODE_KP_3 76
#define PAL_KEYCODE_KP_4 77
#define PAL_KEYCODE_KP_5 78
#define PAL_KEYCODE_KP_6 79
#define PAL_KEYCODE_KP_7 80
#define PAL_KEYCODE_KP_8 81
#define PAL_KEYCODE_KP_9 82
#define PAL_KEYCODE_KP_ENTER 83
#define PAL_KEYCODE_KP_ADD 84
#define PAL_KEYCODE_KP_SUBTRACT 85
#define PAL_KEYCODE_KP_MULTIPLY 86
#define PAL_KEYCODE_KP_DIVIDE 87
#define PAL_KEYCODE_KP_DECIMAL 88
#define PAL_KEYCODE_KP_EQUAL 89
#define PAL_KEYCODE_PRINTSCREEN 90
#define PAL_KEYCODE_PAUSE 91
#define PAL_KEYCODE_MENU 92
#define PAL_KEYCODE_APOSTROPHE 93
#define PAL_KEYCODE_BACKSLASH 94
#define PAL_KEYCODE_COMMA 95
#define PAL_KEYCODE_EQUAL 96
#define PAL_KEYCODE_GRAVEACCENT 97
#define PAL_KEYCODE_SUBTRACT 98
#define PAL_KEYCODE_PERIOD 99
#define PAL_KEYCODE_SEMICOLON 100
#define PAL_KEYCODE_SLASH 101
#define PAL_KEYCODE_LBRACKET 102
#define PAL_KEYCODE_RBRACKET 103
#define PAL_KEYCODE_LSUPER 104
#define PAL_KEYCODE_RSUPER 105
#define PAL_KEYCODE_COUNT 106
/** @} */

/**
 * @defgroup mouse_buttons Mouse Buttons
 * @brief Mouse Buttons
 * 
 * @{
 */
#define PAL_MOUSE_BUTTON_UNKNOWN 0
#define PAL_MOUSE_BUTTON_LEFT 1
#define PAL_MOUSE_BUTTON_RIGHT 2
#define PAL_MOUSE_BUTTON_MIDDLE 3
#define PAL_MOUSE_BUTTON_X1 4
#define PAL_MOUSE_BUTTON_X2 5
#define PAL_MOUSE_BUTTON_COUNT 6
/** @} */

/**
 * @defgroup cursor_types Cursor Types
 * @brief Cursor Types
 * 
 * @{
 */
#define PAL_CURSOR_TYPE_ARROW 0
#define PAL_CURSOR_TYPE_HAND 1
#define PAL_CURSOR_TYPE_CROSS 2
#define PAL_CURSOR_TYPE_IBEAM 3
#define PAL_CURSOR_TYPE_WAIT 4
#define PAL_CURSOR_TYPE_COUNT 5
/** @} */

/**
 * @struct PalEventDriver
 * @brief Opaque handle to an event driver.
 *
 * @since Added in version 2.0
 */
typedef struct PalEventDriver PalEventDriver;

/**
 * @struct PalMonitor
 * @brief Opaque handle to a monitor.
 *
 * @since Added in version 2.0
 */
typedef struct PalMonitor PalMonitor;

/**
 * @struct PalWindow
 * @brief Opaque handle to a window.
 *
 * @since Added in version 2.0
 */
typedef struct PalWindow PalWindow;

/**
 * @struct PalIcon
 * @brief Opaque handle to an icon.
 *
 * @since Added in version 2.0
 */
typedef struct PalIcon PalIcon;

/**
 * @struct PalCursor
 * @brief Opaque handle to a cursor.
 *
 * @since Added in version 2.0
 */
typedef struct PalCursor PalCursor;

/**
 * @typedef PalVideoFeatures
 * @brief Video system features.
 * 
 * This is a bitmask of all supported features of the video system.
 * 
 * All values of this type follow the format `PAL_VIDEO_FEATURE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint64_t PalVideoFeatures;

/**
 * @typedef PalOrientation
 * @brief Orientation types for a monitor.
 * 
 * All values of this type follow the format `PAL_ORIENTATION_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalOrientation;

/**
 * @typedef PalWindowStyle
 * @brief Window styles. 
 * 
 * Multiple styles can be OR'ed together using bitwise OR operator (`|`).
 * 
 * All values of this type follow the format `PAL_WINDOW_STYLE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalWindowStyle;

/**
 * @typedef PalWindowState
 * @brief Window states.
 * 
 * All values of this type follow the format `PAL_WINDOW_STATE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalWindowState;

/**
 * @typedef PalFlashFlags
 * @brief Flash flags. 
 * 
 * Multiple flags can be OR'ed together using bitwise OR operator (`|`).
 *
 * `PAL_FLASH_FLAG_STOP` is not a bit and must not be combined with other bits.
 * 
 * All values of this type follow the format `PAL_FLASH_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalFlashFlags;

/**
 * @typedef PalFBConfigBackend
 * @brief The backend of a FBConfig.
 * 
 * All values of this type follow the format `PAL_FBCONFIG_BACKEND_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalFBConfigBackend;

/**
 * @typedef PalScancode
 * @brief scancodes (layout independent keys) of a keyboard.
 * 
 * All values of this type follow the format `PAL_SCANCODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalScancode;

/**
 * @typedef PalKeycode
 * @brief Keycodes (layout aware keys) of a keyboard.
 * 
 * All values of this type follow the format `PAL_KEYCODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalKeycode;

/**
 * @typedef PalMouseButton
 * @brief Buttons of a mouse.
 * 
 * All values of this type follow the format `PAL_MOUSE_BUTTON_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalMouseButton;

/**
 * @typedef PalCursorType
 * @brief System cursor types.
 * 
 * All values of this type follow the format `PAL_CURSOR_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalCursorType;

/**
 * @struct PalMonitorInfo
 * @brief Contains information about a monitor.
 *
 * @since Added in version 2.0
 * 
 * @var PalMonitorInfo::x
 * The x position of the monitor in pixels.
 * 
 * @var PalMonitorInfo::y
 * The y position of the monitor in pixels.
 * 
 * @var PalMonitorInfo::width
 * The width of the monitor in pixels.
 * 
 * @var PalMonitorInfo::height
 * The height of the monitor in pixels.
 * 
 * @var PalMonitorInfo::dpi
 * The DPI of the monitor.
 * 
 * @var PalMonitorInfo::refreshRate
 * The refresh rate of the monitor in Hz.
 * 
 * @var PalMonitorInfo::orientation
 * The orientation of the monitor.
 * (eg. `PAL_ORIENTATION_LANDSCAPE`)
 * 
 * @var PalMonitorInfo::primary
 * `PAL_TRUE` if this is the primary monitor.
 * 
 * @var PalMonitorInfo::name
 * The name of the monitor.
 */
typedef struct PalMonitorInfo
{
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t dpi;
    uint32_t refreshRate;
    PalOrientation orientation;
    PalBool primary;
    char name[PAL_MONITOR_NAME_SIZE];
} PalMonitorInfo;

/**
 * @struct PalMonitorMode
 * @brief Contains information about a monitor display mode.
 *
 * @since Added in version 2.0
 * 
 * @var PalMonitorMode::bpp
 * The bits per pixel of the display mode.
 * 
 * @var PalMonitorMode::refreshRate
 * The refresh rate of the display mode in Hz.
 * 
 * @var PalMonitorMode::width
 * The width of the display mode in pixels.
 * 
 * @var PalMonitorMode::height
 * The height of the display mode in pixels.
 */
typedef struct PalMonitorMode
{
    uint32_t bpp;
    uint32_t refreshRate;
    uint32_t width;
    uint32_t height;
} PalMonitorMode;

/**
 * @struct PalFlashInfo
 * @brief Contains information for flashing a window.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalFlashInfo::flags
 * The flash flags. This is a bitmask.
 * `PAL_FLASH_FLAG_STOP` - Stop a flash operation. @nl
 * `PAL_FLASH_FLAG_CAPTION` - Flash the titlebar of a window. 
 * `PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION` must be supported. @nl
 * `PAL_FLASH_FLAG_CAPTION` - Flash the taskbar icon of a window.
 * `PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY` must be supported.
 * 
 * @var PalFlashInfo::interval
 * The flash interval in milliseconds. Set to `0` for default.
 * `PAL_VIDEO_FEATURE_WINDOW_FLASH_INTERVAL` must be supported @nl
 * if the interval is greater than `0`.
 * 
 * @var PalFlashInfo::count
 * The number of times to flash. Set to `0` to flash until 
 * focused or cancelled.
 */
typedef struct PalFlashInfo
{
    PalFlashFlags flags;
    uint32_t interval;
    uint32_t count;
} PalFlashInfo;

/**
 * @struct PalIconCreateInfo
 * @brief Contains creation parameters of an icon.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalIconCreateInfo::pixels
 * The pixels to use to create the icon. It must be in `RGBA` format.
 * 
 * @var PalIconCreateInfo::width
 * The width of the icon in pixels.
 * 
 * @var PalIconCreateInfo::height
 * The height of the icon in pixels.
 */
typedef struct PalIconCreateInfo
{
    const uint8_t* pixels;
    uint32_t width;
    uint32_t height;
} PalIconCreateInfo;

/**
 * @struct PalCursorCreateInfo
 * @brief Contains creation parameters of a cursor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalCursorCreateInfo::pixels
 * The pixels to use to create the cursor. It must be in `RGBA` format.
 * 
 * @var PalCursorCreateInfo::width
 * The width of the cursor in pixels.
 * 
 * @var PalCursorCreateInfo::height
 * The height of the cursor in pixels.
 * 
 * @var PalCursorCreateInfo::xHotspot
 * The x pixel for detecting clicks.
 * 
 * @var PalCursorCreateInfo::yHotspot
 * The y pixel for detecting clicks.
 */
typedef struct PalCursorCreateInfo
{
    const uint8_t* pixels;
    uint32_t width;
    uint32_t height;
    int32_t xHotspot;
    int32_t yHotspot;
} PalCursorCreateInfo;

/**
 * @struct PalWindowHandleInfo
 * @brief Contains information about a window handle.
 *
 * @since Added in version 2.0
 * 
 * @var PalWindowHandleInfo::nativeInstance
 * The platform (OS) display or instance of the window. Will always be valid.
 * 
 * @var PalWindowHandleInfo::nativeWindow
 * The platform (OS) handle of the window. Will always be valid.
 * 
 * @var PalWindowHandleInfo::nativeHandle1
 * First extra handle of the window. Can be `nullptr` if not available.
 * (eg. `xdgSurface` on Wayland).
 * 
 * @var PalWindowHandleInfo::nativeHandle2
 * Second extra handle of the window. Can be `nullptr` if not available.
 * (eg. `xdgToplevel` on Wayland).
 * 
 * @var PalWindowHandleInfo::nativeHandle3
 * Third extra handle of the window. Can be `nullptr` if not available.
 * (eg. `wl_egl_window` on Wayland).
 */
typedef struct PalWindowHandleInfo
{
    void* nativeInstance;
    void* nativeWindow;
    void* nativeHandle1;
    void* nativeHandle2;
    void* nativeHandle3;
} PalWindowHandleInfo;

/**
 * @struct PalWindowCreateInfo
 * @brief Contains creation parameters of a window.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalWindowCreateInfo::title
 * The title of the window in `UTF-8` encoding.
 * 
 * @var PalWindowCreateInfo::monitor
 * The monitor the window should be created on.
 * Set to `nullptr` to use the primary or first monitor available.
 * 
 * @var PalWindowCreateInfo::appName
 * The window app name in `UTF-8` encoding. If `nullptr`, PAL will be used.
 * 
 * @var PalWindowCreateInfo::instanceName
 * The window instance name in `UTF-8` encoding. 
 * If `nullptr`, `title` will be used.
 * 
 * @var PalWindowCreateInfo::fbConfigBackend
 * The FBConfig backend of the index at `fbConfigIndex`.
 * Will be ignored if the index is `0`.
 * 
 * @var PalWindowCreateInfo::fbConfigIndex
 * The FBConfig index. This is the loop index of the fbconfig in the
 * drivers fbconfigs. `fbConfigBackend` must match the source to get the @nl
 * exact fbconfig. Set to `0` to create the window without any FBConfig.
 * 
 * @var PalWindowCreateInfo::width
 * The width of the window in pixels.
 * 
 * @var PalWindowCreateInfo::height
 * The height of the window in pixels.
 * 
 * @var PalWindowCreateInfo::show
 * `PAL_TRUE` to show the window after its created.
 * 
 * @var PalWindowCreateInfo::style
 * The window style. This is a bitmask.
 * `PAL_WINDOW_STYLE_RESIZABLE` - Create a resizable window. @nl
 * `PAL_WINDOW_STYLE_TOPMOST` - Create a topmost window. 
 * `PAL_VIDEO_FEATURE_TOPMOST_WINDOW` must be supported. @nl
 * `PAL_WINDOW_STYLE_TRANSPARENT` - Create a transparent window. 
 * `PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW` must be supported. @nl
 * `PAL_WINDOW_STYLE_NO_MINIMIZEBOX` - Create a window with no minimize box.
 * `PAL_VIDEO_FEATURE_NO_MINIMIZEBOX` must be supported. @nl
 * `PAL_WINDOW_STYLE_NO_MAXIMIZEBOX` - Create a window with no maximize box.
 * `PAL_VIDEO_FEATURE_NO_MAXIMIZEBOX` must be supported. @nl
 * `PAL_WINDOW_STYLE_TOOL` - Create a tool window. 
 * `PAL_VIDEO_FEATURE_TOOL_WINDOW` must be supported. @nl
 * `PAL_WINDOW_STYLE_BORDERLESS` - Create a borderless window. 
 * `PAL_VIDEO_FEATURE_BORDERLESS_WINDOW` must be supported. @nl
 * 
 * @var PalWindowCreateInfo::state
 * The initial window state. Set to `PAL_WINDOW_STATE_NORMAL` to create
 * the window in windowed mode.
 * 
 * @var PalWindowCreateInfo::center
 * `PAL_TRUE` to center the window after creation. This only works for 
 * windowed mode.
 */
typedef struct PalWindowCreateInfo
{
    const char* title;
    PalMonitor* monitor;
    const char* appName;
    const char* instanceName;
    PalFBConfigBackend fbConfigBackend;
    int32_t fbConfigIndex;
    uint32_t width;
    uint32_t height;
    PalBool show;
    PalWindowStyle style;
    PalWindowState state;
    PalBool center;
} PalWindowCreateInfo;

/**
 * @brief Initializes the video system.
 * 
 * This must be called before any video function.
 *
 * `allocator` and `eventDriver` will not not copied, therefore the pointers
 * must remain valid until the video system is shutdown. The event driver 
 * must be valid to recieve video events.
 *
 * If `preferredInstance` is `nullptr`, the video system creates one and control its lifetime.
 * The provided instance will not be freed by the video system.
 * 
 * The video system must be shutdown with `palShutdownVideo()` when no 
 * longer needed.
 *
 * @param[in] allocator The allocator. Set to `nullptr` to use the
 * thread-safe default.
 * @param[in] eventDriver The event driver. If `nullptr`, the video system
 * will not process events.
 * @param[in] preferredInstance User provided instance 
 * (eg. HINSTANCE, wl_display, etc). Can be `nullptr`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palShutdownVideo
 */
PAL_API PalResult PAL_CALL palInitVideo(
    const PalAllocator* allocator,
    PalEventDriver* eventDriver,
    void* preferredInstance);

/**
 * @brief Shutdowns the video system.
 *
 * If the video system has not been initialized, the function returns silently.
 * All created windows, icons, and cursors must be destroyed before this call.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palInitVideo
 */
PAL_API void PAL_CALL palShutdownVideo();

/**
 * @brief Update the video system and all created windows.
 *
 * If the video system has not been initialized, the function returns silently.
 * This function pushes generated video events to the event driver set at
 * `palInitVideo()`. If there was no event driver, the events will not be
 * processed.
 * 
 * Windows might be responsive but thats implementation-defined behavior.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palInitVideo
 */
PAL_API void PAL_CALL palUpdateVideo();

/**
 * @brief Gets the supported features of the video system.
 *
 * @return video features on success or `0` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palInitVideo
 */
PAL_API PalVideoFeatures PAL_CALL palGetVideoFeatures();

/**
 * @brief Returns a list of all connected monitors.
 *
 * The video system must be initialized before this call.
 * 
 * The monitor handles must not be freed by the user, they are managed by the
 * platform (OS). Users are required to cache this, and call this function again
 * if monitors are added or removed.
 * 
 * Set `monitors` to `nullptr` to get the total number of connected
 * monitors. If the monitor array passed is less than the number of
 * connected monitors, PAL will fill the array upto that limit sequentially.
 *
 * @param[in, out] count The capacity of the monitor array.
 * @param[out] monitors The monitor array.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palGetPrimaryMonitor
 */
PAL_API PalResult PAL_CALL palEnumerateMonitors(
    uint32_t* count,
    PalMonitor** monitors);

/**
 * @brief Gets the primary connected monitor.
 *
 * `PAL_VIDEO_FEATURE_MONITOR_GET_PRIMARY` must be supported
 * otherwise undefined behavior.
 * 
 * This function is not guaranteed to work on all platforms, its
 * recommended to enumerate the monitors and select the first one
 * if the feature is not supported on your platform.
 *
 * The monitor handle must not be freed by the user, it is managed by the
 * platform (OS).
 *
 * @param[out] monitor The output to recieve the primary monitor.
 * Must not be `nullptr`.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palEnumerateMonitors
 */
PAL_API void PAL_CALL palGetPrimaryMonitor(PalMonitor** monitor);

/**
 * @brief Gets information about a monitor.
 *
 * @param[in] monitor The monitor. Must not be `nullptr`.
 * @param[out] info The output struct to recieve the monitor info. 
 * Must not be `nullptr`.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetMonitorInfo(
    PalMonitor* monitor,
    PalMonitorInfo* info);

/**
 * @brief Returns a list of all supported display modes of a monitor.
 * 
 * Set `modes` to `nullptr` to get the total number of supported
 * display modes of the `monitor`. If the modes array passed is less than 
 * the number of display modes, PAL will fill the array upto that limit 
 * sequentially.
 *
 * @param[in] monitor The monitor. `Must not be nullptr`.
 * @param[in, out] count The capacity of the monitor display mode array.
 * @param[out] modes The display mode array.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palEnumerateMonitorModes(
    PalMonitor* monitor,
    uint32_t* count,
    PalMonitorMode* modes);

/**
 * @brief Gets the current display mode of the monitor.
 *
 * `PAL_VIDEO_FEATURE_MONITOR_GET_MODE` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] monitor The monitor. Must not ne `nullptr`.
 * @param[out] mode The output struct to recieve the display mode. 
 * Must not ne `nullptr`.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palSetMonitorMode
 */
PAL_API void PAL_CALL palGetCurrentMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode);

/**
 * @brief Sets the active display mode of the monitor.
 *
 * `PAL_VIDEO_FEATURE_MONITOR_SET_MODE` Must be supported 
 * otherwise undefined behavior.
 * 
 * Validate the display mode with `palValidateMonitorMode()` before switching
 * on supported platforms or use a display mode from
 * `palEnumerateMonitorModes()` to be safe.
 * 
 * If the monitor display mode submitted is invalid, the behavior is platform
 * specific.
 *
 * @param[in] monitor The monitor. Must not ne `nullptr`.
 * @param[in] mode The display mode.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palGetCurrentMonitorMode
 */
PAL_API PalResult PAL_CALL palSetMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode);

/**
 * @brief Checks if a display mode is valid on the monitor.
 *
 * `PAL_VIDEO_FEATURE_MONITOR_VALIDATE_MODE` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] monitor The monitor.
 * @param[in] mode The display mode.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palValidateMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode);

/**
 * @brief Sets the orientation for the monitor.
 *
 * `PAL_VIDEO_FEATURE_MONITOR_SET_ORIENTATION` must be supported
 * otherwise undefined behavior.
 *
 * This change is temporary and is reset when the platform (OS) reboots.
 *
 * @param[in] monitor The monitor.
 * @param[in] orientation The orientation.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palSetMonitorOrientation(
    PalMonitor* monitor,
    PalOrientation orientation);

/**
 * @brief Creates a window.
 * 
 * The window width and height is restricted by the platform. Very small 
 * or large width and height might be overridden by the system or
 * the window will fail to create.
 * 
 * This function creates the window and optionally sets the pixel format
 * or fbconfig if `PalWindowCreateInfo::fbConfigIndex` is not `0`.
 * This does not create context for opengl, vulkan etc. 
 *
 * @param[in] info Information about how to create the window.
 * @param[out] window The output handle to recieve the created window.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @note On Wayland:
 *
 * - Creating non resizable windows is not supported. It will be ignored.
 *
 * - Creating windows on a specific monitor is not supported.
 *
 * - Creating hidden windows is not supported. It will be ignored.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palCreateWindow(
    const PalWindowCreateInfo* info,
    PalWindow** window);

/**
 * @brief Destroys the window.
 *
 * This only destroys windows created by PAL.
 *
 * @param[in] window The window.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palCreateWindow
 */
PAL_API void PAL_CALL palDestroyWindow(PalWindow* window);

/**
 * @brief Minimizes a maximized or restored window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_STATE` must be supported 
 * otherwise undefined behavior.
 * 
 * If the window is already minimized, this functions does nothing.
 *
 * @param[in] window The window. Must not be `nullptr`.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palMaximizeWindow
 * @sa palRestoreWindow
 */
PAL_API void PAL_CALL palMinimizeWindow(PalWindow* window);

/**
 * @brief Maximizes a minimized or restored window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_STATE` must be supported 
 * otherwise undefined behavior.
 * 
 * If the window is already maximized, this functions does nothing.
 *
 * @param[in] window The window. Must not be `nullptr`.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palMinimizeWindow
 * @sa palRestoreWindow
 */
PAL_API void PAL_CALL palMaximizeWindow(PalWindow* window);

/**
 * @brief Restores a window to it previous state.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_STATE` must be supported
 * otherwise undefined behavior.
 * 
 * If the window is already restored, this functions does nothing.
 *
 * @param[in] window The window. Must not be `nullptr`.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @note Wayland does not support restoring a minimized window.
 *
 * @since Added in version 2.0
 * @sa palMinimizeWindow
 * @sa palMaximizeWindow
 */
PAL_API void PAL_CALL palRestoreWindow(PalWindow* window);

/**
 * @brief Shows the window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY` must be supported
 * otherwise undefined behavior.
 * 
 * All windows are created hidden if not explicitly shown.
 * This does nothing if the window is already shown.
 *
 * @param[in] window The window. Must not be `nullptr`.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palHideWindow
 */
PAL_API void PAL_CALL palShowWindow(PalWindow* window);

/**
 * @brief Hides the window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY` must be supported
 * otherwise undefined behavior.
 * 
 * This does nothing if the window is already hidden.
 *
 * @param[in] window The window. Must not be `nullptr`.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palShowWindow
 */
PAL_API void PAL_CALL palHideWindow(PalWindow* window);

/**
 * @brief Requests the platform (OS) to visually flash the window.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] info Information about how to flash the window.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info);

/**
 * @brief Gets the style of the window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_GET_STYLE` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[out] style The output to recieve the window style.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palSetWindowStyle
 */
PAL_API void PAL_CALL palGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* outStyle);

/**
 * @brief Gets the monitor the window is currently on.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_GET_MONITOR` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[out] monitor The output handle to recieve the monitor.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetWindowMonitor(
    PalWindow* window,
    PalMonitor** monitor);

/**
 * @brief Gets the title of the window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_GET_TITLE` must be supported
 * otherwise undefined behavior.
 *
 * Set the buffer to `nullptr` to get the size of the window name in bytes.
 * If the size of the provided buffer is less than the actual size of window
 * title, PAL will write upto that limit.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] bufferSize The size of `buffer` in bytes.
 * @param[out] size The actual size of the window title in bytes.
 * @param[out] buffer The output buffer to write to.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palSetWindowTitle
 */
PAL_API void PAL_CALL palGetWindowTitle(
    PalWindow* window,
    uint64_t bufferSize,
    uint64_t* size,
    char* buffer);

/**
 * @brief Gets the position of the window in pixels.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_GET_POS` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[out] x The output to recieve the window x position in pixels.
 * @param[out] y The output to recieve the window y position in pixels.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palSetWindowPos
 */
PAL_API void PAL_CALL palGetWindowPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y);

/**
 * @brief Gets the size of the window in pixels.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_GET_SIZE` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[out] width The output to recieve the window width in pixels.
 * @param[out] height The output to recieve the window height in pixels.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palSetWindowSize
 */
PAL_API void PAL_CALL palGetWindowSize(
    PalWindow* window,
    uint32_t* width,
    uint32_t* height);

/**
 * @brief Gets the state of the window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_GET_STATE` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[out] state The output to recieve the window state.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetWindowState(
    PalWindow* window,
    PalWindowState* state);

/**
 * @brief Gets the state of the keycodes (layout aware keys) of the
 * keyboard.
 *
 * The returned pointer must not be freed. The state is updated when
 * `palUpdateVideo()` is called. The array must be index with 
 * PalKeycodes values and not exceed `PAL_KEYCODE_COUNT`.
 *
 * @return A pointer to the keycodes array on success or `nullptr` on failure.
 *
 * @Thread-safety Thread-safe.
 *
 * @since Added in version 2.0
 */
PAL_API const PalBool* PAL_CALL palGetKeycodeState();

/**
 * @brief Gets the state of the scancodes (layout independent keys) of
 * the keyboard.
 *
 * The returned pointer must not be freed. The state is updated when
 * `palUpdateVideo()` is called. The array must be index with 
 * PalScancodes values and not exceed `PAL_SCANCODE_COUNT`.
 *
 * @return A pointer to the scancodes array on success or `nullptr` on failure.
 *
 * @Thread-safety Thread-safe.
 *
 * @since Added in version 2.0
 */
PAL_API const PalBool* PAL_CALL palGetScancodeState();

/**
 * @brief Gets the state of the mouse buttons.
 *
 * The returned pointer must not be freed. The state is updated when
 * `palUpdateVideo()` is called. The array must be index with 
 * PalMouseButton values and not exceed `PAL_MOUSE_BUTTON_COUNT`.
 *
 * @return A pointer to the mouse button array on success or `nullptr` on failure.
 *
 * @@Thread-safety Thread-safe.
 *
 * @since Added in version 2.0
 */
PAL_API const PalBool* PAL_CALL palGetMouseState();

/**
 * @brief Gets the relative movement of the mouse in desktop pixels.
 *
 * The relative movement will be updated when `palUpdateVideo()` is called.
 *
 * @param[in] dx The output to recieve the relative x. Can be `nullptr`.
 * @param[in] dy The output to recieve the relative y. Can be `nullptr`.
 *
 * @Thread-safety `dx` and `dy` must be per thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetMouseDelta(
    float* dx,
    float* dy);

/**
 * @brief Gets the wheel delta of the mouse.
 *
 * The wheel delta will be updated when `palUpdateVideo()` is called.
 *
 * @param[in] dx The output to recieve the x wheel delta. Can be `nullptr`.
 * @param[in] dy The output to recieve the y wheel delta. Can be `nullptr`.
 *
 * @Thread-safety dx` and `dy` must be per thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetMouseWheelDelta(
    float* dx,
    float* dy);

/**
 * @brief Checks if the window is visible.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_GET_VISIBILITY` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 *
 * @return `PAL_TRUE` if the window is visible otherwise `PAL_FALSE`.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 */
PAL_API PalBool PAL_CALL palIsWindowVisible(PalWindow* window);

/**
 * @brief Gets the current input-focused window per application.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_GET_INPUT_FOCUS` must be supported
 * otherwise undefined behavior.
 *
 * @return The current input-focused window on success or `nullptr` on
 * failure.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API PalWindow* PAL_CALL palGetFocusWindow();

/**
 * @brief Gets the native handle of the window.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[out] info The output struct to recieve the window handle info.
 *
 * @Thread-safety Thread-safe.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetWindowHandleInfo(
    PalWindow* window,
    PalWindowHandleInfo* info);

/**
 * @brief Sets the opacity of the window.
 *
 * `PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW` must be supported
 * otherwise undefined behavior.
 * 
 * The window must have `PAL_WINDOW_STYLE_TRANSPARENT` style.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] opacity Must be in the range 0.0 - 1.0.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palSetWindowOpacity(
    PalWindow* window,
    float opacity);

/**
 * @brief Sets the style of the window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_STYLE` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] style The style.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palGetWindowStyle
 */
PAL_API void PAL_CALL palSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style);

/**
 * @brief Sets the title of the provided window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_TITLE` must be supported 
 * otherwise undefined behavior.
 * 
 * The title must be a `UTF-8` encoding null terminated string.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] title The title.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palGetWindowTitle
 */
PAL_API void PAL_CALL palSetWindowTitle(
    PalWindow* window,
    const char* title);

/**
 * @brief Sets the position of the window in pixels.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_POS` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] x The new x coordinate in pixels.
 * @param[in] y The new y coordinate in pixels.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palGetWindowPos
 */
PAL_API void PAL_CALL palSetWindowPos(
    PalWindow* window,
    int32_t x,
    int32_t y);

/**
 * @brief Sets the size of the window in pixels.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_SIZE` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] width The new width of the window in pixels. 
 * Must be greater than `0`.
 * @param[in] height The new height of the window in pixels. 
 * Must be greater than `0`.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palGetWindowSize
 */
PAL_API void PAL_CALL palSetWindowSize(
    PalWindow* window,
    uint32_t width,
    uint32_t height);

/**
 * @brief Requests input focus for the window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_INPUT_FOCUS` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palGetFocusWindow
 */
PAL_API void PAL_CALL palSetFocusWindow(PalWindow* window);

/**
 * @brief Creates an icon.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_ICON` must be supported 
 * otherwise undefined behavior.
 *
 * @param[in] info Information about how to create the icon.
 * @param[out] icon The output handle to recieve the created icon.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palDestroyIcon
 */
PAL_API PalResult PAL_CALL palCreateIcon(
    const PalIconCreateInfo* info,
    PalIcon** icon);

/**
 * @brief Destroys the icon.
 *
 * @param[in] icon The icon.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palCreateIcon
 */
PAL_API void PAL_CALL palDestroyIcon(PalIcon* icon);

/**
 * @brief Sets the icon for the window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_ICON` must be supporte
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] icon The icon. Set to `nullptr` to revert to default.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palSetWindowIcon(
    PalWindow* window,
    PalIcon* icon);

/**
 * @brief Creates a cursor.
 *
 * @param[in] info Information about how to create the cursor.
 * @param[out] cursor The output handle to recieve the created cursor.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palDestroyCursor
 */
PAL_API PalResult PAL_CALL palCreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** cursor);

/**
 * @brief Creates a system cursor.
 *
 * @param[in] type The system cursor type.
 * @param[out] cursor The output handle to recieve the created cursor.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palDestroyCursor
 */
PAL_API PalResult PAL_CALL palCreateCursorFrom(
    PalCursorType type,
    PalCursor** cursor);

/**
 * @brief Destroys the cursor.
 *
 * @param[in] cursor The cursor.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palCreateCursor
 */
PAL_API void PAL_CALL palDestroyCursor(PalCursor* cursor);

/**
 * @brief Shows or hide the cursor.
 *
 * `PAL_VIDEO_FEATURE_CURSOR_SET_VISIBILITY` must be supported
 * otherwise undefined behavior.
 *
 * This affects all created cursors since the platform (OS) merges all cursors
 * into a single one on the screen.
 *
 * @param[in] show `PAL_TRUE` to make the cursor visible otherwise `PAL_FALSE`.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palShowCursor(PalBool show);

/**
 * @brief Clips the cursor to the window.
 *
 * `PAL_VIDEO_FEATURE_CLIP_CURSOR` must be supported 
 * otherwise undefined behavior.
 *
 * If the window is destroyed without unclipping the cursor, this cursor might
 * not reset depending on the platform (OS). To be safe, unclip the cursor from
 * the window before destroying the window.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] clip `PAL_TRUE` to clip to window or `PAL_FALSE` to unclip.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palClipCursor(
    PalWindow* window,
    PalBool clip);

/**
 * @brief Gets the position of the cursor relative to the window in
 * pixels.
 *
 * `PAL_VIDEO_FEATURE_CURSOR_GET_POS` must be supported 
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[out] x The output to recieve the x in pixels. Can be `nullptr`.
 * @param[out] y The output to recieve the y in pixels. Can be `nullptr`.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetCursorPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y);

/**
 * @brief Sets the position of the cursor relative to the window in
 * pixels.
 *
 * `PAL_VIDEO_FEATURE_CURSOR_SET_POS` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] x The new x coordinate of the cursor in pixels.
 * @param[in] y The new y coordinate of the cursor in pixels.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palSetCursorPos(
    PalWindow* window,
    int32_t x,
    int32_t y);

/**
 * @brief Sets the cursor for the window.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] cursor The cursor. Set to `nullptr` to revert to default.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor);

/**
 * @brief Gets the native application instance or display.
 *
 * This returns the native instance or display of the application
 * PAL video was initialized in.
 *
 * On `Linux:` This is the Display associated with the connection.

 * On `Windows:` This is the HINSTANCE of the process.
 *
 * @return The instance or display on success or `nullptr` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @note The returned instance or display must not be freed if its
 * owned by PAL.
 *
 * @since Added in version 2.0
 */
PAL_API void* PAL_CALL palGetInstance();

/**
 * @brief Attachs a foreign or native window to PAL video system.
 *
 * `PAL_VIDEO_FEATURE_FOREIGN_WINDOWS` must be supported 
 * otherwise undefined behavior.
 *
 * This function registers the window with PAL video system so it
 * can manage events and use its functionality/API.
 *
 * PAL does not own the window, it just sends events to that window.
 * Users are responsible for destroying the window when no longer needed.
 * `palDestroyWindow()` does not destroy the foreign or native window.
 *
 * Use Case:
 *
 * PAL takes your native foreign or native window and gives you a PalWindow
 * which can be used with all of PAL API. The native window must be valid
 * till the PalWindow has been detached with `palDetachWindow()`.
 *
 * The window must be created with the same instance or display
 * that PAL uses. see `palGetInstance()`.
 *
 * @param[in] windowHandle The foreign or native window. `Must not be nullptr`.
 * @param[out] window The output handle to recieve the attached window.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palGetInstance
 * @sa palDestroyWindow
 * @sa palDetachWindow
 */
PAL_API PalResult PAL_CALL palAttachWindow(
    void* windowHandle,
    PalWindow** outWindow);

/**
 * @brief Detachs a foreign or native window from PAL video system.
 *
 * `PAL_VIDEO_FEATURE_FOREIGN_WINDOWS` must be supported
 * otherwise undefined behavior.
 *
 * This function unregisters the window from PAL video system.
 * The window must not be owned by PAL otherwise the function fails
 * and return `PAL_RESULT_CODE_INVALID_HANDLE`.
 *
 * Detaching the window does not destroy the window,
 * therefore destroying the window is the users responsibility.
 *
 * Use Case:
 *
 * Give back the PalWindow returned at `palAttachWindow()`
 * and optionally get back your native window.
 *
 * @param[in] window The window. `Must not be nullptr`.
 * @param[out] windowHandle The output handle to recieve the foreign
 * or native window. Can be `nullptr`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palAttachWindow
 */
PAL_API PalResult PAL_CALL palDetachWindow(
    PalWindow* window,
    void** windowHandle);

/** @} */

#endif // PAL_VIDEO_H
