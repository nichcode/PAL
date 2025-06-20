
#ifndef PAL_INTERNAL_H
#define PAL_INTERNAL_H

#include "pal/pal.h"

typedef struct PalLibrary
{
    PalAllocator* allocator;
    Uint32 flags;
    bool initialized;

} PalLibrary;

static PalLibrary s_PAL;

#endif // PAL_INTERNAL_H