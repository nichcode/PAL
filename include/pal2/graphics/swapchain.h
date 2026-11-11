/**
 * @file next.h
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

#ifndef PAL_GRAPHICS_NEXT_H
#define PAL_GRAPHICS_NEXT_H

#include "types.h"

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
#define PAL_WINDOW_INSTANCE_TYPE_WIN32 3 /**< Win32 module instance. */
#define PAL_WINDOW_INSTANCE_TYPE_COUNT 4 /**< Number of window instance types. */
/** @} */

/**
 * @struct PalSwapchainNextImageInfo
 * @brief Swapchain next image information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalSwapchainNextImageInfo {
    uint64_t timeout;              /**< Timeout in milliseconds. */
    PalSemaphore* signalSemaphore; /**< Semaphore to signal when the image is acquired. */
    PalFence* fence;               /**< Fence to signal when the image is acquired. */
} PalSwapchainNextImageInfo;

/**
 * @struct PalSwapchainCreateInfo
 * @brief Swapchain creation parameters.
 *
 * This struct is used only during @ref palCreateSwapchain() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalSwapchainCreateInfo {
    PalBool clipped;                  /**< Whether to discard pixels that are not visible. */
    uint32_t width;                   /**< Width in pixels. */
    uint32_t height;                  /**< Height in pixels. */
    uint32_t imageCount;              /**< Number of images or back buffers. */
    uint32_t imageArrayLayerCount;    /**< Number of array layers. */
    PalPresentMode presentMode;       /**< Present mode. */
    PalCompositeAplha compositeAlpha; /**< Composite alpha. */
    PalSurfaceFormat format;          /**< Swapchain format. */
} PalSwapchainCreateInfo;

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

/**
 * @brief Creates a swapchain.
 *
 * The created swapchain must be destroyed using @ref palDestroySwapchain().
 *
 * This function creates a swapchain using the specified creation parameters.
 * `info` must remain valid for the duration of this function. PAL does not
 * copy its contents.
 *
 * @ref PAL_ADAPTER_FEATURE_SWAPCHAIN feature must be supported and enabled when
 * creating the device or this function results in undefined behavior.
 *
 * @param[in] device Device that creates the swapchain.
 * @param[in] queue Queue to create swapchain with. This must be a graphics queue.
 * @param[in] surface Surface to create swapchain with.
 * @param[in] info Swapchain creation parameters.
 * @param[out] outSwapchain Output handle to receive the created swapchain.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `device` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palDestroySwapchain
 */
PAL_API PalResult PAL_CALL palCreateSwapchain(
    PalDevice* device,
    PalQueue* queue,
    PalSurface* surface,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain);

/**
 * @brief Destroys a swapchain.
 *
 * @param[in] swapchain Swapchain to destroy.
 *
 * @thread-safety The device used to create `swapchain` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateSwapchain
 */
PAL_API void PAL_CALL palDestroySwapchain(PalSwapchain* swapchain);

/**
 * @brief Gets a swapchain image by index.
 *
 * @param[in] swapchain Swapchain to get image from.
 * @param[in] index Index of the swapchain image. Must be less than the swapchain image count.
 * @return A pointer to the image on success otherwise `nullptr` on failure.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palGetNextSwapchainImage
 */
PAL_API PalImage* PAL_CALL palGetSwapchainImage(
    PalSwapchain* swapchain,
    uint32_t index);

/**
 * @brief Acquires the next available image from the swapchain.
 *
 * @param[in] swapchain Swapchain to get image index from.
 * @param[in] info Next image acquisition parameters.
 * @param[out] outIndex Output to receive the next image index.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `swapchain` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_TIMEOUT
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palGetSwapchainImage
 */
PAL_API PalResult PAL_CALL palGetNextSwapchainImage(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    uint32_t* outIndex);

/**
 * @brief Presents a swapchain image.
 *
 * @param[in] swapchain Swapchain containing the image to present.
 * @param[in] imageIndex Index of the swapchain image to present.
 * @param[in] waitSemaphore Semaphore to wait on before presenting the image.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety Must be called from the main thread.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palPresentSwapchain(
    PalSwapchain* swapchain,
    uint32_t imageIndex,
    PalSemaphore* waitSemaphore);

/**
 * @brief Resizes the specified swapchain.
 *
 * The swapchain images must not be in use when this function is called. Image views and other
 * resources that reference the old swapchain images must be destroyed before this call and
 * recreated afterward.
 *
 * @param[in] swapchain Swapchain to resize.
 * @param[in] newWidth New swapchain width.
 * @param[in] newHeight New swapchain height.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `swapchain` must be externally synchronized.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_DEVICE_LOST.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palResizeSwapchain(
    PalSwapchain* swapchain,
    uint32_t newWidth,
    uint32_t newHeight);

#endif // PAL_GRAPHICS_NEXT_H