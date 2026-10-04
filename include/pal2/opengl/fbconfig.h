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

#ifndef PAL_OPENGL_FBCONFIG_H
#define PAL_OPENGL_FBCONFIG_H

#include "pal2/core/defines.h"
#include "pal2/core/result.h"

/**
 * @brief Returns a list of all supported framebuffer configs by 
 * the opengl driver.
 *
 * The opengl system must be initialized before this call.
 * 
 * Set `configs` to `nullptr` to get the total number of supported framebuffer
 * configs. If the configs array passed is less than the number of
 * supported framebuffer configs, PAL will fill the array upto that limit
 * sequentially.
 *
 * @param[in, out] count The capacity of the configs array.
 * @param[out] configs The configs array.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 * 
 * @sa palInitGL
 */
PAL_API PalResult PAL_CALL palEnumerateGLFBConfigs(
    uint32_t* count,
    PalGLFBConfig* configs);

/**
 * @brief Gets the closest match framebuffer config with a desired config.
 *
 * @param[in] configs The framebuffer configs array.
 * @param[in] count The capacity of the framebuffer configs array.
 * @param[in] desired The desired framebuffer config.
 *
 * @return The closest match framebuffer config on success or `nullptr`
 * on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
PAL_API const PalGLFBConfig* PAL_CALL palGetClosestGLFBConfig(
    PalGLFBConfig* configs,
    uint32_t count,
    const PalGLFBConfig* desired);

#endif // PAL_OPENGL_FBCONFIG_H