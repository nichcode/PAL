
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup pal_opengl Opengl
 * @brief PAL Opengl Functionality and API.
 */

/** @{ */

#ifndef _PAL_OPENGL_H
#define _PAL_OPENGL_H

#include "pal_core.h"

// Opengl API entry
#ifdef _WIN32
#define PAL_GL_APIENTRY __stdcall
#else
#define PAL_GL_APIENTRY
#endif // _WIN32

#define PAL_GL_VENDOR_NAME_SIZE 32
#define PAL_GL_GRAPHICS_CARD_NAME_SIZE 64
#define PAL_GL_VERSION_NAME_SIZE 64

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

#define PAL_GL_PROFILE_NONE 0
#define PAL_GL_PROFILE_CORE 1
#define PAL_GL_PROFILE_COMPATIBILITY 2
#define PAL_GL_PROFILE_ES 3
#define PAL_GL_PROFILE_COUNT 4

#define PAL_GL_CONTEXT_RESET_NONE 0
#define PAL_GL_CONTEXT_RESET_NO_NOTIFICATION 1
#define PAL_GL_CONTEXT_RESET_LOSE_CONTEXT 2
#define PAL_GL_CONTEXT_RESET_COUNT 3

#define PAL_GL_RELEASE_BEHAVIOR_NONE 0
#define PAL_GL_RELEASE_BEHAVIOR_FLUSH 1
#define PAL_GL_RELEASE_BEHAVIOR_COUNT 2

#define PAL_GL_BACKEND_EGL 0
#define PAL_GL_BACKEND_GLX 1
#define PAL_GL_BACKEND_WGL 2
#define PAL_GL_BACKEND_COUNT 3

#define PAL_GL_API_OPENGL 0
#define PAL_GL_API_OPENGL_ES 1
#define PAL_GL_API_COUNT 2

/**
 * @struct PalGLContext
 * @brief Opaque handle to an opengl context.
 *
 * @sa palCreateGLContext()
 * @sa palDestroyGLContext()
 * @sa palMakeContextCurrent()
 * @sa palSwapBuffers()
 * 
 * @since 2.0
 */
typedef struct PalGLContext PalGLContext;

/**
 * @typedef PalGLExtensions
 * @brief Opengl system extensions.
 *
 * All opengl extensions follow the format `PAL_GL_EXTENSION_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint64_t PalGLExtensions;

/**
 * @typedef PalGLProfile
 * @brief Opengl context creation profiles. This is not a bitmask.
 *
 * All opengl profiles follow the format `PAL_GL_PROFILE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalGLProfile;

/**
 * @typedef PalGLContextReset
 * @brief Opengl context reset behavior. This is not a bitmask.
 *
 * All context reset behavior follow the format `PAL_GL_CONTEXT_RESET_**`
 * for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalGLContextReset;

/**
 * @typedef PalGLReleaseBehavior
 * @brief Opengl context release behavior. This is not a bitmask.
 *
 * All opengl context release behavior follow the format
 * `PAL_GL_RELEASE_BEHAVIOR_**` for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalGLReleaseBehavior;

/**
 * @typedef PalGLBackend
 * @brief Opengl backend. This is not a bitmask.
 *
 * All opengl backends follow the format `PAL_GL_BACKEND_**` for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalGLBackend;

/**
 * @typedef PalGLAPI
 * @brief Opengl api. This is not a bitmask.
 *
 * All opengl apis follow the format `PAL_GL_API_**` for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalGLAPI;

/**
 * @struct PalGLInfo
 * @brief Information about the opengl driver.
 *
 * @since 2.0
 */
typedef struct {
    PalGLExtensions extensions;                        /**< Supported extensions.*/
    uint32_t major;                                    /**< Version major.*/
    uint32_t minor;                                    /**< Version minor.*/
    PalGLBackend backend;                              /**< (eg. `PAL_GL_BACKEND_WGL`).*/
    PalGLAPI api;                                      /**< (eg. `PAL_GL_API_OPENGL_ES`).*/
    char vendor[PAL_GL_VENDOR_NAME_SIZE];              /**< Graphics card vendor name.*/
    char graphicsCard[PAL_GL_GRAPHICS_CARD_NAME_SIZE]; /**< Graphics card name.*/
    char version[PAL_GL_VERSION_NAME_SIZE];            /**< Graphics card version string.*/
} PalGLInfo;

/**
 * @struct PalGLFBConfig
 * @brief Information about an opengl framebuffer.
 *
 * @since 2.0
 */
typedef struct {
    PalBool doubleBuffer; /**< If `PAL_TRUE` double buffering is supported.*/
    PalBool stereo;       /**< If `PAL_TRUE` stereo is supported.*/
    PalBool sRGB;         /**< If `PAL_TRUE` SRGB colorspace is supported.*/
    uint16_t index;       /**< Driver index or id.*/
    uint16_t redBits;     /**< Number of bits in the red channel.*/
    uint16_t greenBits;   /**< Number of bits in the green channel.*/
    uint16_t blueBits;    /**< Number of bits in the blue channel.*/
    uint16_t alphaBits;   /**< Number of bits in the alpha channel.*/
    uint16_t depthBits;   /**< Number of depth buffer bits.*/
    uint16_t stencilBits; /**< Number of stencil buffer bits.*/
    uint16_t samples;     /**< Number of samples.*/
} PalGLFBConfig;

/**
 * @struct PalGLWindow
 * @brief Information about an opengl window.
 *
 * This can be allocated statically or dynamically since its used for
 * holding native handles. The handles will not be copied.
 *
 * @since 2.0
 */
typedef struct {
    void* instance; /**< (HINSTANCE on Win32 or wl_display on Wayland)*/
    void* window;   /**< (egl_wl_window on Wayland)*/
} PalGLWindow;

/**
 * @struct PalGLContextCreateInfo
 * @brief Creation parameters for an opengl context.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    const PalGLWindow* window;     /**< Window to create context for.*/
    const PalGLFBConfig* fbConfig; /**< The framebuffer config to use.*/
    PalGLContext* shareContext;    /**< Can be `nullptr`.*/
    PalGLProfile profile;          /**< (eg. `PAL_GL_PROFILE_CORE`).*/
    PalGLContextReset reset;       /**< (eg. `PAL_GL_CONTEXT_RESET_LOSE_CONTEXT`).*/
    PalGLReleaseBehavior release;  /**< (eg. `PAL_GL_RELEASE_BEHAVIOR_FLUSH`).*/
    PalBool forward;               /**< Create a forward compatible context.*/
    PalBool noError;               /**< Create a no error context.*/
    PalBool debug;                 /**< Create a debug context.*/
    uint32_t major;                /**< Must not be greater than what the driver supports.*/
    uint32_t minor;                /**< Must not be greater than what the driver supports.*/
} PalGLContextCreateInfo;

/**
 * @brief Initialize the opengl system.
 *
 * This must be called before any opengl function. Call `palGetSupportedGLAPIs()` to check
 * if `api` is supported on `instance`.
 *
 * The allocator will not not copied, therefore the pointer must remain valid
 * until the opengl system is shutdown.
 *
 * `instance` must not be `nullptr` and will not be freed by the opengl system. It must be valid
 * until palShutdownGL() is called. `Linux`: This is the Display associated with the connection.
 * `Windows`: This is the HINSTANCE of the process.
 *
 * @param[in] api The api to use. (eg. `PAL_GL_API_OPENGL`).
 * @param[in] instance The instance the opengl system will be tied to (eg. XDisplay).
 * @param[in] allocator Optional user-provided allocator. Set to `nullptr` to use default.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 2.0
 * @sa palShutdownGL
 */
PAL_API PalResult PAL_CALL palInitGL(
    PalGLAPI api,
    void* instance,
    const PalAllocator* allocator);

/**
 * @brief Shutdown the opengl system.
 *
 * If the opengl system has not been initialized, the function returns silently.
 * All created contexts must be destroyed before this call.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 2.0
 * @sa palInitGL
 */
PAL_API void PAL_CALL palShutdownGL();

/**
 * @brief Get information about the opengl driver.
 *
 * The opengl system must be initialized before this call. The returned
 * PalGLInfo pointer must not be freed.
 *
 * @return A pointer to a PalGLInfo on success or `nullptr` on failure.
 *
 * Thread safety: Thread-safe.
 *
 * @since 2.0
 */
PAL_API const PalGLInfo* PAL_CALL palGetGLInfo();

/**
 * @brief Return a list of all supported PalGLFBConfig on the provided window.
 *
 * The opengl system must be initialized before this call.
 *
 * Call this function first with PalGLFBConfig array set to `nullptr` to get the
 * number of supported PalGLFBConfig. Allocate memory for the PalGLFBConfig
 * array and passed in the count and the allocated array. If the count of the
 * array is less than the number of supported PalGLFBConfigs, PAL will write
 * upto that limit.
 *
 * @param[in, out] count Capacity of the PalGLFBConfig array.
 * @param[out] configs User allocated array of PalGLFBConfig.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 2.0
 * @sa palInitGL
 */
PAL_API PalResult PAL_CALL palEnumerateGLFBConfigs(
    uint32_t* count,
    PalGLFBConfig* configs);

/**
 * @brief Get the closest PalGLFBConfig from the array of PalGLFBConfig with a
 * desired PalGLFBConfig.
 *
 * The opengl system must be initialized before this call.
 * This function uses missing and score system to get the closest PalGLFBConfig.
 *
 * @param[in] configs Pointer to the array of PalGLFBConfig.
 * @param[in] count Capacity of the PalGLFBConfig array.
 * @param[in] desired The desired PalGLFBConfig.
 *
 * @return The closest PalGLFBConfig on success or `nullptr` on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 */
PAL_API const PalGLFBConfig* PAL_CALL palGetClosestGLFBConfig(
    PalGLFBConfig* configs,
    uint32_t count,
    const PalGLFBConfig* desired);

/**
 * @brief Creates an opengl context.
 *
 * The opengl system must be initialized before this call. The created context
 * will not be made current.
 *
 * The provided PalGLFBConfig must be the same as the one used to create the
 * window. Once set, it cannot be changed. To change it, you must destroy the
 * window and recreate it.
 *
 * On Wayland: PalGLContextCreateInfo::PalGLWindow::window is the wl_egl_window
 * not the wl_surface.
 *
 * @param[in] info Pointer to a PalGLContextCreateInfo struct that specifies parameters.
 * @param[out] outContext Pointer to a PalGLContext to recieve the created context.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 2.0
 * @sa palDestroyGLContext
 */
PAL_API PalResult PAL_CALL palCreateGLContext(
    const PalGLContextCreateInfo* info,
    PalGLContext** outContext);

/**
 * @brief Destroy the provided opengl context.
 *
 * The opengl system must be initialized before this call.
 * The context must not be current in any thread before this call.
 *
 * @param[in] context Pointer to the context to destroy.
 *
 * Thread safety: `context` must be externally synchronized.
 *
 * @since 2.0
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
 * `PAL_RESULT_CODE_INVALID_HANDLE`.
 *
 * @param[in] glWindow Pointer to the opengl window.
 * @param[in] context Pointer to the context to make current.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Only one thread may have the current context at a time.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palMakeContextCurrent(
    PalGLWindow* glWindow,
    PalGLContext* context);

/**
 * @brief Get the pointer to a named opengl function.
 *
 * The opengl system must be initialized before this call.
 *
 * @param[in] name UTF-8 string for the function name.
 *
 * @return the pointer to the function on success or `nullptr` on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 * @sa palInitGL
 */
PAL_API void* PAL_CALL palGetGLProcAddress(const char* name);

/**
 * @brief Present the contents of the back buffer of the provided context to
 * the screen.
 *
 * The opengl system must be initialized before this call.
 *
 * @param[in] glWindow Pointer to the opengl window.
 * @param[in] context Pointer to the context.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from a thread that has a
 * bound context.
 *
 * @since 2.0
 * @sa palMakeContextCurrent
 */
PAL_API PalResult PAL_CALL palSwapBuffers(
    PalGLWindow* glWindow,
    PalGLContext* context);

/**
 * @brief Set the swap interval for the current context.
 *
 * The opengl system must be initialized before this call.
 * This affects the currently bound context on the calling thread.
 * `PAL_GL_EXTENSION_SWAP_CONTROL` must be supported otherwise undefined behavoir.
 *
 * @param[in] interval The swap interval
 *
 * Thread safety: Must only be called from a thread with a bound
 * context.
 *
 * @since 2.0
 * @sa palMakeContextCurrent
 */
PAL_API void PAL_CALL palSetSwapInterval(int32_t interval);

/**
 * @brief Get supported opengl APIs
 *
 * @param[in] instance The instance (eg. HINSTANCE or XDisplay).
 *
 * @return An array of bools or `nullptr` on failure.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 2.0
 */
PAL_API const PalBool* PAL_CALL palGetSupportedGLAPIs(void* instance);

/** @} */

#endif // _PAL_OPENGL_H
