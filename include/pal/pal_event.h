
#ifndef _PAL_EVENTS_H
#define _PAL_EVENTS_H

#include "pal_core.h"

PAL_DEFINE_HANDLE(PalEventInstance);
PAL_DEFINE_HANDLE(PalEvent);

typedef void (*PalEventCallback)(PalEvent*);
typedef void (*PalPushFn)(void*, PalEvent*);
typedef PalResult (*PalPollFn)(void*, PalEvent*);

typedef enum PalEventType
{
    PAL_EVENT_QUIT,
    PAL_EVENT_WINDOW_MOVE,
    PAL_EVENT_WINDOW_RESIZE,
    PAL_EVENT_MAX
} PalEventType;

typedef enum PalMode
{
    PAL_MODE_NONE,
    PAL_MODE_POLL,
    PAL_MODE_CALLBACK
} PalMode;

typedef struct PalEvent
{
    PalEventType type;
    Uint32 id;
    Int64 data;
    Int64 data2;
} PalEvent;

typedef struct PalEventPosInfo
{
    int x;
    int y;
} PalEventPosInfo;

typedef struct PalEventSizeInfo
{
    Uint32 width;
    Uint32 height;
} PalEventSizeInfo;

typedef struct PalEventQueue
{
    PalPushFn push;
    PalPollFn poll;
    void* userData;
} PalEventQueue;

typedef struct PalEventInstanceDesc
{
    PalAllocator* allocator;
    PalEventQueue* queue;
    PalEventCallback callback;
} PalEventInstanceDesc;

_PAPI PalResult _PCALL palCreateEventInstance(
    PalEventInstanceDesc* desc, 
    PalEventInstance** outEventInstance);

_PAPI void _PCALL palDestroyEventInstance(
    PalEventInstance* eventInstance);

_PAPI PalResult _PCALL palEnableEvent(
    PalEventInstance* eventInstance, 
    PalEventType type, 
    PalMode mode);

_PAPI PalResult _PCALL palGetEventMode(
    PalEventInstance* eventInstance, 
    PalEventType type, 
    PalMode* mode);

_PAPI PalResult _PCALL palPushEvent(
    PalEventInstance* eventInstance, 
    PalEvent* event);

_PAPI PalResult _PCALL palPollEvent(
    PalEventInstance* eventInstance, 
    PalEvent* event);

static inline Int64 palPackUint32(
    Uint32 low,
    Uint32 high)
{
    return (Int64) (((Uint64)high << 32) | (Uint64)low);
}

static inline Int64 palPackInt32(
    int low,
    int high)
{
    return ((Int64) (Uint32)high << 32) | (Uint32)low;
}

static inline void palUnpackUint32(
    Int64 data,
    Uint32* outLow,
    Uint32* outHigh)
{
    if (outLow) {
        *outLow = (Uint32)(data & 0xFFFFFFFF);
    }

    if (outHigh) {
        *outHigh = (Uint32)((Uint64)data >> 32);
    }
}

static inline void palUnpackInt32(
    Int64 data,
    int* outLow,
    int* outHigh)
{
    if (outLow) {
        *outLow = (int)(data & 0xFFFFFFFF);
    }

    if (outHigh) {
        *outHigh = (int)((Uint64)data >> 32);
    }
}

static inline void palGetEventPosInfo(
    PalEvent* event,
    PalEventPosInfo* info)
{
    if (!event || !info) {
        return;
    }
    int x, y;
    palUnpackInt32(event->data, &x, &y);
    info->x = x;
    info->y = y;
}

static inline void palGetEventSizeInfo(
    PalEvent* event,
    PalEventSizeInfo* info)
{
    if (!event || !info) {
        return;
    }
    Uint32 width, height;
    palUnpackUint32(event->data, &width, &height);
    info->width = width;
    info->height = height;
}

#endif // _PAL_EVENTS_H