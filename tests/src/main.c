
#include "pal/pal.h"

void OnLoggerLog(void* userData, PalLogLevel level, const char* msg)
{
    palLogConsole(level, msg);
}

int main(int argc, char** argv) 
{
    PalVersion version;
    palGetVerion(&version);
    palLogConsoleInfo("PAL v(%i.%i.%i)", version.major, version.minor, version.patch);

    // logger
    PalLogger logger;
    logger.userData = PAL_NULL;
    logger.callback = OnLoggerLog;
    palLog(&logger, PAL_LOG_LEVEL_TRACE, "Hello from float - %f", 10.0f);

    return 0;
}