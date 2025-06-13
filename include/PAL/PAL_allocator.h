
#ifndef PAL_ALLOCATOR_H
#define PAL_ALLOCATOR_H

#include "PAL_defines.h"

typedef void* (*PAL_AllocatorAllocFun)(Uint64 size);
typedef void (*PAL_AllocatorFreeFun)(void* memoryBlock);

typedef struct PAL_Allocator
{
    PAL_AllocatorAllocFun alloc;
    PAL_AllocatorFreeFun free;

} PAL_Allocator;

PAL_API PAL_Allocator* PAL_GetAllocator();
PAL_API PAL_BOOL PAL_SetAllocator(PAL_Allocator* allocator);

PAL_API void* PAL_Alloc(Uint64 size);
PAL_API void PAL_Free(void* memoryBlock);

#endif // PAL_ALLOCATOR_H
