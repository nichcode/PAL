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
 */

#ifndef PAL_VIDEO_H
#define PAL_VIDEO_H

#include "video/cursor.h"
#include "video/icon.h"
#include "video/keyboard.h"
#include "video/monitor.h"
#include "video/mouse.h"
#include "video/window.h"

/**
 * @defgroup video_features Video Features
 * @brief Video features
 * 
 * @ingroup pal_video
 */

/** @brief The video driver supports high DPI windows.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_HIGH_DPI (1ULL << 0)

/** @brief The video driver supports setting monitor orientation.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_MONITOR_SET_ORIENTATION (1ULL << 1)

/** @brief The video driver supports getting monitor orientation.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_MONITOR_GET_ORIENTATION (1ULL << 2)

/** @brief The video driver supports borderless windows.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_BORDERLESS_WINDOW (1ULL << 3)

/** @brief The video driver supports transparent windows.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW (1ULL << 4)

/** @brief The video driver supports tool windows.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_TOOL_WINDOW (1ULL << 5)

/** @brief The video driver supports setting current monitor display mode.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_MONITOR_SET_MODE (1ULL << 6)

/** @brief The video driver supports getting current monitor display mode.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_MONITOR_GET_MODE (1ULL << 7)

/** @brief The video driver supports multiple monitors.
 * 
 * This is depreciated and will be removed.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_MULTI_MONITORS (1ULL << 8)

/** @brief The video driver supports setting window size dynamically.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_SET_SIZE (1ULL << 9)

/** @brief The video driver supports getting window size dynamically.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_GET_SIZE (1ULL << 10)

/** @brief The video driver supports setting window position dynamically.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_SET_POS (1ULL << 11)

/** @brief The video driver supports getting window position dynamically.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_GET_POS (1ULL << 12)

/** @brief The video driver supports setting window state dynamically.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_SET_STATE (1ULL << 13)

/** @brief The video driver supports getting window state dynamically.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_GET_STATE (1ULL << 14)

/** @brief The video driver supports setting window visibility dynamically.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY (1ULL << 15)

/** @brief The video driver supports getting window visibility dynamically.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_GET_VISIBILITY (1ULL << 16)

/** @brief The video driver supports setting window title dynamically.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_SET_TITLE (1ULL << 17)

/** @brief The video driver supports getting window title dynamically.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_GET_TITLE (1ULL << 18)

/** @brief The video driver supports removing the maximizebox from windows.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_NO_MAXIMIZEBOX (1ULL << 19)

/** @brief The video driver supports removing the minimizebox from windows.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_NO_MINIMIZEBOX (1ULL << 20)

/** @brief The video driver supports clipping cursor to a window.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_CLIP_CURSOR (1ULL << 21)

/** @brief The video driver supports flashing the titlebar of a window.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION (1ULL << 22)

/** @brief The video driver supports flashing the taskbar icon of a window.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY (1ULL << 23)

/** @brief The video driver supports flash interval greater than `0`.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_FLASH_INTERVAL (1ULL << 24)

/** @brief The video driver supports setting input-focus for a window.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_SET_INPUT_FOCUS (1ULL << 25)

/** @brief The video driver supports getting input-focus for a window.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_GET_INPUT_FOCUS (1ULL << 26)

/** @brief The video driver supports setting window styles dynamically.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_SET_STYLE (1ULL << 27)

/** @brief The video driver supports getting window styles dynamically.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_GET_STYLE (1ULL << 28)

/** @brief The video driver supports setting cursor position.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_CURSOR_SET_POS (1ULL << 29)

/** @brief The video driver supports getting cursor position.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_CURSOR_GET_POS (1ULL << 30)

/** @brief The video driver supports setting window icon.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_SET_ICON (1ULL << 31)

/** @brief The video driver supports topmost windows.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_TOPMOST_WINDOW (1ULL << 32)

/** @brief The video driver supports decorated windows.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_DECORATED_WINDOW (1ULL << 33)

/** @brief The video driver supports setting cursor visibility.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_CURSOR_SET_VISIBILITY (1ULL << 34)

/** @brief The video driver supports getting the monitor a window 
 * is currently on.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_GET_MONITOR (1ULL << 35)

/** @brief The video driver supports getting primary monitors.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_MONITOR_GET_PRIMARY (1ULL << 36)

/** @brief The video driver supports attaching and detaching foreign windows.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_FOREIGN_WINDOWS (1ULL << 37)

/** @brief The video driver supports validating monitor display modes.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_MONITOR_VALIDATE_MODE (1ULL << 38)

/** @brief The video driver supports setting window cursors.
 * 
 * @ingroup video_features
*/
#define PAL_VIDEO_FEATURE_WINDOW_SET_CURSOR (1ULL << 39)

typedef struct PalEventDriver PalEventDriver;

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
 * @ingroup pal_video
 */
typedef uint64_t PalVideoFeatures;

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
