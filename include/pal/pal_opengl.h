
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
 * @defgroup pal_opengl Opengl
 * Opengl PAL functionality such as FBConfigs and contexts.
 *
 * @{
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
 * @since 1.0.
 * @ingroup pal_opengl
 */
typedef struct PalGLContext PalGLContext;

/**
 * @enum PalGLExtensions
 * @brief Opengl system extensions.
 *
 * All opengl extensions follow the format `PAL_GL_EXTENSION_**` for
 * consistency and API use.
 *
 * @since 1.0.
 * @ingroup pal_opengl
 */
typedef enum {
    PAL_GL_EXTENSION_CREATE_CONTEXT = PAL_BIT(0), /** < Modern context.*/
    PAL_GL_EXTENSION_CONTEXT_PROFILE = PAL_BIT(1),
    PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2 = PAL_BIT(2),
    PAL_GL_EXTENSION_ROBUSTNESS = PAL_BIT(3),     /** < Reset behaviour.*/
    PAL_GL_EXTENSION_NO_ERROR = PAL_BIT(4),       /** < No errors*/
    PAL_GL_EXTENSION_PIXEL_FORMAT = PAL_BIT(5),   /** < Modern PalGLFBConfig.*/
    PAL_GL_EXTENSION_MULTISAMPLE = PAL_BIT(6),    /** < Multisample FBConfigs.*/
    PAL_GL_EXTENSION_SWAP_CONTROL = PAL_BIT(7),   /** < Vsync.*/
    PAL_GL_EXTENSION_FLUSH_CONTROL = PAL_BIT(8),  /** < Release behavior.*/
    PAL_GL_EXTENSION_COLORSPACE_SRGB = PAL_BIT(9) /** < Standard RGB.*/
} PalGLExtensions;

/**
 * @enum PalGLProfile
 * @brief Opengl context creation profiles. This is not a bitmask.
 *
 * All opengl profiles follow the format `PAL_GL_PROFILE_**` for
 * consistency and API use.
 *
 * @since 1.0.
 * @ingroup pal_opengl
 */
typedef enum {
    PAL_GL_PROFILE_NONE,          /** < Default profile.*/
    PAL_GL_PROFILE_CORE,          /** < PAL_GL_EXTENSION_CONTEXT_PROFILE.*/
    PAL_GL_PROFILE_COMPATIBILITY, /** < PAL_GL_EXTENSION_CONTEXT_PROFILE.*/
    PAL_GL_PROFILE_ES             /** < PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2.*/
} PalGLProfile;

/**
 * @enum PalGLContextReset
 * @brief Opengl context reset behavior. This is not a bitmask.
 *
 * All context reset behavior follow the format `PAL_GL_CONTEXT_RESET_**`
 * for consistency and API use.
 *
 * @since 1.0.
 * @ingroup pal_opengl
 */
typedef enum {
    PAL_GL_CONTEXT_RESET_NONE,            /** < Default reset behaviour.*/
    PAL_GL_CONTEXT_RESET_NO_NOTIFICATION, /** < PAL_GL_EXTENSION_ROBUSTNESS.*/
    PAL_GL_CONTEXT_RESET_LOSE_CONTEXT     /** < PAL_GL_EXTENSION_ROBUSTNESS.*/
} PalGLContextReset;

/**
 * @enum PalGLRelease
 * @brief Opengl context release behavior. This is not a bitmask.
 *
 * All opengl context release behavior follow the format
 * `PAL_GL_RELEASE_BEHAVIOR_**` for consistency and API use.
 *
 * @since 1.0.
 * @ingroup pal_opengl
 */
typedef enum {
    PAL_GL_RELEASE_BEHAVIOR_NONE, /** < Default release behaviour..*/
    PAL_GL_RELEASE_BEHAVIOR_FLUSH /** < PAL_GL_EXTENSION_FLUSH_CONTROL.*/
} PalGLRelease;

/**
 * @struct PalGLInfo
 * @brief Information about the opengl driver.
 *
 * @since 1.0.
 * @ingroup pal_opengl
 */
typedef struct {
    PalGLExtensions extensions;
    Uint32 major;               /** < Version major.*/
    Uint32 minor;               /** < Version minor.*/
    char vendor[32];            /** < Graphics card vendor name (eg. Intel).*/
    char graphicsCard[64];      /** < Graphics card name.*/
    char version[64];           /** < Version string (major, minor, build).*/
} PalGLInfo;

/**
 * @struct PalGLFBConfig
 * @brief Information about an opengl framebuffer.
 *
 * @since 1.0.
 * @ingroup pal_opengl
 */
typedef struct {
    bool doubleBuffer;
    bool stereo;
    bool sRGB;
    Uint16 index; /** < Driver index. Must not be changed.*/
    Uint16 redBits;
    Uint16 greenBits;
    Uint16 blueBits;
    Uint16 alphaBits;
    Uint16 depthBits;
    Uint16 stencilBits;
    Uint16 samples; /** < PAL_GL_EXTENSION_MULTISAMPLE or 1.*/
} PalGLFBConfig;

/**
 * @struct PalGLWindow
 * @brief Information about an opengl window.
 *
 * This can be allocated statically or dynamically since its used for
 * holding native handles. The handles will not be copied.
 *
 * @since 1.0.
 * @ingroup pal_opengl
 */
typedef struct {
    void* display; /** < Can be nullptr depending on platform (eg. Windows).*/
    void* window;  /** < Must not be nullptr.*/
} PalGLWindow;

/**
 * @struct PalGLContextCreateInfo
 * @brief Creation parameters for an opengl context.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.0.
 * @ingroup pal_opengl
 */
typedef struct {
    bool forward; /** < Forward compatible context.*/
    bool noError; /** < No error context.*/
    bool debug;   /** < Debug context. */
    Uint16 major; /** Must not exceed supported major version. see PalGLInfo.*/
    Uint16 minor; /** Must not exceed supported minor version. see PalGLInfo.*/
    PalGLProfile profile;          /** < see PalGLProfile.*/
    PalGLContextReset reset;       /** < see PalGLContextReset.*/
    PalGLRelease release;          /** < see PalGLRelease.*/
    PalGLContext* shareContext;    /** < Can be nullptr.*/
    const PalGLFBConfig* fbConfig; /** < Must not be nullptr.*/
    const PalGLWindow* window;     /** Must not be nullptr.*/
} PalGLContextCreateInfo;

/**
 * @brief Initialize the opengl system.
 *
 * This must be called before any opengl function. The opengl system must be
 * shutdown with palShutdownGL() when no longer needed.
 *
 * The allocator will not not copied, therefore the pointer must remain valid
 * until the opengl system is shutdown.
 *
 * @param[in] allocator Optional user-provided allocator. Set to nullptr to use
 * default.
 *
 * @return PAL_RESULT_SUCCESS on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0.
 * @ingroup pal_opengl
 * @sa palShutdownGL
 */
PAL_API PalResult PAL_CALL palInitGL(const PalAllocator* allocator);

/**
 * @brief Shutdown the opengl system.
 *
 * If the opengl system has not been initialized, the function returns silently.
 * All created contexts must be destroyed before this call.
 *
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0.
 * @ingroup pal_opengl
 * @sa palInitGL
 */
PAL_API void PAL_CALL palShutdownGL();

/**
 * @brief Get information about the opengl driver.
 *
 * The opengl system must be initialized before this call. The returned
 * PalGLInfo pointer must not be freed.
 *
 * @return A pointer to a PalGLInfo on success or nullptr on failure.
 *
 * Thread safety: This function is thread-safe.
 *
 * @since 1.0.
 * @ingroup pal_opengl
 */
PAL_API const PalGLInfo* PAL_CALL palGetGLInfo();

/**
 * @brief Return a list of all supported PalGLFBConfig on the provided window.
 *
 * The opengl system must be initialized before this call.
 *
 * Call this function first with PalGLFBConfig array set to nullptr to get the
 * number of supported PalGLFBConfig. Allocate memory for the PalGLFBConfig
 * array and passed in the count and the array. If the count of the array is
 * less than the number of supported PalGLFBConfigs, PAL will write upto that
 * limit.
 *
 * If the count is 0 and the PalGLFBConfigs array is nullptr, the function fails
 * and returns PAL_RESULT_INSUFFICIENT_BUFFER.
 *
 * @param[in] glWindow Pointer to the opengl window.
 * @param[in] count Capacity of the PalGLFBConfig array.
 * @param[out] configs User allocated array of PalGLFBConfig.
 *
 * @return PAL_RESULT_SUCCESS on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0.
 * @ingroup pal_opengl
 * @sa palInitGL
 */
PAL_API PalResult PAL_CALL palEnumerateGLFBConfigs(
    PalGLWindow* glWindow,
    Int32* count,
    PalGLFBConfig* configs);

/**
 * @brief Get the closest PalGLFBConfig from the array of PalGLFBConfig with a
 * desired PalGLFBConfig.
 *
 * The opengl system must be initialized before this call.
 * This function uses missing and score system to get the closest PalGLFBConfig.
 *
 * The count must be the number of PalGLFBConfig in the
 * array of PalGLFBConfig. If the count is less than or equal to 0 or the
 * desired PalGLFBConfig or the PalGLFBConfig array is nullptr, this function
 * fails and returns nullptr.
 *
 * @param[in] configs Pointer to the array of PalGLFBConfig.
 * @param[in] count Capacity of the PalGLFBConfig array.
 * @param[in] desired The desired PalGLFBConfig.
 *
 * @return The closest PalGLFBConfig on success or nullptr on failure.
 *
 * Thread safety: This function is thread safe.
 *
 * @since 1.0.
 * @ingroup pal_opengl
 */
PAL_API const PalGLFBConfig* PAL_CALL palGetClosestGLFBConfig(
    PalGLFBConfig* configs,
    Int32 count,
    const PalGLFBConfig* desired);

/**
 * @brief Creates an opengl context.
 *
 * The opengl system must be initialized before this call. The created context
 * will not be made current.
 *
 * After this call, the provided PalGLFBConfig will be set to the window
 * permanently and cannot be changed. To change it, you must destroy the window
 * and recreate it. If the window already has a PalGLFBConfig, the opengl system
 * will use that and discard the provided one.
 *
 * @param[in] info Pointer to a PalGLContextCreateInfo struct that specifies
 * paramters. Must not be nullptr.
 * @param[out] outContext Pointer to a PalGLContext to recieve the created
 * context. Must not be nullptr.
 *
 * @return PAL_RESULT_SUCCESS on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.0.
 * @ingroup pal_opengl
 * @sa palDestroyGLContext
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
 * silently. The context must not be current in any thread before this call.
 *
 * @param[in] context Pointer to the context to destroy.
 *
 * Thread safety: This function is thread safe if the `context` is thread local.
 *
 * @since 1.0.
 * @ingroup pal_opengl
 * @sa palCreateGLContext
 */
PAL_API void PAL_CALL palDestroyGLContext(PalGLContext* context);

/**
 * @brief Make the provided context current on the calling thread.
 *
 * The opengl system must be initialized before this call.
 *
 * The opengl window must have the same PalGLFBConfig used to create the
 * context. If the PalGLFBConfig of the opengl window is not the same as the one
 * used to create the context, this function fails and returns
 * PAL_RESULT_INVALID_GL_WINDOW.
 *
 * @param[in] context Pointer to the context to make current.
 *
 * @return PAL_RESULT_SUCCESS on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function is thread safe, but only one thread may have the
 * current context at a time.
 *
 * @since 1.0.
 * @ingroup pal_opengl
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
 * Thread safety: This function is thread safe.
 *
 * @since 1.0.
 * @ingroup pal_opengl
 * @sa palInitGL
 */
PAL_API void* PAL_CALL palGLGetProcAddress(const char* name);

/**
 * @brief Present the contents of the back buffer of the provided context to
 * the screen.
 *
 * The opengl system must be initialized before this call.
 *
 * @param[in] context Pointer to the context.
 *
 * @return PAL_RESULT_SUCCESS on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function must only be called from a thread that has a
 * bound context.
 *
 * @since 1.0.
 * @ingroup pal_opengl
 * @sa palMakeContextCurrent
 */
PAL_API PalResult PAL_CALL palSwapBuffers(
    PalGLWindow* glWindow,
    PalGLContext* context);

/**
 * @brief Set the swap interval for the current context.
 *
 * The opengl system must be initialized before this call.
 * This affects the currently bound context on the calling thread, see
 * PAL_GL_EXTENSION_SWAP_CONTROL must be supported. Set interval to 1 for vsync.
 *
 * @param[in] context The swap interval
 *
 * @return PAL_RESULT_SUCCESS on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function must only be called from a thread with a bound context.
 *
 * @since 1.0.
 * @ingroup pal_opengl
 * @sa palMakeContextCurrent
 */
PAL_API PalResult PAL_CALL palSetSwapInterval(Int32 interval);

/** @} */ // end of pal_opengl group

#endif // _PAL_OPENGL_H