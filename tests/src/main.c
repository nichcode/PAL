
#include "tests.h"
#include "pal/pal_core.h"

int main(int argc, char** argv) 
{
    PalVersion version;
    palGetVerion(&version);
    palLogConsoleInfo("PAL v(%i.%i.%i)", version.major, version.minor, version.patch);

    // registerTest("Logging Test", loggingTest);
    // registerTest("Display Test", displayTest);
    // registerTest("Display Mode Test", displayModeTest);
    // registerTest("Window Test", windowTest);
    //registerTest("Event Test", eventTest);
    registerTest("Window Event Test", windowEventTest);

    runTests();
    return 0;
}