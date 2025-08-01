
#include "pal/pal_core.h"
#include "tests.h"

// config reflection
#include "pal/pal_config.h"

int main(int argc, char** argv) {

    const char* versionString = palGetVersionString();
    palLog("PAL v(%s)", versionString);

    // registerTest("Allocator Test", allocatorTest);
    // registerTest("Time Test", timeTest);

#if PAL_HAS_VIDEO
    registerTest("Video Test", videoTest);
#endif // PAL_HAS_VIDEO

    runTests();
    return 0;
}