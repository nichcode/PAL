
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup pal_event Event
 * @ingroup pal_event
 * @{
 */

#ifndef _PAL_EVENT_H
#define _PAL_EVENT_H

#include "pal/pal_core.h"

#define PAL_DECORATION_MODE_CLIENT_SIDE 0
#define PAL_DECORATION_MODE_SERVER_SIDE 1

/**
 * event.data2 : window
 *
 * Use inline helpers:
 * - palUnpackPointer()
 */
#define PAL_EVENT_WINDOW_CLOSE 0

/**
 * event.data : lower 32 bits = width, upper 32 bits = height
 *
 * event.data2 : window
 *
 * Use inline helpers:
 * - palUnpackUint32()
 * - palUnpackPointer()
 */
#define PAL_EVENT_WINDOW_SIZE 1

/**
 * event.data : lower 32 bits = x, upper 32 bits = y
 *
 * event.data2 : window
 *
 * Use inline helpers:
 * - palUnpackInt32()
 * - palUnpackPointer()
 */
#define PAL_EVENT_WINDOW_MOVE 2

/**
 * event.data : state(minimized, maximized, restored).
 *
 * event.data2 : window
 *
 * Use inline helpers:
 * - palUnpackPointer()
 */
#define PAL_EVENT_WINDOW_STATE 3

/**
 * event.data : `true` for focus gained or `false` for focus lost.
 *
 * event.data2 : window
 *
 * Use inline helpers:
 * - palUnpackPointer()
 */
#define PAL_EVENT_WINDOW_FOCUS 4

/**
 * event.data : `true` for visible or `false` for hidden.
 *
 * event.data2 : window
 *
 * Use inline helpers:
 * - palUnpackPointer()
 */
#define PAL_EVENT_WINDOW_VISIBILITY 5

/**
 * event.data2 : window
 */
#define PAL_EVENT_WINDOW_MODAL_BEGIN 6

/**
 * event.data2 : window
 */
#define PAL_EVENT_WINDOW_MODAL_END 7

/**
 * event.data2 : window
 */
#define PAL_EVENT_MONITOR_DPI_CHANGED 8

/**
 * event.data2 : window
 */
#define PAL_EVENT_MONITOR_LIST_CHANGED 9

/**
 * event.data : lower 32 bits = keycode, upper 32 bits = scancode
 *
 * event.data2 : window
 *
 * Use inline helpers:
 * - palUnpackUint32()
 * - palUnpackPointer()
 */
#define PAL_EVENT_KEYDOWN 10

/**
 * event.data : lower 32 bits = keycode, upper 32 bits = scancode
 *
 * event.data2 : window
 *
 * Use inline helpers:
 * - palUnpackUint32()
 * - palUnpackPointer()
 */
#define PAL_EVENT_KEYREPEAT 11

/**
 * event.data : lower 32 bits = keycode, upper 32 bits = scancode
 *
 * event.data2 : window
 *
 * Use inline helpers:
 * - palUnpackUint32()
 * - palUnpackPointer()
 */
#define PAL_EVENT_KEYUP 12

/**
 * event.data : lower 32 bits = button, upper 32 bits = serial
 *
 * event.data2 : window
 *
 * Use inline helpers:
 * - palUnpackPointer()
 */
#define PAL_EVENT_MOUSE_BUTTONDOWN 13

/**
 * event.data : lower 32 bits = button, upper 32 bits = serial
 *
 * event.data2 : window
 *
 * Use inline helpers:
 * - palUnpackPointer()
 */
#define PAL_EVENT_MOUSE_BUTTONUP 14

/**
 * event.data : lower 32 bits = x, upper 32 bits = y
 *
 * event.data2 : window
 *
 * Use inline helpers:
 * - palUnpackInt32()
 * - palUnpackPointer()
 */
#define PAL_EVENT_MOUSE_MOVE 15

/**
 * event.data : lower 32 bits = dx, upper 32 bits = dy
 *
 * event.data2 : window
 *
 * Use inline helpers:
 * - palUnpackInt32()
 * - palUnpackPointer()
 */
#define PAL_EVENT_MOUSE_DELTA 16

/**
 * event.data : lower 32 bits = dx, upper 32 bits = dy
 *
 * event.data2 : window
 *
 * Use inline helpers:
 * - palUnpackInt32()
 * - palUnpackPointer()
 */
#define PAL_EVENT_MOUSE_WHEEL 17

/**
 * event.userId : User event ID or type.
 *
 * Use inline helpers:
 * - palPackInt32()
 * - palPackUint32()
 * - palPackPointer()
 * - palUnpackInt32()
 * - palUnpackUint32()
 * - palUnpackPointer()
 */
#define PAL_EVENT_USER 18

/**
 * event.data : codepoint
 *
 * event.data2 : window
 *
 * Use inline helpers:
 * - palUnpackPointer()
 */
#define PAL_EVENT_KEYCHAR 19

/**
 * event.data : negotiated decorations mode
 *
 * event.data2 : window
 *
 * Use inline helpers:
 * - palUnpackPointer()
 */
#define PAL_EVENT_WINDOW_DECORATION_MODE 20

#define PAL_EVENT_MAX 21

/**
 * No dispatch.
 */
#define PAL_DISPATCH_NONE 0

/**
 * Dispatch to event callback.
 */
#define PAL_DISPATCH_CALLBACK 1

/**
 * Dispatch to event queue.
 */
#define PAL_DISPATCH_POLL 2

#define PAL_DISPATCH_MAX

/**
 * @struct PalEventDriver
 * @brief Opaque handle to an event driver.
 *
 * @since 1.0
 */
typedef struct PalEventDriver PalEventDriver;

/**
 * @struct PalEvent
 * @brief A single event.
 *
 * @since 1.0
 */
typedef struct PalEvent PalEvent;

/**
 * @typedef PalDecorationMode
 * @brief Decoration types. This is not a bitmask enum.
 *
 * All decoration types follow the format `PAL_DECORATION_MODE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalDecorationMode;

/**
 * @typedef PalEventType
 * @brief Event types. This is not a bitmask enum.
 *
 * All event types follow the format `PAL_EVENT_**` for consistency and
 * API use.
 *
 * @since 2.0
 */
typedef uint64_t PalEventType;

/**
 * @typedef PalDispatchMode
 * @brief Dispatch types for an event. This is not a bitmask enum.
 *
 * All dispatch modes follow the format `PAL_DISPATCH_**` for consistency
 * and API use.
 *
 * @since 2.0
 */
typedef uint64_t PalDispatchMode;

/**
 * @typedef PalEventCallback
 * @brief Function pointer type used for event callbacks.
 *
 * @param[in] userData Optional pointer to user data. Can be nullptr.
 * @param[in] event Pointer to the event.
 *
 * @since 1.0
 * @sa PalPushFn
 */
typedef void(PAL_CALL* PalEventCallback)(
    void* userData,
    const PalEvent* event);

/**
 * @typedef PalPushFn
 * @brief Function pointer type used for pushing events into event queues.
 *
 * @param[in] userData Optional pointer to user data. Can be nullptr.
 * @param[in] event Pointer to the event to push.
 *
 * @since 1.0
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
 * @param[out] event Pointer to the PalEvent to recieve the event.
 *
 * @since 1.0
 * @sa PalPushFn
 */
typedef PalBool(PAL_CALL* PalPollFn)(
    void* userData,
    PalEvent* outEvent);

struct PalEvent {
    int64_t userId; /**< You can have user events upto int64_t max.*/
    int64_t data;   /**< First data payload.*/
    int64_t data2;  /**< Second data payload.*/
    PalEventType type;
};

/**
 * @struct PalEventQueue
 * @brief Custom event queue.
 *
 * Provides user-defined event queue push and poll functions.
 *
 * @since 1.0
 */
typedef struct {
    PalPushFn push;
    PalPollFn poll;
    void* userData; /**< Optional user-provided data. Can be nullptr.*/
} PalEventQueue;

/**
 * @struct PalEventDriverCreateInfo
 * @brief Creation parameters for an event driver.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.0
 */
typedef struct {
    const PalAllocator* allocator; /**< Set to nullptr to use default.*/
    PalEventQueue* queue;          /**< Set to nullptr to use default.*/
    PalEventCallback callback;     /**< Can be nullptr.*/
    void* userData;                /**< Optional user-provided data. Can be nullptr.*/
} PalEventDriverCreateInfo;

/**
 * @brief Create an event driver.
 *
 * The allocator field in the provided PalEventDriverCreateInfo struct will not
 * be copied, therefore the pointer must remain valid until the event driver is
 * destroyed. Destroy the event driver with palDestroyEventDriver() when no
 * longer needed.
 *
 * @param[in] info Pointer to a PalEventDriverCreateInfo struct that specifies
 * parameters. Must not be nullptr.
 * @param[out] outEventDriver Pointer to a PalEventDriver to recieve the created
 * event driver. Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if the provided allocator is
 * thread safe and `outEventDriver` is per thread. The default allocator is
 * thread safe.
 *
 * @since 1.0
 * @sa palDestroyEventDriver
 */
PAL_API PalResult PAL_CALL palCreateEventDriver(
    const PalEventDriverCreateInfo* info,
    PalEventDriver** outEventDriver);

/**
 * @brief Destroy the provided event driver.
 *
 * If the provided event driver is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] eventDriver Pointer to the event driver to destroy.
 *
 * Thread safety: Thread safe if the allocator used to create
 * the event driver is thread safe and `eventDriver` is per thread.
 *
 * @since 1.0
 * @sa palCreateEventDriver
 */
PAL_API void PAL_CALL palDestroyEventDriver(PalEventDriver* eventDriver);

/**
 * @brief Set the dispatch mode for an event type with the provided event
 * driver.
 *
 * If the provided event driver is invalid or nullptr, this function returns
 * silently.
 *
 * If the dispatch mode is `PAL_DISPATCH_POLL`, the event will be dispatched
 * into the event drivers event queue. If the dispatch mode is
 * `PAL_DISPATCH_CALLBACK` and the event driver has a valid callback function,
 * the event will be dispatched to the callback function of the event driver
 * otherwise the event will be discarded.
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[in] type Event type to set dispatch mode for.
 * @param[in] mode Dispatch mode to use.
 *
 * Thread safety: Thread if multiple threads are not
 * simultaneously setting dispatch mode on the same `eventDriver`.
 *
 * @since 1.0
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
 * @return The dispatch mode on success or `PAL_DISPATCH_NONE` on failure.
 *
 * Thread safety: Thread if multiple threads are not
 * simultaneously setting dispatch mode on the same `eventDriver`.
 *
 * @since 1.0
 * @sa palSetEventDispatchMode
 */
PAL_API PalDispatchMode PAL_CALL palGetEventDispatchMode(
    PalEventDriver* eventDriver,
    PalEventType type);

/**
 * @brief Push an event into the queue or callback function of the provided
 * event driver.
 *
 * If the provided event driver is invalid or nullptr, this function returns
 * silently.
 *
 * If the dispatch mode for the event is `PAL_DISPATCH_POLL`, the event will be
 * pushed to the event queue.
 *
 * If dispatch mode is `PAL_DISPATCH_CALLBACK` and the event driver has a valid
 * event callback, the callback will be called otherwise the event will be
 * discarded.
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[in] event Pointer to the event to push.
 *
 * Thread safety: Thread if the provided event queue is thread
 * safe or every thread has its own `eventDriver`. The default event queue is
 * not thread safe.
 *
 * @since 1.0
 * @sa palPollEvent
 */
PAL_API void PAL_CALL palPushEvent(
    PalEventDriver* eventDriver,
    PalEvent* event);

/**
 * @brief Retrieve the next available event from the queue of the provided event
 * driver.
 *
 * If the provided event driver is invalid or nullptr, this function returns
 * silently.
 *
 * This function retrieves the next pending event from the queue of the
 * provided event driver without blocking. If no events are available, it
 * returns false.
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[out] outEvent Pointer to a PalEvent to recieve the event. Must be
 * valid.
 *
 * Thread safety: Thread if the provided event queue is thread
 * safe or every thread has its own `eventDriver`. The default event queue is
 * not thread safe.
 *
 * @since 1.0
 * @sa palPushEvent
 */
PAL_API PalBool PAL_CALL palPollEvent(
    PalEventDriver* eventDriver,
    PalEvent* outEvent);

/** @} */

#endif // _PAL_EVENT_H
