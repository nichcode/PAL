/**
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

#include "pal2/core/defines.h"
#include "pal2/core/memory.h"
#include "pal2/core/result.h"

#define PAL_MONITOR_NAME_SIZE 32

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
 * @struct PalMonitor
 * @brief Opaque handle to a monitor.
 *
 * @since Added in version 2.0
 */
typedef struct PalMonitor PalMonitor;

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

#endif // PAL_VIDEO_MONITOR_H