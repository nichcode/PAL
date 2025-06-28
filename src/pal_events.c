
#include "pal_pch.h"
#include "pal_events_c.h"

bool _PCALL palInitEvent(const PalAllocator* allocator)
{
    bool success = PAL_FALSE;
    palSetAllocator(&s_Event.allocator, allocator);
    if (!success) {
        return PAL_FALSE;
    }
    
    success = palRegisterWindowClass();
    if (!success) {
        palSetError(PAL_PLATFORM_ERROR);
        return PAL_FALSE;
    }

    success = palCreateHiddenWindow();
    if (!success) {
        palSetError(PAL_PLATFORM_ERROR);
        return PAL_FALSE;
    }
    
    return success;
}

void _PCALL palShutdownEvent()
{
    if (s_Event.initialized) {
        palDestroyHiddenWindow();
    }

    palUnregisterWindowClass();
    s_Event.initialized = PAL_FALSE;
}

bool _PCALL palIsEventInit()
{
    return s_Event.initialized;
}

bool _PCALL palRegisterEvent(PalEventType type, PalDispatch dispatch)
{
    if (type < 0 || type > PAL_EVENT_MAX) {
        palSetError(PAL_INVALID_EVENT);
        return PAL_FALSE;
    }

    s_Event.dispatchs[type] = dispatch;
    return PAL_TRUE;
}

PalDispatch _PCALL palGetDispatch(PalEventType type)
{
    if (type < 0 || type > PAL_EVENT_MAX) {
        palSetError(PAL_INVALID_EVENT);
        return PAL_FALSE;
    }
    return s_Event.dispatchs[type];
}

void _PCALL palSetEventCallback(PalEventCallback callback)
{
    s_Event.callback = callback;
}

void _PCALL palTriggerEvent(const PalEvent* event)
{
    if (!event) {
        palSetError(PAL_NULL_POINTER);
        return;
    }

    PalDispatch dispatchType;
    dispatchType = s_Event.dispatchs[event->type];
    if (dispatchType == PAL_DISPATCH_CALLBACK) {
        if (s_Event.callback) {
            s_Event.callback(event);
        } 
    }
}

void _PCALL palPushEvent(PalEvent event)
{
    PalDispatch dispatchType;
    dispatchType = s_Event.dispatchs[event.type];

    if (dispatchType == PAL_DISPATCH_POLL) { 
        if (palIsEventCoalesce(event.type)) {
            for (int i = s_Event.tail - 1; i >= s_Event.head; --i) {
                PalEvent* e = &s_Event.queue[i & PAL_MAX_EVENTS];
                if (e->type == event.type) {
                    // event found. Copy the new event into the existing one
                    *e = event;
                    return;
                }
            }
        }

        // event is not a repeating event
        s_Event.queue[s_Event.tail++ & PAL_MAX_EVENTS] = event;
    }
}

bool palIsEventCoalesce(PalEventType type)
{
    switch (type) {
        case PAL_EVENT_WINDOW_RESIZE:
        return PAL_TRUE;
    }
    return PAL_FALSE;
}