
#include "pal_pch.h"
#include "pal_win32platform.h"

void* palPlatformAlloc(Uint64 size)
{
    return malloc(size);
}

void palPlatformFree(void* memory)
{
    free(memory);
}

void palPlatformMemZero(void* memory, Uint64 size)
{
    memset(memory, 0, size);
}

void palPlatformMemSet(void* memory, int value, Uint64 size)
{
    memset(memory, value, size);
}

void palPlatformMemCpy(void* destMemory, void* srcMemory, Uint64 size)
{
    memcpy(destMemory, srcMemory, size);
}

