
/**

Copyright (C) 2025 Nicholas Agbo

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

/**
 * @defgroup pal_event Event
 * Event PAL functionality such as queues, event drivers, and event callbacks.
 *
 * @{
 */

#ifndef _PAL_EVENT_H
#define _PAL_EVENT_H

#include "pal_core.h"

/**
 * @struct PalEventDriver
 * @brief Opaque handle to an event driver.
 *
 * @since 1.0
 * @ingroup pal_event
 */
typedef struct PalEventDriver PalEventDriver;

/**
 * @struct PalEvent
 * @brief A single event.
 *
 * @since 1.0
 * @ingroup pal_event
 */
typedef struct PalEvent PalEvent;

/**
 * @typedef PalEventCallback
 * @brief Function pointer type used for event callbacks.
 *
 * @param[in] userData Optional pointer to user data. Can be nullptr.
 * @param[in] event The event.
 *
 * @since 1.0
 * @ingroup pal_event
 * @sa PalPushFn
 */
typedef void(PAL_CALL* PalEventCallback)(
    void* userData,
    const PalEvent* event);

/**
 * @typedef PalPushFn
 * @brief Function pointer type used for pushing events.
 *
 * @param[in] userData Optional pointer to user data. Can be nullptr.
 * @param[in] event The event to push.
 *
 * @since 1.0
 * @ingroup pal_event
 * @sa PalEventCallback
 */
typedef void(PAL_CALL* PalPushFn)(
    void* userData,
    PalEvent* event);

/**
 * @typedef PalPollFn
 * @brief Function pointer type used for polling events from event queues.
 *
 * This function should return false if the event queue is empty.
 * If the queue is not empty and the event was retrieved, this should return
 * true.
 *
 * @param[in] userData Optional pointer to user data. Can be nullptr.
 * @param[out] event Pointer to recieve the event.
 *
 * @since 1.0
 * @ingroup pal_event
 * @sa PalPushFn
 */
typedef bool(PAL_CALL* PalPollFn)(
    void* userData,
    PalEvent* outEvent);

/**
 * @enum PalEventType
 * @brief Event types. This is not a bitmask enum.
 *
 * All event types follow the format `PAL_EVENT_**` for consistency and
 * API use.
 *
 * @since 1.0
 * @ingroup pal_event
 */
typedef enum {
    PAL_EVENT_WINDOW_CLOSE, /** < Window close button.*/
    PAL_EVENT_WINDOW_SIZE,
    PAL_EVENT_WINDOW_MOVE,
    PAL_EVENT_WINDOW_STATE,       /** < (minimized, maximized, restored).*/
    PAL_EVENT_WINDOW_FOCUS,       /** < True for focus gained.*/
    PAL_EVENT_WINDOW_VISIBILITY,  /** < True for visible.*/
    PAL_EVENT_WINDOW_MODAL_BEGIN, /** < WM_ENTERSIZEMOVE (Windows Only).*/
    PAL_EVENT_WINDOW_MODAL_END,   /** < WM_EXITSIZEMOVE. (Windows Only).*/
    PAL_EVENT_MONITOR_DPI_CHANGED,
    PAL_EVENT_MONITOR_LIST_CHANGED, /** < Monitor list changed.*/
    PAL_EVENT_KEYDOWN,
    PAL_EVENT_KEYREPEAT,
    PAL_EVENT_KEYUP,
    PAL_EVENT_MOUSE_BUTTONDOWN,
    PAL_EVENT_MOUSE_BUTTONUP,
    PAL_EVENT_MOUSE_MOVE,
    PAL_EVENT_MOUSE_DELTA, /** < Mouse movement delta.*/
    PAL_EVENT_MOUSE_WHEEL,
    PAL_EVENT_USER,
    PAL_EVENT_MAX
} PalEventType;

/**
 * @enum PalDispatchMode
 * @brief Dispatch types for an event. This is not a bitmask enum.
 *
 * All dispatch modes follow the format `PAL_DISPATCH_**` for consistency
 * and API use.
 *
 * @since 1.0
 * @ingroup pal_event
 */
typedef enum {
    PAL_DISPATCH_NONE,     /** < No dispatch.*/
    PAL_DISPATCH_CALLBACK, /** < Dispatch to event callback.*/
    PAL_DISPATCH_POLL,     /** < Dispatch to the event queue.*/
    PAL_DISPATCH_MAX
} PalDispatchMode;

struct PalEvent {
    PalEventType type;
    Int64 data;   /** < First data payload.*/
    Int64 data2;  /** < Second data payload.*/
    Int64 userId; /** You can have user events upto Int64 max.*/
};

/**
 * @struct PalEventQueue
 * @brief Custom event queue.
 *
 * Provides user-defined event queue push and poll functions.
 *
 * @since 1.0
 * @ingroup pal_event
 */
typedef struct {
    PalPushFn push;
    PalPollFn poll;
    void* userData; /** < Optional user-provided data. Can be nullptr.*/
} PalEventQueue;

/**
 * @struct PalEventDriverCreateInfo
 * @brief Creation parameters for an event driver.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.0
 * @ingroup pal_event
 */
typedef struct {
    const PalAllocator* allocator; /** < Set to nullptr to use default.*/
    PalEventQueue* queue;          /** < Set to nullptr to use default.*/
    PalEventCallback callback;     /** < Can be nullptr.*/
    void* userData; /** < Optional user-provided data. Can be nullptr.*/
} PalEventDriverCreateInfo;

/**
 * @brief Create an event driver.
 *
 * The allocator field in the provided PalEventDriverCreateInfo struct will not
 * be copied, so the pointer must remain valid until the event driver is
 * destroyed. Destroy the event driver with palDestroyEventDriver() when no
 * longer needed.
 *
 * @param[in] info Pointer to a PalEventDriverCreateInfo struct that specifies
 * paramters.
 * @param[out] outEventDriver Pointer to a PalEventDriver to recieve the created
 * event driver.
 *
 * @return PAL_RESULT_SUCCESS on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * @note This function may be called from any thread, but not concurrently.
 * Users must ensure synchronization.
 *
 * @sa PalEventDriverCreateInfo
 * @sa palDestroyEventDriver()
 *
 * @since 1.0
 * @ingroup pal_event
 */
PAL_API PalResult PAL_CALL palCreateEventDriver(
    const PalEventDriverCreateInfo* info,
    PalEventDriver** outEventDriver);

/**
 * @brief Destroy the event driver.
 *
 * This function can be called multiple times without any undefined behavior.
 * If the event driver is invalid or nullptr, this function returns silently.
 *
 * @param[in] eventDriver Pointer to the event driver to destroy.
 *
 * @note This function may be called from any thread, but not concurrently.
 * Users must ensure synchronization.
 *
 * @sa palCreateEventDriver()
 *
 * @since 1.0
 * @ingroup pal_event
 */
PAL_API void PAL_CALL palDestroyEventDriver(PalEventDriver* eventDriver);

/**
 * @brief Set the dispatch mode for an event type for the provided event driver.
 *
 * If the event driver is not valid, the function fails and the dispatch mode
 * will not be set.
 *
 * If the dispatch mode is PAL_DISPATCH_POLL, the event will be dispatched to
 * the event queue. If the dispatch mode is PAL_DISPATCH_CALLBACK and the
 * event driver has a valid callback, the event will be dispatched to the event
 * callback. If not the event will be discared.
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[in] type Event type to set dispatch mode for.
 * @param[in] mode Dispatch mode to use. See PalDispatchMode.
 *
 * @note This function may be called from any thread, but not concurrently.
 * Users must ensure synchronization.
 *
 * @sa palCreateEventDriver()
 * @sa PalDispatchMode
 * @sa PalEventType
 *
 * @since 1.0
 * @ingroup pal_event
 */
PAL_API void PAL_CALL palSetEventDispatchMode(
    PalEventDriver* eventDriver,
    PalEventType type,
    PalDispatchMode mode);

/**
 * @brief Get the dispatch mode for an event type for the provided event driver.
 *
 * If the event driver is not valid, the function fails and returns
 * PAL_DISPATCH_NONE.
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[in] type The event type.
 *
 * @return The dispatch mode on success or PAL_DISPATCH_NONE on failure.
 *
 * @note This function is thread-safe and may be called from any thread.
 *
 * @sa palCreateEventDriver()
 * @sa PalDispatchMode
 * @sa PalEventType
 *
 * @since 1.0
 * @ingroup pal_event
 */
PAL_API PalDispatchMode PAL_CALL palGetEventDispatchMode(
    PalEventDriver* eventDriver,
    PalEventType type);

/**
 * @brief Push an event onto the queue of the provided event driver.
 *
 * If the event driver is not valid, the function fails and the event will not
 * be pushed.
 * If the dispatch mode for the event is PAL_DISPATCH_POLL, the event will be
 * pushed to the event queue.
 *
 * If dispatch mode is PAL_DISPATCH_CALLBACK and the event driver has a valid
 * event callback, the callback will be called. If not the event will be
 * discared.
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[in] event Pointer to thes event. This must be valid and should be
 * initialized explicitly.
 *
 * @note This function may be called from any thread, but not concurrently.
 * Users must ensure synchronization.
 *
 * @sa palCreateEventDriver()
 * @sa PalDispatchMode
 * @sa PalEvent
 *
 * @since 1.0
 * @ingroup pal_event
 */
PAL_API void PAL_CALL palPushEvent(
    PalEventDriver* eventDriver,
    PalEvent* event);

/**
 * @brief Retrieve the next available event from the queue of the provided event
 * driver.
 *
 * If the event driver is not valid, the function fails and returns false.
 *
 * This function retrieves the next pending event from the queue of the
 * provided event driver without blocking. If no events are available, it
 * returns false.
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[out] outEvent Pointer to a PalEvent to recieve the event. Must be
 * valid.
 *
 * @note This function may be called from any thread, but not concurrently.
 * Users must ensure synchronization.
 *
 * @sa palCreateEventDriver()
 * @sa PalEvent
 *
 * @since 1.0
 * @ingroup pal_event
 */
PAL_API bool PAL_CALL palPollEvent(
    PalEventDriver* eventDriver,
    PalEvent* outEvent);

/** @} */ // end of pal_event group

#endif // _PAL_EVENT_H