/**
 * @brief This is the header file for PAL Video Module API.
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
 * @{
 */
#define PAL_VIDEO_FEATURE_HIGH_DPI (1ULL << 0) /**< support for high-DPI windows */
#define PAL_VIDEO_FEATURE_MONITOR_SET_ORIENTATION (1ULL << 1) /**< support for setting monitor orientation */
#define PAL_VIDEO_FEATURE_MONITOR_GET_ORIENTATION (1ULL << 2) /**< support for getting monitor orientation */
#define PAL_VIDEO_FEATURE_BORDERLESS_WINDOW (1ULL << 3) /**< support for borderless windows */
#define PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW (1ULL << 4) /**< support for transparent windows */
#define PAL_VIDEO_FEATURE_TOOL_WINDOW (1ULL << 5) /**< support for windows without taskbar icon */
#define PAL_VIDEO_FEATURE_MONITOR_SET_MODE (1ULL << 6) /**< support for setting monitor display mode */
#define PAL_VIDEO_FEATURE_MONITOR_GET_MODE (1ULL << 7) /**< support for getting monitor display mode */
#define PAL_VIDEO_FEATURE_MULTI_MONITORS (1ULL << 8) /**< depreciated. */
#define PAL_VIDEO_FEATURE_WINDOW_SET_SIZE (1ULL << 9) /**< support for setting window size */
#define PAL_VIDEO_FEATURE_WINDOW_GET_SIZE (1ULL << 10) /**< support for getting window size */
#define PAL_VIDEO_FEATURE_WINDOW_SET_POS (1ULL << 11) /**< support for setting window position */
#define PAL_VIDEO_FEATURE_WINDOW_GET_POS (1ULL << 12) /**< support for getting window position */
#define PAL_VIDEO_FEATURE_WINDOW_SET_STATE (1ULL << 13) /**< support for setting window state */
#define PAL_VIDEO_FEATURE_WINDOW_GET_STATE (1ULL << 14) /**< support for getting window state */
#define PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY (1ULL << 15) /**< support for setting window visibility */
#define PAL_VIDEO_FEATURE_WINDOW_GET_VISIBILITY (1ULL << 16) /**< support for getting window visibility */
#define PAL_VIDEO_FEATURE_WINDOW_SET_TITLE (1ULL << 17) /**< support for setting window title */
#define PAL_VIDEO_FEATURE_WINDOW_GET_TITLE (1ULL << 18) /**< support for getting window title */
#define PAL_VIDEO_FEATURE_NO_MAXIMIZEBOX (1ULL << 19) /**< support for windows with no maximize button */
#define PAL_VIDEO_FEATURE_NO_MINIMIZEBOX (1ULL << 20) /**< support for windows with no minimize button */
#define PAL_VIDEO_FEATURE_CLIP_CURSOR (1ULL << 21) /**< support for clipping cursor to window */
#define PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION (1ULL << 22) /**< support for flashing window titlebar */
#define PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY (1ULL << 23) /**< support for flashing window taskbar icon */
#define PAL_VIDEO_FEATURE_WINDOW_FLASH_INTERVAL (1ULL << 24) /**< support for setting flash intervals */
#define PAL_VIDEO_FEATURE_WINDOW_SET_INPUT_FOCUS (1ULL << 25) /**< support for setting window input focus */
#define PAL_VIDEO_FEATURE_WINDOW_GET_INPUT_FOCUS (1ULL << 26) /**< support for getting window input focus */
#define PAL_VIDEO_FEATURE_WINDOW_SET_STYLE (1ULL << 27) /**< support for setting window style */
#define PAL_VIDEO_FEATURE_WINDOW_GET_STYLE (1ULL << 28) /**< support for getting window style */
#define PAL_VIDEO_FEATURE_CURSOR_SET_POS (1ULL << 29) /**< support for setting cursor position */
#define PAL_VIDEO_FEATURE_CURSOR_GET_POS (1ULL << 30) /**< support for getting cursor position */
#define PAL_VIDEO_FEATURE_WINDOW_SET_ICON (1ULL << 31) /**< support for setting window icon */
#define PAL_VIDEO_FEATURE_TOPMOST_WINDOW (1ULL << 32) /**< support for topmost windows */
#define PAL_VIDEO_FEATURE_DECORATED_WINDOW (1ULL << 33) /**< support for decorated windows */
#define PAL_VIDEO_FEATURE_CURSOR_SET_VISIBILITY (1ULL << 34) /**< support for setting cursor visibility */
#define PAL_VIDEO_FEATURE_WINDOW_GET_MONITOR (1ULL << 35) /**< support for getting window monitor */
#define PAL_VIDEO_FEATURE_MONITOR_GET_PRIMARY (1ULL << 36) /**< support for getting primary monitor */
#define PAL_VIDEO_FEATURE_FOREIGN_WINDOWS (1ULL << 37) /**< support for attaching and detaching foreign windows */
#define PAL_VIDEO_FEATURE_MONITOR_VALIDATE_MODE (1ULL << 38) /**< support for validating monitor display modes */
#define PAL_VIDEO_FEATURE_WINDOW_SET_CURSOR (1ULL << 39) /**< support for setting window cursor */
/** @} */

/**
 * @defgroup video_drivers Video Drivers
 * @{
 */
#define PAL_VIDEO_DRIVER_TYPE_UNKNOWN 0 /**< unknown video driver type to PAL */
#define PAL_VIDEO_DRIVER_TYPE_WIN32 1
#define PAL_VIDEO_DRIVER_TYPE_WAYLAND 2
#define PAL_VIDEO_DRIVER_TYPE_X11 3
#define PAL_VIDEO_DRIVER_TYPE_COUNT 4 /**< number of video drivers */
/** @} */

/**
 * @defgroup window_styles Window Styles
 * @{
 */
#define PAL_WINDOW_STYLE_RESIZABLE (1U << 0) /**< window is resizable */
#define PAL_WINDOW_STYLE_TRANSPARENT (1U << 1) /**< window is transparent */
#define PAL_WINDOW_STYLE_TOPMOST (1U << 2) /**< window is topmost */
#define PAL_WINDOW_STYLE_NO_MINIMIZEBOX (1U << 3) /**< window has no minimize button */
#define PAL_WINDOW_STYLE_NO_MAXIMIZEBOX (1U << 4) /**< window has no maximize button */
#define PAL_WINDOW_STYLE_TOOL (1U << 5) /**< window has no taskbar icon */
#define PAL_WINDOW_STYLE_BORDERLESS (1U << 6) /**< window has no decorations */
/** @} */

/**
 * @defgroup window_states Window States
 * @{
 */
#define PAL_WINDOW_STATE_NORMAL 0 /**< windowed mode */
#define PAL_WINDOW_STATE_MAXIMIZED 1
#define PAL_WINDOW_STATE_MINIMIZED 2
#define PAL_WINDOW_STATE_RESTORED 3
#define PAL_WINDOW_STATE_COUNT 4 /**< number of window states */
/** @} */

/**
 * @defgroup flash_flags Flash Flags
 * @{
 */
#define PAL_FLASH_FLAG_STOP 0 /**< stop all flash operations */
#define PAL_FLASH_FLAG_CAPTION (1U << 0) /**< flash window titlebar */
#define PAL_FLASH_FLAG_TRAY (1U << 1) /**< flash window taskbar icon */
/** @} */

/**
 * @defgroup fbconfig_backends Framebuffer Configuration Backends
 * @{
 */
#define PAL_FBCONFIG_BACKEND_PAL_OPENGL 0 /**< PAL OpenGL backend */
#define PAL_FBCONFIG_BACKEND_EGL 1
#define PAL_FBCONFIG_BACKEND_GLX 2
#define PAL_FBCONFIG_BACKEND_WGL 3
#define PAL_FBCONFIG_BACKEND_COUNT 4 /**< number of framebuffer configuration backends */
/** @} */

/**
 * @defgroup orientations Orientations
 * @{
 */
#define PAL_ORIENTATION_LANDSCAPE 0
#define PAL_ORIENTATION_PORTRAIT 1
#define PAL_ORIENTATION_LANDSCAPE_FLIPPED 2 /**< landscape rotated 180 degrees */
#define PAL_ORIENTATION_PORTRAIT_FLIPPED 3 /**< portrait rotated 180 degrees */
#define PAL_ORIENTATION_COUNT 4 /**< number of orientations */
/** @} */

/**
 * @defgroup cursor_types Cursor Types
 * @{
 */
#define PAL_CURSOR_TYPE_ARROW 0
#define PAL_CURSOR_TYPE_HAND 1
#define PAL_CURSOR_TYPE_CROSS 2
#define PAL_CURSOR_TYPE_IBEAM 3
#define PAL_CURSOR_TYPE_WAIT 4
#define PAL_CURSOR_TYPE_COUNT 5 /**< number of cursor types */
/** @} */

/**
 * @defgroup keycodes Keycodes
 * @{
 */
#define PAL_KEYCODE_UNKNOWN 0 /**< unknown keycode to PAL*/
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
#define PAL_KEYCODE_LSHIFT 57 /**< left shift */
#define PAL_KEYCODE_RSHIFT 58 /**< right shift */
#define PAL_KEYCODE_LCTRL 59 /**< left control */
#define PAL_KEYCODE_RCTRL 60 /**< right control */
#define PAL_KEYCODE_LALT 61 /**< left alt */
#define PAL_KEYCODE_RALT 62 /**< right alt */
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
#define PAL_KEYCODE_APOSTROPHE 93 /**< ' */
#define PAL_KEYCODE_BACKSLASH 94 /**< \ */
#define PAL_KEYCODE_COMMA 95
#define PAL_KEYCODE_EQUAL 96
#define PAL_KEYCODE_GRAVEACCENT 97 /**< ` */
#define PAL_KEYCODE_SUBTRACT 98
#define PAL_KEYCODE_PERIOD 99 /**< . */
#define PAL_KEYCODE_SEMICOLON 100
#define PAL_KEYCODE_SLASH 101 /**< / */
#define PAL_KEYCODE_LBRACKET 102 /**< [ */
#define PAL_KEYCODE_RBRACKET 103 /**< ] */
#define PAL_KEYCODE_LSUPER 104 /**< left super or windows */
#define PAL_KEYCODE_RSUPER 105 /**< right super or windows */
#define PAL_KEYCODE_COUNT 106 /**< number of keycodes */
/** @} */

/**
 * @defgroup scancodes Scancodes
 * @{
 */
#define PAL_SCANCODE_UNKNOWN 0 /**< unknown scancode to PAL*/
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
#define PAL_SCANCODE_LSHIFT 57 /**< left shift */
#define PAL_SCANCODE_RSHIFT 58 /**< right shift */
#define PAL_SCANCODE_LCTRL 59 /**< left control */
#define PAL_SCANCODE_RCTRL 60 /**< right control */
#define PAL_SCANCODE_LALT 61 /**< left alt */
#define PAL_SCANCODE_RALT 62 /**< right alt */
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
#define PAL_SCANCODE_APOSTROPHE 93 /**< ' */
#define PAL_SCANCODE_BACKSLASH 94 /**< \ */
#define PAL_SCANCODE_COMMA 95
#define PAL_SCANCODE_EQUAL 96
#define PAL_SCANCODE_GRAVEACCENT 97 /**< ` */
#define PAL_SCANCODE_SUBTRACT 98
#define PAL_SCANCODE_PERIOD 99 /**< . */
#define PAL_SCANCODE_SEMICOLON 100
#define PAL_SCANCODE_SLASH 101 /**< / */
#define PAL_SCANCODE_LBRACKET 102 /**< [ */
#define PAL_SCANCODE_RBRACKET 103 /**< ] */
#define PAL_SCANCODE_LSUPER 104 /**< left super or windows */
#define PAL_SCANCODE_RSUPER 105 /**< right super or windows */
#define PAL_SCANCODE_COUNT 106 /**< number of scancodes */
/** @} */

/**
 * @defgroup mouse_buttons Mouse Buttons
 * @{
 */
#define PAL_MOUSE_BUTTON_UNKNOWN 0 /**< unknown mouse button to PAL */
#define PAL_MOUSE_BUTTON_LEFT 1
#define PAL_MOUSE_BUTTON_RIGHT 2
#define PAL_MOUSE_BUTTON_MIDDLE 3
#define PAL_MOUSE_BUTTON_X1 4
#define PAL_MOUSE_BUTTON_X2 5
#define PAL_MOUSE_BUTTON_COUNT 6 /**< number of mouse buttons */
/** @} */

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
 * @typedef PalVideoDriverType
 * @brief Video driver type.
 * 
 * All values of this type follow the format `PAL_VIDEO_DRIVER_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.2
 */
typedef uint32_t PalVideoDriverType;

/**
 * @typedef PalWindowStyle
 * @brief Window style.
 * 
 * All values of this type follow the format `PAL_WINDOW_STYLE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalWindowStyle;

/**
 * @typedef PalWindowState
 * @brief Window state.
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
 * All values of this type follow the format `PAL_FLASH_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalFlashFlags;

/**
 * @typedef PalFBConfigBackend
 * @brief Framebuffer configuration backend.
 * 
 * All values of this type follow the format `PAL_FBCONFIG_BACKEND_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalFBConfigBackend;

/**
 * @typedef PalOrientation
 * @brief Monitor orientation.
 * 
 * All values of this type follow the format `PAL_ORIENTATION_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalOrientation;

/**
 * @typedef PalCursorType
 * @brief System cursor type.
 * 
 * All values of this type follow the format `PAL_CURSOR_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalCursorType;

/**
 * @typedef PalKeycode
 * @brief Keycode (layout aware key) of a keyboard.
 * 
 * All values of this type follow the format `PAL_KEYCODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalKeycode;

/**
 * @typedef PalScancode
 * @brief Scancode (layout independent key) of a keyboard.
 * 
 * All values of this type follow the format `PAL_SCANCODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalScancode;

/**
 * @typedef PalMouseButton
 * @brief Mouse button.
 * 
 * All values of this type follow the format `PAL_MOUSE_BUTTON_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef uint32_t PalMouseButton;

/**
 * @struct PalWindow
 * @brief Opaque handle to a window.
 *
 * @since Added in version 2.0
 */
typedef struct PalWindow PalWindow;

/**
 * @struct PalMonitor
 * @brief Opaque handle to a monitor.
 *
 * @since Added in version 2.0
 */
typedef struct PalMonitor PalMonitor;

/**
 * @struct PalCursor
 * @brief Opaque handle to a cursor.
 *
 * @since Added in version 2.0
 */
typedef struct PalCursor PalCursor;

/**
 * @struct PalIcon
 * @brief Opaque handle to an icon.
 *
 * @since Added in version 2.0
 */
typedef struct PalIcon PalIcon;
/** @} */

typedef struct PalEventDriver PalEventDriver;

/**
 * @struct PalVideoDriver
 * @brief Video driver.
 *
 * @since Added in version 2.2
 * @ingroup pal_video
 */
typedef struct PalVideoDriver {
    PalVideoFeatures features; /**< bitmask of supported features */
    PalVideoDriverType type; /**< video driver type */
    uint32_t id; /**< video driver id */
} PalVideoDriver;

/**
 * @struct PalFlashInfo
 * @brief Flash information.
 * 
 * `interval` greater than `0` requires @ref PAL_VIDEO_FEATURE_WINDOW_FLASH_INTERVAL
 * feature to be supported.
 * 
 * The following flash flags requires:
 * 
 * - PAL_FLASH_FLAG_CAPTION - @ref PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION feature to be supported
 * 
 * - PAL_FLASH_FLAG_TRAY - @ref PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY feature to be supported
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalFlashInfo {
    PalFlashFlags flags; /**< bitmask of supported flash flags */
    uint32_t interval; /**< flash interval or `0` */
    uint32_t count; /**< number of times to flash or `0` */
} PalFlashInfo;

/**
 * @struct PalWindowHandleInfo
 * @brief Window handle information.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalWindowHandleInfo {
    void* nativeInstance; /**< window native instance or display */
    void* nativeWindow; /**< window handle */
    void* nativeHandle1; /**< additional window handle or `nullptr` */
    void* nativeHandle2; /**< additional window handle or `nullptr` */
    void* nativeHandle3; /**< additional window handle or `nullptr` */
} PalWindowHandleInfo;

/**
 * @struct PalMonitorInfo
 * @brief Monitor information.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalMonitorInfo {
    int32_t x; /**< monitor x position in screen coordinates */
    int32_t y; /**< monitor y position in screen coordinates */
    uint32_t width; /**< monitor wdith in screen coordinates */
    uint32_t height; /**< monitor height in screen coordinates */
    uint32_t dpi; /**< monitor DPI */
    uint32_t refreshRate; /**< monitor refresh rate in Hz*/
    PalOrientation orientation; /**< monitor orientation */
    PalBool primary; /**< whether this is the primary monitor */
    char name[PAL_MONITOR_NAME_SIZE]; /**< monitor name*/
} PalMonitorInfo;

/**
 * @struct PalMonitorMode
 * @brief Monitor display mode.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalMonitorMode {
    uint32_t bpp; /**< display mode bits per pixel */
    uint32_t refreshRate; /**< display mode refresh rate in Hz */
    uint32_t width; /**< display mode width in screen coordinates */
    uint32_t height; /**< display mode height in screen coordinates */
} PalMonitorMode;

/**
 * @struct PalWindowCreateInfo
 * @brief Window creation parameters.
 * 
 * This struct is used only during @ref palCreateWindow() and may be
 * discarded after the function returns.
 * 
 * `center` only works for windowed mode. If `monitor` is `nullptr` and the initial state
 * of the window is not @ref PAL_WINDOW_STATE_NORMAL, it will be ignored.
 * 
 * The following window styles requires:
 * 
 * - PAL_WINDOW_STYLE_TRANSPARENT - @ref PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW feature to be supported
 * 
 * - PAL_WINDOW_STYLE_TOPMOST - @ref PAL_VIDEO_FEATURE_TOPMOST_WINDOW feature to be supported
 * 
 * - PAL_WINDOW_STYLE_NO_MINIMIZEBOX - @ref PAL_VIDEO_FEATURE_NO_MINIMIZEBOX feature to be supported
 * 
 * - PAL_WINDOW_STYLE_NO_MAXIMIZEBOX - @ref PAL_VIDEO_FEATURE_NO_MAXIMIZEBOX feature to be supported
 * 
 * - PAL_WINDOW_STYLE_TOOL - @ref PAL_VIDEO_FEATURE_TOOL_WINDOW feature to be supported
 * 
 * - PAL_WINDOW_STYLE_BORDERLESS - @ref PAL_VIDEO_FEATURE_BORDERLESS_WINDOW feature to be supported
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalWindowCreateInfo {
    const char* title; /**< null-terminated UTF-8 encoded title */
    PalMonitor* monitor; /**< monitor to use or `nullptr` for windowed mode */
    const char* appName; /**< null-terminated UTF-8 encoded app name or `nullptr` to use `PAL` */
    const char* instanceName; /**< null-terminated UTF-8 encoded instance name or `nullptr` to use `title` */
    PalFBConfigBackend fbConfigBackend; /**< framebuffer configuration backend */
    int32_t fbConfigIndex; /**< framebuffer configuration index or `0` for no configuration */
    uint32_t width; /**< window width in screen coordinates */
    uint32_t height; /**< window height in screen coordinates */
    PalBool show; /**< whether window should be visible after creation */
    PalWindowStyle style; /**< bitmask of supported window styles */
    PalWindowState state; /**< initial window state */
    PalBool center; /**< whether to center window after creation. */
} PalWindowCreateInfo;

/**
 * @struct PalCursorCreateInfo
 * @brief Cursor creation parameters.
 * 
 * This struct is used only during @ref palCreateCursor() and may be
 * discarded after the function returns.
 * 
 * Coordinates are relative to the upper-left corner of the cursor. 
 * X-coordinate increases to the right and Y-coordinate increases down. 
 * 
 * `pixels` must be little-endian 32-bit, RGBA 8-bits per channel.
 * After the cursor is created, the pixels can be freed, it is copied.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalCursorCreateInfo {
    const uint8_t* pixels; /**< cursor pixels */
    uint32_t width; /**< cursor width in pixels */
    uint32_t height; /**< cursor height in pixels */
    int32_t xHotspot; /**< cursor x hotspot pixel */
    int32_t yHotspot; /**< cursor y hotspot pixel */
} PalCursorCreateInfo;

/**
 * @struct PalIconCreateInfo
 * @brief Icon creation parameters.
 * 
 * This struct is used only during @ref palCreateIcon() and may be
 * discarded after the function returns.
 * 
 * Coordinates are relative to the upper-left corner of the icon. 
 * X-coordinate increases to the right and Y-coordinate increases down. 
 * 
 * `pixels` must be little-endian 32-bit, RGBA 8-bits per channel.
 * After the icon is created, the pixels can be freed, it is copied.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalIconCreateInfo {
    const uint8_t* pixels; /**< icon pixels */
    uint32_t width; /**< icon width in pixels */
    uint32_t height; /**< icon height in pixels */
} PalIconCreateInfo;

/**
 * @brief Enumerates supported video drivers.
 * 
 * This function gets all supported video drivers that can be used with the platform.
 * This is a two-call function, set `drivers` to `nullptr` and `count` to 0 to obtain
 * the number of supported video drivers. Allocate the array and call this function
 * again to with `drivers` set to the allocated array and `count` set to the 
 * capacity of the array.
 * 
 * If the specified count is less than the number of supported video drivers,
 * only the video drivers that fit in the array will be written. You may call
 * this function once, video drivers are not removed or added at runtime.
 * 
 * @param[in, out] count The capacity of the video drivers array.
 * @param drivers The video drivers array.
 * 
 * @Thread-safety Thread safe
 * 
 * @since Added in version 2.2
 * @ingroup pal_video
 * 
 * @sa palSetVideoDriver
 * @sa palGetVideoDriver
 */
PAL_API void PAL_CALL palEnumerateVideoDrivers(
    uint32_t* count,
    PalVideoDriver* drivers);

/**
 * @brief Sets the preferred video driver.
 * 
 * This function sets the preferred video driver the video system should use. 
 * The video driver must be supported. If the video system has been initialized,
 * the video driver will be ignored. Setting an unsupported video driber will be
 * ignored and the video system will select a default.
 * 
 * @param driver Preferred video driver.
 * 
 * @Thread-safety Must only be called from the main thread.
 * 
 * @since Added in version 2.2
 * @ingroup pal_video
 * 
 * @sa palEnumerateVideoDrivers
 * @sa palGetVideoDriver
 */
PAL_API void PAL_CALL palSetVideoDriver(PalVideoDriver* driver);

/**
 * @brief Gets the active video driver.
 * 
 * This function gets the active or selected video driver of the system.
 * If the video system has not been initialized, this function will
 * set `driver` to `nullptr`.
 * 
 * @param[out] driver The output struct to recieve the video driver.
 * 
 * @Thread-safety Thread safe.
 * 
 * @since Added in version 2.2
 * @ingroup pal_video
 * 
 * @sa palEnumerateVideoDrivers
 * @sa palSetVideoDriver
 */
PAL_API void PAL_CALL palGetVideoDriver(PalVideoDriver* driver);

/**
 * @brief Initializes the video system.
 * 
 * The function initializes the video system. If the video system has been initialized,
 * the function does nothing.
 *
 * `allocator` and `eventDriver` are not copied. The allocator and the event driver
 * with any state referenced by them must remain valid until @ref palShutdownVideo() is called.
 * If the event driver is `nullptr`, the video system will not process events`.
 *
 * If `preferredInstance` is `nullptr`, the video system creates one and 
 * control its lifetime. On Linux, `preferredInstance` is the displat associated with
 * the connection. On Windows, `preferredInstance` is the process HINSTANCE.
 * The preferred instance will not be destroyed by the video system when
 * @ref palShutdownVideo().
 * 
 * If the preferred instance is `nullptr`, the video system creates and control
 * its lifetime.
 * 
 * @param[in] allocator Allocator to use or `nullptr` for default.
 * @param[in] eventDriver Event driver to use or `nullptr` to disable events.
 * @param[in] preferredInstance User-provided instance or `nullptr`.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 *
 * @Thread-safety Must only be called from the main thread.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palEnumerateVideoDrivers
 * @sa palSetVideoDriver
 * @sa palGetVideoDriver
 * @sa palShutdownVideo
 */
PAL_API PalResult PAL_CALL palInitVideo(
    const PalAllocator* allocator,
    PalEventDriver* eventDriver,
    void* preferredInstance);

/**
 * @brief Shutdowns the video system.
 * 
 * This function shutdowns the video system. All created windows, icons and cursors
 * must be destroyed before this call. If the video system has not been
 * initialized, the function returns silently.
 * 
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palInitVideo
 */
PAL_API void PAL_CALL palShutdownVideo();

/**
 * @brief Updates the video system.
 * 
 * This function updates the video system and created and attached windows.
 * If the event driver supplied to palInitVideo() is `nullptr`, the
 * events will not be processed and created or attached windows may be 
 * unresponsive, the behavior is undefined. This function does not
 * block or waits for the event driver to populate events.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palInitVideo
 */
PAL_API void PAL_CALL palUpdateVideo();

/**
 * @brief Gets the supported features of the video system.
 * 
 * This function returns the selected or active video driver features.
 *
 * @return Video features on success or `0` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palInitVideo
 */
PAL_API PalVideoFeatures PAL_CALL palGetVideoFeatures();

/**
 * @brief Gets the video system instance.
 * 
 * This function returns the video system instance.
 * The instance is the display associated with the connect on Linux and the
 * process HINSTANCE on Windows.
 * 
 * The instance must not be destroyed if its owned by video system.
 *
 * @return Instance on success or `nullptr` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void* PAL_CALL palGetInstance();

/**
 * @brief Creates a window.
 * 
 * This function creates a window using the specified creation parameters.
 * `info` must remain valid for the duration of this function. PAL does not
 * copy the its contents.
 * 
 * Very large or small `info->width` and `info->height` will be overridden
 * by the video system and will be ignored if `info->monitor` is not `nullptr`.
 * Setting `info->fbConfigIndex` does not create a vulkan surface or OpengGL context
 * for the specified window. This creates the window with the specified
 * framebuffer configuration or pixel format index.
 * 
 * To create a borderless fullscreen window, use @ref PAL_WINDOW_STYLE_BORDERLESS style
 * and select a monitor. The window will be created in windowed mode if the selected
 * monitor is not valid.To create an exclusive fullscreen window, use @ref palSetMonitorMode
 * to set the display mode of the monitor. The monitor display mode will not be switched if 
 * its invalid.
 * 
 * The created window must be destroyed with @ref palDestroyWindow() when
 * no longer needed.
 *
 * @param[in] info window creation parameters.
 * @param[out] window Output handle to recieve the created window.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 *
 * @Thread-safety Must only be called from the main thread.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @note On `Wayland`:
 *
 * - Creating non resizable windows is not supported. It will be ignored.
 *
 * - Creating windows on a specific monitor is not supported.
 *
 * - Creating hidden windows is not supported. It will be ignored.
 * 
 * On `Win32`:
 * 
 * - Creating non resizable windows also removes the maximize box.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palDestroyWindow
 */
PAL_API PalResult PAL_CALL palCreateWindow(
    const PalWindowCreateInfo* info,
    PalWindow** window);

/**
 * @brief Destroys a window.
 * 
 * This function destroys the specified window and frees it resources if it was
 * created by the video system. Using this function with a foreign does nothing.
 *
 * @param[in] window The window to destroy.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palCreateWindow
 */
PAL_API void PAL_CALL palDestroyWindow(PalWindow* window);

/**
 * @brief Minimizes a window.
 * 
 * This function minimizes a window if its not already minimized.
 * @ref PAL_VIDEO_FEATURE_WINDOW_SET_STATE feature must be supported or this function
 * results in undefined behavior.
 *
 * @param[in] window Window to minimize.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palMaximizeWindow
 * @sa palRestoreWindow
 */
PAL_API void PAL_CALL palMinimizeWindow(PalWindow* window);

/**
 * @brief Maximizes a window.
 * 
 * This function Maximizes a window if its not already maximized.
 * @ref PAL_VIDEO_FEATURE_WINDOW_SET_STATE feature must be supported or this function
 * results in undefined behavior.
 *
 * @param[in] window Window to maximize.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palMinimizeWindow
 * @sa palRestoreWindow
 */
PAL_API void PAL_CALL palMaximizeWindow(PalWindow* window);

/**
 * @brief Restores a window.
 * 
 * This function restores the specified window to its previous state if not restored already.
 * @ref PAL_VIDEO_FEATURE_WINDOW_SET_STATE feature must be supported or this function
 * results in undefined behavior.
 *
 * @param[in] window Window to restore.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @note Wayland does not support restoring a minimized window.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palMinimizeWindow
 * @sa palMaximizeWindow
 */
PAL_API void PAL_CALL palRestoreWindow(PalWindow* window);

/**
 * @brief Shows the window.
 * 
 * This function shows the specified window if its not shown.
 * @ref PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY feature must be supported or this function
 * results in undefined behavior.
 *
 * @param[in] window Window to show.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palHideWindow
 */
PAL_API void PAL_CALL palShowWindow(PalWindow* window);

/**
 * @brief Hides the window.
 *
 * This function hides the specified window if its not hidden.
 * @ref PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY feature must be supported or this function
 * results in undefined behavior.
 *
 * @param[in] window Window to hide.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palShowWindow
 */
PAL_API void PAL_CALL palHideWindow(PalWindow* window);

/**
 * @brief Flash a window.
 * 
 * This function requests the platform to visually flash the specified window.
 *
 * @param[in] window Window to flash.
 * @param[in] info Window flash parameters.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info);

/**
 * @brief Attachs a window.
 * 
 * This functions attachs a foreign or native window to the video system.
 * @ref PAL_VIDEO_FEATURE_FOREIGN_WINDOWS feature must be supported
 *
 * This function registers `windowHandle` with the video system to allow the window
 * use the video system API. The video system does mot own the attached window, users
 * are required to destroy the window after it has been detached with @ref palDetachWindow().
 * 
 * `windowHandle` must be created with the same instance the video system uses. Use
 * @ref palGetInstance() to get the instance the video system uses. see @ref palInitVideo()
 * to set a preferred instance.
 *
 * @param[in] windowHandle Foreign or native window.
 * @param[out] window Output handle to recieve the attached window.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 *
 * @Thread-safety Must be called from the main thread.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palInitVideo
 * @sa palGetInstance
 * @sa palDetachWindow
 */
PAL_API PalResult PAL_CALL palAttachWindow(
    void* windowHandle,
    PalWindow** window);

/**
 * @brief Detaches a window.
 * 
 * This function detaches a foreign or native window from the video system.
 * `window` must not be owned by the video system and detaching the window will not
 * destroy it, users are responsible for destroy the window after its been detached.
 * @ref PAL_VIDEO_FEATURE_FOREIGN_WINDOWS feature must be supported.
 *
 * @param[in] window Window to detach.
 * @param[out] windowHandle Output handle to recieve the foreign window or `nullptr`.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 *
 * @Thread-safety Must be called from the main thread.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *               @ref PAL_RESULT_CODE_INVALID_HANDLE
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palAttachWindow
 */
PAL_API PalResult PAL_CALL palDetachWindow(
    PalWindow* window,
    void** windowHandle);

/**
 * @brief Gets the current style of the window.
 *
 * PAL_VIDEO_FEATURE_WINDOW_GET_STYLE must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[out] style The output to recieve the window style.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palSetWindowStyle
 */
PAL_API void PAL_CALL palGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* style);

/**
 * @brief Gets the monitor the window is currently on.
 *
 * PAL_VIDEO_FEATURE_WINDOW_GET_MONITOR must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[out] monitor The output handle to recieve the monitor.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palGetWindowMonitor(
    PalWindow* window,
    PalMonitor** monitor);

/**
 * @brief Gets the title of the window.
 *
 * PAL_VIDEO_FEATURE_WINDOW_GET_TITLE must be supported
 * otherwise undefined behavior.
 *
 * Set the buffer to nullptr to get the size of the window name in bytes.
 * If the size of the provided buffer is less than the actual size of window
 * title, PAL will write upto that limit.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[in] bufferSize The size of the buffer in bytes.
 * @param[out] size The actual size of the window title in bytes.
 * @param[out] buffer The output buffer to write to.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palSetWindowTitle
 */
PAL_API void PAL_CALL palGetWindowTitle(
    PalWindow* window,
    uint64_t bufferSize,
    uint64_t* size,
    char* buffer);

/**
 * @brief Gets the position of the window in screen coordinates.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_GET_POS` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[out] x The output to recieve the window x position in
 *               screen coordinates. Can be nullptr.
 * @param[out] y The output to recieve the window y position in
 *               screen coordinates. Can be nullptr.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palSetWindowPos
 */
PAL_API void PAL_CALL palGetWindowPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y);

/**
 * @brief Gets the size of the window in screen coordinates.
 *
 * PAL_VIDEO_FEATURE_WINDOW_GET_SIZE must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[out] width The output to recieve the window width in
 * screen coordinates. Can be nullptr.
 * @param[out] height The output to recieve the window height in
 * screen coordinates. Can be nullptr.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palSetWindowSize
 */
PAL_API void PAL_CALL palGetWindowSize(
    PalWindow* window,
    uint32_t* width,
    uint32_t* height);

/**
 * @brief Gets the state of the window.
 *
 * PAL_VIDEO_FEATURE_WINDOW_GET_STATE must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[out] state The output to recieve the window state.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palGetWindowState(
    PalWindow* window,
    PalWindowState* state);

/**
 * @brief Checks if the window is visible.
 *
 * PAL_VIDEO_FEATURE_WINDOW_GET_VISIBILITY must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be nullptr.
 * @return `PAL_TRUE` if the window is visible otherwise `PAL_FALSE`.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API PalBool PAL_CALL palIsWindowVisible(PalWindow* window);

/**
 * @brief Gets the current input-focused window per application.
 *
 * PAL_VIDEO_FEATURE_WINDOW_GET_INPUT_FOCUS must be supported
 * otherwise undefined behavior. This function returns the keyboard
 * or mouse input-focused window.
 *
 * @return The current input-focused window on success or nullptr on
 * failure.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API PalWindow* PAL_CALL palGetFocusWindow();

/**
 * @brief Gets the native handle of the window.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[out] info The output struct to recieve the window handle info.
 *
 * @Thread-safety Thread-safe.
 * 
 * @note On `Wayland`:
 * 
 * - PalWindowHandleInfo::nativeHandle1 is set to the `xdg_surface`.
 * 
 * - PalWindowHandleInfo::nativeHandle1 is set to the `xdg_toplevel`.
 * 
 * - PalWindowHandleInfo::nativeHandle1 is set to the `wl_egl_window`.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palGetWindowHandleInfo(
    PalWindow* window,
    PalWindowHandleInfo* info);

/**
 * @brief Sets the opacity of the window.
 *
 * PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW must be supported
 * otherwise undefined behavior. The window must have be
 * have been created with @ref PAL_WINDOW_STYLE_TRANSPARENT style.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[in] opacity Must be in the range `0.0 - 1.0`.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palSetWindowOpacity(
    PalWindow* window,
    float opacity);

/**
 * @brief Sets the style of the window.
 *
 * PAL_VIDEO_FEATURE_WINDOW_SET_STYLE must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[in] style The window style.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palGetWindowStyle
 */
PAL_API void PAL_CALL palSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style);

/**
 * @brief Sets the title of the window.
 *
 * PAL_VIDEO_FEATURE_WINDOW_SET_TITLE must be supported 
 * otherwise undefined behavior. The title must be a UTF-8 encoding
 * null terminated string.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[in] title The window title.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palGetWindowTitle
 */
PAL_API void PAL_CALL palSetWindowTitle(
    PalWindow* window,
    const char* title);

/**
 * @brief Sets the position of the window in screen coordinates.
 *
 * PAL_VIDEO_FEATURE_WINDOW_SET_POS must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[in] x The new x coordinate in screen coordinates.
 * @param[in] y The new y coordinate in screen coordinates.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palGetWindowPos
 */
PAL_API void PAL_CALL palSetWindowPos(
    PalWindow* window,
    int32_t x,
    int32_t y);

/**
 * @brief Sets the size of the window in screen coordinates.
 *
 * PAL_VIDEO_FEATURE_WINDOW_SET_SIZE must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[in] width The new width of the window in screen coordinates. 
 *                  Must be greater than `0`.
 * @param[in] height The new height of the window in screen coordinates. 
 *                   Must be greater than `0`.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palGetWindowSize
 */
PAL_API void PAL_CALL palSetWindowSize(
    PalWindow* window,
    uint32_t width,
    uint32_t height);

/**
 * @brief Requests input focus for the window.
 *
 * PAL_VIDEO_FEATURE_WINDOW_SET_INPUT_FOCUS must be supported
 * otherwise undefined behavior. The window must be visible.
 *
 * @param[in] window The window. Must not be nullptr.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palGetFocusWindow
 */
PAL_API void PAL_CALL palSetFocusWindow(PalWindow* window);

#endif // PAL_VIDEO_H
