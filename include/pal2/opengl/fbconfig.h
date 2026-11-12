/**
 * @file fbconfig.h
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

#ifndef PAL_OPENGL_FBCONFIG_H
#define PAL_OPENGL_FBCONFIG_H

#include "pal2/core/base.h"
#include "pal2/core/result.h"

/**
 * @struct PalGLFBConfig
 * @brief OpenGL framebuffer configuration.
 *
 * Filled in by @ref palEnumerateGLFBConfigs(). It is also used to describe the desired
 * configuration passed to @ref palGetClosestGLFBConfig(); in that case, uninitialized fields
 * may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
typedef struct PalGLFBConfig {
    PalBool doubleBuffer; /**< Whether double buffering is supported. */
    PalBool stereo; /**< Whether stereo is supported. */
    PalBool sRGB; /**< Whether sRGB colorspace is supported. */
    uint16_t index; /**< Driver configuration index. */
    uint16_t redBits; /**< Number of bits in the red channel. */
    uint16_t greenBits; /**< Number of bits in the green channel. */
    uint16_t blueBits; /**< Number of bits in the blue channel. */
    uint16_t alphaBits; /**< Number of bits in the alpha channel. */
    uint16_t depthBits; /**< Number of depth buffer bits. */
    uint16_t stencilBits; /**< Number of stencil buffer bits. */
    uint16_t samples; /**< Number of samples per pixel. */
} PalGLFBConfig;

/**
 * @brief Enumerates supported OpenGL framebuffer configurations.
 *
 * This function gets all framebuffer configurations supported by the driver.
 * This is a two-call function, set `configs` to `nullptr` and `count` to
 * `0` to get the number of supported framebuffer configurations. Allocate the array
 * and call this function again with `configs` set to the allocated array and
 * `count` set to the capacity of the array.
 *
 * If the specified count is less than the number of supported framebuffer configurations,
 * only the configurations that fit in the array will be written.
 *
 * The configurations are copied into the array. The OpenGL system must be initialized
 * before this call.
 *
 * @param[in, out] count Capacity of the framebuffer configuration array.
 * @param[out] configs Output array to receive the framebuffer configurations, or `nullptr` to get the number of
 *                     framebuffer configurations.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety Must be called from the main thread.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
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
 * @brief Gets the closest matching OpenGL framebuffer configuration to a desired configuration.
 *
 * This function compares the specified framebuffer configuration array against
 * the desired configuration and returns the closest matching configuration according
 * to the OpenGL framebuffer configuration rules.
 *
 * `configs` array must have been obtained from @ref palEnumerateGLFBConfigs().
 * The returned pointer, if not `nullptr`, is an element in the configuration
 * array and must not be freed separately. The contents of the configuration
 * array will not be modified by this function.
 *
 * @param[in] configs Framebuffer configuration array.
 * @param[in] count Number of elements in the `configs` array.
 * @param[in] desired Desired framebuffer configuration.
 * @return Closest matching framebuffer configuration on success or `nullptr` on failure.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
PAL_API const PalGLFBConfig* PAL_CALL palGetClosestGLFBConfig(
    PalGLFBConfig* configs,
    uint32_t count,
    const PalGLFBConfig* desired);

#endif // PAL_OPENGL_FBCONFIG_H