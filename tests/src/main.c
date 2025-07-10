
#include "tests.h"
#include "pal/pal_core.h"

int main(int argc, char** argv) 
{
    PalVersion version;
    palGetVerion(&version);
    palLogConsoleInfo("PAL v(%i.%i.%i)", version.major, version.minor, version.patch);

    registerTest("Logging Test", loggingTest);
    registerTest("Display Test", displayTest);

    runTests();
    return 0;
}