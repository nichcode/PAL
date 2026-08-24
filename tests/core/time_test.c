
#include "tests.h"

PalBool timeTest()
{
    // create and set the frequency and start time for time related calculations
    MyTimer timer;
    timer.frequency = palGetPerformanceFrequency();
    timer.startTime = palGetPerformanceCounter();

    // get the start time normalize by timer.startTime
    double lastTime = getTime(&timer);
    double totalTime = 0.0;
    int32_t frameCount = 0;

    // run the loop for 5 seconds
    while (totalTime < 5.0) {
        double now = getTime(&timer);
        totalTime = now - lastTime;
        palLog(nullptr, "Frame %d, Total Time %f seconds", frameCount, totalTime);
        frameCount++;
    }

    palLog(nullptr, "Loop finished after %f seconds and %d frames", totalTime, frameCount);
    return PAL_TRUE;
}
