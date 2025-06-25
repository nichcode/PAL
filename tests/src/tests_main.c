
#include "pal/pal.h"

void onLog(PalLogLevel level, const char* msg)
{
    palLogInfo(msg);
}

int main(int argc, char** argv)
{
    PalError error = PAL_ERROR_NONE;
    PAlAllocator allocator;
    palZeroMemory(&allocator, sizeof(PAlAllocator));
    Uint32 flags = PAL_INIT_EVERYTHING;

    if (palInit(PAL_NULL, flags)) {
        PalVersion version = palGetVersion();
        palLogInfo("PAL Version (%i.%i.%i)", version.major, version.minor, version.patch);

        palShutdown();
        return 0;
    }

    error = palGetError();
    palLogError(palFormatError(error));
    return -1;
}