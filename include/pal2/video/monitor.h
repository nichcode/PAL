
/**
 * @file monitor.h
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

#ifndef PAL_VIDEO_MONITOR_H
#define PAL_VIDEO_MONITOR_H

#include "types.h"

/**
 * @brief Maximum size of the monitor name, including the NULL terminator.
 * @ingroup pal_video
*/
#define PAL_MONITOR_NAME_SIZE 32

/**
 * @defgroup orientations Monitor Orientations
 * @ingroup pal_video
 * @{
 */
#define PAL_ORIENTATION_LANDSCAPE 0 /**< Landscape. */
#define PAL_ORIENTATION_PORTRAIT 1 /**< Portrait. */
#define PAL_ORIENTATION_LANDSCAPE_FLIPPED 2 /**< Landscape rotated 180 degrees. */
#define PAL_ORIENTATION_PORTRAIT_FLIPPED 3 /**< Portrait rotated 180 degrees. */
#define PAL_ORIENTATION_COUNT 4 /**< Number of orientations. */
/** @} */

/**
 * @struct PalMonitorInfo
 * @brief Monitor information.
 *
 * Filled in by @ref palGetMonitorInfo().
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalMonitorInfo {
    int32_t x; /**< Monitor x position in screen coordinates. */
    int32_t y; /**< Monitor y position in screen coordinates. */
    uint32_t width; /**< Monitor width in screen coordinates. */
    uint32_t height; /**< Monitor height in screen coordinates. */
    uint32_t dpi; /**< Monitor DPI, where 96 is scale 1.0x. */
    uint32_t refreshRate; /**< Monitor refresh rate in Hz. */
    PalOrientation orientation; /**< Monitor orientation. */
    PalBool primary; /**< Whether this is the primary monitor. */
    char name[PAL_MONITOR_NAME_SIZE]; /**< Monitor name (NULL-terminated). */
} PalMonitorInfo;

/**
 * @struct PalMonitorMode
 * @brief Monitor display mode.
 *
 * Filled in by @ref palEnumerateMonitorModes() and @ref palGetCurrentMonitorMode().
 * It is also the input of @ref palSetMonitorMode() and @ref palValidateMonitorMode().
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalMonitorMode {
    uint32_t bpp; /**< Display mode bits per pixel. */
    uint32_t refreshRate; /**< Display mode refresh rate in Hz. */
    uint32_t width; /**< Display mode width in screen coordinates. */
    uint32_t height; /**< Display mode height in screen coordinates. */
} PalMonitorMode;

/**
 * @brief Enumerates connected monitors.
 *
 * This function gets all connected monitors. This is a two-call function,
 * set `monitors` to `nullptr` and `count` to `0` to get the number of connected
 * monitors. Allocate the array and call this function again with `monitors` set to
 * the allocated array and `count` set to the capacity of the array.
 *
 * If the specified count is less than the number of connected monitors,
 * only the monitors that fit in the array will be written. The monitor handles
 * must not be modified or freed.
 *
 * The video system must be initialized before this call.
 *
 * @param[in, out] count Capacity of the monitor array.
 * @param[out] monitors Monitor array.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety Must be called from the main thread.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palGetPrimaryMonitor
 */
PAL_API PalResult PAL_CALL palEnumerateMonitors(
    uint32_t* count,
    PalMonitor** monitors);

/**
 * @brief Gets the primary monitor.
 *
 * This function gets the primary connected monitor of the platform. The returned
 * monitor handle must not be modified or freed.
 * @ref PAL_VIDEO_FEATURE_MONITOR_GET_PRIMARY feature must be supported or this function
 * results in undefined behavior.
 *
 * @param[out] monitor Output to receive the monitor.
 *
 * @thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palEnumerateMonitors
 */
PAL_API void PAL_CALL palGetPrimaryMonitor(PalMonitor** monitor);

/**
 * @brief Gets information about a monitor.
 *
 * @param[in] monitor Monitor to get its information.
 * @param[out] info Output struct to receive the monitor information.
 *
 * @thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palGetMonitorInfo(
    PalMonitor* monitor,
    PalMonitorInfo* info);

/**
 * @brief Enumerates monitor display modes.
 *
 * This function gets all supported display modes of a monitor. This is a two-call function,
 * set `modes` to `nullptr` and `count` to `0` to get the number of supported display modes.
 * Allocate the array and call this function again with `modes` set to
 * the allocated array and `count` set to the capacity of the array.
 *
 * If the specified count is less than the number of supported display modes,
 * only the display modes that fit in the array will be written.
 *
 * The returned monitor display modes are sorted in descending order
 * using the fields in @ref PalMonitorMode, in the following order
 * of precedence: width, height, refresh rate and bits per pixel.
 * The first display mode has the highest resolution, with the
 * refresh rate and bits per pixel used to sort modes with the
 * same resolution.
 *
 * @param[in] monitor Monitor to get its display modes.
 * @param[in, out] count Capacity of the monitor display mode array.
 * @param[out] modes Monitor display mode array.
 *
 * @thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palEnumerateMonitorModes(
    PalMonitor* monitor,
    uint32_t* count,
    PalMonitorMode* modes);

/**
 * @brief Gets the current display mode of the monitor.
 *
 * @ref PAL_VIDEO_FEATURE_MONITOR_GET_MODE feature must be supported or this function
 * results in undefined behavior.
 *
 * @param[in] monitor Monitor to get its current display mode.
 * @param[out] mode Output struct to receive the display mode.
 *
 * @thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palSetMonitorMode
 */
PAL_API void PAL_CALL palGetCurrentMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode);

/**
 * @brief Sets the display mode of a monitor.
 *
 * This function sets the active display mode of the specified monitor.
 * `mode` is not validated by the video system; use @ref palEnumerateMonitorModes()
 * to get a supported one or call @ref palValidateMonitorMode() to validate the
 * display mode before switching. Setting an invalid display mode results in undefined
 * behavior. @ref PAL_VIDEO_FEATURE_MONITOR_SET_MODE feature must be supported.
 *
 * @param[in] monitor Monitor to set its current display mode.
 * @param[in] mode Monitor display mode.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety Must be called from the main thread.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palGetCurrentMonitorMode
 * @sa palEnumerateMonitorModes
 * @sa palValidateMonitorMode
 */
PAL_API PalResult PAL_CALL palSetMonitorMode(
    PalMonitor* monitor,
    const PalMonitorMode* mode);

/**
 * @brief Checks whether a display mode is valid on a monitor.
 *
 * This function checks whether the specified display mode is supported
 * on the specified monitor. @ref PAL_VIDEO_FEATURE_MONITOR_VALIDATE_MODE
 * feature must be supported.
 *
 * @param[in] monitor Monitor to validate display mode on.
 * @param[in] mode Monitor display mode.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety Must be called from the main thread.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palSetMonitorMode
 */
PAL_API PalResult PAL_CALL palValidateMonitorMode(
    PalMonitor* monitor,
    const PalMonitorMode* mode);

/**
 * @brief Sets the orientation for a monitor.
 *
 * This function sets the orientation of the specified monitor. The change is temporary
 * and will be reset when the platform reboots.
 * @ref PAL_VIDEO_FEATURE_MONITOR_SET_ORIENTATION feature must be supported.
 *
 * @param[in] monitor Monitor to set its orientation.
 * @param[in] orientation Monitor orientation.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety Must be called from the main thread.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API PalResult PAL_CALL palSetMonitorOrientation(
    PalMonitor* monitor,
    PalOrientation orientation);

#endif // PAL_VIDEO_MONITOR_H