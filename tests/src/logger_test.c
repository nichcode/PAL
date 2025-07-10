
#include "tests.h"
#include "pal/pal_core.h"

#define SIZE 1024

void OnLogger1(void* userData, PalLogLevel level, const char* msg)
{
    char buffer[SIZE] = {};
    palFormat(buffer, "%s: %s", "Logger 1", msg);
    palLogConsole(level, buffer);
}

void OnLogger2(void* userData, PalLogLevel level, const char* msg)
{
    char buffer[SIZE] = {};
    palFormat(buffer, "%s: %s", "Logger 2", msg);
    palLogConsole(level, buffer);
}

void OnLogger3(void* userData, PalLogLevel level, const char* msg)
{
    char buffer[SIZE] = {};
    palFormat(buffer, "%s: %s", "Logger 3", msg);
    palLogConsole(level, buffer);
}

PalResult loggingTest()
{
    palLogConsoleInfo("");
    palLogConsoleInfo("===========================================");
    palLogConsoleInfo("Logger Test");
    palLogConsoleInfo("===========================================");
    palLogConsoleInfo("");

    PalLogger loggers[3];
    loggers[0].callback = OnLogger1;
    loggers[0].userData = PAL_NULL;

    loggers[1].callback = OnLogger2;
    loggers[1].userData = PAL_NULL;

    loggers[2].callback = OnLogger3;
    loggers[2].userData = PAL_NULL;

    palLogInfo(&loggers[0], "Log to Logger 1 callback");
    palLogInfo(&loggers[1], "Log to Logger 2 callback");
    palLogInfo(&loggers[2], "Log to Logger 3 callback");

    return PAL_RESULT_OK;
}