
#ifndef _PAL_EVENT_H
#define _PAL_EVENT_H

#include "pal_core.h"

typedef struct PalEventDriver PalEventDriver;
typedef struct PalEvent PalEvent;

typedef void (PAL_CALL *PalEventCallback)(
    void* userData, 
    const PalEvent* event);

typedef void (PAL_CALL *PalPushFn)(
    void* userData, 
    PalEvent* event);

typedef bool (PAL_CALL *PalPollFn)(
    void* userData, 
    PalEvent* outEvent);

typedef enum {
    PAL_EVENT_WINDOW_CLOSE,
    PAL_EVENT_WINDOW_SIZE,
    PAL_EVENT_WINDOW_MOVE,
    PAL_EVENT_WINDOW_STATE,
    PAL_EVENT_WINDOW_FOCUS,
    PAL_EVENT_WINDOW_VISIBILITY,
    PAL_EVENT_WINDOW_MODAL_BEGIN,
    PAL_EVENT_WINDOW_MODAL_END,
    PAL_EVENT_MONITOR_DPI_CHANGED,
    PAL_EVENT_MONITOR_LIST_CHANGED,
    PAL_EVENT_KEYDOWN,
    PAL_EVENT_KEYREPEAT,
    PAL_EVENT_KEYUP,
    PAL_EVENT_MOUSE_BUTTONDOWN,
    PAL_EVENT_MOUSE_BUTTONUP,
    PAL_EVENT_MOUSE_MOVE,
    PAL_EVENT_MOUSE_DELTA,
    PAL_EVENT_MOUSE_WHEEL,
    PAL_EVENT_USER,
    PAL_EVENT_MAX
} PalEventType;

typedef enum {
    PAL_DISPATCH_NONE,
    PAL_DISPATCH_CALLBACK,
    PAL_DISPATCH_POLL,
    PAL_DISPATCH_MAX
} PalDispatchMode;

typedef struct PalEvent {
    PalEventType type;
    Int64 data;
    Int64 data2;
    Int64 userId;
} PalEvent;

typedef struct {
    PalPushFn push;
    PalPollFn poll;
    void* userData;
} PalEventQueue;

typedef struct {
    const PalAllocator* allocator;
    PalEventQueue* queue;
    PalEventCallback callback;
    void* userData;
} PalEventDriverCreateInfo;

PAL_API PalResult PAL_CALL palCreateEventDriver(
    const PalEventDriverCreateInfo* info, 
    PalEventDriver** outEventDriver);

PAL_API void PAL_CALL palDestroyEventDriver(
    PalEventDriver* eventDriver);

PAL_API void PAL_CALL palSetEventDispatchMode(
    PalEventDriver* eventDriver, 
    PalEventType type, 
    PalDispatchMode mode);

PAL_API PalDispatchMode PAL_CALL palGetEventDispatchMode(
    PalEventDriver* eventDriver, 
    PalEventType type);

PAL_API void PAL_CALL palPushEvent(
    PalEventDriver* eventDriver,
    PalEvent* event);

PAL_API bool PAL_CALL palPollEvent(
    PalEventDriver* eventDriver,
    PalEvent* outEvent);

#endif // _PAL_EVENT_H