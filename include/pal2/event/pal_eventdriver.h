/**
 * @file pal_eventdriver.h
 * @brief This is the header file for PAL Event Driver API.
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

#ifndef PAL_EVENTDRIVER_H
#define PAL_EVENTDRIVER_H

#include "pal2/core/pal_result.h"
#include "pal2/core/pal_memory.h"
#include "pal_eventtypes.h"

#define PAL_DEFAULT_QUEUE_EVENT_COUNT 512

/**
 * @defgroup dispatch_modes Dispatch Modes
 * @brief Dispatch modes for events.
 * 
 * `PAL_DISPATCH_MODE_NONE` Disables the event. @nl
 * `PAL_DISPATCH_MODE_CALLBACK` Dispatches the event to the callback. @nl
 * `PAL_DISPATCH_MODE_POLL` Dispatches the event to the queue. @nl
 * 
 * @{
 */
#define PAL_DISPATCH_MODE_NONE 0
#define PAL_DISPATCH_MODE_CALLBACK 1
#define PAL_DISPATCH_MODE_POLL 2
#define PAL_DISPATCH_MODE_COUNT 3
/** @} */

/**
 * @typedef PalDispatchMode
 * @brief Dispatch modes for an event.
 *
 * All values of this type follow the format `PAL_DISPATCH_MODE_*` for API
 * consistency and ease of use.
 * 
 * @since Added in version 2.0
 */
typedef uint32_t PalDispatchMode;

/**
 * @struct PalEventDriver
 * @brief Opaque handle to an event driver.
 *
 * @since Added in version 2.0
 */
typedef struct PalEventDriver PalEventDriver;

/**
 * @typedef PalPushFn
 * @brief Function pointer type used for pushing events.
 * 
 * The function signature should look like this:
 * @code
 * void PAL_CALL queuePush(void* userData, PalEvent* event);
 * @endcode
 * 
 * This callback must respect the dispatch mode 
 * (eg.`PAL_DISPATCH_MODE_CALLBACK`) of the event and push the event 
 * accordingly. If the dispatch mode of the event is callback and the
 * event driver has no callback passed to it, the event must be discarded.
 * 
 * If the dispatch mode is `PAL_DISPATCH_MODE_NONE`, the event must be 
 * discarded.
 *
 * @param[in] userData User data passed from the queue. Can be `nullptr`.
 * @param[in] event Pointer to the event to push. This will always be valid.
 *
 * @since Added in version 2.0
 * @sa PalEventCallback
 */
typedef void(PAL_CALL* PalPushFn)(
    void* userData,
    PalEvent* event);

/**
 * @typedef PalPollFn
 * @brief Function pointer type used for polling events from event queues.
 * 
 * The function signature should look like this:
 * @code
 * PalBool PAL_CALL queuePoll(void* userData, PalEvent* event);
 * @endcode
 * 
 * The polled event is only valid for the duration of the callback and must
 * not be modified or freed by the callback, the memory is owned by PAL.
 *
 * The callback must return `PAL_FALSE` if the event queue is empty.
 * 
 * If the event queue is not empty and the event was retrieved, the callback
 * must return `PAL_TRUE`.
 *
 * @param[in] userData User data passed from the queue. Can be `nullptr`.
 * @param[out] event The output struct to recieve the event.
 *
 * @since Added in version 2.0
 * @sa PalPushFn
 */
typedef PalBool(PAL_CALL* PalPollFn)(
    void* userData,
    PalEvent* event);

/**
 * @struct PalEventQueue
 * @brief Contains information about an event queue.
 * 
 * This struct provides a way to use custom event queues with PAL.
 * If the event driver that will use the event queue will be called
 * from multiple threads with `PAL_DISPATCH_MODE_POLL`, the event queue
 * must be thread-safe.
 * 
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalEventQueue::push
 * The push function of the event queue. Must not be `nullptr`.
 * 
 * @var PalEventQueue::poll
 * The poll function of the event queue. Must not be `nullptr`.
 * 
 * @var PalEventQueue::userData
 * User data passed to push and poll function. Can be `nullptr`.
 */
typedef struct PalEventQueue 
{
    PalPushFn push;
    PalPollFn poll;
    void* userData;
} PalEventQueue;

/**
 * @struct PalEventDriverCreateInfo
 * @brief Contains creation parameters of an event driver.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalEventDriverCreateInfo::allocator
 * The allocator the event driver should use. Set to `nullptr` to use the
 * thread-safe default.
 * 
 * @var PalEventDriverCreateInfo::queue
 * The event queue to use for `PAL_DISPATCH_MODE_POLL` event pushes. 
 * Set to `nullptr` to use the non thread-safe default. @nl
 * `PAL_DEFAULT_QUEUE_EVENT_COUNT` is the maximum events the default queue can
 * contain.
 * 
 * @var PalEventDriverCreateInfo::callback
 * The event callback to use for `PAL_DISPATCH_MODE_CALLBACK` event pushes.
 * If `nullptr`, the event will be discarded if its callback dispatch mode.
 * 
 * @var PalEventDriverCreateInfo::userData
 * User data passed to callback function. Can be `nullptr`.
 */
typedef struct PalEventDriverCreateInfo
{
    const PalAllocator* allocator;
    PalEventQueue* queue;
    PalEventCallback callback;
    void* userData;
} PalEventDriverCreateInfo;

/**
 * @brief Creates an event driver.
 * 
 * `PalEventDriverCreateInfo::allocator` and `PalEventDriverCreateInfo::queue`
 * will not be copied, therefore the pointers must remain valid until the
 * event driver is destroyed if they were provided.
 * 
 * After the event driver is created, all event dispatch modes are set
 * to `PAL_DISPATCH_MODE_NONE`. To recieve events, the corresponding type 
 * must be enabled with `palSetEventDispatchMode()`.
 * 
 * Destroy the event driver with `palDestroyEventDriver()` when no longer 
 * needed.
 *
 * @param[in] info Information about how to create the event driver.
 * @param[out] eventDriver The output handle to recieve the created 
 * event driver.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 * 
 * @Thread-safety `eventDriver` must be per thread and 
 * `PalEventDriverCreateInfo::allocator` must be thread-safe.
 *
 * @since Added in version 2.0
 * @sa palDestroyEventDriver
 */
PAL_API PalResult PAL_CALL palCreateEventDriver(
    const PalEventDriverCreateInfo* info,
    PalEventDriver** eventDriver);

/**
 * @brief Destroys an event driver.
 *
 * @param[in] eventDriver The event driver.
 *
 * @Thread-safety `eventDriver` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateEventDriver
 */
PAL_API void PAL_CALL palDestroyEventDriver(PalEventDriver* eventDriver);

/**
 * @brief Sets the dispatch mode for an event type.
 *
 * If the dispatch mode is `PAL_DISPATCH_MODE_POLL`, the event will be 
 * dispatched to the event queue.
 * 
 * If the dispatch mode is `PAL_DISPATCH_MODE_CALLBACK`, the event will be
 * dispatched to the event callback. The event will be discared if
 * the event callback is not valid.
 *
 * @param[in] eventDriver The event driver.
 * @param[in] type Event type to set dispatch mode for.
 * @param[in] mode Dispatch mode to use.
 *
 * @Thread-safety `eventDriver` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palGetEventDispatchMode
 */
PAL_API void PAL_CALL palSetEventDispatchMode(
    PalEventDriver* eventDriver,
    PalEventType type,
    PalDispatchMode mode);

/**
 * @brief Gets the dispatch mode for an event type.
 *
 * @param[in] eventDriver The event driver.
 * @param[in] type The event type.
 *
 * @return The dispatch mode on success or `PAL_DISPATCH_MODE_NONE` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palSetEventDispatchMode
 */
PAL_API PalDispatchMode PAL_CALL palGetEventDispatchMode(
    PalEventDriver* eventDriver,
    PalEventType type);

/**
 * @brief Pushes an event.
 * 
 * If the dispatch mode of the event is `PAL_DISPATCH_MODE_POLL`, the event 
 * will be dispatched to the event queue.
 * 
 * If the dispatch mode of the event is `PAL_DISPATCH_MODE_CALLBACK`, the event 
 * will be dispatched to the event callback. The event will be discared if
 * the event callback is not valid.
 *
 * @param[in] eventDriver The event driver.
 * @param[in] event The event to push.
 *
 * @Thread-safety The event queue or event callback of `eventDriver` must 
 * be thread safe with respect to the dispatch mode of `event`. 
 *
 * @since Added in version 2.0
 * @sa palPollEvent
 */
PAL_API void PAL_CALL palPushEvent(
    PalEventDriver* eventDriver,
    PalEvent* event);

/**
 * @brief Retrieves the next available event from the queue.
 *
 * This function retrieves the next pending event from the queue of 
 * `eventDriver` without blocking. If no events are available, it
 * returns `PAL_FALSE`.
 * 
 * The polled event must not be modified or freed by the caller, The
 * memory is owned by PAL.
 *
 * @param[in] eventDriver The event driver.
 * @param[out] event The output struct to recieve the polled event.
 * 
 * @return `PAL_TRUE` if the event was polled or `PAL_FALSE`.
 *
 * @Thread-safety The event queue of `eventDriver` must be thread safe.
 *
 * @since Added in version 2.0
 * @sa palPushEvent
 */
PAL_API PalBool PAL_CALL palPollEvent(
    PalEventDriver* eventDriver,
    PalEvent* event);

#endif // PAL_EVENTDRIVER_H