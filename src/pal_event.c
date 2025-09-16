
/**

Copyright (C) 2025 Nicholas Agbo

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

// ==================================================
// Includes
// ==================================================

#include "pal/pal_event.h"

#include <string.h>

// ==================================================
// Typedefs, enums and structs
// ==================================================

#define PAL_MAX_EVENTS 512

typedef struct {
    Uint8    head;
    Uint8    tail;
    PalEvent data[PAL_MAX_EVENTS];
} QueueData;

typedef struct PalEventDriver {
    bool                freeQueue;
    PalEventQueue*      queue;
    const PalAllocator* allocator;
    PalEventCallback    callback;
    void*               userData;
    PalDispatchMode     modes[PAL_MAX_EVENTS];
} PalEventDriver;

// ==================================================
// Internal API
// ==================================================

static void PAL_CALL defaultPush(void* queue, PalEvent* event)
{

    PalEventQueue* eventQueue                 = queue;
    QueueData*     data                       = eventQueue->userData;
    data->data[data->tail++ % PAL_MAX_EVENTS] = *event;
}

static bool PAL_CALL defaultPoll(void* queue, PalEvent* outEvent)
{

    PalEventQueue* eventQueue = queue;
    QueueData*     data       = eventQueue->userData;
    if (data->head == data->tail) {
        return false;
    }

    *outEvent = data->data[data->head++ % PAL_MAX_EVENTS];
    return true;
}

// ==================================================
// Public API
// ==================================================

PalResult PAL_CALL palCreateEventDriver(const PalEventDriverCreateInfo* info,
                                        PalEventDriver**                outEventDriver)
{

    if (!info || !outEventDriver) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (info->allocator) {
        if (!info->allocator->allocate && !info->allocator->free) {
            return PAL_RESULT_INVALID_ALLOCATOR;
        }
    }

    PalEventDriver* driver = palAllocate(info->allocator, sizeof(PalEventDriver), 0);
    if (!driver) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(driver, 0, sizeof(PalEventDriver));
    if (info->allocator) {
        driver->allocator = info->allocator;
    }

    if (info->queue) {
        // user supplied an event queue
        driver->queue     = info->queue;
        driver->freeQueue = false;

    } else {
        // we create a default event queue
        driver->queue = palAllocate(info->allocator, sizeof(PalEventQueue), 0);
        if (!driver->queue) {
            palFree(info->allocator, driver);
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        driver->queue->userData = palAllocate(info->allocator, sizeof(QueueData), 0);
        if (!driver->queue->userData) {
            palFree(info->allocator, driver->queue);
            palFree(info->allocator, driver);
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        memset(driver->queue->userData, 0, sizeof(QueueData));
        driver->queue->poll = defaultPoll;
        driver->queue->push = defaultPush;
        driver->freeQueue   = true;
    }

    driver->callback = info->callback;
    *outEventDriver  = driver;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyEventDriver(PalEventDriver* eventDriver)
{

    if (!eventDriver) {
        return;
    }

    const PalAllocator* allocator = eventDriver->allocator;
    if (eventDriver->freeQueue) {
        palFree(allocator, eventDriver->queue->userData);
        palFree(allocator, eventDriver->queue);
    }
    palFree(allocator, eventDriver);
}

void PAL_CALL palSetEventDispatchMode(PalEventDriver* eventDriver, PalEventType type,
                                      PalDispatchMode mode)
{

    if (eventDriver) {
        eventDriver->modes[type] = mode;
    }
}

PalDispatchMode PAL_CALL palGetEventDispatchMode(PalEventDriver* eventDriver, PalEventType type)
{

    if (!eventDriver) {
        return PAL_DISPATCH_NONE;
    }
    return eventDriver->modes[type];
}

void PAL_CALL palPushEvent(PalEventDriver* eventDriver, PalEvent* event)
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

bool PAL_CALL palPollEvent(PalEventDriver* eventDriver, PalEvent* outEvent)
{

    if (!eventDriver || !outEvent) {
        return false;
    }

    return eventDriver->queue->poll(eventDriver->queue, outEvent);
}