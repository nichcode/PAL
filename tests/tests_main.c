
#include "PAL/PAL.h"

void onPALError(Uint32 code, const char* description)
{
    PAL_LogError(description);
}

int main(int argc, char** argv)
{
    PAL_SetErrorCallback(onPALError);

    PAL_InitDesc initDesc;
    initDesc.flags = PAL_INIT_FLAGS_DEBUG;
    PAL_BOOL success = PAL_Init(&initDesc);
    if (success) {
        PAL_LogInfo("Hello from PAL float %f", 20.0f);

        PAL_Terminate();
        return 0;
    }

    return -1;
}