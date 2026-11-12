/**
 * @file context.h
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

#ifndef PAL_OPENGL_CONTEXT_H
#define PAL_OPENGL_CONTEXT_H

#include "pal2/core/base.h"
#include "pal2/core/result.h"
#include "fbconfig.h"

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

/**
 * @defgroup opengl_profiles OpenGL Context Profiles
 * @ingroup pal_opengl
 * @{
 */
#define PAL_GL_PROFILE_NONE 0 /**< Driver default. */
#define PAL_GL_PROFILE_CORE 1 /**< Core profile. */
#define PAL_GL_PROFILE_COMPATIBILITY 2 /**< Compatibility profile. */
#define PAL_GL_PROFILE_ES 3 /**< OpenGL ES profile. */
#define PAL_GL_PROFILE_COUNT 4 /**< Number of OpenGL profiles. */
/** @} */

/**
 * @defgroup opengl_context_reset OpenGL Context Reset Behaviors
 * @ingroup pal_opengl
 * @{
 */
#define PAL_GL_CONTEXT_RESET_NONE 0 /**< Driver default. */
#define PAL_GL_CONTEXT_RESET_NO_NOTIFICATION 1 /**< No notification is delivered when a reset occurs. */
#define PAL_GL_CONTEXT_RESET_LOSE_CONTEXT 2 /**< Context is lost when a reset occurs. */
#define PAL_GL_CONTEXT_RESET_COUNT 3 /**< Number of context reset behaviors. */
/** @} */

/**
 * @defgroup opengl_release OpenGL Context Release Behaviors
 * @ingroup pal_opengl
 * @{
 */
#define PAL_GL_RELEASE_BEHAVIOR_NONE 0 /**< Driver default. */
#define PAL_GL_RELEASE_BEHAVIOR_FLUSH 1 /**< Flush context before release. */
#define PAL_GL_RELEASE_BEHAVIOR_COUNT 2 /**< Number of context release behaviors. */
/** @} */

/**
 * @struct PalGLWindow
 * @brief OpenGL window.
 *
 * This may be allocated statically or dynamically. The OpenGL system does not
 * copy or take ownership of the native handles in the struct.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
typedef struct PalGLWindow {
    void* instance; /**< Native instance. */
    void* window; /**< Native window handle. */
} PalGLWindow;

/**
 * @struct PalGLContextCreateInfo
 * @brief OpenGL context creation parameters.
 *
 * This struct is used only during @ref palCreateGLContext() and may be
 * discarded after the function returns. The structs referenced by this struct are not
 * copied and must remain valid for the duration of that function.
 *
 * `fbConfig` must be compatible with the specified window.
 * The requested major and minor version must be supported by the
 * driver. Creating a forward-compatible context requires the @ref PAL_GL_EXTENSION_CREATE_CONTEXT
 * extension to be supported.
 *
 * The following context profiles require extension support:
 * - @ref PAL_GL_PROFILE_CORE requires @ref PAL_GL_EXTENSION_CONTEXT_PROFILE.
 * - @ref PAL_GL_PROFILE_COMPATIBILITY requires @ref PAL_GL_EXTENSION_CONTEXT_PROFILE.
 * - @ref PAL_GL_PROFILE_ES requires @ref PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2.
 *
 * The following context reset behaviors require @ref PAL_GL_EXTENSION_ROBUSTNESS:
 * - @ref PAL_GL_CONTEXT_RESET_NO_NOTIFICATION.
 * - @ref PAL_GL_CONTEXT_RESET_LOSE_CONTEXT.
 *
 * @ref PAL_GL_RELEASE_BEHAVIOR_FLUSH requires @ref PAL_GL_EXTENSION_FLUSH_CONTROL.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
typedef struct PalGLContextCreateInfo {
    const PalGLWindow* window; /**< Window the context will be created for. */
    const PalGLFBConfig* fbConfig; /**< Framebuffer configuration to use. */
    PalGLContext* shareContext; /**< Context to share resources with, or `nullptr` for no sharing. */
    PalGLProfile profile; /**< Context profile. */
    PalGLContextReset reset; /**< Context reset behavior. */
    PalGLReleaseBehavior release; /**< Context release behavior. */
    PalBool forward; /**< Whether to create a forward-compatible context. */
    PalBool noError; /**< Whether to create a no-error context. */
    PalBool debug; /**< Whether to create a debug context. */
    uint32_t major; /**< Context major version. */
    uint32_t minor; /**< Context minor version. */
} PalGLContextCreateInfo;

/**
 * @brief Creates an OpenGL context.
 *
 * The created context must be destroyed using @ref palDestroyGLContext().
 *
 * This function creates an OpenGL context using the specified creation parameters.
 * `info` must remain valid for the duration of this function. PAL does not
 * copy its contents.
 *
 * `info->fbConfig` is the framebuffer configuration the context will be created
 * with. The specified window must use the same configuration. Once a
 * framebuffer configuration has been selected by the specified window,
 * the OpenGL system cannot change it. The window must be destroyed and recreated
 * to change the configuration.
 *
 * If `info->shareContext` is not `nullptr`, the created context shares resources
 * with that context. The created context will not be made current. Use
 * @ref palMakeContextCurrent() to make the context current on the calling thread.
 *
 * The OpenGL system must be initialized before this call.
 *
 * @param[in] info Context creation parameters.
 * @param[out] context Output handle to receive the created context.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `context` must be per thread.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @note On Wayland, `info->window->window` is `wl_egl_window`.
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
 * @brief Destroys an OpenGL context.
 *
 * This function destroys the specified context and frees any resources
 * owned by the context. The context must not be current on any thread when
 * this function is called. Use @ref palMakeContextCurrent() with `context`
 * set to `nullptr` to release the context if it is current.
 *
 * If `context` shares resources with other contexts, the resources will not
 * be destroyed according to OpenGL rules.
 *
 * @param[in] context Context to destroy.
 *
 * @thread-safety `context` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 *
 * @sa palCreateGLContext
 */
PAL_API void PAL_CALL palDestroyGLContext(PalGLContext* context);

/**
 * @brief Makes an OpenGL context current on the calling thread.
 *
 * This function makes the specified OpenGL context current on the calling thread
 * and associates it to the specified window. A thread can only have one current
 * context. If another context is already current on the calling thread,
 * it will be replaced by `context`.
 *
 * The framebuffer configuration of the specified window must be compatible with
 * the one used to create the context. When `context` is `nullptr`, the current
 * context of the calling thread is released. In this case, `window` will be ignored
 * even if it is not `nullptr`.
 *
 * When switching from one current context to another, the previous context is flushed
 * according to the behavior used to create the context.
 *
 * @param[in] window OpenGL window to associate the context with.
 * @param[in] context Context to make current, or `nullptr` to release the current context.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety A context may be current to only one thread at a time.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
PAL_API PalResult PAL_CALL palMakeContextCurrent(
    PalGLWindow* window,
    PalGLContext* context);

/**
 * @brief Swaps the front and back buffers of an OpenGL window.
 *
 * This function swaps the front and back buffers of the specified window using
 * `context`. The specified window and context must have been created with a
 * double-buffered framebuffer configuration. If the configuration is single-buffered,
 * behavior is undefined.
 *
 * `context` must be current on the calling thread. The buffers are swapped according
 * to the platform rules.
 *
 * @param[in] window OpenGL window whose buffers will be swapped.
 * @param[in] context Context to associate with the OpenGL window.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `context` must be externally synchronized.
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
 * This function sets the minimum number of video frame periods that must elapse
 * between buffer swaps for the window associated with the current context of the
 * calling thread. An interval of `0` disables synchronization of buffer swaps.
 *
 * The @ref PAL_GL_EXTENSION_SWAP_CONTROL_TEAR extension must be supported if negative
 * swap intervals will be used. Negative intervals allow the driver to swap
 * immediately even if a frame arrives late.
 *
 * The @ref PAL_GL_EXTENSION_SWAP_CONTROL extension must be supported,
 * otherwise the behavior is undefined.
 *
 * @param[in] interval Minimum number of video frame periods between swaps.
 *
 * @thread-safety Must be called from a thread with a bound context.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 *
 * @sa palMakeContextCurrent
 */
PAL_API void PAL_CALL palSetSwapInterval(int32_t interval);

#endif // PAL_OPENGL_CONTEXT_H