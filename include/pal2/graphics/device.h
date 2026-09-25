/**
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

#ifndef PAL_GRAPHICS_DEVICE_H
#define PAL_GRAPHICS_DEVICE_H

#include "adapter.h"

/**
 * @defgroup descriptor_indexing_flags Descriptor Indexing Flags
 * @brief Descriptor indexing flags
 * 
 * @{
 */
#define PAL_DESCRIPTOR_INDEXING_FLAG_NONE 0
#define PAL_DESCRIPTOR_INDEXING_FLAG_UPDATE_AFTER_BIND (1U << 0)
#define PAL_DESCRIPTOR_INDEXING_FLAG_PARTIALLY_BOUND (1U << 1)
#define PAL_DESCRIPTOR_INDEXING_FLAG_NON_UNIFORM_INDEXING (1U << 2)
/** @} */

/**
 * @typedef PalDescriptorIndexingFlags
 * @brief Descriptor indexing subfeature flags.
 * 
 * These flags show the capabilities of the descriptor indexing feature.
 * Each flag determines the operations that are allowed.
 * 
 * All values of this type follow the format 
 * `PAL_DESCRIPTOR_INDEXING_FLAG_*` for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalDescriptorIndexingFlags;

/**
 * @struct PalDevice
 * @brief Opaque handle to a device.
 *
 * @since Added in version 2.0
 */
typedef struct PalDevice PalDevice;

/**
 * @struct PalSamplerAnisotropyCapabilities
 * @brief Contains sampler anisotropy capabilities of an adapter (GPU).
 *
 * @since Added in version 2.0
 * 
 * @var PalSamplerAnisotropyCapabilities::maxAnisotropy
 * The maximum texture filtering level.
 */
typedef struct PalSamplerAnisotropyCapabilities
{
    uint32_t maxAnisotropy;
} PalSamplerAnisotropyCapabilities;

/**
 * @struct PalMultiViewCapabilities
 * @brief Contains multi view capabilities of an adapter (GPU).
 *
 * @since Added in version 2.0
 * 
 * @var PalMultiViewCapabilities::maxViewCount
 * The maximum number of views.
 */
typedef struct PalMultiViewCapabilities
{
    uint32_t maxViewCount;
} PalMultiViewCapabilities;

/**
 * @struct PalMultiViewportCapabilities
 * @brief Contains multi viewport capabilities of an adapter (GPU).
 *
 * @since Added in version 2.0
 * 
 * @var PalMultiViewportCapabilities::maxCount
 * The maximum number of simultaneous viewports.
 */
typedef struct PalMultiViewportCapabilities
{
    uint32_t maxCount;
} PalMultiViewportCapabilities;

/**
 * @struct PalDepthStencilCapabilities
 * @brief Contains depth stencil capabilities of an adapter (GPU).
 *
 * @since Added in version 2.0
 * 
 * @var PalDepthStencilCapabilities::supportedDepthResolveModes
 * A bitmask of supported depth resolve modes. A specific resolve mode
 * should be check like this: @nl
 * palIsSupported(::supportedDepthResolveModes, `PAL_RESOLVE_MODE_AVERAGE`).
 * 
 * @var PalDepthStencilCapabilities::supportedStencilResolveModes
 * A bitmask of supported stencil resolve modes. A specific resolve mode
 * should be check like this: @nl
 * palIsSupported(::supportedStencilResolveModes, `PAL_RESOLVE_MODE_MIN`).
 * 
 * @var PalDepthStencilCapabilities::supportsIndependentResolve
 * If `PAL_TRUE`, depth and stencil can have seperate resolve modes.
 * 
 * @var PalDepthStencilCapabilities::supportsIndependentResolveNone
 * If `PAL_TRUE`, depth or stencil can be `PAL_RESOLVE_MODE_NONE` 
 * while the other is resolved.
 */
typedef struct PalDepthStencilCapabilities
{
    uint32_t supportedDepthResolveModes;
    uint32_t supportedStencilResolveModes;
    PalBool supportsIndependentResolve;
    PalBool supportsIndependentResolveNone;
} PalDepthStencilCapabilities;

/**
 * @struct PalFragmentShadingRateCapabilities
 * @brief Contains fragment shading rate capabilities of an adapter (GPU).
 *
 * @since Added in version 2.0
 * 
 * @var PalFragmentShadingRateCapabilities::supportedShadingRates
 * A bitmask of supported fragment shading rates. A specific shading rate
 * should be check like this: @nl
 * palIsSupported(::supportedShadingRates, `PAL_FRAGMENT_SHADING_RATE_2X2`).
 * 
 * @var PalFragmentShadingRateCapabilities::supportedCombinerOps
 * A bitmask of supported fragment shading rate combiner operations.
 * A specific combiner operation should be check like this: @nl
 * palIsSupported(::supportedCombinerOps,
 * `PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP`).
 * 
 * @var PalFragmentShadingRateCapabilities::minTexelWidth
 * The minimum fragment texel width.
 * 
 * @var PalFragmentShadingRateCapabilities::minTexelHeight
 * The minimum fragment texel height.
 * 
 * @var PalFragmentShadingRateCapabilities::maxTexelWidth
 * The maximum fragment texel width.
 * 
 * @var PalFragmentShadingRateCapabilities::maxTexelHeight
 * The maximum fragment texel height.
 */
typedef struct PalFragmentShadingRateCapabilities
{
    uint32_t supportedShadingRates;
    uint32_t supportedCombinerOps;
    uint32_t minTexelWidth;
    uint32_t minTexelHeight;
    uint32_t maxTexelWidth;
    uint32_t maxTexelHeight;
} PalFragmentShadingRateCapabilities;

/**
 * @struct PalMeshShaderCapabilities
 * @brief Contains mesh shader capabilities of an adapter (GPU).
 *
 * @since Added in version 2.0
 * 
 * @var PalMeshShaderCapabilities::maxOutputPrimitives
 * The maximum number of primitives per mesh workgroup.
 * 
 * @var PalMeshShaderCapabilities::maxOutputVertices
 * The maximum number of vertices per mesh workgroup.
 * 
 * @var PalMeshShaderCapabilities::maxWorkGroupInvocations
 * The maximum number of shader invocations inside a single mesh workgroup
 * 
 * @var PalMeshShaderCapabilities::maxTaskWorkGroupInvocations
 * The maximum number of shader invocations inside a single task workgroup
 * 
 * @var PalMeshShaderCapabilities::maxWorkGroupCount
 * The maximum number of mesh workgroups per dimension.
 * 
 * @var PalMeshShaderCapabilities::maxTaskWorkGroupCount
 * The maximum number of task workgroups per dimension.
 */
typedef struct PalMeshShaderCapabilities
{
    uint32_t maxOutputPrimitives;
    uint32_t maxOutputVertices;
    uint32_t maxWorkGroupInvocations;
    uint32_t maxTaskWorkGroupInvocations;
    uint32_t maxWorkGroupCount[3];
    uint32_t maxTaskWorkGroupCount[3];
} PalMeshShaderCapabilities;

/**
 * @struct PalRayTracingCapabilities
 * @brief Contains ray tracing capabilities of an adapter (GPU).
 *
 * @since Added in version 2.0
 * 
 * @var PalRayTracingCapabilities::maxRecursionDepth
 * The maximum ray recursion depth.
 * 
 * @var PalRayTracingCapabilities::maxHitAttributeSize
 * The maximum ray attribute size in bytes.
 * 
 * @var PalRayTracingCapabilities::maxInstanceCount
 * The maximum number of ray instances.
 * 
 * @var PalRayTracingCapabilities::maxPrimitiveCount
 * The maximum number of ray primitives.
 * 
 * @var PalRayTracingCapabilities::maxGeometryCount
 * The maximum number of ray geometries.
 * 
 * @var PalRayTracingCapabilities::maxPayloadSize
 * The maximum ray payload size in bytes.
 * 
 * @var PalRayTracingCapabilities::maxDispatchInvocations
 * The maximum number of ray dispatch threads.
 */
typedef struct PalRayTracingCapabilities
{
    uint32_t maxRecursionDepth;
    uint32_t maxHitAttributeSize;
    uint32_t maxInstanceCount;
    uint32_t maxPrimitiveCount;
    uint32_t maxGeometryCount;
    uint32_t maxPayloadSize;
    uint32_t maxDispatchInvocations;
} PalRayTracingCapabilities;

/**
 * @struct PalDescriptorIndexingCapabilities
 * @brief Contains descriptor indexing capabilities of an adapter (GPU).
 *
 * @since Added in version 2.0
 * 
 * @var PalDescriptorIndexingCapabilities::flags
 * A bitmask of supported descriptor indexing flags.
 * (eg. `PAL_DESCRIPTOR_INDEXING_FLAG_UPDATE_AFTER_BIND`)
 * 
 * @var PalDescriptorIndexingCapabilities::maxPerStageSampledImages
 * The maximum sampled images per shader stage.
 * 
 * @var PalDescriptorIndexingCapabilities::maxPerSetSampledImages
 * The maximum sampled images per descriptor set.
 * 
 * @var PalDescriptorIndexingCapabilities::maxPerStageStorageImages
 * The maximum storage images per shader stage.
 * 
 * @var PalDescriptorIndexingCapabilities::maxPerSetStorageImages
 * The maximum storage images per descriptor set.
 * 
 * @var PalDescriptorIndexingCapabilities::maxPerStageSamplers
 * The maximum samplers per shader stage.
 * 
 * @var PalDescriptorIndexingCapabilities::maxPerSetSamplers
 * The maximum samplers per descriptor set.
 * 
 * @var PalDescriptorIndexingCapabilities::maxPerStageStorageBuffers
 * The maximum storage buffers per shader stage.
 * 
 * @var PalDescriptorIndexingCapabilities::maxPerSetStorageBuffers
 * The maximum storage buffers per descriptor set.
 * 
 * @var PalDescriptorIndexingCapabilities::maxPerStageUniformBuffers
 * The maximum uniform buffers per shader stage.
 * 
 * @var PalDescriptorIndexingCapabilities::maxPerSetUniformBuffers
 * The maximum uniform buffers per descriptor set.
 * 
 * @var PalDescriptorIndexingCapabilities::maxPerStageAccelerationStructure
 * The maximum acceleration structures per shader stage.
 * 
 * @var PalDescriptorIndexingCapabilities::maxPerSetAccelerationStructure
 * The maximum acceleration structures per descriptor set.
 */
typedef struct PalDescriptorIndexingCapabilities
{
    PalDescriptorIndexingFlags flags;
    uint32_t maxPerStageSampledImages;
    uint32_t maxPerSetSampledImages;
    uint32_t maxPerStageStorageImages; 
    uint32_t maxPerSetStorageImages;   
    uint32_t maxPerStageSamplers;      
    uint32_t maxPerSetSamplers;        
    uint32_t maxPerStageStorageBuffers;
    uint32_t maxPerSetStorageBuffers;  
    uint32_t maxPerStageUniformBuffers;
    uint32_t maxPerSetUniformBuffers;  
    uint32_t maxPerStageAccelerationStructure;
    uint32_t maxPerSetAccelerationStructure;
} PalDescriptorIndexingCapabilities;

/**
 * @brief Create a device from an adapter (GPU).
 *
 * PAL does not enable any features by default. The created device must be destroyed using
 * `palDestroyDevice()`.
 *
 * Every requested feature must be supported by the adapter. Use `palGetAdapterFeatures` to check
 * the supported features of the adapter that can be enabled. Using a feature which is not
 * supported will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] adapter Adapter that creates the device.
 * @param[in] features Adapter features to enable. Must be supported.
 * @param[out] outDevice Pointer to a PalDevice to recieve the created device.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `adapter` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palDestroyDevice
 */
PAL_API PalResult PAL_CALL palCreateDevice(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice);

/**
 * @brief Destroy a device.
 *
 * All resources created with the device must be destroyed before this call.
 *
 * @param[in] device Pointer to the device to destroy.
 *
 * Thread safety: Thread safe if the adapter used to create the device is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateDevice
 */
PAL_API void PAL_CALL palDestroyDevice(PalDevice* device);

/**
 * @brief Get the native device lost reason code.
 *
 * This function returns the backend-specific native code for the reason the device
 * was lost. Backends that do not provide explicit device lost reason codes return
 * their standard device lost code.
 *
 * @param[in] device The device.
 *
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
 */
PAL_API uint32_t PAL_CALL palGetDeviceLostReason(PalDevice* device);

/**
 * @brief Get sampler anisotropy feature capabilites or limits about a device.
 *
 * `PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY` must be supported and enabled when creating the
 * device. Otherwise behavior is undefined.
 *
 * @param[in] device Device to query sampler anisotropy feature capabilities on.
 * @param[out] caps Pointer to a PalSamplerAnisotropyCapabilities to fill.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palQuerySamplerAnisotropyCapabilities(
    PalDevice* device,
    PalSamplerAnisotropyCapabilities* caps);

/**
 * @brief Get multi view feature capabilites or limits about a device.
 *
 * `PAL_ADAPTER_FEATURE_MULTI_VIEW` must be supported and enabled when creating the
 * device. Otherwise behavior is undefined.
 *
 * @param[in] device Device to query multi view feature capabilities on.
 * @param[out] caps Pointer to a PalMultiViewCapabilities to fill.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palQueryMultiViewCapabilities(
    PalDevice* device,
    PalMultiViewCapabilities* caps);

/**
 * @brief Get multi viewport feature capabilites or limits about a device.
 *
 * `PAL_ADAPTER_FEATURE_MULTI_VIEWPORT` must be supported and enabled when creating the
 * device. Otherwise behavior is undefined.
 *
 * @param[in] device Device to query multi viewport feature capabilities on.
 * @param[out] caps Pointer to a PalMultiViewportCapabilities to fill.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palQueryMultiViewportCapabilities(
    PalDevice* device,
    PalMultiViewportCapabilities* caps);

/**
 * @brief Get depth stencil feature capabilites or limits about a device.
 *
 * `PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE` must be supported and enabled when creating the
 * device. Otherwise behavior is undefined.
 *
 * @param[in] device Device to query depth stencil feature capabilities on.
 * @param[out] caps Pointer to a PalDepthStencilCapabilities to fill.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palQueryDepthStencilCapabilities(
    PalDevice* device,
    PalDepthStencilCapabilities* caps);

/**
 * @brief Get fragment shading rate feature capabilites or limits about a device.
 *
 * `PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE` must be supported and enabled when creating the
 * device. Otherwise behavior is undefined.
 *
 * @param[in] device Device to query fragment shading rate feature capabilities on.
 * @param[out] caps Pointer to a PalFragmentShadingRateCapabilities to fill.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palQueryFragmentShadingRateCapabilities(
    PalDevice* device,
    PalFragmentShadingRateCapabilities* caps);

/**
 * @brief Get mesh shader feature capabilites or limits about a device.
 *
 * `PAL_ADAPTER_FEATURE_MESH_SHADER` must be supported and enabled when creating the
 * device. Otherwise behavior is undefined.
 *
 * @param[in] device Device to query mesh shader feature capabilities on.
 * @param[out] caps Pointer to a PalMeshShaderCapabilities to fill.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palQueryMeshShaderCapabilities(
    PalDevice* device,
    PalMeshShaderCapabilities* caps);

/**
 * @brief Get ray tracing feature capabilites or limits about a device.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled when creating the
 * device. Otherwise behavior is undefined.
 *
 * @param[in] device Device to query ray tracing feature capabilities on.
 * @param[out] caps Pointer to a PalRayTracingCapabilities to fill.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palQueryRayTracingCapabilities(
    PalDevice* device,
    PalRayTracingCapabilities* caps);

/**
 * @brief Get descriptor indexing feature capabilites or limits about a device.
 *
 * `PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING` must be supported and enabled when creating the
 * device. Otherwise behavior is undefined.
 *
 * @param[in] device Device to query descriptor indexing feature capabilities on.
 * @param[out] caps Pointer to a PalDescriptorIndexingCapabilities to fill.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palQueryDescriptorIndexingCapabilities(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps);

#endif // PAL_GRAPHICS_DEVICE_H