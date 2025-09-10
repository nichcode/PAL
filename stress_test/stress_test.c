
#include "stress_test.h"

#define MAX_STRESS_TESTS 32 // will change

typedef struct {
    StressTestFn func;
    const char* name;
    Int32 iterations;
} StressTestEntry;

typedef struct {
    StressTestEntry tests[MAX_STRESS_TESTS];
    Int32 count;
} StressTests;

typedef struct {
    Uint64 frequency;
    Uint64 startTime;
} MyTimer;

static StressTests s_Test;

static inline double getTime(
    MyTimer* timer) {

    Uint64 now = palGetPerformanceCounter();
    return (double)(now - timer->startTime) / (double)timer->frequency;
}

void registerStressTest(
    const char* name, 
    StressTestFn func,
    Int32 iterations) {
    
    StressTestEntry *entry = &s_Test.tests[s_Test.count++];
    entry->func = func;
    entry->name = name;
    entry->iterations = iterations;
}

void runStressTests() {

    MyTimer timer;
    timer.frequency = palGetPerformanceFrequency();
    timer.startTime = palGetPerformanceCounter();

    for (Int32 i = 0; i < s_Test.count; i++) {
        StressTestEntry* test = &s_Test.tests[i];

        // get start time
        double startTime = getTime(&timer);
        for (Int32 i = 0; i < test->iterations; i++) {
            test->func();
        }

        // get end time
        double endTime = getTime(&timer);
        double averageTime = (endTime - startTime) / test->iterations;

        palLog(
            nullptr, 
            "%s: %.6f seconds per iteration (average over %d)", 
            test->name, 
            averageTime, 
            test->iterations
        );
    }
}