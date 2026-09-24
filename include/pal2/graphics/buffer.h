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

#ifndef GRAPHICS_BUFFER_H
#define GRAPHICS_BUFFER_H

#include "device.h"

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
 * @struct PalBuffer
 * @brief Opaque handle to a buffer.
 *
 * @since Added in version 2.0
 */
typedef struct PalBuffer PalBuffer;

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

/**
 * @brief Compute size for an acceleration structure instance buffer.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * This does not allocate memory for the buffer. This function must is required for all
 * acceleration structure instance buffers.
 *
 * @param[in] device The device to use.
 * @param[in] instanceCount Number of instances the instance buffer will hold.
 * @param[out] outSize Pointer to a uint64_t to recieve the required size.
 *
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
 * @sa palWriteInstanceStaging
 */
PAL_API void PAL_CALL palComputeInstanceStagingSize(
    PalDevice* device,
    uint32_t instanceCount,
    uint64_t* outSize);

/**
 * @brief Compute requirements for an image staging buffer.
 *
 * This does not allocate memory for the buffer. This function is required for all
 * image copy staging buffers.
 *
 * `PalBufferImageCopyInfo::bufferRowLength` and `PalBufferImageCopyInfo::bufferImageHeight`
 * are hints. The driver might used it defaults if the requested is not supported. After this call,
 * set those values to the required ones from `requirements`.
 * If the driver supports the proivded, the values will be the same.
 *
 * @param[in] device The device to use.
 * @param[in] imageFormat Destination image format.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 * @param[out] requirements Pointer to a PalImageStagingRequirements to recieve the requirements
 *
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
 * @sa palWriteImageStaging
 */
PAL_API void PAL_CALL palComputeImageStagingRequirements(
    PalDevice* device,
    PalFormat imageFormat,
    const PalBufferImageCopyInfo* copyInfo,
    PalImageStagingRequirements* requirements);

/**
 * @brief Write data to an instance staging buffer.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] device The device to use.
 * @param[in] instanceCount Number of instances.
 * @param[in] instances Array of PalAccelerationStructureInstance struct to write.
 * @param[out] ptr Pointer to the CPU visible memory. Must be mapped.
 *
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
 * @sa palComputeInstanceStagingSize
 */
PAL_API void PAL_CALL palWriteInstanceStaging(
    PalDevice* device,
    uint32_t instanceCount,
    PalAccelerationStructureInstance* instances,
    void* ptr);

/**
 * @brief Write data to an image staging buffer.
 *
 * @param[in] device The device to use.
 * @param[in] imageFormat Destination image format.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 * @param[out] srcData Pointer to the CPU visible memory with the data.
 * @param[out] ptr Pointer to the CPU visible memory. Must be mapped.
 *
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
 * @sa palComputeImageStagingRequirements
 */
PAL_API void PAL_CALL palWriteImageStaging(
    PalDevice* device,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo,
    void* srcData,
    void* ptr);

#endif // GRAPHICS_BUFFER_H