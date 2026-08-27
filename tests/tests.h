
#ifndef _TESTS_H
#define _TESTS_H

#include "pal2/pal_core.h"

typedef PalBool (*TestFn)();

void registerTest(
    TestFn func,
    const char* name);

void runTests();

#endif // _TESTS_H