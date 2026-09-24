/**
 * @file pal_eventtypes.h
 * @brief This is the header file for PAL Event Types API.
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

#ifndef PAL_EVENTTYPES_H
#define PAL_EVENTTYPES_H

#include "pal2/core/pal_types.h"

/**
 * @defgroup decoration_modes Decoration Modes
 * @brief Decoration modes for `PAL_EVENT_TYPE_WINDOW_DECORATION_MODE` event.
 * 
 * @{
 */
#define PAL_DECORATION_MODE_CLIENT_SIDE 0
#define PAL_DECORATION_MODE_SERVER_SIDE 1
#define PAL_DECORATION_MODE_COUNT 2
/** @} */

/**
 * @defgroup event_types Event Types
 * @brief Event types.
 * 
 * See [Event Payload](@ref event_payload) for how to get the payload for each
 * event type.
 * 
 * @{
 */
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
/** @} */

/**
 * @typedef PalEventType
 * @brief Event types.
 * 
 * All values of this type follow the format `PAL_EVENT_TYPE_*` for API
 * consistency and ease of use.
 * 
 * @since Added in version 2.0
 */
typedef uint32_t PalEventType;

/**
 * @typedef PalDecorationMode
 * @brief Decoration modes.
 * 
 * All values of this type follow the format `PAL_DECORATION_MODE_*` for API
 * consistency and ease of use.
 * 
 * @since Added in version 2.0
 */
typedef uint32_t PalDecorationMode;

/**
 * @struct PalEvent
 * @brief Contains information about an event.
 * 
 * The payloads are packed in the `::data` and `::data2` field of the struct.
 * 
 * User events defined how their payloads are packed.
 * 
 * @since Added in version 2.0
 * 
 * @var PalEvent::data
 * The first payload. The data is defined by `::type`.
 * 
 * @var PalEvent::data2
 * The second payload. The data is defined by `::type`.
 * 
 * @var PalEvent::userId
 * An additional payload for `PAL_EVENT_TYPE_USER` events. 
 * This is not used by the other event types.
 * 
 * @var PalEvent::type
 * The event type. This defines how data is a laid out in `::data`
 * and `::data2`. User events are excluded since users defined the @nl
 * payload structure.
 */
typedef struct PalEvent
{
    uint64_t data;
    uint64_t data2;
    uint32_t userId;
    PalEventType type;
} PalEvent;

/**
 * @typedef PalEventCallback
 * @brief Function pointer type used for event callbacks.
 * 
 * The function signature should look like this:
 * @code
 * void PAL_CALL eventCallback(void* userData, const PalEvent* event);
 * @endcode
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

#endif // PAL_EVENTTYPES_H