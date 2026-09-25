/**
 * @brief This is the header file for PAL Graphics Module API.
 *
 * It defines all the types and functions of the graphics module.
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
 * @defgroup pal_graphics Graphics Module
 */

#ifndef PAL_GRAPHICS_H
#define PAL_GRAPHICS_H

#include "pal_core.h"

#define PAL_MAX_CUSTOM_BACKENDS 16

/**
 * @defgroup debug_message_severities Debug Message Severities
 * @brief Debug message severities
 * 
 * @{
 */
#define PAL_DEBUG_MESSAGE_SEVERITY_INFO 0
#define PAL_DEBUG_MESSAGE_SEVERITY_WARNING 1
#define PAL_DEBUG_MESSAGE_SEVERITY_ERROR 2
#define PAL_DEBUG_MESSAGE_SEVERITY_COUNT 3
/** @} */

/**
 * @defgroup debug_message_types Debug Message Types
 * @brief Debug message types
 * 
 * @{
 */
#define PAL_DEBUG_MESSAGE_TYPE_GENERAL 0
#define PAL_DEBUG_MESSAGE_TYPE_VALIDATION 1
#define PAL_DEBUG_MESSAGE_TYPE_PERFORMANCE 2
#define PAL_DEBUG_MESSAGE_TYPE_COUNT 3
/** @} */

/**
 * @typedef PalDebugMessageSeverity
 * @brief Debugger messages severity types used to filter incoming messages.
 * 
 * All values of this type follow the format `PAL_DEBUG_MESSAGE_SEVERITY_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalDebugMessageSeverity;

/**
 * @typedef PalDebugMessageType
 * @brief Debugger messages types used to filter incoming messages.
 * 
 * All values of this type follow the format `PAL_DEBUG_MESSAGE_TYPE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalDebugMessageType;

/**
 * @typedef PalDebugCallback
 * @brief Function pointer type used for debug callbacks.
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
 * @param userData User data passed from the debugger. Can be `nullptr`.
 * @param severity Severity of the message.
 * @param type Type of the message.
 * @param msg Null-terminated `UTF-8` debug message.
 *
 * @since Added in version 2.0
 * @sa palInitGraphics
 */
typedef void(PAL_CALL* PalDebugCallback)(
    void* userData,
    PalDebugMessageSeverity severity,
    PalDebugMessageType type,
    const char* msg);

/**
 * @struct PalGraphicsDebugger
 * @brief Contains information about a graphics debugger.
 *
 * The debugger will not be initialized if 
 * PalGraphicsDebugger::callback is not valid.
 *
 * @since Added in version 2.0
 * 
 * @var PalGraphicsDebugger::userData
 * User data passed to ::callback. Can be `nullptr`.
 * 
 * @var PalGraphicsDebugger::callback
 * The function to forward debug messages to. Must not be `nullptr`.
 * 
 * @var PalGraphicsDebugger::enableGPUValidation
 * `PAL_TRUE` to enable GPU-Based Validation.
 * 
 * @var PalGraphicsDebugger::denyGeneral
 * If `PAL_TRUE`, general type debug messages wil be filtered out.
 * 
 * @var PalGraphicsDebugger::denyValidation
 * If `PAL_TRUE`, validation type debug messages wil be filtered out.
 * 
 * @var PalGraphicsDebugger::denyPerformance
 * If `PAL_TRUE`, performance type debug messages wil be filtered out.
 * 
 * @var PalGraphicsDebugger::denyInfoSeverity
 * If `PAL_TRUE`, information severity debug messages wil be filtered out.
 * 
 * @var PalGraphicsDebugger::denyWarningSeverity
 * If `PAL_TRUE`, warning severity debug messages wil be filtered out.
 * 
 * @var PalGraphicsDebugger::denyErrorSeverity
 * If `PAL_TRUE`, error severity debug messages wil be filtered out.
 * 
 * @var PalGraphicsDebugger::reserved
 * Not used. Set to `0`.
 */
typedef struct PalGraphicsDebugger
{
    void* userData;
    PalDebugCallback callback;
    PalBool enableGPUValidation;
    PalBool denyGeneral;
    PalBool denyValidation;
    PalBool denyPerformance;
    PalBool denyInfoSeverity;
    PalBool denyWarningSeverity;
    PalBool denyErrorSeverity;
    uint32_t reserved;
} PalGraphicsDebugger;

/**
 * @struct PalGraphicsBackendInfo
 * @brief Contains information of a graphics backend.
 *
 * All backend handles implementation (eg. struct CustomBuffer) must reserve
 * its first field as a `void*`. This will be used by the graphics layer.
 *
 * Each backend Vtable version (eg. `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_1`) 
 * has required functions that must be present implemented. This will be 
 * validated at initialization. See version constant for the required 
 * functions. Optional functions have their own requirements.
 * 
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalGraphicsBackendInfo::vtable
 * The graphics backend vtable.
 * 
 * @var PalGraphicsBackendInfo::version
 * The version of the graphics backend vtable.
 * 
 * @var PalGraphicsBackendInfo::version
 * Not used. Set to `0`.
 */
typedef struct PalGraphicsBackendInfo
{
    const void* vtable;
    PalGraphicsBackendVtableVersion version;
    uint32_t reserved;
} PalGraphicsBackendInfo;

/**
 * @brief Initialize the graphics system.
 *
 * The debugger, allocator and custom backends will not not copied, therefore the pointers must
 * remain valid until the graphics system is shutdown. Set the debugger to `nullptr` to disable
 * debugging and validation layers.
 *
 * If `debugger` is not `nullptr` and there is no debug layers, this function will not fail but
 * debugging will be disabled.
 *
 * All backends must have their vtable functions fully set according to the version requirements.
 * All required pointers must be set. If an optional feature is not supported, `nullptr` must be
 * set and its appropriate feature bit (eg. `PAL_ADAPTER_FEATURE_RAY_TRACING`) must not be set.
 *
 * @param[in] debugger Optional debugger. Set to `nullptr` to disable debugging and validation
 * layers.
 * @param[in] allocator Optional user-provided allocator. Set to `nullptr` to use default.
 * @param[in] customBackendCount The number of custom backends in `customBackends`.
 * @param[in] customBackends Pointer to an array of custom backends.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palShutdownGraphics
 */
PAL_API PalResult PAL_CALL palInitGraphics(
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator,
    uint32_t customBackendCount,
    const PalGraphicsBackendInfo* customBackends);

/**
 * @brief Shutdown the graphics system.
 *
 * If the graphics system has not been initialized, the function returns silently.
 * All created devices, queues, images, swapchains etc must be destroyed before this call.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palInitGraphics
 */
PAL_API void PAL_CALL palShutdownGraphics();

#endif // PAL_GRAPHICS_H