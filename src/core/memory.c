
/**
 * PAL - Prime Abstraction Layer (PAL)
 * A cross platform abstraction layer over graphics and windowing APIs
 * -------------------------------------------------------------------
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

#include "pal2/pal_core.h"
#include "shared.h"
#include <stdlib.h>

void* PAL_CALL palAllocate(
    const PalAllocator* allocator,
    uint64_t size,
    uint64_t alignment)
{
    uint64_t align = alignment;
    if (align == 0) {
        align = 16;
    }

    if (allocator) {
        return allocator->allocate(allocator->userData, size, align);
    }

    uint64_t totalSize = (size + align - 1) * sizeof(void*);
    void* block = malloc(totalSize);
    if (!block) {
        return nullptr;
    }

    uintptr_t address = (uintptr_t)block + sizeof(void*);
    uintptr_t aligned = ALIGN_(address, align);

    ((void**)aligned)[-1] = block;
    return (void*)aligned;
}

void PAL_CALL palFree(
    const PalAllocator* allocator,
    void* ptr)
{
    if (allocator) {
        allocator->free(allocator->userData, ptr);
    } else {
        free(((void**)ptr)[-1]);
    }
}