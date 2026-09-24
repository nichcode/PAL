
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

#ifndef PAL_CORE_MEMORY_H
#define PAL_CORE_MEMORY_H

#include "defines.h"

/**
 * @brief Function pointer type used for memory allocations.
 * 
 * The function signature should look like this:
 * @code
 * void* PAL_CALL alloc(void* userData, uint64_t size, uint64_t alignment);
 * @endcode
 *
 * The callback must allocate atleast `size` with the requested
 * `alignment` or return `nullptr` if the allocation failed.
 * If the requested alignment is `0`, the callback must determines the
 * alignment to use for the allocation.
 *
 * If the requested size is `0`, the callback must determines the behavior. The
 * callback must decide whether to initialize the allocated memory or not.
 * PAL does not filter any requested size or alignment.
 *
 * PAL uses this callback to allocate all of its internal memory, this means
 * if a PAL function is thread safe and it will be called from multiple
 * threads, the callback implementation must also be thread safe.
 *
 * If a PAL function which uses the callback fails, it will deallocate the
 * memory it allocated using the same alloator's free callback.
 *
 * @param[in] userData User data passed from the allocator. Can be `nullptr`.
 * @param[in] size Number of bytes to allocate.
 * @param[in] alignment The alignment.
 *
 * @return The allocated memory on success or `nullptr` on failure.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * @sa PalFreeFn
 */
typedef void*(PAL_CALL* PalAllocateFn)(
    void* userData,
    uint64_t size,
    uint64_t alignment);

/**
 * @brief Function pointer type used for memory deallocations.
 * 
 * The function signature should look like this:
 * @code
 * void PAL_CALL free(void* userData, void* ptr);
 * @endcode
 *
 * The memory must have been allocated by the corresponding allocation
 * callback and must not have been deallocated. The callback will not
 * be called if the `ptr` is `nullptr`.
 *
 * @param[in] userData User data passed from the allocator. Can be `nullptr`.
 * @param[in] ptr The memory to free.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * @sa PalAllocateFn
 */
typedef void(PAL_CALL* PalFreeFn)(
    void* userData,
    void* ptr);

/**
 * @brief Contains information about a memory allocator.
 *
 * This struct provides a way to use a custom allocator with PAL.
 * Some APIs are thread safe, therefore the custom allocator must be thread
 * safe if those APIs will be used.
 *
 * Uninitialized fields may result in undefined behavior.
 * 
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @var PalAllocator::allocate
 * The allocate function of the allocator. Must not be `nullptr`.
 * 
 * @var PalAllocator::free
 * The free function of the allocator. Must not be `nullptr`.
 * 
 * @var PalAllocator::userData
 * User data passed to allocate and free function. Can be `nullptr`.
 */
typedef struct PalAllocator 
{
    PalAllocateFn allocate;
    PalFreeFn free;
    void* userData;
} PalAllocator;

/**
 * @brief Allocates memory using a custom or default allocator.
 *
 * This function allocate atleast `size` of memory with the requested
 * `alignment`. 
 * If allocations will be made from multiple threads, the `allocator`
 * parameter must be thread safe.
 * 
 * After memory has been allocated, the allocator must stay valid untill
 * the memory is deallocated. We strictly require the allocator
 * used to allocate the memory is the same used to free the memory.
 * PAL does not validate this and might crash your system.
 *
 * @param[in] allocator The allocator. `nullptr` to use the default 
 * thread-safe allocator.
 * @param[in] size Number of bytes to allocate. A size of `0` is
 * implementation-defined.
 * @param[in] alignment Must be power of two. An alignment of `0` uses the
 * implementation-defined default.
 *
 * @return Allocated memory on success, or `nullptr` on failure.
 *
 * @Thread-safety `allocator` implementation must be thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * @sa palFree
 */
PAL_API void* PAL_CALL palAllocate(
    const PalAllocator* allocator,
    uint64_t size,
    uint64_t alignment);

/**
 * @brief Deallocates memory allocated by palAllocate.
 *
 * The memory must be valid and not deallocated before this call.
 * This function does not set the `ptr` to `nullptr` after
 * deallocation. We recommended the caller does that after this call to
 * prevent any double deallocations.
 *
 * If deallocations will be made from multiple threads, the `allocator`
 * must be thread safe.
 *
 * @param[in] allocator The allocator used to allocate the memory. `nullptr` for
 * the default thread-safe allocator.
 * @param[in] ptr Memory to free. Must be valid.
 *
 * @Thread-safety `allocator` implementation must be thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * @sa palAllocate
 */
PAL_API void PAL_CALL palFree(
    const PalAllocator* allocator,
    void* ptr);

#endif // PAL_CORE_MEMORY_H