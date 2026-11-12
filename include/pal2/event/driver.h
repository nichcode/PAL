
/**
 * @file driver.h
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

#ifndef PAL_EVENT_DRIVER_H
#define PAL_EVENT_DRIVER_H

#include "pal2/core/memory.h"
#include "queue.h"

/**
 * @defgroup dispatch_modes Event Dispatch Modes
 * @ingroup pal_event
 * @{
 */
#define PAL_DISPATCH_MODE_NONE 0 /**< Event will be discarded. */
#define PAL_DISPATCH_MODE_CALLBACK 1 /**< Event will be dispatched to the event callback. */
#define PAL_DISPATCH_MODE_POLL 2 /**< Event will be pushed to the event queue. */
#define PAL_DISPATCH_MODE_COUNT 3 /**< Number of dispatch modes. */
/** @} */

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
    const PalAllocator* allocator; /**< Allocator to use, or `nullptr` for the default. */
    PalEventQueue* queue; /**< Event queue to use, or `nullptr` for the default. */
    PalEventCallback callback; /**< Event callback, or `nullptr` to discard callback events. */
    void* userData; /**< User-defined data passed to the callback, or `nullptr`. */
} PalEventDriverCreateInfo;

/**
 * @brief Creates an event driver.
 *
 * The created event driver must be destroyed using @ref palDestroyEventDriver().
 *
 * This function creates an event driver using the specified creation parameters.
 * `info` must remain valid for the duration of this function. PAL does not
 * copy its contents.
 *
 * `info->allocator` and `info->queue` are not copied. The allocator and event queue
 * with any state referenced by them must remain valid until the event driver is
 * destroyed.
 *
 * This function creates the event driver and sets the dispatch mode of all
 * the event types to @ref PAL_DISPATCH_MODE_NONE. Change the dispatch mode
 * of an event type with @ref palSetEventDispatchMode().
 *
 * @param[in] info Event driver creation parameters.
 * @param[out] eventDriver Output handle to receive the created event driver.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `eventDriver` must be per thread.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 *
 * @sa palDestroyEventDriver
 */
PAL_API PalResult PAL_CALL palCreateEventDriver(
    const PalEventDriverCreateInfo* info,
    PalEventDriver** eventDriver);

/**
 * @brief Destroys an event driver.
 *
 * This function destroys the specified event driver. After this function is called,
 * the allocator and event queue referenced by the event driver can be freed if
 * they were provided when creating the event driver.
 *
 * @param[in] eventDriver Event driver to destroy.
 *
 * @thread-safety `eventDriver` must be externally synchronized.
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
 * @thread-safety `eventDriver` must be externally synchronized.
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
 * @return The dispatch mode on success or @ref PAL_DISPATCH_MODE_NONE on failure.
 *
 * @thread-safety Thread safe.
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
 * If the dispatch mode of `event` is @ref PAL_DISPATCH_MODE_NONE, the event
 * will be discarded.
 *
 * @param[in] eventDriver The event driver.
 * @param[in] event The event to push.
 *
 * @thread-safety The event queue or callback of `eventDriver` must be thread
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
 * returns `PAL_FALSE`. On success, the event is copied into the struct pointed to
 * by `event`, which is owned by the caller.
 *
 * @param[in] eventDriver Event driver.
 * @param[out] event Output struct to receive the event.
 * @return `PAL_TRUE` if an event was retrieved or `PAL_FALSE` if the queue is empty.
 *
 * @thread-safety The event queue of `eventDriver` must be thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 *
 * @sa palPushEvent
 */
PAL_API PalBool PAL_CALL palPollEvent(
    PalEventDriver* eventDriver,
    PalEvent* event);

#endif // PAL_EVENT_DRIVER_H