
#include "pal_pch.h"
#include "pal/pal_events.h"
#include "pal_internal.h"

bool isEventRepeat(PalEventType type);

bool _PCALL palRegisterEvent(PalEventType type, PalDispatch dispatch)
{
    if (type < 0 || type > PAL_EVENT_MAX) {
        palSetError(PAL_INVALID_EVENT);
        return PAL_FALSE;
    }

    s_EventQueue.dispatchs[type] = dispatch;
    return PAL_TRUE;
}

PalDispatch _PCALL palGetDispatch(PalEventType type)
{
    if (type < 0 || type > PAL_EVENT_MAX) {
        palSetError(PAL_INVALID_EVENT);
        return PAL_FALSE;
    }
    return s_EventQueue.dispatchs[type];
}

void _PCALL palSetEventCallback(PalEventCallback callback)
{
    s_EventQueue.callback = callback;
}

void _PCALL palTriggerEvent(const PalEvent* event)
{
    if (!event) {
        palSetError(PAL_NULL_POINTER);
        return;
    }

    PalDispatch dispatchType;
    dispatchType = s_EventQueue.dispatchs[event->type];
    if (dispatchType == PAL_DISPATCH_CALLBACK) {
        if (s_EventQueue.callback) {
            s_EventQueue.callback(event);
        } 
    }
}

void _PCALL palPushEvent(PalEvent event)
{
    PalDispatch dispatchType;
    dispatchType = s_EventQueue.dispatchs[event.type];

    if (dispatchType == PAL_DISPATCH_POLL) { 
        // check if the event is a repeating event. eg. window pos, window size
        // and search the event queue for duplicates
        if (isEventRepeat(event.type)) {
            for (int i = s_EventQueue.tail - 1; i >= s_EventQueue.head; --i) {
                PalEvent* e = &s_EventQueue.data[i & _PAL_MAX_EVENTS];
                if (e->type == event.type) {
                    // event found. Copy the new event into the existing one
                    *e = event;
                    return;
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
        case PAL_EVENT_WINDOW_RESIZE:
        return PAL_TRUE;
    }
    return PAL_FALSE;
}