
#include "tests.h"

#define LOG_SIZE 1024

void OnLogger1(void* userData, PalLogLevel level, const char* msg) {

    char buffer[LOG_SIZE] = {};
    Uint64 size = LOG_SIZE;
    palFormat(buffer, &size, "%s: %s", "Logger 1", msg);

    // log to console
    palLog(PAL_NULL, level, buffer);
}

void OnLogger2(void* userData, PalLogLevel level, const char* msg) {

    char buffer[LOG_SIZE] = {};
    Uint64 size = LOG_SIZE;
    palFormat(buffer, &size, "%s: %s", "Logger 2", msg);

    // log to console
    palLog(PAL_NULL, level, buffer);
}

void OnLogger3(void* userData, PalLogLevel level, const char* msg) {

    char buffer[LOG_SIZE] = {};
    Uint64 size = LOG_SIZE;
    palFormat(buffer, &size, "%s: %s", "Logger 3", msg);

    // log to console
    palLog(PAL_NULL, level, buffer);
}

void logTest() {

    palLogInfo(PAL_NULL, "");
    palLogInfo(PAL_NULL, "===========================================");
    palLogInfo(PAL_NULL, "Logger Test");
    palLogInfo(PAL_NULL, "===========================================");
    palLogInfo(PAL_NULL, "");

    palLogInfo(PAL_NULL, "Pal v(%s)", palGetVersionString());
    palLogInfo(PAL_NULL, "Unicode - àà");

    PalLogger loggers[3];
    loggers[0].callback = OnLogger1;
    loggers[0].userData = PAL_NULL;

    loggers[1].callback = OnLogger2;
    loggers[1].userData = PAL_NULL;

    loggers[2].callback = OnLogger3;
    loggers[2].userData = PAL_NULL;

    palLogInfo(&loggers[0], "Routing");
    palLogInfo(&loggers[1], "Routing");
    palLogInfo(&loggers[2], "Routing");
}