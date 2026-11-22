/**
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

#include "pal2/core/memory.h"
#include "pal2/event/queue.h"
#include <string.h>

typedef struct QueueData
{
    uint32_t head;
    uint32_t tail;
    PalEvent data[PAL_DEFAULT_QUEUE_EVENT_COUNT];
} QueueData;

typedef struct DefaultEventQueue
{
    PalEventQueue interface;
    QueueData data;
} DefaultEventQueue;

static void PAL_CALL eventPush(
    void* userData,
    const PalEvent* event)
{
    QueueData* data = userData;

    if ((data->tail - data->head) >= PAL_DEFAULT_QUEUE_EVENT_COUNT) {
        data->head++;
    }

    data->data[data->tail++ % PAL_DEFAULT_QUEUE_EVENT_COUNT] = *event;
}

static PalBool PAL_CALL eventPoll(
    void* userData,
    PalEvent* outEvent)
{
    QueueData* data = userData;
    if (data->head == data->tail) {
        return PAL_FALSE;
    }

    *outEvent = data->data[data->head++ % PAL_DEFAULT_QUEUE_EVENT_COUNT];
    return PAL_TRUE;
}

PalEventQueue* createDefaultQueue(const PalAllocator* allocator)
{
    DefaultEventQueue* queue = palAllocate(allocator, sizeof(DefaultEventQueue), 0);
    if (!queue) {
        return nullptr;
    }

    memset(queue, 0, sizeof(DefaultEventQueue));
    queue->interface.userData = &queue->data;
    queue->interface.poll = eventPoll;
    queue->interface.push = eventPush;

    return &queue->interface;
}

void destroyDefaultQueue(
    const PalAllocator* allocator,
    PalEventQueue* queue)
{
    palFree(allocator, queue);
}