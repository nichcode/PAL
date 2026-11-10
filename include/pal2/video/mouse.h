
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

#ifndef PAL_VIDEO_MOUSE_H
#define PAL_VIDEO_MOUSE_H

#include "types.h"

/**
 * @defgroup mouse_buttons Mouse Buttons
 * @ingroup pal_video
 * @{
 */
#define PAL_MOUSE_BUTTON_UNKNOWN 0 /**< Unknown mouse button. */
#define PAL_MOUSE_BUTTON_LEFT 1 /**< Left button. */
#define PAL_MOUSE_BUTTON_RIGHT 2 /**< Right button. */
#define PAL_MOUSE_BUTTON_MIDDLE 3 /**< Middle button. */
#define PAL_MOUSE_BUTTON_X1 4 /**< First extra button. */
#define PAL_MOUSE_BUTTON_X2 5 /**< Second extra button. */
#define PAL_MOUSE_BUTTON_COUNT 6 /**< Number of mouse buttons. */
/** @} */

/**
 * @brief Gets the state of the mouse buttons.
 *
 * This function returns the state of the buttons of the mouse.
 * The returned pointer must not be freed. The state is updated when @ref palUpdateVideo() is called.
 * The returned array contains one PalBool for each button, indexed by the corresponding constant
 * (for example `PAL_MOUSE_BUTTON_LEFT`). Indices must be less than @ref PAL_MOUSE_BUTTON_COUNT.
 *
 * @return Pointer to the mouse button array on success, or `nullptr` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API const PalBool* PAL_CALL palGetMouseState(void);

/**
 * @brief Gets the relative movement of the mouse.
 *
 * The relative movement will be updated when @ref palUpdateVideo() is called.
 *
 * @param[out] dx Output to receive the relative x movement, or `nullptr` to ignore it.
 * @param[out] dy Output to receive the relative y movement, or `nullptr` to ignore it.
 *
 * @Thread-safety `dx` and `dy` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palGetMouseDelta(
    float* dx,
    float* dy);

/**
 * @brief Gets the wheel delta of the mouse.
 *
 * The wheel delta will be updated when @ref palUpdateVideo() is called.
 *
 * @param[out] dx Output to receive the x wheel delta, or `nullptr` to ignore it.
 * @param[out] dy Output to receive the y wheel delta, or `nullptr` to ignore it.
 *
 * @Thread-safety `dx` and `dy` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palGetMouseWheelDelta(
    float* dx,
    float* dy);

#endif // PAL_VIDEO_MOUSE_H