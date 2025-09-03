
#include "tests.h"

#define LOGGER_COUNT 4

static const char* g_LoggerNames[LOGGER_COUNT] = {
    "Logger1",
    "Logger2",
    "Logger3",
    "Logger4"
};

static void PAL_CALL onLogger(
    void* userData, 
    const char* msg) {

    // if the logger paramter is not set to nullptr when logging in a log callback,
    // the log will be discard. Example doing this below:
    // and myLogger's callback function is the same function.
    // This will trigger a recursive call and pal will discard the log.
    //palLog(myLogger, "%s - %s", "Logger1 -", msg);

    char* name = (char*)userData;
    palLog(nullptr, "%s: %s", name, msg);
}

bool loggerTest() {

    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Logger Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalLogger loggers[LOGGER_COUNT];
    for (Int32 i = 0; i < LOGGER_COUNT; i++) {
        loggers[i].callback = onLogger;
        loggers[i].userData = (void*)g_LoggerNames[i];
    }

    for (Int32 i = 0; i < LOGGER_COUNT; i++) {
        // push a log message to all loggers
        palLog(&loggers[i], "This is directed to a logger");
    }

    return true;
}