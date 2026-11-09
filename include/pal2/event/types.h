
/**
 * @file types.h
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

#ifndef PAL_EVENT_TYPES_H
#define PAL_EVENT_TYPES_H

#include "pal2/core/types.h"

/**
 * @typedef PalDecorationMode
 * @brief Window decoration mode.
 *
 * Identifies whether the application or the windowing system draws the window decorations.
 *
 * All values of this type follow the format `PAL_DECORATION_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 */
typedef uint32_t PalDecorationMode;

/**
 * @typedef PalEventType
 * @brief Event type.
 *
 * Identifies the kind of an event.
 *
 * All values of this type follow the format `PAL_EVENT_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 */
typedef uint32_t PalEventType;

/**
 * @typedef PalDispatchMode
 * @brief Event dispatch mode.
 *
 * Identifies where an event of a given type is delivered.
 *
 * All values of this type follow the format `PAL_DISPATCH_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 */
typedef uint32_t PalDispatchMode;

/**
 * @struct PalEventDriver
 * @brief Opaque handle to an event driver.
 *
 * @since Added in version 2.0
 * @ingroup pal_event
 */
typedef struct PalEventDriver PalEventDriver;

#endif // PAL_EVENT_TYPES_H