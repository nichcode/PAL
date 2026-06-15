
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal_default_queue.h"
#include "pal_shared.h"
#include <string.h>

#ifdef _WIN32
#define PLATFORM_SOURCE PAL_RESULT_SOURCE_WINDOWS

uint32_t __stdcall GetLastError();

#elif defined(__linux__)
#include <errno.h>
#define PLATFORM_SOURCE PAL_RESULT_SOURCE_LINUX
#endif // _WIN32

static inline uint32_t getLastErrorCode()
{
#ifdef _WIN32
    return GetLastError();
#elif defined(__linux__)
    return (uint32_t)errno;
#endif // _WIN32
}

struct PalEventDriver {
    PalBool freeQueue;
    PalEventQueue* queue;
    const PalAllocator* allocator;
    PalEventCallback callback;
    void* userData;
    PalDispatchMode modes[MAX_EVENTS];
};

PalResult PAL_CALL palCreateEventDriver(
    const PalEventDriverCreateInfo* info,
    PalEventDriver** outEventDriver)
{
    if (!info || !outEventDriver) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PLATFORM_SOURCE, 
            getLastErrorCode());
    }

    if (info->allocator) {
        if (!info->allocator->allocate && !info->allocator->free) {
            return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PLATFORM_SOURCE, 
            getLastErrorCode());
        }
    }

    PalEventDriver* driver = nullptr;
    driver = palAllocate(info->allocator, sizeof(PalEventDriver), 0);
    if (!driver) {
        return palMakeResult(
            PAL_RESULT_OUT_OF_MEMORY, 
            PLATFORM_SOURCE, 
            getLastErrorCode());
    }

    memset(driver, 0, sizeof(PalEventDriver));
    if (info->allocator) {
        driver->allocator = info->allocator;
    }

    if (info->queue) {
        // user supplied an event queue
        driver->queue = info->queue;
        driver->freeQueue = PAL_FALSE;

    } else {
        // we create a default event queue
        PalEventQueue* queue = createDefaultEventQueue(info->allocator);
        if (!queue) {
            palFree(info->allocator, driver);
            return palMakeResult(
                PAL_RESULT_OUT_OF_MEMORY, 
                PLATFORM_SOURCE, 
                getLastErrorCode());
        }

        driver->queue = queue;
        driver->freeQueue = PAL_TRUE;
    }

    driver->callback = info->callback;
    driver->userData = info->userData;
    *outEventDriver = driver;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyEventDriver(PalEventDriver* eventDriver)
{
    if (!eventDriver) {
        return;
    }

    const PalAllocator* allocator = eventDriver->allocator;
    if (eventDriver->freeQueue) {
        destroyDefaultEventQueue(allocator, eventDriver->queue);
    }
    palFree(allocator, eventDriver);
}

void PAL_CALL palSetEventDispatchMode(
    PalEventDriver* eventDriver,
    PalEventType type,
    PalDispatchMode mode)
{
    if (eventDriver) {
        eventDriver->modes[type] = mode;
    }
}

PalDispatchMode PAL_CALL palGetEventDispatchMode(
    PalEventDriver* eventDriver,
    PalEventType type)
{
    if (!eventDriver) {
        return PAL_DISPATCH_NONE;
    }
    return eventDriver->modes[type];
}

void PAL_CALL palPushEvent(
    PalEventDriver* eventDriver,
    PalEvent* event)
{
    if (!eventDriver || !event) {
        return;
    }

    // get the event mode
    PalDispatchMode mode = eventDriver->modes[event->type];
    if (mode == PAL_DISPATCH_CALLBACK) {
        if (eventDriver->callback) {
            eventDriver->callback(eventDriver->userData, event);
        }
        return; // we have dispatched the event
    }

    if (mode == PAL_DISPATCH_POLL) {
        eventDriver->queue->push(eventDriver->queue, event);
    }
}

PalBool PAL_CALL palPollEvent(
    PalEventDriver* eventDriver,
    PalEvent* outEvent)
{
    if (!eventDriver || !outEvent) {
        return PAL_FALSE;
    }

    return eventDriver->queue->poll(eventDriver->queue, outEvent);
}
