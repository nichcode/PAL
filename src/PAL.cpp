
#include "PAL_pch.h"
#include "PAL_platform.h"

PAL_BOOL PAL_Init(PAL_InitDesc* desc)
{
    // get platform  allocator
    s_PAL.defaultAllocator = PAL_PlatformGetAllocator();

    if (desc->flags & PAL_DEBUG) {
        s_PAL.debug = PAL_TRUE;
    }

    if (desc->flags & PAL_CUSTOM_ALLOCATOR) {
        if (desc->allocator) { 
            PAL_SetError(
                PAL_PLATFORM_ERROR, 
                "User requested a custom allocator with an invalid allocator"
            );
            return PAL_FALSE;
        }
        s_PAL.allocator = desc->allocator;
    } else {
        s_PAL.allocator = &s_PAL.defaultAllocator;
    }

    return PAL_TRUE;
}

void PAL_Terminate()
{

}