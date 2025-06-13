
#ifndef PAL_PLATFORM_H
#define PAL_PLATFORM_H

#include "PAL/PAL.h"

struct PAL_PlatformData
{
    PAL_Allocator defaultAllocator;
    PAL_Allocator* allocator = nullptr;
    bool debug = false;
};

static PAL_PlatformData s_PAL;

PAL_Allocator PAL_PlatformGetAllocator();

#endif // PAL_PLATFORM_H