
#include "allocator.h"

int main(int argc, char** argv)
{
    PAL_Result result;

    // check the lifetime. PAL reference the allocator, it does not copy it
    // always zero out a struct
    PalAllocator allocator;
    palZeroMemory(&allocator, sizeof(PalAllocator));
    allocator.allocate = allocatorAlloc;
    allocator.free = allocatorFree;

    PalInitInfo initInfo;
    initInfo.allocator = &allocator;
    initInfo.flags = PAL_INIT_FLAG_DEBUG;

    result = palInit(&initInfo);
    if (result == PAL_RESULT_OK) {

        palTerminate();
        return 0;
    }

    // destroy your allocator eg. arena, dynamic, pool, linear etc

    return -1;
}