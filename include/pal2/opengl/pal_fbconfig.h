/**
 * @file pal_fbconfig.h
 * @brief This is the header file for PAL OpenGL framebuffer configuration API.
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

#ifndef PAL_FBCONFIG_H
#define PAL_FBCONFIG_H

#include "pal2/core/pal_types.h"
#include "pal2/core/pal_result.h"

/**
 * @struct PalGLFBConfig
 * @brief Contains information about an opengl framebuffer.
 *
 * @since Added in version 2.0
 * 
 * @var PalGLFBConfig::doubleBuffer
 * If `PAL_TRUE`, double buffering is supported.
 * 
 * @var PalGLFBConfig::stereo
 * If `PAL_TRUE`, stereo is supported.
 * 
 * @var PalGLFBConfig::sRGB
 * If `PAL_TRUE`, SRGB colorspace is supported.
 * 
 * @var PalGLFBConfig::index
 * The driver index of the opengl framebuffer config. 
 * Its driver specific and must not be changed.
 * 
 * @var PalGLFBConfig::redBits
 * The number of bits in the red channel.
 * 
 * @var PalGLFBConfig::greenBits
 * The number of bits in the green channel.
 *
 * @var PalGLFBConfig::blueBits
 * The number of bits in the blue channel.
 * 
 * @var PalGLFBConfig::alphaBits
 * The number of bits in the alpha channel.
 * 
 * @var PalGLFBConfig::depthBits
 * The number of depth buffer bits.
 * 
 * @var PalGLFBConfig::stencilBits
 * The number of stencil buffer bits.
 * 
 * @var PalGLFBConfig::samples
 * The number of `MSAA` samples.
 */
typedef struct PalGLFBConfig
{
    PalBool doubleBuffer;
    PalBool stereo;
    PalBool sRGB;
    uint16_t index;
    uint16_t redBits;
    uint16_t greenBits;
    uint16_t blueBits;
    uint16_t alphaBits;
    uint16_t depthBits;
    uint16_t stencilBits;
    uint16_t samples;
} PalGLFBConfig;

#endif // PAL_FBCONFIG_H