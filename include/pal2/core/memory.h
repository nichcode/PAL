
/**
 * @file memory.h
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

#ifndef PAL_CORE_MEMORY_H
#define PAL_CORE_MEMORY_H

#include "types.h"

/**
 * @brief Memory allocation function.
 *
 * The callback must allocate at least `size` bytes with the requested `alignment`
 * and return a pointer to the allocated memory. If allocation fails, the
 * callback must return `nullptr`. If the requested alignment is `0`,
 * the callback must use an implementation-defined default.
 *
 * The callback may initialize the allocated memory. The behavior of a zero-size
 * allocation is implementation-defined. The requested alignment must be a
 * power of two.
 *
 * The function signature should look like this:
 * @code
 * void* PAL_CALL alloc(void* userData, uint64_t size, uint64_t alignment);
 * @endcode
 *
 * @param[in] userData User-defined data passed to the callback or `nullptr`.
 * @param[in] size Number of bytes to allocate.
 * @param[in] alignment Alignment of the allocated memory.
 * @return Pointer to the allocated memory on success or `nullptr` on failure.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
 * @sa PalFreeFn
 */
typedef void*(PAL_CALL* PalAllocateFn)(
    void* userData,
    uint64_t size,
    uint64_t alignment);

/**
 * @brief Memory deallocation function.
 *
 * The callback must deallocate memory previously allocated by the corresponding
 * memory allocation function.
 *
 * The function signature should look like this:
 * @code
 * void PAL_CALL free(void* userData, void* ptr);
 * @endcode
 *
 * @param[in] userData User-defined data passed to the callback or `nullptr`.
 * @param[in] ptr Memory to free.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
 * @sa PalAllocateFn
 */
typedef void(PAL_CALL* PalFreeFn)(
    void* userData,
    void* ptr);

/**
 * @struct PalAllocator
 * @brief Memory allocator.
 *
 * The allocator will be used by PAL to allocate its internal memory.
 * The allocator must be thread safe if it is used by PAL functions that
 * are called concurrently from multiple threads.
 *
 * The allocator will not be copied, so it must remain valid for as long
 * as PAL may use it. The default allocator is thread safe and owned by PAL.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 */
typedef struct PalAllocator {
    PalAllocateFn allocate; /**< memory allocation function */
    PalFreeFn free; /**< memory deallocation function */
    void* userData; /**< user-defined data passed to callbacks or `nullptr` */
} PalAllocator;

/**
 * @brief Allocates memory.
 *
 * This function allocates memory with a custom or default allocator.
 * `allocator` is not copied, so it must remain valid for as long as memory allocated with it
 * has not been freed. The allocated memory must be freed using the same allocator.
 * PAL does not validate this requirement; using a different allocator results
 * in undefined behavior.
 *
 * This function allocates at least `size` bytes with the requested `alignment`.
 * The requested alignment must be a power of two. If the requested alignment is `0`,
 * a default will be used. The behavior of a zero-size allocation is
 * implementation-defined.
 *
 * @param[in] allocator Allocator to use, or `nullptr` for the default.
 * @param[in] size Number of bytes to allocate.
 * @param[in] alignment Alignment of the allocated memory in bytes.
 * @return Pointer to the allocated memory on success, or `nullptr` if allocation fails.
 *
 * @Thread-safety `allocator` implementation must be thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
 * @sa palFree
 */
PAL_API void* PAL_CALL palAllocate(
    const PalAllocator* allocator,
    uint64_t size,
    uint64_t alignment);

/**
 * @brief Deallocates memory.
 *
 * This function deallocates memory with a custom or default allocator.
 * The memory must be freed using the same allocator used to allocate it.
 * PAL does not validate this requirement; using a different allocator results
 * in undefined behavior.
 *
 * `allocator` is not copied. The allocator and any state referenced by it must
 * remain valid for the duration of this function.
 *
 * This function does not set `ptr` to `nullptr` after the memory is deallocated.
 * Passing memory that has already been freed results in undefined behavior. If
 * `ptr` is `nullptr`, this function returns silently.
 *
 * @param[in] allocator Allocator to use, or `nullptr` for the default.
 * @param[in] ptr Memory to free, or `nullptr` to do nothing.
 *
 * @Thread-safety `allocator` implementation must be thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
 * @sa palAllocate
 */
PAL_API void PAL_CALL palFree(
    const PalAllocator* allocator,
    void* ptr);

#endif // PAL_CORE_MEMORY_H