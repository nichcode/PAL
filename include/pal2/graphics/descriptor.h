/**
 * @file descriptor.h
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

#ifndef PAL_GRAPHICS_DESCRIPTOR_H
#define PAL_GRAPHICS_DESCRIPTOR_H

#include "pal2/core/base.h"
#include "pal2/core/result.h"
#include "device.h"
#include "image.h"
#include "buffer.h"

#include "acceleration_structure.h"
#include "sampler.h"

/**
 * @typedef PalDescriptorIndexingFlags
 * @brief Descriptor indexing flags.
 *
 * Identifies the descriptor indexing behaviors of descriptors.
 *
 * All values of this type follow the format `PAL_DESCRIPTOR_INDEXING_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalDescriptorIndexingFlags;

/**
 * @typedef PalDescriptorType
 * @brief Descriptor type.
 *
 * Identifies the kind of resource a descriptor refers to.
 *
 * All values of this type follow the format `PAL_DESCRIPTOR_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalDescriptorType;

/**
 * @struct PalDescriptorSetLayout
 * @brief Opaque handle to a descriptor set layout.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorSetLayout PalDescriptorSetLayout;

/**
 * @struct PalDescriptorPool
 * @brief Opaque handle to a descriptor pool.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorPool PalDescriptorPool;

/**
 * @struct PalDescriptorSet
 * @brief Opaque handle to a descriptor set.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorSet PalDescriptorSet;

/**
 * @defgroup descriptor_indexing_flags Descriptor Indexing Flags
 * @ingroup pal_graphics
 * @{
 */
#define PAL_DESCRIPTOR_INDEXING_FLAG_NONE 0 /**< No descriptor indexing flags. */
#define PAL_DESCRIPTOR_INDEXING_FLAG_UPDATE_AFTER_BIND (1U << 0) /**< Update after bind. */
#define PAL_DESCRIPTOR_INDEXING_FLAG_PARTIALLY_BOUND (1U << 1) /**< Unused descriptors can be left uninitialized. */
#define PAL_DESCRIPTOR_INDEXING_FLAG_NON_UNIFORM_INDEXING (1U << 2) /**< Non-uniform descriptor access. */
/** @} */

/**
 * @defgroup descriptor_types Descriptor Types
 * @ingroup pal_graphics
 * @{
 */
#define PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER 0 /**< Storage buffer. */
#define PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER 1 /**< Uniform buffer. */
#define PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE 2 /**< Sampled image. */
#define PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE 3 /**< Storage image. */
#define PAL_DESCRIPTOR_TYPE_SAMPLER 4 /**< Sampler. */
#define PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE 5 /**< Acceleration structure. */
#define PAL_DESCRIPTOR_TYPE_COUNT 6 /**< Number of descriptor types. */
/** @} */

/**
 * @struct PalDescriptorIndexingCapabilities
 * @brief Adapter descriptor indexing capabilities.
 *
 * Filled in by @ref palQueryDescriptorIndexingCapabilities().
 *
 * `flags` shows the sub-features or capabilities of the descriptor indexing feature.
 * Each bit represents a supported operation. See below for more information:
 * - PAL_DESCRIPTOR_INDEXING_FLAG_UPDATE_AFTER_BIND - Descriptors can be updated after
 *   the descriptor set has been bound.
 * - PAL_DESCRIPTOR_INDEXING_FLAG_PARTIALLY_BOUND - Unused descriptors can be
 *   left uninitialized if shaders never access them.
 * - PAL_DESCRIPTOR_INDEXING_FLAG_NON_UNIFORM_INDEXING - Different threads
 *   can access different descriptors.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorIndexingCapabilities {
    PalDescriptorIndexingFlags flags;   /**< Descriptor indexing flags. */
    uint32_t maxPerStageSampledImages;  /**< Maximum sampled images per shader stage. */
    uint32_t maxPerSetSampledImages;    /**< Maximum sampled images per descriptor set. */
    uint32_t maxPerStageStorageImages;  /**< Maximum storage images per shader stage. */
    uint32_t maxPerSetStorageImages;    /**< Maximum storage images per descriptor set. */
    uint32_t maxPerStageSamplers;       /**< Maximum samplers per shader stage. */
    uint32_t maxPerSetSamplers;         /**< Maximum samplers per descriptor set. */
    uint32_t maxPerStageStorageBuffers; /**< Maximum storage buffers per shader stage. */
    uint32_t maxPerSetStorageBuffers;   /**< Maximum storage buffers per descriptor set. */
    uint32_t maxPerStageUniformBuffers; /**< Maximum uniform buffers per shader stage. */
    uint32_t maxPerSetUniformBuffers;   /**< Maximum uniform buffers per descriptor set. */
    uint32_t maxPerStageAccelerationStructure; /**< Maximum acceleration structures per shader stage. */
    uint32_t maxPerSetAccelerationStructure; /**< Maximum acceleration structures per descriptor set. */
} PalDescriptorIndexingCapabilities;

/**
 * @struct PalDescriptorSetLayoutBinding
 * @brief Descriptor set layout binding.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorSetLayoutBinding {
    uint32_t descriptorCount;         /**< Number of descriptors. */
    PalDescriptorType descriptorType; /**< Descriptor type. */
} PalDescriptorSetLayoutBinding;

/**
 * @struct PalDescriptorPoolBindingSize
 * @brief Descriptor pool binding size.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorPoolBindingSize {
    uint32_t bindingCount;            /**< Number of bindings. */
    PalDescriptorType descriptorType; /**< Descriptor type. */
} PalDescriptorPoolBindingSize;

/**
 * @struct PalDescriptorBufferInfo
 * @brief Buffer descriptor information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorBufferInfo {
    PalBuffer* buffer; /**< Buffer associated with the descriptor. */
    uint64_t offset;   /**< Offset in bytes (divided by `stride` if the buffer is structured). */
    uint64_t size;     /**< Size in bytes. */
    uint64_t stride;   /**< Stride of structured buffers in bytes. */
} PalDescriptorBufferInfo;

/**
 * @struct PalDescriptorImageViewInfo
 * @brief Image view descriptor information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorImageViewInfo {
    PalImageView* imageView; /**< Image view associated with the descriptor. */
} PalDescriptorImageViewInfo;

/**
 * @struct PalDescriptorSamplerInfo
 * @brief Sampler descriptor information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorSamplerInfo {
    PalSampler* sampler; /**< Sampler associated with the descriptor. */
} PalDescriptorSamplerInfo;

/**
 * @struct PalDescriptorTLASInfo
 * @brief TLAS descriptor information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorTLASInfo {
    PalAccelerationStructure* tlas; /**< TLAS associated with the descriptor. */
} PalDescriptorTLASInfo;

/**
 * @struct PalDescriptorSetWriteInfo
 * @brief Descriptor set write information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorSetWriteInfo {
    PalDescriptorSet* descriptorSet;            /**< Descriptor set to write into. */
    PalDescriptorBufferInfo* bufferInfos;       /**< Used with buffer descriptors. */
    PalDescriptorImageViewInfo* imageViewInfos; /**< Used with image descriptors. */
    PalDescriptorSamplerInfo* samplerInfos;     /**< Used with sampler descriptors. */
    PalDescriptorTLASInfo* tlasInfos;           /**< Used with TLAS descriptors. */
    PalDescriptorType descriptorType;           /**< Descriptor type. */
    uint32_t layoutBindingIndex; /**< Index into the descriptor set layout bindings array. */
    uint32_t arrayElement;       /**< First array element within the binding to write. */
    uint32_t descriptorCount;    /**< Number of descriptors to write. */
} PalDescriptorSetWriteInfo;

/**
 * @struct PalDescriptorSetLayoutCreateInfo
 * @brief Descriptor set layout creation parameters.
 *
 * This struct is used only during @ref palCreateDescriptorSetLayout() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorSetLayoutCreateInfo {
    PalDescriptorSetLayoutBinding* bindings; /**< Bindings. */
    PalDescriptorIndexingFlags flags;        /**< Descriptor indexing flags. */
    uint32_t bindingCount;                   /**< Number of bindings. */
} PalDescriptorSetLayoutCreateInfo;

/**
 * @struct PalDescriptorPoolCreateInfo
 * @brief Descriptor pool creation parameters.
 *
 * This struct is used only during @ref palCreateDescriptorPool() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorPoolCreateInfo {
    PalDescriptorPoolBindingSize* bindingSizes; /**< Binding sizes. */
    uint32_t bindingSizeCount;                  /**< Number of bindings sizes. */
    uint32_t maxDescriptorSets; /**< Maximum number of descriptor sets that can be allocated. */
    PalDescriptorIndexingFlags flags; /**< Descriptor indexing flags. */
    uint32_t reserved;                /**< Must be set to `0`. */
} PalDescriptorPoolCreateInfo;

/**
 * @brief Creates a descriptor set layout that defines the bindings used by descriptor sets.
 *
 * The created descriptor set layout must be destroyed using @ref palDestroyDescriptorSetLayout().
 *
 * This defines the layout, ordering and the number of descriptors a descriptor set uses.
 *
 * The layouts should reflect the exact layout of the shaders. Eg.
 * descriptorBindings[2] = { sampler, sampled image } is different from
 * descriptorBindings[2] = { sampled image, sampler }. The ordering must be correct.
 *
 * @param[in] device Device that creates the descriptor set layout.
 * @param[in] info Pointer to a PalDescriptorSetLayoutCreateInfo struct that specifies parameters.
 * @param[out] outLayout Pointer to a PalDescriptorSetLayout to receive the created layout.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `device` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palDestroyDescriptorSetLayout
 */
PAL_API PalResult PAL_CALL palCreateDescriptorSetLayout(
    PalDevice* device,
    const PalDescriptorSetLayoutCreateInfo* info,
    PalDescriptorSetLayout** outLayout);

/**
 * @brief Destroys a descriptor set layout.
 *
 * @param[in] layout Descriptor set layout to destroy.
 *
 * @thread-safety The device used to create `layout` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateDescriptorSetLayout
 */
PAL_API void PAL_CALL palDestroyDescriptorSetLayout(PalDescriptorSetLayout* layout);

/**
 * @brief Creates a descriptor pool to allocate descriptor sets.
 *
 * The created descriptor pool must be destroyed using @ref palDestroyDescriptorPool().
 *
 * @param[in] device Device that creates the descriptor pool.
 * @param[in] info Pointer to a PalDescriptorPoolCreateInfo struct that specifies parameters.
 * @param[out] outPool Pointer to a PalDescriptorPool to receive the created descriptor pool.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `device` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palDestroyDescriptorPool
 */
PAL_API PalResult PAL_CALL palCreateDescriptorPool(
    PalDevice* device,
    const PalDescriptorPoolCreateInfo* info,
    PalDescriptorPool** outPool);

/**
 * @brief Destroys a descriptor pool.
 *
 * @param[in] pool Descriptor pool to destroy.
 *
 * @thread-safety The device used to create `pool` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateDescriptorPool
 */
PAL_API void PAL_CALL palDestroyDescriptorPool(PalDescriptorPool* pool);

/**
 * @brief Resets the provided descriptor pool. This resets all allocated descriptor sets.
 *
 * @param[in] pool Descriptor pool to reset.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `pool` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palResetDescriptorPool(PalDescriptorPool* pool);

/**
 * @brief Allocates a descriptor set from the provided descriptor pool.
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
 * @param[out] outSet Pointer to a PalDescriptorSet to receive the created descriptor set.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `device` and `pool` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palAllocateDescriptorSet(
    PalDevice* device,
    PalDescriptorPool* pool,
    PalDescriptorSetLayout* layout,
    PalDescriptorSet** outSet);

/**
 * @brief Updates a descriptor set with descriptors (resources).
 *
 * If the write info has no valid resource handle, then `PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS`
 * must be supported and enabled when creating the device. Otherwise behavior is undefined.
 *
 * @param[in] device The Device. Must match the one used to allocate descriptor set.
 * @param[in] count Capacity of the PalDescriptorSetWriteInfo array.
 * @param[in] infos Array of PalDescriptorSetWriteInfo to write.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `device` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palUpdateDescriptorSet(
    PalDevice* device,
    uint32_t count,
    PalDescriptorSetWriteInfo* infos);

/**
 * @brief Gets bindless/descriptor indexing feature capabilities of a device.
 *
 * @ref PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING feature must be supported and enabled when
 * creating the device or this function results in undefined behavior.
 *
 * @param[in] device Device to get its bindless/descriptor indexing capabilities.
 * @param[out] caps Output struct to receive the bindless/descriptor indexing capabilities.
 *
 * @thread-safety `caps` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palQueryDescriptorIndexingCapabilities(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps);

#endif // PAL_GRAPHICS_DESCRIPTOR_H