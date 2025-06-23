
#ifndef _PAL_SYSTEM_H
 #define _PAL_SYSTEM_H

#include "pal_core.h"

typedef struct PalVersion
{
    int major;
    int minir;
    int patch;

} PalVersion;

_PAPI bool _PCALL palInitSystem(const PAlAllocator* allocator, bool debug);
_PAPI void _PCALL palShutdownSystem();

_PAPI const PalVersion* _PCALL palGetVersion();
_PAPI const PAlAllocator* _PCALL palGetSysAllocator();

#endif // _PAL_SYSTEM_H