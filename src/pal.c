
#include "pal_pch.h"
#include "pal_internal.h"
#include "pal_platform.h"

PAL_Result PAL_CALL palInit(const PalInitInfo* pInfo)
{
    // to get init errors
    s_PAL.flags = PAL_DEBUG_BIT;

    if (!pInfo) {
        PAL_SET_ERROR(
            PAL_INVALID_ERROR,
            "PalInitInfo struct pointer is invalid"
        )
        return PAL_INVALID_ERROR;
    }

    if (!pInfo->allocator) {
        PAL_SET_ERROR(
            PAL_INVALID_ERROR,
            "PalInitInfo struct pointer does not have a valid allocator pointer"
        )
        return PAL_INVALID_ERROR;
    }

    if (!pInfo->allocator->allocate) {
        PAL_SET_ERROR(
            PAL_PLATFORM_WARN,
            "Allocator does not have an allocate function pointer"
        )
    }

    if (!pInfo->allocator->free) {
        PAL_SET_ERROR(
            PAL_PLATFORM_WARN,
            "Allocator does not have a free function pointer"
        )
    }

    s_PAL.allocator = pInfo->allocator;
    s_PAL.flags = pInfo->flags;
    s_PAL.initialized = PAL_TRUE;

    return PAL_SUCCESS;
}

void PAL_CALL palTerminate()
{
    s_PAL.flags = 0;
    s_PAL.initialized = PAL_FALSE;
}

PAL_Result PAL_CALL palAllocate(Uint64 size, void** ppMemory)
{
    void* block = palPlatformAlloc(size);
    if (block) {
        palPlatformMemZero(block, size);
        *ppMemory = block;
    } else {
        PAL_SET_ERROR(
            PAL_OUT_OF_MEMORY,
            "Memory allocation failed"
        )
    }
    return PAL_OUT_OF_MEMORY;
}

void PAL_CALL palFree(void* pMemory)
{
    if (pMemory) {
        palPlatformFree(pMemory);
    }
}

void PAL_CALL palZeroMemory(void* pMemory, Uint64 size)
{
    if (pMemory && size) {
        palPlatformMemZero(pMemory, size);
    }
}

void PAL_CALL palSetMemory(void* pMemory, int value, Uint64 size)
{
    if (pMemory && size) {
        palPlatformMemSet(pMemory, value, size);
    }
}

void PAL_CALL palCopyMemory(
    void* pDestMemory, 
    void* pSrcMemory, 
    Uint64 size)
{
    if (pDestMemory && pSrcMemory && size) {
        palPlatformMemCpy(pDestMemory, pSrcMemory, size);
    }
}

void PAL_CALL palSetDebugCallback(PalDebugMessageFn pCallback)
{
    s_PAL.debugCallback = pCallback;
}
