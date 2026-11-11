/**
 * @file adapter.h
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

#ifndef PAL_GRAPHICS_ADAPTER_H
#define PAL_GRAPHICS_ADAPTER_H

#include "types.h"

/**
 * @brief Maximum size of the adapter name, including the NULL terminator.
 * @ingroup pal_graphics
 */
#define PAL_ADAPTER_NAME_SIZE 128

/**
 * @brief Maximum size of the backend name, including the NULL terminator.
 * @ingroup pal_graphics
 */
#define PAL_ADAPTER_BACKEND_NAME_SIZE 32

/**
 * @brief Encodes a shader target.
 * @ingroup pal_graphics
 */
#define PAL_MAKE_SHADER_TARGET(major, minor) ((uint32_t)((major) << 8) | (minor))

/**
 * @brief Gets the major part of a shader target.
 * @ingroup pal_graphics
 */
#define PAL_SHADER_TARGET_MAJOR(target) ((uint32_t)(target) >> 8)

/**
 * @brief Gets the minor part of a shader target.
 * @ingroup pal_graphics
 */
#define PAL_SHADER_TARGET_MINOR(target) ((uint32_t)(target) & 0xFF)

/**
 * @defgroup adapter_features Adapter Features
 * @ingroup pal_graphics
 * @{
 */
#define PAL_ADAPTER_FEATURE_NONE 0 /**< No adapter features. */
#define PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY (1ULL << 1) /**< Anisotropic texture filtering. */
#define PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING (1ULL << 2) /**< Per-sample shading. */
#define PAL_ADAPTER_FEATURE_MULTI_VIEWPORT (1ULL << 3) /**< Multiple viewports. */
#define PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE (1ULL << 4) /**< Timeline semaphores. */
#define PAL_ADAPTER_FEATURE_TESSELLATION_SHADER (1ULL << 5) /**< Tessellation shaders. */
#define PAL_ADAPTER_FEATURE_GEOMETRY_SHADER (1ULL << 6) /**< Geometry shaders. */
#define PAL_ADAPTER_FEATURE_SHADER_FLOAT16 (1ULL << 7) /**< 16-bit floats in shaders. */
#define PAL_ADAPTER_FEATURE_SHADER_FLOAT64 (1ULL << 8) /**< 64-bit floats in shaders. */
#define PAL_ADAPTER_FEATURE_SHADER_INT16 (1ULL << 9) /**< 16-bit integers in shaders. */
#define PAL_ADAPTER_FEATURE_SHADER_INT64 (1ULL << 10) /**< 64-bit integers in shaders. */
#define PAL_ADAPTER_FEATURE_RAY_TRACING (1ULL << 11) /**< Ray tracing pipelines. */
#define PAL_ADAPTER_FEATURE_INDIRECT_RAY_TRACING (1ULL << 12) /**< Indirect ray tracing dispatch. */
#define PAL_ADAPTER_FEATURE_MESH_SHADER (1ULL << 13) /**< Mesh and task shaders. */
#define PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE (1ULL << 14) /**< Variable shading rate. */
#define PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING (1ULL << 15) /**< Bindless/descriptor indexing. */
#define PAL_ADAPTER_FEATURE_SWAPCHAIN (1ULL << 16) /**< Presenting to surfaces. */
#define PAL_ADAPTER_FEATURE_MULTI_VIEW (1ULL << 17) /**< Rendering to multiple views in one pass. */
#define PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY (1ULL << 18) /**< Cube map array image views. */
#define PAL_ADAPTER_FEATURE_FENCE_RESET (1ULL << 19) /**< Resetting fences. */
#define PAL_ADAPTER_FEATURE_POLYGON_MODE_LINE (1ULL << 20) /**< Wireframe rendering. */
#define PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE (1ULL << 21) /**< Cull mode set at record time. */
#define PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE (1ULL << 22) /**< Front face set at record time. */
#define PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY (1ULL << 23) /**< Primitive topology set at record time. */
#define PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE (1ULL << 24) /**< Depth test toggle set at record time. */
#define PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE (1ULL << 25) /**< Depth write toggle set at record time. */
#define PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP (1ULL << 26) /**< Stencil ops set at record time. */
#define PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE (1ULL << 27) /**< Depth/stencil resolve. */
#define PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT (1ULL << 28) /**< Variable shading rate attachment. */
#define PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS (1ULL << 29) /**< Buffer device addresses. */
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW (1ULL << 30) /**< Indirect draws. */
#define PAL_ADAPTER_FEATURE_INDIRECT_DISPATCH (1ULL << 31) /**< Indirect compute dispatch. */
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT (1ULL << 32) /**< Indirect draws with a GPU-side count. */
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH (1ULL << 33) /**< Indirect mesh draws. */
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT (1ULL << 34) /**< Indirect mesh draws with a GPU-side count. */
#define PAL_ADAPTER_FEATURE_DISPATCH_BASE (1ULL << 35) /**< Compute dispatch with a base work group. */
#define PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS (1ULL << 36) /**< Empty descriptors. */
#define PAL_ADAPTER_FEATURE_RAY_QUERY (1ULL << 37) /**< Ray queries from non ray tracing shaders. */
/** @} */

/**
 * @defgroup adapter_types Adapter Types
 * @ingroup pal_graphics
 * @{
 */
#define PAL_ADAPTER_TYPE_UNKNOWN 0 /**< Unknown adapter type to PAL. */
#define PAL_ADAPTER_TYPE_DISCRETE 1 /**< Dedicated GPU. */
#define PAL_ADAPTER_TYPE_INTEGRATED 2 /**< Integrated GPU. */
#define PAL_ADAPTER_TYPE_VIRTUAL 3 /**< Virtualized GPU. */
#define PAL_ADAPTER_TYPE_CPU 4 /**< Software/CPU renderer. */
#define PAL_ADAPTER_TYPE_COUNT 5 /**< Number of adapter types. */
/** @} */

/**
 * @defgroup adapter_api_types Adapter API Types
 * @ingroup pal_graphics
 * @{
 */
#define PAL_ADAPTER_API_TYPE_UNKNOWN 0 /**< Unknown adapter API type to PAL. */
#define PAL_ADAPTER_API_TYPE_VULKAN 1 /**< Vulkan. */
#define PAL_ADAPTER_API_TYPE_D3D12 2 /**< Direct3D 12. */
#define PAL_ADAPTER_API_TYPE_METAL 3 /**< Metal. */
#define PAL_ADAPTER_API_TYPE_D3D11 4 /**< Direct3D 11. */
#define PAL_ADAPTER_API_TYPE_D3D9 5 /**< Direct3D 9. */
#define PAL_ADAPTER_API_TYPE_OPENGL 6 /**< OpenGL. */
#define PAL_ADAPTER_API_TYPE_OPENGLES 7 /**< OpenGL ES. */
#define PAL_ADAPTER_API_TYPE_WEBGPU 8 /**< WebGPU. */
#define PAL_ADAPTER_API_TYPE_COUNT 9 /**< Number of adapter API types. */
/** @} */

/**
 * @defgroup shader_formats Adapter Shader Formats
 * @ingroup pal_graphics
 * @{
 */
#define PAL_SHADER_FORMAT_UNKNOWN 0 /**< Unknown shader format to PAL. */
#define PAL_SHADER_FORMAT_SPIRV (1U << 0) /**< SPIR-V bytecode. */
#define PAL_SHADER_FORMAT_DXIL (1U << 1) /**< DXIL bytecode. */
#define PAL_SHADER_FORMAT_DXBC (1U << 2) /**< DXBC bytecode. */
#define PAL_SHADER_FORMAT_METALLIB (1U << 3) /**< Metal library (metallib) binary. */
#define PAL_SHADER_FORMAT_MSL (1U << 4) /**< Metal Shading Language (MSL) source. */
#define PAL_SHADER_FORMAT_GLSL (1U << 5) /**< GLSL source. */
#define PAL_SHADER_FORMAT_HLSL (1U << 6) /**< HLSL source. */
#define PAL_SHADER_FORMAT_WGSL (1U << 7) /**< WGSL source. */
/** @} */

/**
 * @struct PalAdapterInfo
 * @brief Adapter information.
 *
 * Filled in by @ref palGetAdapterInfo().
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalAdapterInfo {
    uint64_t vram;                                   /**< Total video memory in bytes. */
    uint64_t sharedMemory;                           /**< Total shared memory in bytes. */
    uint64_t driverVersion;                          /**< Driver version. */
    uint32_t vendorId;                               /**< Vendor identifier of the adapter. */
    uint32_t deviceId;                               /**< Device identifier of the adapter. */
    PalShaderFormats shaderFormats;                  /**< Bitmask of supported shader formats. */
    PalAdapterType type;                             /**< Adapter type. */
    PalAdapterApiType apiType;                       /**< Adapter API type. */
    char name[PAL_ADAPTER_NAME_SIZE];                /**< Adapter name (NULL-terminated). */
    char backendName[PAL_ADAPTER_BACKEND_NAME_SIZE]; /**< Adapter backend name (NULL-terminated). */
    PalGraphicsBackendVtableVersion vtableVersion;   /**< Backend vtable version. */
} PalAdapterInfo;

/**
 * @struct PalImageCapabilities
 * @brief Adapter image capabilities.
 *
 * Filled in by @ref palGetAdapterCapabilities() as part of @ref PalAdapterCapabilities.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalImageCapabilities {
    uint32_t maxWidth;       /**< Maximum width in pixels. */
    uint32_t maxHeight;      /**< Maximum height in pixels. */
    uint32_t maxDepth;       /**< Maximum depth in pixels. */
    uint32_t maxArrayLayers; /**< Maximum array layers. */
    uint32_t maxMipLevels;   /**< Maximum mipmap levels. */
} PalImageCapabilities;

/**
 * @struct PalResourceCapabilities
 * @brief Adapter resource capabilities.
 *
 * Filled in by @ref palGetAdapterCapabilities() as part of @ref PalAdapterCapabilities.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalResourceCapabilities {
    uint32_t maxPerStageSampledImages;         /**< Maximum sampled images per shader stage. */
    uint32_t maxPerSetSampledImages;           /**< Maximum sampled images per descriptor set. */
    uint32_t maxPerStageStorageImages;         /**< Maximum storage images per shader stage. */
    uint32_t maxPerSetStorageImages;           /**< Maximum storage images per descriptor set. */
    uint32_t maxPerStageSamplers;              /**< Maximum samplers per shader stage. */
    uint32_t maxPerSetSamplers;                /**< Maximum samplers per descriptor set. */
    uint32_t maxPerStageStorageBuffers;        /**< Maximum storage buffers per shader stage. */
    uint32_t maxPerSetStorageBuffers;          /**< Maximum storage buffers per descriptor set. */
    uint32_t maxPerStageUniformBuffers;        /**< Maximum uniform buffers per shader stage. */
    uint32_t maxPerSetUniformBuffers;          /**< Maximum uniform buffers per descriptor set. */
    uint32_t maxPerStageAccelerationStructure; /**< Maximum acceleration structures per shader stage. */
    uint32_t maxPerSetAccelerationStructure; /**< Maximum acceleration structures per descriptor set. */
    uint32_t maxBoundSets;                   /**< Maximum bound descriptor sets. */
} PalResourceCapabilities;

/**
 * @struct PalComputeCapabilities
 * @brief Adapter compute capabilities.
 *
 * Filled in by @ref palGetAdapterCapabilities() as part of @ref PalAdapterCapabilities.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalComputeCapabilities {
    uint32_t maxWorkGroupInvocations; /**< Maximum invocations across all workgroups. */
    uint32_t maxWorkGroupCount[3];    /**< Maximum workgroups per dimension. */
    uint32_t maxWorkGroupSize[3];     /**< Maximum workgroup size per dimension. */
} PalComputeCapabilities;

/**
 * @struct PalViewportCapabilities
 * @brief Adapter viewport capabilities.
 *
 * Filled in by @ref palGetAdapterCapabilities() as part of @ref PalAdapterCapabilities.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalViewportCapabilities {
    uint32_t maxWidth;    /**< Maximum width in pixels. */
    uint32_t maxHeight;   /**< Maximum height in pixels. */
    float minBoundsRange; /**< Minimum coordinate range. */
    float maxBoundsRange; /**< Maximum coordinate range. */
} PalViewportCapabilities;

/**
 * @struct PalAdapterCapabilities
 * @brief Adapter capabilities.
 *
 * Filled in by @ref palGetAdapterCapabilities().
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalAdapterCapabilities {
    uint32_t maxComputeQueues;            /**< Maximum compute queues that can be created. */
    uint32_t maxGraphicsQueues;           /**< Maximum graphics queues that can be created. */
    uint32_t maxCopyQueues;               /**< Maximum copy queues that can be created. */
    uint32_t maxColorAttachments;         /**< Maximum number of simultaneous color attachments. */
    uint32_t maxUniformBufferSize;        /**< Maximum uniform buffer size in bytes. */
    uint32_t maxStorageBufferSize;        /**< Maximum storage buffer size in bytes. */
    uint32_t maxPushConstantSize;         /**< Maximum push constants size in bytes. */
    uint32_t maxVertexLayouts;            /**< Maximum vertex layouts. */
    uint32_t maxVertexAttributes;         /**< Maximum vertex attributes across all vertex layouts. */
    uint32_t maxTessellationPatchPoint;   /**< Maximum tessellation patch point. */
    PalViewportCapabilities viewportCaps; /**< Viewport capabilities. */
    PalImageCapabilities imageCaps;       /**< Image capabilities. */
    PalResourceCapabilities resourceCaps; /**< Resource capabilities. */
    PalComputeCapabilities computeCaps;   /**< Compute capabilities. */
} PalAdapterCapabilities;

/**
 * @brief Enumerates available adapters.
 *
 * This function enumerates both PAL adapters and custom-backend adapters. This is a two-call function,
 * set `outAdapters` to `nullptr` and `count` to `0` to get the number of available adapters. Allocate the
 * array and call this function again with `outAdapters` set to the allocated array and `count` set to the
 * capacity of the array.
 *
 * If the specified count is less than the number of available adapters, only the adapters that fit in the
 * array will be written. The adapter handles are owned by PAL and must not be modified or freed.
 *
 * If two adapters have the same API type, both will be returned. Get the adapter information with
 * @ref palGetAdapterInfo() and check PalAdapterInfo::backendName to differentiate between the custom and
 * internal adapters. The backend name for the internal adapters is `PAL`.
 *
 * Call this function once and cache the result. Subsequent calls invalidate the previously returned adapter
 * handles. Only enumerate again if an adapter has been removed or added.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in, out] count Capacity of the adapter array.
 * @param[out] outAdapters Output array to receive the adapters, or `nullptr` to get the number of adapters.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety Must be called from the main thread.
 *
 * @result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT,
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palEnumerateAdapters(
    uint32_t* count,
    PalAdapter** outAdapters);

/**
 * @brief Gets information about an adapter.
 *
 * @param[in] adapter Adapter to get its information.
 * @param[out] info Output struct to receive the adapter information.
 *
 * @thread-safety `info` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palEnumerateAdapters
 */
PAL_API void PAL_CALL palGetAdapterInfo(
    PalAdapter* adapter,
    PalAdapterInfo* info);

/**
 * @brief Gets capabilities of an adapter.
 *
 * @param[in] adapter Adapter to get its capabilities.
 * @param[out] caps Output struct to receive the adapter capabilities.
 *
 * @thread-safety `caps` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palEnumerateAdapters
 */
PAL_API void PAL_CALL palGetAdapterCapabilities(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps);

/**
 * @brief Gets the supported features of an adapter.
 *
 * @param[in] adapter Adapter to get its features.
 * @return Adapter features on success or `0` on failure.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palEnumerateAdapters
 */
PAL_API PalAdapterFeatures PAL_CALL palGetAdapterFeatures(PalAdapter* adapter);

/**
 * @brief Gets the highest supported shader target for an adapter.
 *
 * This function gets the highest supported shader target per shader format
 * on the specified adapter. `shaderFormat` must be supported by the adapter
 * otherwise, the function returns `0`.
 *
 * The returned shader target is encoded with `PAL_MAKE_SHADER_TARGET` constant.
 * Use `PAL_SHADER_TARGET_MAJOR` and `PAL_SHADER_TARGET_MINOR` to get the major and
 * minor parts of the target.
 *
 * @param[in] adapter Adapter to query its highest shader target.
 * @param[in] shaderFormat Shader format. Must only have a single bit set.
 * @return Highest supported shader target, or `0` if the format is unsupported.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palEnumerateAdapters
 */
PAL_API uint32_t PAL_CALL palGetHighestSupportedShaderTarget(
    PalAdapter* adapter,
    PalShaderFormats shaderFormat);

#endif // PAL_GRAPHICS_ADAPTER_H