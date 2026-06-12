
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup memory Memory section
 * @ingroup pal_core
 * @{
 */

#ifndef _CORE_MEMORY_H
#define _CORE_MEMORY_H

#include "defines.h"

/**
 * @typedef PalAllocateFn
 * @brief Function pointer type used for memory allocations.
 *
 * @param[in] userData Optional pointer to user data passed from ::PalAllocator. Can be nullptr.
 * @param[in] size Number of bytes to allocate. Must not be 0.
 * @param[in] alignment Must be power of two. Set to 0 to use default (16).
 *
 * @return Pointer to the allocated memory on success or nullptr on failure.
 *
 * @since 1.0
 * @sa PalFreeFn
 */
typedef void*(PAL_CALL* PalAllocateFn)(
    void* userData,
    Uint64 size,
    Uint64 alignment);

/**
 * @typedef PalFreeFn
 * @brief Function pointer type used for memory deallocations.
 *
 * @param[in] userData Optional pointer to user data passed from ::PalAllocator. Can be nullptr.
 * @param[in] ptr Pointer to memory previously allocated by PalAllocateFn. Must return safely if
 * pointer is nullptr.
 *
 * @since 1.0
 * @sa PalAllocateFn
 */
typedef void(PAL_CALL* PalFreeFn)(
    void* userData,
    void* ptr);

/**
 * @struct PalAllocator
 * @brief Custom memory allocator.
 *
 * Provides user-defined memory allocation and free functions.
 *
 * @since 1.0
 */
typedef struct {
    PalAllocateFn allocate;
    PalFreeFn free;
    void* userData; /**< Optional user-provided data. Can be nullptr.*/
} PalAllocator;

/**
 * Allocate memory using the provided allocator.
 *
 * @param allocator The allocator to use. Set to nullptr to use default.
 * @param size Number of bytes to allocate.
 * @param alignment Alignment in bytes. Must be a power of two.
 *
 * @return Pointer to allocated memory on success, or nullptr on failure.
 *
 * Thread safety: Thread safe only if the provided allocator is thread safe. The default allocator
 * is thread safe.
 *
 * @since 1.0
 * @sa palFree
 */
PAL_API void* PAL_CALL palAllocate(
    const PalAllocator* allocator,
    Uint64 size,
    Uint64 alignment);

/**
 * Free memory allocated by palAllocate.
 *
 * @param allocator The allocator used to allocate the memory. Set to nullptr to
 * use default.
 * @param ptr Pointer to memory to free. If nullptr, the function returns
 * silently.
 *
 * Thread safety: Thread safe only if the provided allocator is thread
 * safe. The default allocator is thread safe.
 *
 * @since 1.0
 * @sa palAllocate
 */
PAL_API void PAL_CALL palFree(
    const PalAllocator* allocator,
    void* ptr);

#endif // _CORE_MEMORY_H

/** @} */