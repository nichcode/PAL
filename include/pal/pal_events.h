
#ifndef _PAL_EVENTS_H
 #define _PAL_EVENTS_H

#include "pal_video.h"

typedef enum PalEventType
{
    PAL_EVENT_QUIT,
    PAL_EVENT_MAX

} PalEventType;

typedef enum PalEventDispatchType
{
    PAL_EVENT_DISPATCH_NONE,
    PAL_EVENT_DISPATCH_POLL,
    PAL_EVENT_DISPATCH_CALLBACK

} PalEventDispatchType;

typedef struct PalEvent
{
    PalEventType type;
    PalWindowID windowID;

} PalEvent;

typedef void (*PalEventCallback)(const PalEvent* event);

_PAPI bool _PCALL palRegisterEvent(PalEventType type, PalEventDispatchType dispatch);
_PAPI PalEventDispatchType _PCALL palGeteventDispatchType(PalEventType type);

_PAPI void _PCALL palSetEventCallback(PalEventCallback callback);
_PAPI void _PCALL palPushEvent(PalEvent event);
_PAPI bool _PCALL palPollEvent(PalEvent* event);

#endif // _PAL_EVENTS_H