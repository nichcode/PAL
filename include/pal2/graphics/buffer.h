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

#ifndef PAL_GRAPHICS_BUFFER_H
#define PAL_GRAPHICS_BUFFER_H

#include "device.h"

/**
 * @defgroup buffer_usages Buffer Usages
 * @brief Buffer usages
 * 
 * @{
 */
#define PAL_BUFFER_USAGE_VERTEX (1U << 0)
#define PAL_BUFFER_USAGE_INDEX (1U << 1)
#define PAL_BUFFER_USAGE_UNIFORM (1U << 2)
#define PAL_BUFFER_USAGE_STORAGE (1U << 3)
#define PAL_BUFFER_USAGE_TRANSFER_SRC (1U << 4)
#define PAL_BUFFER_USAGE_TRANSFER_DST (1U << 5)
#define PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE (1U << 6)
#define PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_SCRATCH (1U << 7)
#define PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_READ_ONLY_INPUT (1U << 8)
#define PAL_BUFFER_USAGE_DEVICE_ADDRESS (1U << 9)
#define PAL_BUFFER_USAGE_INDIRECT (1U << 10)
/** @} */

/**
 * @defgroup buffer_memory_usages Buffer Memory Usages
 * @brief Buffer memory usages
 * 
 * @{
 */
#define PAL_BUFFER_MEMORY_USAGE_MANUAL 0
#define PAL_BUFFER_MEMORY_USAGE_AUTO_GPU_ONLY 1
#define PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_UPLOAD 2
#define PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_READBACK 3
#define PAL_BUFFER_MEMORY_USAGE_COUNT 4
/** @} */

/**
 * @typedef PalBufferMemoryUsage
 * @brief Buffer memory usages.
 * 
 * All values of this type follow the format `PAL_BUFFER_MEMORY_USAGE_*` 
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalBufferMemoryUsage;

/**
 * @typedef PalBufferUsages
 * @brief Buffer usages. 
 * 
 * Multiple buffer usages can be OR'ed together using bitwise
 * OR operator (`|`).
 * 
 * All values of this type follow the format `PAL_BUFFER_USAGE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalBufferUsages;

/**
 * @typedef PalDeviceAddress
 * @brief The device address of a buffer.
 *
 * @since Added in version 2.0
 */
typedef uint64_t PalDeviceAddress;

/**
 * @struct PalBuffer
 * @brief Opaque handle to a buffer.
 *
 * @since Added in version 2.0
 */
typedef struct PalBuffer PalBuffer;

/**
 * @struct PalBufferCopyInfo
 * @brief Contains information for buffer to buffer copies.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalBufferCopyInfo::size
 * The size in bytes to copy from the source buffer.
 * 
 * @var PalBufferCopyInfo::dstOffset
 * The offset in bytes of the destination buffer.
 * 
 * @var PalBufferCopyInfo::srcOffset
 * The offset in bytes of the source buffer.
 */
typedef struct PalBufferCopyInfo
{
    uint64_t size;
    uint64_t dstOffset;
    uint64_t srcOffset;
} PalBufferCopyInfo;

/**
 * @struct PalDrawIndirectData
 * @brief Contains indirect data of a draw call.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDrawIndirectData::vertexCount
 * The number of vertices.
 * 
 * @var PalDrawIndirectData::instanceCount
 * The number of instances.
 * 
 * @var PalDrawIndirectData::firstVertex
 * The first vertex.
 * 
 * @var PalDrawIndirectData::firstInstance
 * The first instance.
 */
typedef struct PalDrawIndirectData
{
    uint32_t vertexCount;
    uint32_t instanceCount;
    uint32_t firstVertex;
    uint32_t firstInstance;
} PalDrawIndirectData;

/**
 * @struct PalDrawIndexedIndirectData
 * @brief Contains indirect data of a draw indexed call.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDrawIndexedIndirectData::indexCount
 * The number of indices.
 * 
 * @var PalDrawIndexedIndirectData::instanceCount
 * The number of instances.
 * 
 * @var PalDrawIndexedIndirectData::firstIndex
 * The first index.
 * 
 * @var PalDrawIndexedIndirectData::vertexOffset
 * The vertex offset.
 * 
 * @var PalDrawIndexedIndirectData::firstInstance
 * The first instance.
 */
typedef struct PalDrawIndexedIndirectData
{
    uint32_t indexCount; 
    uint32_t instanceCount;
    uint32_t firstIndex; 
    int32_t vertexOffset;
    uint32_t firstInstance;
} PalDrawIndexedIndirectData;

/**
 * @struct PalDispatchIndirectData
 * @brief Contains indirect data of a dispatch call.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDispatchIndirectData::groupCountXOrWidth
 * The number of groups on the x dimension or the dispatch width.
 * 
 * @var PalDispatchIndirectData::groupCountXOrHeight
 * The number of groups on the y dimension or the dispatch height.
 * 
 * @var PalDispatchIndirectData::groupCountXOrDepth
 * The number of groups on the z dimension or the dispatch depth.
 */
typedef struct PalDispatchIndirectData
{
    uint32_t groupCountXOrWidth; 
    uint32_t groupCountXOrHeight;
    uint32_t groupCountXOrDepth; 
} PalDispatchIndirectData;

/**
 * @struct PalBufferCreateInfo
 * @brief Contains creation parameters of a buffer.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalBufferCreateInfo::size
 * The size in bytes of the buffer.
 * 
 * @var PalBufferCreateInfo::usages
 * The usages of the buffer.
 * 
 * @var PalBufferCreateInfo::memoryUsage
 * The memory usage model of the buffer (eg. `PAL_BUFFER_MEMORY_USAGE_MANUAL`).
 */
typedef struct PalBufferCreateInfo
{
    uint64_t size;
    PalBufferUsages usages;
    PalBufferMemoryUsage memoryUsage;
} PalBufferCreateInfo;

/**
 * @brief Create a buffer.
 *
 * The created buffer must be destroyed using `palDestroyBuffer()`.
 *
 * `PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS` must be supported and enabled by the devic.
 * Otherwise behavior is undefined.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if
 * `PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE` or `PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_SCRATCH`
 * or `PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_READ_ONLY_INPUT` will be used.
 *
 * `PAL_BUFFER_USAGE_INDIRECT` must be supported and enabled by the device if the buffer will be
 * used as an indirect buffer.
 *
 * @param[in] device Device that creates the buffer.
 * @param[in] info Pointer to a PalBufferCreateInfo struct that specifies parameters.
 * @param[out] outBuffer Pointer to a PalBuffer to recieve the created buffer.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palDestroyBuffer
 */
PAL_API PalResult PAL_CALL palCreateBuffer(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer);

/**
 * @brief Destroy a buffer.
 *
 * @param[in] buffer buffer to destroy.
 *
 * Thread safety: Thread safe if the device used to create the buffer is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateBuffer
 */
PAL_API void PAL_CALL palDestroyBuffer(PalBuffer* buffer);

/**
 * @brief Get memory requirements for the provided buffer.
 *
 * @param[in] buffer Buffer to query memory requirements on.
 * @param[out] requirements Pointer to a PalMemoryRequirements to fill.
 *
 * Thread safety: Thread safe if `requirements` is per thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetBufferMemoryRequirements(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements);

/**
 * @brief Bind an allocated memory to a buffer.
 *
 * The memory size and alignment should match the requirements of the buffer.
 * Get the requirements with palGetBufferMemoryRequirements().
 *
 * @param[in] buffer Buffer to bind memory to.
 * @param[in] memory Memory to bind.
 * @param[in] offset Starting point within the memory.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `requirements` is per thread.
 *
 * @since Added in version 2.0
 * @sa palGetBufferMemoryRequirements
 */
PAL_API PalResult PAL_CALL palBindBufferMemory(
    PalBuffer* buffer,
    PalMemory* memory,
    uint64_t offset);

/**
 * @brief Maps buffer to CPU visible address space.
 *
 * The buffer must have a valid memory bound to it before this call.
 *
 * Only `PAL_MEMORY_TYPE_CPU_UPLOAD` and `PAL_MEMORY_TYPE_CPU_READBACK` can be mapped to
 * CPU visible space. Mapping `PAL_MEMORY_TYPE_GPU_ONLY` will fail and return
 * `PAL_RESULT_MEMORY_MAP_FAILED`.
 *
 * @param[in] buffer Pointer to buffer to map. Memory must be bound.
 * @param[in] offset Starting point within the buffer.
 * @param[in] size Number of bytes to map from the offset. `offset + size` must not be
 * greater than buffer size.
 * @param[out] outPtr Pointer to a void* to recieved the mapped memory.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `buffer` is externally synchronized.
 * Mapping with different offsets into the same buffer is thread safe as long as `buffer`
 * is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palUnmapBuffer
 */
PAL_API PalResult PAL_CALL palMapBuffer(
    PalBuffer* buffer,
    uint64_t offset,
    uint64_t size,
    void** outPtr);

/**
 * @brief Unmap buffer from CPU visible address space.
 *
 * The buffer must be mapped before this call. After this call, the CPU pointer must not
 * be used anymore.
 *
 * @param[in] buffer Pointer to buffer to unmap.
 *
 * Thread safety: Thread safe if `buffer` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palMapBuffer
 */
PAL_API void PAL_CALL palUnmapBuffer(PalBuffer* buffer);

/**
 * @brief Get the device address of the provided buffer.
 *
 * Buffer must have `PAL_BUFFER_USAGE_DEVICE_ADDRESS` usage flag.
 *
 * @param[in] buffer Buffer to get its device address.
 *
 * @return Buffer device address on success or `0` on failure.
 *
 * Thread safety: Thread safe if `buffer` is per thread.
 *
 * @since Added in version 2.0
 */
PAL_API PalDeviceAddress PAL_CALL palGetBufferDeviceAddress(PalBuffer* buffer);

#endif // PAL_GRAPHICS_BUFFER_H