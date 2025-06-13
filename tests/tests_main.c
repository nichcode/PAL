
#include "PAL/PAL.h"

int main(int argc, char** argv)
{
    PAL_InitDesc initDesc;
    initDesc.flags = PAL_INIT_FLAGS_DEBUG;
    PAL_BOOL success = PAL_Init(&initDesc);
    if (success) {
        PAL_Terminate();
        return 0;
    }

    return -1;
}