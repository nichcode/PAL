
#include "tests.h"
#include "pal/pal_event.h"

#define QUIT_TIME 6

void eventTest() {

    palLogInfo(PAL_NULL, "");
    palLogInfo(PAL_NULL, "===========================================");
    palLogInfo(PAL_NULL, "Event Test");
    palLogInfo(PAL_NULL, "===========================================");
    palLogInfo(PAL_NULL, "");

    PalResult result;
    PalEventDriver eventDriver;

    PalEventDriverConfig eventDriverConfig;
    eventDriverConfig.allocator = PAL_NULL;
    eventDriverConfig.callback = PAL_NULL; // for callback mode
    eventDriverConfig.queue = PAL_NULL; // for polling mode
    eventDriverConfig.userData = PAL_NULL; // for callback mode

    result = palCreateEventDriver(&eventDriverConfig, &eventDriver);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return;
    }

    // enable or diable all events with with a mode
    palEnableAllEvents(eventDriver, PAL_EVENT_MODE_POLL);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return;
    }

    PalTimer sysTimer = palGetSysTimer();
    double startTime = palGetTime(&sysTimer);
    bool running = PAL_TRUE;

    while (running) {
        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            if (event.type == PAL_EVENT_USER) {
                running = PAL_FALSE;
                break;
            }
        }

        // check for 3 seconds and push a user event
        double currentTime = palGetTime(&sysTimer);
        double timePassed = currentTime - startTime;

        if (running && (timePassed > QUIT_TIME)) {
            palLogInfo(PAL_NULL, "6 seconds have passed");
            PalEvent event = {};
            event.sourceID = 120; // can be a window id or anything;
            event.type = PAL_EVENT_USER;

            // push the event. this either calls the callback or push to the queue
            // depending on the event mode
            palPushEvent(eventDriver, &event);
            if (result != PAL_SUCCESS) {
                const char* resultString = palResultToString(result);
                palLogError(PAL_NULL, resultString);
                return;
            }
        }
    }

    palDestroyEventDriver(eventDriver);
}