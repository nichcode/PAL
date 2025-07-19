
#ifndef _PAL_EVENT_QUEUE
#define _PAL_EVENT_QUEUE

#include "pal/pal_core.h"

typedef struct PalEventQueueData {
    PalEvent* data;
    Uint32 count;
    Uint32 capacity;
    Uint32 iterator;
} PalEventQueueData;

bool palCreateEventQueue(
    PalAllocator* allocator,
    PalEventQueue** outQueue);

void palDestroyEventQueue(
    PalAllocator* allocator,
    PalEventQueue* queue);

void palEventQueuePush(
    void* data, 
    PalEvent* event);

bool palEventQueuePoll(
    void* data, 
    PalEvent* outEvent);

#endif // _PAL_EVENT_QUEUE