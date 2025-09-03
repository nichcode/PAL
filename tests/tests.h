
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

#endif // _TESTS_H