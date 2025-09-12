
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
 *
 */

#ifndef _PAL_OPENGL_H
#define _PAL_OPENGL_H

#include "pal_core.h"

// A simple opengl API entry
#ifdef _WIN32
#define PAL_GL_APIENTRY __stdcall
#else 
#define PAL_GL_APIENTRY
#endif // _WIN32

/**
 * @struct PalGLContext
 * @brief Opaque handle to an opengl context.
 *
 * @ingroup opengl
 */
typedef struct PalGLContext PalGLContext;

typedef enum {
    PAL_GL_EXTENSION_CREATE_CONTEXT = PAL_BIT(0),          /** < Modern context creation.*/
    PAL_GL_EXTENSION_CONTEXT_PROFILE = PAL_BIT(1),         /** < Setting context profile.*/
    PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2 = PAL_BIT(2),
    PAL_GL_EXTENSION_ROBUSTNESS = PAL_BIT(3),
    PAL_GL_EXTENSION_NO_ERROR = PAL_BIT(4),
    PAL_GL_EXTENSION_PIXEL_FORMAT = PAL_BIT(5),            /** < Modern FBConfig.*/
    PAL_GL_EXTENSION_MULTISAMPLE = PAL_BIT(6),             /** < Multisample FBConfigs.*/
    PAL_GL_EXTENSION_SWAP_CONTROL = PAL_BIT(7),            /** < Vsync.*/
    PAL_GL_EXTENSION_FLUSH_CONTROL = PAL_BIT(8),
    PAL_GL_EXTENSION_COLORSPACE_SRGB = PAL_BIT(9)
} PalGLExtension;

typedef enum {
    PAL_GL_PROFILE_NONE,
    PAL_GL_PROFILE_CORE,
    PAL_GL_PROFILE_COMPATIBILITY,
    PAL_GL_PROFILE_ES
} PalGLProfile;

typedef enum {
    PAL_GL_CONTEXT_RESET_NONE,
    PAL_GL_CONTEXT_RESET_NO_NOTIFICATION,
    PAL_GL_CONTEXT_RESET_LOSE_CONTEXT
} PalGLContextReset;

typedef enum {
    PAL_GL_RELEASE_BEHAVIOR_NONE,
    PAL_GL_CONTEXT_RESET_FLUSH
} PalGLRelease;

typedef struct {
    PalGLExtension extensions;       /** < Supported extensions.*/
    Uint32 major;
    Uint32 minor;
    char vendor[32];
    char graphicsCard[64];
    char version[64];
} PalGLInfo;

typedef struct {
    bool doubleBuffer;
    bool stereo;
    bool sRGB;
    Uint16 index;
    Uint16 redBits;
    Uint16 greenBits;
    Uint16 blueBits;
    Uint16 alphaBits;
    Uint16 depthBits;
    Uint16 stencilBits;
    Uint16 samples;
} PalGLFBConfig;

/**
 * @struct PalGLWindow
 * @brief Information about an opengl window.
 * 
 * This can be allocated statically or dynamically since and its used for holding native handles.
 *
 * @ingroup opengl
 */
typedef struct {
    void* display;          /** < Native display. Can be `nullptr` depending on platform*/
    void* window;           /** < Native window.*/
} PalGLWindow;

/**
 * @struct PalGLContextCreateInfo
 * @brief Specifications for creating an opengl context.
 *
 * This struct must be initialized and passed to `palCreateGLContext()`.
 * All fields must be explicitly set by the user.
 *
 * @note Uninitialized fields may result in undefined behavior.
 *
 * @ingroup opengl
 */
typedef struct {
    bool forward;                     /** < Forward compatible context.*/
    bool robustness;                  /** < Robustness. Set to `true` if reset is not `PAL_GL_CONTEXT_RESET_NONE`*/
    bool noError;
    bool debug;                       /** < Debug context.*/
    Uint16 major;
    Uint16 minor;
    PalGLProfile profile;
    PalGLContextReset reset;          /** < Robustness must be `true` if reset is not `PAL_GL_CONTEXT_RESET_NONE`*/
    PalGLRelease release;
    PalGLContext* shareContext;       /** < Share context.*/
    const PalGLFBConfig* fbConfig;    /** < Context Framebuffer config.*/
    const PalGLWindow* window;        /** < Opengl window with native handles.*/
} PalGLContextCreateInfo;

/**
 * @brief Initialize the opengl system.
 * 
 * The opengl system must be initialized before enumerating framebuffer configs,
 * query supported extensions, create contexts and other opengl related functionality.
 * This must called before any opengl related function.
 * 
 * The opengl system must be shutdown with `palShutdownGL()` when no longer needed.
 *
 * @param[in] allocator Optional user provided allocator. Can be `nullptr` to use default.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palShutdownGL()
 * @ingroup opengl
 */
PAL_API PalResult PAL_CALL palInitGL(
    const PalAllocator* allocator);

/**
 * @brief Shutdown the opengl system.
 *
 * The opengl system must be initialized before this call otherwise this functions returns silently.
 * This function does not destroy created contexts.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palInitGL()
 * @ingroup opengl
 */
PAL_API void PAL_CALL palShutdownGL();

/**
 * @brief Get information about the opengl system
 * 
 * The opengl system must be initialized before this call.
 * see `PalGLInfo`
 * 
 * @return A pointer to the opengl info on success or `nullptr` on failure.
 *
 * @note This function is thread-safe.
 * @note The returned pointer must not be freed
 *
 * @sa palInitGL()
 * @ingroup opengl
 */
PAL_API const PalGLInfo* PAL_CALL palGetGLInfo();

/**
 * @brief Return a list of all opengl framebuffer configs supported by the provided window
 * 
 * The opengl system must be initialized before this call.
 * 
 * Users must `allocate` statically or dynamically and pass the maximum capacity of the
 * allocated array as `count` and also pass the array itself as `configs`.
 * The user is responsible for the life time of the array.
 *
 * The `count` should be the number, not the size in bytes. Example:
 * 
 * @code
 * PalGLFBConfig configs[2];
 * @endcode
 * 
 * @code
 * Int32 count = 2;
 * @endcode
 *
 * you can set the `configs` to `nullptr` and PAL will set the number of the connected configs to `count`.
 * 
 * If the `count` is zero or less than zero and the `configs` is not `nullptr`, 
 * this function fails and returns `PAL_RESULT_INSUFFICIENT_BUFFER`.
 * If `count` is less than the number of connected configs, PAL will write up to `count`.
 *
 * @param[in] glWindow Pointer to the opengl window.
 * @param[in] count Capacity of the `configs` array.
 * @param[out] configs User allocated array of PalGLFBConfig.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function must be called from the main thread.
 *
 * @sa palInitGL(), PalGLWindow, PalGLFBConfig
 * @ingroup video
 */
PAL_API PalResult PAL_CALL palEnumerateGLFBConfigs(
    PalGLWindow* glWindow,
    Int32 *count,
    PalGLFBConfig *configs);

/**
 * @brief Get the closest opengl framebuffer from a list of framebuffers with a desired one.
 * 
 * The opengl system must be initialized before this call.
 * This users missing and score system to get the closest framebuffer config.
 * Users can use their own way to find the closest.
 * 
 * The count must be the number of opengl framebuffers in the framebuffer configs array.
 * 
 * If `configs` or `desired` is `nullptr`, or `count` is less or equal to zero.
 * this function fails and returns `nullptr`.
 *
 * @param[in] configs Pointer to the array of PalGLFBConfig.
 * @param[in] count Capacity of the `configs` array.
 * @param[in] desired The desired PalGLFBConfig
 *
 * @return The closest PalGLFBConfig on success or `nullptr` on failure.
 *
 * @note This function is thread safe
 *
 * @sa palInitGL(), PalGLFBConfig
 * @ingroup video
 */
PAL_API const PalGLFBConfig* PAL_CALL palGetClosestGLFBConfig(
    PalGLFBConfig *configs,
    Int32 count,
    const PalGLFBConfig* desired);

PAL_API PalResult PAL_CALL palCreateGLContext(
    const PalGLContextCreateInfo* info,
    PalGLContext** outContext);

PAL_API void PAL_CALL palDestroyGLContext(
    PalGLContext* context);

PAL_API PalResult PAL_CALL palMakeContextCurrent(
    PalGLWindow* glWindow,
    PalGLContext* context);

PAL_API void* PAL_CALL palGLGetProcAddress(
    const char* name);

PAL_API PalResult PAL_CALL palSwapBuffers(
    PalGLWindow* glWindow,
    PalGLContext* context);

PAL_API void PAL_CALL palSetSwapInterval(
    PalGLWindow* glWindow,
    PalGLContext* context,
    Int32 interval);

#endif // _PAL_OPENGL_H