
#include "allocator.h"

// TODO: remove use our own logger 
#include <stdio.h>

void onPALError(PAL_Result code, const char* msg)
{
    printf("%s\n", msg);
}

int main(int argc, char** argv)
{
    PAL_Result result;
    // this only works if PAL_INIT_FLAG_DEBUG is set
    palSetDebugCallback(onPALError);

    // check the lifetime. PAL reference the allocator, it does not copy it
    // always zero out a struct
    PalAllocator allocator;
    palZeroMemory(&allocator, sizeof(PalAllocator));
    allocator.allocate = allocatorAlloc;
    allocator.free = allocatorFree;

    PalInitInfo initInfo;
    initInfo.allocator = &allocator;
    initInfo.flags = PAL_DEBUG_BIT;

    result = palInit(&initInfo);
    if (result == PAL_SUCCESS) {

        palTerminate();
        return 0;
    }

    // destroy your allocator eg. arena, dynamic, pool, linear etc
    palSetDebugCallback(PAL_NULL);

    return -1;
}