
#include "tests.h"
#include "pal/pal_config.h" // for systems reflection

int main(int argc, char**) {

    PalVersion version = palGetVersion();
    palLog(nullptr, "%s: %s", "PAL Version", palGetVersionString());

    // core
    // registerTest("Logger Test", loggerTest);
    // registerTest("Time Test", timeTest);
    // registerTest("User Event Test", userEventTest);

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
    // registerTest("Monitor Test", monitorTest);
    // registerTest("Monitor Mode Test", monitorModeTest);
    // registerTest("Window Test", windowTest);
    // registerTest("Icon Test", iconTest);
    // registerTest("Cursor Test", cursorTest);
    // registerTest("Input Window Test", inputWindowTest);
#endif // PAL_HAS_VIDEO

#if PAL_HAS_OPENGL
    // registerTest("Opengl Test", openglTest);
#endif // PAL_HAS_OPENGL

    runTests();
    return 0;
}