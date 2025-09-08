
#include "tests.h"
#include "pal/pal_config.h" // for systems reflection

int main(int argc, char**) {

    PalVersion version = palGetVersion();
    palLog(nullptr, "%s: %s", "PAL Version", palGetVersionString());

    // core
    // registerTest("Logger Test", loggerTest);
    // registerTest("Time Test", timeTest);

#if PAL_HAS_SYSTEM
    // registerTest("System Test", systemTest);
#endif // PAL_HAS_SYSTEM

#if PAL_HAS_THREAD
    // registerTest("Thread Test", threadTest);
    // registerTest("TLS Test", tlsTest);
    // registerTest("Mutex Test", mutexTest);
    // registerTest("Condvar Test", condvarTest);
#endif // PAL_HAS_THREAD

#if PAL_HAS_VIDEO
    // registerTest("Video Test", videoTest);
    // registerTest("Display Test", displayTest);
    // registerTest("Display Mode Test", displayModeTest);
    // registerTest("Window Test", windowTest);
    // registerTest("Window Icon Test", windowIconTest);
    registerTest("Input Window Test", inputWindowTest);
#endif // PAL_HAS_VIDEO

    runTests();
    return 0;
}