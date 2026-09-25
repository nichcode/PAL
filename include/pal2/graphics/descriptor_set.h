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

#ifndef PAL_GRAPHICS_DESCRIPTOR_SET_H
#define PAL_GRAPHICS_DESCRIPTOR_SET_H

#include "device.h"

/**
 * @struct PalDescriptorSet
 * @brief Opaque handle to a descriptor set.
 *
 * @since Added in version 2.0
 */
typedef struct PalDescriptorSet PalDescriptorSet;

/**
 * @struct PalDescriptorBufferInfo
 * @brief Contains information about a buffer descriptor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDescriptorBufferInfo::buffer
 * The buffer associated with the descriptor.
 * 
 * @var PalDescriptorBufferInfo::offset
 * The offset of ::buffer in bytes. This will be divided by ::stride 
 * if the buffer is marked as structured.
 * 
 * @var PalDescriptorBufferInfo::size
 * The size of ::buffer in bytes.
 * 
 * @var PalDescriptorBufferInfo::stride
 * This will be ignored if ::buffer is not marked as structured.
 */
typedef struct PalDescriptorBufferInfo
{
    PalBuffer* buffer;
    uint64_t offset;
    uint64_t size;  
    uint64_t stride;
} PalDescriptorBufferInfo;

/**
 * @struct PalDescriptorImageViewInfo
 * @brief Contains information about an image view descriptor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDescriptorBufferInfo::imageView
 * The image view associated with the descriptor.
 */
typedef struct PalDescriptorImageViewInfo
{
    PalImageView* imageView;
} PalDescriptorImageViewInfo;

/**
 * @struct PalDescriptorSamplerInfo
 * @brief Contains information about a sampler descriptor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDescriptorSamplerInfo::sampler
 * The sampler associated with the descriptor.
 */
typedef struct PalDescriptorSamplerInfo
{
    PalSampler* sampler;
} PalDescriptorSamplerInfo;

/**
 * @struct PalDescriptorTLASInfo
 * @brief Contains information about a TLAS descriptor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDescriptorTLASInfo::tlas
 * The top level acceleration structure associated with the descriptor.
 */
typedef struct PalDescriptorTLASInfo
{
    PalAccelerationStructure* tlas;
} PalDescriptorTLASInfo;

/**
 * @struct PalDescriptorSetWriteInfo
 * @brief Contains write information of a descriptor set.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDescriptorSetWriteInfo::descriptorSet
 * The descriptor set to update. Must not be `nullptr`.
 * 
 * @var PalDescriptorSetWriteInfo::bufferInfos
 * The buffer infos. This will be used if ::descriptorType is
 * `PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER`  or 
 * `PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER`
 * 
 * @var PalDescriptorSetWriteInfo::imageViewInfos
 * The image view infos. This will be used if ::descriptorType is
 * `PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE`  or 
 * `PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE`
 * 
 * @var PalDescriptorSetWriteInfo::samplerInfos
 * The sampler infos. This will be used if ::descriptorType is
 * `PAL_DESCRIPTOR_TYPE_SAMPLER`.
 * 
 * @var PalDescriptorSetWriteInfo::tlasInfos
 * The top level acceleration structure infos. This will be used if
 * ::descriptorType is `PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE`.
 * 
 * @var PalDescriptorSetWriteInfo::descriptorType
 * The type of the descriptor.
 * 
 * @var PalDescriptorSetWriteInfo::layoutBindingIndex
 * The index into the descriptor set layout (bindings) that was used
 * to create ::descriptorSet.
 * 
 * @var PalDescriptorSetWriteInfo::arrayElement
 * The first index in the descriptor set layout (bindings) that was used
 * to create ::descriptorSet.
 * 
 * @var PalDescriptorSetWriteInfo::descriptorCount
 * The number of descriptors to write.
 */
typedef struct PalDescriptorSetWriteInfo
{
    PalDescriptorSet* descriptorSet;
    PalDescriptorBufferInfo* bufferInfos;
    PalDescriptorImageViewInfo* imageViewInfos;
    PalDescriptorSamplerInfo* samplerInfos;
    PalDescriptorTLASInfo* tlasInfos;
    PalDescriptorType descriptorType;
    uint32_t layoutBindingIndex;
    uint32_t arrayElement;
    uint32_t descriptorCount;
} PalDescriptorSetWriteInfo;

/**
 * @brief Allocate a descriptor set from the provided descriptor pool.
 *
 * The descriptor set will be allocated uninitialized therefore update it before
 * use except the case where descriptor indexing is enabled.
 *
 * `pool` and `layout` must either be created with descriptor indexing enabled or not. Any other
 * pair will fail and return `PAL_RESULT_INVALID_OPERATION`.
 *
 * @param[in] device Device to allocate descriptor set on.
 * @param[in] pool Descriptor pool to allocate descriptor set from.
 * @param[in] layout Descriptor set layout that defines the bindings.
 * @param[out] outSet Pointer to a PalDescriptorSet to recieve the created descriptor set.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` and `pool` are externally synchronized.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palAllocateDescriptorSet(
    PalDevice* device,
    PalDescriptorPool* pool,
    PalDescriptorSetLayout* layout,
    PalDescriptorSet** outSet);

/**
 * @brief Update a descriptor set with descriptors (resources).
 *
 * If the write info has no valid resource handle, then `PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS`
 * must be supported and enabled when creating the device. Otherwise behavior is undefined.
 *
 * @param[in] device The Device. Must match the one used to allocate descriptor set.
 * @param[in] count Capacity of the PalDescriptorSetWriteInfo array.
 * @param[in] infos Array of PalDescriptorSetWriteInfo to write.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palUpdateDescriptorSet(
    PalDevice* device,
    uint32_t count,
    PalDescriptorSetWriteInfo* infos);

#endif // PAL_GRAPHICS_DESCRIPTOR_SET_H