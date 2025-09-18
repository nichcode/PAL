
#include "pal/pal_event.h"
#include "tests.h"

#define USER_CLOSE_EVENT_ID 44568
#define USER_PRINT_EVENT_ID 4490000

// a simple timer object to hold frequency and start time
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

bool userEventTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "User Event Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalResult result;
    PalEventDriver* eventDriver = nullptr;
    PalEventDriverCreateInfo createInfo;
    bool running, logged = false;

    // fill the event driver create info
    createInfo.allocator = nullptr; // default allocator
    createInfo.callback = nullptr;  // for callback dispatch
    createInfo.queue = nullptr;     // default queue
    createInfo.userData = nullptr;  // nullptr

    // create the event driver
    result = palCreateEventDriver(&createInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(
            nullptr,
            "Failed to create event driver %s",
            palFormatResult(result));
        return false;
    }

    palSetEventDispatchMode(eventDriver, PAL_EVENT_USER, PAL_DISPATCH_POLL);

    // create and set the frequency and start time for time related calculations
    MyTimer timer;
    timer.frequency = palGetPerformanceFrequency();
    timer.startTime = palGetPerformanceCounter();

    // get the start time normalize by timer.startTime
    double lastTime = getTime(&timer);

    running = true;
    while (running) {
        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_USER: {
                    // a user event. USe another switch to check which event
                    // using the user Id
                    switch (event.userId) {
                        case USER_PRINT_EVENT_ID: {
                            // log a message
                            if (!logged) {
                                palLog(nullptr, "5 seconds have passed");
                                logged = true;
                            }
                            break;
                        }

                        case USER_CLOSE_EVENT_ID: {
                            palLog(nullptr, "10 seconds have passed");
                            running = false;
                            break;
                        }
                    }
                }
            }
        }

        // update time
        double now = getTime(&timer);
        double timePassed = now - lastTime;

        // push a user event to print a message after 5 seconds
        if (timePassed > 5.0) {
            PalEvent event = {0};
            event.userId = USER_PRINT_EVENT_ID;
            event.type = PAL_EVENT_USER;
            palPushEvent(eventDriver, &event);
        }

        // push a user event to close the application after 10 seconds
        if (timePassed > 10.0) {
            PalEvent event = {0};
            event.userId = USER_CLOSE_EVENT_ID;
            event.type = PAL_EVENT_USER;
            palPushEvent(eventDriver, &event);
        }
    }

    // destroy the event driver
    palDestroyEventDriver(eventDriver);

    return true;
}