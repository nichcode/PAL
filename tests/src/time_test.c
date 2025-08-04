
#include "tests.h"
#include "pal/pal_core.h"

void timeTest() {

    palLog("");
    palLog("===========================================");
    palLog("Time Test");
    palLog("===========================================");
    palLog("");

    // get the system timer
    // the start Time will be the current time since the OS started
    PalTimer systemTimer = palGetSystemTimer();

    // get the current time for both systems and cache
    double lastTime = palGetTime(&systemTimer);

    // log the timer
    palLog("System Timer:");
    palLog( "Frequency - %llu", systemTimer.frequency);
    palLog( "Start Time - %llu", systemTimer.startTime);

    double totalTime = 0.0;
    int frameCount = 0;

    // run the loop for 5 seconds
    while (totalTime < 5.0) {
        // we get the current time using the system timer
        totalTime = palGetTime(&systemTimer);
        double timePassed = totalTime - lastTime;

        // if you need to replay an action at a specific time range
        // then you need to reset the timer's start time when it reaches the point you want
        // example systemTimer.startTime = palGetTime(&systemTimer).

        palLog("Frame %i, Total Time - %f seconds", frameCount++, totalTime);
    }

    palLog("Loop finished after %f seconds and %i frames", totalTime, frameCount);
}