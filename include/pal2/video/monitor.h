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

/**
 * @brief Returns a list of all connected monitors.
 *
 * The video system must be initialized before this call.
 * This function returns a snapshot of the currently connected
 * monitors.The monitor handles must not be freed by the user, they are
 * managed by the platform (OS). Users are required to cache this, and call
 * this function again if monitors are added or removed.
 * 
 * Set `monitors` parameter to nullptr to get the total number of connected
 * monitors. If the monitor array passed is less than the number of
 * connected monitors, PAL will fill the array upto that limit sequentially.
 *
 * @param[in, out] count The capacity of the monitor array.
 * @param[out] monitors The monitor array.
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 *         failure. Call @ref palFormatResult() to get the string representation
 *         of the result value.
 *
 * @Thread-safety Must only be called from the main thread.
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
 * @brief Gets the primary connected monitor.
 *
 * PAL_VIDEO_FEATURE_MONITOR_GET_PRIMARY must be supported
 * otherwise undefined behavior.
 * 
 * This function is not guaranteed to work on all platforms, enumerate
 * the monitors and select the first one if the feature is not 
 * supported on the platform.
 *
 * The monitor handle must not be freed by the user, it is managed by the
 * platform (OS).
 *
 * @param[out] monitor The output to recieve the primary monitor.
 *                     Must not be nullptr.
 *
 * @Thread-safety Must only be called from the main thread.
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
 * @param[in] monitor The monitor. Must not be nullptr.
 * @param[out] info The output struct to recieve the monitor info. 
 *                  Must not be nullptr.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palGetMonitorInfo(
    PalMonitor* monitor,
    PalMonitorInfo* info);

/**
 * @brief Returns a list of all supported display modes of a monitor.
 * 
 * Set `modes` parameter to nullptr to get the total number of supported
 * display modes of the monitor. If the modes array passed is less than 
 * the number of display modes, PAL will fill the array upto that limit 
 * sequentially.
 * 
 * The returned monitor display modes are sorted in descending order
 * using the fields in @ref PalMonitorMode, in the following order
 * of precedence: width, height, refresh rate and bits per pixel.
 * This fist display mode has the highest resolution, with the
 * refresh rate an bits per pixel used to sort modes with the
 * same resolution.
 *
 * @param[in] monitor The monitor. Must not be nullptr.
 * @param[in, out] count The capacity of the monitor display mode array.
 * @param[out] modes The display mode array.
 *
 * @Thread-safety Must only be called from the main thread.
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
 * PAL_VIDEO_FEATURE_MONITOR_GET_MODE must be supported
 * otherwise undefined behavior.
 *
 * @param[in] monitor The monitor. Must not be nullptr.
 * @param[out] mode The output struct to recieve the display mode. 
 *                  Must not ne nullptr.
 *
 * @Thread-safety Must only be called from the main thread.
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
 * @brief Sets the active display mode of the monitor.
 *
 * PAL_VIDEO_FEATURE_MONITOR_SET_MODE Must be supported 
 * otherwise undefined behavior.
 * 
 * Validate the display mode with @ref palValidateMonitorMode before switching
 * on supported platforms or use a display mode from
 * @ref palEnumerateMonitorModes to be safe. If the monitor display mode 
 * submitted is invalid, the behavior is platform specific.
 *
 * @param[in] monitor The monitor. Must not ne nullptr.
 * @param[in] mode The display mode.
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 *         failure. Call @ref palFormatResult() to get the string representation
 *         of the result value.
 *
 * @Thread-safety Must only be called from the main thread.
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
    PalMonitorMode* mode);

/**
 * @brief Checks if a display mode is valid on the monitor.
 *
 * PAL_VIDEO_FEATURE_MONITOR_VALIDATE_MODE must be supported
 * otherwise undefined behavior.
 *
 * @param[in] monitor The monitor.
 * @param[in] mode The display mode.
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 *         failure. Call @ref palFormatResult() to get the string representation
 *         of the result value.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palSetMonitorMode
 */
PAL_API PalResult PAL_CALL palValidateMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode);

/**
 * @brief Sets the orientation for the monitor.
 *
 * PAL_VIDEO_FEATURE_MONITOR_SET_ORIENTATION must be supported
 * otherwise undefined behavior. This change is temporary and will
 * be reset when the platform (OS) reboots.
 *
 * @param[in] monitor The monitor.
 * @param[in] orientation The orientation.
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 *         failure. Call @ref palFormatResult() to get the string representation
 *         of the result value.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API PalResult PAL_CALL palSetMonitorOrientation(
    PalMonitor* monitor,
    PalOrientation orientation);

#endif // PAL_VIDEO_MONITOR_H