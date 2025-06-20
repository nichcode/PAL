
#include "pal_pch.h"
#include "pal_internal.h"
#include "pal_platform.h"

PAL_Result PAL_CALL palInit(const PalInitInfo* info)
{
    if (!info) {
        return PAL_RESULT_INVALID_ERROR;
    }

    if (!info->allocator) {
        return PAL_RESULT_INVALID_ERROR;
    }

    if (!info->allocator->allocate || !info->allocator->free) {
        return PAL_RESULT_INVALID_ERROR;
    }

    s_PAL.allocator = info->allocator;
    s_PAL.flags = info->flags;
    s_PAL.initialized = PAL_TRUE;

    return PAL_RESULT_OK;
}

void PAL_CALL palTerminate()
{
    s_PAL.flags = 0;
    s_PAL.initialized = PAL_FALSE;
}

PAL_Result PAL_CALL palAllocate(Uint64 size, void** memory)
{
    void* block = palPlatformAlloc(size);
    if (block) {
        palPlatformMemZero(block, size);
        *memory = block;
    }
    return PAL_RESULT_OUT_OF_MEMORY;
}

void PAL_CALL palFree(void* memory)
{
    if (memory) {
        palPlatformFree(memory);
    }
}

void PAL_CALL palZeroMemory(void* memory, Uint64 size)
{
    if (memory && size) {
        palPlatformMemZero(memory, size);
    }
}

void PAL_CALL palSetMemory(void* memory, int value, Uint64 size)
{
    if (memory && size) {
        palPlatformMemSet(memory, value, size);
    }
}

void PAL_CALL palCopyMemory(
    void* destMemory, 
    void* srcMemory, 
    Uint64 size)
{
    if (destMemory && srcMemory && size) {
        palPlatformMemCpy(destMemory, srcMemory, size);
    }
}
