
/**
 * @file queue.h
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

#ifndef PAL_EVENT_QUEUE_H
#define PAL_EVENT_QUEUE_H

#include "event.h"

/**
 * @brief Number of events the default queue can hold at a time.
 * @ingroup pal_event
 */
#define PAL_DEFAULT_QUEUE_EVENT_COUNT 512

/**
 * @typedef PalPushFn
 * @brief Queue push function.
 *
 * If the dispatch mode for `event->type` is @ref PAL_DISPATCH_MODE_NONE or
 * @ref PAL_DISPATCH_MODE_CALLBACK, the event must be discarded.
 * Otherwise, `event` must be copied by the function; it is not guaranteed to
 * remain valid after the function returns.
 *
 * The function signature should look like this:
 * @code
 * void PAL_CALL queuePush(void* userData, PalEvent* event);
 * @endcode
 *
 * @param[in] userData User-defined data passed to the function, or `nullptr`.
 * @param[in] event Pointer to the event to push.
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 *
 * @sa PalEventCallback
 */
typedef void(PAL_CALL* PalPushFn)(
    void* userData,
    PalEvent* event);

/**
 * @typedef PalPollFn
 * @brief Queue poll function.
 *
 * The function must get the next available event from the queue and return
 * `PAL_TRUE` if an event was found or `PAL_FALSE` if the queue is empty. This
 * function must not block or wait for the queue to process new events; it must
 * return immediately.
 *
 * The function signature should look like this:
 * @code
 * PalBool PAL_CALL queuePoll(void* userData, PalEvent* event);
 * @endcode
 *
 * @param[in] userData User-defined data passed to the function, or `nullptr`.
 * @param[out] event Output struct to receive the event.
 * @return `PAL_TRUE` if an event was found or `PAL_FALSE` if the queue is empty.
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 *
 * @sa PalPushFn
 */
typedef PalBool(PAL_CALL* PalPollFn)(
    void* userData,
    PalEvent* event);

/**
 * @struct PalEventQueue
 * @brief Event queue.
 *
 * The event queue must be thread-safe if the event driver that uses it will be
 * called concurrently from multiple threads. This only applies to
 * @ref PAL_DISPATCH_MODE_POLL events.
 *
 * The event queue will not be copied, so it must remain valid for as long
 * as the event driver uses it. The default event queue is not thread-safe
 * and can only contain @ref PAL_DEFAULT_QUEUE_EVENT_COUNT events at a time.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 */
typedef struct PalEventQueue {
    PalPushFn push; /**< Push function. */
    PalPollFn poll; /**< Poll function. */
    void* userData; /**< User-defined data passed to the functions or `nullptr`. */
} PalEventQueue;

#endif // PAL_EVENT_QUEUE_H