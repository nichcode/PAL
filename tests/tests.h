
#ifndef _TESTS_H
#define _TESTS_H

#include "pal/pal_core.h"

typedef bool (*TestFn)();

void registerTest(
    const char* name, 
    TestFn func);

void runTests();

// core tests
bool loggerTest();
bool timeTest();

// system tests
bool systemTest();

// system tests
bool threadTest();
bool tlsTest();
bool mutexTest();
bool condvarTest();

// video test
bool videoTest();
bool displayTest();
bool displayModeTest();
bool windowTest();
bool windowIconTest();

// input test. // since we want to be self contained, we will use video system
bool inputTest();

#endif // _TESTS_H