
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
    PAL_EVENT_WINDOW_SHOWN,
    PAL_EVENT_WINDOW_FOCUS,
    PAL_EVENT_WINDOW_MODAL_BEGIN,
    PAL_EVENT_WINDOW_MODAL_END,
    PAL_EVENT_DPI_CHANGED,
    PAL_EVENT_DISPLAYS_CHANGED,
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

static inline Int64 PAL_CALL palPackUint32(
    Uint32 low,
    Uint32 high) {

    return (Int64) (((Uint64)high << 32) | (Uint64)low);
}

static inline Int64 PAL_CALL palPackInt32(
    Int32 low,
    Int32 high) {

    return ((Int64) (Uint32)high << 32) | (Uint32)low;
}

static inline Int64 PAL_CALL palPackPointer(
    void* ptr) {
    
    return (Int64)(UintPtr)ptr;
}

static inline void PAL_CALL palUnpackUint32(
    Int64 data,
    Uint32* outLow,
    Uint32* outHigh) {

    if (outLow) {
        *outLow = (Uint32)(data & 0xFFFFFFFF);
    }

    if (outHigh) {
        *outHigh = (Uint32)((Uint64)data >> 32);
    }
}

static inline void PAL_CALL palUnpackInt32(
    Int64 data,
    Int32* outLow,
    Int32* outHigh) {

    if (outLow) {
        *outLow = (Int32)(data & 0xFFFFFFFF);
    }

    if (outHigh) {
        *outHigh = (Int32)((Uint64)data >> 32);
    }
}

static inline void* PAL_CALL palUnpackPointer(
    Int64 data) {
    
    return (void*)(UintPtr)data;
}

#endif // _PAL_EVENT_H