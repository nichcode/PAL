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

/**
 * PAL_EVENT_TYPE_WINDOW_MOVE
 *
 * data:
 *  bits 0-31: x
 *  bits 32-63: y
 *
 * data2:
 *  window
 *
 * Helpers:
 * palUnpackInt32(event.data, &x, &y)
 * window = palUnpackPointer(event.data2)
 */

#define PAL_EVENT_TYPE_WINDOW_MOVE 2

/**
 * PAL_EVENT_TYPE_WINDOW_STATE
 *
 * data:
 *  bits 0-31: state (minimized, maximized, restored).
 *  bits 32-63: unused
 *
 * data2:
 *  window
 *
 * Helpers:
 * window = palUnpackPointer(event.data2)
 */
#define PAL_EVENT_TYPE_WINDOW_STATE 3

/**
 * PAL_EVENT_TYPE_WINDOW_FOCUS
 *
 * data:
 *  bits 0-31: focus gained (`PAL_TRUE`/`PAL_FALSE`).
 *  bits 32-63: unused
 *
 * data2:
 *  window
 *
 * Helpers:
 * window = palUnpackPointer(event.data2)
 */
#define PAL_EVENT_TYPE_WINDOW_FOCUS 4

/**
 * PAL_EVENT_TYPE_WINDOW_VISIBILITY
 *
 * data:
 *  bits 0-31: visibility (`PAL_TRUE`/`PAL_FALSE`).
 *  bits 32-63: unused
 *
 * data2:
 *  window
 *
 * Helpers:
 * window = palUnpackPointer(event.data2)
 */
#define PAL_EVENT_TYPE_WINDOW_VISIBILITY 5

/**
 * PAL_EVENT_TYPE_WINDOW_MODAL_BEGIN
 *
 * data2:
 *  window
 *
 * Helpers:
 * window = palUnpackPointer(event.data2)
 */
#define PAL_EVENT_TYPE_WINDOW_MODAL_BEGIN 6

/**
 * PAL_EVENT_TYPE_WINDOW_MODAL_END
 *
 * data2:
 *  window
 *
 * Helpers:
 * window = palUnpackPointer(event.data2)
 */
#define PAL_EVENT_TYPE_WINDOW_MODAL_END 7

/**
 * PAL_EVENT_TYPE_MONITOR_DPI_CHANGED
 *
 * data:
 *  bits 0-31: dpi
 *  bits 32-63: unused
 *
 * data2:
 *  window
 *
 * Helpers:
 * window = palUnpackPointer(event.data2)
 */
#define PAL_EVENT_TYPE_MONITOR_DPI_CHANGED 8

/**
 * PAL_EVENT_TYPE_MONITOR_LIST_CHANGED
 *
 * data2:
 *  window
 *
 * Helpers:
 * window = palUnpackPointer(event.data2)
 */
#define PAL_EVENT_TYPE_MONITOR_LIST_CHANGED 9

/**
 * PAL_EVENT_TYPE_KEYDOWN
 *
 * data:
 *  bits 0-31: keycode
 *  bits 32-63: scancode
 *
 * data2:
 *  window
 *
 * Helpers:
 * palUnpackUint32(event.data, &keycode, &scancode)
 * window = palUnpackPointer(event.data2)
 */
#define PAL_EVENT_TYPE_KEYDOWN 10

/**
 * PAL_EVENT_TYPE_KEYREPEAT
 *
 * data:
 *  bits 0-31: keycode
 *  bits 32-63: scancode
 *
 * data2:
 *  window
 *
 * Helpers:
 * palUnpackUint32(event.data, &keycode, &scancode)
 * window = palUnpackPointer(event.data2)
 */
#define PAL_EVENT_TYPE_KEYREPEAT 11

/**
 * PAL_EVENT_TYPE_KEYUP
 *
 * data:
 *  bits 0-31: keycode
 *  bits 32-63: scancode
 *
 * data2:
 *  window
 *
 * Helpers:
 * palUnpackUint32(event.data, &keycode, &scancode)
 * window = palUnpackPointer(event.data2)
 */
#define PAL_EVENT_TYPE_KEYUP 12

/**
 * PAL_EVENT_TYPE_MOUSE_BUTTONDOWN
 *
 * data:
 *  bits 0-31: button
 *  bits 32-63: serial
 *
 * data2:
 *  window
 *
 * Helpers:
 * palUnpackUint32(event.data, &button, &serial)
 * window = palUnpackPointer(event.data2)
 */
#define PAL_EVENT_TYPE_MOUSE_BUTTONDOWN 13

/**
 * PAL_EVENT_TYPE_MOUSE_BUTTONUP
 *
 * data:
 *  bits 0-31: button
 *  bits 32-63: serial
 *
 * data2:
 *  window
 *
 * Helpers:
 * palUnpackUint32(event.data, &button, &serial)
 * window = palUnpackPointer(event.data2)
 */
#define PAL_EVENT_TYPE_MOUSE_BUTTONUP 14

/**
 * PAL_EVENT_TYPE_MOUSE_MOVE
 *
 * data:
 *  bits 0-31: x
 *  bits 32-63: y
 *
 * data2:
 *  window
 *
 * Helpers:
 * palUnpackInt32(event.data, &x, &y)
 * window = palUnpackPointer(event.data2)
 */
#define PAL_EVENT_TYPE_MOUSE_MOVE 15

/**
 * PAL_EVENT_TYPE_MOUSE_DELTA
 *
 * data:
 *  bits 0-31: dx
 *  bits 32-63: dy
 *
 * data2:
 *  window
 *
 * Helpers:
 * palUnpackFloat(event.data, &dx, &dy)
 * window = palUnpackPointer(event.data2)
 */
#define PAL_EVENT_TYPE_MOUSE_DELTA 16

/**
 * PAL_EVENT_TYPE_MOUSE_WHEEL
 *
 * data:
 *  bits 0-31: dx
 *  bits 32-63: dy
 *
 * data2:
 *  window
 *
 * Helpers:
 * palUnpackFloat(event.data, &dx, &dy)
 * window = palUnpackPointer(event.data2)
 */
#define PAL_EVENT_TYPE_MOUSE_WHEEL 17

/**
 * PAL_EVENT_TYPE_USER
 *
 * userId:
 *  User event ID or type.
 *
 * Helpers:
 * palPackInt32()
 * palPackUint32()
 * palPackPointer()
 * palUnpackInt32()
 * palUnpackUint32()
 * palUnpackPointer()
 */
#define PAL_EVENT_TYPE_USER 18

/**
 * PAL_EVENT_TYPE_KEYCHAR
 *
 * data:
 *  bits 0-31: codepoint
 *  bits 32-63: unused
 *
 * data2:
 *  window
 *
 * Helpers:
 * window = palUnpackPointer(event.data2)
 */
#define PAL_EVENT_TYPE_KEYCHAR 19

/**
 * PAL_EVENT_TYPE_WINDOW_DECORATION_MODE
 *
 * data:
 *  bits 0-31: decorations mode
 *  bits 32-63: unused
 *
 * data2:
 *  window
 *
 * Helpers:
 * window = palUnpackPointer(event.data2)
 */
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
 * @since 2.0
 */
typedef struct PalEventDriver PalEventDriver;

/**
 * @struct PalEvent
 * @brief A single event.
 * 
 * The payloads are packed in the `data` and `data2` field of the struct. 
 * Each event type determines how its information is packed. See 
 * `PalEventType` for more information.
 * 
 * Below are helpers to make the extraction simple and easy:
 * 
 * - `palPackInt32()` / `palUnpackInt32()` - Pack and unpack two int32_ts.
 * 
 * - `palPackUint32()` / `palUnpackUint32()` - Pack and unpack two uint32_ts.
 * 
 * - `palPackFloat()` / `palUnpackFloat()` - Pack and unpack two floats.
 * This works with big and small endian systems.
 * 
 * - `palPackPointer()` / `palUnpackPointer()` - Pack and unpack two int32_t.
 *
 * @since 2.0
 */
typedef struct PalEvent PalEvent;

/**
 * @typedef PalDecorationMode
 * @brief Window decoration modes.
 * 
 * All values of this type follow the format `PAL_DECORATION_MODE_*` for API
 * consistency and ease of use.
 * @since 2.0
 * 
 * @def PAL_DECORATION_MODE_CLIENT_SIDE
 * Window decoration must be handled by the client.
 * @def PAL_DECORATION_MODE_SERVER_SIDE
 * Window decoration will be handled by the server.
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
 * @since 2.0
 * 
 * @def PAL_EVENT_TYPE_WINDOW_CLOSE
 * `event.data`: unused.
 * `event.data2`: pointer to the window.
 * 
 * @def PAL_EVENT_TYPE_WINDOW_SIZE
 * `event.data`: (bits 0-31: window width, bits 32-63: window height).
 * `event.data2`: pointer to the window.
 */
typedef uint32_t PalEventType;

/**
 * @typedef PalDispatchMode
 * @brief Dispatch modes for an event.
 *
 * All values of this type follow the format `PAL_DISPATCH_MODE_*` for API
 * consistency and ease of use.
 * @since 2.0
 * 
 * @def PAL_DISPATCH_MODE_NONE
 * The event will be discarded.
 * @def PAL_DISPATCH_MODE_CALLBACK
 * The event will be push to the event callback.
 * @def PAL_DISPATCH_MODE_POLL
 * The event will be pushed to the event queue.
 * @def PAL_DISPATCH_MODE_COUNT
 * The number of dispatch modes. The literal value must not be used.
 */
typedef uint32_t PalDispatchMode;

/**
 * @typedef PalEventCallback
 * @brief Function pointer type used for event callbacks.
 *
 * @param[in] userData Optional pointer to user data. Can be `nullptr`.
 * @param[in] event Pointer to the event.
 *
 * @since 2.0
 * @sa PalPushFn
 */
typedef void(PAL_CALL* PalEventCallback)(
    void* userData,
    const PalEvent* event);

/**
 * @typedef PalPushFn
 * @brief Function pointer type used for pushing events into event queues.
 *
 * @param[in] userData Optional pointer to user data. Can be `nullptr`.
 * @param[in] event Pointer to the event to push.
 *
 * @since 2.0
 * @sa PalEventCallback
 */
typedef void(PAL_CALL* PalPushFn)(
    void* userData,
    PalEvent* event);

/**
 * @typedef PalPollFn
 * @brief Function pointer type used for polling events from event queues.
 *
 * This function should return `PAL_FALSE` if the event queue is empty.
 * If the queue is not empty and the event was retrieved, this should return
 * `PAL_TRUE`.
 *
 * @param[in] userData Optional pointer to user data. Can be `nullptr`.
 * @param[out] outEvent Pointer to the PalEvent to recieve the event.
 *
 * @since 2.0
 * @sa PalPushFn
 */
typedef PalBool(PAL_CALL* PalPollFn)(
    void* userData,
    PalEvent* outEvent);

struct PalEvent 
{
    uint64_t data;     /**< First data payload.*/
    uint64_t data2;    /**< Second data payload.*/
    uint32_t userId;   /**< User event id.*/
    PalEventType type; /**< (eg. `PAL_EVENT_TYPE_WINDOW_MOVE`).*/
};

/**
 * @struct PalEventQueue
 * @brief Custom event queue.
 *
 * Provides user-defined event queue push and poll functions.
 *
 * @since 2.0
 */
typedef struct {
    PalPushFn push; /**< Push function pointer.*/
    PalPollFn poll; /**< Poll function pointer.*/
    void* userData; /**< Optional user-provided data. Can be `nullptr`.*/
} PalEventQueue;

/**
 * @struct PalEventDriverCreateInfo
 * @brief Creation parameters for an event driver.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    const PalAllocator* allocator; /**< Set to `nullptr` to use default.*/
    PalEventQueue* queue;          /**< Set to `nullptr` to use default.*/
    PalEventCallback callback;     /**< Can be `nullptr`.*/
    void* userData;                /**< Optional user-provided data. Can be `nullptr`.*/
} PalEventDriverCreateInfo;

/**
 * @brief Create an event driver.
 *
 * The allocator field in the provided PalEventDriverCreateInfo struct will not
 * be copied, therefore the pointer must remain valid until the event driver is
 * destroyed. Destroy the event driver with `palDestroyEventDriver()` when no
 * longer needed.
 *
 * @param[in] info Pointer to a PalEventDriverCreateInfo struct that specifies
 * parameters.
 * @param[out] outEventDriver Pointer to a PalEventDriver to recieve the created
 * event driver.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if the provided allocator is
 * thread safe and `outEventDriver` is per thread. The default allocator is
 * thread safe.
 *
 * @since 2.0
 * @sa palDestroyEventDriver
 */
PAL_API PalResult PAL_CALL palCreateEventDriver(
    const PalEventDriverCreateInfo* info,
    PalEventDriver** outEventDriver);

/**
 * @brief Destroy the provided event driver.
 *
 * @param[in] eventDriver Pointer to the event driver to destroy.
 *
 * Thread safety: Thread safe if the allocator used to create
 * the event driver is thread safe and `eventDriver` is per thread.
 *
 * @since 2.0
 * @sa palCreateEventDriver
 */
PAL_API void PAL_CALL palDestroyEventDriver(PalEventDriver* eventDriver);

/**
 * @brief Set the dispatch mode for an event type with the provided event
 * driver.
 *
 * If the dispatch mode is `PAL_DISPATCH_MODE_POLL`, the event will be dispatched
 * into the event drivers event queue. If the dispatch mode is
 * `PAL_DISPATCH_MODE_CALLBACK`, the event driver must have a valid callback function otherwise
 * undefined behavior.
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[in] type Event type to set dispatch mode for.
 * @param[in] mode Dispatch mode to use.
 *
 * Thread safety: Thread safe if multiple threads are not
 * simultaneously setting dispatch mode on the same `eventDriver`.
 *
 * @since 2.0
 * @sa palGetEventDispatchMode
 */
PAL_API void PAL_CALL palSetEventDispatchMode(
    PalEventDriver* eventDriver,
    PalEventType type,
    PalDispatchMode mode);

/**
 * @brief Get the dispatch mode for an event type with the provided event
 * driver.
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[in] type The event type.
 *
 * @return The dispatch mode on success or `PAL_DISPATCH_MODE_NONE` on failure.
 *
 * Thread safety: Thread safe if multiple threads are not
 * simultaneously setting dispatch mode on the same `eventDriver`.
 *
 * @since 2.0
 * @sa palSetEventDispatchMode
 */
PAL_API PalDispatchMode PAL_CALL palGetEventDispatchMode(
    PalEventDriver* eventDriver,
    PalEventType type);

/**
 * @brief Push an event into the queue or callback function of the provided
 * event driver.
 *
 * If the dispatch mode for the event is `PAL_DISPATCH_MODE_POLL`, the event will be
 * pushed to the event queue.
 *
 * If dispatch mode is `PAL_DISPATCH_MODE_CALLBACK`, the event driver must have a valid callback
 * function otherwise undefined behavior.
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[in] event Pointer to the event to push.
 *
 * Thread safety: Thread safe if the provided event queue is thread
 * safe or every thread has its own `eventDriver`. The default event queue is
 * not thread safe.
 *
 * @since 2.0
 * @sa palPollEvent
 */
PAL_API void PAL_CALL palPushEvent(
    PalEventDriver* eventDriver,
    PalEvent* event);

/**
 * @brief Retrieve the next available event from the queue of the provided event
 * driver.
 *
 * This function retrieves the next pending event from the queue of the
 * provided event driver without blocking. If no events are available, it
 * returns `PAL_FALSE`.
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[out] outEvent Pointer to a PalEvent to recieve the event. Must be
 * valid.
 *
 * Thread safety: Thread safe if the provided event queue is thread
 * safe or every thread has its own `eventDriver`. The default event queue is
 * not thread safe.
 *
 * @since 2.0
 * @sa palPushEvent
 */
PAL_API PalBool PAL_CALL palPollEvent(
    PalEventDriver* eventDriver,
    PalEvent* outEvent);

/** @} */

#endif // _PAL_EVENT_H
