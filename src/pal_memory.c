
#include "pal_pch.h"
#include "pal_internal.h"

void* _PCALL palAllocate(Uint64 size)
{
    if (!s_PAL.initialized) {
        return PAL_NULL;
    }
    return s_PAL.allocator->alloc(size);
}

void* _PCALL palAlignAllocate(Uint64 size, Uint64 alignment)
{
    if (!s_PAL.initialized) {
        return PAL_NULL;
    }
    return s_PAL.allocator->alignedAlloc(size, alignment);
}

void _PCALL palFree(void* memory)
{
    if (!s_PAL.initialized) {
        return;
    }
    return s_PAL.allocator->free(memory);
}

void _PCALL palAlignFree(void* memory)
{
    if (!s_PAL.initialized) {
        return;
    }
    return s_PAL.allocator->alignedFree(memory);
}