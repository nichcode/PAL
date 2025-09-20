
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

// Opengl API entry
#ifdef _WIN32
#define PAL_GL_APIENTRY __stdcall
#else
#define PAL_GL_APIENTRY
#endif // _WIN32

/**
 * @struct PalGLContext
 * @brief Opaque handle to an opengl context.
 *
 * @since Added in version 1.0.0.
 * @ingroup opengl
 */
typedef struct PalGLContext PalGLContext;

/**
 * @enum PalGLExtensions
 * @brief Opengl system extensions.
 *
 * @note All opengl extensions follow the format `PAL_GL_EXTENSION_**` for
 * consistency and API use.
 *
 * @sa PalGLInfo
 * @sa palgetGLInfo()
 *
 * @since Added in version 1.0.0.
 * @ingroup opengl
 */
typedef enum {
    /** Modern context creation.*/
    PAL_GL_EXTENSION_CREATE_CONTEXT = PAL_BIT(0),

    /** Context profile.*/
    PAL_GL_EXTENSION_CONTEXT_PROFILE = PAL_BIT(1),

    /** Context profile es2.*/
    PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2 = PAL_BIT(2),

    /** Context reset (lose context, no notification).*/
    PAL_GL_EXTENSION_ROBUSTNESS = PAL_BIT(3),

    /** No errors*/
    PAL_GL_EXTENSION_NO_ERROR = PAL_BIT(4),

    /** Modern FBConfig.*/
    PAL_GL_EXTENSION_PIXEL_FORMAT = PAL_BIT(5),

    /** Multisample FBConfigs.*/
    PAL_GL_EXTENSION_MULTISAMPLE = PAL_BIT(6),

    /** Context swap interval (vsync).*/
    PAL_GL_EXTENSION_SWAP_CONTROL = PAL_BIT(7),

    /** Context release behavior (none, flush).*/
    PAL_GL_EXTENSION_FLUSH_CONTROL = PAL_BIT(8),

    /** Standard red, green and blue.*/
    PAL_GL_EXTENSION_COLORSPACE_SRGB = PAL_BIT(9)
} PalGLExtensions;

/**
 * @enum PalGLProfile
 * @brief Opengl context creation profiles. This is not a bitmask.
 *
 * @note All opengl profiles follow the format `PAL_GL_PROFILE_**` for
 * consistency and API use.
 *
 * @since Added in version 1.0.0.
 * @ingroup opengl
 */
typedef enum {
    /** Default context profile.*/
    PAL_GL_PROFILE_NONE,

    /** Core context profile. PAL_GL_EXTENSION_CONTEXT_PROFILE extension must be
     * supported.*/
    PAL_GL_PROFILE_CORE,

    /** Compatibility context profile. PAL_GL_EXTENSION_CONTEXT_PROFILE
     * extension must be supported.*/
    PAL_GL_PROFILE_COMPATIBILITY,

    /** Context profile es2. PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2 extension must
     * be supported.*/
    PAL_GL_PROFILE_ES
} PalGLProfile;

/**
 * @enum PalGLContextReset
 * @brief Opengl context reset behavior. This is not a bitmask.
 *
 * @note All context reset behavior follow the format `PAL_GL_CONTEXT_RESET_**`
 * for consistency and API use.
 *
 * @since Added in version 1.0.0.
 * @ingroup opengl
 */
typedef enum {
    /** Context reset none.*/
    PAL_GL_CONTEXT_RESET_NONE,

    /** Context reset no notification. PAL_GL_EXTENSION_ROBUSTNESS extension
     * must be supported.*/
    PAL_GL_CONTEXT_RESET_NO_NOTIFICATION,

    /** Context reset lose. PAL_GL_EXTENSION_ROBUSTNESS extension must be
     * supported.*/
    PAL_GL_CONTEXT_RESET_LOSE_CONTEXT
} PalGLContextReset;

/**
 * @enum PalGLRelease
 * @brief Opengl context release behavior. This is not a bitmask.
 *
 * @note All opengl context release behavior follow the format
 * `PAL_GL_RELEASE_BEHAVIOR_**` for consistency and API use.
 *
 * @since Added in version 1.0.0.
 * @ingroup opengl
 */
typedef enum {
    /** Context release none. Do not flush when context is switched.*/
    PAL_GL_RELEASE_BEHAVIOR_NONE,

    /** Context release flush. PAL_GL_EXTENSION_FLUSH_CONTROL extension must be
     * supported.*/
    PAL_GL_RELEASE_BEHAVIOR_FLUSH
} PalGLRelease;

/**
 * @struct PalGLInfo
 * @brief Information about the opengl driver.
 *
 * @sa palGetGLInfo()
 *
 * @since Added in version 1.0.0.
 * @ingroup opengl
 */
typedef struct {
    /** Supported opengl extensions. see PalGLExtensions.*/
    PalGLExtensions extensions;

    /** Highest supported opengl major.*/
    Uint32 major;

    /** Highest supported opengl minor.*/
    Uint32 minor;

    /** Graphics card vendor name (eg. Intel)*/
    char vendor[32];

    /** Graphics card name.*/
    char graphicsCard[64];

    /** Opengl version. This is a string of major, minor, and the build version
     * respectively.*/
    char version[64];
} PalGLInfo;

/**
 * @struct PalGLFBConfig
 * @brief Information about an opengl framebuffer.
 *
 * @sa palEnumerateGLFBConfigs()
 *
 * @note The fields should not be changed. PAL does not validate every field one
 * by one.
 *
 * @since Added in version 1.0.0.
 * @ingroup opengl
 */
typedef struct {
    /** Framebuffer supports double buffering.*/
    bool doubleBuffer;

    /** Framebuffer supports stereo.*/
    bool stereo;

    /** Framebuffer supports RGB colorspace.*/
    bool sRGB;

    /** Framebuffer index. Use to identify the framebuffer at the driver
     * level.*/
    Uint16 index;

    /** Framebuffer red bits.*/
    Uint16 redBits;

    /** Framebuffer green bits.*/
    Uint16 greenBits;

    /** Framebuffer blue bits.*/
    Uint16 blueBits;

    /** Framebuffer alpha bits.*/
    Uint16 alphaBits;

    /** Framebuffer depth bits.*/
    Uint16 depthBits;

    /** Framebuffer stencil bits.*/
    Uint16 stencilBits;

    /** Framebuffer supported sameples. If PAL_GL_EXTENSION_MULTISAMPLE
     * extension is not supported, this will be 1.*/
    Uint16 samples;
} PalGLFBConfig;

/**
 * @struct PalGLWindow
 * @brief Information about an opengl window.
 *
 * This can be allocated statically or dynamically since and its used for
 * holding native handles. The handles will not be copied therefore they must be
 * valid.
 *
 * @since Added in version 1.0.0.
 * @ingroup opengl
 */
typedef struct {
    /** Native display. Can be nullptr depending on platform (eg. X11)*/
    void* display;

    /** Native window. Must not be nullptr.*/
    void* window;
} PalGLWindow;

/**
 * @struct PalGLContextCreateInfo
 * @brief Describes options for creating an opengl context.
 *
 * This struct must be initialized explicitly and passed to
 * `palCreateGLContext()`.
 *
 * @note Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 1.0.0.
 * @ingroup opengl
 */
typedef struct {
    /** Forward compatible context.*/
    bool forward;

    /** No error context. It is recommended to set to true on release builds.*/
    bool noError;

    /** Debug context. It is recommended to set to false on release builds.*/
    bool debug;

    /** Must not exceed supported major version. see PalGLInfo.*/
    Uint16 major;

    /** Must not exceed supported minor version. see PalGLInfo.*/
    Uint16 minor;

    /** If PAL_GL_PROFILE_CORE or PAL_GL_PROFILE_COMPATIBILITY,
     * PAL_GL_EXTENSION_CONTEXT_PROFILE extension must be supported. If
     * PAL_GL_PROFILE_ES, PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2 extension must be
     * supported.*/
    PalGLProfile profile;

    /** If PAL_GL_CONTEXT_RESET_NO_NOTIFICATION or
     * PAL_GL_CONTEXT_RESET_LOSE_CONTEXT, PAL_GL_EXTENSION_ROBUSTNESS extension
     * must be supported.*/
    PalGLContextReset reset;

    /** If PAL_GL_RELEASE_BEHAVIOR_FLUSH, PAL_GL_EXTENSION_FLUSH_CONTROL
     * extension must be supported.*/
    PalGLRelease release;

    /** Set to nullptr to not share context.*/
    PalGLContext* shareContext;

    /** Context Framebuffer config. Must not be nullptr. see PalGLFBConfig.*/
    const PalGLFBConfig* fbConfig;

    /** Opengl window with native handles.*/
    const PalGLWindow* window;
} PalGLContextCreateInfo;

/**
 * @brief Initialize the opengl system.
 *
 * The opengl system must be initialized before enumerating framebuffer configs,
 * query supported extensions, create contexts and other opengl related
 * functionality. This must called before any opengl related function.
 *
 * The opengl system must be shutdown with `palShutdownGL()` when no longer
 * needed.
 *
 * @param[in] allocator Optional user provided allocator. Set to nullptr to use
 * default.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on
 * failure. Call palFormatResult() for more information.
 *
 * @note This function must only be called from the main thread.
 *
 * @sa palShutdownGL()
 *
 * @since Added in version 1.0.0.
 * @ingroup opengl
 */
PAL_API PalResult PAL_CALL palInitGL(const PalAllocator* allocator);

/**
 * @brief Shutdown the opengl system.
 *
 * If the opengl system has not been initialized, the function returns silently.
 * All created contexts must be destroyed before this call.
 *
 * @note This function must only be called from the main thread.
 *
 * @sa palInitGL()
 *
 * @since Added in version 1.0.0.
 * @ingroup opengl
 */
PAL_API void PAL_CALL palShutdownGL();

/**
 * @brief Get information about the opengl driver.
 *
 * The opengl system must be initialized before this call. see `PalGLInfo`
 *
 * @return A pointer to the opengl info on success or nullptr on failure.
 *
 * @note This function is thread-safe and may be called from any thread.
 * @note The returned pointer must not be freed
 *
 * @sa palInitGL()
 *
 * @since Added in version 1.0.0.
 * @ingroup opengl
 */
PAL_API const PalGLInfo* PAL_CALL palGetGLInfo();

/**
 * @brief Return a list of all opengl framebuffer configs supported by the
 * provided window.
 *
 * The opengl system must be initialized before this call.
 *
 * Call first with `configs` as nullptr to get the number of supported
 * framebuffer configs. If `count` is less than the number of supported
 * framebuffer configs, PAL will write up to limit.
 *
 * Users must `allocate` statically or dynamically and pass the maximum capacity
 * of the allocated array as `count` and also pass the array itself as
 * `configs`. The user is responsible for the lifetime of the array.
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
 *
 * @param[in] glWindow Pointer to the opengl window. See PalGLWindow.
 * @param[in] count Capacity of the `configs` array.
 * @param[out] configs User allocated array of PalGLFBConfig.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on
 * failure. Call palFormatResult() for more information.
 *
 * @note This function must only be called from the main thread.
 *
 * @sa palInitGL()
 * @sa PalGLWindow
 * @sa PalGLFBConfig
 *
 * @since Added in version 1.0.0.
 * @ingroup opengl
 */
PAL_API PalResult PAL_CALL palEnumerateGLFBConfigs(
    PalGLWindow* glWindow,
    Int32* count,
    PalGLFBConfig* configs);

/**
 * @brief Get the closest opengl framebuffer config from provided list of
 * framebuffers with a desired framebuffer config.
 *
 * The opengl system must be initialized before this call.
 * This function uses missing and score system to get the closest framebuffer
 * config. Since this function takes in the array of framebuffer configs, users
 * can use their own algorithm to get the closest.
 *
 * The count must be the number of opengl framebuffers configs in the
 * framebuffer configs array.
 *
 * If `configs` or `desired` is nullptr, or `count` is less or equal to zero.
 * this function fails and returns nullptr.
 *
 * @param[in] configs Pointer to the array of PalGLFBConfig.
 * @param[in] count Capacity of the `configs` array.
 * @param[in] desired The desired PalGLFBConfig
 *
 * @return The closest PalGLFBConfig on success or nullptr on failure.
 *
 * @note This function is thread safe and may be called from any thread.
 *
 * @sa palInitGL()
 * @sa PalGLFBConfig
 *
 * @since Added in version 1.0.0.
 * @ingroup video
 */
PAL_API const PalGLFBConfig* PAL_CALL palGetClosestGLFBConfig(
    PalGLFBConfig* configs,
    Int32 count,
    const PalGLFBConfig* desired);

/**
 * @brief Creates an opengl context.
 *
 * The opengl system must be initialized before this call.
 * The `info` pointer must be valid and initialized explicitly. see
 * PalGLContextCreateInfo struct. The created context will not be make current,
 * make the context current of the calling thread with palMakeContextCurrent().
 *
 * After this call, the provided FBConfig will be set to the window permanently
 * and cannot be changed. To change it, you must destroy the window and recreate
 * it. If the window already has a FBConfig, the opengl system skips the
 * provided FBConfig in the `info` struct and create the context with the
 * already set FBConfig.
 *
 * @param[in] info Pointer to the PalGLContextCreateInfo struct with creation
 * options.
 * @param[out] outContext Pointer to a PalGLContext to recieve the created
 * context.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on
 * failure. Call palFormatResult() for more information.
 *
 * @note This function must only be called from the main thread.
 *
 * @sa PalGLContextCreateInfo
 * @sa palDestroyGLContext()
 *
 * @since Added in version 1.0.0.
 * @ingroup opengl
 */
PAL_API PalResult PAL_CALL palCreateGLContext(
    const PalGLContextCreateInfo* info,
    PalGLContext** outContext);

/**
 * @brief Destroy the provided opengl context.
 *
 * The opengl system must be initialized before this call.
 *
 * This must be destroyed before the opengl system is shutdown.
 * If the provided context is invalid or nullptr, this function returns
 * silently. This destroys the context not the already set FBConfig.
 *
 * @param[in] context Pointer to the context.
 *
 * @note This function must only be called from the main thread.
 *
 * @sa palCreateGLContext()
 *
 * @since Added in version 1.0.0.
 * @ingroup opengl
 */
PAL_API void PAL_CALL palDestroyGLContext(PalGLContext* context);

/**
 * @brief Make the provided context current of the calling thread.
 *
 * The opengl system must be initialized before this call.
 *
 * The `glWindow` must have the same FBConfig used to create the context.
 * If the FBConfig of the `glWindow` is not the same as the one used to
 * create the context, this function fails and returns
 * `PAL_RESULT_INVALID_GL_WINDOW`. see `palCreateGLContext()`
 *
 * @param[in] context Pointer to the context.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on
 * failure. Call palFormatResult() for more information.
 *
 * @note This function may be called from any thread, but only one thread may
 * have the current context at a time.
 *
 * @sa palCreateGLContext()
 *
 * @since Added in version 1.0.0.
 * @ingroup opengl
 */
PAL_API PalResult PAL_CALL palMakeContextCurrent(
    PalGLWindow* glWindow,
    PalGLContext* context);

/**
 * @brief Get the pointer to a named opengl function.
 *
 * The opengl system must be initialized before this call.
 *
 * @param[in] context Pointer to the context.
 *
 * @return the pointer to the function on success or nullptr on failure.
 *
 * @note This function is thread safe and may be called from any thread.
 *
 * @sa palInitGL()
 *
 * @since Added in version 1.0.0.
 * @ingroup opengl
 */
PAL_API void* PAL_CALL palGLGetProcAddress(const char* name);

/**
 * @brief Present the contents of the back buffer of the the provided context to
 * the screen
 *
 * The opengl system must be initialized before this call.
 *
 * @param[in] context Pointer to the context.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on
 * failure. Call palFormatResult() for more information.
 *
 * @note This function must only be called from a thread that has a bound
 * context.
 *
 * @sa palCreateGLContext()
 *
 * @since Added in version 1.0.0.
 * @ingroup opengl
 */
PAL_API PalResult PAL_CALL palSwapBuffers(
    PalGLWindow* glWindow,
    PalGLContext* context);

/**
 * @brief Set the swap interval for the current context.
 *
 * The opengl system must be initialized before this call.
 * This affects the currently bound context on the calling thread, see
 * palMakeContextCurrent(). PAL_GL_EXTENSION_SWAP_CONTROL must be supported.
 *
 * `interval` == 0, no vsync
 *
 * `interval` == 0, vsync
 *
 * `interval` > 0, multiples of refresh if supported by the driver.
 *
 * @param[in] context The swap interval
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on
 * failure. Call palFormatResult() for more information.
 *
 * @note This function must only be called from a thread with a bound context.
 *
 * @sa palInitGL()
 * @sa palCreateGLContext()
 *
 * @since Added in version 1.0.0.
 * @ingroup opengl
 */
PAL_API PalResult PAL_CALL palSetSwapInterval(Int32 interval);

#endif // _PAL_OPENGL_H