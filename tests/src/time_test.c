
#include "tests.h"
#include "pal/pal_core.h"

void timeTest() {

    palLog("");
    palLog("===========================================");
    palLog("Time Test");
    palLog("===========================================");
    palLog("");

    // get the system clock
    PalClock clock = palGetSystemClock();

    double lastTime = palGetTime(&clock);
    double resetTime = lastTime;

    double totalTime = 0.0;
    int frameCount = 0;

    bool running = true;
    int actionCount = 0;

    // run the loop for 5 seconds
    while (totalTime < 5.0) {
        double now = palGetTime(&clock);
        totalTime = now - lastTime;
        palLog("Frame %i, Total Time - %f seconds", frameCount++, totalTime);
    }

    palLog("Loop finished after %f seconds and %i frames", totalTime, frameCount);

    // now repeat an action every 5 seconds for five times and quit.
    //resetTime = palGetTime(&clock); Do this to reset the time.
    while (running) {
        double now = palGetTime(&clock);
        double timePassed = now - resetTime;

        // this will be called as soon as the five seconds loop finishes
        // since resetTime == lastTime. if you dont want this, reset resetTime to the current time.
        if (timePassed > 5.0) {
            // over 5 seconds
            actionCount++;
            palLog("Action Count: %d", actionCount);
            if (actionCount == 5) {
                running = false;
            }

            resetTime = palGetTime(&clock);
        }

        
    }
}