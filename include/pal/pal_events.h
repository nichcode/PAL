
#ifndef _PAL_EVENTS_H
 #define _PAL_EVENTS_H

#include "pal_core.h"

typedef enum PalEventType
{
    //PAL_EVENT_NONE,
    PAL_EVENT_QUIT,
    PAL_EVENT_WINDOW_RESIZE,
    PAL_EVENT_WINDOW_MOVE,
    PAL_EVENT_MAX

} PalEventType;

typedef enum PalDispatch
{
    PAL_DISPATCH_NONE,
    PAL_DISPATCH_POLL,
    PAL_DISPATCH_CALLBACK

} PalDispatch;

typedef struct PalEvent
{
    PalEventType type;
    PalWindowID windowID;

    union {
        struct { Uint32 width; Uint32 height; } size;
        struct { int x; int y; } windowPos;
    };

} PalEvent;

typedef void (*PalEventCallback)(const PalEvent* event);

_PAPI bool _PCALL palInitEvent(const PalAllocator* allocator);
_PAPI void _PCALL palShutdownEvent();
_PAPI bool _PCALL palIsEventInit();

_PAPI bool _PCALL palRegisterEvent(PalEventType type, PalDispatch dispatch);
_PAPI PalDispatch _PCALL palGetDispatch(PalEventType type);

_PAPI void _PCALL palSetEventCallback(PalEventCallback callback);
_PAPI void _PCALL palTriggerEvent(const PalEvent* event);

_PAPI void _PCALL palPushEvent(PalEvent event);
_PAPI bool _PCALL palPollEvent(PalEvent* event);

#endif // _PAL_EVENTS_H