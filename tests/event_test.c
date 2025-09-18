
#include "pal/pal_event.h"
#include "tests.h"

#define MAX_ITERATIONS 10
#define MAX_EVENTS 100000

static Uint32 s_CallbackCounter = 0;
static Uint32 s_PollCounter = 0;

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

static void PAL_CALL onEvent(
    void* userData,
    const PalEvent* event)
{
    // discard event
    s_CallbackCounter++;
}

static inline void eventDispatchTest(bool poll)
{
    PalResult result;
    PalEventDriver* driver = nullptr;
    PalEventDriverCreateInfo createInfo = {0};
    createInfo.callback = onEvent;

    result = palCreateEventDriver(&createInfo, &driver);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to driver %s", palFormatResult(result));
        return;
    }

    // set dispatch mode
    PalDispatchMode mode = PAL_DISPATCH_CALLBACK;
    if (poll) {
        mode = PAL_DISPATCH_POLL;
    }

    for (Uint32 e = 0; e < PAL_EVENT_MAX; e++) {
        palSetEventDispatchMode(driver, e, mode);
    }

    Int32 counter = 0;
    while (counter < MAX_EVENTS) {
        // push all types of event up to max
        for (Int32 i = 0; i < MAX_EVENTS; i++) {
            PalEventType type = i % PAL_EVENT_MAX;
            PalEvent event = {0};
            event.type = type;
            palPushEvent(driver, &event);

            // poll events at the same thing since pal default queue has a fixed
            // size this limitation is not the same for callback mode. pal
            // default queue can handle up to 512 events pushed without polling
            // else some events will be lost
            while (palPollEvent(driver, &event)) {
                // discard event
                s_PollCounter++;
            }
            counter++;
        }
    }

    palDestroyEventDriver(driver);
}

bool eventTest()
{

    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Event Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    MyTimer timer;
    timer.frequency = palGetPerformanceFrequency();
    timer.startTime = palGetPerformanceCounter();

    // callback mode
    // get start time
    double startTime = getTime(&timer);

    for (Int32 i = 0; i < MAX_ITERATIONS; i++) {
        eventDispatchTest(false); // callback mode first
    }

    // get end time
    double endTime = getTime(&timer);
    double averageTime = (endTime - startTime) / MAX_ITERATIONS;

    palLog(
        nullptr,
        "%.6f seconds per iteration for %d events using callback mode (average "
        "over %d iterations)",
        averageTime,
        MAX_EVENTS,
        MAX_ITERATIONS);

    // poll mode
    // get start time
    startTime = getTime(&timer);

    for (Int32 i = 0; i < MAX_ITERATIONS; i++) {
        eventDispatchTest(true); // poll mode first
    }

    // get end time
    endTime = getTime(&timer);
    averageTime = (endTime - startTime) / MAX_ITERATIONS;

    palLog(
        nullptr,
        "%.6f seconds per iteration for %d events using poll mode (average "
        "over %d iterations)",
        averageTime,
        MAX_EVENTS,
        MAX_ITERATIONS);

    // how many times the callback was called
    palLog(nullptr, "Callback counter: %d", s_CallbackCounter);

    // how many times poll event was called
    palLog(nullptr, "Poll counter: %d", s_PollCounter);

    return true;
}