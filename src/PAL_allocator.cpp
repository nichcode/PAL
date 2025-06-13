
#include "PAL_pch.h"
#include "PAL_platform.h"

PAL_BOOL PAL_SetAllocator(PAL_Allocator* allocator)
{
    if (!s_PAL.customAllocator) { return PAL_FALSE; }

    if (!allocator) { 
        PAL_SetError(PAL_INVALID_ERROR, "PAL_SetAllocator expects a valid allocator");
        return PAL_FALSE; 
    }

    if (!allocator->alloc) { 
        PAL_SetError(PAL_INVALID_ERROR, "Allocator does not have a valid allocate function pointer");
        return PAL_FALSE; 
    }

    if (!allocator->free) { 
        PAL_SetError(PAL_INVALID_ERROR, "Allocator does not have a valid free function pointer");
        return PAL_FALSE; 
    }

    s_PAL.allocator = allocator;
    return PAL_TRUE;
}

PAL_Allocator* PAL_GetAllocator()
{
    return s_PAL.allocator;
}

void* PAL_Alloc(Uint64 size)
{
    PAL_CHECK_INIT(PAL_NULL)
    if (s_PAL.customAllocator) {
        if (!s_PAL.allocator) {
            PAL_SetError(
                PAL_ALLOCATION_ERROR,
                "No allocator available. Set with PAL_SetAllocator. Custom allocator flag must be set."
            );
            return PAL_NULL;
        }
    }

    return s_PAL.allocator->alloc(size);
}

void PAL_Free(void* memoryBlock)
{
    PAL_CHECK_INIT()
    if (s_PAL.customAllocator) {
        if (!s_PAL.allocator) {
            PAL_SetError(
                PAL_ALLOCATION_ERROR,
                "No allocator available. Set with PAL_SetAllocator. Custom allocator flag must be set."
            );
            return;
        }
    }

    s_PAL.allocator->free(memoryBlock);
}