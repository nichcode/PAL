
#ifndef PAL_H
    #define PAL_H

#include "pal/pal_log.h"

PAL_API PAL_Result PAL_CALL palInit(const PalInitInfo* pInfo);
PAL_API void PAL_CALL palTerminate();

PAL_API PAL_Result PAL_CALL palAllocate(Uint64 size, void** ppMemory);
PAL_API PAL_CALL void palFree(void* pMemory);

PAL_API void PAL_CALL palZeroMemory(void* pMemory, Uint64 size);
PAL_API void PAL_CALL palSetMemory(void* pMemory, int value, Uint64 size);

PAL_API void PAL_CALL palCopyMemory(
    void* pDestMemory, 
    void* pSrcMemory, 
    Uint64 size);

PAL_API void PAL_CALL palSetDebugCallback(PalDebugMessageFn pCallback);

#endif // PAL_H