
#include "PAL/PAL.h"

// TODO: remove and use PAL logger
#include <stdio.h>

void onPALError(Uint32 code, const char* description)
{
    printf(description);
}

int main(int argc, char** argv)
{
    PAL_SetErrorCallback(onPALError);

    PAL_InitDesc initDesc;
    initDesc.flags = PAL_INIT_FLAGS_DEBUG;
    PAL_BOOL success = PAL_Init(&initDesc);
    if (success) {
        PAL_Terminate();
        return 0;
    }

    return -1;
}