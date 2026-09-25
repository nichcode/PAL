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

#ifndef PAL_GRAPHICS_IMAGE_H
#define PAL_GRAPHICS_IMAGE_H

#include "device.h"
#include "types.h"

/**
 * @defgroup image_types Image Types
 * @brief Image types
 * 
 * @{
 */
#define PAL_IMAGE_TYPE_1D 0
#define PAL_IMAGE_TYPE_2D 1
#define PAL_IMAGE_TYPE_3D 2
#define PAL_IMAGE_TYPE_COUNT 3
/** @} */

/**
 * @defgroup image_aspects Image Aspects
 * @brief Image aspects
 * 
 * @{
 */
#define PAL_IMAGE_ASPECT_COLOR 0
#define PAL_IMAGE_ASPECT_DEPTH 1
#define PAL_IMAGE_ASPECT_STENCIL 2
#define PAL_IMAGE_ASPECT_DEPTH_STENCIL 3
#define PAL_IMAGE_ASPECT_COUNT 4
/** @} */

/**
 * @defgroup image_usages Image Usages
 * @brief Image usages
 * 
 * @{
 */
#define PAL_IMAGE_USAGE_UNDEFINED 0
#define PAL_IMAGE_USAGE_COLOR_ATTACHEMENT (1U << 0)
#define PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT (1U << 1)
#define PAL_IMAGE_USAGE_TRANSFER_SRC (1U << 2)
#define PAL_IMAGE_USAGE_TRANSFER_DST (1U << 3)
#define PAL_IMAGE_USAGE_STORAGE (1U << 4)
#define PAL_IMAGE_USAGE_SAMPLED (1U << 5)
/** @} */

/**
 * @defgroup image_memory_usages Image Memory Usages
 * @brief Image memory usages
 * 
 * @{
 */
#define PAL_IMAGE_MEMORY_USAGE_MANUAL 0
#define PAL_IMAGE_MEMORY_USAGE_AUTO_GPU_ONLY 1
#define PAL_IMAGE_MEMORY_USAGE_COUNT 2
/** @} */

/**
 * @typedef PalImageUsages
 * @brief Image usages. 
 * 
 * Multiple image usages can be OR'ed together using bitwise OR operator (`|`).
 * 
 * All values of this type follow the format `PAL_IMAGE_USAGE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalImageUsages;

/**
 * @typedef PalImageType
 * @brief Image types.
 * 
 * All values of this type follow the format `PAL_IMAGE_TYPE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalImageType;

/**
 * @typedef PalImageAspect
 * @brief Image aspects.
 * 
 * All values of this type follow the format `PAL_IMAGE_ASPECT_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalImageAspect;

/**
 * @typedef PalImageMemoryUsage
 * @brief Image memory usages.
 * 
 * All values of this type follow the format `PAL_IMAGE_MEMORY_USAGE_*` 
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalImageMemoryUsage;

/**
 * @struct PalImage
 * @brief Opaque handle to an image.
 *
 * @since Added in version 2.0
 */
typedef struct PalImage PalImage;

/**
 * @struct PalImageInfo
 * @brief Contains information about an image.
 *
 * @since Added in version 2.0
 * 
 * @var PalImageInfo::usages
 * A bitmask of image usages.
 * 
 * @var PalImageInfo::width
 * The width of the image in pixels.
 * 
 * @var PalImageInfo::height
 * The height of the image in pixels.
 * 
 * @var PalImageInfo::depth
 * The depth of the image in pixels.
 * 
 * @var PalImageInfo::arrayLayerCount
 * The number of array layers of the image.
 * 
 * @var PalImageInfo::mipLevelCount
 * The number of mipmap levels of the image.
 * 
 * @var PalImageInfo::sampleCount
 * The `MSAA` samples of the image.
 * 
 * @var PalImageInfo::type
 * The type of the image (eg. `PAL_IMAGE_TYPE_2D`).
 * 
 * @var PalImageInfo::format
 * The format of the image (eg. `PAL_FORMAT_R8G8B8A8_UNORM`).
 * 
 * @var PalImageInfo::belongsToSwapchain
 * If `PAL_TRUE`, the image belongs to a swapchain.
 */
typedef struct PalImageInfo
{
    PalImageUsages usages;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t arrayLayerCount;
    uint32_t mipLevelCount; 
    PalSampleCount sampleCount;
    PalImageType type;
    PalFormat format; 
    PalBool belongsToSwapchain;
} PalImageInfo;

/**
 * @struct PalImageSubresourceRange
 * @brief Contains subresource range for images.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalImageSubresourceRange::aspect
 * The image aspect. (eg. `PAL_IMAGE_ASPECT_COLOR`).
 * 
 * @var PalImageSubresourceRange::startMipLevel
 * The start mipmap level of the image.
 * 
 * @var PalImageSubresourceRange::mipLevelCount
 * The number of mipmap levels of the image.
 * 
 * @var PalImageSubresourceRange::startArrayLayer
 * The start array layer of the image.
 * 
 * @var PalImageSubresourceRange::layerArrayCount
 * The number of array layers of the image.
 */
typedef struct PalImageSubresourceRange
{
    PalImageAspect aspect;
    uint32_t startMipLevel;
    uint32_t mipLevelCount;
    uint32_t startArrayLayer;
    uint32_t layerArrayCount;
} PalImageSubresourceRange;

/**
 * @struct PalBufferImageCopyInfo
 * @brief Contains information for image to buffer and vice versa copies.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalBufferImageCopyInfo::bufferOffset
 * The offset in bytes into the buffer.
 * 
 * @var PalBufferImageCopyInfo::imageAspect
 * The aspect of the image.
 * 
 * @var PalBufferImageCopyInfo::bufferRowLength
 * The buffer row length in texels.
 * 
 * @var PalBufferImageCopyInfo::bufferImageHeight
 * The buffer image height in texels.
 * 
 * @var PalBufferImageCopyInfo::ImageMipLevel
 * The mipmap of the image.
 * 
 * @var PalBufferImageCopyInfo::ImageStartArrayLayer
 * The start array layer of the image.
 * 
 * @var PalBufferImageCopyInfo::ImageArrayLayerCount
 * The number of array layers of the image.
 * 
 * @var PalBufferImageCopyInfo::imageOffsetX
 * The x offset in bytes of the image.
 * 
 * @var PalBufferImageCopyInfo::imageOffsetY
 * The y offset in bytes of the image.
 * 
 * @var PalBufferImageCopyInfo::imageOffsetZ
 * The z offset in bytes of the image.
 * 
 * @var PalBufferImageCopyInfo::imageWidth
 * The width in bytes of the image.
 * 
 * @var PalBufferImageCopyInfo::imageHeight
 * The height in bytes of the image.
 * 
 * @var PalBufferImageCopyInfo::imageDepth
 * The depth in bytes of the image.
 */
typedef struct PalBufferImageCopyInfo
{
    uint64_t bufferOffset;
    PalImageAspect imageAspect;
    uint32_t bufferRowLength;
    uint32_t bufferImageHeight;
    uint32_t ImageMipLevel;
    uint32_t ImageStartArrayLayer;
    uint32_t ImageArrayLayerCount;
    int32_t imageOffsetX;
    int32_t imageOffsetY;
    int32_t imageOffsetZ;
    uint32_t imageWidth;
    uint32_t imageHeight;
    uint32_t imageDepth;
} PalBufferImageCopyInfo;

/**
 * @struct PalImageCopyInfo
 * @brief Contains information for image to image copies.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalImageCopyInfo::aspect
 * The aspect of the image.
 * 
 * @var PalImageCopyInfo::dstMipLevel
 * The destination mipmap level of the image.
 * 
 * @var PalImageCopyInfo::srcMipLevel
 * The source mipmap level of the image.
 * 
 * @var PalImageCopyInfo::dstStartArrayLayer
 * The start array layer of the destination image.
 * 
 * @var PalImageCopyInfo::srcStartArrayLayer
 * The start array layer of the source image.
 * 
 * @var PalImageCopyInfo::arrayLayerCount
 * The number of array layers of the destination and source images.
 * 
 * @var PalImageCopyInfo::dstOffsetX
 * The x offset in bytes of the destination image.
 * 
 * @var PalImageCopyInfo::srcOffsetX
 * The x offset in bytes of the source image.
 * 
 * @var PalImageCopyInfo::dstOffsetY
 * The y offset in bytes of the destination image.
 * 
 * @var PalImageCopyInfo::srcOffsetY
 * The y offset in bytes of the source image.
 * 
 * @var PalImageCopyInfo::dstOffsetZ
 * The z offset in bytes of the destination image.
 * 
 * @var PalImageCopyInfo::srcOffsetZ
 * The z offset in bytes of the source image.
 * 
 * @var PalImageCopyInfo::width
 * The width of the copy region into the source image.
 * 
 * @var PalImageCopyInfo::height
 * The height of the copy region into the source image.
 * 
 * @var PalImageCopyInfo::depth
 * The depth of the copy region into the source image.
 */
typedef struct PalImageCopyInfo
{
    PalImageAspect aspect;
    uint32_t dstMipLevel;
    uint32_t srcMipLevel;
    uint32_t dstStartArrayLayer;
    uint32_t srcStartArrayLayer;
    uint32_t arrayLayerCount;
    int32_t dstOffsetX;
    int32_t srcOffsetX;
    int32_t dstOffsetY;
    int32_t srcOffsetY;
    int32_t dstOffsetZ;
    int32_t srcOffsetZ;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
} PalImageCopyInfo;

/**
 * @struct PalImageStagingRequirements
 * @brief Contains requirements for an image staging buffer.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalImageStagingRequirements::bufferSize
 * The required buffer size.
 * 
 * @var PalImageStagingRequirements::bufferRowLength
 * The required buffer row length.
 * 
 * @var PalImageStagingRequirements::bufferImageHeight
 * The required buffer image height.
 */
typedef struct PalImageStagingRequirements
{
    uint64_t bufferSize; 
    uint32_t bufferRowLength;
    uint32_t bufferImageHeight;
} PalImageStagingRequirements;

/**
 * @struct PalImageCreateInfo
 * @brief Contains creation parameters of an image.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalImageCreateInfo::usages
 * A bitmask of the usages of the image.
 * The usages must be supported by ::format.
 * 
 * @var PalImageCreateInfo::width
 * The width of the image in bytes.
 * 
 * @var PalImageCreateInfo::height
 * The height of the image in bytes.
 * 
 * @var PalImageCreateInfo::depth
 * The depth of the image in bytes.
 * 
 * @var PalImageCreateInfo::arrayLayerCount
 * The number of array layers of the image.
 * 
 * @var PalImageCreateInfo::arrayLayerCount
 * The number of mipmap levels of the image.
 * 
 * @var PalImageCreateInfo::sampleCount
 * The `MSAA` samples of the image.
 * 
 * @var PalImageCreateInfo::type
 * The type of the image (eg. `PAL_IMAGE_TYPE_2D`).
 * 
 * @var PalImageCreateInfo::format
 * The format of the image (eg. `PAL_FORMAT_B8G8R8A8_UNORM`).
 * 
 * @var PalImageCreateInfo::memoryUsage
 * The memory usage model of the image (eg. `PAL_IMAGE_MEMORY_USAGE_MANUAL`).
 */
typedef struct PalImageCreateInfo
{
    PalImageUsages usages;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t arrayLayerCount;
    uint32_t mipLevelCount;
    PalSampleCount sampleCount;
    PalImageType type;
    PalFormat format;
    PalImageMemoryUsage memoryUsage;
} PalImageCreateInfo;

/**
 * @brief Create an image.
 *
 * The created image must be destroyed using `palDestroyImage()`.
 *
 * PalImageCreateInfo::width, PalImageCreateInfo::height and PalImageCreateInfo::sampleCount
 * must not be greater than the limits of the adapter used to create the device. Check
 * adapter capabilities for the limits.
 *
 * @param[in] device Device that creates the image.
 * @param[in] info Pointer to a PalImageCreateInfo struct that specifies parameters.
 * @param[out] outImage Pointer to a PalImage to recieve the created image.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palDestroyImage
 */
PAL_API PalResult PAL_CALL palCreateImage(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage);

/**
 * @brief Destroy an image.
 *
 * @param[in] image Image to destroy.
 *
 * Thread safety: Thread safe if the device used to create the image is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateImage
 */
PAL_API void PAL_CALL palDestroyImage(PalImage* image);

/**
 * @brief Get information about an image.
 *
 * This function also supports swapchain images.
 *
 * @param[in] image Image to query information on.
 * @param[out] info Pointer to a PalImageInfo to fill.
 *
 * Thread safety: Thread safe if `info` is per thread.
 *
 * @since Added in version 2.0
 * @sa palCreateImage
 */
PAL_API void PAL_CALL palGetImageInfo(
    PalImage* image,
    PalImageInfo* info);

/**
 * @brief Get memory requirements for the provided image.
 *
 * @param[in] image Image to query memory requirements on.
 * @param[out] requirements Pointer to a PalMemoryRequirements to fill.
 *
 * Thread safety: Thread safe if `requirements` is per thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetImageMemoryRequirements(
    PalImage* image,
    PalMemoryRequirements* requirements);

/**
 * @brief Bind an allocated memory to an image.
 *
 * The memory size and alignment should match the requirements of the image.
 * Get the requirements with palGetImageMemoryRequirements().
 *
 * @param[in] image Image to bind memory to.
 * @param[in] memory Memory to bind.
 * @param[in] offset Starting point within the memory.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `requirements` is per thread.
 *
 * @since Added in version 2.0
 * @sa palGetImageMemoryRequirements
 */
PAL_API PalResult PAL_CALL palBindImageMemory(
    PalImage* image,
    PalMemory* memory,
    uint64_t offset);

/**
 * @brief Compute requirements for an image staging buffer.
 *
 * This does not allocate memory for the buffer. This function is required for all
 * image copy staging buffers.
 *
 * `PalBufferImageCopyInfo::bufferRowLength` and `PalBufferImageCopyInfo::bufferImageHeight`
 * are hints. The driver might used it defaults if the requested is not supported. After this call,
 * set those values to the required ones from `requirements`.
 * If the driver supports the proivded, the values will be the same.
 *
 * @param[in] device The device to use.
 * @param[in] imageFormat Destination image format.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 * @param[out] requirements Pointer to a PalImageStagingRequirements to recieve the requirements
 *
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
 * @sa palWriteImageStaging
 */
PAL_API void PAL_CALL palComputeImageStagingRequirements(
    PalDevice* device,
    PalFormat imageFormat,
    const PalBufferImageCopyInfo* copyInfo,
    PalImageStagingRequirements* requirements);

/**
 * @brief Write data to an image staging buffer.
 *
 * @param[in] device The device to use.
 * @param[in] imageFormat Destination image format.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 * @param[out] srcData Pointer to the CPU visible memory with the data.
 * @param[out] ptr Pointer to the CPU visible memory. Must be mapped.
 *
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
 * @sa palComputeImageStagingRequirements
 */
PAL_API void PAL_CALL palWriteImageStaging(
    PalDevice* device,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo,
    void* srcData,
    void* ptr);

#endif // PAL_GRAPHICS_IMAGE_H