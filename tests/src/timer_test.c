
#include "tests.h"

// seconds
#define LOG_1_TIME 3.0
#define LOG_2_TIME 6.0
#define LOG_3_TIME 9.0

void timerTest() {

    palLogInfo(PAL_NULL, "");
    palLogInfo(PAL_NULL, "===========================================");
    palLogInfo(PAL_NULL, "Timer Test Seconds");
    palLogInfo(PAL_NULL, "===========================================");
    palLogInfo(PAL_NULL, "");

    // get the system timer.
    // Or create your own timer with your own frequency and startTime
    PalTimer sysTimer = palGetSysTimer();

    double startTime = palGetTime(&sysTimer);
    bool logged1, logged2 = PAL_FALSE;

    bool running = PAL_TRUE;
    while (running) {
        double currentTime = palGetTime(&sysTimer);
        double timePassed = currentTime - startTime;

        if (!logged1 && (timePassed > LOG_1_TIME)) {
            palLogInfo(PAL_NULL, "3 seconds have passed");
            logged1 = PAL_TRUE;
        }

        if (!logged2 && (timePassed > LOG_2_TIME)) {
            palLogInfo(PAL_NULL, "6 seconds have passed");
            logged2 = PAL_TRUE;
        }

        // break out of the loop
        if (logged1 && logged2) {
             running = PAL_FALSE;
        }
    } 
}