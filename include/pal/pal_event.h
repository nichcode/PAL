
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
    PAL_EVENT_EVENT,
    PAL_EVENT_MAX
} PalEventType;

typedef enum PalEventMode {
    PAL_EVENT_MODE_DISABLED,
    PAL_EVENT_MODE_POLL,
    PAL_EVENT_MODE_CALLBACK
} PalEventMode;

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

#endif // _PAL_EVENT_H