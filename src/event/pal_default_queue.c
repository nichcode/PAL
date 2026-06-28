
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal_default_queue.h"

typedef struct {
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

PalEventQueue* createDefaultEventQueue(const PalAllocator* allocator)
{
    PalEventQueue* queue = nullptr;
    queue = palAllocate(allocator, sizeof(PalEventQueue), 0);
    if (!queue) {
        return nullptr;
    }

    // we create a default event queue data
    QueueData* queueData = nullptr;
    queueData = palAllocate(allocator, sizeof(QueueData), 0);
    if (!queueData) {
        palFree(allocator, queue);
        return nullptr;
    }

    memset(queueData, 0, sizeof(QueueData));
    queue->userData = queueData;
    queue->poll = eventPoll;
    queue->push = eventPush;

    return queue;
}

void destroyDefaultEventQueue(
    const PalAllocator* allocator, 
    PalEventQueue* queue)
{
    palFree(allocator, queue->userData);
    palFree(allocator, queue);
}