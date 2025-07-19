
#include "pal_pch.h"
#include "pal_event_queue.h"

bool eventCoalesce(PalEventType type);

bool palCreateEventQueue(
    PalAllocator* allocator,
    PalEventQueue** outQueue) {


        

    PalEventQueue* queue;
    PalEventQueueData* queueData;
    queue = palAllocate(allocator, sizeof(PalEventQueue));
    if (!queue) {
        return PAL_FALSE;
    }

    queueData = palAllocate(allocator, sizeof(PalEventQueueData));
    if (!queue) {
        return PAL_FALSE;
    }

    queueData->capacity = 2;
    queueData->count = 0;
    queueData->iterator = 0;
    queueData->data = palAllocate(allocator, 2 * sizeof(PalEvent));

    if (queueData->data) {
        palFree(allocator, queue);
        return PAL_FALSE;
    }

    queue->push = palEventQueuePush;
    queue->poll = palEventQueuePoll;
    queue->userData = queueData;

    *outQueue = queue;
    return PAL_TRUE;
}

void palDestroyEventQueue(
    PalAllocator* allocator,
    PalEventQueue* queue) {

    PalEventQueueData* data = queue->userData;
    palFree(allocator, data->data);
    palFree(allocator, data);
    palFree(allocator, queue);
}

void palEventQueuePush(
    void* data, 
    PalEvent* event) {

    PalEventQueueData* queueData = data;
    if (queueData->count > 0 && eventCoalesce(event->type)) {
        PalEvent* lastEvent = &queueData->data[queueData->count - 1];
        *lastEvent = *event;
    }

    // check and resize if maxed out
    if (queueData->count >= queueData->capacity) {
        Uint32 newCapacity = queueData->capacity * 2;
        PalEvent* newData = realloc(queueData->data, newCapacity * sizeof(PalEvent));
        if (!newData) {
            // TODO: reset queue
            return;
        }

        queueData->capacity = newCapacity;
        queueData->data = newData;
    }

    queueData->data[queueData->count++] = *event;
    return PAL_TRUE;
}

bool palEventQueuePoll(
    void* data, 
    PalEvent* outEvent) {

    PalEventQueueData* queueData = data;
    if (queueData->iterator >= queueData->count) {
        return PAL_FALSE;
    }

    *outEvent = queueData->data[queueData->iterator++];
    return PAL_TRUE;
}

bool eventCoalesce(PalEventType type) {
    return PAL_FALSE;
}