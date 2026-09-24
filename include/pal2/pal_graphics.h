/**
 * @brief This is the header file for PAL Graphics API.
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
 * @struct PalWorkGroupBuildData
 * @brief Contains build information about a workgroup.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalWorkGroupBuildData::workCount
 * The workload per dimension.
 * 
 * @var PalWorkGroupBuildData::workGroupSize
 * The maximum workgroup size per dimension of the adapter.
 * 
 * @var PalWorkGroupBuildData::workGroupCount
 * The maximum number of workgroups per dimension of the adapter.
 */
typedef struct PalWorkGroupBuildData
{
    uint32_t workCount[3];
    uint32_t workGroupSize[3];
    uint32_t workGroupCount[3];
} PalWorkGroupBuildData;

/**
 * @struct PalWorkGroupInfo
 * @brief Contains information about a dispatch tile.
 * 
 * @since Added in version 2.0
 * 
 * @var PalWorkGroupInfo::workGroupBase
 * The offsets per dimension of the dispatch tile.
 * 
 * @var PalWorkGroupInfo::workGroupBase
 * The number of workgroups per dimension of the dispatch tile.
 */
typedef struct PalWorkGroupInfo
{
    uint32_t workGroupBase[3];
    uint32_t workGroupCount[3];
} PalWorkGroupInfo;

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

/**
 * @brief Create a command pool from a device.
 *
 * The created command pool must be destroyed using `palDestroyCommandPool()`.
 *
 * @param[in] device Device that creates the command pool.
 * @param[in] queue Queue the command pool buffers will be submitted to.
 * @param[out] outPool Pointer to a PalCommandPool to recieve the created command pool.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palDestroyCommandPool
 */
PAL_API PalResult PAL_CALL palCreateCommandPool(
    PalDevice* device,
    PalQueue* queue,
    PalCommandPool** outPool);

/**
 * @brief Destroy a command pool.
 *
 * All command buffers allocated from the pool must be freed before this call,
 * otherwise undefined behavior.
 *
 * @param[in] pool Command pool to destroy.
 *
 * Thread safety: Thread safe if the device used to create the command pool is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateCommandPool
 */
PAL_API void PAL_CALL palDestroyCommandPool(PalCommandPool* pool);

/**
 * @brief Reset all command buffers allocated from the provided command pool.
 *
 * @param[in] pool Command pool to reset its command buffers.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `pool` is externally synchronized.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palResetCommandPool(PalCommandPool* pool);

/**
 * @brief Create a descriptor set layout that defines the bindings used by descriptor sets.
 *
 * The created descriptor set layout must be destroyed using `palDestroyDescriptorSetLayout()`.
 *
 * This defines the layout, ordering and the number of descriptors a descriptor set uses.
 *
 * The layouts should reflect the exact layout of the shaders. Eg.
 * descriptorBindings[2] = { sampler, sampled image } is different from
 * descriptorBindings[2] = { sampled image, sampler }. The ordering must be correct.
 *
 * @param[in] device Device that creates the descriptor set layout.
 * @param[in] info Pointer to a PalDescriptorSetLayoutCreateInfo struct that specifies parameters.
 * @param[out] outLayout Pointer to a PalDescriptorSetLayout to recieve the created layout.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palDestroyDescriptorSetLayout
 */
PAL_API PalResult PAL_CALL palCreateDescriptorSetLayout(
    PalDevice* device,
    const PalDescriptorSetLayoutCreateInfo* info,
    PalDescriptorSetLayout** outLayout);

/**
 * @brief Destroy a descriptor set layout.
 *
 * @param[in] layout Descriptor set layout to destroy.
 *
 * Thread safety: Thread safe if the device used to create the descriptor set layout is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateDescriptorSetLayout
 */
PAL_API void PAL_CALL palDestroyDescriptorSetLayout(PalDescriptorSetLayout* layout);

/**
 * @brief Create a descriptor pool to allocate descriptor sets.
 *
 * The created descriptor pool must be destroyed using `palDestroyDescriptorPool()`.
 *
 * @param[in] device Device that creates the descriptor pool.
 * @param[in] info Pointer to a PalDescriptorPoolCreateInfo struct that specifies parameters.
 * @param[out] outPool Pointer to a PalDescriptorPool to recieve the created descriptor pool.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palDestroyDescriptorPool
 */
PAL_API PalResult PAL_CALL palCreateDescriptorPool(
    PalDevice* device,
    const PalDescriptorPoolCreateInfo* info,
    PalDescriptorPool** outPool);

/**
 * @brief Destroy a descriptor pool.
 *
 * @param[in] pool Descriptor pool to destroy.
 *
 * Thread safety: Thread safe if the device used to create the descriptor pool is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateDescriptorPool
 */
PAL_API void PAL_CALL palDestroyDescriptorPool(PalDescriptorPool* pool);

/**
 * @brief Reset the provided descriptor pool. This resets all allocated descriptor sets.
 *
 * @param[in] pool Descriptor pool to reset.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `pool` is externally synchronized.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palResetDescriptorPool(PalDescriptorPool* pool);

/**
 * @brief Allocate a descriptor set from the provided descriptor pool.
 *
 * The descriptor set will be allocated uninitialized therefore update it before
 * use except the case where descriptor indexing is enabled.
 *
 * `pool` and `layout` must either be created with descriptor indexing enabled or not. Any other
 * pair will fail and return `PAL_RESULT_INVALID_OPERATION`.
 *
 * @param[in] device Device to allocate descriptor set on.
 * @param[in] pool Descriptor pool to allocate descriptor set from.
 * @param[in] layout Descriptor set layout that defines the bindings.
 * @param[out] outSet Pointer to a PalDescriptorSet to recieve the created descriptor set.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` and `pool` are externally synchronized.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palAllocateDescriptorSet(
    PalDevice* device,
    PalDescriptorPool* pool,
    PalDescriptorSetLayout* layout,
    PalDescriptorSet** outSet);

/**
 * @brief Update a descriptor set with descriptors (resources).
 *
 * If the write info has no valid resource handle, then `PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS`
 * must be supported and enabled when creating the device. Otherwise behavior is undefined.
 *
 * @param[in] device The Device. Must match the one used to allocate descriptor set.
 * @param[in] count Capacity of the PalDescriptorSetWriteInfo array.
 * @param[in] infos Array of PalDescriptorSetWriteInfo to write.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palUpdateDescriptorSet(
    PalDevice* device,
    uint32_t count,
    PalDescriptorSetWriteInfo* infos);

/**
 * @brief Build work group info(s) from work inputs specified in pixels, vertices etc.
 *
 * Call this function first with PalWorkGroupInfo array set to `nullptr` to get the number of work
 * group infos. Allocate memory for the PalWorkGroupInfo array and passed in the count and the
 * allocated array. If the count of the array is less than the number of work group infos, PAL will
 * write upto that limit.
 *
 * If the count is 0 and the PalWorkGroupInfo array is `nullptr`, the function fails
 * and returns `PAL_FALSE`.
 *
 * This function works the maths for how many work groups to dispatch in each axis and how many
 * times it needs to be dispatch in order for the work to be done. It works well with
 * palCmdDispatchBase() since its also gives the base for each work group.
 *
 * @param[in] data Pointer to a PalWorkGroupBuildData with parameters.
 * @param[in, out] count Capacity of the PalWorkGroupInfo array.
 * @param[out] infos Pointer to an Array of PalWorkGroupInfo.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palCmdDrawMeshTasks
 * @sa palCmdDrawMeshTasksIndirect
 * @sa palCmdDrawMeshTasksIndirectCount
 * @sa palCmdDispatch
 * @sa palCmdDispatchBase
 */
PAL_API void PAL_CALL palBuildWorkGroupInfo(
    const PalWorkGroupBuildData* data,
    uint32_t* count,
    PalWorkGroupInfo* info);

/** @} */

#endif // PAL_GRAPHICS_H