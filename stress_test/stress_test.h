
#ifndef _STRESS_TESTS_H
#define _STRESS_TESTS_H

#include "pal/pal_core.h"

typedef void (*StressTestFn)();

void registerStressTest(
    const char* name, 
    StressTestFn func,
    Int32 iterations);

void runStressTests();

#endif // _STRESS_TESTS_H