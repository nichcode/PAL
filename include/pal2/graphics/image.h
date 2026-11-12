/**
 * @file image.h
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

#ifndef PAL_GRAPHICS_IMAGE_H
#define PAL_GRAPHICS_IMAGE_H

#include "types.h"

/**
 * @defgroup image_types Image Types
 * @ingroup pal_graphics
 * @{
 */
#define PAL_IMAGE_TYPE_1D 0 /**< 1D image. */
#define PAL_IMAGE_TYPE_2D 1 /**< 2D image. */
#define PAL_IMAGE_TYPE_3D 2 /**< 3D image. */
#define PAL_IMAGE_TYPE_COUNT 3 /**< Number of image types. */
/** @} */

/**
 * @defgroup image_aspect Image Aspects
 * @ingroup pal_graphics
 * @{
 */
#define PAL_IMAGE_ASPECT_COLOR 0 /**< Color aspect. */
#define PAL_IMAGE_ASPECT_DEPTH 1 /**< Depth aspect. */
#define PAL_IMAGE_ASPECT_STENCIL 2 /**< Stencil aspect. */
#define PAL_IMAGE_ASPECT_DEPTH_STENCIL 3 /**< Depth and stencil aspects. */
#define PAL_IMAGE_ASPECT_COUNT 4 /**< Number of image aspects. */
/** @} */

/**
 * @defgroup image_usages Image Usages
 * @ingroup pal_graphics
 * @{
 */
#define PAL_IMAGE_USAGE_UNDEFINED 0 /**< Invalid image usage. */
#define PAL_IMAGE_USAGE_COLOR_ATTACHEMENT (1U << 0) /**< Color attachment. */
#define PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT (1U << 1) /**< Depth/stencil attachment. */
#define PAL_IMAGE_USAGE_TRANSFER_SRC (1U << 2) /**< Source of a transfer (copy) operation. */
#define PAL_IMAGE_USAGE_TRANSFER_DST (1U << 3) /**< Destination of a transfer (copy) operation. */
#define PAL_IMAGE_USAGE_STORAGE (1U << 4) /**< Storage image. */
#define PAL_IMAGE_USAGE_SAMPLED (1U << 5) /**< Sampled image. */
/** @} */

/**
 * @defgroup image_view_types Image View Types
 * @ingroup pal_graphics
 * @{
 */
#define PAL_IMAGE_VIEW_TYPE_1D 0 /**< 1D image view. */
#define PAL_IMAGE_VIEW_TYPE_1D_ARRAY 1 /**< 1D array image view. */
#define PAL_IMAGE_VIEW_TYPE_2D 2 /**< 2D image view. */
#define PAL_IMAGE_VIEW_TYPE_2D_ARRAY 3 /**< 2D array image view. */
#define PAL_IMAGE_VIEW_TYPE_3D 4 /**< 3D image view. */
#define PAL_IMAGE_VIEW_TYPE_CUBE 5 /**< Cube map image view. */
#define PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY 6 /**< Cube map array image view. */
#define PAL_IMAGE_VIEW_TYPE_COUNT 7 /**< Number of image view types. */
/** @} */

/**
 * @defgroup image_memory_usages Image Memory Usages
 * @ingroup pal_graphics
 * @{
 */
#define PAL_IMAGE_MEMORY_USAGE_MANUAL 0 /**< PAL should not allocate memory for the image. */
#define PAL_IMAGE_MEMORY_USAGE_AUTO_GPU_ONLY 1 /**< PAL allocates GPU memory for the image. */
#define PAL_IMAGE_MEMORY_USAGE_COUNT 2 /**< Number of image memory usages. */
/** @} */

/**
 * @struct PalImageInfo
 * @brief Image information.
 *
 * Filled in by @ref palGetImageInfo().
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalImageInfo {
    PalImageUsages usages; /**< Bitmask of image usages. */
    uint32_t width;             /**< Image width in pixels. */
    uint32_t height;            /**< Image height in pixels. */
    uint32_t depth;             /**< Image depth in pixels. */
    uint32_t arrayLayerCount;   /**< Number of array layers. */
    uint32_t mipLevelCount;     /**< Number of mipmap levels. */
    PalSampleCount sampleCount; /**< Number of samples per pixel. */
    PalImageType type;          /**< Image type. */
    PalFormat format;           /**< Image format. */
    PalBool belongsToSwapchain; /**< Whether image belongs to a swapchain. */
} PalImageInfo;

/**
 * @struct PalImageCopyInfo
 * @brief Image copy information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalImageCopyInfo {
    PalImageAspect aspect;       /**< Image aspect. */
    uint32_t dstMipLevel;        /**< Mipmap level of destination image. */
    uint32_t srcMipLevel;        /**< Mipmap level of source image. */
    uint32_t dstStartArrayLayer; /**< Starting array layer of destination image. */
    uint32_t srcStartArrayLayer; /**< Starting array layer of source image. */
    uint32_t arrayLayerCount;    /**< Number of array layers of destination and source images. */
    int32_t dstOffsetX;          /**< X destination image offset in texels. */
    int32_t srcOffsetX;          /**< X source image offset in texels. */
    int32_t dstOffsetY;          /**< Y destination image offset in texels. */
    int32_t srcOffsetY;          /**< Y source image offset in texels. */
    int32_t dstOffsetZ;          /**< Z destination image offset in texels. */
    int32_t srcOffsetZ;          /**< Z source image offset in texels. */
    uint32_t width;              /**< Width of the region to copy in texels. */
    uint32_t height;             /**< Height of the region to copy in texels. */
    uint32_t depth;              /**< Depth of the region to copy in texels. */
} PalImageCopyInfo;

/**
 * @struct PalImageSubresourceRange
 * @brief Image subresource range.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalImageSubresourceRange {
    PalImageAspect aspect;    /**< Image aspect. */
    uint32_t startMipLevel;   /**< Image start mipmap level. */
    uint32_t mipLevelCount;   /**< Number of image mipmap levels. */
    uint32_t startArrayLayer; /**< Image start array layer. */
    uint32_t layerArrayCount; /**< Number of image array layers. */
} PalImageSubresourceRange;

/**
 * @struct PalImageCreateInfo
 * @brief Image creation parameters.
 *
 * `memoryUsage` may be one of the following:
 *
 * - PAL_IMAGE_MEMORY_USAGE_MANUAL - PAL does not allocate memory for the image.
 *   Call @ref palGetImageMemoryRequirements() to get the
 *   memory requirement of the image and allocate memory with @ref palAllocateMemory()
 *   after the image has been created. Users are required to free the allocated
 *   memory with @ref palFreeMemory() when no longer needed.
 *
 * - PAL_IMAGE_MEMORY_USAGE_AUTO_GPU_ONLY - PAL allocates GPU only memory for the image
 *   after creation. This is recommended if a custom allocator or sub-allocations
 *   allocator is used.
 *
 * This struct is used only during @ref palCreateImage() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalImageCreateInfo {
    PalImageUsages usages;    /**< Bitmask of image usages. */
    uint32_t width;           /**< Image width in pixels. */
    uint32_t height;          /**< Image height in pixels. */
    uint32_t depth;           /**< Image depth in pixels. */
    uint32_t arrayLayerCount; /**< Number of array layers. */
    uint32_t mipLevelCount;   /**< Number of mipmap levels. */
    PalSampleCount sampleCount;      /**< Number of samples per pixel. */
    PalImageType type;               /**< Image type. */
    PalFormat format;                /**< Image format. */
    PalImageMemoryUsage memoryUsage; /**< Image memory usage. */
} PalImageCreateInfo;

/**
 * @struct PalImageViewCreateInfo
 * @brief Image view creation parameters.
 *
 * `format` and `type` must be compatible with the specified image the
 * image view is being created from.
 *
 * This struct is used only during @ref palCreateImageView() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalImageViewCreateInfo {
    PalFormat format;                          /**< Image view format. */
    PalImageViewType type;                     /**< Image view type. */
    PalImageSubresourceRange subresourceRange; /**< Image view range. */
} PalImageViewCreateInfo;

/**
 * @brief Creates an image.
 *
 * The created image must be destroyed using @ref palDestroyImage().
 *
 * This function creates an image using the specified creation parameters.
 * `info` must remain valid for the duration of this function. PAL does not
 * copy its contents.
 *
 * `info->width`, `info->height` and `info->sampleCount` must not be greater than the limits
 * of the adapter used to create the device. Check limits with @ref palGetAdapterCapabilities().
 *
 * @param[in] device Device that creates the image.
 * @param[in] info Image creation parameters.
 * @param[out] outImage Output handle to receive the created image.
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
 * @sa palDestroyImage
 */
PAL_API PalResult PAL_CALL palCreateImage(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage);

/**
 * @brief Destroys an image.
 *
 * @param[in] image Image to destroy.
 *
 * @thread-safety The device used to create `image` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateImage
 */
PAL_API void PAL_CALL palDestroyImage(PalImage* image);

/**
 * @brief Gets information about an image.
 *
 * @param[in] image Image to query its information.
 * @param[out] info Output struct to receive image information.
 *
 * @thread-safety `info` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateImage
 */
PAL_API void PAL_CALL palGetImageInfo(
    PalImage* image,
    PalImageInfo* info);

/**
 * @brief Gets memory requirements of an image.
 *
 * @param[in] image Image to query its memory requirements.
 * @param[out] requirements Output struct to receive image memory requirements.
 *
 * @thread-safety `requirements` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palGetImageMemoryRequirements(
    PalImage* image,
    PalMemoryRequirements* requirements);

/**
 * @brief Binds an allocated memory to an image.
 *
 * The memory size and alignment should match the requirements of the image.
 * Get the requirements with @ref palGetImageMemoryRequirements().
 *
 * @param[in] image Image to bind memory to.
 * @param[in] memory Memory to bind.
 * @param[in] offset Starting point within the memory.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `image` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_INVALID_OPERATION
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palGetImageMemoryRequirements
 */
PAL_API PalResult PAL_CALL palBindImageMemory(
    PalImage* image,
    PalMemory* memory,
    uint64_t offset);

/**
 * @brief Creates an image view.
 *
 * The created image view must be destroyed using @ref palDestroyImageView().
 *
 * This function creates an image view using the specified creation parameters.
 * `info` must remain valid for the duration of this function. PAL does not
 * copy its contents.
 *
 * `info->type` must be compatible by the type of the base image. For example, A 2D base
 * image must be have an image view of either `PAL_IMAGE_VIEW_TYPE_2D` or
 * `PAL_IMAGE_VIEW_TYPE_2D_ARRAY`.
 *
 * @ref PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY must be supported and enabled by the device
 * used to create the image view if `PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY` will be used.
 * Otherwise behavior is undefined.
 *
 * @param[in] device Device that creates the image view.
 * @param[in] image Image to create the image view with.
 * @param[in] info Image view creation parameters.
 * @param[out] imageView Output handle to receive the created image view.
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
 * @sa palDestroyImageView
 */
PAL_API PalResult PAL_CALL palCreateImageView(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** imageView);

/**
 * @brief Destroys an image view.
 *
 * @param[in] imageView Image view to destroy.
 *
 * @thread-safety The device used to create `imageView` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateImageView
 */
PAL_API void PAL_CALL palDestroyImageView(PalImageView* imageView);

#endif // PAL_GRAPHICS_IMAGE_H