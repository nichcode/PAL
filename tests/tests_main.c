
#include "tests.h"

void registerCoreTests();
void registerEventTests();
void registerVideoTests();

// clang-format off
int main(int argc, char** argv)
{
    // clang-format on
    palLog(nullptr, "%s: %s", "PAL Version", palGetVersionString());

    registerCoreTests();
    registerEventTests();
    registerVideoTests();
    runTests();

    return 0;
}