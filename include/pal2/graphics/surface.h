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

#ifndef PAL_GRAPHICS_SURFACE_H
#define PAL_GRAPHICS_SURFACE_H

#include "device.h"

/**
 * @defgroup present_modes Present Modes
 * @brief Present modes
 * 
 * @{
 */
#define PAL_PRESENT_MODE_FIFO 0
#define PAL_PRESENT_MODE_IMMEDIATE 1
#define PAL_PRESENT_MODE_MAILBOX 2
#define PAL_PRESENT_MODE_COUNT 3
/** @} */

/**
 * @defgroup composite_alphas Composite Alphas
 * @brief Composite alphas
 * 
 * @{
 */
#define PAL_COMPOSITE_ALPHA_OPAQUE 0
#define PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED 1
#define PAL_COMPOSITE_ALPHA_POST_MULTIPLIED 2
#define PAL_COMPOSITE_ALPHA_COUNT 3
/** @} */

/**
 * @defgroup surface_formats Surface Formats
 * @brief Surface formats
 * 
 * @{
 */
#define PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR 0
#define PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR 1
#define PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR 2
#define PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10 3
#define PAL_SURFACE_FORMAT_COUNT 4
/** @} */

/**
 * @defgroup window_instance_types Window Instance Types
 * @brief Window instance types
 * 
 * @{
 */
#define PAL_WINDOW_INSTANCE_TYPE_WAYLAND 0
#define PAL_WINDOW_INSTANCE_TYPE_X11 1
#define PAL_WINDOW_INSTANCE_TYPE_XCB 2
#define PAL_WINDOW_INSTANCE_TYPE_WIN32 3
#define PAL_WINDOW_INSTANCE_TYPE_COUNT 4
/** @} */

/**
 * @typedef PalPresentMode
 * @brief Present modes
 * 
 * All values of this type follow the format `PAL_PRESENT_MODE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalPresentMode;

/**
 * @typedef PalCompositeAplha
 * @brief Composite alphas
 * 
 * All values of this type follow the format `PAL_COMPOSITE_ALPHA_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalCompositeAplha;

/**
 * @typedef PalSurfaceFormat
 * @brief Surface format types.
 * 
 * All values of this type follow the format `PAL_SURFACE_FORMAT_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalSurfaceFormat;

/**
 * @typedef PalWindowInstanceType
 * @brief Instance types of a window instance.
 * 
 * All values of this type follow the format `PAL_WINDOW_INSTANCE_TYPE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalWindowInstanceType;

/**
 * @struct PalSurface
 * @brief Opaque handle to a surface.
 *
 * @since Added in version 2.0
 */
typedef struct PalSurface PalSurface;

/**
 * @struct PalSurfaceCapabilities
 * @brief Contains surface capabilities of an adapter (GPU).
 *
 * @since Added in version 2.0
 * 
 * @var PalSurfaceCapabilities::supportedPresentModes
 * A bitmask of supported present modes. A specific present mode
 * should be check like this: @nl
 * palIsSupported(::supportedPresentModes, `PAL_PRESENT_MODE_FIFO`).
 * 
 * @var PalSurfaceCapabilities::supportedCompositeAlphas
 * A bitmask of supported composite alphas. A specific composite alpha
 * should be check like this: @nl
 * palIsSupported(::supportedCompositeAlphas, `PAL_COMPOSITE_ALPHA_OPAQUE`).
 * 
 * @var PalSurfaceCapabilities::supportedFormats
 * A bitmask of supported surface formats. A specific surface format
 * should be check like this: @nl
 * palIsSupported(::supportedFormats, `PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10`).
 * 
 * @var PalSurfaceCapabilities::minImageCount
 * The minimum number of image or back buffer count.
 * 
 * @var PalSurfaceCapabilities::maxImageCount
 * The maximum number of image or back buffer count.
 * 
 * @var PalSurfaceCapabilities::minImageWidth
 * The minimum width of the image.
 * 
 * @var PalSurfaceCapabilities::minImageHeight
 * The minimum height of the image.
 * 
 * @var PalSurfaceCapabilities::maxImageWidth
 * The maximum width of the image.
 * 
 * @var PalSurfaceCapabilities::maxImageHeight
 * The maximum height of the image.
 * 
 * @var PalSurfaceCapabilities::maxImageArrayLayers
 * The maximum array layers of the image.
 */
typedef struct PalSurfaceCapabilities
{
    uint32_t supportedPresentModes;
    uint32_t supportedCompositeAlphas;
    uint32_t supportedFormats;
    uint32_t minImageCount;
    uint32_t maxImageCount;
    uint32_t minImageWidth;
    uint32_t minImageHeight;
    uint32_t maxImageWidth;
    uint32_t maxImageHeight;
    uint32_t maxImageArrayLayers;
} PalSurfaceCapabilities;

/**
 * @brief Create a surface for a window.
 *
 * The created surface must be destroyed using `palDestroySurface()`.
 *
 * `PAL_ADAPTER_FEATURE_SWAPCHAIN` must be supported and enabled when creating the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] device Device that creates the surface.
 * @param[in] window Window to create the surface for.
 * @param[in] windowInstance The instance of the window.
 * @param[in] instanceType The instance type of the window.
 * @param[out] outSurface Pointer to a PalSurface to recieve the created surface.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palDestroySurface
 */
PAL_API PalResult PAL_CALL palCreateSurface(
    PalDevice* device,
    void* window,
    void* windowInstance,
    PalWindowInstanceType instanceType,
    PalSurface** outSurface);

/**
 * @brief Destroy a surface.
 *
 * @param[in] surface Surface to destroy.
 *
 * Thread safety: Thread safe if the device used to create the surface is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateSurface
 */
PAL_API void PAL_CALL palDestroySurface(PalSurface* surface);

/**
 * @brief Get surface capabilites about a device.
 *
 * `PAL_ADAPTER_FEATURE_SWAPCHAIN` must be supported and enabled when creating the
 * device. Otherwise behavior is undefined.
 *
 * @param[in] device Device to query surface feature capabilities on.
 * @param[in] surface Surface to query capabilities.
 * @param[out] caps Pointer to a PalSurfaceCapabilities to fill.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetSurfaceCapabilities(
    PalDevice* device,
    PalSurface* surface,
    PalSurfaceCapabilities* caps);

#endif // PAL_GRAPHICS_SURFACE_H