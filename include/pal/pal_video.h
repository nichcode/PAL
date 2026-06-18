
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup pal_video Video
 * @ingroup pal_video
 * @{
 */

#ifndef _PAL_VIDEO_H
#define _PAL_VIDEO_H

#include "pal_event.h"

#define PAL_MONITOR_NAME_SIZE 32

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

#define PAL_ORIENTATION_LANDSCAPE 0
#define PAL_ORIENTATION_PORTRAIT 1
#define PAL_ORIENTATION_LANDSCAPE_FLIPPED 2
#define PAL_ORIENTATION_PORTRAIT_FLIPPED 3

#define PAL_WINDOW_STYLE_RESIZABLE (1ULL << 0)
#define PAL_WINDOW_STYLE_TRANSPARENT (1ULL << 1)
#define PAL_WINDOW_STYLE_TOPMOST (1ULL << 2)
#define PAL_WINDOW_STYLE_NO_MINIMIZEBOX (1ULL << 3)
#define PAL_WINDOW_STYLE_NO_MAXIMIZEBOX (1ULL << 4)
#define PAL_WINDOW_STYLE_TOOL (1ULL << 5)
#define PAL_WINDOW_STYLE_BORDERLESS (1ULL << 6)

#define PAL_WINDOW_STATE_MAXIMIZED 0
#define PAL_WINDOW_STATE_MINIMIZED 1
#define PAL_WINDOW_STATE_RESTORED 2

#define PAL_FLASH_STOP 0                 /**< Stop flashing.*/
#define PAL_FLASH_CAPTION (1ULL << 0)    /**< Flash the titlebar of the window.*/
#define PAL_FLASH_TRAY (1ULL << 1)       /**< Flash the icon of the window.*/

#define PAL_CONFIG_BACKEND_PAL_OPENGL 0 /**< Use PAL opengl module backend.*/
#define PAL_CONFIG_BACKEND_EGL 1
#define PAL_CONFIG_BACKEND_GLX 2
#define PAL_CONFIG_BACKEND_WGL 3
#define PAL_CONFIG_BACKEND_GLES 4

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

#define PAL_SCANCODE_MAX 106

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

#define PAL_KEYCODE_MAX 106

#define PAL_MOUSE_BUTTON_UNKNOWN 0
#define PAL_MOUSE_BUTTON_LEFT 1
#define PAL_MOUSE_BUTTON_RIGHT 2
#define PAL_MOUSE_BUTTON_MIDDLE 3
#define PAL_MOUSE_BUTTON_X1 4
#define PAL_MOUSE_BUTTON_X2 5

#define PAL_MOUSE_BUTTON_MAX 6

#define PAL_CURSOR_ARROW 0
#define PAL_CURSOR_HAND 1
#define PAL_CURSOR_CROSS 2
#define PAL_CURSOR_IBEAM 3
#define PAL_CURSOR_WAIT 4

#define PAL_CURSOR_MAX 5

/**
 * @struct PalMonitor
 * @brief Opaque handle to a monitor.
 *
 * @since 1.0
 */
typedef struct PalMonitor PalMonitor;

/**
 * @struct PalWindow
 * @brief Opaque handle to a window.
 *
 * @since 1.0
 */
typedef struct PalWindow PalWindow;

/**
 * @struct PalIcon
 * @brief Opaque handle to an icon.
 *
 * @since 1.0
 */
typedef struct PalIcon PalIcon;

/**
 * @struct PalCursor
 * @brief Opaque handle to a cursor.
 *
 * @since 1.0
 */
typedef struct PalCursor PalCursor;

/**
 * @typedef PalVideoFeatures
 * @brief Video system features.
 *
 * All video features follow the format `PAL_VIDEO_FEATURE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint64_t PalVideoFeatures;

/**
 * @typedef PalOrientation
 * @brief Orientation types for a monitor.
 *
 * All orientation types follow the format `PAL_ORIENTATION_**` for consistency
 * and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalOrientation;

/**
 * @typedef PalWindowStyle
 * @brief Window styles. Multiple styles can be OR'ed together using bitwise
 * OR operator (`|`).
 *
 * All window flags follow the format `PAL_WINDOW_STYLE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint64_t PalWindowStyle;

/**
 * @typedef PalWindowState
 * @brief Represents the current state of a window.
 *
 * All window states follow the format `PAL_WINDOW_STATE_**` for consistency and
 * API use.
 *
 * @since 2.0
 */
typedef uint32_t PalWindowState;

/**
 * @typedef PalFlashFlag
 * @brief Flash flags. Multiple flash flags can be OR'ed together using bitwise
 * OR operator (`|`).
 *
 * `PAL_FLASH_STOP` is not a bit and must not be combined with other bits.
 *
 * All flash flags follow the format `PAL_FLASH_**` for consistency and
 * API use.
 *
 * @since 2.0
 */
typedef uint64_t PalFlashFlag;

/**
 * @typedef PalFBConfigBackend
 * @brief Represents the backend of a FBConfig.
 *
 * All FBConfig backends follow the format `PAL_CONFIG_BACKEND**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalFBConfigBackend;

/**
 * @typedef PalScancode
 * @brief scancodes (layout independent keys) of a keyboard.
 *
 * All scancodes follow the format `PAL_SCANCODE_**` for consistency and
 * API use.
 *
 * @since 2.0
 */
typedef uint32_t PalScancode;

/**
 * @typedef PalKeycode
 * @brief Keycodes (layout aware keys) of a keyboard.
 *
 * All keycodes follow the format `PAL_KEYCODE_**` for consistency and API
 * use.
 *
 * @since 2.0
 */
typedef uint32_t PalKeycode;

/**
 * @typedef PalMouseButton
 * @brief Buttons of a mouse.
 *
 * All mouse buttons follow the format `PAL_MOUSE_BUTTON_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalMouseButton;

/**
 * @typedef PalCursorType
 * @brief System cursor types.
 *
 * All cursor types follow the format `PAL_CURSOR_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalCursorType;

/**
 * @struct PalMonitorInfo
 * @brief Information about a monitor.
 *
 * @since 2.0
 */
typedef struct {
    int32_t x;       /**< X position in pixels.*/
    int32_t y;       /**< Y position in pixels.*/
    uint32_t width;  /**< Width in pixels.*/
    uint32_t height; /**< Height in pixels.*/
    uint32_t dpi;
    uint32_t refreshRate;
    PalOrientation orientation;
    PalBool primary; /**< True if this is the primary monitor.*/
    char name[PAL_MONITOR_NAME_SIZE];
} PalMonitorInfo;

/**
 * @struct PalMonitorMode
 * @brief information about a monitor display mode.
 *
 * @since 1.0
 */
typedef struct {
    uint32_t bpp; /**< Bits per pixel.*/
    uint32_t refreshRate;
    uint32_t width;  /**< Width in pixels.*/
    uint32_t height; /**< Height in pixels.*/
} PalMonitorMode;

/**
 * @struct PalFlashInfo
 * @brief Parameters for flashing a window to request focus.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalFlashFlag flags;   /**< See PalFlashFlag.*/
    uint32_t interval;    /**< In milliseconds. Set to 0 for default.*/
    uint32_t count;       /**< Set to 0 to flash until focused or cancelled.*/
} PalFlashInfo;

/**
 * @struct PalIconCreateInfo
 * @brief Creation parameters for an icon.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    const uint8_t* pixels; /**< Pixels in `RGBA` format.*/
    uint32_t width;        /**< Width in pixels.*/
    uint32_t height;       /**< Height in pixels.*/
} PalIconCreateInfo;

/**
 * @struct PalCursorCreateInfo
 * @brief Creation parameters for a cursor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    const uint8_t* pixels; /**< Pixels in `RGBA` format.*/
    uint32_t width;        /**< Width in pixels..*/
    uint32_t height;       /**< Height in pixels.*/
    int32_t xHotspot;      /**< X pixel for detecting clicks.*/
    int32_t yHotspot;      /**< Y pixel for detecting clicks.*/
} PalCursorCreateInfo;

/**
 * @struct PalWindowHandleInfo
 * @brief Information about a window handle.
 *
 * @since 2.0
 */
typedef struct {
    void* nativeDisplay; /**< The platform (OS) display or instance.*/
    void* nativeWindow;  /**< The window platform (OS) handle.*/
    void* nativeHandle1; /**< Extra window handle (xdgSurface)*/
    void* nativeHandle2; /**< Extra window handle (xdgToplevel)*/
    void* nativeHandle3; /**< Extra window handle (wl_egl_window)*/
} PalWindowHandleInfo;

/**
 * @struct PalWindowCreateInfo
 * @brief Creation parameters for a window.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalWindowStyle style;    /**< Window style.*/
    const char* title;       /**< Title in UTF-8 encoding.*/
    PalMonitor* monitor;     /**< Set to nullptr to use primary monitor.*/
    const char* appName;     /**< If nullptr, `PAL` will be used.*/
    const char* instanceName;  /**< If nullptr, `title` will be used.*/
    uint32_t width;          /**< Width in pixels.*/
    uint32_t height;         /**< Width in pixels.*/
    PalBool show;            /**< Show after creation.*/
    PalBool maximized;       /**< Maximize after creation.*/
    PalBool minimized;       /**< Minimze after creation.*/
    PalBool center;          /**< Center after creation.*/
} PalWindowCreateInfo;

/**
 * @brief Initialize the video system.
 *
 * This must be called before any video function. The video system must be
 * shutdown with palShutdownVideo() when no longer needed.
 *
 * The allocator will not not copied, therefore the pointer must remain valid
 * until the video system is shutdown. The event driver must be valid to recieve
 * video events.
 * 
 * If `preferredInstance` is nullptr, the video system creates one and control its lifetime.
 * The provided instance will not be freed by the video system.
 * `Linux`: This is the Display associated with the connection. 
 * `Windows`: This is the HINSTANCE of the process.
 *
 * @param[in] allocator Optional user-provided allocator. Set to nullptr to use
 * default.
 * @param[in] eventDriver Optional user-provided event driver. This is needed to
 * push video events. Set to nullptr to use default.
 * @param[in] preferredInstance User-provided instance (eg. HINSTANCE). Can be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 2.0
 * @sa palShutdownVideo
 */
PAL_API PalResult PAL_CALL palInitVideo(
    const PalAllocator* allocator,
    PalEventDriver* eventDriver,
    void* preferredInstance);

/**
 * @brief Shutdown the video system.
 *
 * If the video system has not been initialized, the function returns silently.
 * All created windows, icons, and cursors must be destroyed before this call.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 * @sa palInitVideo
 */
PAL_API void PAL_CALL palShutdownVideo();

/**
 * @brief Update the video system and all created windows.
 *
 * If the video system has not been initialized, the function returns silently.
 * This function pushes generated video events to the event driver set at
 * palInitVideo().
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 * @sa palInitVideo
 */
PAL_API void PAL_CALL palUpdateVideo();

/**
 * @brief Get the supported features of the video system.
 *
 * The video system must be initialized before this call.
 *
 * @return video features on success or `0` on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palInitVideo
 */
PAL_API PalVideoFeatures PAL_CALL palGetVideoFeatures();

/**
 * @brief Set the FBConfig for the video system.
 *
 * The video system must be initialized before this call.
 * The provided FBConfig will be used for all created windows after this call.
 * The `index` is the loop index from the drivers
 * supported FBConfigs.
 *
 * The `backend` is used to tell the video system, the source of the index.
 * Examples: PAL_CONFIG_BACKEND_EGL tells the video system, we got this loop
 * index from EGL. This will enable the video system to find your FBConfig.
 *
 * Example Flow:
 * Enumerate and select your FBConfig using any backend(EGL, GLX, WGL, etc)
 * and just let the video system know which one you used.
 *
 * If the backend passed is not the same as the one used,
 * the video system might still get a FBConfig but it will not be the
 * one requested.
 *
 * @param[in] index The FBConfig driver index.
 * @param[in] backend The FBConfig backend or source.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must be called from the main thread.
 *
 * @since 1.1
 */
PAL_API PalResult PAL_CALL palSetFBConfig(
    const int index,
    PalFBConfigBackend backend);

/**
 * @brief Return a list of all connected monitors.
 *
 * The video system must be initialized before this call.
 *
 * Call this function first with PalMonitor array set to nullptr to get the
 * number of connected monitors. Allocate memory for the PalMonitor
 * array and passed in the count and the allocated array. If the count of the
 * array is less than the number of connected monitors, PAL will write upto that
 * limit.
 *
 * The monitor handles must not be freed by the user, they are managed by the
 * platform (OS). Users are required to cache this, and call this function again
 * if monitors are added or removed.
 *
 * @param[in, out] count Capacity of the PalMonitor array.
 * @param[out] monitors User allocated array of PalMonitor.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 * @sa palGetPrimaryMonitor
 */
PAL_API PalResult PAL_CALL palEnumerateMonitors(
    int32_t* count,
    PalMonitor** outMonitors);

/**
 * @brief Get the primary connected monitor.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_MONITOR_GET_PRIMARY` must be supported.
 *
 * The monitor handle must not be freed by the user, they are managed by the
 * platform (OS).
 *
 * @param[out] outMonitor Pointer to a PalMonitor to recieve the primary
 * monitor.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 * @sa palEnumerateMonitors
 */
PAL_API PalResult PAL_CALL palGetPrimaryMonitor(PalMonitor** outMonitor);

/**
 * @brief Get information about a monitor.
 *
 * The video system must be initialized before this call.
 *
 * This function takes in a PalMonitorInfo and fills it.
 * Some of the fields are set to defaults if the operation is not supported on
 * the platform (OS). example: On Windows 7, DPI will always be 96.
 *
 * @param[in] monitor Monitor to query information on.
 * @param[out] info Pointer to a PalMonitorInfo to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must be called from the main thread.
 *
 * @since 1.0
 */
PAL_API PalResult PAL_CALL palGetMonitorInfo(
    PalMonitor* monitor,
    PalMonitorInfo* info);

/**
 * @brief Return a list of all supported monitor display modes for the provided
 * monitor.
 *
 * The video system must be initialized before this call.
 *
 * Call this function first with PalMonitorMode array set to nullptr to get the
 * number of supported monitor display modes. Allocate memory for the
 * PalMonitorMode array and passed in the count and the allocated array. If the
 * count of the array is less than the number of supported monitor display
 * modes, PAL will write upto that limit.
 *
 * @param[in] monitor Monitor to query display modes on.
 * @param[in, out] count Capacity of the PalMonitorMode array.
 * @param[out] modes User allocated array of PalMonitorMode.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 */
PAL_API PalResult PAL_CALL palEnumerateMonitorModes(
    PalMonitor* monitor,
    int32_t* count,
    PalMonitorMode* modes);

/**
 * @brief Get the current monitor display mode of the provided monitor.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_MONITOR_GET_MODE` must be supported.
 *
 * @param[in] monitor Monitor to query its current display mode.
 * @param[out] mode Pointer to a PalMonitorMode to recieve the current monitor.
 * mode.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 * @sa palSetMonitorMode
 */
PAL_API PalResult PAL_CALL palGetCurrentMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode);

/**
 * @brief Set the active display monitor mode of the provided monitor.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_MONITOR_SET_MODE` Must be supported.
 *
 * PAL only validates the monitor display mode pointer not the values. To be
 * safe, users must get the monitor mode from palEnumerateMonitorModes() or call
 * palValidateMonitorMode() to validate before switching.
 * palValidateMonitorMode() is not supported on all platforms.
 *
 * If the monitor display mode submitted is invalid, this function might fail
 * depending on the platform (OS).
 *
 * @param[in] monitor Monitor to set its current display mode.
 * @param[in] mode Pointer to a PalMonitorMode to set.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 * @sa palGetCurrentMonitorMode
 */
PAL_API PalResult PAL_CALL palSetMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode);

/**
 * @brief Check if a monitor display mode is valid on the provided monitor.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_MONITOR_VALIDATE_MODE` must be supported.
 *
 * @param[in] monitor The monitor.
 * @param[in] mode Pointer to a PalMonitorMode to validate.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 */
PAL_API PalResult PAL_CALL palValidateMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode);

/**
 * @brief Set the orientation for a monitor.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_MONITOR_SET_ORIENTATION` must be supported.
 *
 * This change is temporary and is reset when the platform (OS) reboots.
 *
 * @param[in] monitor Monitor to set its orientation.
 * @param[in] orientation The orientation to set.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 */
PAL_API PalResult PAL_CALL palSetMonitorOrientation(
    PalMonitor* monitor,
    PalOrientation orientation);

/**
 * @brief Create a window.
 *
 * The video system must be initialized before this call.
 *
 * @param[in] info Pointer to a PalWindowCreateInfo struct that specifies
 * parameters. Must not be nullptr.
 * @param[out] outWindow Pointer to a PalWindow to recieve the created
 * window. Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @note On Wayland
 *
 * - creating non resizable windows is not supported.
 * PAL will always creating resizable windows.
 *
 * - Creating windows on a specific monitor is not supported.
 *
 * - Creating hidden window is not supported. It will be ignored.
 *
 * @since 1.0
 */
PAL_API PalResult PAL_CALL palCreateWindow(
    const PalWindowCreateInfo* info,
    PalWindow** outWindow);

/**
 * @brief Destroy the provided window.
 *
 * The video system must be initialized before this call.
 * If the provided window is invalid or nullptr, this function returns
 * silently. This only destroys windows created by PAL.
 *
 * @param[in] window Pointer to the window to destroy.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 * @sa palCreateWindow
 */
PAL_API void PAL_CALL palDestroyWindow(PalWindow* window);

/**
 * @brief Minimize a maximized or restored window.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_SET_STATE` must be supported.
 * If the window is already minimized, this functions does nothing.
 *
 * @param[in] window Window to minimize.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 * @sa palMaximizeWindow
 * @sa palRestoreWindow
 */
PAL_API PalResult PAL_CALL palMinimizeWindow(PalWindow* window);

/**
 * @brief Maximize a minimized or restored window.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_SET_STATE` must be supported.
 * If the window is already maximized, this functions does nothing.
 *
 * @param[in] window Window to maximize.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 * @sa palMinimizeWindow
 * @sa palRestoreWindow
 */
PAL_API PalResult PAL_CALL palMaximizeWindow(PalWindow* window);

/**
 * @brief Restores a window to it previous state.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_SET_STATE` must be supported.
 * If the window is already restored, this functions does nothing.
 *
 * @param[in] window Window to restore.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @note Wayland does not support restoring a minimized windows.
 *
 * @since 1.0
 * @sa palMinimizeWindow
 * @sa palMaximizeWindow
 */
PAL_API PalResult PAL_CALL palRestoreWindow(PalWindow* window);

/**
 * @brief Show the provided window.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY` must be supported.
 * All windows are created hidden if not explicitly shown.
 * This does nothing if the window is already shown.
 *
 * @param[in] window Window to show.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 * @sa palHideWindow
 */
PAL_API PalResult PAL_CALL palShowWindow(PalWindow* window);

/**
 * @brief Hide the provided window.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY` must be supported.
 * This does nothing if the window is already hidden.
 *
 * @param[in] window Window to hide.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 * @sa palShowWindow
 */
PAL_API PalResult PAL_CALL palHideWindow(PalWindow* window);

/**
 * @brief Request the platform (OS) to visually flash the provided window.
 *
 * The video system must be initialized before this call.
 *
 * If `PAL_FLASH_CAPTION` is used, `PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION` must
 * be supported.
 *
 * If `PAL_FLASH_TRAY` is used, `PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY` must be
 * supported.
 *
 * @param[in] window Pointer to the window.
 * @param[in] info Pointer to a PalFlashInfo struct with flash parameters.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 */
PAL_API PalResult PAL_CALL palFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info);

/**
 * @brief Get the style of the provided window.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_GET_STYLE` must be supported.
 *
 * @param[in] window Pointer to the window.
 * @param[out] outStyle Pointer to a PalWindowStyle to recieve the window style.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 * @sa palSetWindowStyle
 */
PAL_API PalResult PAL_CALL palGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* outStyle);

/**
 * @brief Get the monitor the provided window is currently on.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_GET_MONITOR` must be supported.
 *
 * @param[in] window Pointer to the window.
 * @param[out] outMonitor Pointer to a PalMonitor to recieve the monitor.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 */
PAL_API PalResult PAL_CALL palGetWindowMonitor(
    PalWindow* window,
    PalMonitor** outMonitor);

/**
 * @brief Get the title of the provided window.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_GET_TITLE` must be supported.
 *
 * Set the buffer to nullptr to get the size of the window name in bytes.
 * If the size of the provided buffer is less than the actual size of window
 * title, PAL will write upto that limit.
 *
 * @param[in] window The window to query its title.
 * @param[in] bufferSize Size of the provided buffer in bytes.
 * @param[out] outSize The actual size of the window title in bytes.
 * @param[out] outBuffer Pointer to a user provided buffer to recieve the title.
 * Can be nullptr.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 * @sa palSetWindowTitle
 */
PAL_API PalResult PAL_CALL palGetWindowTitle(
    PalWindow* window,
    uint64_t bufferSize,
    uint64_t* outSize,
    char* outBuffer);

/**
 * @brief Get the position of the provided window in pixels.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_GET_POS` must be supported.
 *
 * @param[in] window Pointer to the window.
 * @param[out] x Pointer to recieve the window x position. Can be nullptr.
 * @param[out] y Pointer to recieve the window y position. Can be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 * @sa palSetWindowPos
 */
PAL_API PalResult PAL_CALL palGetWindowPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y);

/**
 * @brief Get the size of the provided window in pixels.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_GET_SIZE` must be supported.
 *
 * @param[in] window Pointer to the window.
 * @param[out] width Pointer to recieve the width. Can be nullptr.
 * @param[out] height Pointer to recieve the height. Can be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 * @sa palSetWindowSize
 */
PAL_API PalResult PAL_CALL palGetWindowSize(
    PalWindow* window,
    uint32_t* width,
    uint32_t* height);

/**
 * @brief Get the state of the provided window.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_GET_STATE` must be supported.
 *
 * @param[in] window Pointer to the window.
 * @param[out] outState Pointer to a PalWindowState to recieve the window state.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 */
PAL_API PalResult PAL_CALL palGetWindowState(
    PalWindow* window,
    PalWindowState* outState);

/**
 * @brief Get the state of the keycodes (layout aware keys) of the
 * keyboard.
 *
 * The video system must be initialized before this call.
 *
 * The returned pointer must not be freed. The state is updated when
 * palUpdateVideo() is called. The array must be index with PalKeycodes and
 * not exceed `PAL_KEYCODE_MAX`.
 *
 * @return A pointer to the keycodes array on success or nullptr on failure.
 *
 * Thread safety: Thread-safe.
 *
 * @since 1.0
 */
PAL_API const PalBool* PAL_CALL palGetKeycodeState();

/**
 * @brief Get the state of the scancodes (layout independent keys) of
 * the keyboard.
 *
 * The video system must be initialized before this call.
 *
 * The returned pointer must not be freed. The state is updated when
 * palUpdateVideo() is called. The array must be index with PalScancodes and
 * not exceed PAL_SCANCODE_MAX.
 *
 * @return A pointer to the scancodes array on success or nullptr on failure.
 *
 * Thread safety: Thread-safe.
 *
 * @since 1.0
 */
PAL_API const PalBool* PAL_CALL palGetScancodeState();

/**
 * @brief Get the state of the buttons of the mouse.
 *
 * The video system must be initialized before this call.
 *
 * The returned pointer must not be freed. The state is updated when
 * palUpdateVideo() is called. The array must be index with PalMouseButton and
 * not exceed `PAL_MOUSE_BUTTON_MAX`.
 *
 * @return A pointer to the mouse button array on success or nullptr on failure.
 *
 * @Thread safety: Thread-safe.
 *
 * @since 1.0
 */
PAL_API const PalBool* PAL_CALL palGetMouseState();

/**
 * @brief Get the relative movement of the mouse in desktop pixels.
 *
 * The video system must be initialized before this call.
 * The relative movement will be updated when palUpdateVideo() is called.
 *
 * @param[in] dx Pointer to recieve the mouse relative movement x. Can be
 * nullptr.
 * @param[in] dy Pointer to recieve the mouse relative movement y. Can be
 * nullptr.
 *
 * Thread safety: Thread-safe if `dx` and `dy` are thread
 * local.
 *
 * @since 2.0
 */
PAL_API void PAL_CALL palGetMouseDelta(
    float* dx,
    float* dy);

/**
 * @brief Get the wheel delta of the mouse.
 *
 * The video system must be initialized before this call.
 * The wheel delta will be updated when palUpdateVideo() is called.
 *
 * @param[in] dx Pointer to recieve the mouse wheel delta x. Can be nullptr.
 * @param[in] dy Pointer to recieve the mouse wheel delta y. Can be nullptr.
 *
 * Thread safety: Thread-safe if `dx` and `dy` are thread
 * local.
 *
 * @since 2.0
 */
PAL_API void PAL_CALL palGetMouseWheelDelta(
    float* dx,
    float* dy);

/**
 * @brief Check if the provided window is visible.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_GET_VISIBILITY` must be supported.
 *
 * @param[in] window Pointer to the window.
 *
 * @return `true` if the window is visible otherwise `PAL_FALSE`.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 */
PAL_API PalBool PAL_CALL palIsWindowVisible(PalWindow* window);

/**
 * @brief Get the current input-focused window per application.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_GET_INPUT_FOCUS` must be supported.
 *
 * @return The current input-focused window on success or nullptr on
 * failure.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 */
PAL_API PalWindow* PAL_CALL palGetFocusWindow();

/**
 * @brief Get the native handle of the provided window.
 *
 * The video system must be initialized before this call.
 * 
 * On Wayland: `::nativeHandle1`, `::nativeHandle2` and `::nativeHandle3` 
 * are `xdg_surface`, `xdg_toplevel` and `wl_egl_window` respectively if available.
 *
 * @param[in] window Pointer to the window.
 * @param[out] info Pointer to a PalWindowHandleInfo to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread-safe.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palGetWindowHandleInfo(
    PalWindow* window, 
    PalWindowHandleInfo* info);

/**
 * @brief Set the opacity of the provided window.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW` must be supported.
 * The window must have `PAL_WINDOW_STYLE_TRANSPARENT` style.
 *
 * @param[in] window Pointer to the window.
 * @param[in] opacity Must be in the range 0.0 - 1.0.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must be called from the main thread.
 *
 * @since 1.0
 */
PAL_API PalResult PAL_CALL palSetWindowOpacity(
    PalWindow* window,
    float opacity);

/**
 * @brief Set the style of the provided window.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_SET_STYLE` must be supported.
 *
 * @param[in] window Pointer to the window.
 * @param[in] style The style to set.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must be called from the main thread.
 *
 * @since 1.0
 * @sa palGetWindowStyle
 */
PAL_API PalResult PAL_CALL palSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style);

/**
 * @brief Set the title of the provided window.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_SET_TITLE` must be supported.
 * The title must be a UTF-8 encoding null terminated string.
 *
 * @param[in] window Pointer to the window.
 * @param[in] title UTF-8 encoding null terminated string.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must be called from the main thread.
 *
 * @since 1.0
 * @sa palGetWindowTitle
 */
PAL_API PalResult PAL_CALL palSetWindowTitle(
    PalWindow* window,
    const char* title);

/**
 * @brief Set the position of the provided window in pixels.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_SET_POS` must be supported.
 *
 * @param[in] window Pointer to the window.
 * @param[in] x The new x coordinate in pixels.
 * @param[in] y The new y coordinate in pixels.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must be called from the main thread.
 *
 * @since 1.0
 * @sa palGetWindowPos
 */
PAL_API PalResult PAL_CALL palSetWindowPos(
    PalWindow* window,
    int32_t x,
    int32_t y);

/**
 * @brief Set the size of the provided window in pixels.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_SET_SIZE` must be supported.
 *
 * @param[in] window Pointer to the window.
 * @param[in] width The new width of the window in pixels. Must be greater than
 * zero.
 * @param[in] height The new height of the window in pixels. Must be greater
 * than zero.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must be called from the main thread.
 *
 * @since 1.0
 * @sa palGetWindowSize
 */
PAL_API PalResult PAL_CALL palSetWindowSize(
    PalWindow* window,
    uint32_t width,
    uint32_t height);

/**
 * @brief Request input focus for the provided window.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_SET_INPUT_FOCUS` must be supported.
 *
 * @param[in] window Pointer to the window.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must be called from the main thread.
 *
 * @since 1.0
 * @sa palGetFocusWindow
 */
PAL_API PalResult PAL_CALL palSetFocusWindow(PalWindow* window);

/**
 * @brief Create an icon.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_SET_ICON` must be supported.
 *
 * @param[in] info Pointer to a PalIconCreateInfo struct that specifies
 * parameters. Must not be nullptr.
 * @param[out] outIcon Pointer to a PalIcon to recieve the created
 * icon. Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 * @sa palDestroyIcon
 */
PAL_API PalResult PAL_CALL palCreateIcon(
    const PalIconCreateInfo* info,
    PalIcon** outIcon);

/**
 * @brief Destroy the provided icon.
 *
 * The video system must be initialized before this call.
 *
 * If the provided icon is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] icon Pointer to the icon to destroy.
 *
 * Thread safety: Must be called from the main thread.
 *
 * @since 1.0
 * @sa palCreateIcon
 */
PAL_API void PAL_CALL palDestroyIcon(PalIcon* icon);

/**
 * @brief Set the icon for the provided window.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_SET_ICON` must be supported.
 *
 * @param[in] window Pointer to the window.
 * @param[in] icon Pointer to the icon. Set to nullptr to revert.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 */
PAL_API PalResult PAL_CALL palSetWindowIcon(
    PalWindow* window,
    PalIcon* icon);

/**
 * @brief Create a cursor.
 *
 * The video system must be initialized before this call.
 *
 * @param[in] info Pointer to a PalCursorCreateInfo struct that specifies
 * parameters. Must not be nullptr.
 * @param[out] outCursor Pointer to a PalCursor to recieve the created
 * cursor. Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.0
 * @sa palDestroyCursor
 */
PAL_API PalResult PAL_CALL palCreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** outCursor);

/**
 * @brief Create a system cursor.
 *
 * The video system must be initialized before this call.
 *
 * @param[in] type The system cursor type to create. Must not be nullptr.
 * @param[out] outCursor Pointer to a PalCursor to recieve the created
 * cursor. Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.1
 * @sa palDestroyCursor
 */
PAL_API PalResult PAL_CALL palCreateCursorFrom(
    PalCursorType type,
    PalCursor** outCursor);

/**
 * @brief Destroy the provided cursor.
 *
 * The video system must be initialized before this call.
 *
 * If the provided icon is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] cursor Pointer to the cursor to destroy.
 *
 * Thread safety: Must be called from the main thread.
 *
 * @since 1.0
 * @sa palCreateCursor
 */
PAL_API void PAL_CALL palDestroyCursor(PalCursor* cursor);

/**
 * @brief Show or hide the cursor.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_CURSOR_SET_VISIBILITY` must be supported.
 *
 * This affects all created cursors since the platform (OS) merges all cursors
 * into a single one on the screen.
 *
 * @param[in] show True to make the cursor visible otherwise `PAL_FALSE`.
 *
 * Thread safety: Must be called from the main thread.
 *
 * @since 1.0
 */
PAL_API void PAL_CALL palShowCursor(PalBool show);

/**
 * @brief Clip the cursor to the provided window.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_CLIP_CURSOR` must be supported.
 *
 * If the window is destroyed without unclipping the cursor, this cursor might
 * not reset depending on the platform (OS). To be safe, unclip the cursor from
 * the window before destroying the window.
 *
 * @param[in] window Pointer to the window.
 * @param[in] clip True to clip to window or `PAL_FALSE` to unclip.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must be called from the main thread.
 *
 * @since 1.0
 */
PAL_API PalResult PAL_CALL palClipCursor(
    PalWindow* window,
    PalBool clip);

/**
 * @brief Get the position of the cursor relative to the provided window in
 * pixels.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_CURSOR_GET_POS` must be supported.
 *
 * @param[in] window Pointer to the window.
 * @param[out] x Pointer to recieve the x position. Can be
 * nullptr.
 * @param[out] y Pointer to recieve the y position. Can be
 * nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must be called from the main thread.
 *
 * @since 1.0
 */
PAL_API PalResult PAL_CALL palGetCursorPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y);

/**
 * @brief Set the position of the cursor relative to the provided window in
 * pixels.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_CURSOR_SET_POS` must be supported.
 *
 * @param[in] window Pointer to the window.
 * @param[in] x The new x coordinate of the cursor in pixels.
 * @param[in] y The new y coordinate of the cursor in pixels.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must be called from the main thread.
 *
 * @since 1.0
 */
PAL_API PalResult PAL_CALL palSetCursorPos(
    PalWindow* window,
    int32_t x,
    int32_t y);

/**
 * @brief Set the cursor for the provided window.
 *
 * The video system must be initialized before this call.
 *
 * @param[in] window Pointer to the window.
 * @param[in] cursor Pointer to the cursor. Set to nullptr to revert.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must be called from the main thread.
 *
 * @since 1.0
 */
PAL_API PalResult PAL_CALL palSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor);

/**
 * @brief Get the native application instance or display.
 *
 * The video system must be initialized before this call.
 *
 * This returns the native instance or display of the application
 * PAL video was initialized in.
 *
 * On Linux: This is the Display associated with the connection.

 * On Windows: This is the HINSTANCE of the process.
 *
 * @return The instance or display on success or nullptr on failure.
 *
 * Thread safety: Thread safe.
 *
 * @note The returned instance or display must not be freed.
 *
 * @since 1.2
 */
PAL_API void* PAL_CALL palGetInstance();

/**
 * @brief Attach a foreign or native window to PAL video system.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_FOREIGN_WINDOWS` must be supported.
 *
 * This function registers the provided window with PAL video system so it
 * can manage events and use its functionality/API for the provided window.
 *
 * PAL does not own the window, it just sends events to that window.
 * Users are responsible for destroying the window when no longer needed.
 * palDestroyWindow() does not destroy the foreign or native window.
 *
 * Use Case:
 *
 * PAL takes your native foreign or native window and gives you a PalWindow
 * which can be used with all of PAL API. The native window must be valid
 * till the PalWindow has been detached with palDetachWindow().
 *
 * The window must be created with the same instance or display
 * that PAL uses. see palGetInstance().
 *
 * @param[in] windowHandle Pointer to the foreign or native window.
 * @param[out] outWindow Pointer to a PalWindow to recieve the attached window.
 * Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must be called from the main thread.
 *
 * @since 1.2
 * @sa palGetInstance
 * @sa palDestroyWindow
 * @sa palDetachWindow
 */
PAL_API PalResult PAL_CALL palAttachWindow(
    void* windowHandle,
    PalWindow** outWindow);

/**
 * @brief Detach a foreign or native window from PAL video system.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_FOREIGN_WINDOWS` must be supported.
 *
 * This function unregisters the provided window from PAL video system.
 * The window must not be owned by PAL otherwise the function fails
 * and return `PAL_RESULT_INVALID_WINDOW`.
 *
 * Detaching the window does not destroy the window,
 * therefore destroying the window is the users responsibility.
 *
 * Use Case:
 *
 * Give back the PalWindow returned at palAttachWindow()
 * and get back your native window.
 *
 * @param[in] window Pointer to the PalWindow to detach. Must not be nullptr.
 * @param[out] outWindowHandle Pointer to recieve the native window. Can be
 * nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must be called from the main thread.
 *
 * @since 1.2
 * @sa palAttachWindow
 */
PAL_API PalResult PAL_CALL palDetachWindow(
    PalWindow* window,
    void** outWindowHandle);

/** @} */ // end of pal_video group

#endif // _PAL_VIDEO_H
