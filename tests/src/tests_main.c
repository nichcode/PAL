
#include "tests.h"

void onLog(PalLogLevel level, const char* msg)
{
    palLogInfo(msg);
}

bool systemTest(void* data)
{
    PalError error = PAL_ERROR_NONE;
    PalAllocator allocator;
    palZeroMemory(&allocator, sizeof(PalAllocator));
    Uint32 flags = PAL_INIT_EVERYTHING;

    if (palInit(PAL_NULL, flags)) {
        PalVersion version = palGetVersion();
        palLogInfo("PAL Version (%i.%i.%i)", version.major, version.minor, version.patch);
        palLogInfo(palGetPlatformString());

        palShutdown();
        return PAL_TRUE;
    }

    error = palGetError();
    palLogError(palFormatError(error));
    return PAL_FALSE;
}

int main(int argc, char** argv)
{
    //registerTest("System Test", systemTest);
    registerTest("Video Test", videoTest);

    runTests();
    return 0;
}