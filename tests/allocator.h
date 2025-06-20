
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "pal/pal.h"

static void* allocatorAlloc(Uint64 size)
{
    void* block = PAL_NULL;
    palAllocate(size, &block);
    return block;
}

static void allocatorFree(void* memory)
{
    if (memory) {
        palFree(memory);
    }
}

#endif // ALLOCATOR_H