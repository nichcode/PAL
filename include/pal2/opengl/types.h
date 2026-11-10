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

#ifndef PAL_OPENGL_TYPES_H
#define PAL_OPENGL_TYPES_H

#include "pal2/core/types.h"

#ifdef _WIN32
/**
 * @brief Calling convention of OpenGL functions.
 * @ingroup pal_opengl
 */
#define PAL_GL_APIENTRY __stdcall
#else
/**
 * @brief Calling convention of OpenGL functions.
 * @ingroup pal_opengl
 */
#define PAL_GL_APIENTRY
#endif // _WIN32

/**
 * @typedef PalGLExtensions
 * @brief OpenGL extensions.
 *
 * Identifies the OpenGL extensions supported by the driver. This is a bitmask of all
 * supported extensions of the OpenGL system known to PAL.
 *
 * All values of this type follow the format `PAL_GL_EXTENSION_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
typedef uint64_t PalGLExtensions;

/**
 * @typedef PalGLBackend
 * @brief OpenGL backend.
 *
 * Identifies the backend used by the OpenGL driver.
 *
 * All values of this type follow the format `PAL_GL_BACKEND_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
typedef uint32_t PalGLBackend;

/**
 * @typedef PalGLAPI
 * @brief OpenGL API.
 *
 * Identifies the kind of OpenGL API used by the driver.
 *
 * All values of this type follow the format `PAL_GL_API_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
typedef uint32_t PalGLAPI;

/**
 * @typedef PalGLProfile
 * @brief OpenGL context profile.
 *
 * Identifies the profile used to create an OpenGL context.
 *
 * All values of this type follow the format `PAL_GL_PROFILE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
typedef uint32_t PalGLProfile;

/**
 * @typedef PalGLContextReset
 * @brief OpenGL context reset behavior.
 *
 * Identifies how a context behaves when a graphics reset occurs.
 *
 * All values of this type follow the format `PAL_GL_CONTEXT_RESET_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
typedef uint32_t PalGLContextReset;

/**
 * @typedef PalGLReleaseBehavior
 * @brief OpenGL context release behavior.
 *
 * Identifies how a context behaves when it is released.
 *
 * All values of this type follow the format `PAL_GL_RELEASE_BEHAVIOR_*` for
 * API consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
typedef uint32_t PalGLReleaseBehavior;

/**
 * @struct PalGLContext
 * @brief Opaque handle to an OpenGL context.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
typedef struct PalGLContext PalGLContext;

#endif // PAL_OPENGL_TYPES_H