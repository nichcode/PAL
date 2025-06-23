
#ifndef _PAL_SYSTEM_H
 #define _PAL_SYSTEM_H

#include "pal_core.h"

typedef struct PalVersion
{
    int major;
    int minor;
    int patch;

} PalVersion;

_PAPI bool _PCALL palInitSystem(const PAlAllocator* allocator, bool debug);
_PAPI void _PCALL palShutdownSystem();

_PAPI const PalVersion* _PCALL palGetVersion();
_PAPI const PAlAllocator* _PCALL palGetSysAllocator();

_PAPI void* _PCALL palAllocate(Uint64 size);
_PAPI void* _PCALL palAlignAllocate(Uint64 size, Uint64 alignment);
_PAPI void _PCALL palFree(void* memory);
_PAPI void _PCALL palAlignFree(void* memory);

#endif // _PAL_SYSTEM_H