/**
 * @file init.h
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

#ifndef PAL_OPENGL_INIT_H
#define PAL_OPENGL_INIT_H

#include "types.h"
#include "pal2/core/memory.h"

/**
 * @brief Maximum size of the vendor name, including the NULL terminator.
 * @ingroup pal_opengl
 */
#define PAL_GL_VENDOR_NAME_SIZE 32

/**
 * @brief Maximum size of the graphics card name, including the NULL terminator.
 * @ingroup pal_opengl
 */
#define PAL_GL_GRAPHICS_CARD_NAME_SIZE 64

/**
 * @brief Maximum size of the version name, including the NULL terminator.
 * @ingroup pal_opengl
 */
#define PAL_GL_VERSION_NAME_SIZE 64

/**
 * @defgroup opengl_extensions OpenGL Extensions
 * @ingroup pal_opengl
 * @{
 */
#define PAL_GL_EXTENSION_CREATE_CONTEXT (1ULL << 0) /**< Modern context creation. */
#define PAL_GL_EXTENSION_CONTEXT_PROFILE (1ULL << 1) /**< Context profile. */
#define PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2 (1ULL << 2) /**< ES2 profile context. */
#define PAL_GL_EXTENSION_ROBUSTNESS (1ULL << 3) /**< Context reset behavior. */
#define PAL_GL_EXTENSION_NO_ERROR (1ULL << 4) /**< No-error context. */
#define PAL_GL_EXTENSION_PIXEL_FORMAT (1ULL << 5) /**< Modern framebuffer configurations. */
#define PAL_GL_EXTENSION_MULTISAMPLE (1ULL << 6) /**< Multisample context. */
#define PAL_GL_EXTENSION_SWAP_CONTROL (1ULL << 7) /**< Swap control options. */
#define PAL_GL_EXTENSION_FLUSH_CONTROL (1ULL << 8) /**< Flush control options. */
#define PAL_GL_EXTENSION_COLORSPACE_SRGB (1ULL << 9) /**< sRGB colorspace. */
#define PAL_GL_EXTENSION_SWAP_CONTROL_TEAR (1ULL << 10) /**< Negative swap intervals. */
/** @} */

/**
 * @defgroup opengl_backend OpenGL Backends
 * @ingroup pal_opengl
 * @{
 */
#define PAL_GL_BACKEND_EGL 0 /**< EGL backend. */
#define PAL_GL_BACKEND_GLX 1 /**< GLX backend (X11). */
#define PAL_GL_BACKEND_WGL 2 /**< WGL backend (Windows). */
#define PAL_GL_BACKEND_COUNT 3 /**< Number of OpenGL backends. */
/** @} */

/**
 * @defgroup opengl_apis OpenGL APIs
 * @ingroup pal_opengl
 * @{
 */
#define PAL_GL_API_OPENGL 0 /**< Desktop OpenGL. */
#define PAL_GL_API_OPENGL_ES 1 /**< OpenGL ES. */
#define PAL_GL_API_COUNT 2 /**< Number of OpenGL APIs. */
/** @} */

/**
 * @struct PalGLInfo
 * @brief OpenGL driver information.
 *
 * Filled in by the OpenGL system and returned by @ref palGetGLInfo().
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
typedef struct PalGLInfo {
    PalGLExtensions extensions; /**< Bitmask of supported OpenGL extensions. */
    uint32_t major; /**< Major version. */
    uint32_t minor; /**< Minor version. */
    PalGLBackend backend; /**< OpenGL backend of the driver. */
    PalGLAPI api; /**< OpenGL API of the driver. */
    char vendor[PAL_GL_VENDOR_NAME_SIZE]; /**< Vendor name (NULL-terminated). */
    char graphicsCard[PAL_GL_GRAPHICS_CARD_NAME_SIZE]; /**< Graphics card name (NULL-terminated). */
    char version[PAL_GL_VERSION_NAME_SIZE]; /**< Version name (NULL-terminated). */
} PalGLInfo;

/**
 * @brief Initializes the OpenGL system.
 *
 * The OpenGL system must be shut down using @ref palShutdownGL().
 *
 * This function initializes the OpenGL system with the specified API and instance.
 * `api` must be supported by the specified instance. Use @ref palGetSupportedGLAPIs() to
 * check the supported APIs of the instance.
 *
 * The specified instance must remain valid until
 * @ref palShutdownGL() is called. The OpenGL system does not take ownership of the instance.
 * On Linux, `instance` is the native display associated with the connection. On Windows,
 * `instance` is the process HINSTANCE.
 *
 * `allocator` is not copied. The allocator and any state referenced by it must remain valid
 * until the OpenGL system has been shut down.
 *
 * @param[in] api OpenGL API to use.
 * @param[in] instance Instance to use.
 * @param[in] allocator Allocator to use, or `nullptr` for the default.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety Must be called from the main thread.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
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
 * @brief Shuts down the OpenGL system.
 *
 * This function shuts down the OpenGL system. All created contexts must be
 * destroyed before this call. If the OpenGL system has not been initialized, the
 * function returns silently.
 *
 * The instance passed to @ref palInitGL() is not freed or destroyed;
 * the caller is responsible for destroying it.
 *
 * @thread-safety Must be called from the main thread.
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
 * owned by the OpenGL system and must not be freed. The pointer is valid
 * until the OpenGL system has shut down.
 *
 * @return Pointer to the driver information on success or `nullptr` on failure.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 */
PAL_API const PalGLInfo* PAL_CALL palGetGLInfo();

/**
 * @brief Gets the address of an OpenGL function.
 *
 * This function gets the address of the specified OpenGL function or extension
 * function. The returned function pointer must not be freed; it is owned by
 * the OpenGL system.
 *
 * The OpenGL system must be initialized before this call.
 *
 * @param[in] name UTF-8 encoded function name (NULL-terminated).
 * @return Pointer to the function on success or `nullptr` on failure.
 *
 * @thread-safety Thread safe.
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
 * The returned pointer must not be freed. The returned array contains one `PalBool` for
 * each OpenGL API, indexed by the corresponding constant (for example `PAL_GL_API_OPENGL`),
 * and has at most @ref PAL_GL_API_COUNT elements.
 *
 * This function must be called before @ref palInitGL() and is intended to determine
 * which OpenGL APIs are supported by `instance`.
 *
 * @param[in] instance Instance to query for supported OpenGL APIs.
 * @return Pointer to the OpenGL APIs array on success or `nullptr` on failure.
 *
 * @thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_opengl
 *
 * @sa palInitGL
 */
PAL_API const PalBool* PAL_CALL palGetSupportedGLAPIs(void* instance);

#endif // PAL_OPENGL_INIT_H