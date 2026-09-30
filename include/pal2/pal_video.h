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
#define PAL_KEYCODE_COUNT 106
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
#define PAL_SCANCODE_COUNT 106
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
 * @brief Information about a video driver.
 *
 * @since Added in version 2.2
 * @ingroup pal_video
 * 
 * @sa palEnumerateVideoDrivers
 */
typedef struct PalVideoDriver {
    /** A bitmask of supported features of the driver.*/
    PalVideoFeatures features;

    /** The video driver type. PAL_VIDEO_DRIVER_TYPE_UNKNOWN will be set
     * if PAL does not recognized or know the drivers type. Select a
     * driver based on ::features instead.
     */
    PalVideoDriverType type;

    /** This is the driver id. This must not be modified.*/
    uint32_t id;
} PalVideoDriver;

/**
 * @brief Returns a list of all supported video drivers of the platform.
 * 
 * This function returns a list of all the supported video drivers of the
 * platform. The returned array must not be freed or modified by the user.
 * This can be called and cached once, video drivers are not removed
 * or added dynamically.
 * 
 * Set `drivers` parameter to nullptr to get the total number of supported
 * video drivers. If the drivers array passed is less than the number of
 * supported drivers, PAL will fill the array upto that limit sequentially.
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
 * @brief Sets the preferred video driver the video system should use.
 * 
 * The video system must not be initialized before this call. If the video
 * system is initialized already, the next initialization will use the
 * driver. The video driver must be valid, otherwise this function will
 * ignore it silently and select a default. Get the driver from
 * @ref palEnumerateVideoDrivers to get a supported driver.
 * 
 * @param driver The video driver.
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
 * @brief Gets the active or selected video driver of the video system.
 * 
 * The video system must be initialized before this.
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
 * This must be called before any video function. Calling this function
 * multiple times does nothing if the video system is already initialized.
 *
 * `allocator` and `eventDriver` parameters will not not copied, therefore
 * the pointers must remain valid until the video system has shutdown. 
 * The event driver must be valid to recieve video events.
 *
 * If `preferredInstance` is nullptr, the video system creates one and 
 * control its lifetime. The provided instance will not be freed by
 * the video system.
 * 
 * The video driver the video system uses is selected by default. To override
 * this, enumerate all the supported video drivers of the platform with
 * @ref palEnumerateVideoDrivers and select one with @ref palSetVideoDriver.
 * Call @ref palGetVideoDriver to get the selected driver if an explicit
 * driver was not selected. This is not a hint, therefore the returned
 * drivers are all available and can be used.
 * 
 * The video system must be shutdown with @ref palShutdownVideo when no
 * longer needed.
 *
 * @param[in] allocator The allocator. Set to nullptr to use the
 *                      thread-safe default.
 * @param[in] eventDriver The event driver. If nullptr, the video system
 *                        will not process events.
 * @param[in] preferredInstance User instance or display. Can be nullptr.
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 *         failure. Call @ref palFormatResult to get the string representation
 *         of the result value.
 *
 * @Thread-safety Must only be called from the main thread.
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
 * If the video system has not been initialized, the function returns silently.
 * All created windows, icons, and cursors must be destroyed before this call.
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
 * @brief Updates the video system and all created windows.
 *
 * If the video system has not been initialized, the function returns silently.
 * This function pushes generated video events to the event driver set at
 * @ref palInitVideo. If there was no event driver, the events will not be
 * processed. Windows might be responsive even if there is no event driver
 * but thats implementation-defined behavior.
 * 
 * This function processes the events and return immediately if there
 * is an event driver. For now, there is no way to block or wait for events
 * to be processed.
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
 * This returned supported features is from the active or 
 * selected video driver.
 *
 * @return video features on success or `0` on failure.
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
 * @brief Gets the native application instance or display.
 *
 * This returns the native instance or display of the application
 * PAL video was initialized in.
 *
 * On `Linux:` This is the Display associated with the connection.

 * On `Windows:` This is the HINSTANCE of the process.
 *
 * @return The instance or display on success or nullptr on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @note The returned instance or display must not be freed if its
 * owned by PAL.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void* PAL_CALL palGetInstance();

#endif // PAL_VIDEO_H
