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
 * @struct PalSurfaceCapabilities
 * @brief Contains surface capabilities of an adapter (GPU).
 *
 * @since Added in version 2.0
 * 
 * @var PalSurfaceCapabilities::supportedPresentModes
 * A bitmask of supported present modes. A specific present mode
 * should be check like this: @nl
 * palIsSupported(::supportedPresentModes, `PAL_PRESENT_MODE_FIFO`).
 * 
 * @var PalSurfaceCapabilities::supportedCompositeAlphas
 * A bitmask of supported composite alphas. A specific composite alpha
 * should be check like this: @nl
 * palIsSupported(::supportedCompositeAlphas, `PAL_COMPOSITE_ALPHA_OPAQUE`).
 * 
 * @var PalSurfaceCapabilities::supportedFormats
 * A bitmask of supported surface formats. A specific surface format
 * should be check like this: @nl
 * palIsSupported(::supportedFormats, `PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10`).
 * 
 * @var PalSurfaceCapabilities::minImageCount
 * The minimum number of image or back buffer count.
 * 
 * @var PalSurfaceCapabilities::maxImageCount
 * The maximum number of image or back buffer count.
 * 
 * @var PalSurfaceCapabilities::minImageWidth
 * The minimum width of the image.
 * 
 * @var PalSurfaceCapabilities::minImageHeight
 * The minimum height of the image.
 * 
 * @var PalSurfaceCapabilities::maxImageWidth
 * The maximum width of the image.
 * 
 * @var PalSurfaceCapabilities::maxImageHeight
 * The maximum height of the image.
 * 
 * @var PalSurfaceCapabilities::maxImageArrayLayers
 * The maximum array layers of the image.
 */
typedef struct PalSurfaceCapabilities
{
    uint32_t supportedPresentModes;
    uint32_t supportedCompositeAlphas;
    uint32_t supportedFormats;
    uint32_t minImageCount;
    uint32_t maxImageCount;
    uint32_t minImageWidth;
    uint32_t minImageHeight;
    uint32_t maxImageWidth;
    uint32_t maxImageHeight;
    uint32_t maxImageArrayLayers;
} PalSurfaceCapabilities;

#endif // PAL_GRAPHICS_DEVICE_H