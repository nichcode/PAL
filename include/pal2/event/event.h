/**
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

#include "pal2/core/defines.h"

/**
 * @defgroup decoration_modes Decoration Modes
 * @brief Decoration modes for `PAL_EVENT_TYPE_WINDOW_DECORATION_MODE` event.
 * 
 * @ingroup pal_event
 */

/**
 * @defgroup event_types Event Types
 * @brief Event types.
 * 
 * See [Event Payload](@ref event_payload) for how to get and use the payload.
 * 
 * @ingroup pal_event
 */

/** @brief A constant used to indicate the client (user) is responsible for
 * the window decorations.
 * 
 * @ingroup decoration_modes
*/
#define PAL_DECORATION_MODE_CLIENT_SIDE 0

/** @brief A constant used to indicate the server is responsible for
 * the window decorations.
 * 
 * @ingroup decoration_modes
*/
#define PAL_DECORATION_MODE_SERVER_SIDE 1

/** @brief A constant used to indicate the maximum number of decoration modes.
 * 
 * The literal value must not be used.
 * 
 * @ingroup decoration_modes
*/
#define PAL_DECORATION_MODE_COUNT 2

/** @brief The window close button has been clicked.
 * 
 * Payload:
 * 
 * - `event.data`: unused
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_WINDOW_CLOSE 0

/** @brief The window has been resized.
 * 
 * Payload:
 * 
 * - `event.data`: width (bits 0-31), height (bits 32-63).
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackUint32()
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_WINDOW_SIZE 1

/** @brief The window has been moved.
 * 
 * Payload:
 * 
 * - `event.data`: x (bits 0-31), y (bits 32-63).
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackInt32()
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_WINDOW_MOVE 2

/** @brief The window state has changed (eg. maximized, minimized).
 * 
 * Payload:
 * 
 * - `event.data`: window state (bits 0-31), unused (bits 32-63).
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_WINDOW_STATE 3

/** @brief The window has gained or lost focus.
 * 
 * Payload:
 * 
 * - `event.data`: focus boolean (bits 0-31), unused (bits 32-63).
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_WINDOW_FOCUS 4

/** @brief The window has been shown or hidden.
 * 
 * Payload:
 * 
 * - `event.data`: shown boolean (bits 0-31), unused (bits 32-63).
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_WINDOW_VISIBILITY 5

/** @brief The window has entered model mode.
 * 
 * Payload:
 * 
 * - `event.data`: unused
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_WINDOW_MODAL_BEGIN 6

/** @brief The window has existed model mode.
 * 
 * Payload:
 * 
 * - `event.data`: unused
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_WINDOW_MODAL_END 7

/** @brief The monitor DPI has changed.
 * 
 * Payload:
 * 
 * - `event.data`: DPI (bits 0-31), unused (bits 32-63).
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_MONITOR_DPI_CHANGED 8

/** @brief The number of connected monitors has changed.
 * 
 * Payload:
 * 
 * - `event.data`: unused
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_MONITOR_LIST_CHANGED 9

/** @brief A key was pressed.
 * 
 * Payload:
 * 
 * - `event.data`: keycode (bits 0-31), scancode (bits 32-63).
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackUint32()
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_KEYDOWN 10

/** @brief A pressed key is being held down.
 * 
 * Payload:
 * 
 * - `event.data`: keycode (bits 0-31), scancode (bits 32-63).
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackUint32()
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_KEYREPEAT 11

/** @brief A pressed key has been released.
 * 
 * Payload:
 * 
 * - `event.data`: keycode (bits 0-31), scancode (bits 32-63).
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackUint32()
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_KEYUP 12

/** @brief A mouse button has been pressed.
 * 
 * Payload:
 * 
 * - `event.data`: mouse button (bits 0-31), mouse serial (bits 32-63).
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackUint32()
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_MOUSE_BUTTONDOWN 13

/** @brief A mouse button has been released.
 * 
 * Payload:
 * 
 * - `event.data`: mouse button (bits 0-31), mouse serial (bits 32-63).
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackUint32()
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_MOUSE_BUTTONUP 14

/** @brief The mouse was moved.
 * 
 * Payload:
 * 
 * - `event.data`: x (bits 0-31), y (bits 32-63).
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackInt32()
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_MOUSE_MOVE 15

/** @brief The mouse movement delta has changed.
 * 
 * Payload:
 * 
 * - `event.data`: dx (bits 0-31), dy (bits 32-63).
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackFloat()
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_MOUSE_DELTA 16

/** @brief The mouse wheel delta has changed.
 * 
 * Payload:
 * 
 * - `event.data`: dx (bits 0-31), dy (bits 32-63).
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackFloat()
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_MOUSE_WHEEL 17

/** @brief A user event
 * Helpers:
 * 
 * - palUnpackFloat()
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_USER 18

/** @brief A character key has been pressed.
 * 
 * Payload:
 * 
 * - `event.data`: codepoint (bits 0-31), unused (bits 32-63).
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_KEYCHAR 19

/** @brief The window decoration mode has been selected.
 * 
 * Payload:
 * 
 * - `event.data`: decoration mode (bits 0-31), unused (bits 32-63).
 * 
 * - `event.data2`: window
 * 
 * Helpers:
 * 
 * - palUnpackPointer()
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_WINDOW_DECORATION_MODE 20

/** @brief The maximum number of event types. The literal value must
 * not be used.
 * 
 * @ingroup event_types
*/
#define PAL_EVENT_TYPE_COUNT 21

/**
 * @typedef PalEventType
 * @brief An event type
 * 
 * All values of this type follow the format `PAL_EVENT_TYPE_*` for API
 * consistency and ease of use.
 * 
 * @since Added in version 2.0
 * @ingroup pal_event
 */
typedef uint32_t PalEventType;

/**
 * @typedef PalDecorationMode
 * @brief A decoration mode
 * 
 * All values of this type follow the format `PAL_DECORATION_MODE_*` for API
 * consistency and ease of use.
 * 
 * @since Added in version 2.0
 * @ingroup pal_event
 */
typedef uint32_t PalDecorationMode;

/**
 * @struct PalEvent
 * @brief Information about an event.
 * 
 * The payloads are packed in the ::data and ::data2 fields of the struct.
 * 
 * User events defined how their payloads are packed.
 * 
 * @since Added in version 2.0
 * @ingroup pal_event
 */
typedef struct PalEvent
{
    /** The first data payload. The data is defined by ::type.*/
    uint64_t data;

    /** The second data payload. The data is defined by ::type.*/
    uint64_t data2;

    /**
     * The id for user events. This is an adaitional payload for user events.
     * This is not used by the other event types.
     */
    uint32_t userId;

    /**
     * The event type. This defines how data is a laid out in ::data and 
     * ::data2. User events define their own payload structure.
     */
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
 * The callback must be thread-safe if the event driver that uses it
 * is thread-safe.
 *
 * @param[in] userData User data passed from
 *                     PalEventDriverCreateInfo::userData. Can be nullptr.
 * @param[in] event The pushed event.
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