
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef _WIN32
#include "pal2/pal_core.h"

#if defined(_MSC_VER) || defined(__MINGW32__)
#include <malloc.h>
#endif // _MSC_VER

void* PAL_CALL palAllocate(
    const PalAllocator* allocator,
    uint64_t size,
    uint64_t alignment)
{
    uint64_t align = alignment;
    if (align == 0) {
        align = 16; // default
    }

    if (allocator) {
        return allocator->allocate(allocator->userData, size, align);
    }

    return _aligned_malloc(size, align);
}

void PAL_CALL palFree(
    const PalAllocator* allocator,
    void* ptr)
{
    if (allocator) {
        allocator->free(allocator->userData, ptr);
    } else {
        _aligned_free(ptr);
    }
}

#endif // _WIN32