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

#ifndef PAL_GRAPHICS_DESCRIPTOR_SET_LAYOUT_H
#define PAL_GRAPHICS_DESCRIPTOR_SET_LAYOUT_H

#include "device.h"

/**
 * @defgroup descriptor_types Descriptor Types
 * @brief Descriptor types
 * 
 * @{
 */
#define PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER 0
#define PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER 1
#define PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE 2
#define PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE 3
#define PAL_DESCRIPTOR_TYPE_SAMPLER 4
#define PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE 5
#define PAL_DESCRIPTOR_TYPE_COUNT 6
/** @} */

/**
 * @typedef PalDescriptorType
 * @brief Descriptor types.
 * 
 * All values of this type follow the format `PAL_DESCRIPTOR_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalDescriptorType;

/**
 * @struct PalDescriptorSetLayout
 * @brief Opaque handle to a descriptor set layout.
 *
 * @since Added in version 2.0
 */
typedef struct PalDescriptorSetLayout PalDescriptorSetLayout;

/**
 * @struct PalDescriptorSetLayoutBinding
 * @brief Contains information about a descriptor set layout binding.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDescriptorSetLayoutBinding::descriptorCount
 * The number of descriptors of ::descriptorType.
 * 
 * @var PalDescriptorSetLayoutBinding::descriptorType
 * The type of the descriptor. (eg. `PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE`).
 */
typedef struct PalDescriptorSetLayoutBinding
{
    uint32_t descriptorCount;
    PalDescriptorType descriptorType;
} PalDescriptorSetLayoutBinding;

/**
 * @struct PalDescriptorSetLayoutCreateInfo
 * @brief Contains creation parameters of a descriptor set layout.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDescriptorSetLayoutCreateInfo::bindings
 * The layout bindings.
 * 
 * @var PalDescriptorSetLayoutCreateInfo::flags
 * A bitmask of the descriptor indexing flags to create the 
 * descriptor set layout with.
 * 
 * @var PalDescriptorSetLayoutCreateInfo::bindingCount
 * The number of layout bindings.
 */
typedef struct PalDescriptorSetLayoutCreateInfo
{
    PalDescriptorSetLayoutBinding* bindings;
    PalDescriptorIndexingFlags flags;
    uint32_t bindingCount;
} PalDescriptorSetLayoutCreateInfo;

/**
 * @brief Create a descriptor set layout that defines the bindings used by descriptor sets.
 *
 * The created descriptor set layout must be destroyed using `palDestroyDescriptorSetLayout()`.
 *
 * This defines the layout, ordering and the number of descriptors a descriptor set uses.
 *
 * The layouts should reflect the exact layout of the shaders. Eg.
 * descriptorBindings[2] = { sampler, sampled image } is different from
 * descriptorBindings[2] = { sampled image, sampler }. The ordering must be correct.
 *
 * @param[in] device Device that creates the descriptor set layout.
 * @param[in] info Pointer to a PalDescriptorSetLayoutCreateInfo struct that specifies parameters.
 * @param[out] outLayout Pointer to a PalDescriptorSetLayout to recieve the created layout.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palDestroyDescriptorSetLayout
 */
PAL_API PalResult PAL_CALL palCreateDescriptorSetLayout(
    PalDevice* device,
    const PalDescriptorSetLayoutCreateInfo* info,
    PalDescriptorSetLayout** outLayout);

/**
 * @brief Destroy a descriptor set layout.
 *
 * @param[in] layout Descriptor set layout to destroy.
 *
 * Thread safety: Thread safe if the device used to create the descriptor set layout is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateDescriptorSetLayout
 */
PAL_API void PAL_CALL palDestroyDescriptorSetLayout(PalDescriptorSetLayout* layout);

#endif // PAL_GRAPHICS_DESCRIPTOR_SET_LAYOUT_H