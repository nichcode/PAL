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

#ifndef PAL_VIDEO_KEYBOARD_H
#define PAL_VIDEO_KEYBOARD_H

#include "pal2/core/defines.h"

/**
 * @brief Gets the state of the keycodes (layout aware keys) of the
 * keyboard.
 *
 * The returned pointer must not be freed. The state is updated when
 * @ref palUpdateVideo is called. The array must be index with 
 * PalKeycode values and not exceed @ref PAL_KEYCODE_COUNT.
 *
 * @return A pointer to the keycodes array on success or nullptr on failure.
 *
 * @Thread-safety Thread-safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API const PalBool* PAL_CALL palGetKeycodeState();

/**
 * @brief Gets the state of the scancodes (layout independent keys) of
 * the keyboard.
 *
 * The returned pointer must not be freed. The state is updated when
 * @ref palUpdateVideo is called. The array must be index with 
 * PalScancodes values and not exceed PAL_SCANCODE_COUNT.
 *
 * @return A pointer to the scancodes array on success or nullptr on failure.
 *
 * @Thread-safety Thread-safe.
 *
 * @since Added in version 2.0
 */
PAL_API const PalBool* PAL_CALL palGetScancodeState();

#endif // PAL_VIDEO_KEYBOARD_H