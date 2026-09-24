/**
 * @file pal_context.h
 * @brief This is the header file for PAL OpenGL context API.
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

#ifndef PAL_CONTEXT_H
#define PAL_CONTEXT_H

#include "pal_fbconfig.h"

/**
 * @defgroup opengl_profiles OpenGL Profiles
 * @brief OpenGL profiles
 * 
 * @{
 */
#define PAL_GL_PROFILE_NONE 0
#define PAL_GL_PROFILE_CORE 1
#define PAL_GL_PROFILE_COMPATIBILITY 2
#define PAL_GL_PROFILE_ES 3
#define PAL_GL_PROFILE_COUNT 4
/** @} */

/**
 * @defgroup opengl_context_reset OpenGL Context Resets
 * @brief OpenGL context resets
 * 
 * @{
 */
#define PAL_GL_CONTEXT_RESET_NONE 0
#define PAL_GL_CONTEXT_RESET_NO_NOTIFICATION 1
#define PAL_GL_CONTEXT_RESET_LOSE_CONTEXT 2
#define PAL_GL_CONTEXT_RESET_COUNT 3
/** @} */

/**
 * @defgroup opengl_release OpenGL Release Behaviors
 * @brief OpenGL release behaviors
 * 
 * @{
 */
#define PAL_GL_RELEASE_BEHAVIOR_NONE 0
#define PAL_GL_RELEASE_BEHAVIOR_FLUSH 1
#define PAL_GL_RELEASE_BEHAVIOR_COUNT 2
/** @} */

/**
 * @typedef PalGLProfile
 * @brief Opengl context creation profiles.
 * 
 * All values of this type follow the format `PAL_GL_PROFILE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalGLProfile;

/**
 * @typedef PalGLContextReset
 * @brief Opengl context reset behavior.
 * 
 * All values of this type follow the format `PAL_GL_CONTEXT_RESET_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalGLContextReset;

/**
 * @typedef PalGLReleaseBehavior
 * @brief Opengl context release behavior.
 * 
 * All values of this type follow the format `PAL_GL_RELEASE_BEHAVIOR_*` for
 * API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalGLReleaseBehavior;

/**
 * @struct PalGLContext
 * @brief Opaque handle to an opengl context.
 *
 * @since Added in version 2.0
 */
typedef struct PalGLContext PalGLContext;

/**
 * @struct PalGLWindow
 * @brief Contains information about an opengl window.
 *
 * This can be allocated statically or dynamically.
 * The handles will not be copied.
 *
 * @since Added in version 2.0
 * 
 * @var PalGLWindow::instance
 * The instance or display (eg. `HINSTANCE` or `wl_display`).
 * 
 * @var PalGLWindow::window
 * The window handle (eg. `HWND` or `egl_wl_window`).
 */
typedef struct PalGLWindow
{
    void* instance;
    void* window;
} PalGLWindow;

/**
 * @struct PalGLContextCreateInfo
 * @brief Contains creation parameters of an opengl context.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalGLContextCreateInfo::window
 * The window to create the context for. Must not be `nullptr`.
 * 
 * @var PalGLContextCreateInfo::fbConfig
 * The opengl framebuffer config to use. Must not be `nullptr`.
 * 
 * @var PalGLContextCreateInfo::shareContext
 * The primary context to share resource ownership with. Can be `nullptr`.
 * 
 * @var PalGLContextCreateInfo::profile
 * The opengl context profile.
 * - `PAL_GL_PROFILE_NONE:` create a default profile context. @nl
 * - `PAL_GL_PROFILE_CORE:` create a core profile context.
 *   `PAL_GL_EXTENSION_CONTEXT_PROFILE` Must be supported. @nl 
 * - `PAL_GL_PROFILE_COMPATIBILITY:` create a compatibility profile context.
 *   `PAL_GL_EXTENSION_CONTEXT_PROFILE` Must be supported. @nl
 * - `PAL_GL_PROFILE_COMPATIBILITY:` create an `ES` profile context.
 *   `PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2` Must be supported.
 * 
 * @var PalGLContextCreateInfo::reset
 * The context reset behavior.
 * - `PAL_GL_CONTEXT_RESET_NONE:` default context reset behavior. @nl
 * - `PAL_GL_CONTEXT_RESET_NO_NOTIFICATION:` context will be reset by driver.
 *   `PAL_GL_EXTENSION_ROBUSTNESS` must be supported. @nl
 * - `PAL_GL_CONTEXT_RESET_LOSE_CONTEXT:` invalidate the context on reset.
 *   `PAL_GL_EXTENSION_ROBUSTNESS` must be supported.
 * 
 * @var PalGLContextCreateInfo::release
 * The context release behavior.
 * - `PAL_GL_RELEASE_BEHAVIOR_NONE:` default context release behavior. @nl
 * - `PAL_GL_RELEASE_BEHAVIOR_FLUSH:` flush context before release.
 *   `PAL_GL_EXTENSION_FLUSH_CONTROL` must be supported.
 * 
 * @var PalGLContextCreateInfo::forward
 * Create a forward compatible context.
 * `PAL_GL_EXTENSION_CREATE_CONTEXT` Must be supported.
 * 
 * @var PalGLContextCreateInfo::noError
 * Create a context with no error checking or reporting.
 * Set `::debug` to `PAL_FALSE` for maximum performance.
 * 
 * @var PalGLContextCreateInfo::debug
 * Create a debug context.
 * 
 * @var PalGLContextCreateInfo::major
 * The major version of the context. The driver must support the version.
 * 
 * @var PalGLContextCreateInfo::minor
 * The minor version of the context. The driver must support the version.
 */
typedef struct PalGLContextCreateInfo
{
    const PalGLWindow* window;
    const PalGLFBConfig* fbConfig;
    PalGLContext* shareContext;
    PalGLProfile profile;
    PalGLContextReset reset;
    PalGLReleaseBehavior release;
    PalBool forward;
    PalBool noError;
    PalBool debug;
    uint32_t major;
    uint32_t minor;
} PalGLContextCreateInfo;

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
 * @sa palMakeContextCurrent
 */
PAL_API void PAL_CALL palSetSwapInterval(int32_t interval);

#endif // PAL_CONTEXT_H