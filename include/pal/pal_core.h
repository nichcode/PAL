
#ifndef _PAL_CORE_H
#define _PAL_CORE_H

#include "pal_defines.h"

typedef struct PalVersion
{
    int major;
    int minor;
    int patch;
} PalVersion;

_PAPI void _PCALL palGetVerion(PalVersion* version);
_PAPI Uint32 _PCALL palGetVerionInt();

#endif // _PAL_CORE_H