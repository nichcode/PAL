
#include "pal/pal_core.h"
#include "tests.h"

int main(int argc, char** argv) {

    const char* versionString = palGetVersionString();
    palLog("PAL v(%s)", versionString);

    registerTest("Allocator Test", allocatorTest);

    runTests();
    return 0;
}