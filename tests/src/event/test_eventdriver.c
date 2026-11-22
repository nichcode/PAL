#include "pal2/event/driver.h"
#include "pal2/core/log.h"
#include <string.h>

#define MAX_EVENTS 64

typedef struct QueueData
{
    uint32_t head;
    uint32_t tail;
    PalEvent data[MAX_EVENTS];
} QueueData;

static void PAL_CALL eventPush(
    void* userData,
    const PalEvent* event)
{
    QueueData* data = userData;
    data->data[data->tail++ % MAX_EVENTS] = *event;
}

static PalBool PAL_CALL eventPoll(
    void* userData,
    PalEvent* outEvent)
{
    QueueData* data = userData;
    if (data->head == data->tail) {
        return PAL_FALSE;
    }

    *outEvent = data->data[data->head++ % MAX_EVENTS];
    return PAL_TRUE;
}

PalBool defaultQueue(void)
{
    PalEventDriverCreateInfo createInfo = {0};

    PalEventDriver* eventDriver = nullptr;
    PalResult result = palCreateEventDriver(&createInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create event driver");
        return PAL_FALSE;
    }

    palDestroyEventDriver(eventDriver);
    return PAL_TRUE;
}

PalBool customQueue(void)
{
    PalEventQueue* queue = palAllocate(nullptr, sizeof(PalEventQueue), 0);
    if (!queue) {
        palLog(nullptr, "Failed to allocate event queue");
        return PAL_FALSE;
    }

    QueueData* queueData = palAllocate(nullptr, sizeof(QueueData), 0);
    if (!queueData) {
        palFree(nullptr, queue);
        palLog(nullptr, "Failed to allocate event queue data");
        return PAL_FALSE;
    }

    memset(queueData, 0, sizeof(QueueData));
    queue->userData = queueData;
    queue->poll = eventPoll;
    queue->push = eventPush;

    PalEventDriverCreateInfo createInfo = {0};
    createInfo.queue = queue;

    PalEventDriver* eventDriver = nullptr;
    PalResult result = palCreateEventDriver(&createInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        palFree(nullptr, queueData);
        palFree(nullptr, queue);
        palLog(nullptr, "Failed to create event driver");
        return PAL_FALSE;
    }

    palSetEventDispatchMode(eventDriver, PAL_EVENT_TYPE_USER, PAL_DISPATCH_MODE_POLL);

    PalEvent pushed = {0};
    pushed.type = PAL_EVENT_TYPE_USER;
    pushed.data = 42;
    palPushEvent(eventDriver, &pushed);

    PalEvent polled = {0};
    PalBool ok = palPollEvent(eventDriver, &polled) && (polled.data == 42);

    palDestroyEventDriver(eventDriver);
    palFree(nullptr, queueData);
    palFree(nullptr, queue);

    return ok;
}

int main(void)
{
    palLog(nullptr, "Running event driver test...");

    if (!defaultQueue()) {
        return -1;
    }

    if (!customQueue()) {
        return -1;
    }

    return 0;
}