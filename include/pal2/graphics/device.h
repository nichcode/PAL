/**
 * @file device.h
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

#ifndef PAL_GRAPHICS_DEVICE_H
#define PAL_GRAPHICS_DEVICE_H

#include "pal2/core/base.h"
#include "pal2/core/result.h"
#include "adapter.h"

/**
 * @struct PalDevice
 * @brief Opaque handle to a graphics device.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDevice PalDevice;

/**
 * @brief Creates a device.
 *
 * The requested features in `features` must all be supported by the adapter. Use
 * @ref palGetAdapterFeatures() to query the supported features before creating the device.
 * Enabling an unsupported feature causes device creation to fail.
 *
 * Enabling every feature the adapter supports is valid, but not recommended. It is
 * better to create the device with only the features you need.
 *
 * @param[in] adapter Adapter that creates the device.
 * @param[in] features Adapter features to enable.
 * @param[out] outDevice Output handle to receive the created device.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `adapter` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palDestroyDevice
 */
PAL_API PalResult PAL_CALL palCreateDevice(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice);

/**
 * @brief Destroys a device.
 *
 * All resources created with the device must be destroyed before this call.
 *
 * @param[in] device Pointer to the device to destroy.
 *
 * @thread-safety The adapter used to create `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateDevice
 */
PAL_API void PAL_CALL palDestroyDevice(PalDevice* device);

/**
 * @brief Gets the native device lost reason code.
 *
 * This function returns the backend-specific native code associated with the device-loss
 * reason. Backends that do not expose an explicit reason return their standard device-lost code.
 *
 * @param[in] device The device.
 * @return Backend-specific native device lost reason code.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API uint32_t PAL_CALL palGetDeviceLostReason(PalDevice* device);

/**
 * @brief Check if a constant is supported in a mask.
 *
 * This function is used to check all masks in `supported_**` format in most of the capabilities
 * query structs.
 *
 * Example:
 *
 * To check if `PAL_PRESENT_MODE_IMMEDIATE` is supported after querying `PalSurfaceCapabilities`
 * capabilities of a surface, PalSurfaceCapabilities::supportedPresentModes should be the `mask`
 * parameter and `PAL_PRESENT_MODE_IMMEDIATE` as the value parameter.
 *
 * @param[in] mask The supported mask.
 * @param[in, out] value The value to check in the supported mask.
 *
 * @return `PAL_TRUE` on success otherwise `PAL_FALSE`.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
static inline PalBool PAL_CALL palIsSupported(
    uint32_t mask,
    uint32_t value)
{
    return (mask & (1U << value)) != 0;
}

#endif // PAL_GRAPHICS_DEVICE_H