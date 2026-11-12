
/**
 * @file icon.h
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

#ifndef PAL_VIDEO_ICON_H
#define PAL_VIDEO_ICON_H

#include "pal2/core/base.h"
#include "pal2/core/result.h"
#include "window.h"

/**
 * @struct PalIcon
 * @brief Opaque handle to an icon.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalIcon PalIcon;

/**
 * @struct PalIconCreateInfo
 * @brief Icon creation parameters.
 *
 * This struct is used only during @ref palCreateIcon() and may be
 * discarded after the function returns.
 *
 * Coordinates are relative to the upper-left corner of the icon.
 * X-coordinate increases to the right and Y-coordinate increases down.
 *
 * `pixels` must be little-endian 32-bit, RGBA 8-bits per channel.
 * The pixels are copied, so they can be freed after the icon is created.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalIconCreateInfo {
    const uint8_t* pixels; /**< Icon pixels. */
    uint32_t width; /**< Icon width in pixels. */
    uint32_t height; /**< Icon height in pixels. */
} PalIconCreateInfo;

/**
 * @brief Creates an icon.
 *
 * The created icon must be destroyed using @ref palDestroyIcon().
 *
 * This function creates an icon from the provided pixels.
 * The created icon should be set to the window with @ref palSetWindowIcon().
 * The provided pixels are copied when the icon is created, so the data may be freed
 * afterwards. @ref PAL_VIDEO_FEATURE_WINDOW_SET_ICON feature must be supported.
 *
 * @param[in] info Icon creation parameters.
 * @param[out] icon Output handle to receive the created icon.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety Must be called from the main thread.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palDestroyIcon
 * @sa palSetWindowIcon
 */
PAL_API PalResult PAL_CALL palCreateIcon(
    const PalIconCreateInfo* info,
    PalIcon** icon);

/**
 * @brief Destroys an icon.
 *
 * Any window still using the icon must be reverted to the platform default by
 * calling @ref palSetWindowIcon() with `nullptr` before this function.
 *
 * @param[in] icon Icon to destroy.
 *
 * @thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palCreateIcon
 */
PAL_API void PAL_CALL palDestroyIcon(PalIcon* icon);

/**
 * @brief Sets the icon for the window.
 *
 * A single icon can be set to multiple windows at the same time. `icon` will not be copied;
 * it must remain valid until the window is destroyed or reverted. Destroying the icon and not
 * setting it to `nullptr` will result in undefined behavior if windows are referencing it.
 * @ref PAL_VIDEO_FEATURE_WINDOW_SET_ICON feature must be supported or this function results
 * in undefined behavior.
 *
 * @param[in] window Window to set icon on.
 * @param[in] icon Icon or `nullptr` to revert to the platform default.
 *
 * @thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palSetWindowIcon(
    PalWindow* window,
    PalIcon* icon);

#endif // PAL_VIDEO_ICON_H