
#include "PAL_pch.h"
#include "PAL_platform.h"

PAL_BOOL PAL_Init(Uint32 flags)
{
    // get platform  allocator
    s_PAL.defaultAllocator = PAL_PlatformGetAllocator();

    if (flags & PAL_DEBUG) {
        s_PAL.debug = PAL_TRUE;
    }

    if (flags & PAL_CUSTOM_ALLOCATOR) {
        s_PAL.allocator = PAL_NULL;
        s_PAL.customAllocator = PAL_TRUE;
    } else {
        s_PAL.allocator = &s_PAL.defaultAllocator;
        s_PAL.customAllocator = PAL_FALSE;
    }

    s_PAL.initialized = PAL_TRUE;
    return PAL_TRUE;
}

void PAL_Terminate()
{
    s_PAL.initialized = PAL_FALSE;
}