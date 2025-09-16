
#include "bench.h"

#define MAX_BENCH 32 // will change

typedef struct {
    BenchFn func;
    const char* name;
    Int32 iterations;
} BenchEntry;

typedef struct {
    BenchEntry benchs[MAX_BENCH];
    Int32 count;
} Benchs;

typedef struct {
    Uint64 frequency;
    Uint64 startTime;
} MyTimer;

static Benchs s_Benchs;

// get the time in seconds
static inline double getTime(
    MyTimer* timer) {

    Uint64 now = palGetPerformanceCounter();
    return (double)(now - timer->startTime) / (double)timer->frequency;
}

void registerBench(
    const char* name, 
    BenchFn func,
    Int32 iterations) {

    BenchEntry *entry = &s_Benchs.benchs[s_Benchs.count++];
    entry->func = func;
    entry->name = name;
    entry->iterations = iterations;
}

void runBenchs() {

    MyTimer timer;
    timer.frequency = palGetPerformanceFrequency();
    timer.startTime = palGetPerformanceCounter();
    double startTime, endTime, averageTime;

    for (Int32 i = 0; i < s_Benchs.count; i++) {
        BenchEntry *entry = &s_Benchs.benchs[i];
        
        // start time
        startTime = getTime(&timer);
        for (Int32 i = 0; i < entry->iterations; i++) { 
            entry->func();
        }

        // end time
        endTime = getTime(&timer);
        averageTime = (endTime - startTime) / entry->iterations;

        palLog(
            nullptr, 
            "%s: %.6f seconds per iteration (average over %d iterations)", 
            entry->name,
            averageTime, 
            entry->iterations
        );
    }
}