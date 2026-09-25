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

#ifndef PAL_GRAPHICS_SAMPLER_H
#define PAL_GRAPHICS_SAMPLER_H

#include "device.h"

/**
 * @defgroup filter_modes Filter Modes
 * @brief Filter modes
 * 
 * @{
 */
#define PAL_FILTER_MODE_NEAREST 0
#define PAL_FILTER_MODE_LINEAR 1
#define PAL_FILTER_MODE_COUNT 2
/** @} */

/**
 * @defgroup sampler_mipmap_modes Sampler Mipmap Modes
 * @brief Sampler mipmap modes
 * 
 * @{
 */
#define PAL_SAMPLER_MIPMAP_MODE_NEAREST 0
#define PAL_SAMPLER_MIPMAP_MODE_LINEAR 1
#define PAL_SAMPLER_MIPMAP_MODE_COUNT 2
/** @} */

/**
 * @defgroup sampler_address_modes Sampler Address Modes
 * @brief Sampler address modes
 * 
 * @{
 */
#define PAL_SAMPLER_ADDRESS_MODE_REPEAT 0
#define PAL_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT 1
#define PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE 2
#define PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER 3
#define PAL_SAMPLER_ADDRESS_MODE_COUNT 4
/** @} */

/**
 * @defgroup border_color Border Colors
 * @brief Border colors
 * 
 * @{
 */
#define PAL_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK 0
#define PAL_BORDER_COLOR_INT_TRANSPARENT_BLACK 1
#define PAL_BORDER_COLOR_FLOAT_OPAQUE_BLACK 2
#define PAL_BORDER_COLOR_INT_OPAQUE_BLACK 3
#define PAL_BORDER_COLOR_FLOAT_OPAQUE_WHITE 4
#define PAL_BORDER_COLOR_INT_OPAQUE_WHITE 5
#define PAL_BORDER_COLOR_COUNT 6
/** @} */

/**
 * @defgroup compare_operations Compare Operations
 * @brief Compare operations
 * 
 * @{
 */
#define PAL_COMPARE_OP_NEVER 0
#define PAL_COMPARE_OP_LESS 1
#define PAL_COMPARE_OP_EQUAL 2
#define PAL_COMPARE_OP_LESS_OR_EQUAL 3
#define PAL_COMPARE_OP_GREATER 4
#define PAL_COMPARE_OP_NOT_EQUAL 5
#define PAL_COMPARE_OP_GREATER_OR_EQUAL 6
#define PAL_COMPARE_OP_ALWAYS 7
#define PAL_COMPARE_OP_COUNT 8
/** @} */

/**
 * @typedef PalSamplerMipmapMode
 * @brief Sampler mipmap modes.
 * 
 * All values of this type follow the format `PAL_SAMPLER_MIPMAP_MODE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalSamplerMipmapMode;

/**
 * @typedef PalSamplerAddressMode
 * @brief Sampler address modes.
 * 
 * All values of this type follow the format `PAL_SAMPLER_ADDRESS_MODE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalSamplerAddressMode;

/**
 * @typedef PalFilterMode
 * @brief Filter modes.
 * 
 * All values of this type follow the format `PAL_FILTER_MODE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalFilterMode;

/**
 * @typedef PalBorderColor
 * @brief Border colors.
 * 
 * All values of this type follow the format `PAL_BORDER_COLOR_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalBorderColor;

/**
 * @typedef PalCompareOp
 * @brief Compare operation modes.
 * 
 * All values of this type follow the format `PAL_COMPARE_OP_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalCompareOp;

/**
 * @struct PalSampler
 * @brief Opaque handle to a sampler.
 *
 * @since Added in version 2.0
 */
typedef struct PalSampler PalSampler;

/**
 * @struct PalSamplerCreateInfo
 * @brief Contains creation parameters of a sampler.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalSamplerCreateInfo::enableCompare
 * `PAL_TRUE` to enable compare operations.
 * 
 * @var PalSamplerCreateInfo::enableAnisotropy
 * `PAL_TRUE` to enable texture filtering.
 * 
 * @var PalSamplerCreateInfo::mipLodBias
 * The offset applied to the selected mipmap level.
 * 
 * @var PalSamplerCreateInfo::minLod
 * The minimum mipmap level allowed.
 * 
 * @var PalSamplerCreateInfo::maxLod
 * The maximum mipmap level allowed.
 * 
 * @var PalSamplerCreateInfo::maxAnisotropy
 * The maximum texture filtering level.
 * 
 * @var PalSamplerCreateInfo::minFilterMode
 * The filter mode used for minification.
 * 
 * @var PalSamplerCreateInfo::magFilterMode
 * The filter mode used for magnification.
 * 
 * @var PalSamplerCreateInfo::mipmapMode
 * The mipmap filtering mode.
 * 
 * @var PalSamplerCreateInfo::addressModeU
 * The address mode for the u texture coordinate.
 * 
 * @var PalSamplerCreateInfo::addressModeV
 * The address mode for the v texture coordinate.
 * 
 * @var PalSamplerCreateInfo::addressModeW
 * The address mode for the w texture coordinate.
 * 
 * @var PalSamplerCreateInfo::compareOp
 * The compare operation.
 * 
 * @var PalSamplerCreateInfo::borderColor
 * The color used outside the texture.
 */
typedef struct PalSamplerCreateInfo
{
    PalBool enableCompare;
    PalBool enableAnisotropy;
    float mipLodBias;
    float minLod;    
    float maxLod;    
    float maxAnisotropy; 
    PalFilterMode minFilterMode;
    PalFilterMode magFilterMode;
    PalSamplerMipmapMode mipmapMode;
    PalSamplerAddressMode addressModeU;
    PalSamplerAddressMode addressModeV;
    PalSamplerAddressMode addressModeW;
    PalCompareOp compareOp;   
    PalBorderColor borderColor;
} PalSamplerCreateInfo;

/**
 * @brief Create a sampler.
 *
 * The created sampler must be destroyed using `palDestroySampler()`.
 *
 * @param[in] device Device that creates the sampler.
 * @param[in] info Pointer to a PalSamplerCreateInfo struct that specifies parameters.
 * @param[out] outSampler Pointer to a PalSampler to recieve the created sampler.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palDestroySampler
 */
PAL_API PalResult PAL_CALL palCreateSampler(
    PalDevice* device,
    const PalSamplerCreateInfo* info,
    PalSampler** outSampler);

/**
 * @brief Destroy a sampler.
 *
 * @param[in] sampler Sampler to destroy.
 *
 * Thread safety: Thread safe if the device used to create the sampler is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateSampler
 */
PAL_API void PAL_CALL palDestroySampler(PalSampler* sampler);

#endif // PAL_GRAPHICS_SAMPLER_H