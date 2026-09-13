
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

typedef struct QueueData
{
    uint32_t head;
    uint32_t tail;
    PalEvent data[PAL_DEFAULT_QUEUE_EVENT_COUNT];
} QueueData;

static void PAL_CALL eventPush(
    void* queue,
    PalEvent* event)
{
    PalEventQueue* eventQueue = queue;
    QueueData* data = eventQueue->userData;
    data->data[data->tail++ % PAL_DEFAULT_QUEUE_EVENT_COUNT] = *event;
}

static PalBool PAL_CALL eventPoll(
    void* queue,
    PalEvent* outEvent)
{
    PalEventQueue* eventQueue = queue;
    QueueData* data = eventQueue->userData;
    if (data->head == data->tail) {
        return PAL_FALSE;
    }

    *outEvent = data->data[data->head++ % PAL_DEFAULT_QUEUE_EVENT_COUNT];
    return PAL_TRUE;
}

PalEventQueue* createDefaultQueue(const PalAllocator* allocator)
{
    PalEventQueue* queue = nullptr;
    queue = palAllocate(allocator, sizeof(PalEventQueue), 0);
    if (!queue) {
        return nullptr;
    }

    QueueData* queueData = nullptr;
    queueData = palAllocate(allocator, sizeof(QueueData), 0);
    if (!queueData) {
        palFree(allocator, queue);
        return nullptr;
    }

    memset(queueData, 0, sizeof(QueueData));
    queue->userData = queueData;
    queue->poll = eventPoll;
    queue->push = eventPush;

    return queue;
}

void destroyDefaultQueue(
    const PalAllocator* allocator,
    PalEventQueue* queue)
{
    palFree(allocator, queue->userData);
    palFree(allocator, queue);
}