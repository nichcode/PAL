
#include "tests.h"

#define MAX_TESTS 64 // will change

typedef struct {
    TestFn func;
    const char* name;
} TestEntry;

static Uint32 s_Count = 0;
static TestEntry s_Test[MAX_TESTS];
static const char* s_FailedString = "FAILED";
static const char* s_PassedString = "PASSED";

void registerTest(TestFn func, const char* name)
{
    TestEntry entry = { func, name };
    s_Test[s_Count++] = entry;
}

void runTests()
{
    bool status = false;
    const char* statusString = nullptr;
    for (Int32 i = 0; i < s_Count; i++) {
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
