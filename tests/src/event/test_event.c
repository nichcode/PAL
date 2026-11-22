#include "pal2/event/driver.h"
#include "pal2/core/log.h"

typedef struct EventContext
{
    uint32_t counter;
} EventContext;

static void PAL_CALL onEvent(
    void* userData,
    const PalEvent* event)
{
    (void)event;

    EventContext* context = userData;
    context->counter++;
}

static PalBool createDefaultEventQueue(
    EventContext* context, 
    PalEventDriver** eventDriver)
{
    PalEventDriverCreateInfo createInfo = {0};
    createInfo.allocator = nullptr;
    createInfo.callback = onEvent;
    createInfo.queue = nullptr;
    createInfo.userData = context;

    PalEventDriver* driver = nullptr;
    PalResult result = palCreateEventDriver(&createInfo, &driver);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create event driver");
        return PAL_FALSE;
    }

    *eventDriver = driver;
    return PAL_TRUE;
}

PalBool pushCallback(void)
{
    PalEventDriver* eventDriver = nullptr;
    EventContext context = {0};

    if (!createDefaultEventQueue(&context, &eventDriver)) {
        return PAL_FALSE;
    }

    PalEvent ignoredEvent = {0};
    ignoredEvent.type = PAL_EVENT_TYPE_USER;
    palPushEvent(eventDriver, &ignoredEvent);
    if (context.counter != 0) {
        palDestroyEventDriver(eventDriver);
        return PAL_FALSE;
    }

    for (uint32_t i = 0; i < PAL_EVENT_TYPE_COUNT; i++) {
        palSetEventDispatchMode(eventDriver, i, PAL_DISPATCH_MODE_CALLBACK);
    }

    for (uint32_t i = 0; i < PAL_DEFAULT_QUEUE_EVENT_COUNT; i++) {
        PalEvent event = {0};
        event.type = i % PAL_EVENT_TYPE_COUNT;
        event.data = 1;
        event.data2 = 2;
        event.userId = 0;

        palPushEvent(eventDriver, &event);
    }

    palDestroyEventDriver(eventDriver);
    return context.counter == PAL_DEFAULT_QUEUE_EVENT_COUNT;
}

PalBool pushPoll(void)
{
    PalEventDriver* eventDriver = nullptr;
    EventContext context = {0};

    if (!createDefaultEventQueue(&context, &eventDriver)) {
        return PAL_FALSE;
    }

    for (uint32_t i = 0; i < PAL_EVENT_TYPE_COUNT; i++) {
        palSetEventDispatchMode(eventDriver, i, PAL_DISPATCH_MODE_POLL);
    }

    for (uint32_t i = 0; i < PAL_DEFAULT_QUEUE_EVENT_COUNT + 1; i++) {
        PalEvent event = {0};
        event.type = i % PAL_EVENT_TYPE_COUNT;
        event.data = i;
        event.data2 = 2;
        event.userId = 0;

        palPushEvent(eventDriver, &event);
    }

    PalEvent event = {0};
    uint64_t expectedFirstData = 1;
    while (palPollEvent(eventDriver, &event)) {
        if (event.data != expectedFirstData++) {
            palDestroyEventDriver(eventDriver);
            return PAL_FALSE;
        }
        context.counter++;
    }

    palDestroyEventDriver(eventDriver);
    return context.counter == PAL_DEFAULT_QUEUE_EVENT_COUNT;
}

int main(void)
{
    palLog(nullptr, "Running event test...");

    if (!pushCallback()) {
        return -1;
    }

    if (!pushPoll()) {
        return -1;
    }

    return 0;
}