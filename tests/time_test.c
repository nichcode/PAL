
#include "tests.h"

typedef struct {
    Uint64 frequency;
    Uint64 startTime;
} MyTimer;

static inline double getTime(MyTimer* timer) {

    Uint64 now = palGetPerformanceCounter();
    return (double)(now - timer->startTime) / (double)timer->frequency;
}

bool timeTest() {

    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Time Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    MyTimer timer;
    timer.frequency = palGetPerformanceFrequency();
    timer.startTime = palGetPerformanceCounter();

    double lastTime = getTime(&timer);

    double totalTime = 0.0;
    Int32 frameCount = 0;

    // run the loop for 5 seconds
    while (totalTime < 5.0) {
        double now = getTime(&timer);
        totalTime = now - lastTime;
        palLog(nullptr, "Frame %d, Total Time %f seconds", frameCount++, totalTime);
    }

    palLog(nullptr, "Loop finished after %f seconds and %d frames", totalTime, frameCount);
    return true;
}