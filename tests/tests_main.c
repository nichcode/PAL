
#include "tests.h"

void registerCoreTests();

// clang-format off
int main(int argc, char** argv)
{
    // clang-format on
    palLog(nullptr, "%s: %s", "PAL Version", palGetVersionString());

    registerCoreTests();
    runTests();

    return 0;
}