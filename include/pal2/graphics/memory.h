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

#ifndef PAL_GRAPHICS_MEMORY_H
#define PAL_GRAPHICS_MEMORY_H

#include "device.h"

/**
 * @defgroup memory_types Memory Types
 * @brief Memory types
 * 
 * @{
 */
#define PAL_MEMORY_TYPE_GPU_ONLY 0
#define PAL_MEMORY_TYPE_CPU_UPLOAD 1
#define PAL_MEMORY_TYPE_CPU_READBACK 2
#define PAL_MEMORY_TYPE_COUNT 3
/** @} */

/**
 * @typedef PalMemoryType
 * @brief Memory types.
 * 
 * All values of this type follow the format `PAL_MEMORY_TYPE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalMemoryType;

/**
 * @struct PalMemory
 * @brief Opaque handle to a GPU device memory.
 *
 * @since Added in version 2.0
 */
typedef struct PalMemory PalMemory;

/**
 * @struct PalMemoryRequirements
 * @brief Memory requirements for a resource (image, buffer etc).
 * 
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalMemoryRequirements::size
 * The required size in bytes of the resource.
 * 
 * @var PalMemoryRequirements::alignment
 * The required alignment in bytes of the resource.
 * This is used for sub allocations.
 * 
 * @var PalMemoryRequirements::memoryMask
 * The memory mask for the driver for the allocation. Must not be changed.
 * 
 * @var PalMemoryRequirements::supportedMemoryTypes
 * A bitmask of supported memory types. A specific memory type
 * should be check like this: @nl
 * palIsSupported(::supportedMemoryTypes, `PAL_MEMORY_TYPE_GPU_ONLY`).
 * 
 * @var PalMemoryRequirements::reserved
 * Not used. Set to `0`.
 */
typedef struct PalMemoryRequirements
{
    uint64_t size;
    uint64_t alignment;
    uint64_t memoryMask;
    uint32_t supportedMemoryTypes;
    uint32_t reserved;
} PalMemoryRequirements;

/**
 * @brief Allocates GPU memory for the specified device.
 *
 * On CPU adapters, there is usually no
 * `PAL_MEMORY_TYPE_GPU_ONLY` memory type available. So it uses shared memory as the vram and set
 * the shared memory to the `PAL_MEMORY_TYPE_GPU_ONLY` for correctness.
 *
 * @param[in] device Pointer to device to allocate memory on.
 * @param[in] type Memory type to allocate. Must be supported by the adapter associated with the
 * device.
 * @param[in] memoryMask Memory mask. Must match memory type.
 * @param[in] size Number of bytes to allocate. Must not be 0.
 * @param[out] outMemory Pointer to a PalMemory to recieved the allocated GPU memory.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized and
 * `outMemory` is per thread.
 *
 * @since Added in version 2.0
 * @sa palFreeMemory
 */
PAL_API PalResult PAL_CALL palAllocateMemory(
    PalDevice* device,
    PalMemoryType type,
    uint64_t memoryMask,
    uint64_t size,
    PalMemory** outMemory);

/**
 * @brief Free GPU memory allocated by palAllocateMemory.
 *
 * If `memory` is `nullptr`, this function will return silently.
 *
 * @param[in] memory Pointer to memory to free.
 *
 * Thread safety: Thread safe if `device` is externally synchronized and
 * `outMemory` is per thread.
 *
 * @since Added in version 2.0
 * @sa palAllocateMemory
 */
PAL_API void PAL_CALL palFreeMemory(PalMemory* memory);

#endif // PAL_GRAPHICS_MEMORY_H