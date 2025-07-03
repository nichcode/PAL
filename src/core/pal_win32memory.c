
#include "pal_pch.h"
#include "pal/pal_core.h"

void* _PCALL palAlloc(Uint64 size)
{
    return malloc(size);
}

void _PCALL palFree(void* memory)
{
    free(memory);
}

void _PCALL palSetMemory(void* memory, int value, Uint64 size)
{
    if (memory) {
        memset(memory, value, size);
    }
}

void _PCALL palCopyMemory(void* dest, void* src, Uint64 size)
{
    if (dest && src) {
        memcpy(dest, src, size);
    }
}