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

#ifndef PAL_GRAPHICS_IMAGEVIEW_H
#define PAL_GRAPHICS_IMAGEVIEW_H

#include "image.h"

/**
 * @defgroup image_view_types Image View Types
 * @brief Image view types
 * 
 * @{
 */
#define PAL_IMAGE_VIEW_TYPE_1D 0
#define PAL_IMAGE_VIEW_TYPE_1D_ARRAY 1
#define PAL_IMAGE_VIEW_TYPE_2D 2
#define PAL_IMAGE_VIEW_TYPE_2D_ARRAY 3
#define PAL_IMAGE_VIEW_TYPE_3D 4
#define PAL_IMAGE_VIEW_TYPE_CUBE 5
#define PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY 6
#define PAL_IMAGE_VIEW_TYPE_COUNT 7
/** @} */

/**
 * @typedef PalImageViewType
 * @brief Image view types.
 * 
 * All values of this type follow the format `PAL_IMAGE_VIEW_TYPE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalImageViewType;

/**
 * @struct PalImageView
 * @brief Opaque handle to an image view.
 *
 * @since Added in version 2.0
 */
typedef struct PalImageView PalImageView;

/**
 * @struct PalImageViewCreateInfo
 * @brief Contains creation parameters of an image view.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalImageViewCreateInfo::format
 * The format of the image view. It must be compatible with the image format.
 * 
 * @var PalImageViewCreateInfo::type
 * The type of the image view. It must be compatible with the image format.
 * 
 * @var PalImageViewCreateInfo::subresourceRange
 * The subresource range of the image to create the image view from.
 */
typedef struct PalImageViewCreateInfo
{
    PalFormat format;
    PalImageViewType type;
    PalImageSubresourceRange subresourceRange;
} PalImageViewCreateInfo;

/**
 * @brief Create an image view.
 *
 * The created image view must be destroyed using `palDestroyImageView()`.
 *
 * PalImageViewCreateInfo::type must be compatible by the type of the base image. Eg. A 2D base
 * image must be have an image view of either `PAL_IMAGE_VIEW_TYPE_2D` or
 * `PAL_IMAGE_VIEW_TYPE_2D_ARRAY`.
 *
 * `PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY` must be supported and enabled by the device
 * used to create the image view if `PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY` will be used.
 * Otherwise behavior is undefined.
 *
 * @param[in] device Device that creates the image view.
 * @param[in] image Image to create the image view with.
 * @param[in] info Pointer to a PalImageViewCreateInfo struct that specifies parameters.
 * @param[out] outImageView Pointer to a PalImageView to recieve the created image view.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palDestroyImageView
 */
PAL_API PalResult PAL_CALL palCreateImageView(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** outImageView);

/**
 * @brief Destroy an image view.
 *
 * @param[in] imageView Image view to destroy.
 *
 * Thread safety: Thread safe if the device used to create the image view is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateImageView
 */
PAL_API void PAL_CALL palDestroyImageView(PalImageView* imageView);

#endif // PAL_GRAPHICS_IMAGEVIEW_H