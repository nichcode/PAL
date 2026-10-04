/**
 * @brief This is the header file for PAL OpenGL API.
 *
 * It defines all the types and functions of the opengl module.
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

/**
 * @defgroup pal_opengl OpenGL Module
 * @{
 */

#ifndef PAL_OPENGL_H
#define PAL_OPENGL_H

#include "pal_core.h"

#ifdef _WIN32
#define PAL_GL_APIENTRY __stdcall
#else
#define PAL_GL_APIENTRY
#endif // _WIN32

#define PAL_GL_VENDOR_NAME_SIZE 32 /**< maximum vendor name size */
#define PAL_GL_GRAPHICS_CARD_NAME_SIZE 64 /**< maximum graphics card name size */
#define PAL_GL_VERSION_NAME_SIZE 64 /**< maximum version name size */

/**
 * @defgroup opengl_extensions OpenGL Extensions
 * @{
 */
#define PAL_GL_EXTENSION_CREATE_CONTEXT (1ULL << 0) /**< support for modern context */
#define PAL_GL_EXTENSION_CONTEXT_PROFILE (1ULL << 1) /**< support for creating profile context */
#define PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2 (1ULL << 2) /**< support for creating ES2 profile context */
#define PAL_GL_EXTENSION_ROBUSTNESS (1ULL << 3) /**< support for creating robust (reset behavior) context */
#define PAL_GL_EXTENSION_NO_ERROR (1ULL << 4) /**< support for creating no error context */
#define PAL_GL_EXTENSION_PIXEL_FORMAT (1ULL << 5) /**< support for modern framebuffer configurations */
#define PAL_GL_EXTENSION_MULTISAMPLE (1ULL << 6) /**< support for multisample context */
#define PAL_GL_EXTENSION_SWAP_CONTROL (1ULL << 7) /**< support for setting swap control options */
#define PAL_GL_EXTENSION_FLUSH_CONTROL (1ULL << 8) /**< support for setting flush control options */
#define PAL_GL_EXTENSION_COLORSPACE_SRGB (1ULL << 9) /**< support for sRGB colorspace */
#define PAL_GL_EXTENSION_SWAP_CONTROL_TEAR (1ULL << 10) /**< support for negative swap intervals */
/** @} */

/**
 * @defgroup opengl_backend OpenGL Backends
 * @{
 */
#define PAL_GL_BACKEND_EGL 0
#define PAL_GL_BACKEND_GLX 1
#define PAL_GL_BACKEND_WGL 2
#define PAL_GL_BACKEND_COUNT 3 /**< number of OpenGL backends */
/** @} */

/**
 * @defgroup opengl_apis OpenGL APIs
 * @{
 */
#define PAL_GL_API_OPENGL 0
#define PAL_GL_API_OPENGL_ES 1
#define PAL_GL_API_COUNT 2 /**< number of OpenGL APIs */
/** @} */

/**
 * @defgroup opengl_profiles OpenGL Context Profiles
 * @{
 */
#define PAL_GL_PROFILE_NONE 0 /**< driver default */
#define PAL_GL_PROFILE_CORE 1
#define PAL_GL_PROFILE_COMPATIBILITY 2
#define PAL_GL_PROFILE_ES 3
#define PAL_GL_PROFILE_COUNT 4 /**< number of OpenGL profiles */
/** @} */

/**
 * @defgroup opengl_context_reset OpenGL Context Reset Behaviors
 * @{
 */
#define PAL_GL_CONTEXT_RESET_NONE 0 /**< driver default */
#define PAL_GL_CONTEXT_RESET_NO_NOTIFICATION 1 /**< context will be reset by driver */
#define PAL_GL_CONTEXT_RESET_LOSE_CONTEXT 2 /**< invalidate context on reset */
#define PAL_GL_CONTEXT_RESET_COUNT 3 /**< number of context reset behaviors */
/** @} */

/**
 * @defgroup opengl_release OpenGL Context Release Behaviors
 * @{
 */
#define PAL_GL_RELEASE_BEHAVIOR_NONE 0 /**< driver default */
#define PAL_GL_RELEASE_BEHAVIOR_FLUSH 1 /**< flush context before release */
#define PAL_GL_RELEASE_BEHAVIOR_COUNT 2 /**< number of context release behaviors */
/** @} */

/**
 * @typedef PalGLExtensions
 * @brief OpenGL extensions.
 * 
 * This is a bitmask of all supported extensions of the opengl system known to PAL.
 * 
 * All values of this type follow the format `PAL_GL_EXTENSION_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint64_t PalGLExtensions;

/**
 * @typedef PalGLBackend
 * @brief OpenGL backend.
 * 
 * All values of this type follow the format `PAL_GL_BACKEND_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalGLBackend;

/**
 * @typedef PalGLAPI
 * @brief OpengGL API.
 * 
 * All values of this type follow the format `PAL_GL_API_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalGLAPI;

/**
 * @typedef PalGLProfile
 * @brief OpenGL context creation profiles.
 * 
 * All values of this type follow the format `PAL_GL_PROFILE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalGLProfile;

/**
 * @typedef PalGLContextReset
 * @brief OpenGL context reset behavior.
 * 
 * All values of this type follow the format `PAL_GL_CONTEXT_RESET_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalGLContextReset;

/**
 * @typedef PalGLReleaseBehavior
 * @brief OpenGL context release behavior.
 * 
 * All values of this type follow the format `PAL_GL_RELEASE_BEHAVIOR_*` for
 * API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalGLReleaseBehavior;

/**
 * @struct PalGLContext
 * @brief Opaque handle to an OpenGL context.
 *
 * @since Added in version 2.0
 */
typedef struct PalGLContext PalGLContext;
/** @} */

/**
 * @struct PalGLInfo
 * @brief OpenGL driver information.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
typedef struct PalGLInfo
{
    PalGLExtensions extensions; /**< bitmask of supported OpenGL extensions */
    uint32_t major; /**< major version */
    uint32_t minor; /**< minor version */
    PalGLBackend backend; /**< OpenGL backend of the driver */
    PalGLAPI api; /**< OpenGL API of the driver */
    char vendor[PAL_GL_VENDOR_NAME_SIZE]; /**< vendor name */
    char graphicsCard[PAL_GL_GRAPHICS_CARD_NAME_SIZE]; /**< graphics card name */
    char version[PAL_GL_VERSION_NAME_SIZE]; /**< version name */
} PalGLInfo;

/**
 * @struct PalGLFBConfig
 * @brief OpenGL framebuffer configuration.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
typedef struct PalGLFBConfig
{
    PalBool doubleBuffer; /**< whether double buffering is supported */
    PalBool stereo; /**< whether stereo is supported */
    PalBool sRGB; /**< whether sRGB colorspace is supported */
    uint16_t index; /**< driver configuration index */
    uint16_t redBits; /**< number of bits in the red channel */
    uint16_t greenBits; /**< number of bits in the green channel */
    uint16_t blueBits; /**< number of bits in the blue channel */
    uint16_t alphaBits; /**< number of bits in the alpha channel */
    uint16_t depthBits; /**< number of depth buffer bits */
    uint16_t stencilBits; /**< number of stencil buffer bits */
    uint16_t samples; /**< number of samples per pixel */
} PalGLFBConfig;

/**
 * @struct PalGLWindow
 * @brief OpenGL window.
 * 
 * This may be allocated statically or dynamically. The opengl system does not 
 * copy or take ownership of the native handles in the struct.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
typedef struct PalGLWindow {
    void* instance; /**< native instance */
    void* window; /**< native window handle */
} PalGLWindow;

/**
 * @struct PalGLContextCreateInfo
 * @brief OpenGL context creation parameters.
 * 
 * This struct is used only during @ref palCreateGLContext() and may be
 * discarded after the function returns.
 * 
 * `fbConfig` must be compatible with the specified window.
 * The requested major and minor version must be supported by the
 * driver. Creating a forward-compatible context requires @ref PAL_GL_EXTENSION_CREATE_CONTEXT
 * extension to be supported.
 * 
 * The following context profiles requires:
 * 
 * - PAL_GL_PROFILE_CORE - @ref PAL_GL_EXTENSION_CONTEXT_PROFILE extension to be supported
 * 
 * - PAL_GL_PROFILE_COMPATIBILITY - @ref PAL_GL_EXTENSION_CONTEXT_PROFILE extension to be supported
 * 
 * - PAL_GL_PROFILE_COMPATIBILITY - @ref PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2 extension to be supported
 * 
 * The following context reset behaviors requires @ref PAL_GL_EXTENSION_ROBUSTNESS extension to be supported
 * 
 * - PAL_GL_CONTEXT_RESET_NO_NOTIFICATION
 * 
 * - PAL_GL_CONTEXT_RESET_LOSE_CONTEXT
 * 
 * @ref PAL_GL_RELEASE_BEHAVIOR_FLUSH requires @ref PAL_GL_EXTENSION_FLUSH_CONTROL extension to be supported
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
typedef struct PalGLContextCreateInfo
{
    const PalGLWindow* window; /**< window the context will be created for */
    const PalGLFBConfig* fbConfig; /**< framebuffer configuration to use */
    PalGLContext* shareContext; /**< context to share resource ownership with or `nullptr` */
    PalGLProfile profile; /**< context profile */
    PalGLContextReset reset; /**< context reset behavior */
    PalGLReleaseBehavior release; /**< context release behavior */
    PalBool forward; /**< whether to create a forward-compatible context */
    PalBool noError; /**< whether to create a no-error context */
    PalBool debug; /**< whether to create a debug context */
    uint32_t major; /**< context major version */
    uint32_t minor; /**< context minor version */
} PalGLContextCreateInfo;

/**
 * @brief Initializes the OpenGL system.
 * 
 * This function initialized the OpenGL system with the specified API and instance.
 * `api` must be supported by the specified instance. Use @ref palGetSupportedAPIs() to
 * to check the supported APIs of the instance.
 * 
 * The specified instance must not be `nullptr` and must remain valid until 
 * @ref palShutdownGL() is called. The OpenGL system does not take ownership of the instance.
 * On Linux, `instance` is the native display associated with the connection. On Windows,
 * `instance` is the process HINSTANCE.
 * 
 * `allocator` is not copied. The allocator and any state referenced by it must remain valid
 * until the OpenGL system has shutdown.
 *
 * @param[in] api OpenGL API to use.
 * @param[in] instance Instance to use.
 * @param[in] allocator Allocator to use or `nullptr` for default.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult for more information.
 *
 * @Thread-safety Must only be called from the main thread.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 * 
 * @sa palShutdownGL
 * @sa palGetSupportedGLAPIs
 */
PAL_API PalResult PAL_CALL palInitGL(
    PalGLAPI api,
    void* instance,
    const PalAllocator* allocator);

/**
 * @brief Shutdowns the OpenGL system.
 * 
 * This function shutdowns the OpenGL system. All created contexts must be
 * destroyed before this call. If the OpenGL system has not been initialized, the
 * function returns silently.
 * 
 * The specified instance at @ref palInitGL() will not be freed or destroyed,
 * users are responsible for destroying the instance.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 * 
 * @sa palInitGL
 */
PAL_API void PAL_CALL palShutdownGL();

/**
 * @brief Gets information about the OpenGL driver.
 * 
 * This function returns information about the OpenGL driver. The OpenGL
 * system must be initialized before this call. The returned pointer is
 * owned by the OpenGL system and must be freed. The pointer is valid
 * until the OpenGL system has shutdown.
 *
 * @return Returned pointer on success or `nullptr` on failure.
 *
 * @Thread-safety Thread-safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
PAL_API const PalGLInfo* PAL_CALL palGetGLInfo();

/**
 * @brief Gets the address of an OpenGL function.
 * 
 * This function gets the address of the specified OpenGL or an extension
 * function. The returned function pointer must not be freed, its owned by
 * the OpenGL system.
 *
 * The opengl system must be initialized before this call.
 *
 * @param[in] name Null-terminated UTF-8 encoded function name.
 * @return Pointer to function on success or `nullptr` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 * 
 * @sa palInitGL
 */
PAL_API void* PAL_CALL palGetGLProcAddress(const char* name);

/**
 * @brief Gets the supported OpenGL APIs of an instance.
 * 
 * This function returns all the OpenGL APIs supported by the specified instance.
 * The returned pointer must not be freed. The returned array contains one PalBool for
 * each OpenGL API, indexed by the corresponding constant (eg. `PAL_GL_API_OPENGL`)
 * and must not exceed @ref PAL_GL_API_COUNT.
 * 
 * This function must be called before @ref palInitGL() and its intended to determine
 * which OpenGL APIs are supported by `instance`.
 *
 * @param[in] instance The instance to check support for OpenGL APIs.
 * @return Pointer to the OpenGL APIs array on success or `nullptr` on failure.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 * 
 * @sa palInitGL
 */
PAL_API const PalBool* PAL_CALL palGetSupportedGLAPIs(void* instance);

/**
 * @brief Enumerates supported OpenGL framebuffer configurations.
 * 
 * This function enumerates supported OpenGL framebuffer configurations by the
 * driver. This is a two-call function, set `configs` to `nullptr` and `count` to
 * 0 to get the number of supported framebuffer configurations. Allocate the array 
 * and call this function again to with `configs` set to the allocated array and 
 * `count` set to the capacity of the array.
 * 
 * If the specified count is less than the number of supported framebuffer configurations,
 * only the configurations that fit in the array will be written.
 * 
 * The OpenGL system must be initialized before this call.
 *
 * @param[in, out] count Capacity of the monitor array.
 * @param[out] configs Monitor array.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult for more information.
 *
 * @Thread-safety Must only be called from the main thread.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY
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
 * @brief Gets the closest match OpenGL framebuffer config with a desired configuration.
 * 
 * This function comapares the specified framebuffer configurations array against
 * the desired configuration and returns the closest match configuration according
 * to the OpenGL framebuffer configuration rules.
 * 
 * `configs` array must have been obtained from @ref palEnumerateGLFBConfigs().
 * The returned pointer if not `nullptr` is an element in the configurations
 * array and must not be freed seperately. The contents of the configurations
 * array will not be modified by this function.
 *
 * @param[in] configs Framebuffer configurations array.
 * @param[in] count Capacity of the framebuffer configurations array.
 * @param[in] desired Desired framebuffer configuration.
 * @return Closest match framebuffer configuration on success or `nullptr` on failure.
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

/**
 * @brief Creates an OpenGL context.
 * 
 * This function creates an OpenGL context using the specified creation parameters.
 * `info` must remain valid for the duration of this function. PAL does not
 * copy the its contents.
 * 
 * `info->fbConfig` is the framebuffer configuration the context will the created
 * with. The specified window must use the same configuration. Once a 
 * framebuffer configuration has been selected by the specified window, 
 * the OpenGL system cannot change it. The window must be destroyed and recreated
 * again to change the configuration.
 * 
 * If `info->shareContext` is not `nullptr`, the created context shares resources
 * with that context. The created context will not be made current. Use 
 * @ref palMakeContextCurrent() to make the context current on the calling thread.
 * 
 * The created context is owned by PAL an must be destroyed with 
 * @ref palDestroyGLContext(). The OpenGL system must be initialized before this call.
 *
 * @param[in] info Context creation parameters.
 * @param[out] context Output handle to recieve the created context.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult for more information.
 *
 * @Thread-safety `context` must be per thread.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
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
 * set to `nullptr` to release the context if its current.
 * 
 * If `context` shares resources with other contexts, the resources will not
 * be destroyed according to OpenGL rules.
 *
 * @param[in] context Context to destroy.
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
 * even if its not `nullptr`.
 * 
 * When switching from one current context to another, the previous context is flushed
 * according to the behavior used to create the context.
 *
 * @param[in] window OpenGL window to associate the context with.
 * @param[in] context Context to make current or `nullptr` to release the current context.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult for more information.
 *
 * @Thread-safety A context may be current to only one thread at a time.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
PAL_API PalResult PAL_CALL palMakeContextCurrent(
    PalGLWindow* window,
    PalGLContext* context);

/**
 * @brief Swaps the front and back buffer of an OpenGL window.
 * 
 * This function swaps the front and back buffer of the specified window using
 * `context`. The specified window and context must have been created with a 
 * double-buffer framebuffer configuration. If the configuration is single-buffer,
 * behavior is undefined.
 * 
 * `context` must be current on the calling thread. The buffers are swapped according
 * to the platform rules. 
 *
 * @param[in] window OpenGL window to swap its buffers.
 * @param[in] context Context to associate with the OpenGL window.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult for more information.
 *
 * @Thread-safety Must only be called from a thread on which `context` is current.
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
 * This function sets the minimum number of video frames periods that must elaspe
 * between buffer swaps for the window associated with the current context of the
 * calling thread. An interval of `0` disables synchronization of buffer swaps.
 * 
 * @ref PAL_GL_EXTENSION_SWAP_CONTROL_TEAR extension must be supported if negative
 * swap intervals will be used. Negative intervals allows the driver to swap
 * immediately even if a frame arrives late.
 * 
 * @ref PAL_GL_EXTENSION_SWAP_CONTROL feature must be 
 * supported or this function results in undefined behavior.
 *
 * @param[in] interval Minimum number of video frame periods between swaps.
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

#endif // PAL_OPENGL_H
