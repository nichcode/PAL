
#include "pal_pch.h"
#include "pal_event_internal.h"

PalResult _PCALL palCreateEventDriver(
    PalEventDriverConfig* config, 
    PalEventDriver* outEventDriver) {

    PalEventDriver eventDriver;
    if (!outEventDriver || !config) {
        return PAL_ERROR_NULL_POINTER;
    }

    if (config->allocator) {
        if (!config->allocator->alloc && !config->allocator->free) {
            return PAL_ERROR_INVALID_ALLOCATOR;
        }
    }

    eventDriver = palAllocate(
        config->allocator, 
        sizeof(struct PalEventDriver_T)
    );

    if (!eventDriver) {
        return PAL_ERROR_OUT_OF_MEMORY;
    }

    palZeroMemory(eventDriver, sizeof(struct PalEventDriver_T));
    if (config->allocator) {
        eventDriver->allocator = config->allocator;
    }

    if (config->queue) {
        eventDriver->queue = config->queue;
        eventDriver->freeQueue = PAL_FALSE;

    } else {
        PalResult result = palCreateEventQueue(
            eventDriver->allocator,
            &eventDriver->queue
        );

        if (result != PAL_SUCCESS) {
            return result;
        }
        eventDriver->freeQueue = PAL_TRUE;
    }

    eventDriver->callback = config->callback;
    *outEventDriver = eventDriver;
    return PAL_SUCCESS;
}

void _PCALL palDestroyEventDriver(
    PalEventDriver eventDriver) {

    if (!eventDriver) {
        return;
    }
    
    PalAllocator* allocator = eventDriver->allocator;
    if (eventDriver->freeQueue) {
        palDestroyEventQueue(allocator, eventDriver->queue);
    }
    palFree(allocator, eventDriver);
}

PalResult _PCALL palEnableEvent(
    PalEventDriver eventDriver, 
    PalEventType type, 
    PalEventMode mode) {

    if (!eventDriver) {
        return PAL_ERROR_NULL_POINTER;
    }

    eventDriver->modes[type] = mode;
    return PAL_SUCCESS;
}

PalResult _PCALL palEnableAllEvents(
    PalEventDriver eventDriver, 
    PalEventMode mode) {

    if (!eventDriver) {
        return PAL_ERROR_NULL_POINTER;
    }

    for (int i = 0; i < PAL_EVENT_MAX; i++) {
        eventDriver->modes[i] = mode;
    }
    return PAL_SUCCESS;
}

PalResult _PCALL palGetEventMode(
    PalEventDriver eventDriver, 
    PalEventType type, 
    PalEventMode* mode) {

    if (!eventDriver || !mode) {
        return PAL_ERROR_NULL_POINTER;
    }

    *mode = eventDriver->modes[type];
    return PAL_SUCCESS;
}

PalResult _PCALL palPushEvent(
    PalEventDriver eventDriver,
    PalEvent* event) {

    if (!eventDriver || !event) {
        return PAL_ERROR_NULL_POINTER;
    }

    // get the event mode
    PalEventMode mode = eventDriver->modes[event->type];
    if (mode == PAL_EVENT_MODE_CALLBACK) {
        if (eventDriver->callback) {
            eventDriver->callback(eventDriver->userData, event);
        }
        return PAL_SUCCESS;
    }

    if (mode == PAL_EVENT_MODE_POLL) {
        eventDriver->queue->push(eventDriver->queue, event);
    }
    return PAL_SUCCESS;
}

bool _PCALL palPollEvent(
    PalEventDriver eventDriver,
    PalEvent* outEvent) {

    if (!eventDriver || !outEvent) {
        return PAL_FALSE;
    }

    return eventDriver->queue->poll(
        eventDriver->queue,
        outEvent
    );
}