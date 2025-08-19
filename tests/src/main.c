
#include "pal/pal_core.h"
#include "tests.h"

// config reflection
#include "pal/pal_config.h"

int main(int argc, char** argv) {

    const char* versionString = palGetVersionString();
    palLog("PAL version - %s", versionString);

    // registerTest(allocatorTest);
    //registerTest(timeTest);
    //registerTest(eventsTest);

#if PAL_HAS_VIDEO
    // registerTest(videoTest);
    // registerTest(displayTest);
    // registerTest(displayModeTest);
    // registerTest(windowTest);
    // registerTest(multiWindowTest);
#endif // PAL_HAS_VIDEO

#if PAL_HAS_INPUT
    // registerTest(inputTest);
    // registerTest(keyboardTest);
    // registerTest(mouseTest);
    // registerTest(keyboardAndMouseTest);
    //registerTest(gamepadTest);
#endif // PAL_HAS_VIDEO

#if PAL_HAS_OPENGL && PAL_HAS_VIDEO
    // registerTest(inputTest);
    // registerTest(keyboardTest);
    // registerTest(mouseTest);
    // registerTest(keyboardAndMouseTest);
    // registerTest(gamepadTest);
    // registerTest(openglTest);
    // registerTest(openglPixelTest);
    registerTest(openglContextTest);
#endif // PAL_HAS_OPENGL

    runTests();
    return 0;
}