
#ifndef _PAL_SYSTEM_H
 #define _PAL_SYSTEM_H

#include "pal_memory.h"

typedef struct PalVersion
{
    int major;
    int minor;
    int patch;

} PalVersion;

typedef enum PalInitFlags
{
    PAL_INIT_NONE = 0,
    PAL_INIT_VIDEO = PAL_BIT(0),
    PAL_INIT_EVERYTHING = PAL_INIT_VIDEO

} PalInitFlags;

_PAPI bool _PCALL palInit(const PalAllocator* allocator, Uint32 flags);
_PAPI void _PCALL palShutdown();

_PAPI bool _PCALL palIsSystemInitialized();
_PAPI PalVersion _PCALL palGetVersion();
_PAPI const char* _PCALL palFormatError(PalError error);

#endif // _PAL_SYSTEM_H