
#include "tests.h"

#define MAX_TESTS 32 // will change

typedef struct {
    TestFn func;
    const char* name;
} TestEntry;

typedef struct {
    TestEntry tests[MAX_TESTS];
    Int32 count;
} Tests;

static Tests s_Test;
static const char* s_FailedString = "FAILED";
static const char* s_PassedString = "PASSED";

void registerTest(
    const char* name, 
    TestFn func) {

    TestEntry *entry = &s_Test.tests[s_Test.count++];
    entry->func = func;
    entry->name = name;
}

void runTests() {

    bool status = false;
    const char* statusString = nullptr;
    for (Int32 i = 0; i < s_Test.count; i++) {
        status = s_Test.tests[i].func();

        if (status) {
            statusString = s_PassedString;
        } else {
            statusString = s_FailedString;
        }

        palLog(nullptr, "%s: %s", s_Test.tests[i].name, statusString);
    }
}