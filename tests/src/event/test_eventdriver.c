
#include "pal2/pal_event.h"
#include <stdbool.h>

#define MAX_EVENTS 64

typedef struct QueueData
{
    uint32_t head;
    uint32_t tail;
    PalEvent data[MAX_EVENTS];
} QueueData;

static void PAL_CALL eventPush(
    void* queue,
    PalEvent* event)
{
    PalEventQueue* eventQueue = queue;
    QueueData* data = eventQueue->userData;
    data->data[data->tail++ % MAX_EVENTS] = *event;
}

static PalBool PAL_CALL eventPoll(
    void* queue,
    PalEvent* outEvent)
{
    PalEventQueue* eventQueue = queue;
    QueueData* data = eventQueue->userData;
    if (data->head == data->tail) {
        return PAL_FALSE;
    }

    *outEvent = data->data[data->head++ % MAX_EVENTS];
    return PAL_TRUE;
}

bool defaultQueue(void)
{
    PalEventDriverCreateInfo createInfo = {0};
    createInfo.allocator = nullptr;
    createInfo.callback = nullptr;
    createInfo.queue = nullptr;
    createInfo.userData = nullptr;

    PalEventDriver* eventDriver = nullptr;
    PalResult result = palCreateEventDriver(&createInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create event driver");
        return false;
    }

    palDestroyEventDriver(eventDriver);

    return true;
}

bool customQueue(void)
{
    PalEventQueue* queue = nullptr;
    queue = palAllocate(nullptr, sizeof(PalEventQueue), 0);
    if (!queue) {
        palLog(nullptr, "Failed to allocate event queue");
        return false;
    }

    QueueData* queueData = nullptr;
    queueData = palAllocate(nullptr, sizeof(QueueData), 0);
    if (!queueData) {
        palFree(nullptr, queue);

        palLog(nullptr, "Failed to allocate event queue data");
        return false;
    }

    memset(queueData, 0, sizeof(QueueData));
    queue->userData = queueData;
    queue->poll = eventPoll;
    queue->push = eventPush;

    PalEventDriverCreateInfo createInfo = {0};
    createInfo.allocator = nullptr;
    createInfo.callback = nullptr;
    createInfo.queue = queue;
    createInfo.userData = nullptr;

    PalEventDriver* eventDriver = nullptr;
    PalResult result = palCreateEventDriver(&createInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        palFree(nullptr, queue->userData);
        palFree(nullptr, queue);

        palLog(nullptr, "Failed to create event driver");
        return false;
    }

    palDestroyEventDriver(eventDriver);

    palFree(nullptr, queue->userData);
    palFree(nullptr, queue);

    return true;
}

int main(void)
{
    palLog(nullptr, "Running event driver test...");

    bool ret = defaultQueue();
    if (!ret) {
        return -1;
    }

    ret = customQueue();
    if (!ret) {
        return -1;
    }

    return 0;
}