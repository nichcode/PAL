
#ifndef TESTS_H
#define TESTS_H

#include "PAL/PAL.h"

typedef PAL_BOOL (*TestFun)();

void addTest(const char* name, TestFun fun);
void runTests();

// tests headers

PAL_BOOL allocatorTest();
PAL_BOOL windowTest();

#endif // TESTS_H