
#include "pal/pal_config.h" // for systems reflection
#include "tests.h"

#include "pal/pal_graphics.h"

// clang-format off
int main(int argc, char** argv)
{
    // clang-format on
    palLog(nullptr, "%s: %s", "PAL Version", palGetVersionString());

    // core
    // registerTest(loggerTest);
    // registerTest(timeTest);
    // registerTest(userEventTest);
    // registerTest(eventTest);

#if PAL_HAS_SYSTEM
    registerTest(systemTest);
#endif // PAL_HAS_SYSTEM

#if PAL_HAS_THREAD
    registerTest(threadTest);
    registerTest(tlsTest);
    registerTest(mutexTest);
    registerTest(condvarTest);
#endif // PAL_HAS_THREAD

#if PAL_HAS_VIDEO
    // registerTest(videoTest);
    // registerTest(monitorTest);
    // registerTest(monitorModeTest);
    // registerTest(windowTest);
    // registerTest(iconTest);
    // registerTest(cursorTest);
    // registerTest(inputWindowTest);
    // registerTest(systemCursorTest);
    // registerTest(attachWindowTest);
    // registerTest(charEventTest);
    // registerTest(nativeIntegrationTest);
    // registerTest(nativeInstanceTest);
    // registerTest(customDecorationTest);
#endif // PAL_HAS_VIDEO

    // This test can run without video system so long as your have a valid
    // window
#if PAL_HAS_OPENGL && PAL_HAS_VIDEO
    registerTest(openglTest);
    registerTest(openglFBConfigTest);
    registerTest(openglContextTest);
    registerTest(openglMultiContextTest);
#endif // PAL_HAS_OPENGL

#if PAL_HAS_OPENGL && PAL_HAS_VIDEO && PAL_HAS_THREAD
    registerTest(multiThreadOpenGlTest);
#endif

#if PAL_HAS_GRAPHICS
    // registerTest(graphicsTest);
    registerTest(computeTest);
#endif // PAL_HAS_GRAPHICS

#if PAL_HAS_GRAPHICS && PAL_HAS_VIDEO
    // registerTest(clearColorTest);
    // registerTest(triangleTest);
    // registerTest(meshTest);
#endif //

    runTests();
    return 0;
}
