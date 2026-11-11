/**
 * @file init.h
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

#ifndef PAL_GRAPHICS_SBT_H
#define PAL_GRAPHICS_SBT_H

#include "types.h"

/**
 * @defgroup filter_modes Filter Modes
 * @ingroup pal_graphics
 * @{
 */
#define PAL_FILTER_MODE_NEAREST 0 /**< Nearest-neighbor filtering. */
#define PAL_FILTER_MODE_LINEAR 1 /**< Linear filtering. */
#define PAL_FILTER_MODE_COUNT 2 /**< Number of filter modes. */
/** @} */

/**
 * @defgroup sampler_mipmap_modes Sampler Mipmap Modes
 * @ingroup pal_graphics
 * @{
 */
#define PAL_SAMPLER_MIPMAP_MODE_NEAREST 0 /**< Select the nearest mipmap level. */
#define PAL_SAMPLER_MIPMAP_MODE_LINEAR 1 /**< Interpolate linearly between mipmap levels. */
#define PAL_SAMPLER_MIPMAP_MODE_COUNT 2 /**< Number of sampler mipmap modes. */
/** @} */

/**
 * @defgroup sampler_address_modes Sampler Address Modes
 * @ingroup pal_graphics
 * @{
 */
#define PAL_SAMPLER_ADDRESS_MODE_REPEAT 0 /**< Repeat the texture. */
#define PAL_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT 1 /**< Repeat the texture, mirroring it on every repeat. */
#define PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE 2 /**< Clamp coordinates to the edge texels. */
#define PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER 3 /**< Use the border color outside the texture. */
#define PAL_SAMPLER_ADDRESS_MODE_COUNT 4 /**< Number of sampler address modes. */
/** @} */

/**
 * @defgroup sampler_border_colors Sampler Border Colors
 * @ingroup pal_graphics
 * @{
 */
#define PAL_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK 0 /**< Transparent black as floating-point values. */
#define PAL_BORDER_COLOR_INT_TRANSPARENT_BLACK 1 /**< Transparent black as integer values. */
#define PAL_BORDER_COLOR_FLOAT_OPAQUE_BLACK 2 /**< Opaque black as floating-point values. */
#define PAL_BORDER_COLOR_INT_OPAQUE_BLACK 3 /**< Opaque black as integer values. */
#define PAL_BORDER_COLOR_FLOAT_OPAQUE_WHITE 4 /**< Opaque white as floating-point values. */
#define PAL_BORDER_COLOR_INT_OPAQUE_WHITE 5 /**< Opaque white as integer values. */
#define PAL_BORDER_COLOR_COUNT 6 /**< Number of sampler border colors. */
/** @} */

/**
 * @struct PalSamplerCreateInfo
 * @brief Sampler creation parameters.
 *
 * This struct is used only during @ref palCreateSampler() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalSamplerCreateInfo {
    PalBool enableCompare;              /**< Whether to enable compare operations. */
    PalBool enableAnisotropy;           /**< Whether to enable anisotropic filtering. */
    float mipLodBias;                   /**< Mipmap level bias. */
    float minLod;                       /**< Minimum Mipmap level allowed. */
    float maxLod;                       /**< Maximum Mipmap level allowed. */
    float maxAnisotropy;                /**< Maximum anisotropy level. */
    PalFilterMode minFilterMode;        /**< Minification filter mode. */
    PalFilterMode magFilterMode;        /**< Magnification filter mode. */
    PalSamplerMipmapMode mipmapMode;    /**< Sampler mipmap mode. */
    PalSamplerAddressMode addressModeU; /**< U address mode. */
    PalSamplerAddressMode addressModeV; /**< V address mode. */
    PalSamplerAddressMode addressModeW; /**< W address mode. */
    PalCompareOp compareOp;             /**< Compare operation. */
    PalBorderColor borderColor;         /**< Border color. */
} PalSamplerCreateInfo;

/**
 * @brief Creates a sampler.
 *
 * The created sampler must be destroyed using @ref palDestroySampler().
 *
 * @param[in] device Device that creates the sampler.
 * @param[in] info Sampler creation parameters.
 * @param[out] sampler Output handle to receive the created sampler.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `device` must be externally synchronized.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palDestroySampler
 */
PAL_API PalResult PAL_CALL palCreateSampler(
    PalDevice* device,
    const PalSamplerCreateInfo* info,
    PalSampler** sampler);

/**
 * @brief Destroys a sampler.
 *
 * @param[in] sampler Sampler to destroy.
 *
 * @thread-safety The device used to create `sampler` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateSampler
 */
PAL_API void PAL_CALL palDestroySampler(PalSampler* sampler);

#endif // PAL_GRAPHICS_SBT_H