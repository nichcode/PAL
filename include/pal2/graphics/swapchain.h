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

#ifndef PAL_GRAPHICS_SWAPCHAIN_H
#define PAL_GRAPHICS_SWAPCHAIN_H

#include "device.h"
#include "sync.h"

/**
 * @struct PalSwapchain
 * @brief Opaque handle to a swapchain.
 *
 * @since Added in version 2.0
 */
typedef struct PalSwapchain PalSwapchain;

/**
 * @struct PalSwapchainNextImageInfo
 * @brief Contains information about getting the next swapchain image.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalSwapchainNextImageInfo::timeout
 * The timeout in milliseconds to wait.
 * 
 * @var PalSwapchainNextImageInfo::signalSemaphore
 * The semaphore to signal when the image is acquired.
 * 
 * @var PalSwapchainNextImageInfo::fence
 * The fence to signal when the image is acquired.
 */
typedef struct PalSwapchainNextImageInfo
{
    uint64_t timeout;
    PalSemaphore* signalSemaphore;
    PalFence* fence;
} PalSwapchainNextImageInfo;

/**
 * @struct PalSwapchainCreateInfo
 * @brief Contains creation parameters of a swapchain.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalSwapchainCreateInfo::clipped
 * `PAL_TRUE` to discard pixels that are not visible.
 * 
 * @var PalSwapchainCreateInfo::width
 * The width of the swapchain in pixels.
 * 
 * @var PalSwapchainCreateInfo::height
 * The height of the swapchain in pixels.
 * 
 * @var PalSwapchainCreateInfo::imageCount
 * The number of images or back buffers of the swapchain.
 * 
 * @var PalSwapchainCreateInfo::imageArrayLayerCount
 * The number of array layers of the swapchain.
 * 
 * @var PalSwapchainCreateInfo::presentMode
 * The present mode of the swapchain.
 * 
 * @var PalSwapchainCreateInfo::compositeAlpha
 * The composite alpha of the swapchain.
 * 
 * @var PalSwapchainCreateInfo::format
 * The format of the swapchain. This is the format and colorspace.
 * (eg. `PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR`).
 */
typedef struct PalSwapchainCreateInfo
{
    PalBool clipped; 
    uint32_t width;   
    uint32_t height;  
    uint32_t imageCount;
    uint32_t imageArrayLayerCount;
    PalPresentMode presentMode;
    PalCompositeAplha compositeAlpha;
    PalSurfaceFormat format;
} PalSwapchainCreateInfo;

/**
 * @brief Create a swaphain.
 *
 * The created swapchain must be destroyed using `palDestroySwapchain()`.
 *
 * `PAL_ADAPTER_FEATURE_SWAPCHAIN` must be supported and enabled when creating the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] device Device that creates the swapchain.
 * @param[in] queue Queue to create swapchain with. This must be a graphics queue.
 * @param[in] surface Surface to create swapchain with.
 * @param[in] info Pointer to a PalSwapchainCreateInfo struct that specifies parameters.
 * @param[out] outSwapchain Pointer to a PalSwapchain to recieve the created swapchain.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palDestroySwapchain
 */
PAL_API PalResult PAL_CALL palCreateSwapchain(
    PalDevice* device,
    PalQueue* queue,
    PalSurface* surface,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain);

/**
 * @brief Destroy a swapchain.
 *
 * @param[in] swapchain Swapchain to destroy.
 *
 * Thread safety: Thread safe if the device used to create the swapchain is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateSwapchain
 */
PAL_API void PAL_CALL palDestroySwapchain(PalSwapchain* swapchain);

/**
 * @brief Get a swapchain image from the list of images with an index.
 *
 * @param[in] swapchain Swapchain to get image from.
 * @param[in] index Index of image in the list. Must not be greater than the image count.
 *
 * @return A pointer to the image on success otherwise `nullptr` on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
 * @sa palGetNextSwapchainImage
 */
PAL_API PalImage* PAL_CALL palGetSwapchainImage(
    PalSwapchain* swapchain,
    uint32_t index);

/**
 * @brief Get the next available image from the swapchain image list.
 *
 * @param[in] swapchain Swapchain to get image index from.
 * @param[in] info Pointer to a PalSwapchainNextImageInfo struct that specifies parameters.
 * @param[out] outIndex Pointer to a uint32_t to recieve the next image index.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `swapchain` externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palGetSwapchainImage
 */
PAL_API PalResult PAL_CALL palGetNextSwapchainImage(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    uint32_t* outIndex);

/**
 * @brief Present the swapchain.
 *
 * @param[in] swapchain Swapchain to present.
 * @param[in] imageIndex Swapchain image index to present.
 * @param[in] waitSemaphore The semaphore to wait for.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palPresentSwapchain(
    PalSwapchain* swapchain,
    uint32_t imageIndex,
    PalSemaphore* waitSemaphore);

/**
 * @brief Resize the provided swapchain.
 *
 * The swapchain images must not be in use before this call. All resources (image views) that
 * reference the swapchain images must be destroyed and recreated.
 *
 * @param[in] swapchain Swapchain to resize.
 * @param[in] newWidth The new width of the swapchain.
 * @param[in] newHeight The new height of the swapchain.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `swapchain` externally synchronized.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palResizeSwapchain(
    PalSwapchain* swapchain,
    uint32_t newWidth,
    uint32_t newHeight);

#endif // PAL_GRAPHICS_SWAPCHAIN_H