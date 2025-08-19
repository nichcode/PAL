
/**

Copyright (C) 2025 Nicholas Agbo

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

/**
 * @file pal_opengl.h
 *
 * Header file for opengl functions, macros, enum and structs
 *
 * @defgroup opengl
 */

#ifndef _PAL_OPENGL_H
#define _PAL_OPENGL_H

#include "pal_core.h"

/**
 * @struct PalGLContext
 * @brief Opaque handle to an opengl context.
 *
 * @ingroup opengl
 */
typedef struct PalGLContext PalGLContext;

typedef enum PalGLExtensions {
    PAL_GL_EXTENSION_CREATE_CONTEXT = PAL_BIT(0),
    PAL_GL_EXTENSION_CONTEXT_PROFILE = PAL_BIT(1),
    PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2 = PAL_BIT(2),
    PAL_GL_EXTENSION_ROBUSTNESS = PAL_BIT(3),
    PAL_GL_EXTENSION_NO_ERROR = PAL_BIT(4),
    PAL_GL_EXTENSION_PIXEL_FORMAT = PAL_BIT(5),
    PAL_GL_EXTENSION_MULTISAMPLE = PAL_BIT(6),
    PAL_GL_EXTENSION_SWAP_CONTROL = PAL_BIT(7),
    PAL_GL_EXTENSION_COLORSPACE_SRGB = PAL_BIT(8)
} PalGLExtensions;

typedef struct PalGLInfo {
    char graphicsCard[64];
    char version[64];
    char vendor[32];
    PalGLExtensions extensions;
    Int32 versionMajor;
    Int32 versionMinor;
} PalGLInfo;

typedef struct PalGLPixelFormat {
    Int32 index;
    Int32 redBits;
    Int32 greenBits;
    Int32 blueBits;
    Int32 alphaBits;
    Int32 depthBits;
    Int32 stencilBits;
    Int32 samples;
    bool doubleBuffer;
    bool stereo;
    bool sRGB;
} PalGLPixelFormat;

// TODO: docs
_PAPI PalResult _PCALL palLoadGLICD();

// TODO: docs
_PAPI PalGLInfo _PCALL palGetGLInfo();

// TODO: docs
_PAPI PalResult _PCALL palEnumerateGLPixelFormats(
    void* nativeWindow,
    Int32 *count,
    PalGLPixelFormat *formats);

// TODO: docs
_PAPI PalGLPixelFormat* _PCALL palGetClosestGLPixelFormat(
    PalGLPixelFormat *formats,
    Int32 count,
    const PalGLPixelFormat* desired);

#endif // _PAL_OPENGL_H