
#ifndef PAL_H
#define PAL_H

#include "pal_defines.h"

PAL_API PAL_Result PAL_CALL palInit(const PalInitInfo* info);
PAL_API void PAL_CALL palTerminate();

PAL_API PAL_Result PAL_CALL palAllocate(Uint64 size, void** memory);
PAL_API PAL_CALL void palFree(void* memory);

PAL_API void PAL_CALL palZeroMemory(void* memory, Uint64 size);
PAL_API void PAL_CALL palSetMemory(void* memory, int value, Uint64 size);

PAL_API void PAL_CALL palCopyMemory(
    void* destMemory, 
    void* srcMemory, 
    Uint64 size);

#endif // PAL_H