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

#ifndef GRAPHICS_ADAPTER_H
#define GRAPHICS_ADAPTER_H

#include "pal2/core/defines.h"
#include "pal2/core/result.h"

#define PAL_ADAPTER_NAME_SIZE 128
#define PAL_ADAPTER_BACKEND_NAME_SIZE 32

/**
 * @defgroup graphics_features Graphics Features
 * @brief Graphics features
 * 
 * @{
 */
#define PAL_ADAPTER_FEATURE_NONE 0
#define PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY (1ULL << 1)
#define PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING (1ULL << 2)
#define PAL_ADAPTER_FEATURE_MULTI_VIEWPORT (1ULL << 3)
#define PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE (1ULL << 4)
#define PAL_ADAPTER_FEATURE_TESSELLATION_SHADER (1ULL << 5)
#define PAL_ADAPTER_FEATURE_GEOMETRY_SHADER (1ULL << 6)
#define PAL_ADAPTER_FEATURE_SHADER_FLOAT16 (1ULL << 7)
#define PAL_ADAPTER_FEATURE_SHADER_FLOAT64 (1ULL << 8)
#define PAL_ADAPTER_FEATURE_SHADER_INT16 (1ULL << 9)
#define PAL_ADAPTER_FEATURE_SHADER_INT64 (1ULL << 10)
#define PAL_ADAPTER_FEATURE_RAY_TRACING (1ULL << 11)
#define PAL_ADAPTER_FEATURE_INDIRECT_RAY_TRACING (1ULL << 12)
#define PAL_ADAPTER_FEATURE_MESH_SHADER (1ULL << 13)
#define PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE (1ULL << 14)
#define PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING (1ULL << 15)
#define PAL_ADAPTER_FEATURE_SWAPCHAIN (1ULL << 16)
#define PAL_ADAPTER_FEATURE_MULTI_VIEW (1ULL << 17)
#define PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY (1ULL << 18)
#define PAL_ADAPTER_FEATURE_FENCE_RESET (1ULL << 19)
#define PAL_ADAPTER_FEATURE_POLYGON_MODE_LINE (1ULL << 20)
#define PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE (1ULL << 21)
#define PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE (1ULL << 22)
#define PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY (1ULL << 23)
#define PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE (1ULL << 24)
#define PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE (1ULL << 25)
#define PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP (1ULL << 26)
#define PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE (1ULL << 27)
#define PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT (1ULL << 28)
#define PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS (1ULL << 29)
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW (1ULL << 30)
#define PAL_ADAPTER_FEATURE_INDIRECT_DISPATCH (1ULL << 31)
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT (1ULL << 32)
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH (1ULL << 33)
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT (1ULL << 34)
#define PAL_ADAPTER_FEATURE_DISPATCH_BASE (1ULL << 35)
#define PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS (1ULL << 36)
#define PAL_ADAPTER_FEATURE_RAY_QUERY (1ULL << 37)
/** @} */

/**
 * @defgroup adapter_types Adapter Types
 * @brief Adapter types
 * 
 * @{
 */
#define PAL_ADAPTER_TYPE_UNKNOWN 0
#define PAL_ADAPTER_TYPE_DISCRETE 1
#define PAL_ADAPTER_TYPE_INTEGRATED 2
#define PAL_ADAPTER_TYPE_VIRTUAL 3
#define PAL_ADAPTER_TYPE_CPU 4
#define PAL_ADAPTER_TYPE_COUNT 5
/** @} */

/**
 * @defgroup adapter_api_types Adapter API Types
 * @brief Adapter API types
 * 
 * @{
 */
#define PAL_ADAPTER_API_TYPE_UNKNOWN 0
#define PAL_ADAPTER_API_TYPE_VULKAN 1
#define PAL_ADAPTER_API_TYPE_D3D12 2
#define PAL_ADAPTER_API_TYPE_METAL 3
#define PAL_ADAPTER_API_TYPE_D3D11 4
#define PAL_ADAPTER_API_TYPE_D3D9 5
#define PAL_ADAPTER_API_TYPE_OPENGL 6
#define PAL_ADAPTER_API_TYPE_OPENGLES 7
#define PAL_ADAPTER_API_TYPE_WEBGPU 8
#define PAL_ADAPTER_API_TYPE_COUNT 9
/** @} */

/**
 * @defgroup shader_formats Shader Formats
 * @brief Shader formats
 * 
 * @{
 */
#define PAL_SHADER_FORMAT_UNKNOWN 0
#define PAL_SHADER_FORMAT_SPIRV (1U << 0)
#define PAL_SHADER_FORMAT_DXIL (1U << 1)
#define PAL_SHADER_FORMAT_DXBC (1U << 2)
#define PAL_SHADER_FORMAT_METALLIB (1U << 3)
#define PAL_SHADER_FORMAT_MSL (1U << 4)
#define PAL_SHADER_FORMAT_GLSL (1U << 5)
#define PAL_SHADER_FORMAT_HLSL (1U << 6)
#define PAL_SHADER_FORMAT_WGSL (1U << 7)
/** @} */

/**
 * @defgroup graphics_backend_vtable Graphics Backend VTables
 * @brief Graphics backend vtables
 * 
 * @{
 */
#define PAL_GRAPHICS_BACKEND_VTABLE_VERSION_1 0
#define PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2 1
/** @} */

/**
 * @typedef PalAdapterFeatures
 * @brief Adapter features.
 * 
 * This is a bitmask of all supported features of an adapter (GPU).
 * 
 * All values of this type follow the format `PAL_ADAPTER_FEATURE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint64_t PalAdapterFeatures;

/**
 * @typedef PalAdapterType
 * @brief Adapter types.
 * 
 * All values of this type follow the format `PAL_ADAPTER_TYPE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalAdapterType;

/**
 * @typedef PalAdapterApiType
 * @brief Adapter API types.
 * 
 * All values of this type follow the format `PAL_ADAPTER_API_TYPE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalAdapterApiType;

/**
 * @typedef PalShaderFormats
 * @brief Shader formats.
 * 
 * This is a bitmask of all supported shader formats of an adapter.
 * 
 * All values of this type follow the format `PAL_SHADER_FORMAT_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalShaderFormats;

/**
 * @typedef PalGraphicsBackendVtableVersion
 * @brief Graphics backend vtable versions.
 * 
 * All values of this type follow the format 
 * `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_*` for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalGraphicsBackendVtableVersion;

/**
 * @struct PalAdapter
 * @brief Opaque handle to an adapter (GPU).
 *
 * @since Added in version 2.0
 */
typedef struct PalAdapter PalAdapter;

/**
 * @brief Contains information about an adapter (GPU).
 *
 * @since Added in version 2.0
 * 
 * @var PalAdapterInfo::vram
 * The total video memory in bytes of the adapter.
 * 
 * @var PalAdapterInfo::sharedMemory
 * The total shared memory in bytes of the adapter.
 * 
 * @var PalAdapterInfo::driverVersion
 * The driver version of the adapter. This format is driver specific.
 * 
 * @var PalAdapterInfo::vendorId
 * The vendor id of the adapter.
 * 
 * @var PalAdapterInfo::deviceId
 * The device id of the adapter.
 * 
 * @var PalAdapterInfo::shaderFormats
 * A bitmask of the supported shader formats of the adapter.
 * 
 * @var PalAdapterInfo::type
 * The type of the adapter (eg. `PAL_ADAPTER_TYPE_DISCRETE`).
 * 
 * @var PalAdapterInfo::apiType
 * The API type of the adapter (eg. `PAL_ADAPTER_API_TYPE_VULKAN`).
 * 
 * @var PalAdapterInfo::name
 * The name of the adapter.
 * 
 * @var PalAdapterInfo::backendName
 * The backend name of the adapter. This is the name of the backend that
 * the adapter belongs to.
 * 
 * @var PalAdapterInfo::vtableVersion
 * The backend version of the adapter. This is different from ::driverVersion.
 * This determines the available vtable implementations of the adapter.
 */
typedef struct PalAdapterInfo
{
    uint64_t vram;
    uint64_t sharedMemory;
    uint64_t driverVersion;
    uint32_t vendorId;
    uint32_t deviceId;
    PalShaderFormats shaderFormats;
    PalAdapterType type;
    PalAdapterApiType apiType;
    char name[PAL_ADAPTER_NAME_SIZE];
    char backendName[PAL_ADAPTER_BACKEND_NAME_SIZE];
    PalGraphicsBackendVtableVersion vtableVersion;
} PalAdapterInfo;

/**
 * @struct PalImageCapabilities
 * @brief Contains image capabilities of an adapter (GPU).
 *
 * @since Added in version 2.0
 * 
 * @var PalImageCapabilities::maxWidth
 * The maximum image width of the adapter in pixels.
 * 
 * @var PalImageCapabilities::maxHeight
 * The maximum image height of the adapter in pixels.
 * 
 * @var PalImageCapabilities::maxDepth
 * The maximum image depth of the adapter in pixels.
 * 
 * @var PalImageCapabilities::maxArrayLayers
 * The maximum image array layers of the adapter.
 * 
 * @var PalImageCapabilities::maxMipLevels
 * The maximum image mip levels of the adapter.
 */
typedef struct PalImageCapabilities
{
    uint32_t maxWidth;
    uint32_t maxHeight;
    uint32_t maxDepth;
    uint32_t maxArrayLayers;
    uint32_t maxMipLevels;
} PalImageCapabilities;

/**
 * @struct PalResourceCapabilities
 * @brief Contains resource capabilities of an adapter (GPU).
 *
 * @since Added in version 2.0
 * 
 * @var PalResourceCapabilities::maxPerStageSampledImages
 * The maximum sampled images per shader stage of the adapter.
 * 
 * @var PalResourceCapabilities::maxPerSetSampledImages
 * The maximum sampled images per descriptor set of the adapter.
 * 
 * @var PalResourceCapabilities::maxPerStageStorageImages
 * The maximum storage images per shader stage of the adapter.
 * 
 * @var PalResourceCapabilities::maxPerSetStorageImages
 * The maximum storage images per descriptor set of the adapter.
 * 
 * @var PalResourceCapabilities::maxPerStageSamplers
 * The maximum samplers per shader stage of the adapter.
 * 
 * @var PalResourceCapabilities::maxPerSetSamplers
 * The maximum samplers per descriptor set of the adapter.
 * 
 * @var PalResourceCapabilities::maxPerStageStorageBuffers
 * The maximum storage buffers per shader stage of the adapter.
 * 
 * @var PalResourceCapabilities::maxPerSetStorageBuffers
 * The maximum storage buffers per descriptor set of the adapter.
 * 
 * @var PalResourceCapabilities::maxPerStageUniformBuffers
 * The maximum uniform buffers per shader stage of the adapter.
 * 
 * @var PalResourceCapabilities::maxPerSetUniformBuffers
 * The maximum uniform buffers per descriptor set of the adapter.
 * 
 * @var PalResourceCapabilities::maxPerStageAccelerationStructure
 * The maximum acceleration structures per shader stage of the adapter.
 * 
 * @var PalResourceCapabilities::maxPerSetAccelerationStructure
 * The maximum acceleration structures per descriptor set of the adapter.
 * 
 * @var PalResourceCapabilities::maxBoundSets
 * The maximum number of descriptor sets of the adapter that can be bound
 * simultaneously.
 */
typedef struct PalResourceCapabilities
{
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
    uint32_t maxBoundSets;
} PalResourceCapabilities;

/**
 * @struct PalComputeCapabilities
 * @brief Contains compute capabilities of an adapter (GPU).
 *
 * @since Added in version 2.0
 * 
 * @var PalComputeCapabilities::maxWorkGroupInvocations
 * The maximum number of shader invocations inside a single workgroup
 * of the adapter.
 * 
 * @var PalComputeCapabilities::maxWorkGroupCount
 * The maximum number of workgroups per dimension of the adapter.
 * 
 * @var PalComputeCapabilities::maxWorkGroupSize
 * The maximum workgroup size per dimension of the adapter.
 */
typedef struct PalComputeCapabilities
{
    uint32_t maxWorkGroupInvocations;
    uint32_t maxWorkGroupCount[3];
    uint32_t maxWorkGroupSize[3];
} PalComputeCapabilities;

/**
 * @struct PalViewportCapabilities
 * @brief Contains viewport capabilities of an adapter (GPU).
 *
 * @since Added in version 2.0
 * 
 * @var PalViewportCapabilities::maxWidth
 * The maximum viewport width of the adapter in pixels.
 * 
 * @var PalViewportCapabilities::maxHeight
 * The maximum viewport height of the adapter in pixels.
 * 
 * @var PalViewportCapabilities::minBoundsRange
 * The minimum viewport bounds range of the adapter.
 * 
 * @var PalViewportCapabilities::maxBoundsRange
 * The maximum viewport bounds range of the adapter.
 */
typedef struct PalViewportCapabilities
{
    uint32_t maxWidth;
    uint32_t maxHeight;
    float minBoundsRange;
    float maxBoundsRange;
} PalViewportCapabilities;

/**
 * @struct PalAdapterCapabilities
 * @brief Contains capabilities of an adapter (GPU).
 *
 * @since Added in version 2.0
 * 
 * @var PalAdapterCapabilities::maxComputeQueues
 * The maximum number of compute queues of the adapter.
 * 
 * @var PalAdapterCapabilities::maxGraphicsQueues
 * The maximum number of graphics queues of the adapter.
 * 
 * @var PalAdapterCapabilities::maxCopyQueues
 * The maximum number of copy queues of the adapter.
 * 
 * @var PalAdapterCapabilities::maxColorAttachments
 * The maximum number of simultaneous color attachments (render targets)
 * of the adapter.
 * 
 * @var PalAdapterCapabilities::maxUniformBufferSize
 * The maximum uniform buffer size of the adapter in bytes.
 * 
 * @var PalAdapterCapabilities::maxStorageBufferSize
 * The maximum storage buffer size of the adapter in bytes.
 * 
 * @var PalAdapterCapabilities::maxPushConstantSize
 * The maximum push constants size of the adapter in bytes.
 * 
 * @var PalAdapterCapabilities::maxVertexLayouts
 * The maximum vertex layouts of the adapter.
 * 
 * @var PalAdapterCapabilities::maxVertexAttributes
 * The maximum vertex attributes across all vertex layouts of the adapter.
 * 
 * @var PalAdapterCapabilities::maxTessellationPatchPoint
 * The maximum tessellation patch point of the adapter.
 * 
 * @var PalAdapterCapabilities::viewportCaps
 * The viewport capabilities of the adapter.
 * 
 * @var PalAdapterCapabilities::imageCaps
 * The image capabilities of the adapter.
 * 
 * @var PalAdapterCapabilities::resourceCaps
 * The resource (descriptors) capabilities of the adapter.
 * 
 * @var PalAdapterCapabilities::computeCaps
 * The compute capabilities of the adapter.
 */
typedef struct PalAdapterCapabilities
{
    uint32_t maxComputeQueues;
    uint32_t maxGraphicsQueues;
    uint32_t maxCopyQueues;
    uint32_t maxColorAttachments;
    uint32_t maxUniformBufferSize;
    uint32_t maxStorageBufferSize;
    uint32_t maxPushConstantSize;
    uint32_t maxVertexLayouts;
    uint32_t maxVertexAttributes;
    uint32_t maxTessellationPatchPoint;
    PalViewportCapabilities viewportCaps;
    PalImageCapabilities imageCaps;
    PalResourceCapabilities resourceCaps;
    PalComputeCapabilities computeCaps;
} PalAdapterCapabilities;

#endif // GRAPHICS_ADAPTER_H