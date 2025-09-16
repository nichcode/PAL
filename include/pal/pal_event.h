
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
 * @ingroup event
 */
typedef struct PalEventDriver PalEventDriver;

typedef struct PalEvent PalEvent;

typedef void(PAL_CALL* PalEventCallback)(void* userData, const PalEvent* event);

typedef void(PAL_CALL* PalPushFn)(void* userData, PalEvent* event);

typedef bool(PAL_CALL* PalPollFn)(void* userData, PalEvent* outEvent);

/**
 * @enum PalEventType
 * @brief Types for an event. This is not a bitmask enum.
 *
 * @note All event types follow the format `PAL_EVENT_**` for consistency and
 * API use.
 *
 * @ingroup event
 */
typedef enum {
    PAL_EVENT_WINDOW_CLOSE,         /** < The window close button was clicked.*/
    PAL_EVENT_WINDOW_SIZE,          /** < The window was resized.*/
    PAL_EVENT_WINDOW_MOVE,          /** < The window was moved.*/
    PAL_EVENT_WINDOW_STATE,         /** < The window state has changed (minimzie,
                                       maximize, restores).*/
    PAL_EVENT_WINDOW_FOCUS,         /** < The window has gained or lost focus. Check
                                       `event.data` if `true` or `false`.*/
    PAL_EVENT_WINDOW_VISIBILITY,    /** < The window is shown or hidden. Check
                                       `event.data` if `true` or `false`.*/
    PAL_EVENT_WINDOW_MODAL_BEGIN,   /** < The window has entered move or resize
                                       modal.`.*/
    PAL_EVENT_WINDOW_MODAL_END,     /** < The window has exited move or resize
                                       modal.`.*/
    PAL_EVENT_MONITOR_DPI_CHANGED,  /** < The monitor DPI has changed.*/
    PAL_EVENT_MONITOR_LIST_CHANGED, /** < The monitor list has changed. Enumerate
                                       `palEnumerateMonitors()` again to get valid
                                       handles.*/
    PAL_EVENT_KEYDOWN,              /** < A keyboard key was pressed.*/
    PAL_EVENT_KEYREPEAT,            /** < A keyboard key is being repeated.*/
    PAL_EVENT_KEYUP,                /** < A keyboard key was released.*/
    PAL_EVENT_MOUSE_BUTTONDOWN,     /** < A mouse button was pressed.*/
    PAL_EVENT_MOUSE_BUTTONUP,       /** < A mouse button was released.*/
    PAL_EVENT_MOUSE_MOVE,           /** < The mouse was moved. This will be triggered if
                                       there is a focus window.*/
    PAL_EVENT_MOUSE_DELTA,          /** < The mouse has movedMouse movement delta.*/
    PAL_EVENT_MOUSE_WHEEL,          /** < The delta of the mouse position. This will be
                                       triggered if there is a focus window.*/
    PAL_EVENT_USER,                 /** < A user event. Differentiate between them with `userId`
                                       field in `PalEvent`.*/
    PAL_EVENT_MAX
} PalEventType;

/**
 * @enum PalDispatchMode
 * @brief Dispatch mode for an event. This is not a bitmask enum.
 *
 * @note All dispatch modes follow the format `PAL_DISPATCH_**` for consistency
 * and API use.
 *
 * @ingroup event
 */
typedef enum {
    PAL_DISPATCH_NONE,     /** < Dont't dispatch event.*/
    PAL_DISPATCH_CALLBACK, /** < Dispatch to event callback.*/
    PAL_DISPATCH_POLL,     /** < Dispatch to the event queue.*/
    PAL_DISPATCH_MAX
} PalDispatchMode;

/**
 * @struct PalEvent
 * @brief This describes a single PAL event.
 *
 * @ingroup event
 */
typedef struct PalEvent {
    PalEventType type;   /** < The type of the event. See `PalEventType`.*/
    Int64        data;   /** < First data payload.*/
    Int64        data2;  /** < Second data payload.*/
    Int64        userId; /** < This is for user events. You can have user events upto
                            Int64 max.*/
} PalEvent;

/**
 * @struct PalEventQueue
 * @brief Describes a user provided event queue.
 *
 * Allows the user to override the default event queue when creating an event
 * driver.
 *
 * @ingroup event
 */
typedef struct {
    PalPushFn push;     /** < Event push function pointer.*/
    PalPollFn poll;     /** < Event poll function pointer.*/
    void*     userData; /** Optional pointer to user data passed into event queue
                           queue functions. Can be `nullptr`.*/
} PalEventQueue;

/**
 * @struct PalEventDriverCreateInfo
 * @brief Specifications for creating an event driver.
 *
 * This struct must be initialized and passed to `palCreateEventDriver()`.
 * All fields must be explicitly set by the user.
 *
 * @note Uninitialized fields may result in undefined behavior.
 *
 * @ingroup event
 */
typedef struct {
    const PalAllocator* allocator; /** < User allocator or `nullptr` for default.*/
    PalEventQueue*      queue; /** < User provided event queue. Set to `nullptr` to use default.*/
    PalEventCallback    callback; /** < Event callback to call for callback type
                                     events. Can be `nullptr`.*/
    void* userData;               /** Optional pointer to user data passed into event callback.
                                     Can be `nullptr`.*/
} PalEventDriverCreateInfo;

/**
 * @brief Create an event driver.
 *
 * An event driver is used to signal events between multiple systems.
 * The user is responsible for destroying the event driver when no longer
 * needed.
 *
 * @param[in] info Pointer to a `PalEventDriverCreateInfo` struct with creation
 * specifications.
 * @param[out] outEventDriver Pointer to recieve the created event driver.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on
 * failure.
 *
 * @note This function is not thread-safe. If multiple threads will call this
 * function, the user is responsible for synchronization.
 * @note The created event driver must be destroyed with
 * `palDestroyEventDriver()` when no longer needed.
 *
 * @sa PalEventDriverCreateInfo, palDestroyEventDriver()
 * @ingroup event
 */
PAL_API PalResult PAL_CALL palCreateEventDriver(const PalEventDriverCreateInfo* info,
                                                PalEventDriver**                outEventDriver);

/**
 * @brief Destroy the event driver.
 *
 * This function can be called multiple times without any undefined behavior.
 * If the event driver is invalid or `nullptr`, the function returns silently.
 *
 * @param[in] eventDriver Pointer to the event driver to destroy.
 *
 * @note This function is not thread-safe. If multiple threads will call this
 * function, the user is responsible for synchronization.
 *
 * @sa palCreateEventDriver()
 * @ingroup event
 */
PAL_API void PAL_CALL palDestroyEventDriver(PalEventDriver* eventDriver);

/**
 * @brief Set the dispatch mode for an event type for the provided event driver.
 *
 * If the dispatch mode is `PAL_DISPATCH_POLL`, the event will be dispatched to
 * the event queue. If the dispatch mode is `PAL_DISPATCH_CALLBACK` and the
 * event driver has a valid callback, the event will be dispatched to the event
 * callback.
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[in] type Event type to set dispatch for.
 * @param[in] mode Dispatch mode to use. See `PalDispatchMode`.
 *
 * @note This function is not thread-safe. If multiple threads will call this
 * function, the user is responsible for synchronization.
 * @note This function is guaranteed not to fail if `eventDriver` is valid.
 *
 * @sa palCreateEventDriver(), PalDispatchMode, PalEventType
 * @ingroup event
 */
PAL_API void PAL_CALL palSetEventDispatchMode(PalEventDriver* eventDriver, PalEventType type,
                                              PalDispatchMode mode);

/**
 * @brief Get the dispatch mode for an event type for the provided event driver.
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[in] type The event type.
 *
 * @return The dispatch mode on success or `PAL_DISPATCH_NONE` on failure.
 *
 * @note This function is thread-safe.
 * @note This function is guaranteed not to fail if `eventDriver` is valid.
 *
 * @sa palCreateEventDriver(), PalDispatchMode, PalEventType
 * @ingroup event
 */
PAL_API PalDispatchMode PAL_CALL palGetEventDispatchMode(PalEventDriver* eventDriver,
                                                         PalEventType    type);

/**
 * @brief Push an event onto the event queue of an event driver.
 *
 * If the dispatch mode for `event` is `PAL_DISPATCH_POLL`, the event will be
 * pushed to the event queue. Otherwise if dispatch mode is
 * `PAL_DISPATCH_CALLBACK`, the event callback function will be called.
 *
 * If the dispatch mode is `PAL_DISPATCH_CALLBACK`, and the `eventDriver` has no
 * valid event callback, the function returns silently.
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[in] event Pointer to an event. This must be valid and should be
 * initialized and explicitly set by the user.
 *
 * @note This function is not thread-safe. If multiple threads will call this
 * function, the user is responsible for synchronization.
 * @note This function is guaranteed not to fail if `eventDriver` and `event`
 * are valid.
 *
 * @sa palCreateEventDriver(), PalDispatchMode, PalEvent
 * @ingroup event
 */
PAL_API void PAL_CALL palPushEvent(PalEventDriver* eventDriver, PalEvent* event);

/**
 * @brief Retrieve the next available event from the event queue of an event
 * driver.
 *
 * This functions retrieves the next pending event from the event queue of an
 * event driver without blocking. If no events are available, it returns
 * `false`.
 *
 * If `eventDriver` is not valid, this function returns `false`
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[out] outEvent Pointer to recieve the event from the event queue. Must
 * be valid.
 *
 * @note This function is not thread-safe. If multiple threads will call this
 * function, the user is responsible for synchronization.
 * @note This function is guaranteed not to fail if `eventDriver` and `outEvent`
 * are valid.
 *
 * @sa palCreateEventDriver(), PalEvent
 * @ingroup event
 */
PAL_API bool PAL_CALL palPollEvent(PalEventDriver* eventDriver, PalEvent* outEvent);

#endif // _PAL_EVENT_H