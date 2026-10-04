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
 * @brief Shutdowns the opengl system.
 *
 * If the opengl system has not been initialized, the function returns silently.
 * All created contexts must be destroyed before this call.
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
 * @ingroup pal_opengl
 */
PAL_API const PalGLInfo* PAL_CALL palGetGLInfo();

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
 * @ingroup pal_opengl
 * 
 * @sa palInitGL
 */
PAL_API void* PAL_CALL palGetGLProcAddress(const char* name);

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
 * @ingroup pal_opengl
 */
PAL_API const PalBool* PAL_CALL palGetSupportedGLAPIs(void* instance);

#endif // PAL_OPENGL_H
