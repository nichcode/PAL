
#include "pal_pch.h"
#include "pal/pal_core.h"

void* _PCALL palAlloc(
    PalAllocator* allocator, 
    Uint64 size)
{
    if (!allocator) {
        return malloc(size);
    }

    if (allocator && allocator->alloc) {
        return allocator->alloc(allocator->userData, size);
    }
}

void _PCALL palFree(
    PalAllocator* allocator, 
    void* ptr)
{
    if (!allocator) {
        return free(ptr);
    }

    if (allocator && allocator->free) {
        return allocator->free(allocator->userData, ptr);
    }
}