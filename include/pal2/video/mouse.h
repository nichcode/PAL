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

#ifndef PAL_VIDEO_MOUSE_H
#define PAL_VIDEO_MOUSE_H

#include "pal2/core/defines.h"

/**
 * @defgroup mouse_buttons Mouse Buttons
 * @brief Mouse Buttons
 * 
 * @{
 */
#define PAL_MOUSE_BUTTON_UNKNOWN 0
#define PAL_MOUSE_BUTTON_LEFT 1
#define PAL_MOUSE_BUTTON_RIGHT 2
#define PAL_MOUSE_BUTTON_MIDDLE 3
#define PAL_MOUSE_BUTTON_X1 4
#define PAL_MOUSE_BUTTON_X2 5
#define PAL_MOUSE_BUTTON_COUNT 6
/** @} */

/**
 * @typedef PalMouseButton
 * @brief Buttons of a mouse.
 * 
 * All values of this type follow the format `PAL_MOUSE_BUTTON_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalMouseButton;

/**
 * @brief Gets the state of the mouse buttons.
 *
 * The returned pointer must not be freed. The state is updated when
 * `palUpdateVideo()` is called. The array must be index with 
 * PalMouseButton values and not exceed `PAL_MOUSE_BUTTON_COUNT`.
 *
 * @return A pointer to the mouse button array on success or `nullptr` on failure.
 *
 * @@Thread-safety Thread-safe.
 *
 * @since Added in version 2.0
 */
PAL_API const PalBool* PAL_CALL palGetMouseState();

/**
 * @brief Gets the relative movement of the mouse in desktop pixels.
 *
 * The relative movement will be updated when `palUpdateVideo()` is called.
 *
 * @param[in] dx The output to recieve the relative x. Can be `nullptr`.
 * @param[in] dy The output to recieve the relative y. Can be `nullptr`.
 *
 * @Thread-safety `dx` and `dy` must be per thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetMouseDelta(
    float* dx,
    float* dy);

/**
 * @brief Gets the wheel delta of the mouse.
 *
 * The wheel delta will be updated when `palUpdateVideo()` is called.
 *
 * @param[in] dx The output to recieve the x wheel delta. Can be `nullptr`.
 * @param[in] dy The output to recieve the y wheel delta. Can be `nullptr`.
 *
 * @Thread-safety `dx` and `dy` must be per thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetMouseWheelDelta(
    float* dx,
    float* dy);

#endif // PAL_VIDEO_MOUSE_H