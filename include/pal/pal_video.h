
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
 */

#ifndef _PAL_VIDEO_H
#define _PAL_VIDEO_H

#include "pal_core.h"

/**
 * @struct PalDisplay
 * @brief Opaque handle to a video display (monitor).
 *
 * @ingroup video
 */
typedef struct PalDisplay PalDisplay;

/**
 * @struct PalDisplay
 * @brief Opaque handle to a window.
 *
 * @ingroup video
 */
typedef struct PalWindow PalWindow;

/**
 * @enum PalDisplayOrientation
 * @brief Orientation for a display or display mode.
 *
 * @note All orientation follow the format `PAL_ORIENTATION_**` for consistency and API use.
 *
 * @ingroup video
 */
typedef enum PalDisplayOrientation {
    PAL_ORIENTATION_LANDSCAPE,         /** < 0 degrees.*/
    PAL_ORIENTATION_PORTRAIT,          /** < 90 degrees.*/
    PAL_ORIENTATION_LANDSCAPE_FLIPPED, /** < 180 degrees.*/
    PAL_ORIENTATION_PORTRAIT_FLIPPED,  /** < 270 degrees.*/
} PalDisplayOrientation;

/**
 * @enum PalVideoFeatures
 * @brief features supported on the video system.
 *
 * @note video instances created on the same OS version will be the same.
 *
 * Example: on Windows 7, `PAL_VIDEO_FEATURE_HIGH_DPI` is not supported.
 * If you create two video instance on Windows 7, they features will be the same.
 * So you can share features flag across multiple video system instance across threads.
 *
 * @code
 * PalVideoFeatures features = `palVideoGetFeatures()`.
 * if (features & PAL_VIDEO_FEATURE_HIGH_DPI) {
 *     windowFlags |= PAL_WINDOW_ALLOW_HIGH_DPI;
 * }
 * @endcode
 *
 * @note All video features follow the format `PAL_VIDEO_FEATURE_**` for consistency and API use.
 *
 * @ingroup video
 */
typedef enum PalVideoFeatures {
    /** < Supports high DPI windows.*/
    PAL_VIDEO_FEATURE_HIGH_DPI = PAL_BIT(0),

    /** < Supports changing display orientation.*/
    PAL_VIDEO_FEATURE_DISPLAY_ORIENTATION = PAL_BIT(1),

    /** < Supports borderless windows.*/
    PAL_VIDEO_FEATURE_BORDERLESS_WINDOW = PAL_BIT(2),

    /** < Supports chaning display modes.*/
    PAL_VIDEO_FEATURE_DISPLAY_MODE_SWITCH = PAL_BIT(3),

    /** < Supports multiple monitors.*/
    PAL_VIDEO_FEATURE_MULTI_DISPLAYS = PAL_BIT(4),

    /** < Supports dynamic resizing of windows.*/
    PAL_VIDEO_FEATURE_WINDOW_RESIZING = PAL_BIT(5),

    /** < Supports dynamic positioning of windows.*/
    PAL_VIDEO_FEATURE_WINDOW_POSITIONING = PAL_BIT(6),

    /** < Supports maximized and minimized operations of windows.*/
    PAL_VIDEO_FEATURE_WINDOW_MINMAX = PAL_BIT(7),

    /** < Supports display gamma control.*/
    PAL_VIDEO_FEATURE_DISPLAY_GAMMA_CONTROL = PAL_BIT(8),

    /** < Supports clipping cursor (mouse).*/
    PAL_VIDEO_FEATURE_CLIP_CURSOR = PAL_BIT(9),

    /** < Supports flashing window titlebar.*/
    PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION = PAL_BIT(10),

    /** < Supports flashing window title icon on the taskbar.*/
    PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY = PAL_BIT(11)
} PalVideoFeatures;

/**
 * @enum PalWindowFlags
 * @brief Specifies behavior and style for a window at creation time.
 * This is a bitmask enum and multiple flags can be OR'ed together using bitwise OR operator (`|`).
 *
 * @code
 * PalWindowFlags flags = PAL_WINDOW_SHOWN | PAL_WINDOW_ALLOW_HIGH_DPI.
 * @endcode
 *
 * @note All window flags follow the format `PAL_WINDOW_**` for consistency and API use.
 *
 * @ingroup video
 */
typedef enum PalWindowFlags {
    PAL_WINDOW_SHOWN = PAL_BIT(0),                    /** < Window is shown after creation.*/
    PAL_WINDOW_MAXIMIZED = PAL_BIT(1),                /** < Window is maximized after creation.*/
    PAL_WINDOW_RESIZABLE = PAL_BIT(2),                /** < Window is resizable.*/
    PAL_WINDOW_CENTER = PAL_BIT(3),                   /** < Window centered after creation.*/
    PAL_WINDOW_MINIMIZEBOX = PAL_BIT(4),              /** < Window has no minimized box.*/
    PAL_WINDOW_ALLOW_HIGH_DPI = PAL_BIT(5),           /** < DPI scalling.*/

    /** < A borderless fullscreen window. `PAL_VIDEO_FEATURE_BORDERLESS_WINDOW` must be supported on the OS. */
    PAL_WINDOW_FULLSCREEN = PAL_BIT(6),               

    /** < Window is created with default behavior (application window).*/
    PAL_WINDOW_DEFAULT = PAL_WINDOW_RESIZABLE | PAL_WINDOW_MINIMIZEBOX | PAL_WINDOW_SHOWN
} PalWindowFlags;

/**
 * @enum PalFlashFlags
 * @brief Specifies behavior for flashing a window.
 * 
 * This is a bitmask enum and multiple flags can be OR'ed together using bitwise OR operator (`|`).
 * 
 * Note: `PAL_FLASH_STOP` is not a bitmask and must not be combined with other bitmask.
 *
 * @code
 * PalFlashFlags flashTypes = PAL_FLASH_CAPTION | PAL_FLASH_TRAY.
 * @endcode
 * 
 * @code
 * PalFlashFlags flashTypeSingle = PAL_FLASH_STOP.
 * @endcode
 * 
 * @note All flash types follow the format `PAL_FLASH_**` for consistency and API use.
 *
 * @ingroup video
 */
typedef enum PalFlashFlags {
    PAL_FLASH_STOP = 0,                        /** < Stop flash.*/
    PAL_FLASH_CAPTION = PAL_BIT(0),            /** < Flash the titlebar of the window.*/
    PAL_FLASH_TRAY = PAL_BIT(1),               /** < Flash the window icon in the taskbar.*/
} PalFlashFlags;

/**
 * @struct PalDisplayInfo
 * @brief Contains information about a display.
 *
 * @sa palGetDisplayInfo()
 *
 * @ingroup video
 */
typedef struct PalDisplayInfo {
    char name[32];                             /** < Name of the display.*/
    Int32 x;                                   /** < X position of yhe display.*/
    Int32 y;                                   /** < Y position of the display.*/
    Uint32 width;                              /** < Width of the display in pixels.*/
    Uint32 height;                             /** < Height of the display in pixels.*/
    Uint32 dpi;                                /** < Display dpi.*/
    Uint32 refreshRate;                        /** < Refresh rate in Hz.*/
    PalDisplayOrientation orientation;         /** < display orientation.*/
    bool primary;                              /** < True if this is the primary display.*/
} PalDisplayInfo;

/**
 * @struct PalDisplayInfo
 * @brief Represents a single display mode.
 * All values are in physical pixels.
 *
 * @sa palEnumerateDisplayModes()
 *
 * @ingroup video
 */
typedef struct PalDisplayMode {
    Uint32 width;              /** < Width of the display mode in pixels.*/
    Uint32 height;             /** < Height of the display mode in pixels.*/
    Uint32 bpp;                /** < Bits per pixel.*/
    Uint32 refreshRate;        /** < Refresh rate in Hz.*/
} PalDisplayMode;

/**
 * @struct PalWindowCreateInfo
 * @brief Specifies options for creating a window using the video system.
 *
 * This struct must be initialized and passed to palCreateWindow().
 *
 * All fields must be explicitly set by the user.
 *
 * @note Uninitialized fields may result in undefined behavior.
 *
 * @ingroup video
 */
typedef struct PalWindowCreateInfo {
    const char *title;             /** < UTF-8 encoded null terminated string for the window title.*/
    PalDisplay *display;           /** < Display to create window on. set to nullptr to use primary display.*/
    Uint32 width;                  /** < Width of the window in pixels.*/
    Uint32 height;                 /** < Height of the window in pixels.*/
    PalWindowFlags flags;          /** < Window behavior. This can be OR'ed together. see `palWindowFlags`*/
} PalWindowCreateInfo;

/**
 * @struct PalFlashInfo
 * @brief Specifies options for flashing a window to ask for focus.
 *
 * This struct must be initialized and passed to palFlashWindow().
 *
 * All fields must be explicitly set by the user.
 *
 * @note Uninitialized fields may result in undefined behavior.
 *
 * @ingroup video
 */
typedef struct PalFlashInfo {
    PalFlashFlags flags;      /** < The flash behavior. This can be OR'ed together. see `PalFlashFlags`*/
    Uint32 count;             /** < Number of times to flash. Set to `0` to flash until focused or cancelled.*/
    Uint32 interval;          /** < Flash interval in milliseconds. Only on Windows. Set to `0` for default.*/
} PalFlashInfo;

/**
 * @brief Initializes the video system.
 * 
 * The video system must be initialized before enumerating connected displays (monitors),
 * query supported features, create windows and other video related functionality.
 * This must called before any video related function.
 * 
 * The video system must be shutdown with `palShutdownVideo()` when no longer needed.
 * The `eventDriver` field in `info` must be valid to recieve video related events.
 *
 * @param[in] allocator Optional user provided allocator. Can be `nullptr` to use default.
 * @param[in] eventDriver Optional event driver to push events to. If `nullptr` no events will be pushed.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palShutdownVideo()
 * @ingroup video
 */
_PAPI PalResult _PCALL palInitVideo(
    PalAllocator *allocator,
    PalEventDriver *eventDriver);

/**
 * @brief Shutdown the video system.
 * 
 * The video system must be initialized before this call.
 * This function does not destroy created windows and video resources, 
 * therefore all created windows and resources must be destroyed before this call.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palInitVideo()
 * @ingroup video
 */
_PAPI void _PCALL palShutdownVideo();

/**
 * @brief Get the supported features of the video system.
 * 
 * The video system must be initialized before this call.
 *
 * @code 
 * PalVideoFeatures features = palGetVideoFeatures();
 * if (features & PAL_VIDEO_FEATURE_HIGH_DPI) {
 *     you do yout logic here
 * }
 * @endcode
 * 
 * @return video features on success or `0` on failure.
 *
 * @note This function is thread-safe.
 *
 * @sa palInitVideo()
 * @ingroup video
 */
_PAPI PalVideoFeatures _PCALL palGetVideoFeatures();

/**
 * @brief Update the video system and all created windows.
 * 
 * The video system must be initialized before this call.
 * This function also pushing triggered video related events,
 * if a valid event driver was set at `palInitVideo()`.
 * 
 * @note This function must be called from the main thread.
 *
 * @sa palInitVideo()
 * @ingroup video
 */
_PAPI void _PCALL palUpdateVideo();

/**
 * @brief Returns a list of active and connected displays (monitors).
 *
 * User must `allocate` statically or dynamically and pass the maximum capacity of the
 * allocated array as `count` and pass the array itself as `displays`.
 * The user is responsible for the life time of the array.
 *
 * The `count` should be the number, not the size in bytes. Example:
 * 
 * @code
 * PalDisplay* displays[2];
 * @endcode
 * 
 * @code
 * Int32 count = 2;
 * @endcode
 *
 * you can set the `displays` to nullptr and PAL will set the count of the connected displays to `count`.
 * If the `count` is zero or less than zero, the function returns `PAL_RESULT_INSUFFICIENT_BUFFER`.
 * If `count` is less than the connected displays, PAL will write up to `count`.
 *
 * @param[in] count Capacity of the `displays` array.
 * @param[out] displays User allocated array of PalDisplay.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note Users must not free the display handles, they are managed by the OS.
 * Users are required to cache this, and call this function again if displays are added or removed.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palInitVideo(), palGetPrimaryDisplay(), PalDisplayInfo
 * @ingroup video
 */
_PAPI PalResult _PCALL palEnumerateDisplays(
    Int32 *count,
    PalDisplay **displays);

/**
 * @brief Get the primary active display (monitors).
 *
 * @param[out] outDisplay User allocated PalDisplay handle to recieve the primary display.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note Users must not free the display handle, it is managed by the OS.
 * @note This function must be called from the main thread.
 *
 * @sa palInitVideo(), palEnumerateDisplays(), PalDisplayInfo
 * @ingroup video
 */
_PAPI PalResult _PCALL palGetPrimaryDisplay(
    PalDisplay **outDisplay);

/**
 * @brief Get information about a display (monitor).
 *
 * This takes in a PalDisplayInfo struct and fills it.
 * Some of the fields are set to defaults if operation is not supported on the OS.
 * example: On Windows 7, DPI will always be 96.
 *
 * @param[in] display Display handle.
 * @param[out] info Pointer to the PalDisplayInfo struct to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palEnumerateDisplays(), palGetPrimaryDisplay(), PalDisplayInfo
 * @ingroup video
 */
_PAPI PalResult _PCALL palGetDisplayInfo(
    PalDisplay *display,
    PalDisplayInfo *info);

/**
 * @brief Returns a list of all supported display modes for a display (monitor).
 *
 * User must `allocate` statically or dynamically and pass the maximum capacity of the
 * allocated array as `count` and pass the array itself as `modes`.
 * The user is responsible for the life time of the array.
 *
 * The `count` should be the number, not the size in bytes. Example:
 * @code
 * PalDisplayModes modes[2];
 * Int32 count = 2;
 * @endcode
 *
 * you can set the `modes` to nullptr and PAL will set the count of the available display modes to `count`.
 * If the `count` is zero or less than zero, the function returns `PAL_RESULT_INSUFFICIENT_BUFFER`.
 * If `count` is less than the available display modes, PAL will write up to `count`.
 *
 * @param[in] display Display handle.
 * @param[in] count Capacity of the `modes` array.
 * @param[out] modes User allocated array of PalDisplayMode.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palGetPrimaryDisplay(), palEnumerateDisplays()
 * @ingroup video
 */
_PAPI PalResult _PCALL palEnumerateDisplayModes(
    PalDisplay *display,
    Int32 *count,
    PalDisplayMode *modes);

/**
 * @brief Get the current display mode of a display (monitor).
 *
 * @param[in] display Display handle.
 * @param[out] mode Pointer to user allocated PalDisplayMode struct.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palGetPrimaryDisplay(), palEnumerateDisplays()
 * @ingroup video
 */
_PAPI PalResult _PCALL palGetCurrentDisplayMode(
    PalDisplay *display,
    PalDisplayMode *mode);

/**
 * @brief Set the active display mode of a display (monitor).
 *
 * PAL only validates the `mode` pointer not the values. To be safe,
 * users must get the display mode from `palEnumerateDisplayModes()`.
 * Or call `palValidateDisplayMode()` to validate the mode before changing.
 *
 * If the display mode submitted is invalid, this function might fail depending on the OS.
 *
 * @param[in] display Display handle.
 * @param[out] mode Pointer to the display mode.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palGetPrimaryDisplay(), palEnumerateDisplays()
 * @sa palEnumerateDisplayModes(), palGetCurrentDisplayMode()
 * @ingroup video
 */
_PAPI PalResult _PCALL palSetDisplayMode(
    PalDisplay *display,
    PalDisplayMode *mode);

/**
 * @brief Check if the supplied display mode is supported on the display (monitor).
 *
 * @param[in] display Display handle.
 * @param[out] mode Pointer to the display mode.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function mus be called from the main thread.
 *
 * @sa palGetPrimaryDisplay(), palEnumerateDisplays()
 * @sa palEnumerateDisplayModes(), palGetCurrentDisplayMode()
 * @ingroup video
 */
_PAPI PalResult _PCALL palValidateDisplayMode(
    PalDisplay *display,
    PalDisplayMode *mode);

/**
 * @brief Set the orientation for a display (monitor).
 *
 * This affects all display modes for the display.
 *
 * @param[in] display Display handle.
 * @param[in] orientation The orientation. example: `PAL_ORIENTATION_PORTRAIT`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 * @note This change is temporary and will be reset when the OS is reboot.
 *
 * @sa palGetPrimaryDisplay(), palEnumerateDisplays()
 * @ingroup video
 */
_PAPI PalResult _PCALL palSetDisplayOrientation(
    PalDisplay *display,
    PalDisplayOrientation orientation);

/**
 * @brief Create a window using the video system.
 * 
 * If the window flags set in `info` is not supported, (see `PalVideoFeatures`)
 * this function will return `PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED`.
 * 
 * If `PAL_WINDOW_FULLSCREEN` is set, `PAL_VIDEO_FEATURE_BORDERLESS_WINDOW` must be supported.
 * The created window must be destroyed with `palDestroyWindow()` when no longer needed.
 *
 * @param[in] info Pointer to a PalWindowCreateInfo struct with creation options.
 * @param[out] outWindow Pointer to recieve the created window.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa PalWindowCreateInfo, palDestroyWindow()
 * @ingroup video
 */
_PAPI PalResult _PCALL palCreateWindow(
    const PalWindowCreateInfo *info,
    PalWindow **outWindow);

/**
 * @brief Destroy the specified window.
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
_PAPI void _PCALL palDestroyWindow(PalWindow *window);

/**
 * @brief Sets or exits borderless mode for the specified window.
 * 
 * `PAL_VIDEO_FEATURE_BORDERLESS_WINDOW` must be supported.
 * 
 * This functions sets borderless mode for the window if `enable` is `true`,
 * otherwise restores to normal mode when `false`.
 *
 * @param[in] window Pointer to the window.
 * @param[in] enable True to set borderless mode otherwise false for normal mode.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function msut be called from the main thread.
 *
 * @sa palCreateWindow(), palGetWindowDisplay()
 * @sa palEnumerateDisplays()
 * @ingroup video
 */
_PAPI PalResult _PCALL palSetWindowBorderless(
    PalWindow* window,
    bool enable);

/**
 * @brief Get the current display (monitor) the specified window is on.
 *
 * @param[in] window Pointer to the window.
 * @param[out] outDisplay Pointer to recieve the display.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palCreateWindow()
 * @ingroup video
 */
_PAPI PalResult _PCALL palGetWindowDisplay(
    PalWindow* window, 
    PalDisplay** outDisplay);

/**
 * @brief Resizes and repositions a window to fully fit the bounds of the specified display (monitor).
 * 
 * This functions moves and resizes the window to the size and position of the display.
 * This function is used by `palSetWindowFullscreen()` to make a window fullscreen.
 * 
 * If the `display` is (nullptr), the current display the window is on will be used.
 *
 * @param[in] window Pointer to the window.
 * @param[in] display Pointer to the display. This may be a nullptr.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called on the main thread.
 *
 * @sa palCreateWindow(), palEnumerateDisplays().
 * @sa palGetPrimaryDisplay(), palGetWindowDisplay().
 * @ingroup video
 */
_PAPI PalResult _PCALL palFitWindowToDisplay(
    PalWindow* window, 
    PalDisplay* display);

/**
 * @brief Restores a window to it previous size, position and show state.
 * 
 * Call this function when you use `palFitWindowToDisplay()` to restore the window.
 * 
 * If `palMaximizedWindow()` or `palMinimizedWindow()` was called, this function reverts
 * the window to the state it was before.
 *
 * @param[in] window Pointer to the window.
 * @param[in] display Pointer to the display. This may be a nullptr.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palCreateWindow()
 * @ingroup video
 */
_PAPI PalResult _PCALL palRestoreWindow(PalWindow* window);

/**
 * @brief Maximize a minimized or windowed mode window.
 * 
 * The window must not be in fullscreen mode, 
 * otherwise this function will fail and return `PAL_RESULT_INVALID_OPERATION`.
 * If the window is already maximized, this functions does nothing and returns `PAL_RESULT_SUCCESS`.
 * 
 * If `PAL_VIDEO_FEATURE_WINDOW_MINMAX` is not supported, 
 * this function will failt and return `PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED`.
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
_PAPI PalResult _PCALL palMaximizeWindow(PalWindow* window);

/**
 * @brief Minimize a maximized or windowed mode window.
 * 
 * The window must not be in fullscreen mode, 
 * otherwise this function will fail and return `PAL_RESULT_INVALID_OPERATION`.
 * If the window is already minimized, this functions does nothing and returns `PAL_RESULT_SUCCESS`.
 * 
 * If `PAL_VIDEO_FEATURE_WINDOW_MINMAX` is not supported, 
 * this function will failt and return `PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED`.
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
_PAPI PalResult _PCALL palMinimizeWindow(PalWindow* window);

/**
 * @brief Sets or exits fullscreen mode (borderless fullscreen) for the specified window.
 * 
 * This functions sets borderless fullscreen mode for the window if `enable` is `true`,
 * otherwise restores to windowed mode when `false`.
 * 
 * If the window is already in fullscreen mode and `enable` is `true`, this returns `PAL_RESULT_SUCCESS`.
 * Same if the window is already in windowed mode and `enable` is `false`.
 * If the `display` is (nullptr), the current display the window is on will be used.
 * 
 * When `enable` is false, this function acts like `palRestoreWindow()`,
 * but also restores the window style. So use this function to set and exit fullscreen mode.
 * 
 * This function is a wrapper for `palSetWindowBorderless()` and `palFitWindowToDisplay()`
 * and can be replace with those functions respectively.
 *
 * @param[in] window Pointer to the window.
 * @param[in] display Pointer to the display. This may be a nullptr.
 * @param[in] enable True to set fullscreen mode otherwise false for windowed mode.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palCreateWindow(), palGetWindowDisplay()
 * @sa palEnumerateDisplays()
 * @ingroup video
 */
_PAPI PalResult _PCALL palSetWindowFullscreen(
    PalWindow* window, 
    PalDisplay* display,
    bool enable);

/**
 * @brief Make the specified window visible.
 * 
 * All windows are created hidden if `PAL_WINDOW_SHOWN` flag is not set.
 * This does nothing if the window is already visible.
 * 
 * On some OS, this functions also set focus and bring the window to the top of all windows.
 *
 * @param[in] window Pointer to the window.
 *
 * @note This function is guaranteed not to fail if the `window` is valid.
 * @note This function may vary slightly on different OS (platforms). 
 * 
 * @note This function must be called from the main thread.
 * 
 * @sa palHideWindow(), palCreateWindow().
 * @ingroup video
 */
_PAPI void _PCALL palShowWindow(PalWindow* window);

/**
 * @brief Hides the specified window.
 * 
 * This is the default behaviour if `PAL_WINDOW_SHOWN` flag is not set. 
 * This does nothing if the window is already hidden.
 * 
 * The window is hidden but still valid and can be shown with `palShowWindow()`.
 *
 * @param[in] window Pointer to the window.
 *
 * @note This function is guaranteed not to fail if the `window` is valid.
 * @note This function may vary slightly on different OS (platforms). 
 * Example: the window will not be hidden instantly.
 * 
 * @note This function must be called from the main thread.
 * 
 * @sa palShowWindow(), palCreateWindow().
 * @ingroup video
 */
_PAPI void _PCALL palHideWindow(PalWindow* window);

/**
 * @brief Request the OS to visually flash the specified window.
 * 
 * Use this function to get user's attention when the window is not in the foreground.
 * Depending on the OS (playform), this may not work. Example:
 * 
 * Windows supports flashing the widows's title bar and taskbar icon.
 * 
 * MacOS supports only bouncing the dock icon. etc.
 * 
 * check for support for flash types with `PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION` for title bar flashing 
 * and `PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY` for taskbar icon flashing.
 * If both are not supported, this function will failt and return `PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED`.
 * 
 * See PalVideoFeatures.
 *
 * @param[in] window Pointer to the window.
 * @param[in] info Pointer to a PalFlashInfo struct with flash options.
 *
 * @note This function must be called from the main thread.
 * 
 * @sa palCreateWindow(), PalFlashInfo, PalVideoFeatures, PalFlashFlags
 * @ingroup video
 */
_PAPI PalResult palFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info);

/**
 * @brief Place the window at the center of the display (monitor) bounds.
 * 
 * Unlike `palFitWindowToDisplay()`, `palRestoreWindow()` does not work on this function.
 * If a window is centered on the display, the window can not revert back to its last position,
 * unless the user explicitly set it using `palSetWindowPos()`.
 * 
 * If the `display` is (nullptr), the current display the window is on will be used.
 *
 * @param[in] window Pointer to the window.
 * @param[in] display Pointer to the display. This may be a nullptr.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palCreateWindow(), palEnumerateDisplays().
 * @sa palGetPrimaryDisplay(), palGetWindowDisplay().
 * @ingroup video
 */
_PAPI PalResult _PCALL palCenterWindow(
    PalWindow* window, 
    PalDisplay* display);

/**
 * @brief Set the title of the specified window.
 * 
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
_PAPI PalResult _PCALL palSetWindowTitle(
    PalWindow* window, 
    const char* title);

/**
 * @brief Set the position of the specified window in pixels relative to the virtual desktop origin.
 * 
 * This function fails and returns `PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED`
 * if `PAL_VIDEO_FEATURE_WINDOW_POSITIONING` is not supported.
 * 
 * If the window is already maximized, minimized or in fullscreen mode,
 * this function fails and return `PAL_RESULT_INVALID_OPERATION`. 
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
_PAPI PalResult _PCALL palSetWindowPos(
    PalWindow* window, 
    Int32 x, 
    Int32 y);

/**
 * @brief Set the size of the client area of the specified window in pixels.
 * 
 * This function will fail and return `PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED`
 * if `PAL_VIDEO_FEATURE_WINDOW_RESIZING` is not supported.
 * 
 * If the window is already maximized, minimized or in fullscreen mode,
 * this function fails and return `PAL_RESULT_INVALID_OPERATION`.
 *
 * @param[in] window Pointer to the window.
 * @param[in] width The new width of the window in pixels. Must be greater than `0`.
 * @param[in] height The new height of the window in pixels. Must be greater than `0`.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 * @note Some OS (platforms) with window managers may clapped or adjust the size.
 *
 * @sa palCreateWindow(), palSetWindowPos(), palRestoreWindow()
 * @ingroup video
 */
_PAPI PalResult _PCALL palSetWindowSize(
    PalWindow* window, 
    Uint32 width, 
    Uint32 height);

/**
 * @brief Get the title of the specified window.
 *
 * @param[in] window Pointer to the window.
 * 
 * @return UTF-8 encoded null terminated string on success or nullptr  on failure.
 *
 * @note This function is thread-safe. 
 * @note The returned string should not be freed.
 *
 * @sa palCreateWindow()
 * @ingroup video
 */
_PAPI const char* _PCALL palGetWindowTitle(PalWindow* window);

/**
 * @brief Get the position of the specified window in pixels relative to the virtual desktop origin.
 *
 * @param[in] window Pointer to the window.
 * @param[out] x Pointer to recieve the window x position. Can be nullptr.
 * @param[out] y Pointer to recieve the window y position. Can be nullptr.
 *
 * @note This function is thread-safe if `x` and `y` are thread local.
 * If not, the user is responsible for synchronization.
 *
 * @sa palCreateWindow(), palSetWindowPos()
 * @ingroup video
 */
_PAPI void _PCALL palGetWindowPos(
    PalWindow* window, 
    Int32* x, 
    Int32* y);

/**
 * @brief Get the size of the specified window in pixels.
 *
 * @param[in] window Pointer to the window.
 * @param[out] width Pointer to recieve the window width. Can be nullptr.
 * @param[out] height Pointer to recieve the window height. Can be nullptr.
 *
 * @note This function is thread-safe if `width` and `height` are thread local.
 * If not, the user is responsible for synchronization.
 *
 * @sa palCreateWindow(), palSetWindowSize()
 * @ingroup video
 */
_PAPI void _PCALL palGetWindowSize(
    PalWindow* window, 
    Uint32* width, 
    Uint32* height);

/**
 * @brief Get the unique ID of the specified window.
 * 
 * This is unique within one process (application).
 * 
 * @return The unique id of the window.
 *
 * @param[in] window Pointer to the window.
 *
 * @note This function is thread-safe.
 * @note This function is guaranteed not to fail if the `window` is valid.
 *
 * @sa palCreateWindow()
 * @ingroup video
 */
_PAPI Uint64 _PCALL palGetWindowID(PalWindow* window);

/**
 * @brief Get the native handle of the specified window.
 * 
 * This is OS (platform) handle of the window. 
 * A simple example on Windows:
 * 
 * @code
 * HWND windowHandle = (HWND)palGetWindowHandle(window);
 * @endcode
 * 
 * The returned handle must not be freed by the user.
 * If users decide to destroy the handle directly with the OS (platform),
 * users must not call any video function take takes in the `window`.
 * 
 * @param[in] window Pointer to the window.
 *
 * @return The native handle of the window on success or `nullptr` on failure.
 *
 * @note This function is thread-safe.
 * @note This function is guaranteed not to fail if the `window` is valid.
 *
 * @sa palCreateWindow()
 * @ingroup video
 */
_PAPI void* _PCALL palGetWindowHandle(PalWindow* window);

/**
 * @brief Check if the specified window is maximized.
 *
 * @param[in] window Pointer to the window.
 * 
 * @return True if the window is maximized otherwise false. 
 * This will return false if the window is invalid.
 *
 * @note This function is thread-safe.
 * @note This function is guaranteed not to fail if the `window` is valid.
 *
 * @sa palCreateWindow(), palMaximizeWindow()
 * @ingroup video
 */
_PAPI bool _PCALL palIsWindowMaximized(PalWindow* window);

/**
 * @brief Check if the specified window is minimized.
 *
 * @param[in] window Pointer to the window.
 * 
 * @return True if the window is minimized otherwise false. 
 * This will return false if the window is invalid.
 *
 * @note This function is thread-safe.
 * @note This function is guaranteed not to fail if the `window` is valid.
 *
 * @sa palCreateWindow(), palMinimizeWindow()
 * @ingroup video
 */
_PAPI bool _PCALL palIsWindowMinimized(PalWindow* window);

/**
 * @brief Check if the specified window is hidden.
 *
 * @param[in] window Pointer to the window.
 * 
 * @return True if the window is hidden otherwise false. 
 * This will return false if the window is invalid.
 *
 * @note This function is thread-safe.
 * @note This function is guaranteed not to fail if the `window` is valid.
 *
 * @sa palCreateWindow(), palHideWindow()
 * @ingroup video
 */
_PAPI bool _PCALL palIsWindowHidden(PalWindow* window);

/**
 * @brief Check if the specified window is in fullscreen mode.
 *
 * @param[in] window Pointer to the window.
 * 
 * @return True if the window is in fullscreen mode otherwise false. 
 * This will return false if the window is invalid.
 *
 * @note This function is thread-safe.
 * @note This function is guaranteed not to fail if the `window` is valid.
 *
 * @sa palCreateWindow(), palSetWindowFullscreen()
 * @ingroup video
 */
_PAPI bool _PCALL palIsWindowFullScreen(PalWindow* window);

#endif // _PAL_VIDEO_H