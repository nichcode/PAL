
#include "bench.h"

typedef struct {
    Uint64 frequency;
    Uint64 startTime;
} MyTimer;

// get the time in seconds
static inline double getTime(MyTimer* timer)
{

    Uint64 now = palGetPerformanceCounter();
    return (double)(now - timer->startTime) / (double)timer->frequency;
}

void runBench(BenchFn func, const char* name, Int32 iterations)
{

    MyTimer timer;
    timer.frequency = palGetPerformanceFrequency();
    timer.startTime = palGetPerformanceCounter();
    double startTime, endTime, averageTime;

    startTime = getTime(&timer);
    for (Int32 i = 0; i < iterations; i++) {
        func();
    }

    // end time
    endTime     = getTime(&timer);
    averageTime = (endTime - startTime) / iterations;

    palLog(nullptr, "%s: %.6f seconds per iteration (average over %d iterations)", name,
           averageTime, iterations);
}