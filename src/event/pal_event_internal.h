
#ifndef _PAL_EVENT_INTERNAL_H
#define _PAL_EVENT_INTERNAL_H

#include "pal/pal_event.h"

#define PAL_MAX_EVENTS 64

typedef struct PalEventQueueData {
    PalEvent data[PAL_MAX_EVENTS];
    int head;
    int tail;
} PalEventQueueData;

struct PalEventDriver_T {
    PalEventMode modes[PAL_MAX_EVENTS];
    PalEventQueue* queue;
    PalAllocator* allocator;
    PalEventCallback callback;
    void* userData;
    bool freeQueue;
};

PalResult palCreateEventQueue(
    PalAllocator* allocator, 
    PalEventQueue** outQueue);

void palDestroyEventQueue(
    PalAllocator* allocator, 
    PalEventQueue* queue);

void palPushQueue(
    void* queue,
    PalEvent* event);

bool palPollQueue(
    void* queue, 
    PalEvent* outEvent);

#endif // _PAL_EVENT_INTERNAL_H