
#ifndef _TESTS_H
#define _TESTS_H

#include "pal/pal_core.h"

typedef bool (*TestFn)();

void registerTest(const char* name, TestFn func);
void runTests();

// Tests

bool loggerTest();
bool systemTest();

#endif // _TESTS_H