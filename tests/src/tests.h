
#ifndef _TESTS_H
#define _TESTS_H

#include "pal/pal_defines.h"

#define MAX_TESTS 32

typedef PalResult (*TestFn)();

typedef struct TestEntry
{
    const char* name;
    TestFn testFn;
} TestEntry;

typedef struct Tests
{
    int count;
    struct TestEntry tests[MAX_TESTS];
} Tests;

static Tests s_Test;

static inline void registerTest(const char* name, TestFn fn)
{
    TestEntry entry;
    entry.name = name;
    entry.testFn = fn;
    s_Test.tests[s_Test.count++] = entry;
}

static inline void runTests()
{
    for (int i = 0; i < s_Test.count; i++) {
        s_Test.tests[i].testFn();
    }
}

// Tests
PalResult loggingTest();
PalResult displayTest();
PalResult displayModeTest();
PalResult windowTest();
PalResult eventTest();

#endif // _TESTS_H