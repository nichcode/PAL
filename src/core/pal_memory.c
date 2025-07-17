
#include "pal_pch.h"
#include "pal/pal_core.h"

void* _PCALL palAllocate(PalAllocator* allocator, Uint64 size) {
    
    if (allocator && allocator->alloc) {
        return allocator->alloc(allocator->userData, size);
    }
    return malloc(size);
}

void _PCALL palFree(PalAllocator* allocator, void* memory) {

    if (allocator && allocator->free) {
        allocator->free(allocator->userData, memory);
        return;
    }
    free(memory);
}

void _PCALL palSetMemory(void* memory, int value, Uint64 size) {

    memset(memory, value, size);
}

void _PCALL palZeroMemory(void* memory, Uint64 size) {

    memset(memory, 0, size);
}

void _PCALL palCopyMemory(void* dest, const void* src, Uint64 size) {

    memcpy(dest, src, size);
}

void _PCALL palMoveMemory(void* dest, const void* src, Uint64 size) {

    memmove(dest, src, size);
}
