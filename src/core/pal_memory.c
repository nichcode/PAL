
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#if defined(_MSC_VER) || defined(__MINGW32__)
#include <malloc.h>
#endif // _MSC_VER

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L
#include "pal/core/memory.h"
#include <stdlib.h>

#define PAL_DEFAULT_ALIGNMENT 16

static inline void* alignedAlloc(
    Uint64 size,
    Uint64 alignment)
{
#if defined(_MSC_VER) || defined(__MINGW32__)
    return _aligned_malloc(size, alignment);
#elif defined(_ISOC11_SOURCE) || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    return aligned_alloc(alignment, size);
#else
    void* ptr = nullptr;
    posix_memalign(&ptr, alignment, size);
    return ptr;
#endif // _MSC_VER
}

static inline void alignedFree(void* ptr)
{
#if defined(_MSC_VER) || defined(__MINGW32__)
    _aligned_free(ptr);
#else
    free(ptr);
#endif // _MSC_VER
}

void* PAL_CALL palAllocate(
    const PalAllocator* allocator,
    Uint64 size,
    Uint64 alignment)
{
    Uint64 align = alignment;
    if (align == 0) {
        align = PAL_DEFAULT_ALIGNMENT;
    }

    if (allocator && allocator->allocate && size != 0) {
        return allocator->allocate(allocator->userData, size, align);
    }
    return alignedAlloc(size, align);
}

void PAL_CALL palFree(
    const PalAllocator* allocator,
    void* ptr)
{
    if (allocator && allocator->free && ptr) {
        allocator->free(allocator->userData, ptr);

    } else {
        alignedFree(ptr);
    }
}