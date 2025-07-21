
#include "pal_pch.h"
#include "pal_event_internal.h"

bool eventIsCoalesce(PalEventType type);

PalResult palCreateEventQueue(
    PalAllocator* allocator, 
    PalEventQueue** outQueue) {

    PalEventQueue* queue = palAllocate(allocator, sizeof(PalEventQueue));
    if (!queue) {
        return PAL_ERROR_OUT_OF_MEMORY;
    }

    queue->userData = palAllocate(allocator, sizeof(PalEventQueueData));
    if (!queue->userData) {
        return PAL_ERROR_OUT_OF_MEMORY;
    }

    palZeroMemory(queue->userData, sizeof(PalEventQueueData));
    queue->poll = palPollQueue;
    queue->push = palPushQueue;

    *outQueue = queue;
    return PAL_SUCCESS;
}

void palDestroyEventQueue(
    PalAllocator* allocator, 
    PalEventQueue* queue) {

    palFree(allocator, queue->userData);
    palFree(allocator, queue);
}

void palPushQueue(
    void* queue,
    PalEvent* event) {

    PalEventQueue* eventQueue = queue;
    PalEventQueueData* data = eventQueue->userData;
    if (eventIsCoalesce(event->type)) {
        for (int i = data->tail - 1; i >= data->head; --i) {
            PalEvent* e = &data->data[i & PAL_MAX_EVENTS];
            if (e->type == event->type) {
                *e = *event;
                return;
            }
        }
    }

    data->data[data->tail++ & PAL_MAX_EVENTS] = *event;
}

bool palPollQueue(
    void* queue, 
    PalEvent* outEvent) {

    PalEventQueue* eventQueue = queue;
    PalEventQueueData* data = eventQueue->userData;
    if (data->head == data->tail) {
        return PAL_FALSE;
    }

    *outEvent = data->data[data->head++ & PAL_MAX_EVENTS];    
    return PAL_TRUE;
}

bool eventIsCoalesce(PalEventType type)
{
    switch (type) {
        case PAL_EVENT_WINDOW_MOVE:
        case PAL_EVENT_WINDOW_RESIZE: {
            return PAL_TRUE;
        }
    }
    return PAL_FALSE;
}