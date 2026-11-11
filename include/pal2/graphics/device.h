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

#ifndef PAL_GRAPHICS_SBT_H
#define PAL_GRAPHICS_SBT_H

#include "types.h"

/**
 * @defgroup descriptor_indexing_flags Descriptor Indexing Flags
 * @ingroup pal_graphics
 * @{
 */
#define PAL_DESCRIPTOR_INDEXING_FLAG_NONE 0 /**< No descriptor indexing flags. */
#define PAL_DESCRIPTOR_INDEXING_FLAG_UPDATE_AFTER_BIND (1U << 0) /**< Update after bind. */
#define PAL_DESCRIPTOR_INDEXING_FLAG_PARTIALLY_BOUND (1U << 1) /**< Unused descriptors can be left uninitialized. */
#define PAL_DESCRIPTOR_INDEXING_FLAG_NON_UNIFORM_INDEXING (1U << 2) /**< Non-uniform descriptor access. */
/** @} */

/**
 * @struct PalSamplerAnisotropyCapabilities
 * @brief Adapter sampler anisotropy capabilities.
 *
 * Filled in by @ref palQuerySamplerAnisotropyCapabilities().
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalSamplerAnisotropyCapabilities {
    uint32_t maxAnisotropy; /**< Maximum texture filtering level. */
} PalSamplerAnisotropyCapabilities;

/**
 * @struct PalMultiViewCapabilities
 * @brief Adapter multi view capabilities.
 *
 * Filled in by @ref palQueryMultiViewCapabilities().
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalMultiViewCapabilities {
    uint32_t maxViewCount; /**< Maximum number views of an image. */
} PalMultiViewCapabilities;

/**
 * @struct PalMultiViewportCapabilities
 * @brief Adapter multi viewport capabilities.
 *
 * Filled in by @ref palQueryMultiViewportCapabilities().
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalMultiViewportCapabilities {
    uint32_t maxCount; /**< Maximum number of simultaneous viewports. */
} PalMultiViewportCapabilities;

/**
 * @struct PalDepthStencilCapabilities
 * @brief Adapter depth stencil capabilities.
 *
 * Filled in by @ref palQueryDepthStencilCapabilities().
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDepthStencilCapabilities {
    uint32_t supportedDepthResolveModes;   /**< Mask of supported depth resolve modes. */
    uint32_t supportedStencilResolveModes; /**< Mask of supported stencil resolve modes. */
    PalBool supportsIndependentResolve; /**< Whether independent resolve modes are supported. */
    PalBool supportsIndependentResolveNone; /**< Whether independent none resolve mode is supported. */
} PalDepthStencilCapabilities;

/**
 * @struct PalFragmentShadingRateCapabilities
 * @brief Adapter fragment shading rate capabilities.
 *
 * Filled in by @ref palQueryFragmentShadingRateCapabilities().
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalFragmentShadingRateCapabilities {
    uint32_t supportedShadingRates; /**< Mask of supported shading rates. */
    uint32_t supportedCombinerOps;  /**< Mask of supported combiner operations. */
    uint32_t minTexelWidth;         /**< Minimum texel width in pixels. */
    uint32_t minTexelHeight;        /**< Minimum texel height in pixels. */
    uint32_t maxTexelWidth;         /**< Maximum texel width in pixels. */
    uint32_t maxTexelHeight;        /**< Maximum texel height in pixels. */
} PalFragmentShadingRateCapabilities;

/**
 * @struct PalMeshShaderCapabilities
 * @brief Adapter mesh shader capabilities.
 *
 * Filled in by @ref palQueryMeshShaderCapabilities().
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalMeshShaderCapabilities {
    uint32_t maxOutputPrimitives;         /**< Maximum number of primitives per mesh workgroup. */
    uint32_t maxOutputVertices;           /**< Maximum number of vertices per mesh workgroup. */
    uint32_t maxWorkGroupInvocations;     /**< Maximum mesh invocations across all workgroups. */
    uint32_t maxTaskWorkGroupInvocations; /**< Maximum task invocations across all workgroups. */
    uint32_t maxWorkGroupCount[3];        /**< Maximum mesh workgroups per dimension. */
    uint32_t maxTaskWorkGroupCount[3];    /**< Maximum task workgroups per dimension. */
} PalMeshShaderCapabilities;

/**
 * @struct PalRayTracingCapabilities
 * @brief Adapter ray tracing capabilities.
 *
 * Filled in by @ref palQueryRayTracingCapabilities().
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalRayTracingCapabilities {
    uint32_t maxRecursionDepth;      /**< Maximum ray recursion depth. */
    uint32_t maxHitAttributeSize;    /**< Maximum attributes size in bytes. */
    uint32_t maxInstanceCount;       /**< Maximum number of instances. */
    uint32_t maxPrimitiveCount;      /**< Maximum number of primitives. */
    uint32_t maxGeometryCount;       /**< Maximum number of geometries. */
    uint32_t maxPayloadSize;         /**< Maximum payload size in bytes. */
    uint32_t maxDispatchInvocations; /**< Maximum number of dispatch threads. */
} PalRayTracingCapabilities;

/**
 * @struct PalDescriptorIndexingCapabilities
 * @brief Adapter descriptor indexing capabilities.
 *
 * Filled in by @ref palQueryDescriptorIndexingCapabilities().
 *
 * `flags` shows the sub-features or capabilities of the descriptor indexing feature.
 * Each bit represents a supported operation. See below for more information:
 *
 * - PAL_DESCRIPTOR_INDEXING_FLAG_UPDATE_AFTER_BIND - Descriptors can be updated after
 *   the descriptor set has been bound.
 *
 * - PAL_DESCRIPTOR_INDEXING_FLAG_PARTIALLY_BOUND - Unused descriptors can be
 *   left uninitialized if shaders never access them.
 *
 * - PAL_DESCRIPTOR_INDEXING_FLAG_NON_UNIFORM_INDEXING - Different threads
 *   can access different descriptors.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorIndexingCapabilities {
    PalDescriptorIndexingFlags flags;   /**< Descriptor indexing flags. */
    uint32_t maxPerStageSampledImages;  /**< Maximum sampled images per shader stage. */
    uint32_t maxPerSetSampledImages;    /**< Maximum sampled images per descriptor set. */
    uint32_t maxPerStageStorageImages;  /**< Maximum storage images per shader stage. */
    uint32_t maxPerSetStorageImages;    /**< Maximum storage images per descriptor set. */
    uint32_t maxPerStageSamplers;       /**< Maximum samplers per shader stage. */
    uint32_t maxPerSetSamplers;         /**< Maximum samplers per descriptor set. */
    uint32_t maxPerStageStorageBuffers; /**< Maximum storage buffers per shader stage. */
    uint32_t maxPerSetStorageBuffers;   /**< Maximum storage buffers per descriptor set. */
    uint32_t maxPerStageUniformBuffers; /**< Maximum uniform buffers per shader stage. */
    uint32_t maxPerSetUniformBuffers;   /**< Maximum uniform buffers per descriptor set. */
    uint32_t maxPerStageAccelerationStructure; /**< Maximum acceleration structures per shader stage. */
    uint32_t maxPerSetAccelerationStructure; /**< Maximum acceleration structures per descriptor set. */
} PalDescriptorIndexingCapabilities;

/**
 * @struct PalSurfaceCapabilities
 * @brief Adapter surface capabilities.
 *
 * Filled in by @ref palGetSurfaceCapabilities().
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalSurfaceCapabilities {
    uint32_t supportedPresentModes;    /**< Mask of supported present modes. */
    uint32_t supportedCompositeAlphas; /**< Mask of supported composite alphas. */
    uint32_t supportedFormats;         /**< Mask of supported surface formats. */
    uint32_t minImageCount;            /**< Minimum image or back buffer count. */
    uint32_t maxImageCount;            /**< Maximum image or back buffer count. */
    uint32_t minImageWidth;            /**< Minimum image width in pixels. */
    uint32_t minImageHeight;           /**< Minimum image height in pixels. */
    uint32_t maxImageWidth;            /**< Maximum image width in pixels. */
    uint32_t maxImageHeight;           /**< Maximum image height in pixels. */
    uint32_t maxImageArrayLayers;      /**< Maximum number of image layers. */
} PalSurfaceCapabilities;

/**
 * @brief Creates a device.
 *
 * The requested features in `features` must all be supported by the adapter. Use
 * @ref palGetAdapterFeatures() to query the supported features before creating the device.
 * Enabling an unsupported feature causes device creation to fail.
 *
 * Enabling every feature the adapter supports is valid, but not recommended. It is
 * better to create the device with only the features you need.
 *
 * @param[in] adapter Adapter that creates the device.
 * @param[in] features Adapter features to enable.
 * @param[out] outDevice Output handle to receive the created device.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `adapter` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palDestroyDevice
 */
PAL_API PalResult PAL_CALL palCreateDevice(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice);

/**
 * @brief Destroys a device.
 *
 * All resources created with the device must be destroyed before this call.
 *
 * @param[in] device Pointer to the device to destroy.
 *
 * @thread-safety The adapter used to create `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateDevice
 */
PAL_API void PAL_CALL palDestroyDevice(PalDevice* device);

/**
 * @brief Gets the native device lost reason code.
 *
 * This function returns the backend-specific native code associated with the device-loss
 * reason. Backends that do not expose an explicit reason return their standard device-lost code.
 *
 * @param[in] device The device.
 * @return Backend-specific native device lost reason code.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API uint32_t PAL_CALL palGetDeviceLostReason(PalDevice* device);

/**
 * @brief Gets sampler anisotropy feature capabilities of a device.
 *
 * @ref PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY feature must be supported and enabled when
 * creating the device or this function results in undefined behavior.
 *
 * @param[in] device Device to get its sampler anisotropy capabilities.
 * @param[out] caps Output struct to receive the sampler anisotropy capabilities.
 *
 * @thread-safety `caps` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palQuerySamplerAnisotropyCapabilities(
    PalDevice* device,
    PalSamplerAnisotropyCapabilities* caps);

/**
 * @brief Gets multi view feature capabilities of a device.
 *
 * @ref PAL_ADAPTER_FEATURE_MULTI_VIEW feature must be supported and enabled when
 * creating the device or this function results in undefined behavior.
 *
 * @param[in] device Device to get its multi view capabilities.
 * @param[out] caps Output struct to receive the multi view capabilities.
 *
 * @thread-safety `caps` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palQueryMultiViewCapabilities(
    PalDevice* device,
    PalMultiViewCapabilities* caps);

/**
 * @brief Gets multi viewport feature capabilities of a device.
 *
 * @ref PAL_ADAPTER_FEATURE_MULTI_VIEWPORT feature must be supported and enabled when
 * creating the device or this function results in undefined behavior.
 *
 * @param[in] device Device to get its multi viewport capabilities.
 * @param[out] caps Output struct to receive the multi viewport capabilities.
 *
 * @thread-safety `caps` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palQueryMultiViewportCapabilities(
    PalDevice* device,
    PalMultiViewportCapabilities* caps);

/**
 * @brief Gets depth stencil feature capabilities of a device.
 *
 * @ref PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE feature must be supported and enabled when
 * creating the device or this function results in undefined behavior.
 *
 * @param[in] device Device to get its depth stencil capabilities.
 * @param[out] caps Output struct to receive the depth stencil capabilities.
 *
 * @thread-safety `caps` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palQueryDepthStencilCapabilities(
    PalDevice* device,
    PalDepthStencilCapabilities* caps);

/**
 * @brief Gets fragment (variable) shading rate feature capabilities of a device.
 *
 * @ref PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE feature must be supported and enabled when
 * creating the device or this function results in undefined behavior.
 *
 * @param[in] device Device to get its fragment shading rate capabilities.
 * @param[out] caps Output struct to receive the fragment shading rate capabilities.
 *
 * @thread-safety `caps` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palQueryFragmentShadingRateCapabilities(
    PalDevice* device,
    PalFragmentShadingRateCapabilities* caps);

/**
 * @brief Gets mesh shader feature capabilities of a device.
 *
 * @ref PAL_ADAPTER_FEATURE_MESH_SHADER feature must be supported and enabled when
 * creating the device or this function results in undefined behavior.
 *
 * @param[in] device Device to get its mesh shader capabilities.
 * @param[out] caps Output struct to receive the mesh shader capabilities.
 *
 * @thread-safety `caps` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palQueryMeshShaderCapabilities(
    PalDevice* device,
    PalMeshShaderCapabilities* caps);

/**
 * @brief Gets ray tracing feature capabilities of a device.
 *
 * @ref PAL_ADAPTER_FEATURE_RAY_TRACING feature must be supported and enabled when
 * creating the device or this function results in undefined behavior.
 *
 * @param[in] device Device to get its ray tracing capabilities.
 * @param[out] caps Output struct to receive the ray tracing capabilities.
 *
 * @thread-safety `caps` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palQueryRayTracingCapabilities(
    PalDevice* device,
    PalRayTracingCapabilities* caps);

/**
 * @brief Gets bindless/descriptor indexing feature capabilities of a device.
 *
 * @ref PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING feature must be supported and enabled when
 * creating the device or this function results in undefined behavior.
 *
 * @param[in] device Device to get its bindless/descriptor indexing capabilities.
 * @param[out] caps Output struct to receive the bindless/descriptor indexing capabilities.
 *
 * @thread-safety `caps` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palQueryDescriptorIndexingCapabilities(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps);

/**
 * @brief Check if a constant is supported in a mask.
 *
 * This function is used to check all masks in `supported_**` format in most of the capabilities
 * query structs.
 *
 * Example:
 *
 * To check if `PAL_PRESENT_MODE_IMMEDIATE` is supported after querying `PalSurfaceCapabilities`
 * capabilities of a surface, PalSurfaceCapabilities::supportedPresentModes should be the `mask`
 * parameter and `PAL_PRESENT_MODE_IMMEDIATE` as the value parameter.
 *
 * @param[in] mask The supported mask.
 * @param[in, out] value The value to check in the supported mask.
 *
 * @return `PAL_TRUE` on success otherwise `PAL_FALSE`.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
static inline PalBool PAL_CALL palIsSupported(
    uint32_t mask,
    uint32_t value)
{
    return (mask & (1U << value)) != 0;
}

#endif // PAL_GRAPHICS_SBT_H