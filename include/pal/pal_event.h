
#ifndef _PAL_EVENT_H
#define _PAL_EVENT_H

#include "pal_core.h"

PAL_DECLARE_HANDLE(PalEventDriver);

typedef struct PalEvent PalEvent;
typedef void (*PalEventCallback)(void*, const PalEvent*);
typedef void (*PalPushFn)(void*, PalEvent*);
typedef bool (*PalPollFn)(void*, PalEvent*);

typedef enum PalEventType {
    PAL_EVENT_QUIT,
    PAL_EVENT_WINDOW_RESIZE,
    PAL_EVENT_WINDOW_MOVE,
    PAL_EVENT_DPI_CHANGED,
    PAL_EVENT_USER,
    PAL_EVENT_MAX
} PalEventType;

typedef enum PalEventMode {
    PAL_EVENT_MODE_DISABLED,
    PAL_EVENT_MODE_POLL,
    PAL_EVENT_MODE_CALLBACK
} PalEventMode;

typedef struct PalEventPosInfo {
    int x;
    int y;
} PalEventPosInfo;

typedef struct PalEventSizeInfo {
    Uint32 width;
    Uint32 height;
} PalEventSizeInfo;

typedef struct PalEvent {
    PalEventType type;
    Int64 data;
    Int64 data2;
    Uint32 userID;
    Uint32 sourceID;
} PalEvent;

typedef struct PalEventQueue {
    PalPushFn push;
    PalPollFn poll;
    void* userData;
} PalEventQueue;

typedef struct PalEventDriverConfig {
    PalAllocator* allocator;
    PalEventQueue* queue;
    PalEventCallback callback;
    void* userData;
} PalEventDriverConfig;

_PAPI PalResult _PCALL palCreateEventDriver(
    PalEventDriverConfig* config, 
    PalEventDriver* outEventDriver);

_PAPI void _PCALL palDestroyEventDriver(
    PalEventDriver eventDriver);

_PAPI PalResult _PCALL palEnableEvent(
    PalEventDriver eventDriver, 
    PalEventType type, 
    PalEventMode mode);

_PAPI PalResult _PCALL palEnableAllEvents(
    PalEventDriver eventDriver, 
    PalEventMode mode);

_PAPI PalResult _PCALL palGetEventMode(
    PalEventDriver eventDriver, 
    PalEventType type, 
    PalEventMode* mode);

_PAPI PalResult _PCALL palPushEvent(
    PalEventDriver eventDriver,
    PalEvent* event);

_PAPI bool _PCALL palPollEvent(
    PalEventDriver eventDriver,
    PalEvent* outEvent);

static inline void palGetEventPosInfo(
    const PalEvent* event,
    PalEventPosInfo* info) {
    if (!event || !info) {
        return;
    }
    int x, y;
    palUnpackInt32(event->data, &x, &y);
    info->x = x;
    info->y = y;
}

static inline void palGetEventSizeInfo(
    const PalEvent* event,
    PalEventSizeInfo* info) {
    if (!event || !info) {
        return;
    }
    Uint32 width, height;
    palUnpackUint32(event->data, &width, &height);
    info->width = width;
    info->height = height;
}

#endif // _PAL_EVENT_H