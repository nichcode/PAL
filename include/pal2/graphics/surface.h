/**
 * @file surface.h
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

#ifndef PAL_GRAPHICS_SURFACE_H
#define PAL_GRAPHICS_SURFACE_H

#include "pal2/core/base.h"
#include "pal2/core/result.h"
#include "device.h"

/**
 * @typedef PalPresentMode
 * @brief Swapchain present mode.
 *
 * Identifies how swapchain images are presented to a surface.
 *
 * All values of this type follow the format `PAL_PRESENT_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalPresentMode;

/**
 * @typedef PalCompositeAplha
 * @brief Swapchain composite alpha.
 *
 * Identifies how a swapchain is composited with other surfaces.
 *
 * All values of this type follow the format `PAL_COMPOSITE_ALPHA_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalCompositeAplha;

/**
 * @typedef PalSurfaceFormat
 * @brief Surface format.
 *
 * Identifies the format and color space of a swapchain.
 *
 * All values of this type follow the format `PAL_SURFACE_FORMAT_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalSurfaceFormat;

/**
 * @typedef PalWindowInstanceType
 * @brief Window instance type.
 *
 * Identifies the windowing system a surface is created for.
 *
 * All values of this type follow the format `PAL_WINDOW_INSTANCE_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalWindowInstanceType;

/**
 * @struct PalSurface
 * @brief Opaque handle to a surface.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalSurface PalSurface;

/**
 * @defgroup present_modes Swapchain Present Modes
 * @ingroup pal_graphics
 * @{
 */
#define PAL_PRESENT_MODE_FIFO 0 /**< V-sync present mode. */
#define PAL_PRESENT_MODE_IMMEDIATE 1 /**< Present right away, may tear. */
#define PAL_PRESENT_MODE_MAILBOX 2 /**< Newest image replaces the queued one, no tearing. */
#define PAL_PRESENT_MODE_COUNT 3 /**< Number of swapchain present modes. */
/** @} */

/**
 * @defgroup composite_alphas Swapchain Composite Alphas
 * @ingroup pal_graphics
 * @{
 */
#define PAL_COMPOSITE_ALPHA_OPAQUE 0 /**< Alpha is ignored. */
#define PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED 1 /**< Color is already multiplied by alpha. */
#define PAL_COMPOSITE_ALPHA_POST_MULTIPLIED 2 /**< Color is multiplied by alpha on composite. */
#define PAL_COMPOSITE_ALPHA_COUNT 3 /**< Number of swapchain composite alphas. */
/** @} */

/**
 * @defgroup surface_formats Surface Formats
 * @ingroup pal_graphics
 * @{
 */
#define PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR 0 /**< 8-bit BGRA UNORM with the sRGB nonlinear color space. */
#define PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR 1 /**< 8-bit BGRA sRGB with the sRGB nonlinear color space. */
#define PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR 2 /**< 8-bit RGBA UNORM with the sRGB nonlinear color space. */
#define PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10 3 /**< 16-bit float RGBA with the HDR10 color space. */
#define PAL_SURFACE_FORMAT_COUNT 4 /**< Number of surface formats. */
/** @} */

/**
 * @defgroup window_instance_types Window Instance Types
 * @ingroup pal_graphics
 * @{
 */
#define PAL_WINDOW_INSTANCE_TYPE_WAYLAND 0 /**< Wayland display. */
#define PAL_WINDOW_INSTANCE_TYPE_X11 1 /**< X11 display (Xlib). */
#define PAL_WINDOW_INSTANCE_TYPE_XCB 2 /**< XCB connection. */
#define PAL_WINDOW_INSTANCE_TYPE_WIN32 3 /**< Win32 application instance. */
#define PAL_WINDOW_INSTANCE_TYPE_COUNT 4 /**< Number of window instance types. */
/** @} */

/**
 * @struct PalSurfaceCapabilities
 * @brief Adapter surface capabilities.
 *
 * Filled in by @ref palGetSurfaceCapabilities().
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalSurfaceCapabilities {
    uint32_t supportedPresentModes;    /**< Mask of supported present modes. */
    uint32_t supportedCompositeAlphas; /**< Mask of supported composite alphas. */
    uint32_t supportedFormats;         /**< Mask of supported surface formats. */
    uint32_t minImageCount;            /**< Minimum image or back buffer count. */
    uint32_t maxImageCount;            /**< Maximum image or back buffer count. */
    uint32_t minImageWidth;            /**< Minimum image width in pixels. */
    uint32_t minImageHeight;           /**< Minimum image height in pixels. */
    uint32_t maxImageWidth;            /**< Maximum image width in pixels. */
    uint32_t maxImageHeight;           /**< Maximum image height in pixels. */
    uint32_t maxImageArrayLayers;      /**< Maximum number of image layers. */
} PalSurfaceCapabilities;

/**
 * @brief Creates a presentation surface for a window.
 *
 * The created surface must be destroyed using @ref palDestroySurface().
 *
 * @ref PAL_ADAPTER_FEATURE_SWAPCHAIN feature must be supported and enabled when
 * creating the device or this function results in undefined behavior.
 *
 * @param[in] device Device that creates the surface.
 * @param[in] window Window to create the surface for.
 * @param[in] windowInstance Instance of the window.
 * @param[in] instanceType Instance type of the window.
 * @param[out] outSurface Output handle to receive the created surface.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `device` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palDestroySurface
 */
PAL_API PalResult PAL_CALL palCreateSurface(
    PalDevice* device,
    void* window,
    void* windowInstance,
    PalWindowInstanceType instanceType,
    PalSurface** outSurface);

/**
 * @brief Destroys a presentation surface.
 *
 * @param[in] surface Surface to destroy.
 *
 * @thread-safety The device used to create `surface` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateSurface
 */
PAL_API void PAL_CALL palDestroySurface(PalSurface* surface);

/**
 * @brief Gets surface capabilities of a device.
 *
 * @ref PAL_ADAPTER_FEATURE_SWAPCHAIN feature must be supported and enabled when
 * creating the device or this function results in undefined behavior.
 *
 * @param[in] device Device to query its surface capabilities.
 * @param[in] surface Surface to query capabilities.
 * @param[out] caps Output struct to receive surface capabilities.
 *
 * @thread-safety `caps` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palGetSurfaceCapabilities(
    PalDevice* device,
    PalSurface* surface,
    PalSurfaceCapabilities* caps);

#endif // PAL_GRAPHICS_SURFACE_H