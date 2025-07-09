
#ifndef _PAL_CORE_H
#define _PAL_CORE_H

#include "pal_defines.h"

typedef int PalTLSID;

typedef void* (*PalAllocFn)(void*, Uint64);
typedef void (*PalFreeFn)(void*, void*);

typedef struct PalVersion
{
    int major;
    int minor;
    int patch;
} PalVersion;

typedef struct PalAllocator
{
    PalAllocFn alloc;
    PalFreeFn free;
    void* userData;

} PalAllocator;

_PAPI void _PCALL palGetVerion(PalVersion* version);
_PAPI Uint32 _PCALL palGetVerionInt();

_PAPI void* _PCALL palAlloc(PalAllocator* allocator, Uint64 size);
_PAPI void _PCALL palFree(PalAllocator* allocator, void* ptr);

_PAPI PalTLSID _PCALL palCreateTLS();
_PAPI void _PCALL palDestroyTLS(PalTLSID id);
_PAPI void* _PCALL palGetTLS(PalTLSID id);
_PAPI void _PCALL palSetTLS(PalTLSID id, void* data);

#endif // _PAL_CORE_H