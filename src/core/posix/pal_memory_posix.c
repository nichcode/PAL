
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal_platform.h"

#if _PAL_HAS_POSIX
#define _POSIX_C_SOURCE 200112L
#include "pal/pal_core.h"
#include <stdlib.h>

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

    void* ptr = nullptr;
    posix_memalign(&ptr, align, size);
    return ptr;
}

void PAL_CALL palFree(
    const PalAllocator* allocator,
    void* ptr)
{
    if (allocator) {
        allocator->free(allocator->userData, ptr);
    } else {
        free(ptr);
    }
}

#endif // _PAL_HAS_POSIX