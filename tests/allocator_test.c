
#include <stdlib.h>
#include <memory.h>

#include "tests.h"

void* allocatorAlloc(Uint64 size) 
{
    void* block = malloc(size);
    memset(block, 0, size);
    PAL_Log(PAL_LOG_INFO, "Allocating memory of size %llu at address %p", size, block);
    return block;
}

void allocatorFree(void* memoryBlock) 
{
    PAL_Log(PAL_LOG_INFO, "Deallocating memory address %p", memoryBlock);
    free(memoryBlock);
}

PAL_BOOL allocatorTest()
{
    // custom allocator 
    PAL_Allocator allocator = {};
    allocator.alloc = allocatorAlloc;
    allocator.free = allocatorFree;

    Uint32 flags = PAL_DEBUG | PAL_CUSTOM_ALLOCATOR;
    PAL_BOOL success = PAL_Init(flags);
    PAL_BOOL allocatorSuccess = PAL_SetAllocator(&allocator);
    if (success && allocatorSuccess) {

        int* pInt = PAL_Alloc(sizeof(int));
        *pInt = 10;
        PAL_LogInfo("%i", *pInt);

        PAL_Free(pInt);

        PAL_Terminate();
        
    } else { return PAL_FALSE; }

    return PAL_TRUE;
}