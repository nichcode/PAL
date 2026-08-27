
#include "tests.h"
#include "pal2/pal_event.h"

#define EVENT_COUNT 128

typedef struct {
    uint32_t head;
    uint32_t tail;
    PalEvent data[EVENT_COUNT];
} QueueData;

static void PAL_CALL eventPush(
    void* queue,
    PalEvent* event)
{
    PalEventQueue* eventQueue = queue;
    QueueData* data = eventQueue->userData;
    data->data[data->tail++ % EVENT_COUNT] = *event;
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

    *outEvent = data->data[data->head++ % EVENT_COUNT];
    return PAL_TRUE;
}

PalBool eventDriverTest()
{
    PalEventDriver* eventDriver = nullptr;
    PalEventQueue* queue = palAllocate(nullptr, sizeof(PalEventQueue), 0);
    QueueData* queueData = palAllocate(nullptr, sizeof(QueueData), 0);
    if (!queue || !queueData) {
        return PAL_FALSE;
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

    PalResult result = palCreateEventDriver(&createInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    palLog(nullptr, "Event Driver Created %p", (void*)eventDriver);
    palDestroyEventDriver(eventDriver);
    palFree(nullptr, queue);
    palFree(nullptr, queueData);
}

void registerEventTests()
{
    registerTest(eventDriverTest, "Event Driver Test");
}