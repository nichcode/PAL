/**
 * @file pal_opengl.h
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
 */

#ifndef PAL_OPENGL_H
#define PAL_OPENGL_H

#include "opengl/fbconfig.h"
#include "opengl/context.h"

#ifdef _WIN32
#define PAL_GL_APIENTRY __stdcall
#else
#define PAL_GL_APIENTRY
#endif // _WIN32

#define PAL_GL_VENDOR_NAME_SIZE 32
#define PAL_GL_GRAPHICS_CARD_NAME_SIZE 64
#define PAL_GL_VERSION_NAME_SIZE 64

/**
 * @defgroup opengl_extensions OpenGL Extensions
 * @brief OpenGL extensions.
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
 * @defgroup opengl_backend OpenGL Backends
 * @brief OpenGL backends
 * 
 * @{
 */
#define PAL_GL_BACKEND_EGL 0
#define PAL_GL_BACKEND_GLX 1
#define PAL_GL_BACKEND_WGL 2
#define PAL_GL_BACKEND_COUNT 3
/** @} */

/**
 * @defgroup opengl_apis OpenGL APIs
 * @brief OpenGL APIs
 * 
 * @{
 */
#define PAL_GL_API_OPENGL 0
#define PAL_GL_API_OPENGL_ES 1
#define PAL_GL_API_COUNT 2
/** @} */

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

#endif // PAL_OPENGL_H
