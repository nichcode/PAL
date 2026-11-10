
/**
 * @file init.h
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

#ifndef PAL_VIDEO_INIT_H
#define PAL_VIDEO_INIT_H

#include "types.h"
#include "pal2/core/memory.h"

/**
 * @defgroup video_features Video Features
 * @ingroup pal_video
 * @{
 */
#define PAL_VIDEO_FEATURE_HIGH_DPI (1ULL << 0) /**< High-DPI windows. */
#define PAL_VIDEO_FEATURE_MONITOR_SET_ORIENTATION (1ULL << 1) /**< Setting the monitor orientation. */
#define PAL_VIDEO_FEATURE_MONITOR_GET_ORIENTATION (1ULL << 2) /**< Getting the monitor orientation. */
#define PAL_VIDEO_FEATURE_BORDERLESS_WINDOW (1ULL << 3) /**< Borderless windows. */
#define PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW (1ULL << 4) /**< Transparent windows. */
#define PAL_VIDEO_FEATURE_TOOL_WINDOW (1ULL << 5) /**< Windows without taskbar icons. */
#define PAL_VIDEO_FEATURE_MONITOR_SET_MODE (1ULL << 6) /**< Setting the monitor display mode. */
#define PAL_VIDEO_FEATURE_MONITOR_GET_MODE (1ULL << 7) /**< Getting the monitor display mode. */
#define PAL_VIDEO_FEATURE_MULTI_MONITORS (1ULL << 8) /**< Deprecated. Will be removed in the next major version. */
#define PAL_VIDEO_FEATURE_WINDOW_SET_SIZE (1ULL << 9) /**< Setting the window size. */
#define PAL_VIDEO_FEATURE_WINDOW_GET_SIZE (1ULL << 10) /**< Getting the window size. */
#define PAL_VIDEO_FEATURE_WINDOW_SET_POS (1ULL << 11) /**< Setting the window position. */
#define PAL_VIDEO_FEATURE_WINDOW_GET_POS (1ULL << 12) /**< Getting the window position. */
#define PAL_VIDEO_FEATURE_WINDOW_SET_STATE (1ULL << 13) /**< Setting the window state. */
#define PAL_VIDEO_FEATURE_WINDOW_GET_STATE (1ULL << 14) /**< Getting the window state. */
#define PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY (1ULL << 15) /**< Setting the window visibility. */
#define PAL_VIDEO_FEATURE_WINDOW_GET_VISIBILITY (1ULL << 16) /**< Getting the window visibility. */
#define PAL_VIDEO_FEATURE_WINDOW_SET_TITLE (1ULL << 17) /**< Setting the window title. */
#define PAL_VIDEO_FEATURE_WINDOW_GET_TITLE (1ULL << 18) /**< Getting the window title. */
#define PAL_VIDEO_FEATURE_NO_MAXIMIZEBOX (1ULL << 19) /**< Windows without a maximize button. */
#define PAL_VIDEO_FEATURE_NO_MINIMIZEBOX (1ULL << 20) /**< Windows without a minimize button. */
#define PAL_VIDEO_FEATURE_CLIP_CURSOR (1ULL << 21) /**< Clipping the cursor to a window. */
#define PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION (1ULL << 22) /**< Flashing the window title bar. */
#define PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY (1ULL << 23) /**< Flashing the window taskbar icon. */
#define PAL_VIDEO_FEATURE_WINDOW_FLASH_INTERVAL (1ULL << 24) /**< Setting the window flash interval. */
#define PAL_VIDEO_FEATURE_WINDOW_SET_INPUT_FOCUS (1ULL << 25) /**< Setting the input-focused window. */
#define PAL_VIDEO_FEATURE_WINDOW_GET_INPUT_FOCUS (1ULL << 26) /**< Getting the input-focused window. */
#define PAL_VIDEO_FEATURE_WINDOW_SET_STYLE (1ULL << 27) /**< Setting the window style. */
#define PAL_VIDEO_FEATURE_WINDOW_GET_STYLE (1ULL << 28) /**< Getting the window style. */
#define PAL_VIDEO_FEATURE_CURSOR_SET_POS (1ULL << 29) /**< Setting the cursor position. */
#define PAL_VIDEO_FEATURE_CURSOR_GET_POS (1ULL << 30) /**< Getting the cursor position. */
#define PAL_VIDEO_FEATURE_WINDOW_SET_ICON (1ULL << 31) /**< Setting the window icon. */
#define PAL_VIDEO_FEATURE_TOPMOST_WINDOW (1ULL << 32) /**< Topmost windows. */
#define PAL_VIDEO_FEATURE_DECORATED_WINDOW (1ULL << 33) /**< Decorated windows. */
#define PAL_VIDEO_FEATURE_CURSOR_SET_VISIBILITY (1ULL << 34) /**< Setting the cursor visibility. */
#define PAL_VIDEO_FEATURE_WINDOW_GET_MONITOR (1ULL << 35) /**< Getting the monitor a window is on. */
#define PAL_VIDEO_FEATURE_MONITOR_GET_PRIMARY (1ULL << 36) /**< Getting the primary monitor. */
#define PAL_VIDEO_FEATURE_FOREIGN_WINDOWS (1ULL << 37) /**< Attaching and detaching foreign windows. */
#define PAL_VIDEO_FEATURE_MONITOR_VALIDATE_MODE (1ULL << 38) /**< Validating a monitor display mode. */
#define PAL_VIDEO_FEATURE_WINDOW_SET_CURSOR (1ULL << 39) /**< Setting the window cursor. */
/** @} */

/**
 * @defgroup video_drivers Video Driver Types
 * @ingroup pal_video
 * @{
 */
#define PAL_VIDEO_DRIVER_TYPE_UNKNOWN 0 /**< Unknown video driver type. */
#define PAL_VIDEO_DRIVER_TYPE_WIN32 1 /**< Windows (Win32) video driver. */
#define PAL_VIDEO_DRIVER_TYPE_WAYLAND 2 /**< Wayland video driver. */
#define PAL_VIDEO_DRIVER_TYPE_X11 3 /**< X11 video driver. */
#define PAL_VIDEO_DRIVER_TYPE_COUNT 4 /**< Number of video driver types. */
/** @} */

typedef struct PalEventDriver PalEventDriver;

/**
 * @struct PalVideoDriver
 * @brief Video driver.
 *
 * Filled in by @ref palEnumerateVideoDrivers() and @ref palGetVideoDriver().
 * Pass it to @ref palSetVideoDriver() to select a preferred video driver.
 *
 * @since Added in version 2.2
 * @ingroup pal_video
 */
typedef struct PalVideoDriver {
    PalVideoFeatures features; /**< Bitmask of features supported by the video driver. */
    PalVideoDriverType type; /**< Video driver type. */
    uint32_t id; /**< Video driver identifier. */
} PalVideoDriver;

/**
 * @brief Enumerates supported video drivers.
 *
 * This function gets all supported video drivers that can be used with the platform.
 * This is a two-call function, set `drivers` to `nullptr` and `count` to `0` to obtain
 * the number of supported video drivers. Allocate the array and call this function
 * again with `drivers` set to the allocated array and `count` set to the
 * capacity of the array.
 *
 * If the specified count is less than the number of supported video drivers,
 * only the video drivers that fit in the array will be written. It is
 * sufficient to call this function once because video drivers are not added or
 * removed at runtime.
 *
 * @param[in, out] count Capacity of the video driver array.
 * @param[out] drivers Output array to receive the video drivers, or `nullptr` to get
 *             the number of video drivers.
 *
 * @thread-safety Thread safe.
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
 * The video driver must be supported. If the video system has already been
 * initialized, the call is ignored. An unsupported video driver is also ignored
 * and the video system selects a default.
 *
 * @param[in] driver Preferred video driver.
 *
 * @thread-safety Must be called from the main thread.
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
 * If the video system has not been initialized, `driver` is left untouched.
 *
 * @param[out] driver Output struct to receive the video driver.
 *
 * @thread-safety Thread safe.
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
 * This function initializes the video system. If the video system has been initialized,
 * the function does nothing.
 *
 * `allocator` and `eventDriver` are not copied. The allocator and the event driver
 * with any state referenced by them must remain valid until @ref palShutdownVideo() is called.
 * If the event driver is `nullptr`, the video system will not process events.
 *
 * If `preferredInstance` is `nullptr`, the video system creates one and
 * controls its lifetime. On Linux, `preferredInstance` is the display associated with
 * the connection. On Windows, `preferredInstance` is the process HINSTANCE.
 * The preferred instance will not be destroyed by the video system when
 * @ref palShutdownVideo() is called.
 *
 * @param[in] allocator Allocator to use, or `nullptr` for the default.
 * @param[in] eventDriver Event driver to use, or `nullptr` to disable events.
 * @param[in] preferredInstance User-provided instance, or `nullptr` to let the video system create one.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety Must be called from the main thread.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_PLATFORM_FAILURE
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
 * @brief Shuts down the video system.
 *
 * This function shuts down the video system. All created windows, icons and cursors
 * must be destroyed before this call. If the video system has not been
 * initialized, the function returns silently.
 *
 * @thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palInitVideo
 */
PAL_API void PAL_CALL palShutdownVideo(void);

/**
 * @brief Updates the video system.
 *
 * This function updates the video system and all created and attached windows.
 * If the event driver supplied to @ref palInitVideo() is `nullptr`, the
 * events will not be processed and created or attached windows may be
 * unresponsive; the behavior is undefined. This function does not
 * block or wait for the event driver to populate events.
 *
 * @thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palInitVideo
 */
PAL_API void PAL_CALL palUpdateVideo(void);

/**
 * @brief Gets the supported features of the video system.
 *
 * This function returns the selected or active video driver features.
 *
 * @return Video features on success or `0` on failure.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palInitVideo
 */
PAL_API PalVideoFeatures PAL_CALL palGetVideoFeatures(void);

/**
 * @brief Gets the video system instance.
 *
 * This function returns the video system instance.
 * The instance is the display associated with the connection on Linux and the
 * process HINSTANCE on Windows.
 *
 * The instance must not be destroyed if it is owned by the video system.
 *
 * @return Instance on success, or `nullptr` on failure.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void* PAL_CALL palGetInstance(void);

#endif // PAL_VIDEO_INIT_H