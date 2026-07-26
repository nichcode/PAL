
#include "pal2/pal_event.h"
#include "tests.h"

#define MAX_ITERATIONS 10
#define MAX_EVENTS 100000

static uint32_t s_CallbackCounter = 0;
static uint32_t s_PollCounter = 0;

typedef struct {
    uint64_t frequency;
    uint64_t startTime;
} MyTimer;

// get the time in seconds
static inline double getTime(MyTimer* timer)
{
    uint64_t now = palGetPerformanceCounter();
    return (double)(now - timer->startTime) / (double)timer->frequency;
}

static void PAL_CALL onEvent(
    void* userData,
    const PalEvent* event)
{
    // discard event
    s_CallbackCounter++;
}

static inline PalBool eventDispatchTest(PalBool poll)
{
    PalResult result;
    PalEventDriver* driver = nullptr;
    PalEventDriverCreateInfo createInfo = {0};
    createInfo.callback = onEvent;

    result = palCreateEventDriver(&createInfo, &driver);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create event driver");
        return PAL_FALSE;
    }

    // set dispatch mode
    PalDispatchMode mode = PAL_DISPATCH_MODE_CALLBACK;
    if (poll) {
        mode = PAL_DISPATCH_MODE_POLL;
    }

    for (uint32_t e = 0; e < PAL_EVENT_TYPE_COUNT; e++) {
        palSetEventDispatchMode(driver, e, mode);
    }

    int32_t counter = 0;
    while (counter < MAX_EVENTS) {
        // push all types of event up to max
        for (int32_t i = 0; i < MAX_EVENTS; i++) {
            PalEventType type = i % PAL_EVENT_TYPE_COUNT;
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
    return PAL_TRUE;
}

PalBool eventTest()
{
    MyTimer timer;
    timer.frequency = palGetPerformanceFrequency();
    timer.startTime = palGetPerformanceCounter();

    // callback mode
    // get start time
    double startTime = getTime(&timer);

    for (int32_t i = 0; i < MAX_ITERATIONS; i++) {
        PalBool success = eventDispatchTest(PAL_FALSE); // callback mode
        if (success == PAL_FALSE) {
            return PAL_FALSE;
        }
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

    for (int32_t i = 0; i < MAX_ITERATIONS; i++) {
        PalBool success = eventDispatchTest(PAL_TRUE); // poll mode
        if (success == PAL_FALSE) {
            return PAL_FALSE;
        }
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

    return PAL_TRUE;
}
