
#include "pal_pch.h"
#include "pal/pal_events.h"
#include "pal_internal.h"

bool isEventRepeat(PalEventType type);

bool _PCALL palRegisterEvent(PalEventType type, PalEventDispatchType dispatch)
{
    if (type < 0 || type > PAL_EVENT_MAX) {
        palSetError(PAL_INVALID_EVENT);
        return PAL_FALSE;
    }

    s_EventQueue.dispatchTypes[type] = dispatch;
    return PAL_TRUE;
}

PalEventDispatchType _PCALL palGeteventDispatchType(PalEventType type)
{
    if (type < 0 || type > PAL_EVENT_MAX) {
        palSetError(PAL_INVALID_EVENT);
        return PAL_FALSE;
    }
    return s_EventQueue.dispatchTypes[type];
}

void _PCALL palSetEventCallback(PalEventCallback callback)
{
    s_EventQueue.callback = callback;

}

void _PCALL palPushEvent(PalEvent event)
{
    PalEventDispatchType dispatchType = s_EventQueue.dispatchTypes[event.type];
    if (dispatchType == PAL_EVENT_DISPATCH_CALLBACK) {
        // check if we have a valid callback
        if (s_EventQueue.callback) {
            s_EventQueue.callback(&event);
        }
        return;
    }

    if (dispatchType == PAL_EVENT_DISPATCH_POLL) { 
        // check if the event is a repeating event. eg. window pos, window size
        // and search the event queue for duplicates
        if (isEventRepeat(event.type)) {
            for (int i = s_EventQueue.tail - 1; i >= s_EventQueue.head; --i) {
                PalEvent* e = &s_EventQueue.data[i & _PAL_MAX_EVENTS];
                if (e->type == event.type) {
                    // event found. Copy the new event into the existing one
                    *e = event;
                    break;
                }
            }
        }

        // event is not a repeating event
        s_EventQueue.data[s_EventQueue.tail++ & _PAL_MAX_EVENTS] = event;
    }
}

bool _PCALL palPollEvent(PalEvent* event)
{
    // poll platform events for both the callbacks and queue
    _palPlatformPollEvents();

    if (!event || s_EventQueue.head == s_EventQueue.tail) {
        // no events registered with PAL_EVENT_DISPATCH_POLL
        return PAL_FALSE;
    }

    *event = s_EventQueue.data[s_EventQueue.head++ & _PAL_MAX_EVENTS];
    return PAL_TRUE;
}

bool isEventRepeat(PalEventType type)
{
    switch (type) {
        // TODO:
    }
    return PAL_FALSE;
}