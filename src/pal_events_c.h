
#ifndef _PAL_EVENT_C_H
 #define _PAL_EVENT_C_H

#include "pal_shared.h"
#include "pal/pal_events.h"

typedef struct PalEventSystem
{
    PalEvent queue[PAL_MAX_EVENTS];
    PalDispatch dispatchs[PAL_MAX_EVENTS];
    PalAllocator allocator;
    PalEventCallback callback;
    void* hiddenWindow;
    int head, tail;
    bool initialized;

} PalEventSystem;

static PalEventSystem s_Event;

bool palIsEventCoalesce(PalEventType type);
bool palCreateHiddenWindow();
void palDestroyHiddenWindow();

#define PAL_CHECK_EVENTS(ret) {                     \
    if (!s_Event.initialized) {                     \
        palSetError(PAL_EVENT_NOT_INITIALIZED);     \
        return ret;                                 \
    }                                               \
} 

#endif // _PAL_EVENT_C_H