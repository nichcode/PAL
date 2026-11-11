/**
 * @file next.h
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

#ifndef PAL_GRAPHICS_NEXT_H
#define PAL_GRAPHICS_NEXT_H

#include "types.h"

/**
 * @defgroup buffer_usages Buffer Usages
 * @ingroup pal_graphics
 * @{
 */
#define PAL_BUFFER_USAGE_VERTEX (1U << 0) /**< Vertex buffer. */
#define PAL_BUFFER_USAGE_INDEX (1U << 1) /**< Index buffer. */
#define PAL_BUFFER_USAGE_UNIFORM (1U << 2) /**< Uniform buffer. */
#define PAL_BUFFER_USAGE_STORAGE (1U << 3) /**< Storage buffer. */
#define PAL_BUFFER_USAGE_TRANSFER_SRC (1U << 4) /**< Source of a transfer (copy) operation. */
#define PAL_BUFFER_USAGE_TRANSFER_DST (1U << 5) /**< Destination of a transfer (copy) operation. */
#define PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE (1U << 6) /**< Acceleration structure storage. */
#define PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_SCRATCH (1U << 7) /**< Build scratch memory. */
#define PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_READ_ONLY_INPUT (1U << 8) /**< Read-only build input. */
#define PAL_BUFFER_USAGE_DEVICE_ADDRESS (1U << 9) /**< Buffer device address queries. */
#define PAL_BUFFER_USAGE_INDIRECT (1U << 10) /**< Indirect draw or dispatch arguments. */
/** @} */

/**
 * @defgroup buffer_memory_usages Buffer Memory Usages
 * @ingroup pal_graphics
 * @{
 */
#define PAL_BUFFER_MEMORY_USAGE_MANUAL 0 /**< PAL should not allocate memory for the buffer. */
#define PAL_BUFFER_MEMORY_USAGE_AUTO_GPU_ONLY 1 /**< PAL allocates GPU memory for the buffer. */
#define PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_UPLOAD 2 /**< PAL allocates CPU upload memory for the buffer. */
#define PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_READBACK 3 /**< PAL allocates CPU readback memory for the buffer. */
#define PAL_BUFFER_MEMORY_USAGE_COUNT 4 /**< Number of buffer memory usages. */
/** @} */

/**
 * @struct PalImageStagingRequirements
 * @brief Image staging buffer requirements.
 *
 * Filled in by @ref palComputeImageStagingRequirements().
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalImageStagingRequirements {
    uint64_t bufferSize;        /**< Required buffer size in bytes. */
    uint32_t bufferRowLength;   /**< Required buffer row length in texels. */
    uint32_t bufferImageHeight; /**< Required buffer image height in texels. */
} PalImageStagingRequirements;

/**
 * @struct PalBufferCopyInfo
 * @brief Buffer copy information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalBufferCopyInfo {
    uint64_t size;      /**< Number of bytes to copy. */
    uint64_t dstOffset; /**< Offset in bytes in destination buffer. */
    uint64_t srcOffset; /**< Offset in bytes in source buffer. */
} PalBufferCopyInfo;

/**
 * @struct PalBufferCreateInfo
 * @brief Buffer creation parameters.
 *
 * `memoryUsage` may be one of the following:
 *
 * - PAL_BUFFER_MEMORY_USAGE_MANUAL - PAL does not allocate
 *   memory for the buffer. Call @ref palGetBufferMemoryRequirements() to get the
 *   memory requirement of the buffer and allocate memory with @ref palAllocateMemory()
 *   after the buffer has been created. Users are required to free the allocated
 *   memory with @ref palFreeMemory() when no longer needed.
 *
 * - PAL_BUFFER_MEMORY_USAGE_AUTO_GPU_ONLY - PAL allocates GPU
 *   only memory for the buffer after creation.
 *
 * - PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_UPLOAD - PAL allocates CPU
 *   upload memory for the buffer after creation.
 *
 * - PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_READBACK - PAL allocates CPU
 *   readback memory for the buffer after creation.
 *
 * This struct is used only during @ref palCreateBuffer() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalBufferCreateInfo {
    uint64_t size;                    /**< Size in bytes. */
    PalBufferUsages usages;           /**< Bitmask of supported buffer usages. */
    PalBufferMemoryUsage memoryUsage; /**< Buffer memory usage. */
} PalBufferCreateInfo;

/**
 * @brief Creates a buffer.
 *
 * The created buffer must be destroyed using @ref palDestroyBuffer().
 *
 * `PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS` must be supported and enabled by the device.
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
 * @param[out] outBuffer Pointer to a PalBuffer to receive the created buffer.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `device` must be externally synchronized.
 *
 * @result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT,
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY,
 *               @ref PAL_RESULT_CODE_PLATFORM_FAILURE.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palDestroyBuffer
 */
PAL_API PalResult PAL_CALL palCreateBuffer(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer);

/**
 * @brief Destroys a buffer.
 *
 * @param[in] buffer Buffer to destroy.
 *
 * @thread-safety The device used to create `buffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateBuffer
 */
PAL_API void PAL_CALL palDestroyBuffer(PalBuffer* buffer);

/**
 * @brief Gets memory requirements for the provided buffer.
 *
 * @param[in] buffer Buffer to query memory requirements on.
 * @param[out] requirements Pointer to a PalMemoryRequirements to fill.
 *
 * @thread-safety `requirements` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palGetBufferMemoryRequirements(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements);

/**
 * @brief Computes size for an acceleration structure instance buffer.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * This does not allocate memory for the buffer. This function is required for all
 * acceleration structure instance buffers.
 *
 * @param[in] device The device to use.
 * @param[in] instanceCount Number of instances the instance buffer will hold.
 * @param[out] outSize Pointer to a uint64_t to receive the required size.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palWriteInstanceStaging
 */
PAL_API void PAL_CALL palComputeInstanceStagingSize(
    PalDevice* device,
    uint32_t instanceCount,
    uint64_t* outSize);

/**
 * @brief Computes requirements for an image staging buffer.
 *
 * This does not allocate memory for the buffer. This function is required for all
 * image copy staging buffers.
 *
 * `PalBufferImageCopyInfo::bufferRowLength` and `PalBufferImageCopyInfo::bufferImageHeight`
 * are hints. The driver might used it defaults if the requested is not supported. After this call,
 * set those values to the required ones from `requirements`.
 * If the driver supports the provided, the values will be the same.
 *
 * @param[in] device The device to use.
 * @param[in] imageFormat Destination image format.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 * @param[out] requirements Pointer to a PalImageStagingRequirements to receive the requirements.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palWriteImageStaging
 */
PAL_API void PAL_CALL palComputeImageStagingRequirements(
    PalDevice* device,
    PalFormat imageFormat,
    const PalBufferImageCopyInfo* copyInfo,
    PalImageStagingRequirements* requirements);

/**
 * @brief Writes data to an instance staging buffer.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] device The device to use.
 * @param[in] instanceCount Number of instances.
 * @param[in] instances Array of PalAccelerationStructureInstance struct to write.
 * @param[out] ptr Pointer to the CPU visible memory. Must be mapped.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palComputeInstanceStagingSize
 */
PAL_API void PAL_CALL palWriteInstanceStaging(
    PalDevice* device,
    uint32_t instanceCount,
    PalAccelerationStructureInstance* instances,
    void* ptr);

/**
 * @brief Writes data to an image staging buffer.
 *
 * @param[in] device The device to use.
 * @param[in] imageFormat Destination image format.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 * @param[out] srcData Pointer to the CPU visible memory with the data.
 * @param[out] ptr Pointer to the CPU visible memory. Must be mapped.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palComputeImageStagingRequirements
 */
PAL_API void PAL_CALL palWriteImageStaging(
    PalDevice* device,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo,
    void* srcData,
    void* ptr);

/**
 * @brief Binds an allocated memory to a buffer.
 *
 * The memory size and alignment should match the requirements of the buffer.
 * Get the requirements with @ref palGetBufferMemoryRequirements().
 *
 * @param[in] buffer Buffer to bind memory to.
 * @param[in] memory Memory to bind.
 * @param[in] offset Starting point within the memory.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `buffer` must be externally synchronized.
 *
 * @result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT,
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY,
 *               @ref PAL_RESULT_CODE_PLATFORM_FAILURE.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
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
 * @param[out] outPtr Pointer to a void* to receive the mapped memory.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `buffer` must be externally synchronized.
 *
 * @result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT,
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY,
 *               @ref PAL_RESULT_CODE_PLATFORM_FAILURE.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palUnmapBuffer
 */
PAL_API PalResult PAL_CALL palMapBuffer(
    PalBuffer* buffer,
    uint64_t offset,
    uint64_t size,
    void** outPtr);

/**
 * @brief Unmaps buffer from CPU visible address space.
 *
 * The buffer must be mapped before this call. After this call, the CPU pointer must not
 * be used anymore.
 *
 * @param[in] buffer Pointer to buffer to unmap.
 *
 * @thread-safety `buffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palMapBuffer
 */
PAL_API void PAL_CALL palUnmapBuffer(PalBuffer* buffer);

/**
 * @brief Gets the device address of the provided buffer.
 *
 * Buffer must have `PAL_BUFFER_USAGE_DEVICE_ADDRESS` usage flag.
 *
 * @param[in] buffer Buffer to get its device address.
 * @return Buffer device address on success or `0` on failure.
 *
 * @thread-safety `buffer` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalDeviceAddress PAL_CALL palGetBufferDeviceAddress(PalBuffer* buffer);

#endif // PAL_GRAPHICS_NEXT_H