
/**
 * @file event.h
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

#ifndef PAL_EVENT_EVENT_H
#define PAL_EVENT_EVENT_H

#include "types.h"

/**
 * @defgroup decoration_modes Window Decoration Modes
 * @brief Defines decoration modes of a decoration mode event.
 * 
 * @ingroup pal_event
 * @{
 */
#define PAL_DECORATION_MODE_CLIENT_SIDE 0 /**< Client (application) is responsible for drawing window decorations. */
#define PAL_DECORATION_MODE_SERVER_SIDE 1 /**< Windowing system (server) is responsible for drawing decorations. */
#define PAL_DECORATION_MODE_COUNT 2 /**< Number of window decoration modes. */
/** @} */

/**
 * @defgroup event_types Event Types
 * @brief Defines event types.
 * 
 * @ingroup pal_event
 * @{
 */
#define PAL_EVENT_TYPE_WINDOW_CLOSE 0 /**< Window close button has been clicked. */
#define PAL_EVENT_TYPE_WINDOW_SIZE 1 /**< Window has been resized. */
#define PAL_EVENT_TYPE_WINDOW_MOVE 2 /**< Window has been moved. */
#define PAL_EVENT_TYPE_WINDOW_STATE 3 /**< Window state has changed. */
#define PAL_EVENT_TYPE_WINDOW_FOCUS 4 /**< Window has gained or lost focus. */
#define PAL_EVENT_TYPE_WINDOW_VISIBILITY 5 /**< Window has been shown or hidden. */
#define PAL_EVENT_TYPE_WINDOW_MODAL_BEGIN 6 /**< Window has entered modal mode. */
#define PAL_EVENT_TYPE_WINDOW_MODAL_END 7 /**< Window has exited modal mode. */
#define PAL_EVENT_TYPE_MONITOR_DPI_CHANGED 8 /**< Monitor DPI has changed. */
#define PAL_EVENT_TYPE_MONITOR_LIST_CHANGED 9 /**< Number of connected monitors has changed. */
#define PAL_EVENT_TYPE_KEYDOWN 10 /**< Key was pressed. */
#define PAL_EVENT_TYPE_KEYREPEAT 11 /**< Key is being held down and is repeating. */
#define PAL_EVENT_TYPE_KEYUP 12 /**< Key has been released. */
#define PAL_EVENT_TYPE_MOUSE_BUTTONDOWN 13 /**< Mouse button has been pressed. */
#define PAL_EVENT_TYPE_MOUSE_BUTTONUP 14 /**< Mouse button has been released. */
#define PAL_EVENT_TYPE_MOUSE_MOVE 15 /**< Mouse was moved. */
#define PAL_EVENT_TYPE_MOUSE_DELTA 16 /**< Mouse movement delta has changed. */
#define PAL_EVENT_TYPE_MOUSE_WHEEL 17 /**< Mouse wheel delta has changed. */
#define PAL_EVENT_TYPE_USER 18 /**< User-defined event. */
#define PAL_EVENT_TYPE_KEYCHAR 19 /**< Character key has been pressed. */
#define PAL_EVENT_TYPE_WINDOW_DECORATION_MODE 20 /**< Window decoration mode has been selected or changed. */
#define PAL_EVENT_TYPE_COUNT 21 /**< Number of event types. */
/** @} */

/**
 * @struct PalEvent
 * @brief Event.
 *
 * The payloads are packed in the `data` and `data2` fields of the struct.
 * User events define how their payloads are packed.
 * See [Event Payload](@ref event_payload) for more information.
 *
 * Filled in by @ref palPollEvent() or passed to the event callback.
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 */
typedef struct PalEvent {
    uint64_t data; /**< Data payload. */
    uint64_t data2; /**< Additional data payload. */
    uint32_t userId; /**< Identifier for user events. */
    PalEventType type; /**< Event type. */
} PalEvent;

/**
 * @typedef PalEventCallback
 * @brief Event callback function.
 *
 * `event` is only valid for the duration of the callback and must not be modified
 * or freed by the callback; the memory is owned by PAL. If the event must be used
 * after the callback returns, the callback must copy it. The callback must be
 * thread-safe if the event driver that uses it will be called from multiple threads.
 *
 * The function signature should look like this:
 * @code
 * void PAL_CALL eventCallback(void* userData, const PalEvent* event);
 * @endcode
 *
 * @param[in] userData User-defined data passed to the callback, or `nullptr`.
 * @param[in] event The event being dispatched.
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 *
 * @sa PalPushFn
 */
typedef void(PAL_CALL* PalEventCallback)(
    void* userData,
    const PalEvent* event);

#endif // PAL_EVENT_EVENT_H