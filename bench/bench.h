
#ifndef _BENCH_H
#define _BENCH_H

#include "pal/pal_core.h"

#define MAX_ITERATIONS 1000

typedef void (*BenchFn)();

void runBench(
    BenchFn func,
    const char* name,
    Int32 iterations);

void windowBench();
void windowBenchPlatform();

#endif // _BENCH_H