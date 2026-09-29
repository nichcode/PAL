/**
 * @brief This is the header file for PAL Event API.
 *
 * It defines all the types and functions of the event module.
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

#ifndef PAL_EVENT_H
#define PAL_EVENT_H

#include "pal_core.h"

/**
 * @defgroup pal_event Event Module
 * @{
 */

#define PAL_DEFAULT_QUEUE_EVENT_COUNT 512 /**< maximum number of default queue events */

/**
 * @defgroup decoration_modes Decoration Modes
 * @{
 */
#define PAL_DECORATION_MODE_CLIENT_SIDE 0 /**< client is responsible for decoration */
#define PAL_DECORATION_MODE_SERVER_SIDE 1 /**< server is responsible for decoration */
#define PAL_DECORATION_MODE_COUNT 2 /**< number of decoration modes */
/** @} */

/**
 * @defgroup event_types Event Types
 * @{
 */
#define PAL_EVENT_TYPE_WINDOW_CLOSE 0 /**< window close button has been clicked */
#define PAL_EVENT_TYPE_WINDOW_SIZE 1 /**< window has been resized */
#define PAL_EVENT_TYPE_WINDOW_MOVE 2 /**< window has been moved */
#define PAL_EVENT_TYPE_WINDOW_STATE 3 /**< window state has changed */
#define PAL_EVENT_TYPE_WINDOW_FOCUS 4 /**< window has gained or lost focus */
#define PAL_EVENT_TYPE_WINDOW_VISIBILITY 5 /**< window has been shown or hidden */
#define PAL_EVENT_TYPE_WINDOW_MODAL_BEGIN 6 /**< window has entered model mode */
#define PAL_EVENT_TYPE_WINDOW_MODAL_END 7 /**< window has exited model mode */
#define PAL_EVENT_TYPE_MONITOR_DPI_CHANGED 8 /**< monitor DPI has changed */
#define PAL_EVENT_TYPE_MONITOR_LIST_CHANGED 9 /**< number of connected monitors has changed */
#define PAL_EVENT_TYPE_KEYDOWN 10 /**< key was pressed */
#define PAL_EVENT_TYPE_KEYREPEAT 11 /**< key is being held down */
#define PAL_EVENT_TYPE_KEYUP 12 /**< key has been released */
#define PAL_EVENT_TYPE_MOUSE_BUTTONDOWN 13 /**< mouse button has been pressed */
#define PAL_EVENT_TYPE_MOUSE_BUTTONUP 14 /**< mouse button has been released */
#define PAL_EVENT_TYPE_MOUSE_MOVE 15 /**< mouse was moved */
#define PAL_EVENT_TYPE_MOUSE_DELTA 16 /**< mouse movement delta has changed */
#define PAL_EVENT_TYPE_MOUSE_WHEEL 17 /**< mouse wheel delta has changed */
#define PAL_EVENT_TYPE_USER 18 /**< user event */
#define PAL_EVENT_TYPE_KEYCHAR 19 /**< character key has been pressed */
#define PAL_EVENT_TYPE_WINDOW_DECORATION_MODE 20 /**< window decoration mode has been selected */
#define PAL_EVENT_TYPE_COUNT 21 /**< number of event types */
/** @} */

/**
 * @defgroup dispatch_modes Dispatch Modes
 * @{
 */
#define PAL_DISPATCH_MODE_NONE 0 /**< event will be discarded */
#define PAL_DISPATCH_MODE_CALLBACK 1 /**< event will be dispatched to the callback */
#define PAL_DISPATCH_MODE_POLL 2 /**< event will be dispatched to the queue */
#define PAL_DISPATCH_MODE_COUNT 3 /**< number of dispatch modes */
/** @} */

/**
 * @typedef PalDecorationMode
 * @brief Decoration mode.
 * 
 * All values of this type follow the format `PAL_DECORATION_MODE_*` for API
 * consistency and ease of use.
 * 
 * @since Added in version 2.0
 */
typedef uint32_t PalDecorationMode;

/**
 * @typedef PalEventType
 * @brief Event type.
 * 
 * All values of this type follow the format `PAL_EVENT_TYPE_*` for API
 * consistency and ease of use.
 * 
 * @since Added in version 2.0
 */
typedef uint32_t PalEventType;

/**
 * @typedef PalDispatchMode
 * @brief Dispatch mode.
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
/** @} */

/**
 * @struct PalEvent
 * @brief Event.
 * 
 * The payloads are packed in the `data` and `data2` fields of the struct.
 * User events define how their payloads are packed.
 * See [Event Guide](@ref event_guide) for more information.
 * 
 * @since Added in version 2.0
 * @ingroup pal_event
 */
typedef struct PalEvent {
    uint64_t data; /**< data payload */
    uint64_t data2; /**< additional data payload */
    uint32_t userId; /**< id for user events */
    PalEventType type; /**< event type */
} PalEvent;

/**
 * @typedef PalEventCallback
 * @brief Event callback function.
 * 
 * `event` is only valid for the duration of the callback and must not be modified
 * or freed by the callback, the memory is owned by PAL. The callback must be thread-safe
 * if the event driver that uses it will be called from multiple threads.
 * 
 * The function signature should look like this:
 * @code
 * void PAL_CALL eventCallback(void* userData, const PalEvent* event);
 * @endcode
 * 
 * @param[in] userData User-defined data passed to the callback or `nullptr`.
 * @param[in] event Pushed event.
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 * 
 * @sa PalPushFn
 */
typedef void(PAL_CALL* PalEventCallback)(
    void* userData,
    const PalEvent* event);

/**
 * @typedef PalPushFn
 * @brief Queue push function.
 * 
 * If the dispatch mode for `event->type` is @ref PAL_DISPATCH_MODE_NONE or 
 * @ref PAL_DISPATCH_MODE_CALLBACK, the event must be discarded.
 * `event` must be copied by the callback.
 * 
 * The function signature should look like this:
 * @code
 * void PAL_CALL queuePush(void* userData, PalEvent* event);
 * @endcode
 *
 * @param[in] userData User-defined data passed to the function or `nullptr`.
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
 * `PAL_TRUE` if an event was found or `PAL_FALSE` if the queue is empty or no
 * event was found. This function must not block or wait for the queue to
 * process new events, it must return immediately.
 * 
 * The function signature should look like this:
 * @code
 * PalBool PAL_CALL queuePoll(void* userData, PalEvent* event);
 * @endcode
 *
 * @param[in] userData User-defined data passed to the function or `nullptr`.
 * @param[out] event Output struct to recieve the event.
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
 * called concurrently from multiple threads. This only applys to 
 * @ref PAL_DISPATCH_MODE_POLL events.
 * 
 * The event queue will not be copied, so it must remain valid for as long
 * as the event driver uses it. The default event queue is not thread safe
 * and can only contain @ref PAL_DEFAULT_QUEUE_EVENT_COUNT events at a time.
 * 
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 */
typedef struct PalEventQueue {
    PalPushFn push; /**< push function */
    PalPollFn poll; /**< poll function */
    void* userData; /**< user-defined data passed to the functions or `nullptr` */
} PalEventQueue;

/**
 * @struct PalEventDriverCreateInfo
 * @brief Event driver creation parameters.
 * 
 * This struct is used only during @ref palCreateEventDriver() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 */
typedef struct PalEventDriverCreateInfo {
    const PalAllocator* allocator; /**< allocator to use or `nullptr` for default */
    PalEventQueue* queue; /**< event queue to use or `nullptr` for default. */
    PalEventCallback callback; /**< event callback or `nullptr` */
    void* userData; /**< user-defined data passed to the callback or `nullptr` */
} PalEventDriverCreateInfo;

/**
 * @brief Creates an event driver.
 * 
 * This function creates an event driver using the specified creation parameters.
 * `info` must remain valid for the duration of this function. PAL does not
 * copy the its contents.
 * 
 * `info->allocator` and `info->queue` are not copied. The allocator and event queue
 * with any state referenced by them must remain valid until the event driver is
 * destroyed.
 * 
 * This function creates the event driver and sets the dispatch mode of all
 * the event types to @ref PAL_DISPATCH_MODE_NONE. Change the dispatch mode
 * of an event type with @ref palSetEventDispatchMode().
 * 
 * The created event driver is owned by PAL an must be destroyed with 
 * @ref palDestroyEventDriver().
 *
 * @param[in] info Event driver creation parameters.
 * @param[out] eventDriver Output handle to recieve the created event driver.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult for more information.
 * 
 * @Thread-safety `eventDriver` must be per thread.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 */
PAL_API PalResult PAL_CALL palCreateEventDriver(
    const PalEventDriverCreateInfo* info,
    PalEventDriver** eventDriver);

/**
 * @brief Destroys an event driver.
 * 
 * This function destroys the specified event driver. After this function is called,
 * the allocator and event queue reference by the event driver can be freed if 
 * they were provided when creating the event driver.
 *
 * @param[in] eventDriver Event driver to destroy.
 *
 * @Thread-safety `eventDriver` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 * 
 * @sa palCreateEventDriver
 */
PAL_API void PAL_CALL palDestroyEventDriver(PalEventDriver* eventDriver);

/**
 * @brief Sets the dispatch mode for an event type.
 *
 * @param[in] eventDriver Event driver to set dispatch mode for.
 * @param[in] type Event type to set dispatch mode for.
 * @param[in] mode Dispatch mode to use.
 *
 * @Thread-safety `eventDriver` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 * 
 * @sa palGetEventDispatchMode
 */
PAL_API void PAL_CALL palSetEventDispatchMode(
    PalEventDriver* eventDriver,
    PalEventType type,
    PalDispatchMode mode);

/**
 * @brief Gets the dispatch mode for an event type.
 *
 * @param[in] eventDriver Event driver.
 * @param[in] type Event type.
 * @return The dispatch mode on success or @ref PAL_DISPATCH_MODE_NONE
 *         on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 * 
 * @sa palSetEventDispatchMode
 */
PAL_API PalDispatchMode PAL_CALL palGetEventDispatchMode(
    PalEventDriver* eventDriver,
    PalEventType type);

/**
 * @brief Pushes an event.
 * 
 * This function pushes an event to the event callback or event queue with
 * respect to the dispatch mode of the event. If the dispatch mode
 * of `event` is @ref PAL_DISPATCH_MODE_POLL, the event will be pushed
 * to the event queue. The event will be copied into the queue.
 * 
 * If the dispatch mode of `event` is @ref PAL_DISPATCH_MODE_CALLBACK,
 * the event will be dispatched to the event callback. The event will be
 * discarded if the event callback is `nullptr`. The event will not be copied
 * and must remain valid for the duration of the callback.
 *
 * @param[in] eventDriver The event driver.
 * @param[in] event The event to push.
 *
 * @Thread-safety The event queue or callback of `eventDriver` must be thread
 *                safe with respect to the dispatch mode of `event`. 
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 * 
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
 * returns `PAL_FALSE`. The `event` must not be modified or freed by the 
 * caller, The memory is owned by PAL.
 *
 * @param[in] eventDriver Event driver.
 * @param[out] event Output struct to recieve the event.
 * @return `PAL_TRUE` if the event was polled or `PAL_FALSE`.
 *
 * @Thread-safety The event queue of `eventDriver` must be thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 * 
 * @sa palPushEvent
 */
PAL_API PalBool PAL_CALL palPollEvent(
    PalEventDriver* eventDriver,
    PalEvent* event);

#endif // PAL_EVENT_H
