/**
 * @file init.h
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

#ifndef PAL_GRAPHICS_INIT_H
#define PAL_GRAPHICS_INIT_H

#include "types.h"

/**
 * @defgroup memory_types Memory Types
 * @ingroup pal_graphics
 * @{
 */
#define PAL_MEMORY_TYPE_GPU_ONLY 0 /**< GPU-local memory that the CPU cannot access. */
#define PAL_MEMORY_TYPE_CPU_UPLOAD 1 /**< CPU-writable memory for uploading data to the GPU. */
#define PAL_MEMORY_TYPE_CPU_READBACK 2 /**< CPU-readable memory for reading data back from the GPU. */
#define PAL_MEMORY_TYPE_COUNT 3 /**< Number of memory types. */
/** @} */

/**
 * @struct PalMemoryRequirements
 * @brief Memory requirements.
 *
 * Filled in by @ref palGetImageMemoryRequirements() and @ref palGetBufferMemoryRequirements().
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalMemoryRequirements {
    uint64_t size;                 /**< Required size in bytes. */
    uint64_t alignment;            /**< Required alignment in bytes. */
    uint64_t memoryMask;           /**< Memory mask. */
    uint32_t supportedMemoryTypes; /**< Mask of supported memory types. */
    uint32_t reserved;             /**< Must be set to `0`. */
} PalMemoryRequirements;

/**
 * @brief Allocates GPU memory for the specified device.
 *
 * CPU adapters usually have no real `PAL_MEMORY_TYPE_GPU_ONLY` memory. On those adapters PAL
 * treats shared memory as VRAM and reports it as `PAL_MEMORY_TYPE_GPU_ONLY`, so the type is
 * always available.
 *
 * @param[in] device Device to allocate memory for.
 * @param[in] type Memory type. Must be supported by the adapter associated with the device.
 * @param[in] memoryMask Memory mask. Must match memory type.
 * @param[in] size Number of bytes to allocate. Must not be `0`.
 * @param[out] memory Output to receive the allocated memory.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `device` must be externally synchronized and `memory` must be per thread.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palFreeMemory
 */
PAL_API PalResult PAL_CALL palAllocateMemory(
    PalDevice* device,
    PalMemoryType type,
    uint64_t memoryMask,
    uint64_t size,
    PalMemory** memory);

/**
 * @brief Frees memory allocated by @ref palAllocateMemory().
 *
 * If `memory` is `nullptr`, this function will return silently.
 *
 * @param[in] memory Memory to free.
 *
 * @thread-safety The device used to allocate `memory` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palAllocateMemory
 */
PAL_API void PAL_CALL palFreeMemory(PalMemory* memory);

#endif // PAL_GRAPHICS_INIT_H