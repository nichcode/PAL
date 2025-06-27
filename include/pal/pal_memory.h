
#ifndef _PAL_MEMORY_H
 #define _PAL_MEMORY_H

#include "pal_defines.h"

typedef void* (*PalAllocFn)(Uint64 size);
typedef void* (*PalAlignedAllocFn)(Uint64 size, Uint64 alignment);
typedef void (*PalFreeFn)(void* memory);
typedef void (*PalAlignedFreeFn)(void* memory);

typedef struct PalAllocator
{
    PalAllocFn alloc;
    PalAlignedAllocFn alignedAlloc;
    PalFreeFn free;
    PalAlignedFreeFn alignedFree;
    void* userData;

} PalAllocator;

_PAPI PalAllocator _PCALL palGetDefaultAllocator();
_PAPI void _PCALL palSetMemory(void* memory, int value, Uint64 size);

_PAPI void* _PCALL palAllocate(Uint64 size);
_PAPI void* _PCALL palAlignAllocate(Uint64 size, Uint64 alignment);
_PAPI void _PCALL palFree(void* memory);
_PAPI void _PCALL palAlignFree(void* memory);

static void _PCALL palZeroMemory(void* memory, Uint64 size)
{
    palSetMemory(memory, 0, size);
}

#endif // _PAL_MEMORY_H