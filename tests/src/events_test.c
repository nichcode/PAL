
#include "tests.h"

#define USER_CLOSE_ID 44568
#define USER_PRINT_ID 4490000

void eventsTest() {

    palLog("");
    palLog("===========================================");
    palLog("Events Test");
    palLog("===========================================");
    palLog("");

    PalResult result;
    PalEventDriverCreateInfo createInfo;
    PalEventDriver* eventDriver;

    createInfo.allocator = nullptr;
    createInfo.callback = nullptr;
    createInfo.queue = nullptr; // use default
    createInfo.userData = nullptr; // not using callback mode
    result = palCreateEventDriver(&createInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        // this can made into a goto to decrease this result checks
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // Set polling dispatch mode for all event types
    palSetAllEventDispatchMode(eventDriver, PAL_DISPATCH_POLL);

    PalClock clock = palGetSystemClock();
    double lastTime = palGetTime(&clock);

    bool running = true;
    bool logged = false;

    while (running) {
        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_USER: {
                    switch (event.userID) {
                        case USER_CLOSE_ID: {
                            running = false;
                            break;
                        }

                        case USER_PRINT_ID: {
                            if (!logged) {
                                palLog("5 seconds have passed");
                                logged = true;
                            }
                            break;
                        }
                    }
                }
            }
        }

        double now = palGetTime(&clock);
        double timePassed = now - lastTime;

        // push a user event to print a message after 5 seconds
        if (timePassed > 5.0) {
            PalEvent event = {};
            event.userID = USER_PRINT_ID;
            event.type = PAL_EVENT_USER;
            palPushEvent(eventDriver, &event);
        }

        // push a user event to stop the while loop after 10 seconds
        if (timePassed > 10.0) {
            PalEvent event = {};
            event.userID = USER_CLOSE_ID;
            event.type = PAL_EVENT_USER;
            palPushEvent(eventDriver, &event);
        }
    }

    palDestroyEventDriver(eventDriver);
}