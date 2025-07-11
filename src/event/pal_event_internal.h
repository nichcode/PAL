
#ifndef _PAL_EVENT_INTERNAL_H
#define _PAL_EVENT_INTERNAL_H

#include "pal/pal_event.h"

#define PAL_MAX_EVENTS 256

typedef struct PalEventData
{
    PalEvent data[PAL_MAX_EVENTS];
    int head;
    int tail;
} PalEventData;

typedef struct PalEventInstance
{
    PalMode modes[PAL_MAX_EVENTS];
    PalAllocator* allocator;
    PalEventQueue* queue;
    PalEventCallback callback;
    PalEventData* eventData;
    bool free;
} PalEventInstance;

#endif // _PAL_EVENT_INTERNAL_H