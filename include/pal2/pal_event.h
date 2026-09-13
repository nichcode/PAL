/**
 * @file pal_event.h
 * @brief This is the header file for PAL Event API.
 *
 * It defines all the types and functions of the event system.
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

/**
 * @defgroup pal_event Event
 */

/** @{ */

#ifndef _PAL_EVENT_H
#define _PAL_EVENT_H

#include "pal2/pal_core.h"

#define PAL_DECORATION_MODE_CLIENT_SIDE 0
#define PAL_DECORATION_MODE_SERVER_SIDE 1
#define PAL_DECORATION_MODE_COUNT 2

#define PAL_EVENT_TYPE_WINDOW_CLOSE 0
#define PAL_EVENT_TYPE_WINDOW_SIZE 1
#define PAL_EVENT_TYPE_WINDOW_MOVE 2
#define PAL_EVENT_TYPE_WINDOW_STATE 3
#define PAL_EVENT_TYPE_WINDOW_FOCUS 4
#define PAL_EVENT_TYPE_WINDOW_VISIBILITY 5
#define PAL_EVENT_TYPE_WINDOW_MODAL_BEGIN 6
#define PAL_EVENT_TYPE_WINDOW_MODAL_END 7
#define PAL_EVENT_TYPE_MONITOR_DPI_CHANGED 8
#define PAL_EVENT_TYPE_MONITOR_LIST_CHANGED 9
#define PAL_EVENT_TYPE_KEYDOWN 10
#define PAL_EVENT_TYPE_KEYREPEAT 11
#define PAL_EVENT_TYPE_KEYUP 12
#define PAL_EVENT_TYPE_MOUSE_BUTTONDOWN 13
#define PAL_EVENT_TYPE_MOUSE_BUTTONUP 14
#define PAL_EVENT_TYPE_MOUSE_MOVE 15
#define PAL_EVENT_TYPE_MOUSE_DELTA 16
#define PAL_EVENT_TYPE_MOUSE_WHEEL 17
#define PAL_EVENT_TYPE_USER 18
#define PAL_EVENT_TYPE_KEYCHAR 19
#define PAL_EVENT_TYPE_WINDOW_DECORATION_MODE 20
#define PAL_EVENT_TYPE_COUNT 21

#define PAL_DISPATCH_MODE_NONE 0
#define PAL_DISPATCH_MODE_CALLBACK 1
#define PAL_DISPATCH_MODE_POLL 2
#define PAL_DISPATCH_MODE_COUNT 3

/**
 * @struct PalEventDriver
 * @brief Opaque handle to an event driver.
 *
 * @since Added in version 2.0
 */
typedef struct PalEventDriver PalEventDriver;

/**
 * @struct PalEvent
 * @brief A single event.
 * 
 * The payloads are packed in the `data` and `data2` field of the struct.
 * 
 * User events defined how their payloads are packed.
 * 
 * Each event type determines how its information is packed. See 
 * @ref PalEventType for more information.
 * 
 * @since Added in version 2.0
 * @sa palPackInt32 
 * @sa palPackUint32 
 * @sa palPackFloat 
 * @sa palPackPointer 
 * @sa palUnpackInt32 
 * @sa palUnpackUint32
 * @sa palUnpackFloat 
 * @sa palUnpackPointer
 * 
 * @var PalEvent::data
 * The first payload. The data is defined by `PalEvent::type`.
 * 
 * @var PalEvent::data2
 * The second payload. The data is defined by `PalEvent::type`.
 * 
 * @var PalEvent::userId
 * An additional payload for `PAL_EVENT_TYPE_USER` events. 
 * This is not used by the other event types.
 * 
 * @var PalEvent::type
 * The event type. This defines how data is a laid out in `PalEvent::data`
 * and `PalEvent::data2`. User events are excluded since users defined the @nl
 * payload structure.
 */
typedef struct PalEvent PalEvent;

/**
 * @typedef PalDecorationMode
 * @brief Window decoration modes.
 * 
 * All values of this type follow the format `PAL_DECORATION_MODE_*` for API
 * consistency and ease of use.
 * 
 * @since Added in version 2.0
 * 
 * @def PAL_DECORATION_MODE_CLIENT_SIDE
 * Window decoration must be handled by the client.
 * 
 * @def PAL_DECORATION_MODE_SERVER_SIDE
 * Window decoration will be handled by the server.
 * 
 * @def PAL_DECORATION_MODE_COUNT
 * The number of decoration modes. The literal value must not be used.
 */
typedef uint32_t PalDecorationMode;

/**
 * @typedef PalEventType
 * @brief Event types.
 * 
 * All values of this type follow the format `PAL_EVENT_TYPE_*` for API
 * consistency and ease of use.
 * 
 * @since Added in version 2.0
 * 
 * @def PAL_EVENT_TYPE_WINDOW_CLOSE
 * See below for how to get the payload.
 * `event.data`: unused @nl
 * `event.data2`: window
 * 
 * @def PAL_EVENT_TYPE_WINDOW_SIZE
 * See below for how to get the payload.
 * `event.data`: bits 0-31 - width, bits 32-63 - height @nl
 * `event.data2`: window
 * 
 *  @def PAL_EVENT_TYPE_WINDOW_MOVE
 * See below for how to get the payload.
 * `event.data`: bits 0-31 - x, bits 32-63 - y @nl
 * `event.data2`: window
 * 
 * @def PAL_EVENT_TYPE_WINDOW_STATE
 * See below for how to get the payload.
 * `event.data`: bits 0-31 - state, bits 32-63 - unused @nl
 * `event.data2`: window
 * 
 * @def PAL_EVENT_TYPE_WINDOW_FOCUS
 * See below for how to get the payload.
 * `event.data`: bits 0-31 - focus, bits 32-63 - unused @nl
 * `event.data2`: window
 * 
 * @def PAL_EVENT_TYPE_WINDOW_VISIBILITY
 * See below for how to get the payload.
 * `event.data`: bits 0-31 - visibility, bits 32-63 - unused @nl
 * `event.data2`: window
 * 
 * @def PAL_EVENT_TYPE_WINDOW_MODAL_BEGIN
 * See below for how to get the payload.
 * `event.data`: unused @nl
 * `event.data2`: window
 * 
 * @def PAL_EVENT_TYPE_WINDOW_MODAL_END
 * See below for how to get the payload.
 * `event.data`: unused @nl
 * `event.data2`: window
 * 
 * @def PAL_EVENT_TYPE_MONITOR_DPI_CHANGED
 * See below for how to get the payload.
 * `event.data`: bits 0-31 - dpi, bits 32-63 - unused @nl
 * `event.data2`: window
 * 
 * @def PAL_EVENT_TYPE_MONITOR_LIST_CHANGED
 * See below for how to get the payload.
 * `event.data`: unused @nl
 * `event.data2`: window
 * 
 * @def PAL_EVENT_TYPE_KEYDOWN
 * See below for how to get the payload.
 * `event.data`: bits 0-31 - keycode, bits 32-63 - scancode @nl
 * `event.data2`: window
 * 
 * @def PAL_EVENT_TYPE_KEYREPEAT
 * See below for how to get the payload.
 * `event.data`: bits 0-31 - keycode, bits 32-63 - scancode @nl
 * `event.data2`: window
 * 
 * @def PAL_EVENT_TYPE_KEYUP
 * See below for how to get the payload.
 * `event.data`: bits 0-31 - keycode, bits 32-63 - scancode @nl
 * `event.data2`: window
 * 
 * @def PAL_EVENT_TYPE_MOUSE_BUTTONDOWN
 * See below for how to get the payload.
 * `event.data`: bits 0-31 - button, bits 32-63 - serial @nl
 * `event.data2`: window
 * 
 * @def PAL_EVENT_TYPE_MOUSE_BUTTONUP
 * See below for how to get the payload.
 * `event.data`: bits 0-31 - button, bits 32-63 - serial @nl
 * `event.data2`: window
 * 
 * @def PAL_EVENT_TYPE_MOUSE_MOVE
 * See below for how to get the payload.
 * `event.data`: bits 0-31 - x, bits 32-63 - y @nl
 * `event.data2`: window
 * 
 * @def PAL_EVENT_TYPE_MOUSE_DELTA
 * See below for how to get the payload.
 * `event.data`: bits 0-31 - dx, bits 32-63 - dy @nl
 * `event.data2`: window
 * 
 * @def PAL_EVENT_TYPE_MOUSE_WHEEL
 * See below for how to get the payload.
 * `event.data`: bits 0-31 - dx, bits 32-63 - dy @nl
 * `event.data2`: window
 * 
 * @def PAL_EVENT_TYPE_USER
 * User defines payload.
 * 
 * @def PAL_EVENT_TYPE_KEYCHAR
 * See below for how to get the payload.
 * `event.data`: bits 0-31 - codepoint, bits 32-63 - unused @nl
 * `event.data2`: window
 * 
 * @def PAL_EVENT_TYPE_WINDOW_DECORATION_MODE
 * See below for how to get the payload.
 * `event.data`: bits 0-31 - decoration mode, bits 32-63 - unused @nl
 * `event.data2`: window
 * 
 * @def PAL_EVENT_TYPE_COUNT
 * The number of event types. The literal value must not be used.
 */
typedef uint32_t PalEventType;

/**
 * @typedef PalDispatchMode
 * @brief Dispatch modes for an event.
 *
 * All values of this type follow the format `PAL_DISPATCH_MODE_*` for API
 * consistency and ease of use.
 * 
 * @since Added in version 2.0
 * 
 * @def PAL_DISPATCH_MODE_NONE
 * The event will be discarded.
 * 
 * @def PAL_DISPATCH_MODE_CALLBACK
 * The event will be push to the event callback.
 * 
 * @def PAL_DISPATCH_MODE_POLL
 * The event will be pushed to the event queue.
 * 
 * @def PAL_DISPATCH_MODE_COUNT
 * The number of dispatch modes. The literal value must not be used.
 */
typedef uint32_t PalDispatchMode;

/**
 * @typedef PalEventCallback
 * @brief Function pointer type used for event callbacks.
 * 
 * The event is only valid for the duration of the callback and must not be
 * modified or freed by the callback, the memory is owned by PAL.
 * 
 * The callback must be thread-safe if the event driver that uses it
 * is thread-safe.
 *
 * @param[in] userData User data passed from 
 * `PalEventDriverCreateInfo::userData`. Can be `nullptr`.
 * @param[in] event The event.
 *
 * @since Added in version 2.0
 * @sa PalPushFn
 */
typedef void(PAL_CALL* PalEventCallback)(
    void* userData,
    const PalEvent* event);

/**
 * @typedef PalPushFn
 * @brief Function pointer type used for pushing events.
 * 
 * This callback must respect the dispatch mode 
 * (eg.`PAL_DISPATCH_MODE_CALLBACK`) of the event and push the event 
 * accordingly. If the dispatch mode of the event is callback and the
 * event driver has no callback passed to it, the event must be discarded.
 * 
 * If the dispatch mode is `PAL_DISPATCH_MODE_NONE`, the event must be 
 * discarded.
 *
 * @param[in] userData User data passed from `PalEventQueue::userData`.
 * Can be `nullptr`.
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
 * The polled event is only valid for the duration of the callback and must
 * not be modified or freed by the callback, the memory is owned by PAL.
 *
 * The callback must return `PAL_FALSE` if the event queue is empty.
 * 
 * If the event queue is not empty and the event was retrieved, the callback
 * must return `PAL_TRUE`.
 *
 * @param[in] userData User data passed from `PalEventQueue::userData`.
 * Can be `nullptr`.
 * @param[out] event Pointer to the PalEvent to recieve the event.
 *
 * @since Added in version 2.0
 * @sa PalPushFn
 */
typedef PalBool(PAL_CALL* PalPollFn)(
    void* userData,
    PalEvent* event);

struct PalEvent 
{
    uint64_t data;
    uint64_t data2;
    uint32_t userId;
    PalEventType type;
};

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
 * The event queue to use for `PAL_DISPATCH_MODE_POLL` event pushes. Set to
 * `nullptr` to use the default. The default event queue is not thread-safe.
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
 * @param[out] eventDriver The created event driver.
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
 * @param[in] eventDriver The event driver to destroy.
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
 * @param[out] event The polled event.
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

/** @} */

#endif // _PAL_EVENT_H
