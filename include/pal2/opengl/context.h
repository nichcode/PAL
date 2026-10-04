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

#ifndef PAL_OPENGL_CONTEXT_H
#define PAL_OPENGL_CONTEXT_H

#include "fbconfig.h"

/**
 * @brief Creates an opengl context.
 * 
 * `PalGLContextCreateInfo::fbConfig` must be the same as the one used to
 * create `PalGLContextCreateInfo::window`. If the window has a different
 * framebuffer config, this function fails and sets the result code to 
 * `PAL_RESULT_CODE_INVALID_ARGUMENT`.
 * 
 * The created context will not be made current. Users are required to make
 * the context current on the thread they want.
 *
 * @param[in] info Information about how to create the context.
 * @param[out] context The output handle to recieve the created context.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety `context` must be per thread and the allocator used to
 * initialize the opengl system must be thread-safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 * 
 * @sa palDestroyGLContext
 */
PAL_API PalResult PAL_CALL palCreateGLContext(
    const PalGLContextCreateInfo* info,
    PalGLContext** context);

/**
 * @brief Destroys the opengl context.
 *
 * The context must not be current on any thread before.
 *
 * @param[in] context The context.
 *
 * @Thread-safety `context` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 * 
 * @sa palCreateGLContext
 */
PAL_API void PAL_CALL palDestroyGLContext(PalGLContext* context);

/**
 * @brief Makes the context current on the calling thread.
 * 
 * The window must have the same framebuffer config used to create context.
 * 
 * Set window and context to `nullptr` to unmake the context current on the
 * calling thread.
 *
 * @param[in] window The opengl window.
 * @param[in] context The context.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety One thread may have the current context at a time.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
PAL_API PalResult PAL_CALL palMakeContextCurrent(
    PalGLWindow* window,
    PalGLContext* context);

/**
 * @brief Presents the contents of the back buffer of the context to
 * the screen.
 *
 * @param[in] window The opengl window.
 * @param[in] context The context.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must only be called from a thread that has a bound context.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 * 
 * @sa palMakeContextCurrent
 */
PAL_API PalResult PAL_CALL palSwapBuffers(
    PalGLWindow* window,
    PalGLContext* context);

/**
 * @brief Sets the swap interval for the current context.
 *
 * This affects the currently bound context on the calling thread.
 * 
 * `PAL_GL_EXTENSION_SWAP_CONTROL` must be supported 
 * otherwise undefined behavoir.
 *
 * @param[in] interval The swap interval
 *
 * @Thread-safety Must only be called from a thread with a bound
 * context.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 * 
 * @sa palMakeContextCurrent
 */
PAL_API void PAL_CALL palSetSwapInterval(int32_t interval);

#endif // PAL_OPENGL_CONTEXT_H