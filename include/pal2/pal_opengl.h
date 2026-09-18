/**
 * @file pal_opengl.h
 * @brief This is the header file for PAL Opengl API.
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
 * @defgroup pal_opengl Opengl Module
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

#define PAL_GL_VENDOR_NAME_SIZE 32
#define PAL_GL_GRAPHICS_CARD_NAME_SIZE 64
#define PAL_GL_VERSION_NAME_SIZE 64

/**
 * @defgroup opengl_extensions OpenGl Extensions
 * @brief OpenGl extensions.
 * 
 * @{
 */
#define PAL_GL_EXTENSION_CREATE_CONTEXT (1ULL << 0)
#define PAL_GL_EXTENSION_CONTEXT_PROFILE (1ULL << 1)
#define PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2 (1ULL << 2)
#define PAL_GL_EXTENSION_ROBUSTNESS (1ULL << 3)
#define PAL_GL_EXTENSION_NO_ERROR (1ULL << 4)
#define PAL_GL_EXTENSION_PIXEL_FORMAT (1ULL << 5)
#define PAL_GL_EXTENSION_MULTISAMPLE (1ULL << 6)
#define PAL_GL_EXTENSION_SWAP_CONTROL (1ULL << 7)
#define PAL_GL_EXTENSION_FLUSH_CONTROL (1ULL << 8)
#define PAL_GL_EXTENSION_COLORSPACE_SRGB (1ULL << 9)
/** @} */

/**
 * @defgroup opengl_profiles OpenGl Profiles
 * @brief OpenGl profiles
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
 * @defgroup opengl_context_reset OpenGl Context Resets
 * @brief OpenGl context resets
 * 
 * @{
 */
#define PAL_GL_CONTEXT_RESET_NONE 0
#define PAL_GL_CONTEXT_RESET_NO_NOTIFICATION 1
#define PAL_GL_CONTEXT_RESET_LOSE_CONTEXT 2
#define PAL_GL_CONTEXT_RESET_COUNT 3
/** @} */

/**
 * @defgroup opengl_release OpenGl Release Behaviors
 * @brief OpenGl release behaviors
 * 
 * @{
 */
#define PAL_GL_RELEASE_BEHAVIOR_NONE 0
#define PAL_GL_RELEASE_BEHAVIOR_FLUSH 1
#define PAL_GL_RELEASE_BEHAVIOR_COUNT 2
/** @} */

/**
 * @defgroup opengl_backend OpenGl Backends
 * @brief OpenGl backends
 * 
 * @{
 */
#define PAL_GL_BACKEND_EGL 0
#define PAL_GL_BACKEND_GLX 1
#define PAL_GL_BACKEND_WGL 2
#define PAL_GL_BACKEND_COUNT 3
/** @} */

/**
 * @defgroup opengl_apis OpenGl APIs
 * @brief OpenGl APIs
 * 
 * @{
 */
#define PAL_GL_API_OPENGL 0
#define PAL_GL_API_OPENGL_ES 1
#define PAL_GL_API_COUNT 2
/** @} */

/**
 * @struct PalGLContext
 * @brief Opaque handle to an opengl context.
 *
 * @since Added in version 2.0
 */
typedef struct PalGLContext PalGLContext;

/**
 * @typedef PalGLExtensions
 * @brief Opengl system extensions.
 * 
 * This is a bitmask of all supported extensions of the opengl system.
 * The extensions are only for context creation.
 * 
 * All values of this type follow the format `PAL_GL_EXTENSION_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint64_t PalGLExtensions;

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
 * @typedef PalGLBackend
 * @brief Opengl backend.
 * 
 * All values of this type follow the format `PAL_GL_BACKEND_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalGLBackend;

/**
 * @typedef PalGLAPI
 * @brief Opengl apis.
 * 
 * All values of this type follow the format `PAL_GL_API_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalGLAPI;

/**
 * @struct PalGLInfo
 * @brief Contains information about the opengl driver.
 *
 * @since Added in version 2.0
 * 
 * @var PalGLInfo::extensions
 * A bitmask of all supported context creation extensions.
 * 
 * @var PalGLInfo::major
 * The major version of the opengl driver.
 * 
 * @var PalGLInfo::minor
 * The minor version of the opengl driver.
 * 
 * @var PalGLInfo::backend
 * The backend of the opengl driver (eg `PAL_GL_BACKEND_WGL`).
 * 
 * @var PalGLInfo::api
 * The API of the opengl driver (eg. `PAL_GL_API_OPENGL_ES`).
 * 
 * @var PalGLInfo::vendor
 * The vendor name of the opengl driver.
 * 
 * @var PalGLInfo::graphicsCard
 * The graphics card name of the opengl driver.
 * 
 * @var PalGLInfo::version
 * The version of `::graphicsCard` in a string format.
 */
typedef struct PalGLInfo
{
    PalGLExtensions extensions;
    uint32_t major;
    uint32_t minor;
    PalGLBackend backend;
    PalGLAPI api;
    char vendor[PAL_GL_VENDOR_NAME_SIZE];
    char graphicsCard[PAL_GL_GRAPHICS_CARD_NAME_SIZE];
    char version[PAL_GL_VERSION_NAME_SIZE];
} PalGLInfo;

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
 * @brief Initializes the opengl system.
 *
 * This must be called before any opengl function. 
 *
 * `allocator` and `instance` will not not copied or destroyed, therefore the
 * pointers must remain valid until the opengl system is shutdown.
 * 
 * The opengl system must be shutdown with `palShutdownGL()` when no 
 * longer needed.
 *
 * @param[in] api The opengl API.
 * @param[in] instance The instance or display. (eg. `HINSTANCE`). 
 * Must not be `nullptr`.
 * @param[in] allocator The allocator. Set to `nullptr` to use the
 * thread-safe default.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palShutdownGL
 * @sa palGetSupportedGLAPIs
 */
PAL_API PalResult PAL_CALL palInitGL(
    PalGLAPI api,
    void* instance,
    const PalAllocator* allocator);

/**
 * @brief Shutdowns the opengl system.
 *
 * If the opengl system has not been initialized, the function returns silently.
 * All created contexts must be destroyed before this call.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palInitGL
 */
PAL_API void PAL_CALL palShutdownGL();

/**
 * @brief Gets information about the opengl driver.
 *
 * The opengl system must be initialized before this call. 
 * 
 * The returned pointer must not be modified or freed by the user. The memory
 * is managed by PAL.
 *
 * @return The pointer to recieve the information on success or `nullptr` on failure.
 *
 * @Thread-safety Thread-safe.
 *
 * @since Added in version 2.0
 */
PAL_API const PalGLInfo* PAL_CALL palGetGLInfo();

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
 */
PAL_API const PalGLFBConfig* PAL_CALL palGetClosestGLFBConfig(
    PalGLFBConfig* configs,
    uint32_t count,
    const PalGLFBConfig* desired);

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
 * @brief Gets the pointer to a named opengl function.
 *
 * The opengl system must be initialized before this call.
 *
 * @param[in] name The function name in `UTF-8` encoding.
 *
 * @return The function on success or `nullptr` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palInitGL
 */
PAL_API void* PAL_CALL palGetGLProcAddress(const char* name);

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

/**
 * @brief Gets the supported opengl APIs of an instance.
 *
 * @param[in] instance The instance.
 *
 * @return An array of bools or `nullptr` on failure.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API const PalBool* PAL_CALL palGetSupportedGLAPIs(void* instance);

/** @} */

#endif // PAL_OPENGL_H
