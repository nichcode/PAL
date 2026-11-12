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

#ifndef PAL_GRAPHICS_INIT_H
#define PAL_GRAPHICS_INIT_H

#include "pal2/core/base.h"
#include "pal2/core/result.h"
#include "pal2/core/memory.h"

/**
 * @typedef PalDebugMessageSeverity
 * @brief Debug message severity.
 *
 * Identifies how severe a debug message is.
 *
 * All values of this type follow the format `PAL_DEBUG_MESSAGE_SEVERITY_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalDebugMessageSeverity;

/**
 * @typedef PalDebugMessageType
 * @brief Debug message type.
 *
 * Identifies the category of a debug message.
 *
 * All values of this type follow the format `PAL_DEBUG_MESSAGE_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalDebugMessageType;

/**
 * @typedef PalDescriptorIndexingFlags
 * @brief Descriptor indexing flags.
 *
 * Identifies the descriptor indexing behaviors of descriptors.
 *
 * All values of this type follow the format `PAL_DESCRIPTOR_INDEXING_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalDescriptorIndexingFlags;

/**
 * @typedef PalGraphicsBackendVtableVersion
 * @brief Graphics backend vtable version.
 *
 * Identifies the version of a graphics backend vtable.
 *
 * All values of this type follow the format `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalGraphicsBackendVtableVersion;

/**
 * @typedef PalDebugCallback
 * @brief Debug callback function.
 *
 * `msg` is only valid for the duration of the callback and must not be modified or freed by the callback;
 * the memory is owned by PAL.
 *
 * The callback may be called concurrently from multiple threads, so the implementation must be thread safe.
 *
 * The function signature should look like this:
 * @code
 * void PAL_CALL debugCallback(
 *     void* userData,
 *     PalDebugMessageSeverity severity,
 *     PalDebugMessageType type,
 *     const char* msg);
 * @endcode
 *
 * @param[in] userData User-defined data passed to the callback, or `nullptr`.
 * @param[in] severity Message severity.
 * @param[in] type Message type.
 * @param[in] msg Debug message (NULL-terminated UTF-8 string).
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palInitGraphics
 */
typedef void(PAL_CALL* PalDebugCallback)(
    void* userData,
    PalDebugMessageSeverity severity,
    PalDebugMessageType type,
    const char* msg);

/**
 * @brief Maximum number of custom backends.
 * @ingroup pal_graphics
 */
#define PAL_MAX_CUSTOM_BACKENDS 16

/**
 * @defgroup gfx_backend_table_version Graphics Backend Vtable Version
 * @ingroup pal_graphics
 * @{
 */
#define PAL_GRAPHICS_BACKEND_VTABLE_VERSION_1 0 /**< Version 1 (@ref PalGraphicsBackendVtable1). */
#define PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2 1 /**< Version 2 (@ref PalGraphicsBackendVtable2). */
#define PAL_GRAPHICS_BACKEND_VTABLE_VERSION_COUNT 2 /**< Number of graphics backend vtable versions. */
/** @} */

/**
 * @defgroup debug_msg_severities Debug Message Severities
 * @ingroup pal_graphics
 * @{
 */
#define PAL_DEBUG_MESSAGE_SEVERITY_INFO 0 /**< Informational message. */
#define PAL_DEBUG_MESSAGE_SEVERITY_WARNING 1 /**< Warning message. */
#define PAL_DEBUG_MESSAGE_SEVERITY_ERROR 2 /**< Error message. */
#define PAL_DEBUG_MESSAGE_SEVERITY_COUNT 3 /**< Number of debug message severities. */
/** @} */

/**
 * @defgroup debug_msg_types Debug Message Types
 * @ingroup pal_graphics
 * @{
 */
#define PAL_DEBUG_MESSAGE_TYPE_GENERAL 0 /**< General message. */
#define PAL_DEBUG_MESSAGE_TYPE_VALIDATION 1 /**< Validation message. */
#define PAL_DEBUG_MESSAGE_TYPE_PERFORMANCE 2 /**< Performance message. */
#define PAL_DEBUG_MESSAGE_TYPE_COUNT 3 /**< Number of debug message types. */
/** @} */

/**
 * @struct PalGraphicsDebugger
 * @brief Graphics debugger.
 *
 * Passed to @ref palInitGraphics(). The struct and any state referenced by it are not copied and must
 * remain valid until @ref palShutdownGraphics() is called.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalGraphicsDebugger {
    void* userData;              /**< User-defined data passed to the callback, or `nullptr`. */
    PalDebugCallback callback;   /**< Debug callback function, or `nullptr` to disable debugging. */
    PalBool enableGPUValidation; /**< Whether to enable GPU-based validation. */
    PalBool denyGeneral;         /**< Do not receive general messages. */
    PalBool denyValidation;      /**< Do not receive validation messages. */
    PalBool denyPerformance;     /**< Do not receive performance messages. */
    PalBool denyInfoSeverity;    /**< Do not receive info severity messages. */
    PalBool denyWarningSeverity; /**< Do not receive warning severity messages. */
    PalBool denyErrorSeverity;   /**< Do not receive error severity messages. */
    uint32_t reserved;           /**< Must be set to `0`. */
} PalGraphicsDebugger;

/**
 * @struct PalGraphicsBackendInfo
 * @brief Graphics backend information.
 *
 * The backend will not be copied, so it must remain valid for as long
 * as PAL may use it. All backend handle implementation must reserve the first field
 * as `void*`. The field will be used by the graphics system.
 *
 * all required function implementation of `version` must be valid, this will be validated
 * when @ref palInitGraphics() is called. See [Custom Graphics Backend Guide](@ref custom_gfx_backend)
 * for more information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalGraphicsBackendInfo {
    const void* vtable;                      /**< Backend vtable. */
    PalGraphicsBackendVtableVersion version; /**< Backend vtable version. */
    uint32_t reserved;                       /**< Must be set to `0`. */
} PalGraphicsBackendInfo;

/**
 * @brief Initializes the graphics system.
 *
 * The function initializes the graphics system. If the graphics system has been
 * initialized, the function does nothing.
 *
 * `allocator` and `debugger` are not copied. The allocator and the debugger with any state
 * referenced by them must remain valid until @ref palShutdownGraphics() is called.
 * If `debugger->callback` is `nullptr`, the function will succeed but debugging will be
 * disabled.
 *
 * This function will fail if a custom backend vtable does not meet its version requirements.
 * The backend vtables are not copied and must remain valid until @ref palShutdownGraphics()
 * is called.
 *
 * @param[in] debugger Debugger to use, or `nullptr` to disable debugging.
 * @param[in] allocator Allocator to use, or `nullptr` for the default.
 * @param[in] backendCount Number of custom backends.
 * @param[in] backends Custom backends array.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety Must be called from the main thread.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palShutdownGraphics
 */
PAL_API PalResult PAL_CALL palInitGraphics(
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator,
    uint32_t backendCount,
    const PalGraphicsBackendInfo* backends);

/**
 * @brief Shuts down the graphics system.
 *
 * All devices, swapchains and other objects created through the graphics system must be
 * destroyed before this call. If the graphics system has not been initialized, the
 * function returns silently.
 *
 * @thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palInitGraphics
 */
PAL_API void PAL_CALL palShutdownGraphics();

#endif // PAL_GRAPHICS_INIT_H