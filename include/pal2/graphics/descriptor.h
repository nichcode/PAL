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

#ifndef PAL_GRAPHICS_DESCRIPTOR_H
#define PAL_GRAPHICS_DESCRIPTOR_H

#include "buffer.h"
#include "image.h"
#include "acceleration_structure.h"

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
 * @struct PalDescriptorPool
 * @brief Opaque handle to a descriptor pool.
 *
 * @since Added in version 2.0
 */
typedef struct PalDescriptorPool PalDescriptorPool;

/**
 * @struct PalDescriptorSetLayout
 * @brief Opaque handle to a descriptor set layout.
 *
 * @since Added in version 2.0
 */
typedef struct PalDescriptorSetLayout PalDescriptorSetLayout;

/**
 * @struct PalDescriptorSet
 * @brief Opaque handle to a descriptor set.
 *
 * @since Added in version 2.0
 */
typedef struct PalDescriptorSet PalDescriptorSet;

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

#endif // PAL_GRAPHICS_DESCRIPTOR_H