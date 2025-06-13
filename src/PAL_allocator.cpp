
#include "PAL_pch.h"
#include "PAL_platform.h"

PAL_BOOL PAL_SetAllocator(PAL_Allocator* allocator)
{
    if (!allocator) { return PAL_FALSE; }
    s_PAL.allocator = allocator;
    return PAL_TRUE;
}

PAL_Allocator* PAL_GetAllocator()
{
    return s_PAL.allocator;
}