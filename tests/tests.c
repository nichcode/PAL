
#include "tests.h"

#define MAX_TESTS 64 // will change

typedef struct {
    TestFn func;
    const char* name;
} TestEntry;

static uint32_t s_Count = 0;
static TestEntry s_Test[MAX_TESTS];
static const char* s_FailedString = "FAILED";
static const char* s_PassedString = "PASSED";

void registerTest(
    TestFn func,
    const char* name)
{
    TestEntry entry = {func, name};
    s_Test[s_Count++] = entry;
}

void runTests()
{
    PalBool status = PAL_FALSE;
    const char* statusString = nullptr;
    for (int32_t i = 0; i < s_Count; i++) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, s_Test[i].name);
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");

        status = s_Test[i].func();
        if (status) {
            statusString = s_PassedString;
        } else {
            statusString = s_FailedString;
        }

        palLog(nullptr, statusString);
    }
}
