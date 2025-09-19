
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
 * @file pal_event.h
 *
 * Header file for event functions, macros, enum and structs
 *
 * @defgroup event
 *
 */

#ifndef _PAL_EVENT_H
#define _PAL_EVENT_H

#include "pal_core.h"

/**
 * @struct PalEventDriver
 * @brief Opaque handle to an event driver.
 *
 * @since Added in version 1.0.0.
 * @ingroup event
 */
typedef struct PalEventDriver PalEventDriver;

/**
 * @struct PalEvent
 * @brief This describes an event.
 *
 * @since Added in version 1.0.0.
 * @ingroup event
 */
typedef struct PalEvent PalEvent;

/**
 * @typedef PalEventCallback
 * @brief Function pointer type used for event callbacks.
 *
 * This function will be called if an event with a dispatch mode of
 * `PAL_DISPATCH_CALLBACK` is triggered. The `event` is only valid for the
 * duration of the callback, copy if you need to store it.
 *
 * @param[in] userData Optional pointer to user data. Can be nullptr.
 * @param[in] event The triggered event.
 *
 * @sa PalEvent
 * @sa PalEventDriver
 * @sa palPushEvent()
 *
 * @since Added in version 1.0.0.
 * @ingroup event
 */
typedef void(PAL_CALL* PalEventCallback)(
    void* userData,
    const PalEvent* event);

/**
 * @typedef PalPushFn
 * @brief Function pointer type used for pushing events to event queues.
 *
 * @param[in] userData Optional pointer to user data. Can be nullptr.
 * @param[in] event The event to push. This is copied.
 *
 * @sa PalEvent
 * @sa PalEventDriver
 * @sa palPushEvent()
 * @sa palPollEvent()
 *
 * @since Added in version 1.0.0.
 * @ingroup event
 */
typedef void(PAL_CALL* PalPushFn)(
    void* userData,
    PalEvent* event);

/**
 * @typedef PalPollFn
 * @brief Function pointer type used for polling events from event queues.
 *
 * This function should return false if the event queue is empty.
 * If the queue is not empty and the event was retrieved, this must return true.
 *
 * @param[in] userData Optional pointer to user data. Can be nullptr.
 * @param[out] event Pointer to recieve the event.
 *
 * @sa PalEvent
 * @sa PalEventDriver
 * @sa palPushEvent()
 * @sa palPollEvent()
 *
 * @since Added in version 1.0.0.
 * @ingroup event
 */
typedef bool(PAL_CALL* PalPollFn)(
    void* userData,
    PalEvent* outEvent);

/**
 * @enum PalEventType
 * @brief Event types. This is not a bitmask enum.
 *
 * @note All event types follow the format `PAL_EVENT_**` for consistency and
 * API use.
 *
 * @since Added in version 1.0.0.
 * @ingroup event
 */
typedef enum {
    /** The window close button was clicked.*/
    PAL_EVENT_WINDOW_CLOSE,

    /** The window was resized.*/
    PAL_EVENT_WINDOW_SIZE,

    /** The window was moved.*/
    PAL_EVENT_WINDOW_MOVE,

    /** The window state has changed (minimized, maximized, restored).*/
    PAL_EVENT_WINDOW_STATE,

    /** The window has gained or lost focus. Check event.data, true for focus
     * gained otherwise false.*/
    PAL_EVENT_WINDOW_FOCUS,

    /** The window is shown or hidden. Check event.data, true for visible
     * otherwise false.*/
    PAL_EVENT_WINDOW_VISIBILITY,

    /** The window has entered move or resize modal.*/
    PAL_EVENT_WINDOW_MODAL_BEGIN,

    /** The window has exited move or resize modal.*/
    PAL_EVENT_WINDOW_MODAL_END,

    /** The monitor DPI has changed.*/
    PAL_EVENT_MONITOR_DPI_CHANGED,

    /** The monitor list has changed. Enumerate `palEnumerateMonitors()` again
     * to get valid handles.*/
    PAL_EVENT_MONITOR_LIST_CHANGED,

    /** A keyboard key was pressed.*/
    PAL_EVENT_KEYDOWN,

    /** A keyboard key is being held down.*/
    PAL_EVENT_KEYREPEAT,

    /** A keyboard key was released.*/
    PAL_EVENT_KEYUP,

    /** A mouse button was pressed.*/
    PAL_EVENT_MOUSE_BUTTONDOWN,

    /** A mouse button was released.*/
    PAL_EVENT_MOUSE_BUTTONUP,

    /** The mouse was moved. This will be triggered only if there is a focused
     * window.*/
    PAL_EVENT_MOUSE_MOVE,

    /** Mouse movement delta. This is triggered after the mouse has been
     * moved.*/
    PAL_EVENT_MOUSE_DELTA,

    /** Mouse wheel (scroll) delta. This will be triggered only if there is a
     * focused window.*/
    PAL_EVENT_MOUSE_WHEEL,

    /** user event. user events are typed with `userId` field in PalEvent.*/
    PAL_EVENT_USER,

    PAL_EVENT_MAX
} PalEventType;

/**
 * @enum PalDispatchMode
 * @brief Dispatch types for an event. This is not a bitmask enum.
 *
 * @note All dispatch modes follow the format `PAL_DISPATCH_**` for consistency
 * and API use.
 *
 * @since Added in version 1.0.0.
 * @ingroup event
 */
typedef enum {
    /** Event should not be generated (no dispatch).*/
    PAL_DISPATCH_NONE,

    /** Dispatch to event callback.*/
    PAL_DISPATCH_CALLBACK,

    /** Dispatch to the event queue.*/
    PAL_DISPATCH_POLL,

    PAL_DISPATCH_MAX
} PalDispatchMode;

struct PalEvent {
    /** The type of the event. See PalEventType.*/
    PalEventType type;

    /** First data payload.*/
    Int64 data;

    /** Second data payload.*/
    Int64 data2;

    /** This is for user events. You can have user events upto Int64 max.*/
    Int64 userId;
};

/**
 * @struct PalEventQueue
 * @brief Describes a user provided event queue.
 *
 * Allows the user to override the default event queue when creating an event
 * driver.
 *
 * @since Added in version 1.0.0.
 * @ingroup event
 */
typedef struct {
    /** Event push function pointer.*/
    PalPushFn push;

    /** Event poll function pointer.*/
    PalPollFn poll;

    /** Optional pointer to user data passed into queue functions. Can be
     * nullptr.*/
    void* userData;
} PalEventQueue;

/**
 * @struct PalEventDriverCreateInfo
 * @brief Describes options for creating an event driver.
 *
 * This struct must be initialized explicitly and passed to
 * `palCreateEventDriver()`.
 *
 * @note Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 1.0.0.
 * @ingroup event
 */
typedef struct {
    /** Allocator. This will not be copied, so the allocator must
     * stay valid until the event driver is destroyed. Set to nullptr to use
     * default.*/
    const PalAllocator* allocator;

    /** Event queue to use for the event driver. see PalEventQueue. Set to
     * nullptr to use default.*/
    PalEventQueue* queue;

    /** Event callback. see PalEventCallback. If nullptr, events with
     * dispatch mode of `PAL_DISPATCH_CALLBACK` will not be triggered.*/
    PalEventCallback callback;

    /** Optional pointer to user data passed to the event callback. Can be
     * nullptr.*/
    void* userData;
} PalEventDriverCreateInfo;

/**
 * @brief Creates an event driver.
 *
 * An event driver is used to signal events between multiple systems.
 * It it used to shared information from one system or entity to another.
 * The created event driver must be destroyed with `palDestroyEventDriver()`
 * when no longer needed to prevent memory leaks.
 *
 * @param[in] info Pointer to the `PalEventDriverCreateInfo` struct with
 * creation options.
 * @param[out] outEventDriver Pointer to a PalEventDriver to recieve the created
 * event driver.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on
 * failure. Call palFormatResult() for more information.
 *
 * @note This function may be called from any thread, but not concurrently.
 * Users must ensure synchronization.
 *
 * @sa PalEventDriverCreateInfo
 * @sa palDestroyEventDriver()
 *
 * @since Added in version 1.0.0.
 * @ingroup event
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
 * @since Added in version 1.0.0.
 * @ingroup event
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
 * @since Added in version 1.0.0.
 * @ingroup event
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
 * @since Added in version 1.0.0.
 * @ingroup event
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
 * @since Added in version 1.0.0.
 * @ingroup event
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
 * @since Added in version 1.0.0.
 * @ingroup event
 */
PAL_API bool PAL_CALL palPollEvent(
    PalEventDriver* eventDriver,
    PalEvent* outEvent);

#endif // _PAL_EVENT_H