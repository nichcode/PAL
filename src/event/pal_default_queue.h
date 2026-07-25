
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_DEFAULT_QUEUE_H
#define _PAL_DEFAULT_QUEUE_H

#include "pal/pal_event.h"

#define MAX_EVENTS 512

PalEventQueue* createDefaultEventQueue(const PalAllocator* allocator);

void destroyDefaultEventQueue(
    const PalAllocator* allocator,
    PalEventQueue* queue);

#endif // _PAL_DEFAULT_QUEUE_H