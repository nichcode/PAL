
#include "tests.h"
#include "pal/pal_config.h" // for systems reflection

int main(int argc, char**) {

    PalVersion version = palGetVersion();

#if PAL_HAS_SYSTEM
    registerTest("System Test", systemTest);
#endif // PAL_HAS_SYSTEM

    runTests();
    return 0;
}