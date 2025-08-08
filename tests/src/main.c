
#include "pal/pal_core.h"
#include "tests.h"

// config reflection
#include "pal/pal_config.h"

int main(int argc, char** argv) {

    const char* versionString = palGetVersionString();
    palLog("PAL v(%s)", versionString);

    // registerTest(allocatorTest);
    //registerTest(timeTest);
    //registerTest(eventsTest);

#if PAL_HAS_VIDEO
    //registerTest(videoTest);
    //registerTest(displayTest);
    //registerTest(displayModeTest);
    //registerTest(windowTest);
    registerTest(multiWindowTest);
#endif // PAL_HAS_VIDEO

    runTests();
    return 0;
}