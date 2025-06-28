
#ifndef _TESTS_H
#define _TESTS_H

#include "pal/pal.h"

typedef bool (*TestFn)();

void registerTest(const char* name, TestFn testFn);
void runTests();

// Tests
bool coreTest();
bool displayTest();
bool windowTest();

#endif // _TESTS_H