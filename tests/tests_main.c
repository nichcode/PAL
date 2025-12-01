
#include "pal/pal_config.h" // for systems reflection
#include "tests.h"

// clang-format off
int main(int argc, char** argv)
{
    // clang-format on
    palLog(nullptr, "%s: %s", "PAL Version", palGetVersionString());

    // core
    // registerTest("Logger Test", loggerTest);
    // registerTest("Time Test", timeTest);
    // registerTest("User Event Test", userEventTest);
    // registerTest("Event Test", eventTest);

#if PAL_HAS_SYSTEM
    registerTest("System Test", systemTest);
#endif // PAL_HAS_SYSTEM

#if PAL_HAS_THREAD
    registerTest("Thread Test", threadTest);
    registerTest("TLS Test", tlsTest);
    registerTest("Mutex Test", mutexTest);
    registerTest("Condvar Test", condvarTest);
#endif // PAL_HAS_THREAD

#if PAL_HAS_VIDEO
    // registerTest("Video Test", videoTest);
    // registerTest("Monitor Test", monitorTest);
    // registerTest("Monitor Mode Test", monitorModeTest);
    // registerTest("Window Test", windowTest);
    // registerTest("Icon Test", iconTest);
    // registerTest("Cursor Test", cursorTest);
    // registerTest("Input Window Test", inputWindowTest);
    // registerTest("System Cursor Test", systemCursorTest);
    // registerTest("Attach Window Test", attachWindowTest);
    // registerTest("Character Event Test", charEventTest);
    // registerTest("Native Integration Test", nativeIntegrationTest);
    // registerTest("Native Instance Test", nativeInstanceTest);
    // registerTest("Custom Decoration Test", customDecorationTest);
#endif // PAL_HAS_VIDEO

    // This test can run without video system so long as your have a valid
    // window
#if PAL_HAS_OPENGL && PAL_HAS_VIDEO
    registerTest("Opengl Test", openglTest);
    registerTest("Opengl FBConfig Test", openglFBConfigTest);
    registerTest("Opengl Context Test", openglContextTest);
    registerTest("Opengl Multi Context Test", openglMultiContextTest);
#endif // PAL_HAS_OPENGL

#if PAL_HAS_OPENGL && PAL_HAS_VIDEO && PAL_HAS_THREAD
    registerTest("Multi Thread OpenGL Test", multiThreadOpenGlTest);
#endif

#if PAL_HAS_GRAPHICS
    // registerTest("Graphics Test", graphicsTest);
    registerTest("Device Test", deviceTest);
#endif // PAL_HAS_GRAPHICS

#if PAL_HAS_GRAPHICS && PAL_HAS_VIDEO
    // registerTest("Swapchain Test", swapchainTest);
#endif

    runTests();
    return 0;
}