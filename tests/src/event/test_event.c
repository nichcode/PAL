
#include "pal2/pal_event.h"
#include <stdbool.h>

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

static bool createDefaultEventQueue(
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
        return false;
    }

    *eventDriver = driver;

    return true;
}

bool pushCallback()
{
    PalEventDriver* eventDriver = nullptr;
    EventContext context = {0};
    context.counter = 0;

    bool ret = createDefaultEventQueue(&context, &eventDriver);
    if (!ret) {
        return false;
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

    if (context.counter != PAL_DEFAULT_QUEUE_EVENT_COUNT) {
        return false;
    }

    return true;
}

bool pushPoll()
{
    PalEventDriver* eventDriver = nullptr;
    EventContext context = {0};
    context.counter = 0;

    bool ret = createDefaultEventQueue(&context, &eventDriver);
    if (!ret) {
        return false;
    }

    for (uint32_t i = 0; i < PAL_EVENT_TYPE_COUNT; i++) {
        palSetEventDispatchMode(eventDriver, i, PAL_DISPATCH_MODE_POLL);
    }

    for (uint32_t i = 0; i < PAL_DEFAULT_QUEUE_EVENT_COUNT; i++) {
        PalEvent event = {0};
        event.type = i % PAL_EVENT_TYPE_COUNT;
        event.data = 1;
        event.data2 = 2;
        event.userId = 0;

        palPushEvent(eventDriver, &event);
    }

    PalEvent event = {0};
    while (palPollEvent(eventDriver, &event)) {
        context.counter++;
    }

    if (context.counter != PAL_DEFAULT_QUEUE_EVENT_COUNT) {
        return false;
    }

    return true;
}

int main(void)
{
    palLog(nullptr, "Running event test...");

    bool ret = pushCallback();
    if (!ret) {
        return false;
    }

    ret = pushPoll();
    if (!ret) {
        return false;
    }

    return 0;
}