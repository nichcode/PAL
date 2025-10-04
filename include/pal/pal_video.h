
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
 * @defgroup pal_video Video
 * Video PAL functionality such as windows, cursors, monitors and icons.
 *
 * @{
 */

#ifndef _PAL_VIDEO_H
#define _PAL_VIDEO_H

#include "pal_event.h"

/**
 * @struct PalMonitor
 * @brief Opaque handle to a monitor.
 *
 * @since 1.0
 * @ingroup pal_video
 */
typedef struct PalMonitor PalMonitor;

/**
 * @struct PalWindow
 * @brief Opaque handle to a window.
 *
 * @since 1.0
 * @ingroup pal_video
 */
typedef struct PalWindow PalWindow;

/**
 * @struct PalIcon
 * @brief Opaque handle to an icon.
 *
 * @since 1.0
 * @ingroup pal_video
 */
typedef struct PalIcon PalIcon;

/**
 * @struct PalCursor
 * @brief Opaque handle to a cursor.
 *
 * @since 1.0
 * @ingroup pal_video
 */
typedef struct PalCursor PalCursor;

/**
 * @enum PalVideoFeatures
 * @brief Video system features.
 *
 * All video features follow the format `PAL_VIDEO_FEATURE_**` for
 * consistency and API use.
 *
 * @since 1.0
 * @ingroup pal_video
 */
typedef enum {
    PAL_VIDEO_FEATURE_HIGH_DPI = PAL_BIT(0),
    PAL_VIDEO_FEATURE_MONITOR_SET_ORIENTATION = PAL_BIT(1),
    PAL_VIDEO_FEATURE_MONITOR_GET_ORIENTATION = PAL_BIT(2),
    PAL_VIDEO_FEATURE_BORDERLESS_WINDOW = PAL_BIT(3),
    PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW = PAL_BIT(4),
    PAL_VIDEO_FEATURE_TOOL_WINDOW = PAL_BIT(5),
    PAL_VIDEO_FEATURE_MONITOR_SET_MODE = PAL_BIT(6),
    PAL_VIDEO_FEATURE_MONITOR_GET_MODE = PAL_BIT(7),
    PAL_VIDEO_FEATURE_MULTI_MONITORS = PAL_BIT(8),
    PAL_VIDEO_FEATURE_WINDOW_SET_SIZE = PAL_BIT(9),
    PAL_VIDEO_FEATURE_WINDOW_GET_SIZE = PAL_BIT(10),
    PAL_VIDEO_FEATURE_WINDOW_SET_POS = PAL_BIT(11),
    PAL_VIDEO_FEATURE_WINDOW_GET_POS = PAL_BIT(12),
    PAL_VIDEO_FEATURE_WINDOW_SET_STATE = PAL_BIT(13),
    PAL_VIDEO_FEATURE_WINDOW_GET_STATE = PAL_BIT(14),
    PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY = PAL_BIT(15),
    PAL_VIDEO_FEATURE_WINDOW_GET_VISIBILITY = PAL_BIT(16),
    PAL_VIDEO_FEATURE_WINDOW_SET_TITLE = PAL_BIT(17),
    PAL_VIDEO_FEATURE_WINDOW_GET_TITLE = PAL_BIT(18),
    PAL_VIDEO_FEATURE_NO_MAXIMIZEBOX = PAL_BIT(19),
    PAL_VIDEO_FEATURE_NO_MINIMIZEBOX = PAL_BIT(20),
    PAL_VIDEO_FEATURE_CLIP_CURSOR = PAL_BIT(21),
    PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION = PAL_BIT(22),
    PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY = PAL_BIT(23),
    PAL_VIDEO_FEATURE_WINDOW_FLASH_INTERVAL = PAL_BIT(24),
    PAL_VIDEO_FEATURE_WINDOW_SET_INPUT_FOCUS = PAL_BIT(25),
    PAL_VIDEO_FEATURE_WINDOW_GET_INPUT_FOCUS = PAL_BIT(26),
    PAL_VIDEO_FEATURE_WINDOW_SET_STYLE = PAL_BIT(27),
    PAL_VIDEO_FEATURE_WINDOW_GET_STYLE = PAL_BIT(28),
    PAL_VIDEO_FEATURE_CURSOR_SET_POS = PAL_BIT(29),
    PAL_VIDEO_FEATURE_CURSOR_GET_POS = PAL_BIT(30),
} PalVideoFeatures;

/**
 * @enum PalOrientation
 * @brief Orientation types for a monitor.
 *
 * All orientation types follow the format `PAL_ORIENTATION_**` for consistency
 * and API use.
 *
 * @since 1.0
 * @ingroup pal_video
 */
typedef enum {
    PAL_ORIENTATION_LANDSCAPE,
    PAL_ORIENTATION_PORTRAIT,
    PAL_ORIENTATION_LANDSCAPE_FLIPPED,
    PAL_ORIENTATION_PORTRAIT_FLIPPED
} PalOrientation;

/**
 * @enum PalWindowStyle
 * @brief Window styles. Multiple styles can be OR'ed together using bitwise
 * OR operator (`|`).
 *
 * All window flags follow the format `PAL_WINDOW_STYLE_**` for
 * consistency and API use.
 *
 * @since 1.0
 * @ingroup pal_video
 */
typedef enum {
    PAL_WINDOW_STYLE_RESIZABLE = PAL_BIT(0),
    PAL_WINDOW_STYLE_TRANSPARENT = PAL_BIT(1),
    PAL_WINDOW_STYLE_TOPMOST = PAL_BIT(2),
    PAL_WINDOW_STYLE_NO_MINIMIZEBOX = PAL_BIT(3),
    PAL_WINDOW_STYLE_NO_MAXIMIZEBOX = PAL_BIT(4),
    PAL_WINDOW_STYLE_TOOL = PAL_BIT(5),
    PAL_WINDOW_STYLE_BORDERLESS = PAL_BIT(6)
} PalWindowStyle;

/**
 * @enum PalWindowState
 * @brief Represents the current state of a window.
 *
 * All window states follow the format `PAL_WINDOW_STATE_**` for consistency and
 * API use.
 *
 * @since 1.0
 * @ingroup pal_video
 */
typedef enum {
    PAL_WINDOW_STATE_MAXIMIZED,
    PAL_WINDOW_STATE_MINIMIZED,
    PAL_WINDOW_STATE_RESTORED
} PalWindowState;

/**
 * @enum PalFlashFlag
 * @brief Flash flags. Multiple flash flags can be OR'ed together using bitwise
 * OR operator (`|`).
 *
 * `PAL_FLASH_STOP` is not a bit and must not be combined with other bits.
 *
 * All flash flags follow the format `PAL_FLASH_**` for consistency and
 * API use.
 *
 * @since 1.0
 * @ingroup pal_video
 */
typedef enum {
    PAL_FLASH_STOP = 0,             /**< Stop flashing.*/
    PAL_FLASH_CAPTION = PAL_BIT(0), /**< Flash the titlebar of the window.*/
    PAL_FLASH_TRAY = PAL_BIT(1)     /**< Flash the icon of the window.*/
} PalFlashFlag;

/**
 * @enum PalScancode
 * @brief scancodes (layout independent keys) of a keyboard.
 *
 * All scancodes follow the format `PAL_SCANCODE_**` for consistency and
 * API use.
 *
 * @since 1.0
 * @ingroup pal_video
 */
typedef enum {
    PAL_SCANCODE_UNKNOWN = 0,

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

    PAL_SCANCODE_LEFT,
    PAL_SCANCODE_RIGHT,
    PAL_SCANCODE_UP,
    PAL_SCANCODE_DOWN,

    PAL_SCANCODE_INSERT,
    PAL_SCANCODE_DELETE,
    PAL_SCANCODE_HOME,
    PAL_SCANCODE_END,
    PAL_SCANCODE_PAGEUP,
    PAL_SCANCODE_PAGEDOWN,

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
 * @enum PalKeycode
 * @brief Keycodes (layout aware keys) of a keyboard.
 *
 * All keycodes follow the format `PAL_KEYCODE_**` for consistency and API
 * use.
 *
 * @since 1.0
 * @ingroup pal_video
 */
typedef enum {
    PAL_KEYCODE_UNKNOWN = 0,

    PAL_KEYCODE_A,
    PAL_KEYCODE_B,
    PAL_KEYCODE_C,
    PAL_KEYCODE_D,
    PAL_KEYCODE_E,
    PAL_KEYCODE_F,
    PAL_KEYCODE_G,
    PAL_KEYCODE_H,
    PAL_KEYCODE_I,
    PAL_KEYCODE_J,
    PAL_KEYCODE_K,
    PAL_KEYCODE_L,
    PAL_KEYCODE_M,
    PAL_KEYCODE_N,
    PAL_KEYCODE_O,
    PAL_KEYCODE_P,
    PAL_KEYCODE_Q,
    PAL_KEYCODE_R,
    PAL_KEYCODE_S,
    PAL_KEYCODE_T,
    PAL_KEYCODE_U,
    PAL_KEYCODE_V,
    PAL_KEYCODE_W,
    PAL_KEYCODE_X,
    PAL_KEYCODE_Y,
    PAL_KEYCODE_Z,

    PAL_KEYCODE_0,
    PAL_KEYCODE_1,
    PAL_KEYCODE_2,
    PAL_KEYCODE_3,
    PAL_KEYCODE_4,
    PAL_KEYCODE_5,
    PAL_KEYCODE_6,
    PAL_KEYCODE_7,
    PAL_KEYCODE_8,
    PAL_KEYCODE_9,

    PAL_KEYCODE_F1,
    PAL_KEYCODE_F2,
    PAL_KEYCODE_F3,
    PAL_KEYCODE_F4,
    PAL_KEYCODE_F5,
    PAL_KEYCODE_F6,
    PAL_KEYCODE_F7,
    PAL_KEYCODE_F8,
    PAL_KEYCODE_F9,
    PAL_KEYCODE_F10,
    PAL_KEYCODE_F11,
    PAL_KEYCODE_F12,

    PAL_KEYCODE_ESCAPE,
    PAL_KEYCODE_ENTER,
    PAL_KEYCODE_TAB,
    PAL_KEYCODE_BACKSPACE,
    PAL_KEYCODE_SPACE,
    PAL_KEYCODE_CAPSLOCK,
    PAL_KEYCODE_NUMLOCK,
    PAL_KEYCODE_SCROLLLOCK,
    PAL_KEYCODE_LSHIFT,
    PAL_KEYCODE_RSHIFT,
    PAL_KEYCODE_LCTRL,
    PAL_KEYCODE_RCTRL,
    PAL_KEYCODE_LALT,
    PAL_KEYCODE_RALT,

    PAL_KEYCODE_LEFT,
    PAL_KEYCODE_RIGHT,
    PAL_KEYCODE_UP,
    PAL_KEYCODE_DOWN,

    PAL_KEYCODE_INSERT,
    PAL_KEYCODE_DELETE,
    PAL_KEYCODE_HOME,
    PAL_KEYCODE_END,
    PAL_KEYCODE_PAGEUP,
    PAL_KEYCODE_PAGEDOWN,

    PAL_KEYCODE_KP_0,
    PAL_KEYCODE_KP_1,
    PAL_KEYCODE_KP_2,
    PAL_KEYCODE_KP_3,
    PAL_KEYCODE_KP_4,
    PAL_KEYCODE_KP_5,
    PAL_KEYCODE_KP_6,
    PAL_KEYCODE_KP_7,
    PAL_KEYCODE_KP_8,
    PAL_KEYCODE_KP_9,
    PAL_KEYCODE_KP_ENTER,
    PAL_KEYCODE_KP_ADD,
    PAL_KEYCODE_KP_SUBTRACT,
    PAL_KEYCODE_KP_MULTIPLY,
    PAL_KEYCODE_KP_DIVIDE,
    PAL_KEYCODE_KP_DECIMAL,
    PAL_KEYCODE_KP_EQUAL,

    PAL_KEYCODE_PRINTSCREEN,
    PAL_KEYCODE_PAUSE,
    PAL_KEYCODE_MENU,
    PAL_KEYCODE_APOSTROPHE,
    PAL_KEYCODE_BACKSLASH,
    PAL_KEYCODE_COMMA,
    PAL_KEYCODE_EQUAL,
    PAL_KEYCODE_GRAVEACCENT,
    PAL_KEYCODE_SUBTRACT,
    PAL_KEYCODE_PERIOD,
    PAL_KEYCODE_SEMICOLON,
    PAL_KEYCODE_SLASH,
    PAL_KEYCODE_LBRACKET,
    PAL_KEYCODE_RBRACKET,
    PAL_KEYCODE_LSUPER,
    PAL_KEYCODE_RSUPER,

    PAL_KEYCODE_MAX
} PalKeycode;

/**
 * @enum PalMouseButton
 * @brief Buttons of a mouse.
 *
 * All mouse buttons follow the format `PAL_MOUSE_BUTTON_**` for
 * consistency and API use.
 *
 * @since 1.0
 * @ingroup pal_video
 */
typedef enum {
    PAL_MOUSE_BUTTON_UNKNOWN = 0,

    PAL_MOUSE_BUTTON_LEFT,
    PAL_MOUSE_BUTTON_RIGHT,
    PAL_MOUSE_BUTTON_MIDDLE,
    PAL_MOUSE_BUTTON_X1,
    PAL_MOUSE_BUTTON_X2,

    PAL_MOUSE_BUTTON_MAX
} PalMouseButton;

/**
 * @enum PalCursorType
 * @brief System cursor types.
 *
 * All cursor types follow the format `PAL_CURSOR_**` for
 * consistency and API use.
 *
 * @since 1.1
 * @ingroup pal_video
 */
typedef enum {
    PAL_CURSOR_ARROW,
    PAL_CURSOR_HAND,
    PAL_CURSOR_CROSS,
    PAL_CURSOR_IBEAM,
    PAL_CURSOR_WAIT,

    PAL_CURSOR_MAX
} PalCursorType;

/**
 * @struct PalMonitorInfo
 * @brief Information about a monitor.
 *
 * @since 1.0
 * @ingroup pal_video
 */
typedef struct {
    bool primary; /**< True if this is the primary monitor.*/
    Uint32 dpi;
    Uint32 refreshRate;
    Int32 x;       /**< X position in pixels.*/
    Int32 y;       /**< Y position in pixels.*/
    Uint32 width;  /**< Width in pixels.*/
    Uint32 height; /**< Height in pixels.*/
    PalOrientation orientation;
    char name[32];
} PalMonitorInfo;

/**
 * @struct PalMonitorMode
 * @brief information about a monitor display mode.
 *
 * @since 1.0
 * @ingroup pal_video
 */
typedef struct {
    Uint32 bpp; /**< Bits per pixel.*/
    Uint32 refreshRate;
    Uint32 width;  /**< Width in pixels.*/
    Uint32 height; /**< Height in pixels.*/
} PalMonitorMode;

/**
 * @struct PalFlashInfo
 * @brief Parameters for flashing a window to request focus.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.0
 * @ingroup pal_video
 */
typedef struct {
    Uint32 interval;    /**< In milliseconds. Set to 0 for default.*/
    PalFlashFlag flags; /**< See PalFlashFlag.*/
    Uint32 count;       /**< Set to 0 to flash until focused or cancelled.*/
} PalFlashInfo;

/**
 * @struct PalIconCreateInfo
 * @brief Creation parameters for an icon.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.0
 * @ingroup pal_video
 */
typedef struct {
    Uint32 width;        /**< Width in pixels.*/
    Uint32 height;       /**< Height in pixels.*/
    const Uint8* pixels; /**< Pixels in `RGBA` format.*/
} PalIconCreateInfo;

/**
 * @struct PalCursorCreateInfo
 * @brief Creation parameters for a cursor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.0
 * @ingroup pal_video
 */
typedef struct {
    Uint32 width;        /**< Width in pixels..*/
    Uint32 height;       /**< Height in pixels.*/
    Int32 xHotspot;      /**< X pixel for detecting clicks.*/
    Int32 yHotspot;      /**< Y pixel for detecting clicks.*/
    const Uint8* pixels; /**< Pixels in `RGBA` format.*/
} PalCursorCreateInfo;

/**
 * @struct PalWindowHandleInfo
 * @brief Information about a window handle.
 *
 * @since 1.0
 * @ingroup pal_video
 */
typedef struct {
    void* nativeDisplay; /**< The platform (OS) display.*/
    void* nativeWindow;  /**< The platform (OS) handle.*/
} PalWindowHandleInfo;

/**
 * @struct PalWindowCreateInfo
 * @brief Creation parameters for a window.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.0
 * @ingroup pal_video
 */
typedef struct {
    bool show;            /**< Show after creation.*/
    bool maximized;       /**< Maximize after creation.*/
    bool minimized;       /**< Minimze after creation.*/
    bool center;          /**< Center after creation.*/
    Uint32 width;         /**< Width in pixels.*/
    Uint32 height;        /**< Width in pixels.*/
    PalWindowStyle style; /**< Window style.*/
    const char* title;    /**< Title in UTF-8 encoding.*/
    PalMonitor* monitor;  /**< Set to nullptr to use primary monitor.*/
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
 * @param[in] allocator Optional user-provided allocator. Set to nullptr to use
 * default.
 * @param[in] eventDriver Optional user-provided event driver. This is needed to
 * push video events. Set to nullptr to use default.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 * @sa palShutdownVideo
 */
PAL_API PalResult PAL_CALL palInitVideo(
    const PalAllocator* allocator,
    PalEventDriver* eventDriver);

/**
 * @brief Shutdown the video system.
 *
 * If the video system has not been initialized, the function returns silently.
 * All created windows, icons, and cursors must be destroyed before this call.
 *
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * Thread safety: This function is thread safe.
 *
 * @since 1.0
 * @ingroup pal_video
 * @sa palInitVideo
 */
PAL_API PalVideoFeatures PAL_CALL palGetVideoFeatures();

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
 * If the count is 0 and the PalMonitor array is nullptr, the function fails
 * and returns `PAL_RESULT_INSUFFICIENT_BUFFER`.
 *
 * The monitor handles must not be freed by the user, they are managed by the
 * platform (OS). Users are required to cache this, and call this function again
 * if monitors are added or removed.
 *
 * @param[in] count Capacity of the PalMonitor array.
 * @param[out] monitors User allocated array of PalMonitor.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 * @sa palGetPrimaryMonitor
 */
PAL_API PalResult PAL_CALL palEnumerateMonitors(
    Int32* count,
    PalMonitor** outMonitors);

/**
 * @brief Get the primary connected monitor.
 *
 * The video system must be initialized before this call.
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
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * Thread safety: This function must be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * If the count is 0 and the PalMonitorMode array is nullptr, the function fails
 * and returns `PAL_RESULT_INSUFFICIENT_BUFFER`.
 *
 * @param[in] monitor Monitor to query display modes on.
 * @param[in] count Capacity of the PalMonitorMode array.
 * @param[out] modes User allocated array of PalMonitorMode.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 */
PAL_API PalResult PAL_CALL palEnumerateMonitorModes(
    PalMonitor* monitor,
    Int32* count,
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
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 * @sa palGetCurrentMonitorMode
 */
PAL_API PalResult PAL_CALL palSetMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode);

/**
 * @brief Check if a monitor display mode is valid on the provided monitor.
 *
 * The video system must be initialized before this call.
 *
 * @param[in] monitor The monitor.
 * @param[in] mode Pointer to a PalMonitorMode to validate.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * paramters. Must not be nullptr.
 * @param[out] outWindow Pointer to a PalWindow to recieve the created
 * window. Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 */
PAL_API PalResult PAL_CALL palCreateWindow(
    const PalWindowCreateInfo* info,
    PalWindow** outWindow);

/**
 * @brief Destroy the provided window.
 *
 * The video system must be initialized before this call.
 * If the provided window is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] window Pointer to the window to destroy.
 *
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * @param[in] info Pointer to a PalFlashInfo struct with flash paramters.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 * @sa palSetWindowStyle
 */
PAL_API PalResult PAL_CALL palGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* outStyle);

/**
 * @brief Get the monitor the provided window is currently on.
 *
 * The video system must be initialized before this call.
 *
 * @param[in] window Pointer to the window.
 * @param[out] outMonitor Pointer to a PalMonitor to recieve the monitor.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 * @sa palSetWindowTitle
 */
PAL_API PalResult PAL_CALL palGetWindowTitle(
    PalWindow* window,
    Uint64 bufferSize,
    Uint64* outSize,
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
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 * @sa palSetWindowPos
 */
PAL_API PalResult PAL_CALL palGetWindowPos(
    PalWindow* window,
    Int32* x,
    Int32* y);

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
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 * @sa palSetWindowSize
 */
PAL_API PalResult PAL_CALL palGetWindowSize(
    PalWindow* window,
    Uint32* width,
    Uint32* height);

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
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * Thread safety: This function is thread-safe.
 *
 * @since 1.0
 * @ingroup pal_video
 */
PAL_API const bool* PAL_CALL palGetKeycodeState();

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
 * Thread safety: This function is thread-safe.
 *
 * @since 1.0
 * @ingroup pal_video
 */
PAL_API const bool* PAL_CALL palGetScancodeState();

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
 * @Thread safety: This function is thread-safe.
 *
 * @since 1.0
 * @ingroup pal_video
 */
PAL_API const bool* PAL_CALL palGetMouseState();

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
 * Thread safety: This function is thread-safe if `dx` and `dy` are thread
 * local.
 *
 * @since 1.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palGetMouseDelta(
    Int32* dx,
    Int32* dy);

/**
 * @brief Get the wheel delta of the mouse.
 *
 * The video system must be initialized before this call.
 * The wheel delta will be updated when palUpdateVideo() is called.
 *
 * @param[in] dx Pointer to recieve the mouse wheel delta x. Can be nullptr.
 * @param[in] dy Pointer to recieve the mouse wheel delta y. Can be nullptr.
 *
 * Thread safety: This function is thread-safe if `dx` and `dy` are thread
 * local.
 *
 * @since 1.0
 * @ingroup pal_video
 */
void PAL_CALL palGetMouseWheelDelta(
    Int32* dx,
    Int32* dy);

/**
 * @brief Check if the provided window is visible.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_GET_VISIBILITY` must be supported.
 *
 * @param[in] window Pointer to the window.
 *
 * @return `true` if the window is visible otherwise `false`.
 *
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 */
PAL_API bool PAL_CALL palIsWindowVisible(PalWindow* window);

/**
 * @brief Get the current input-focused window per application.
 *
 * The video system must be initialized before this call.
 * `PAL_VIDEO_FEATURE_WINDOW_GET_INPUT_FOCUS` must be supported.
 *
 * @return The current input-focused window on success or nullptr on
 * failure.
 *
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 */
PAL_API PalWindow* PAL_CALL palGetFocusWindow();

/**
 * @brief Get the native handle of the provided window.
 *
 * The video system must be initialized before this call.
 *
 * @param[in] window Pointer to the window.
 *
 * @return The native handle of the window on success or nullptr on failure.
 *
 * Thread safety: This function is thread-safe.
 *
 * @since 1.0
 * @ingroup pal_video
 */
PAL_API PalWindowHandleInfo PAL_CALL palGetWindowHandleInfo(PalWindow* window);

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
 * Thread safety: This function must be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * Thread safety: This function must be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * Thread safety: This function must be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * Thread safety: This function must be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 * @sa palGetWindowPos
 */
PAL_API PalResult PAL_CALL palSetWindowPos(
    PalWindow* window,
    Int32 x,
    Int32 y);

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
 * Thread safety: This function must be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 * @sa palGetWindowSize
 */
PAL_API PalResult PAL_CALL palSetWindowSize(
    PalWindow* window,
    Uint32 width,
    Uint32 height);

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
 * Thread safety: This function must be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 * @sa palGetFocusWindow
 */
PAL_API PalResult PAL_CALL palSetFocusWindow(PalWindow* window);

/**
 * @brief Create an icon.
 *
 * The video system must be initialized before this call.
 *
 * @param[in] info Pointer to a PalIconCreateInfo struct that specifies
 * paramters. Must not be nullptr.
 * @param[out] outIcon Pointer to a PalIcon to recieve the created
 * icon. Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * Thread safety: This function must be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 * @sa palCreateIcon
 */
PAL_API void PAL_CALL palDestroyIcon(PalIcon* icon);

/**
 * @brief Set the icon for the provided window.
 *
 * The video system must be initialized before this call.
 *
 * @param[in] window Pointer to the window.
 * @param[in] icon Pointer to the icon.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * paramters. Must not be nullptr.
 * @param[out] outCursor Pointer to a PalCursor to recieve the created
 * cursor. Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
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
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.1
 * @ingroup pal_video
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
 * Thread safety: This function must be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 * @sa palCreateCursor
 */
PAL_API void PAL_CALL palDestroyCursor(PalCursor* cursor);

/**
 * @brief Show or hide the provided cursor.
 *
 * The video system must be initialized before this call.
 * This affects all created cursors since the platform (OS) merges all cursors
 * into a single one on the screen.
 *
 * @param[in] show True to make the cursor visible otherwise false.
 *
 * Thread safety: This function must be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palShowCursor(bool show);

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
 * @param[in] clip True to clip to window or false to unclip.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function must be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 */
PAL_API PalResult PAL_CALL palClipCursor(
    PalWindow* window,
    bool clip);

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
 * Thread safety: This function must be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 */
PAL_API PalResult PAL_CALL palGetCursorPos(
    PalWindow* window,
    Int32* x,
    Int32* y);

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
 * Thread safety: This function must be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 */
PAL_API PalResult PAL_CALL palSetCursorPos(
    PalWindow* window,
    Int32 x,
    Int32 y);

/**
 * @brief Set the cursor for the provided window.
 *
 * The video system must be initialized before this call.
 *
 * @param[in] window Pointer to the window.
 * @param[in] cursor Pointer to the cursor.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function must be called from the main thread.
 *
 * @since 1.0
 * @ingroup pal_video
 */
PAL_API PalResult PAL_CALL palSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor);

/** @} */ // end of pal_video group

#endif // _PAL_VIDEO_H