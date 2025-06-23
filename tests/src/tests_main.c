
#include "pal/pal.h"

void onLog(PalError code, PalLogLevel level, const char* msg)
{
    palLogInfo(msg);
}

int main(int argc, char** argv)
{
    palSetLogCallback(onLog);

    if (palInitSystem(PAL_NULL, PAL_TRUE)) {
        const PalVersion* version = palGetVersion();

        palShutdownSystem();
        return 0;
    }

    return -1;
}