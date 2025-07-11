
#ifndef _PAL_EVENTS_H
#define _PAL_EVENTS_H

#include "pal_core.h"

PAL_DEFINE_HANDLE(PalEventInstance);
PAL_DEFINE_HANDLE(PalEvent);

typedef void (*PalEventCallback)(const PalEvent*);
typedef void (*PalPushFn)(void*, PalEvent*);
typedef PalResult (*PalPollFn)(void*, PalEvent*);

typedef enum PalEventType
{
    PAL_EVENT_QUIT,
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
    void* id;
    Int64 data1;
    Int64 data2;
} PalEvent;

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

_PAPI void _PCALL palDestroyEventInstance(PalEventInstance* eventInstance);

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

#endif // _PAL_EVENTS_H