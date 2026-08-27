
#include "tests.h"

void registerCoreTests();
void registerEventTests();

// clang-format off
int main(int argc, char** argv)
{
    // clang-format on
    palLog(nullptr, "%s: %s", "PAL Version", palGetVersionString());

    registerCoreTests();
    registerEventTests();
    runTests();

    return 0;
}