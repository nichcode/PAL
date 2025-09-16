
#ifndef _BENCH_H
#define _BENCH_H

#include "pal/pal_core.h"

typedef void (*BenchFn)();

void registerBench(
    const char* name, 
    BenchFn func,
    Int32 iterations);

void runBenchs();

#endif // _BENCH_H