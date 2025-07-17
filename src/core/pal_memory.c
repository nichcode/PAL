
#include "pal_pch.h"
#include "pal/pal_core.h"

void* _PCALL palAllocate(Uint64 size) {
    
    return malloc(size);
}

void _PCALL palFree(void* memory) {

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
