
#ifndef _BENCH_H
#define _BENCH_H

#include "pal/pal_core.h"

typedef void (*BenchFn)();

void runBench(
    BenchFn func, 
    const char* name, 
    Int32 iterations);

void windowBench();

#endif // _BENCH_H