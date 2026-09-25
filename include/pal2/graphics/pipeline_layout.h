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

#ifndef PAL_GRAPHICS_PIPELINE_LAYOUT_H
#define PAL_GRAPHICS_PIPELINE_LAYOUT_H

#include "descriptor_set_layout.h"

/**
 * @struct PalPipelineLayout
 * @brief Opaque handle to a pipeline layout.
 *
 * @since Added in version 2.0
 */
typedef struct PalPipelineLayout PalPipelineLayout;

/**
 * @struct PalPushConstantInfo
 * @brief Conatains information about a push constant.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalPushConstantInfo::offset
 * The offset in bytes of the push constant.
 * 
 * @var PalPushConstantInfo::size
 * The size in bytes of the push constant.
 */
typedef struct PalPushConstantInfo
{
    uint32_t offset;
    uint32_t size;
} PalPushConstantInfo;

/**
 * @struct PalPipelineLayoutCreateInfo
 * @brief Contains creation parameters of a pipeline layout.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalPipelineLayoutCreateInfo::descriptorSetLayouts
 * The descriptor set layouts.
 * 
 * @var PalPipelineLayoutCreateInfo::pushConstantInfo
 * The push constant info of the pipeline layout.
 * 
 * @var PalPipelineLayoutCreateInfo::descriptorSetLayoutCount
 * The number of descriptor set layouts.
 * 
 * @var PalPipelineLayoutCreateInfo::usePushConstant
 * If `PAL_TRUE`, ::pushConstantInfo will be read and used.
 */
typedef struct PalPipelineLayoutCreateInfo
{
    PalDescriptorSetLayout** descriptorSetLayouts;
    PalPushConstantInfo pushConstantInfo;
    uint32_t descriptorSetLayoutCount;
    PalBool usePushConstant;
} PalPipelineLayoutCreateInfo;

/**
 * @brief Create a pipeline layout. This defines the descriptor set interfaces and push
 * constant info.
 *
 * The created pipeline layout must be destroyed using `palDestroyPipelineLayout()`.
 *
 * @param[in] device Device that creates the pipeline layout.
 * @param[in] info Pointer to a PalPipelineLayoutCreateInfo struct that specifies parameters.
 * @param[out] outLayout Pointer to a PalPipelineLayout to recieve the created pipeline layout.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palDestroyPipelineLayout
 */
PAL_API PalResult PAL_CALL palCreatePipelineLayout(
    PalDevice* device,
    const PalPipelineLayoutCreateInfo* info,
    PalPipelineLayout** outLayout);

/**
 * @brief Destroy a pipeline layout.
 *
 * @param[in] layout Pipeline layout to destroy.
 *
 * Thread safety: Thread safe if the device used to create the pipeline layout is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreatePipelineLayout
 */
PAL_API void PAL_CALL palDestroyPipelineLayout(PalPipelineLayout* layout);

#endif // PAL_GRAPHICS_PIPELINE_LAYOUT_H