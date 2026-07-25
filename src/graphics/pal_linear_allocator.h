
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_LINEAR_ALLOCATOR_H
#define _PAL_LINEAR_ALLOCATOR_H

#include "pal/pal_core.h"

#define align(v, a) (v + a - 1) & ~(a - 1)

typedef struct {
    uint8_t* memory;
    uint64_t size;
    uint64_t offset;
} PalLinearAllocator;

static void* palLinearAlloc(
    PalLinearAllocator* allocator,
    uint64_t size,
    uint64_t alignment)
{
    uint64_t defAlign = alignment;
    if (alignment == 0) {
        defAlign = 16;
    }

    uint64_t offset = align(allocator->offset, defAlign);
    if (offset + size > allocator->size) {
        // allocate a bigger block
        void* block = palAllocate(nullptr, allocator->size * 2, 0);
        if (!block) {
            return nullptr;
        }

        memcpy(block, (const void*)allocator->memory, allocator->size);
        palFree(nullptr, (void*)allocator->memory);
        allocator->memory = (uint8_t*)block;
        allocator->size = allocator->size * 2;
    }

    void* ptr = allocator->memory + offset;
    allocator->offset = offset + size;
    return ptr;
}

#endif // _PAL_LINEAR_ALLOCATOR_H