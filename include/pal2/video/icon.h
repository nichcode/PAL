/**
 * @file icon.h
 * @brief This is the header file for PAL Icon API.
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

#ifndef ICON_H
#define ICON_H

#include "window.h"

/**
 * @struct PalIcon
 * @brief Opaque handle to an icon.
 *
 * @since Added in version 2.0
 */
typedef struct PalIcon PalIcon;

/**
 * @struct PalIconCreateInfo
 * @brief Contains creation parameters of an icon.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalIconCreateInfo::pixels
 * The pixels to use to create the icon. It must be in `RGBA` format.
 * 
 * @var PalIconCreateInfo::width
 * The width of the icon in pixels.
 * 
 * @var PalIconCreateInfo::height
 * The height of the icon in pixels.
 */
typedef struct PalIconCreateInfo
{
    const uint8_t* pixels;
    uint32_t width;
    uint32_t height;
} PalIconCreateInfo;

/**
 * @brief Creates an icon.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_ICON` must be supported 
 * otherwise undefined behavior.
 *
 * @param[in] info Information about how to create the icon.
 * @param[out] icon The output handle to recieve the created icon.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palDestroyIcon
 */
PAL_API PalResult PAL_CALL palCreateIcon(
    const PalIconCreateInfo* info,
    PalIcon** icon);

/**
 * @brief Destroys the icon.
 *
 * @param[in] icon The icon.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palCreateIcon
 */
PAL_API void PAL_CALL palDestroyIcon(PalIcon* icon);

/**
 * @brief Sets the icon for the window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_ICON` must be supporte
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] icon The icon. Set to `nullptr` to revert to default.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palSetWindowIcon(
    PalWindow* window,
    PalIcon* icon);

#endif // ICON_H