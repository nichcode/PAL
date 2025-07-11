
#include "pal_pch.h"
#include "pal_event_internal.h"

PalResult coalesce(
    PalEventType type)
{
    return PAL_RESULT_FAIL;
}

void palDefaultPushFunction(
    void* data, 
    PalEvent* event)
{
    PalEventData* queue = (PalEventData*)data;
    // check for coalesce event
    if (coalesce(event->type)) {
        for (int i = queue->tail - 1; i >= queue->head; --i) {
            PalEvent* e = &queue->data[i & PAL_MAX_EVENTS];
            if (e->type == event->type) {
                *e = *event;
                return;
            }
        }
    }
    queue->data[queue->tail++ & PAL_MAX_EVENTS] = *event;
}

PalResult palDefaultPollFunction(
    void* data, 
    PalEvent* event)
{
    PalEventData* queue = (PalEventData*)data;
    if (queue->head == queue->tail) {
        return PAL_RESULT_FAIL;
    }
    *event = queue->data[queue->head++ & PAL_MAX_EVENTS];    
    return PAL_RESULT_OK;
}

PalResult _PCALL palCreateEventInstance(
    PalEventInstanceDesc* desc, 
    PalEventInstance** outEventInstance)
{
    PalEventInstance* eventInstance = PAL_NULL;
    if (!outEventInstance || !desc) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return PAL_RESULT_FAIL;
    }

    if (desc->allocator) {
        if (!desc->allocator->alloc && !desc->allocator->free) {
            palSetError(PAL_ERROR_INVALID_ALLOCATOR);
            return PAL_RESULT_FAIL;
        }
    }

    eventInstance = palAlloc(desc->allocator, sizeof(PalEventInstance));
    if (!eventInstance) {
        palSetError(PAL_ERROR_ALLOCATION_FAILED);
        return PAL_RESULT_FAIL;
    }

    memset(eventInstance, 0, sizeof(PalEventInstance));
    if (desc->allocator) {
        eventInstance->allocator = desc->allocator;
    }

    // check for user event queue or allocate one
    if (desc->queue) {
        eventInstance->queue = desc->queue;
        eventInstance->free = PAL_FALSE;
    } else {

        // allocate the event queue and event data struct
        eventInstance->queue = palAlloc(desc->allocator, sizeof(PalEventQueue));
        if (!eventInstance->queue) {
            palSetError(PAL_ERROR_ALLOCATION_FAILED);
            return PAL_RESULT_FAIL;
        }
        memset(eventInstance->queue, 0, sizeof(PalEventQueue));

        eventInstance->eventData = palAlloc(desc->allocator, sizeof(PalEventData));
        if (!eventInstance->eventData) {
            palSetError(PAL_ERROR_ALLOCATION_FAILED);
            return PAL_RESULT_FAIL;
        }
        memset(eventInstance->eventData, 0, sizeof(PalEventData));

        // set functions
        eventInstance->queue->push = palDefaultPushFunction;
        eventInstance->queue->poll = palDefaultPollFunction;
        eventInstance->queue->userData = eventInstance->eventData;
        eventInstance->free = PAL_TRUE;
    }

    eventInstance->callback = desc->callback;
    *outEventInstance = eventInstance;
    return PAL_RESULT_OK;
}

void _PCALL palDestroyEventInstance(
    PalEventInstance* eventInstance)
{
    if (!eventInstance) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return;
    }
    
    PalAllocator* allocator = eventInstance->allocator;
    if (eventInstance->free) {
        palFree(allocator, eventInstance->queue);
        palFree(allocator, eventInstance->eventData);
    }
    palFree(allocator, eventInstance);
}

PalResult _PCALL palEnableEvent(
    PalEventInstance* eventInstance, 
    PalEventType type, 
    PalMode mode)
{
    if (!eventInstance) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return PAL_RESULT_FAIL;
    }

    eventInstance->modes[type] = mode;
    return PAL_RESULT_OK;
}

PalResult _PCALL palGetEventMode(
    PalEventInstance* eventInstance, 
    PalEventType type, 
    PalMode* mode)
{
    if (!eventInstance) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return PAL_RESULT_FAIL;
    }

    *mode = eventInstance->modes[type];
    return PAL_RESULT_OK;
}

PalResult _PCALL palPushEvent(
    PalEventInstance* eventInstance, 
    PalEvent* event)
{
    if (!eventInstance || !event) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return PAL_RESULT_FAIL;
    }

    // get the event mode to check if we poll or trigger the callback
    PalMode mode = eventInstance->modes[event->type];
    if (mode == PAL_MODE_CALLBACK) {
        if (eventInstance->callback) {
            eventInstance->callback(event);
        }
        return PAL_RESULT_OK;
    }

    // else poll to the event queue
    if (mode == PAL_MODE_POLL) {
        eventInstance->queue->push(eventInstance->eventData, event);
    }
}

PalResult _PCALL palPollEvent(
    PalEventInstance* eventInstance, 
    PalEvent* event)
{
    if (!eventInstance || !event) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return PAL_RESULT_FAIL;
    }

    return eventInstance->queue->poll(eventInstance->eventData, event);
}