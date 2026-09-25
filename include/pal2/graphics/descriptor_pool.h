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

#ifndef PAL_GRAPHICS_DESCRIPTOR_POOL_H
#define PAL_GRAPHICS_DESCRIPTOR_POOL_H

#include "device.h"

/**
 * @struct PalDescriptorPool
 * @brief Opaque handle to a descriptor pool.
 *
 * @since Added in version 2.0
 */
typedef struct PalDescriptorPool PalDescriptorPool;

/**
 * @struct PalDescriptorPoolBindingSize
 * @brief Contains information about descriptor pool binding size.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDescriptorPoolBindingSize::bindingCount
 * The number of bindings of ::descriptorType.
 * 
 * @var PalDescriptorPoolBindingSize::descriptorType
 * The type of the descriptor.
 */
typedef struct PalDescriptorPoolBindingSize
{
    uint32_t bindingCount;
    PalDescriptorType descriptorType;
} PalDescriptorPoolBindingSize;

/**
 * @struct PalDescriptorPoolCreateInfo
 * @brief Contains creation parameters of a descriptor pool.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDescriptorPoolCreateInfo::bindingSizes
 * The binding sizes.
 * 
 * @var PalDescriptorPoolCreateInfo::bindingSizeCount
 * The number of binding sizes.
 * 
 * @var PalDescriptorPoolCreateInfo::maxDescriptorSets
 * The maximum number of descriptor sets that can be allocated.
 * 
 * @var PalDescriptorPoolCreateInfo::flags
 * A bitmask of the descriptor indexing flags to create the 
 * descriptor pool with.
 * 
 * @var PalDescriptorPoolCreateInfo::reserved
 * Not used. Set to `0`.
 */
typedef struct PalDescriptorPoolCreateInfo
{
    PalDescriptorPoolBindingSize* bindingSizes;
    uint32_t bindingSizeCount;
    uint32_t maxDescriptorSets;
    PalDescriptorIndexingFlags flags; 
    uint32_t reserved;
} PalDescriptorPoolCreateInfo;

/**
 * @brief Create a descriptor pool to allocate descriptor sets.
 *
 * The created descriptor pool must be destroyed using `palDestroyDescriptorPool()`.
 *
 * @param[in] device Device that creates the descriptor pool.
 * @param[in] info Pointer to a PalDescriptorPoolCreateInfo struct that specifies parameters.
 * @param[out] outPool Pointer to a PalDescriptorPool to recieve the created descriptor pool.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palDestroyDescriptorPool
 */
PAL_API PalResult PAL_CALL palCreateDescriptorPool(
    PalDevice* device,
    const PalDescriptorPoolCreateInfo* info,
    PalDescriptorPool** outPool);

/**
 * @brief Destroy a descriptor pool.
 *
 * @param[in] pool Descriptor pool to destroy.
 *
 * Thread safety: Thread safe if the device used to create the descriptor pool is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateDescriptorPool
 */
PAL_API void PAL_CALL palDestroyDescriptorPool(PalDescriptorPool* pool);

/**
 * @brief Reset the provided descriptor pool. This resets all allocated descriptor sets.
 *
 * @param[in] pool Descriptor pool to reset.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `pool` is externally synchronized.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palResetDescriptorPool(PalDescriptorPool* pool);

#endif // PAL_GRAPHICS_DESCRIPTOR_POOL_H