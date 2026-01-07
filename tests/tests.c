
#include "tests.h"

#define MAX_TESTS 64 // will change

static Uint32 s_Count = 0;
static TestFn s_Test[MAX_TESTS];
static const char* s_FailedString = "FAILED";
static const char* s_PassedString = "PASSED";

void registerTest(TestFn func)
{
    s_Test[s_Count++] = func;
}

void runTests()
{
    bool status = false;
    const char* statusString = nullptr;
    for (Int32 i = 0; i < s_Count; i++) {
        status = s_Test[i]();

        if (status) {
            statusString = s_PassedString;
        } else {
            statusString = s_FailedString;
        }

        palLog(nullptr, statusString);
    }
}