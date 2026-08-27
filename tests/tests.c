
#include "tests.h"
#include <string.h>

#define MAX_TESTS 64

typedef struct {
    TestFn func;
    const char* name;
    PalBool status;
} TestEntry;

typedef struct {
    uint32_t count;
    TestEntry tests[MAX_TESTS];
} TestManager;

static TestManager s_TestManager = {0};

void registerTest(
    TestFn func,
    const char* name)
{
    TestEntry* entry = &s_TestManager.tests[s_TestManager.count++];
    entry->func = func;
    entry->name = name;
    entry->status = PAL_FALSE;
}

void runTests()
{
    // clang-format off
    const char* resultStrings[2] = {
        "FAILED",
        "PASSED"
    };
    // clang-format on

    palLog(nullptr, "");
    for (uint32_t i = 0; i < s_TestManager.count; i++) {
        TestEntry* entry = &s_TestManager.tests[i];

        palLog(nullptr, "===========================================");
        palLog(nullptr, entry->name);
        palLog(nullptr, "===========================================");

        entry->status = entry->func();
        palLog(nullptr, "");
    }

    palLog(nullptr, "===========================================");
    palLog(nullptr, "Tests Status:");
    palLog(nullptr, "===========================================");

    for (uint32_t i = 0; i < s_TestManager.count; i++) {
        TestEntry* entry = &s_TestManager.tests[i];
        palLog(nullptr, " %s: %s", entry->name, resultStrings[entry->status]);
    }

    palLog(nullptr, "");
}