
#include "tests.h"

static void onLogger1(void* userData, const char* msg) {

    // if the logger paramter is not set to nullptr when logging in a log callback,
    // the log will be discard. Example doing this below:
    // and myLogger's callback function is the same function.
    // This will trigger a recursive call and pal will discard the log.
    //palLog(myLogger, "%s - %s", "Logger1 -", msg);

    palLog(nullptr, "%s: %s", "Logger1", msg);
}

static void onLogger2(void* userData, const char* msg) {

    // if the logger paramter is not set to nullptr when logging in a log callback,
    // the log will be discard. Example doing this below:
    // and myLogger's callback function is the same function.
    // This will trigger a recursive call and pal will discard the log.
    //palLog(myLogger, "%s - %s", "Logger1 -", msg);
    palLog(nullptr, "%s: %s", "Logger2", msg);
}

static void onLogger3(void* userData, const char* msg) {

    // if the logger paramter is not set to nullptr when logging in a log callback,
    // the log will be discard. Example doing this below:
    // and myLogger's callback function is the same function.
    // This will trigger a recursive call and pal will discard the log.
    //palLog(myLogger, "%s - %s", "Logger1 -", msg);
    palLog(nullptr, "%s: %s", "Logger3", msg);
}

bool loggerTest() {

    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Logger Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    // logger 1
    PalLogger logger1;
    logger1.userData = nullptr;
    logger1.callback = onLogger1;

    // logger 2
    PalLogger logger2;
    logger2.userData = nullptr;
    logger2.callback = onLogger2;

    // logger 3
    PalLogger logger3;
    logger3.userData = nullptr;
    logger3.callback = onLogger3;

    palLog(&logger1, "This is directed to a logger");
    palLog(&logger2, "This is directed to a logger");
    palLog(&logger3, "This is directed to a logger");

    return true;
}