
#include "pal_pch.h"
#include "pal/pal_core.h"

void _PCALL palPushEvent(
    PalEventDriver* eventDriver,
    PalEvent* event) {

    if (!eventDriver || !event) {
        return;
    }

    if (!eventDriver->queue->push) {
        return;
    }

    if (eventDriver->mask & event->type) {
        eventDriver->queue->poll(eventDriver->queue->userData, event);
    }
}

bool _PCALL palPollEvent(
    PalEventDriver* eventDriver,
    PalEvent* outEvent) {

    if (!eventDriver || !outEvent) {
        return PAL_FALSE;
    }

    if (!eventDriver->queue->poll) {
        return PAL_FALSE;
    }

    return eventDriver->queue->poll(eventDriver->queue->userData, outEvent);
}


void _PCALL palTriggerEvent(
    PalEventDriver* eventDriver,
    PalEvent* event) {

    if (!eventDriver || !event) {
        return PAL_FALSE;
    }

    if (!eventDriver->callback) {
        return PAL_FALSE;
    }

    if (eventDriver->mask & event->type) {
        eventDriver->callback(eventDriver->userData, event);
    }
}