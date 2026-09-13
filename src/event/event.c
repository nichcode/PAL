
/**
 * PAL - Prime Abstraction Layer (PAL)
 * A cross platform abstraction layer over graphics and windowing APIs
 * -------------------------------------------------------------------
 *
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "pal2/pal_event.h"
#include <string.h>

struct PalEventDriver 
{
    PalBool freeQueue;
    PalEventQueue* queue;
    const PalAllocator* allocator;
    PalEventCallback callback;
    void* userData;
    PalDispatchMode modes[PAL_DEFAULT_QUEUE_EVENT_COUNT];
};

PalEventQueue* createDefaultQueue(const PalAllocator* allocator);

void destroyDefaultQueue(
    const PalAllocator* allocator,
    PalEventQueue* queue);

PalResult PAL_CALL palCreateEventDriver(
    const PalEventDriverCreateInfo* info,
    PalEventDriver** outEventDriver)
{
    if (!info || !outEventDriver) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalEventDriver* driver = nullptr;
    driver = palAllocate(info->allocator, sizeof(PalEventDriver), 0);
    if (!driver) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
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
        PalEventQueue* queue = createDefaultQueue(info->allocator);
        if (!queue) {
            palFree(info->allocator, driver);
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
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
    const PalAllocator* allocator = eventDriver->allocator;
    if (eventDriver->freeQueue) {
        destroyDefaultQueue(allocator, eventDriver->queue);
    }

    palFree(allocator, eventDriver);
}

void PAL_CALL palSetEventDispatchMode(
    PalEventDriver* eventDriver,
    PalEventType type,
    PalDispatchMode mode)
{
    eventDriver->modes[type] = mode;
}

PalDispatchMode PAL_CALL palGetEventDispatchMode(
    PalEventDriver* eventDriver,
    PalEventType type)
{
    return eventDriver->modes[type];
}

void PAL_CALL palPushEvent(
    PalEventDriver* eventDriver,
    PalEvent* event)
{
    PalDispatchMode mode = eventDriver->modes[event->type];
    if (mode == PAL_DISPATCH_MODE_CALLBACK) {
        if (eventDriver->callback) {
            eventDriver->callback(eventDriver->userData, event);
        }

        return;
    }

    if (mode == PAL_DISPATCH_MODE_POLL) {
        eventDriver->queue->push(eventDriver->queue, event);
    }
}

PalBool PAL_CALL palPollEvent(
    PalEventDriver* eventDriver,
    PalEvent* outEvent)
{
    return eventDriver->queue->poll(eventDriver->queue, outEvent);
}
