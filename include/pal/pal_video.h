
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
 * @file pal_video.h
 *
 * Header file for video functions, macros, enum and structs
 *
 * @defgroup video
 *
 */

#ifndef _PAL_VIDEO_H
#define _PAL_VIDEO_H

#include "pal_event.h"

/**
 * @struct PalMonitor
 * @brief Opaque handle to a monitor.
 *
 * @ingroup video
 */
typedef struct PalMonitor PalMonitor;

/**
 * @struct PalWindow
 * @brief Opaque handle to a window.
 *
 * @ingroup video
 */
typedef struct PalWindow PalWindow;

/**
 * @struct PalIcon
 * @brief Opaque handle to an icon.
 *
 * @ingroup video
 */
typedef struct PalIcon PalIcon;

/**
 * @struct PalCursor
 * @brief Opaque handle to a cursor.
 *
 * @ingroup video
 */
typedef struct PalCursor PalCursor;

/**
 * @enum PalVideoFeatures
 * @brief Get the supported features of the video system.
 * 
 * Check after palInitVideo() to check which features are supported on the platform (OS).
 * Example: on Windows 7, `PAL_VIDEO_FEATURE_HIGH_DPI` is not supported.
 *
 * @code
 * PalVideoFeatures features = `palGetVideoFeatures()`.
 * @endcode
 * 
 * @code
 * if (features & PAL_VIDEO_FEATURE_HIGH_DPI) {
 *     // do logic
 * }
 * @endcode
 *
 * @note All video features follow the format `PAL_VIDEO_FEATURE_**` for consistency and API use.
 *
 * @ingroup video
 */
typedef enum {
    PAL_VIDEO_FEATURE_HIGH_DPI = PAL_BIT(0),                   /** < High DPI windows.*/
    PAL_VIDEO_FEATURE_MONITOR_ORIENTATION = PAL_BIT(1),        /** < Switching monitor orientation.*/
    PAL_VIDEO_FEATURE_BORDERLESS_WINDOW = PAL_BIT(2),          /** < Borderless windows.*/
    PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW = PAL_BIT(3),         /** < Transparent windows.*/
    PAL_VIDEO_FEATURE_TOOL_WINDOW = PAL_BIT(4),                /** < Tool windows.*/
    PAL_VIDEO_FEATURE_MONITOR_MODE_SWITCH = PAL_BIT(5),        /** < Switching monitor modes.*/
    PAL_VIDEO_FEATURE_MULTI_MONITORS = PAL_BIT(6),             /** < Multiple monitors.*/
    PAL_VIDEO_FEATURE_WINDOW_RESIZING = PAL_BIT(7),            /** < Resizing windows.*/
    PAL_VIDEO_FEATURE_WINDOW_POSITIONING = PAL_BIT(8),         /** < Moving windows.*/
    PAL_VIDEO_FEATURE_WINDOW_MINMAX = PAL_BIT(9),              /** < Window minimize and maximize operations.*/
    PAL_VIDEO_FEATURE_NO_MAXIMIZEBOX = PAL_BIT(10),            /** < No window minimize box.*/
    PAL_VIDEO_FEATURE_NO_MINIMIZEBOX = PAL_BIT(11),            /** < No window maximize box.*/
    PAL_VIDEO_FEATURE_MONITOR_GAMMA_CONTROL = PAL_BIT(12),     /** < Switching monitor gamma.*/
    PAL_VIDEO_FEATURE_CLIP_CURSOR = PAL_BIT(13),               /** < Clipping cursor to window.*/
    PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION = PAL_BIT(14),      /** < Flashing window title bar.*/
    PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY = PAL_BIT(15),         /** < Flashing window icon.*/
    PAL_VIDEO_FEATURE_WINDOW_FLASH_INTERVAL = PAL_BIT(16)      /** < Setting flash interval.*/
} PalVideoFeatures;

/**
 * @enum PalOrientation
 * @brief Orientation for a monitor
 *
 * @note All orientation follow the format `PAL_ORIENTATION_**` for consistency and API use.
 *
 * @ingroup video
 */
typedef enum {
    PAL_ORIENTATION_LANDSCAPE,            /** < 0 degrees.*/
    PAL_ORIENTATION_PORTRAIT,             /** < 90 degrees.*/
    PAL_ORIENTATION_LANDSCAPE_FLIPPED,    /** < 180 degrees.*/
    PAL_ORIENTATION_PORTRAIT_FLIPPED      /** < 270 degrees.*/
} PalOrientation;

/**
 * @enum PalWindowStyle
 * @brief Specifies style for a window.
 * This is a bitmask enum and multiple flags can be OR'ed together using bitwise OR operator (`|`).
 *
 * @code
 * PalWindowStyle style = PAL_WINDOW_STYLE_RESIZABLE | PAL_WINDOW_STYLE_TOPMOST.
 * @endcode
 *
 * @note All window flags follow the format `PAL_WINDOW_STYLE_**` for consistency and API use.
 *
 * @ingroup video
 */
typedef enum {
    PAL_WINDOW_STYLE_RESIZABLE = PAL_BIT(0),         /** < Window is resizable.*/
    PAL_WINDOW_STYLE_TRANSPARENT = PAL_BIT(1),       /** < Window is transparent.*/
    PAL_WINDOW_STYLE_TOPMOST = PAL_BIT(2),           /** < Topmost window.*/
    PAL_WINDOW_STYLE_NO_MINIMIZEBOX = PAL_BIT(3),    /** < Window has no minimze box.*/
    PAL_WINDOW_STYLE_NO_MAXIMIZEBOX = PAL_BIT(4),    /** < Window has no maximized box.*/
    PAL_WINDOW_STYLE_TOOL = PAL_BIT(5),              /** < Tooling window.*/
    PAL_WINDOW_STYLE_BORDERLESS = PAL_BIT(6)         /** < Window is borderless.*/
} PalWindowStyle;

/**
 * @enum PalWindowState
 * @brief Represents the current state of a window.
 *
 * @note All state follow the format `PAL_WINDOW_STATE_**` for consistency and API use.
 *
 * @ingroup video
 */
typedef enum {
    PAL_WINDOW_STATE_MAXIMIZED,          /** < Window is maximized.*/
    PAL_WINDOW_STATE_MINIMIZED,          /** < Window is minimized.*/
    PAL_WINDOW_STATE_RESTORED            /** < Window has been restored from revious state (eg. maximized).*/
} PalWindowState;

/**
 * @enum PalFlashFlag
 * @brief Specifies behavior for flashing a window.
 * 
 * This is a bitmask enum and multiple flags can be OR'ed together using bitwise OR operator (`|`).
 * If `PAL_FLASH_CAPTION` will be used, `PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION` must be supported.
 * 
 * If `PAL_FLASH_TRAY` will be used, `PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY` must be supported. 
 * 
 * Example:
 *
 * @code
 * PalFlashFlag flashType = PAL_FLASH_CAPTION | PAL_FLASH_TRAY.
 * @endcode
 * 
 * @code
 * PalFlashFlag flashTypeSingle = PAL_FLASH_STOP.
 * @endcode
 * 
 * @note All flash types follow the format `PAL_FLASH_**` for consistency and API use.
 * @note `PAL_FLASH_STOP` is not a bit and must not be combined with other bits.
 *
 * @ingroup video
 */
typedef enum {
    PAL_FLASH_STOP = 0,                 /** < Stop flashing.*/
    PAL_FLASH_CAPTION = PAL_BIT(0),     /** < Flash the titlebar of the window.*/
    PAL_FLASH_TRAY = PAL_BIT(1)         /** < Flash the icon of the window.*/
} PalFlashFlag;

/**
 * @enum PalScancode
 * @brief scancodes (layout independent keys) of a keyboard.
 *
 * @note All scancodes follow the format `PAL_SCANCODE_**` for consistency and API use.
 *
 * @ingroup video
 */
typedef enum {
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
 * @enum PalKeycode
 * @brief keycodes (layout aware keys) of a keyboard.
 *
 * @note All keycodes follow the format `PAL_KEYCODE_**` for consistency and API use.
 *
 * @ingroup video
 */
typedef enum {
    PAL_KEYCODE_UNKNOWN = 0,

    // Letters
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

    // Numbers (top row)
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

    // Function
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

    // Control
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

    // Arrows
    PAL_KEYCODE_LEFT,
    PAL_KEYCODE_RIGHT,
    PAL_KEYCODE_UP,
    PAL_KEYCODE_DOWN,

    // Navigation
    PAL_KEYCODE_INSERT,
    PAL_KEYCODE_DELETE,
    PAL_KEYCODE_HOME,
    PAL_KEYCODE_END,
    PAL_KEYCODE_PAGEUP,
    PAL_KEYCODE_PAGEDOWN,

    // Keypad
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

    // Misc
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
 * @brief mouse buttons of a mouse.
 *
 * @note All mouse buttons follow the format `PAL_MOUSE_BUTTON_**` for consistency and API use.
 *
 * @ingroup video
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
 * @struct PalMonitorInfo
 * @brief Information about a monitor.
 *
 * @sa palGetMonitorInfo(), palEnumerateMonitors()
 *
 * @ingroup video
 */
typedef struct {
    bool primary;                 /** < `true` if this is the primary monitor.*/
    Uint32 dpi;                   /** < Monitor DPI.*/
    Uint32 refreshRate;           /** < Monitor refresh rate in Hz.*/
    Int32 x;                      /** < Position x of the monitor.*/
    Int32 y;                      /** < Position y of the monitor.*/
    Uint32 width;                 /** < Width of the monitor in pixels.*/
    Uint32 height;                /** < Height of the monitor in pixels.*/
    PalOrientation orientation;   /** < Orientation of the monitor.*/
    char name[32];                /** < Name of the monitor.*/
} PalMonitorInfo;

/**
 * @struct PalMonitorMode
 * @brief Infomartion about a monitor mode.
 *
 * @sa palEnumerateMonitorModes(), palEnumerateMonitors()
 *
 * @ingroup video
 */
typedef struct {
    Uint32 bpp;                /** < Monitor Mode Bits per pixel.*/
    Uint32 refreshRate;        /** < Monitor Mode Refresh rate in Hz.*/
    Uint32 width;              /** < Width of the monitor mode in pixels.*/
    Uint32 height;             /** < Height of the monitor mode in pixels.*/
} PalMonitorMode;

/**
 * @struct PalFlashInfo
 * @brief Specifications for flashing a window to request focus.
 *
 * This struct must be initialized and passed to `palFlashWindow()`.
 * All fields must be explicitly set by the user.
 *
 * @note Uninitialized fields may result in undefined behavior.
 *
 * @ingroup video
 */
typedef struct {
    Uint32 interval;               /** < Flash interval in milliseconds. Set to `0` for default.*/
    PalFlashFlag flags;            /** < The flash behavior. This can be OR'ed together. see `PalFlashFlags`*/
    Uint32 count;                  /** < Number of times to flash. Set to `0` to flash until focused or cancelled.*/
} PalFlashInfo;

/**
 * @struct PalIconCreateInfo
 * @brief Specifications for creating an icon using the video system.
 *
 * This struct must be initialized and passed to `palCreateIcon()`.
 * All fields must be explicitly set by the user.
 *
 * @note Uninitialized fields may result in undefined behavior.
 *
 * @ingroup video
 */
typedef struct {
    Uint32 width;          /** < Width of the icon in pixels. Must be greater than zero.*/
    Uint32 height;         /** < Height of the icon in pixels. Must be greater than zero.*/
    const Uint8* pixels;   /** < Pixels in `RGBA` format.*/
} PalIconCreateInfo;

/**
 * @struct PalCursorCreateInfo
 * @brief Specifications for creating a cursor using the video system.
 *
 * This struct must be initialized and passed to `palCreateCursor()`.
 * All fields must be explicitly set by the user.
 *
 * @note Uninitialized fields may result in undefined behavior.
 *
 * @ingroup video
 */
typedef struct {
    Uint32 width;           /** < Width of the cursor in pixels. Must be greater than zero.*/
    Uint32 height;          /** < Width of the cursor in pixels. Must be greater than zero.*/
    Int32 xHotspot;         /** < The x active pixel for deteciting clicks.*/
    Int32 yHotspot;         /** < The y active pixel for deteciting clicks.*/
    const Uint8* pixels;    /** < Pixels in `RGBA` format.*/
} PalCursorCreateInfo;

/**
 * @struct PalWindowHandleInfo
 * @brief Information about a window handle.
 *
 * @sa PalWindow, palGetWindowHandleInfo(), palCreateWindow()
 *
 * @ingroup video
 */
typedef struct {
    void* nativeDisplay;           /** < The platform (OS) monitor handle of the window.*/
    void* nativeWindow;            /** < The platform (OS) handle of the window.*/
} PalWindowHandleInfo;

/**
 * @struct PalWindowCreateInfo
 * @brief Specifications for creating a window using the video system.
 *
 * This struct must be initialized and passed to `palCreateWindow()`.
 * All fields must be explicitly set by the user.
 *
 * @note Uninitialized fields may result in undefined behavior.
 *
 * @ingroup video
 */
typedef struct {
    bool show;                   /** < Show window after creation.*/
    bool showMaximized;          /** < Show window maximized after creation. If set `show` and `showMinimized` must be false.*/
    bool showMinimized;          /** < Show window minimzed after creation. If set `show` and `showMaximized` must be false.*/
    bool center;                 /** < Center window minimzed after creation.*/
    Uint32 width;                /** < Width of the window in pixels. Must be greater than zero.*/
    Uint32 height;               /** < Height of the window in pixels. Must be greater than zero.*/
    PalWindowStyle style;        /** < The style of the window. see `PalWindowStyle`.*/
    const char* title;           /** < The title of the window in UTF-8 encoding.*/
    PalMonitor* monitor;         /** < The monitor to create the window on. Set to `nullptr` to use primary monitor.*/
} PalWindowCreateInfo;

/**
 * @brief Initialize the video system.
 * 
 * The video system must be initialized before enumerating monitors,
 * query supported features, create windows and other video related functionality.
 * This must called before any video related function.
 * 
 * The video system must be shutdown with `palShutdownVideo()` when no longer needed.
 * The `eventDriver` must be valid to recieve video related events.
 *
 * @param[in] allocator Optional user provided allocator. Can be `nullptr` to use default.
 * @param[in] eventDriver Optional event driver to push events to. If `nullptr` no events will be generated.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palShutdownVideo()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palInitVideo(
    const PalAllocator *allocator,
    PalEventDriver* eventDriver);

/**
 * @brief Shutdown the video system.
 *
 * * The video system must be initialized before this call otherwise this functions returns silently.
 * This function does not destroy created windows and other video resources, 
 * therefore all created windows and other video resources must be destroyed explicitly before this function.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palInitVideo()
 * @ingroup video
 */
PAL_API void PAL_CALL palShutdownVideo();

/**
 * @brief Update the video system and all created windows.
 * 
 * The video system must be initialized before this call otherwise this functions returns silently.
 * This function also pushes generated video related events, if a valid event driver was set at `palInitVideo()`.
 * 
 * @note This function must be called from the main thread.
 *
 * @sa palInitVideo()
 * @ingroup video
 */
PAL_API void PAL_CALL palUpdateVideo();

/**
 * @brief Get the supported features of the video system.
 * 
 * The video system must be initialized before this call.
 * see `PalVideoFeatures`
 * 
 * @return video features on success or `0` on failure.
 *
 * @note This function is thread-safe.
 *
 * @sa palInitVideo()
 * @ingroup video
 */
PAL_API PalVideoFeatures PAL_CALL palGetVideoFeatures();

/**
 * @brief Return a list of all onnected monitors.
 * 
 * The video system must be initialized before this call.
 * 
 * User must `allocate` statically or dynamically and pass the maximum capacity of the
 * allocated array as `count` and also pass the array itself as `monitors`.
 * The user is responsible for the life time of the array.
 *
 * The `count` should be the number, not the size in bytes. Example:
 * 
 * @code
 * PalMonitor* monitors[2];
 * @endcode
 * 
 * @code
 * Int32 count = 2;
 * @endcode
 *
 * you can set the `monitors` to `nullptr` and PAL will set the number of the connected monitors to `count`.
 * 
 * If the `count` is zero or less than zero and the `monitors` is not `nullptr`, 
 * this function fails and returns `PAL_RESULT_INSUFFICIENT_BUFFER`.
 * If `count` is less than the number of connected monitors, PAL will write up to `count`.
 *
 * @param[in] count Capacity of the `monitors` array.
 * @param[out] monitors User allocated array of PalMonitor.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note Users must not free the monitor handles, they are managed by the platform (OS).
 * Users are required to cache this, 
 * and call this function again if monitors are added or removed. see `PAL_EVENT_MONITOR_LIST_CHANGED`
 *
 * @note This function must be called from the main thread.
 *
 * @sa palInitVideo(), palGetPrimaryMonitor(), PalMonitorInfo
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palEnumerateMonitors(
    Int32 *count,
    PalMonitor **outMonitors);

/**
 * @brief Get the primary connected monitor.
 * 
 * The video system must be initialized before this call.
 *
 * @param[out] outMonitor Pointer to recieve the primary monitor.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note Users must not free the monitor handle, it is managed by the platform (OS).
 * @note This function must be called from the main thread.
 *
 * @sa palInitVideo(), palEnumerateMonitors(), PalMonitorInfo
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palGetPrimaryMonitor(
    PalMonitor **outMonitor);

/**
 * @brief Get information about a monitor.
 * 
 * The video system must be initialized before this call.
 *
 * This function takes in a PalMonitorInfo struct and fills it.
 * Some of the fields are set to defaults if the operation is not supported on the platform (OS).
 * example: On Windows 7, DPI will always be 96.
 *
 * @param[in] monitor Monitor handle.
 * @param[out] info Pointer to the PalMonitorInfo struct to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palEnumerateMonitors(), palGetPrimaryMonitor(), PalMonitorInfo
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palGetMonitorInfo(
    PalMonitor *monitor,
    PalMonitorInfo *info);

/**
 * @brief Return a list of all supported monitor modes for the provided monitor.
 * 
 * The video system must be initialized before this call.
 *
 * User must `allocate` statically or dynamically and pass the maximum capacity of the
 * allocated array as `count` and also pass the array itself as `modes`.
 * The user is responsible for the life time of the array.
 *
 * The `count` should be the number, not the size in bytes. Example:
 * 
 * @code
 * PalMonitorMode modes[2];
 * @endcode
 * 
 * @code
 * Int32 count = 2;
 * @endcode
 *
 * you can set the `modes` to `nullptr` and PAL will set the number of available monitor modes to `count`.
 * 
 * If the `count` is zero or less than zero and the `modes` is nut `nullptr`, 
 * this function fails and returns `PAL_RESULT_INSUFFICIENT_BUFFER`.
 * If `count` is less than the available monitor modes, PAL will write up to `count`.
 *
 * @param[in] monitor Monitor handle.
 * @param[in] count Capacity of the `modes` array.
 * @param[out] modes User allocated array of PalMonitorMode.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palGetPrimaryMonitor(), palEnumerateMonitors()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palEnumerateMonitorModes(
    PalMonitor *monitor,
    Int32 *count,
    PalMonitorMode *modes);

/**
 * @brief Get the current monitor mode of a monitor.
 * 
 * The video system must be initialized before this call.
 *
 * @param[in] monitor Monitor handle.
 * @param[out] mode Pointer to recieve the current monitor mode.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palGetPrimaryMonitor(), palEnumerateMonitors()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palGetCurrentMonitorMode(
    PalMonitor *monitor,
    PalMonitorMode *mode);

/**
 * @brief Set the active monitor mode of a monitor.
 * 
 * The video system must be initialized before this call.
 *
 * PAL only validates the `mode` pointer not the values. To be safe,
 * users must get the monitor mode from `palEnumerateMonitorModes()` or `palValidateMonitorMode()` to validate the mode before changing.
 *
 * If the monitor mode submitted is invalid, this function might fail depending on the platform (OS).
 * 
 * `PAL_VIDEO_FEATURE_MONITOR_MODE_SWITCH` must be supported 
 * otherwise this function fails and returns `PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] monitor Monitor handle.
 * @param[out] mode Pointer to the monitor mode.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palGetPrimaryMonitor(), palEnumerateMonitors()
 * @sa palEnumerateMonitorModes(), palGetCurrentMonitorMode()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palSetMonitorMode(
    PalMonitor *monitor,
    PalMonitorMode *mode);

/**
 * @brief Check if the provided monitor mode is valid on the monitor.
 * 
 * The video system must be initialized before this call.
 * 
 * @param[in] monitor Monitor handle.
 * @param[out] mode Pointer to the monitor mode.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function mus be called from the main thread.
 *
 * @sa palGetPrimaryMonitor(), palEnumerateMonitors()
 * @sa palEnumerateMonitorModes(), palGetCurrentMonitorMode()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palValidateMonitorMode(
    PalMonitor *monitor,
    PalMonitorMode *mode);

/**
 * @brief Set the orientation for a monitor.
 * 
 * The video system must be initialized before this call.
 *
 * This affects all monitor modes for the monitor.
 * 
 * `PAL_VIDEO_FEATURE_MONITOR_ORIENTATION` must be supported 
 * otherwise this function fails and returns `PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] monitor Monitor handle.
 * @param[in] orientation The monitor orientation. example: `PAL_ORIENTATION_PORTRAIT`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 * @note This change is temporary and will be reset when the platform (OS) is reboot.
 *
 * @sa palGetPrimaryMonitor(), palEnumerateMonitors()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palSetMonitorOrientation(
    PalMonitor *monitor,
    PalOrientation orientation);

/**
 * @brief Create a window with PAL video system.
 * 
 * The video system must be initialized before this call.
 * The `info` pointer must be valid, explicitly initialized by the user. see `PalWindowCreateInfo` struct.
 *
 * @param[in] info Pointer to the PalWindowCreateInfo struct with the specifications.
 * @param[out] outWindow Pointer to recieve the created window.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa PalWindowCreateInfo, palDestroyWindow()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palCreateWindow(
    const PalWindowCreateInfo *info,
    PalWindow **outWindow);

/**
 * @brief Destroy the provided window.
 * 
 * The video system must be initialized before this call.
 *
 * This must be destroyed before the video system is shutdown.
 * If `window` is invalid, this function returns silently.
 *
 * @param[in] window Pointer to the window to destroy.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palCreateWindow()
 * @ingroup video
 */
PAL_API void PAL_CALL palDestroyWindow(
    PalWindow *window);

/**
 * @brief Minimize a maximized or restored window.
 * 
 * The video system must be initialized before this call.
 * 
 * If the window is already minimized, this functions does nothing and returns `PAL_RESULT_SUCCESS`.
 * If `PAL_VIDEO_FEATURE_WINDOW_MINMAX` is not supported, 
 * this function will fail and return `PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED`.
 * 
 * @param[in] window Pointer to the window.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function is thread safe.
 *
 * @sa palCreateWindow()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palMinimizeWindow(
    PalWindow* window);

/**
 * @brief Maximize a minimized or restored window.
 * 
 * The video system must be initialized before this call.
 * 
 * If the window is already maximized, this functions does nothing and returns `PAL_RESULT_SUCCESS`.
 * If `PAL_VIDEO_FEATURE_WINDOW_MINMAX` is not supported, 
 * this function will fail and return `PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED`.
 * 
 * @param[in] window Pointer to the window.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function is thread safe.
 *
 * @sa palCreateWindow()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palMaximizeWindow(
    PalWindow* window);

/**
 * @brief Restores a window to it previous state.
 * 
 * The video system must be initialized before this call.
 * 
 * If the window was previously maximized or minimized, this function reverts state to `PAL_WINDOW_STATE_RESTORED`
 *
 * @param[in] window Pointer to the window.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function is thread safe.
 *
 * @sa palCreateWindow()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palRestoreWindow(
    PalWindow* window);

/**
 * @brief Make the provided window visible.
 * 
 * The video system must be initialized before this call.
 * 
 * All windows are created hidden if not explicitly shown.
 * This does nothing if the window is already visible.
 *
 * @param[in] window Pointer to the window.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function is thread safe.
 * 
 * @sa palHideWindow(), palCreateWindow().
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palShowWindow(
    PalWindow* window);

/**
 * @brief Make the provided window invisble.
 * 
 * The video system must be initialized before this call.
 * 
 * This does nothing if the window is already invisble.
 *
 * @param[in] window Pointer to the window.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function is thread safe.
 * 
 * @sa palShowWindow(), palCreateWindow()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palHideWindow(
    PalWindow* window);

/**
 * @brief Request the platform (OS) to visually flash the provided window.
 * 
 * The video system must be initialized before this call.
 * 
 * If `PAL_FLASH_CAPTION` is used, `PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION` must be supported.
 * 
 * If `PAL_FLASH_TRAY` is used, `PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY` must be supported.
 * 
 * If a flash type is used and its not supported, this function will fail and return `PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] window Pointer to the window.
 * @param[in] info Pointer to a PalFlashInfo struct with flash specifications.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function is not thread safe.
 * 
 * @sa palCreateWindow(), PalFlashInfo, PalVideoFeatures, PalFlashFlags
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info);

/**
 * @brief Get the style of the provided window.
 * 
 * The video system must be initialized before this call.
 *
 * @param[in] window Pointer to the window.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function is thread safe if `outStyle` is thread local.
 * 
 * @sa palCreateWindow()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* outStyle);

/**
 * @brief Get the current monitor the provided window is currently on.
 * 
 * The video system must be initialized before this call.
 *
 * @param[in] window Pointer to the window.
 * @param[out] outMonitor Pointer to recieve the monitor.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function is thread safe if `outMonitor` is thread local.
 *
 * @sa palCreateWindow()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palGetWindowMonitor(
    PalWindow* window, 
    PalMonitor** outMonitor);

/**
 * @brief Get the title of the provided window.
 * 
 * The video system must be initialized before this call.
 * 
 * This function returns an allocated UTF-8 encoded string which must be freed when no longer needed with `palFree()`
 * The allocator used to allocate the string is the same passed in `palInitVideo()`.
 *
 * @param[in] window Pointer to the window.
 * @param[out] outTitle Pointer to buffer to recieve the window title.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function is not thread-safe. 
 * @note The returned string should not be freed with `palFree()`
 *
 * @sa palCreateWindow()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palGetWindowTitle(
    PalWindow* window,
    char** outTitle);

/**
 * @brief Get the position of the provided window in pixels.
 * 
 * The video system must be initialized before this call.
 *
 * @param[in] window Pointer to the window.
 * @param[out] x Pointer to recieve the window position x. Can be `nullptr`.
 * @param[out] y Pointer to recieve the window position y. Can be `nullptr`.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function is thread-safe if `x` and `y` are thread local.
 *
 * @sa palCreateWindow(), palSetWindowPos()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palGetWindowPos(
    PalWindow* window, 
    Int32* x, 
    Int32* y);

/**
 * @brief Get the position of the provided window's client area in pixels.
 * 
 * The video system must be initialized before this call.
 *
 * @param[in] window Pointer to the window.
 * @param[out] x Pointer to recieve the window client position x. Can be `nullptr`.
 * @param[out] y Pointer to recieve the window client position y. Can be `nullptr`.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function is thread-safe if `x` and `y` are thread local.
 *
 * @sa palCreateWindow(), palSetWindowPos()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palGetWindowClientPos(
    PalWindow* window, 
    Int32* x, 
    Int32* y);

/**
 * @brief Get the size of the provided window in pixels.
 * 
 * The video system must be initialized before this call.
 *
 * @param[in] window Pointer to the window.
 * @param[out] width Pointer to recieve the window width. Can be `nullptr`.
 * @param[out] height Pointer to recieve the window height. Can be `nullptr`.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function is thread-safe if `width` and `height` are thread local.
 *
 * @sa palCreateWindow(), palSetWindowSize()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palGetWindowSize(
    PalWindow* window, 
    Uint32* width, 
    Uint32* height);

/**
 * @brief Get the size of the provided window's client area in pixels.
 * 
 * The video system must be initialized before this call.
 *
 * @param[in] window Pointer to the window.
 * @param[out] width Pointer to recieve the window width. Can be `nullptr`.
 * @param[out] height Pointer to recieve the window height. Can be `nullptr`.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function is thread-safe if `width` and `height` are thread local.
 *
 * @sa palCreateWindow(), palSetWindowSize()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palGetWindowClientSize(
    PalWindow* window, 
    Uint32* width, 
    Uint32* height);

/**
 * @brief Get the current state of the provided window.
 * 
 * The video system must be initialized before this call.
 * 
 * The returns either `PAL_WINDOW_STATE_MINIMIZED` or `PAL_WINDOW_STATE_MAXIMIZED` or `PAL_WINDOW_STATE_RESTORED`, 
 * if the window is minimzed or maximized or restored respectively.
 *
 * @param[in] window Pointer to the window.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function is thread-safe if `state` is thread local.
 *
 * @sa palCreateWindow(), palMaximizeWindow()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palGetWindowState(
    PalWindow* window, 
    PalWindowState* state);

PAL_API const bool* PAL_CALL palGetKeycodeState();

PAL_API const bool* PAL_CALL palGetScancodeState();

PAL_API const bool* PAL_CALL palGetMouseState();

PAL_API void PAL_CALL palGetMouseDelta(
    Int32* dx,
    Int32* dy);

void PAL_CALL palGetMouseWheelDelta(
    Int32* dx, 
    Int32* dy);

/**
 * @brief Check if the provided window is visible.
 * 
 * The video system must be initialized before this call.
 *
 * @param[in] window Pointer to the window.
 * 
 * @return True if the window is visible otherwise false. This will return false if the window is invalid.
 *
 * @note This function is thread-safe.
 * @note This function is guaranteed not to fail if the `window` is valid.
 *
 * @sa palCreateWindow(), palHideWindow()
 * @ingroup video
 */
PAL_API bool PAL_CALL palIsWindowVisible(
    PalWindow* window);

/**
 * @brief Get the current input focused window.
 * 
 * The video system must be initialized before this call.
 *
 * @return The currently input focused window on success or `nullptr` on failure.
 *
 * @note This function is thread-safe.
 *
 * @sa palCreateWindow()
 * @ingroup video
 */
PAL_API PalWindow* PAL_CALL palGetFocusWindow();

/**
 * @brief Get the native handle of the provided window.
 * 
 * The video system must be initialized before this call.
 * 
 * This function returns the platform (OS) handle of the window. 
 * 
 * If the window handle is destroyed with the platform (OS),
 * users must not call any video function take takes in the `window` after deletion.
 * 
 * @param[in] window Pointer to the window.
 *
 * @return The native handle of the window on success or `nullptr` on failure.
 *
 * @note This function is thread-safe.
 *
 * @sa palCreateWindow()
 * @ingroup video
 */
PAL_API PalWindowHandleInfo PAL_CALL palGetWindowHandleInfo(
    PalWindow* window);

/**
 * @brief Set the opacity of the provided window.
 * 
 * The video system must be initialized before this call.
 * 
 * The window must have `PAL_WINDOW_STYLE_TRANSPARENT` style. The `opacity` must be in the range 0.0 - 1.0.
 *
 * @param[in] window Pointer to the window.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 * 
 * @sa palCreateWindow()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palSetWindowOpacity(
    PalWindow* window,
    float opacity);

/**
 * @brief Set the style of the provided window.
 * 
 * The video system must be initialized before this call.
 *
 * @param[in] window Pointer to the window.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 * 
 * @sa palCreateWindow()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style);

/**
 * @brief Set the title of the provided window.
 * 
 * The video system must be initialized before this call.
 * The `title` must be a UTF-8 encoding null terminated string.
 *
 * @param[in] window Pointer to the window.
 * @param[in] title UTF-8 encoding null terminated string.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 * 
 * @sa palCreateWindow()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palSetWindowTitle(
    PalWindow* window, 
    const char* title);

/**
 * @brief Set the position of the specified window in pixels.
 * 
 * The video system must be initialized before this call.
 * 
 * This function will fail and return `PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED`
 * if `PAL_VIDEO_FEATURE_WINDOW_POSITIONING` is not supported.
 * 
 * The position is not capped and might go beyond a single display, 
 * to be safe get the display bounds and cap the position with it.
 *
 * @param[in] window Pointer to the window.
 * @param[in] x The new x coordinate in pixels.
 * @param[in] y The new y coordinate in pixels.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palCreateWindow(), palSetWindowSize(), palRestoreWindow()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palSetWindowPos(
    PalWindow* window, 
    Int32 x, 
    Int32 y);

/**
 * @brief Set the size of the provided window in pixels.
 * 
 * The video system must be initialized before this call.
 * 
 * This function will fail and return `PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED`
 * if `PAL_VIDEO_FEATURE_WINDOW_RESIZING` is not supported.
 * 
 * @param[in] window Pointer to the window.
 * @param[in] width The new width of the window in pixels. Must be greater than zero.
 * @param[in] height The new height of the window in pixels. Must be greater than zero.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 * @note Some platforms (OS) with window managers may clapped or adjust the size.
 *
 * @sa palCreateWindow(), palSetWindowPos()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palSetWindowSize(
    PalWindow* window, 
    Uint32 width, 
    Uint32 height);

/**
 * @brief Request input focus for the provided window.
 * 
 * The video system must be initialized before this call.
 *
 * @param[in] window Pointer to the window.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 * 
 * @sa palCreateWindow()
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palSetFocusWindow(
    PalWindow* window);

PAL_API PalResult PAL_CALL palCreateIcon(
    const PalIconCreateInfo* info,
    PalIcon** outIcon);

PAL_API void PAL_CALL palDestroyIcon(
    PalIcon* icon);

PAL_API PalResult PAL_CALL palSetWindowIcon(
    PalWindow* window,
    PalIcon* icon);

PAL_API PalResult PAL_CALL palCreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** outCursor);

PAL_API void PAL_CALL palDestroyCursor(
    PalCursor* cursor);

PAL_API void PAL_CALL palShowCursor(
    bool show);

PAL_API void PAL_CALL palClipCursor(
    PalWindow* window,
    bool clip);

PAL_API void PAL_CALL palGetCursorPos(
    PalWindow* window,
    Int32* x,
    Int32* y);

PAL_API void PAL_CALL palSetCursorPos(
    PalWindow* window,
    Int32 x,
    Int32 y);

PAL_API void PAL_CALL palSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor);

#endif // _PAL_VIDEO_H