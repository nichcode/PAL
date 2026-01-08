
/**

Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>

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
 * @defgroup pal_graphics Graphics
 * Graphics PAL functionality such as Adapters, Device, Swapchains and more.
 *
 * @{
 */

#ifndef _PAL_GRAPHICS_H
#define _PAL_GRAPHICS_H

#include "pal_core.h"

#define PAL_ADAPTER_NAME_SIZE 128
#define PAL_ADAPTER_VERSION_SIZE 16
#define PAL_MAX_RESOLVE_MODES 8
#define PAL_MAX_COMBINER_OPS 8

typedef struct PalAdapter PalAdapter;
typedef struct PalDevice PalDevice;
typedef struct PalMemory PalMemory;
typedef struct PalQueue PalQueue;
typedef struct PalSwapchain PalSwapchain;

typedef struct PalImage PalImage;
typedef struct PalImageView PalImageView;
typedef struct PalShader PalShader;

typedef struct PalBuffer PalBuffer;
typedef struct PalFence PalFence;
typedef struct PalSemaphore PalSemaphore;
typedef struct PalCommandPool PalCommandPool;
typedef struct PalCommandBuffer PalCommandBuffer;

typedef struct PalPipeline PalPipeline;
typedef struct PalPipelineLayout PalPipelineLayout;
typedef struct PalAccelerationStructure PalAccelerationStructure;

typedef enum PalDebugMessageSeverity PalDebugMessageSeverity;
typedef enum PalDebugMessageType PalDebugMessageType;

typedef void(PAL_CALL* PalDebugCallback)(
    void* userData,
    PalDebugMessageSeverity severity,
    PalDebugMessageType type,
    const char* msg);

typedef enum {
    PAL_ADAPTER_TYPE_UNKNOWN,
    PAL_ADAPTER_TYPE_DISCRETE,
    PAL_ADAPTER_TYPE_INTEGRATED,
    PAL_ADAPTER_TYPE_VIRTUAL,
    PAL_ADAPTER_TYPE_CPU
} PalAdapterType;

typedef enum {
    PAL_ADAPTER_API_TYPE_VULKAN,
    PAL_ADAPTER_API_TYPE_D3D12,
    PAL_ADAPTER_API_TYPE_METAL,
    PAL_ADAPTER_API_TYPE_OPENGL,
    PAL_ADAPTER_API_TYPE_GLES,
    PAL_ADAPTER_API_TYPE_D3D11,
    PAL_ADAPTER_API_TYPE_D3D9,
    PAL_ADAPTER_API_TYPE_PPM
} PalAdapterApiType;

typedef enum {
    PAL_QUEUE_TYPE_GRAPHICS,
    PAL_QUEUE_TYPE_COMPUTE,
    PAL_QUEUE_TYPE_COPY
} PalQueueType;

typedef enum {
    PAL_PRESENT_MODE_FIFO,
    PAL_PRESENT_MODE_IMMEDIATE,
    PAL_PRESENT_MODE_MAILBOX,

    PAL_PRESENT_MODE_MAX
} PalPresentMode;

typedef enum {
    PAL_COMPOSITE_ALPHA_OPAQUE,
    PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED,
    PAL_COMPOSITE_ALPHA_POST_MULTIPLIED,

    PAL_COMPOSITE_ALPHA_MAX
} PalCompositeAplha;

typedef enum {
    PAL_FORMAT_UNDEFINED,

    PAL_FORMAT_R8_UNORM,
    PAL_FORMAT_R8_SNORM,
    PAL_FORMAT_R8_UINT,
    PAL_FORMAT_R8_SINT,
    PAL_FORMAT_R8_SRGB,
    PAL_FORMAT_R16_UNORM,
    PAL_FORMAT_R16_SNORM,
    PAL_FORMAT_R16_UINT,
    PAL_FORMAT_R16_SINT,
    PAL_FORMAT_R16_SFLOAT,
    PAL_FORMAT_R32_UINT,
    PAL_FORMAT_R32_SINT,
    PAL_FORMAT_R32_SFLOAT,
    PAL_FORMAT_R64_UINT,
    PAL_FORMAT_R64_SINT,
    PAL_FORMAT_R64_SFLOAT,
    PAL_FORMAT_R8G8_UNORM,
    PAL_FORMAT_R8G8_SNORM,
    PAL_FORMAT_R8G8_UINT,
    PAL_FORMAT_R8G8_SINT,
    PAL_FORMAT_R8G8_SRGB,
    PAL_FORMAT_R16G16_UNORM,
    PAL_FORMAT_R16G16_SNORM,
    PAL_FORMAT_R16G16_UINT,
    PAL_FORMAT_R16G16_SINT,
    PAL_FORMAT_R16G16_SFLOAT,
    PAL_FORMAT_R32G32_UINT,
    PAL_FORMAT_R32G32_SINT,
    PAL_FORMAT_R32G32_SFLOAT,
    PAL_FORMAT_R64G64_UINT,
    PAL_FORMAT_R64G64_SINT,
    PAL_FORMAT_R64G64_SFLOAT,
    PAL_FORMAT_R8G8B8_UNORM,
    PAL_FORMAT_R8G8B8_SNORM,
    PAL_FORMAT_R8G8B8_UINT,
    PAL_FORMAT_R8G8B8_SINT,
    PAL_FORMAT_R8G8B8_SRGB,
    PAL_FORMAT_R16G16B16_UNORM,
    PAL_FORMAT_R16G16B16_SNORM,
    PAL_FORMAT_R16G16B16_UINT,
    PAL_FORMAT_R16G16B16_SINT,
    PAL_FORMAT_R16G16B16_SFLOAT,
    PAL_FORMAT_R32G32B32_UINT,
    PAL_FORMAT_R32G32B32_SINT,
    PAL_FORMAT_R32G32B32_SFLOAT,
    PAL_FORMAT_R64G64B64_UINT,
    PAL_FORMAT_R64G64B64_SINT,
    PAL_FORMAT_R64G64B64_SFLOAT,
    PAL_FORMAT_B8G8R8_UNORM,
    PAL_FORMAT_B8G8R8_SNORM,
    PAL_FORMAT_B8G8R8_UINT,
    PAL_FORMAT_B8G8R8_SINT,
    PAL_FORMAT_B8G8R8_SRGB,
    PAL_FORMAT_R8G8B8A8_UNORM,
    PAL_FORMAT_R8G8B8A8_SNORM,
    PAL_FORMAT_R8G8B8A8_UINT,
    PAL_FORMAT_R8G8B8A8_SINT,
    PAL_FORMAT_R8G8B8A8_SRGB,
    PAL_FORMAT_R16G16B16A16_UNORM,
    PAL_FORMAT_R16G16B16A16_SNORM,
    PAL_FORMAT_R16G16B16A16_UINT,
    PAL_FORMAT_R16G16B16A16_SINT,
    PAL_FORMAT_R16G16B16A16_SFLOAT,
    PAL_FORMAT_R32G32B32A32_UINT,
    PAL_FORMAT_R32G32B32A32_SINT,
    PAL_FORMAT_R32G32B32A32_SFLOAT,
    PAL_FORMAT_R64G64B64A64_UINT,
    PAL_FORMAT_R64G64B64A64_SINT,
    PAL_FORMAT_R64G64B64A64_SFLOAT,
    PAL_FORMAT_B8G8R8A8_UNORM,
    PAL_FORMAT_B8G8R8A8_SNORM,
    PAL_FORMAT_B8G8R8A8_UINT,
    PAL_FORMAT_B8G8R8A8_SINT,
    PAL_FORMAT_B8G8R8A8_SRGB,
    PAL_FORMAT_S8_UINT,
    PAL_FORMAT_D16_UNORM,
    PAL_FORMAT_D32_SFLOAT,
    PAL_FORMAT_D32_SFLOAT_S8_UINT,
    PAL_FORMAT_D16_UNORM_S8_UINT,
    PAL_FORMAT_D24_UNORM_S8_UINT,

    PAL_FORMAT_MAX
} PalFormat;

typedef enum {
    PAL_IMAGE_USAGE_UNDEFINED = 0,
    PAL_IMAGE_USAGE_COLOR_ATTACHEMENT = PAL_BIT(0),
    PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT = PAL_BIT(1),
    PAL_IMAGE_USAGE_TRANSFER_SRC = PAL_BIT(2),
    PAL_IMAGE_USAGE_TRANSFER_DST = PAL_BIT(3),
    PAL_IMAGE_USAGE_STORAGE = PAL_BIT(4),
    PAL_IMAGE_USAGE_SAMPLED = PAL_BIT(5)
} PalImageUsages;

typedef enum {
    PAL_IMAGE_VIEW_USAGE_UNDEFINED = 0,
    PAL_IMAGE_VIEW_USAGE_COLOR = PAL_BIT(0),
    PAL_IMAGE_VIEW_USAGE_DEPTH = PAL_BIT(1),
    PAL_IMAGE_VIEW_USAGE_STENCIL = PAL_BIT(2),
    PAL_IMAGE_VIEW_USAGE_FRAGMENT_SHADING_RATE = PAL_BIT(3)
} PalImageViewUsages;

typedef enum {
    PAL_SHADER_FORMAT_SPIRV = PAL_BIT(0),
    PAL_SHADER_FORMAT_DXIL = PAL_BIT(1),
    PAL_SHADER_FORMAT_DXBC = PAL_BIT(2),
    PAL_SHADER_FORMAT_GLSL = PAL_BIT(3),
    PAL_SHADER_FORMAT_MSL = PAL_BIT(4),
    PAL_SHADER_FORMAT_PPM = PAL_BIT(5)
} PalShaderFormats;

typedef enum {
    PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY = PAL_BIT64(0),
    PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING = PAL_BIT64(1),
    PAL_ADAPTER_FEATURE_MULTI_VIEWPORT = PAL_BIT64(2),
    PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE = PAL_BIT64(3),
    PAL_ADAPTER_FEATURE_TESSELLATION_SHADER = PAL_BIT64(4),
    PAL_ADAPTER_FEATURE_GEOMETRY_SHADER = PAL_BIT64(5),
    PAL_ADAPTER_FEATURE_COMPUTE_SHADER = PAL_BIT64(6),
    PAL_ADAPTER_FEATURE_SHADER_FLOAT16 = PAL_BIT64(7),
    PAL_ADAPTER_FEATURE_SHADER_FLOAT64 = PAL_BIT64(8),
    PAL_ADAPTER_FEATURE_SHADER_INT16 = PAL_BIT64(9),
    PAL_ADAPTER_FEATURE_SHADER_INT64 = PAL_BIT64(10),
    PAL_ADAPTER_FEATURE_RAY_TRACING = PAL_BIT64(11),
    PAL_ADAPTER_FEATURE_MESH_SHADER = PAL_BIT64(12),
    PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE = PAL_BIT64(13),
    PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING = PAL_BIT64(14),
    PAL_ADAPTER_FEATURE_SWAPCHAIN = PAL_BIT64(15),
    PAL_ADAPTER_FEATURE_MULTI_VIEW = PAL_BIT64(16),
    PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY = PAL_BIT64(17),
    PAL_ADAPTER_FEATURE_FENCE_RESET = PAL_BIT64(18),
    PAL_ADAPTER_FEATURE_FENCE_TIMEOUT = PAL_BIT64(19),
    PAL_ADAPTER_FEATURE_POLYGON_MODE_LINE = PAL_BIT64(20),
    PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE = PAL_BIT64(21),
    PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE = PAL_BIT64(22),
    PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY = PAL_BIT64(23),
    PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE = PAL_BIT64(24),
    PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE = PAL_BIT64(25),
    PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP = PAL_BIT64(26),
    PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE = PAL_BIT64(27),
    PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT = PAL_BIT64(28),
    PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS = PAL_BIT64(29),
    PAL_ADAPTER_FEATURE_INDIRECT_DRAW = PAL_BIT64(30),
    PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT = PAL_BIT64(31),
    PAL_ADAPTER_FEATURE_DISPATCH_BASE = PAL_BIT64(32)
} PalAdapterFeatures;

typedef enum {
    PAL_LOAD_OP_LOAD,
    PAL_LOAD_OP_CLEAR,
    PAL_LOAD_OP_DONT_CARE,
} PalLoadOp;

typedef enum {
    PAL_STORE_OP_STORE,
    PAL_STORE_OP_DONT_CARE
} PalStoreOp;

typedef enum {
    PAL_MEMORY_TYPE_GPU_ONLY,
    PAL_MEMORY_TYPE_CPU_UPLOAD,
    PAL_MEMORY_TYPE_CPU_READBACK,
    PAL_MEMORY_TYPE_MAX
} PalMemoryType;

typedef enum {
    PAL_IMAGE_TYPE_1D,
    PAL_IMAGE_TYPE_2D,
    PAL_IMAGE_TYPE_3D
} PalImageType;

typedef enum {
    PAL_IMAGE_VIEW_TYPE_1D,
    PAL_IMAGE_VIEW_TYPE_1D_ARRAY,
    PAL_IMAGE_VIEW_TYPE_2D,
    PAL_IMAGE_VIEW_TYPE_2D_ARRAY,
    PAL_IMAGE_VIEW_TYPE_3D,
    PAL_IMAGE_VIEW_TYPE_CUBE,
    PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY,
} PalImageViewType;

typedef enum {
    PAL_SWAPCHAIN_FORMAT_BGRA8_UNORM_SRGB,
    PAL_SWAPCHAIN_FORMAT_BGRA8_SRGB_SRGB,
    PAL_SWAPCHAIN_FORMAT_RGBA8_UNORM_SRGB,
    PAL_SWAPCHAIN_FORMAT_RGBA16_FLOAT_HDR10,

    PAL_SWAPCHAIN_FORMAT_MAX
} PalSwapchainFormat;

typedef enum {
    PAL_SHADER_STAGE_UNDEFINED,
    PAL_SHADER_STAGE_VERTEX,
    PAL_SHADER_STAGE_FRAGMENT,
    PAL_SHADER_STAGE_COMPUTE,
    PAL_SHADER_STAGE_GEOMETRY,
    PAL_SHADER_STAGE_MESH,
    PAL_SHADER_STAGE_TASK,
    PAL_SHADER_STAGE_TESSELLATION_CONTROL,
    PAL_SHADER_STAGE_TESSELLATION_EVALUATION
} PalShaderStage;

typedef enum {
    PAL_SAMPLE_COUNT_1,
    PAL_SAMPLE_COUNT_2,
    PAL_SAMPLE_COUNT_4,
    PAL_SAMPLE_COUNT_8,
    PAL_SAMPLE_COUNT_16,
    PAL_SAMPLE_COUNT_32,
    PAL_SAMPLE_COUNT_64
} PalSampleCount;

typedef enum {
    PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
    PAL_PRIMITIVE_TOPOLOGY_LINE_LIST,
    PAL_PRIMITIVE_TOPOLOGY_LINE_STRIP,
    PAL_PRIMITIVE_TOPOLOGY_POINT_LIST,
    PAL_PRIMITIVE_TOPOLOGY_PATCH
} PalPrimitiveTopology;

typedef enum {
    PAL_CULL_MODE_NONE,
    PAL_CULL_MODE_FRONT,
    PAL_CULL_MODE_BACK
} PalCullMode;

typedef enum {
    PAL_FRONT_FACE_CLOCKWISE,
    PAL_FRONT_FACE_COUNTER_CLOCKWISE
} PalFrontFace;

typedef enum {
    PAL_POLYGON_MODE_FILL,
    PAL_POLYGON_MODE_LINE
} PalPolygonMode;

typedef enum {
    PAL_VERTEX_TYPE_UNDEFINED,

    PAL_VERTEX_TYPE_INT32,
    PAL_VERTEX_TYPE_INT32_2,
    PAL_VERTEX_TYPE_INT32_3,
    PAL_VERTEX_TYPE_INT32_4,

    PAL_VERTEX_TYPE_UINT32,
    PAL_VERTEX_TYPE_UINT32_2,
    PAL_VERTEX_TYPE_UINT32_3,
    PAL_VERTEX_TYPE_UINT32_4,

    PAL_VERTEX_TYPE_INT8_2,
    PAL_VERTEX_TYPE_INT8_4,
    PAL_VERTEX_TYPE_UINT8_2,
    PAL_VERTEX_TYPE_UINT8_4,

    PAL_VERTEX_TYPE_INT8_2NORM,
    PAL_VERTEX_TYPE_INT8_4NORM,
    PAL_VERTEX_TYPE_UINT8_2NORM,
    PAL_VERTEX_TYPE_UINT8_4NORM,

    PAL_VERTEX_TYPE_INT16_2,
    PAL_VERTEX_TYPE_INT16_4,
    PAL_VERTEX_TYPE_UINT16_2,
    PAL_VERTEX_TYPE_UINT16_4,

    PAL_VERTEX_TYPE_INT16_2NORM,
    PAL_VERTEX_TYPE_INT16_4NORM,
    PAL_VERTEX_TYPE_UINT16_2NORM,
    PAL_VERTEX_TYPE_UINT16_4NORM,

    PAL_VERTEX_TYPE_FLOAT,
    PAL_VERTEX_TYPE_FLOAT2,
    PAL_VERTEX_TYPE_FLOAT3,
    PAL_VERTEX_TYPE_FLOAT4,

    PAL_VERTEX_TYPE_HALF_FLOAT16_2,
    PAL_VERTEX_TYPE_HALF_FLOAT16_4
} PalVertexType;

typedef enum {
    PAL_COMMAND_BUFFER_TYPE_PRIMARY,
    PAL_COMMAND_BUFFER_TYPE_SECONDARY
} PalCommandBufferType;

typedef enum {
    PAL_VERTEX_LAYOUT_TYPE_PER_VERTEX,
    PAL_VERTEX_LAYOUT_TYPE_PER_INSTANCE
} PalVertexLayoutType;

typedef enum {
    PAL_COMPARE_OP_NEVER,
    PAL_COMPARE_OP_LESS,
    PAL_COMPARE_OP_EQUAL,
    PAL_COMPARE_OP_LESS_OR_EQUAL,
    PAL_COMPARE_OP_GREATER,
    PAL_COMPARE_OP_NOT_EQUAL,
    PAL_COMPARE_OP_GREATER_OR_EQUAL,
    PAL_COMPARE_OP_ALWAYS
} PalCompareOp;

typedef enum {
    PAL_STENCIL_OP_KEEP,
    PAL_STENCIL_OP_ZERO,
    PAL_STENCIL_OP_REPLACE,
    PAL_STENCIL_OP_INCREMENT_AND_CLAMP,
    PAL_STENCIL_OP_DECREMENT_AND_CLAMP,
    PAL_STENCIL_OP_INVERT,
    PAL_STENCIL_OP_INCREMENT_AND_WRAP,
    PAL_STENCIL_OP_DECREMENT_AND_WRAP
} PalStencilOp;

typedef enum {
    PAL_BLEND_OP_ADD,
    PAL_BLEND_OP_SUBTRACT,
    PAL_BLEND_OP_REVERSE_SUBTRACT,
    PAL_BLEND_OP_MIN,
    PAL_BLEND_OP_MAX
} PalBlendOp;

typedef enum {
    PAL_BLEND_FACTOR_ZERO,
    PAL_BLEND_FACTOR_ONE,
    PAL_BLEND_FACTOR_SRC_COLOR,
    PAL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
    PAL_BLEND_FACTOR_DST_COLOR,
    PAL_BLEND_FACTOR_ONE_MINUX_DST_COLOR,
    PAL_BLEND_FACTOR_SRC_ALPHA,
    PAL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    PAL_BLEND_FACTOR_DST_ALPHA,
    PAL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
    PAL_BLEND_FACTOR_CONSTANT_COLOR,
    PAL_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
    PAL_BLEND_FACTOR_CONSTANT_ALPHA,
    PAL_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA
} PalBlendFactor;

typedef enum {
    PAL_COLOR_MASK_NONE = 0,
    PAL_COLOR_MASK_RED = PAL_BIT(0),
    PAL_COLOR_MASK_GREEN = PAL_BIT(1),
    PAL_COLOR_MASK_BLUE = PAL_BIT(2),
    PAL_COLOR_MASK_ALPHA = PAL_BIT(3),
} PalColorMask;

typedef enum {
    PAL_RESOLVE_MODE_NONE = 0,
    PAL_RESOLVE_MODE_SAMPLE_ZERO,
    PAL_RESOLVE_MODE_AVERAGE,
    PAL_RESOLVE_MODE_MIN,
    PAL_RESOLVE_MODE_MAX
} PalResolveMode;

typedef enum {
    PAL_FRAGMENT_SHADING_RATE_1X1,
    PAL_FRAGMENT_SHADING_RATE_1X2,
    PAL_FRAGMENT_SHADING_RATE_2X1,
    PAL_FRAGMENT_SHADING_RATE_2X2,
    PAL_FRAGMENT_SHADING_RATE_2X4,
    PAL_FRAGMENT_SHADING_RATE_4X2,
    PAL_FRAGMENT_SHADING_RATE_4X4,

    PAL_FRAGMENT_SHADING_RATE_MAX
} PalFragmentShadingRate;

typedef enum {
    PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP,
    PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE,
    PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN,
    PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX,
    PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL
} PalFragmentShadingRateCombinerOp;

typedef enum {
    PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL,
    PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL
} PalAccelerationStructureType;

typedef enum {
    PAL_GEOMETRY_TYPE_TRIANGLE,
    PAL_GEOMETRY_TYPE_AABBS,
    PAL_GEOMETRY_TYPE_INSTANCE
} PalGeometryType;

typedef enum {
    PAL_INDEX_TYPE_UINT16,
    PAL_INDEX_TYPE_UINT32
} PalIndexType;

typedef enum {
    PAL_BUFFER_USAGE_VERTEX = PAL_BIT(0),
    PAL_BUFFER_USAGE_INDEX = PAL_BIT(1),
    PAL_BUFFER_USAGE_UNIFORM = PAL_BIT(2),
    PAL_BUFFER_USAGE_STORAGE = PAL_BIT(3),
    PAL_BUFFER_USAGE_TRANSFER_SRC = PAL_BIT(4),
    PAL_BUFFER_USAGE_TRANSFER_DST = PAL_BIT(5),
    PAL_BUFFER_USAGE_RAY_TRACING = PAL_BIT(6),
    PAL_BUFFER_USAGE_DEVICE_ADDRESS = PAL_BIT(7)
} PalBufferUsages;

enum PalDebugMessageSeverity {
    PAL_DEBUG_MESSAGE_SEVERITY_INFO,
    PAL_DEBUG_MESSAGE_SEVERITY_WARNING,
    PAL_DEBUG_MESSAGE_SEVERITY_ERROR
};

enum PalDebugMessageType {
    PAL_DEBUG_MESSAGE_TYPE_GENERAL,
    PAL_DEBUG_MESSAGE_TYPE_VALIDATION,
    PAL_DEBUG_MESSAGE_TYPE_PERFORMANCE
};

typedef enum {
    PAL_USAGE_STATE_UNDEFINED,
    PAL_USAGE_STATE_PRESENT,
    PAL_USAGE_STATE_COLOR_ATTACHMENT,
    PAL_USAGE_STATE_DEPTH_ATTACHMENT,
    PAL_USAGE_STATE_STENCIL_ATTACHMENT,
    PAL_USAGE_STATE_FRAGMENT_SHADING_RATE_ATTACHMENT,
    PAL_USAGE_STATE_TRANSFER_WRITE,
    PAL_USAGE_STATE_TRANSFER_READ,
    PAL_USAGE_STATE_VERTEX_READ,
    PAL_USAGE_STATE_INDEX_READ,
    PAL_USAGE_STATE_UNIFORM_READ,
    PAL_USAGE_STATE_SHADER_READ,
    PAL_USAGE_STATE_STORAGE_READ,
    PAL_USAGE_STATE_STORAGE_WRITE
} PalUsageState;

typedef struct {
    Uint32 vendorId;
    Uint32 deviceId;
    PalAdapterType type;
    PalAdapterApiType apiType;
    PalShaderFormats shaderFormats;
    Uint64 vram;
    Uint64 sharedMemory;
    Uint64 version;
    char versionString[PAL_ADAPTER_VERSION_SIZE];
    char name[PAL_ADAPTER_NAME_SIZE];
    char backendName[PAL_ADAPTER_NAME_SIZE];
} PalAdapterInfo;

typedef struct {
    bool debugLayer;
    Uint32 maxComputeQueues;
    Uint32 maxGraphicsQueues;
    Uint32 maxCopyQueues;
    Uint32 maxImageWidth;
    Uint32 maxImageHeight;
    Uint32 maxImageDepth;
    Uint32 maxImageArrayLayers;
    Uint32 maxImageMipLevels;
    PalSampleCount maxColorSampleCount;
    PalSampleCount maxDepthSampleCount;
    Uint32 maxColorAttachments;
    Uint32 maxMultiViews;
    Uint32 maxViewports;
    Uint32 maxSamplers;
    Uint32 maxUniformBufferSize;
    Uint32 maxStorageBufferSize;
    Uint32 maxPushConstantSize;
    Uint32 maxComputeWorkGroupInvocations;
    Uint32 maxComputeWorkGroupCount[3];
    Uint32 maxComputeWorkGroupSize[3];
} PalAdapterCapabilities;

typedef struct {
    bool independentDepthStencilResolve;
    bool depthResolveModes[PAL_MAX_RESOLVE_MODES];
    bool stencilResolveModes[PAL_MAX_RESOLVE_MODES];
} PalDepthStencilCapabilities;

typedef struct {
    bool shadingRates[PAL_FRAGMENT_SHADING_RATE_MAX];
    Uint32 minTexelWidth;
    Uint32 minTexelHeight;
    Uint32 maxTexelWidth;
    Uint32 maxTexelHeight;
    bool combinerOps[PAL_MAX_COMBINER_OPS];
} PalFragmentShadingRateCapabilities;

typedef struct {
    Uint32 maxMeshOutputPrimitives;
    Uint32 maxMeshOutputVertices;
    Uint32 maxTaskWorkGroupInvocations;
    Uint32 maxMeshWorkGroupInvocations;
    Uint32 maxTaskWorkGroupCount[3];
    Uint32 maxMeshWorkGroupCount[3];
} PalMeshShaderCapabilities;

typedef struct {
    Uint32 maxRecursionDepth;
    Uint32 maxHitAttributeSize;
    Uint32 maxInstanceCount;
    Uint32 maxPrimitiveCount;
    Uint32 maxGeometryCount;
    Uint32 maxPayloadSize;
    Uint32 maxDispatchInvocations;
    Uint32 maxShaderGroupStride;
    Uint32 shaderGroupHandleSize;
    Uint32 shaderGroupHandleAlignment;
    Uint32 shaderGroupBaseAlignment;
} PalRayTracingCapabilities;

typedef struct {
    bool presentModes[PAL_PRESENT_MODE_MAX];
    bool compositeAlphas[PAL_COMPOSITE_ALPHA_MAX];
    bool formats[PAL_SWAPCHAIN_FORMAT_MAX];
    Uint32 minImageCount;
    Uint32 maxImageCount;
    Uint32 minImageWidth;
    Uint32 minImageHeight;
    Uint32 maxImageWidth;
    Uint32 maxImageHeight;
    Uint32 maxImageArrayLayers;
} PalSwapchainCapabilities;

typedef struct {
    void* display;
    void* window;
} PalGraphicsWindow;

typedef struct {
    PalFormat format;
    PalImageUsages usages;
    PalImageViewUsages viewUsages;
} PalFormatInfo;

typedef struct {
    Uint32 width;
    Uint32 height;
    Uint32 depthOrArraySize;
    Uint32 mipLevelCount;
    PalSampleCount sampleCount;
    PalImageType type;
    PalFormat format;
    PalImageUsages usages;
} PalImageInfo;

typedef struct {
    float color[4];
    float depth;
    Uint32 stencil;
} PalClearValue;

typedef struct {
    PalLoadOp loadOp;
    PalStoreOp storeOp;
    PalResolveMode resolveMode;
    Uint32 texelWidth;
    Uint32 texelHeight;
    PalImageView* imageView;
    PalImageView* resolveImageView;
    PalClearValue clearValue;
} PalAttachmentDesc;

typedef struct {
    float x;
    float y;
    float width;
    float height;
    float minDepth;
    float maxDepth;
} PalViewport;

typedef struct {
    Int32 x;
    Int32 y;
    Uint32 width;
    Uint32 height;
} PalRect2D;

typedef struct {
    bool memoryTypes[PAL_MEMORY_TYPE_MAX];
    Uint64 size;
    Uint32 alignment;
} PalMemoryRequirements;

typedef struct {
    Uint64 waitValue;
    Uint64 signalValue;
    PalCommandBuffer* cmdBuffer;
    PalSemaphore* waitSemaphore;
    PalSemaphore* signalSemaphore;
    PalFence* fence;
} PalCommandBufferSubmitInfo;

typedef struct {
    Uint64 timeout;
    Uint64 signalValue;
    PalSemaphore* signalSemaphore;
    PalFence* fence;
} PalSwapchainNextImageInfo;

typedef struct {
    Uint32 imageIndex;
    Uint64 waitValue;
    PalSemaphore* waitSemaphore;
} PalSwapchainPresentInfo;

typedef struct {
    Uint32 viewCount;
    Uint32 layerCount;
    Uint32 colorAttachentCount;
    PalSampleCount multisampleCount;
    PalAttachmentDesc* colorAttachments;
    PalAttachmentDesc* depthAttachment;
    PalAttachmentDesc* stencilAttachment;
    PalAttachmentDesc* fragmentShadingRateAttachment;
    PalRect2D renderArea;
} PalRenderingInfo;

typedef struct {
    Uint32 viewCount;
    Uint32 colorAttachentCount;
    PalSampleCount multisampleCount;
    PalFormat depthAttachmentFormat;
    PalFormat stencilAttachmentFormat;
    PalFormat fragmentShadingRateAttachmentFormat;
    PalFormat* colorAttachmentsFormat;
} PalRenderingLayoutInfo;

typedef struct {
    Uint32 workCount[3];
    Uint32 workGroupSize[3];
    Uint32 workGroupCount[3];
} PalWorkGroupBuildData;

typedef struct {
    Uint32 workGroupBase[3];
    Uint32 workGroupCount[3];
} PalWorkGroupInfo;

typedef struct {
    Uint32 vertexCount;
    Uint32 instanceCount;
    Uint32 firstVertex;
    Uint32 firstInstance;
} PalDrawData;

typedef struct {
    Uint32 indexCount;
    Uint32 instanceCount;
    Uint32 firstIndex;
    Int32 vertexOffset;
    Uint32 firstInstance;
} PalDrawIndexedData;

typedef struct {
    PalVertexType type;
    Uint32 location;
} PalVertexAttribute;

typedef struct {
    PalVertexLayoutType type;
    Uint32 binding;
    Uint32 attributeCount;
    PalVertexAttribute* attributes;
} PalVertexLayout;

typedef struct {
    void* userData;
    PalDebugCallback callback;
} PalGraphicsDebugger;

typedef struct {
    bool enableDepthClamp;
    bool enableDepthBias;
    PalPolygonMode polygonMode;
    PalCullMode cullMode;
    PalFrontFace frontFace;
} PalRasterizerState;

typedef struct {
    bool enableSampleShading;
    bool enableAlphaToCoverage;
    PalSampleCount sampleCount;
    Uint64 sampleMask;
    float minSampleShading;
} PalMultisampleState;

typedef struct {
    PalStencilOp failOp;
    PalStencilOp passOp;
    PalStencilOp depthFailOp;
    PalCompareOp compareOp;
} PalStencilOpState;

typedef struct {
    bool enableDepthTest;
    bool enableDepthWrite;
    bool enableStencilTest;
    PalCompareOp compareOp;
    PalStencilOpState frontStencilOpState;
    PalStencilOpState backStencilOpState;
} PalDepthStencilState;

typedef struct {
    bool enableBlend;
    PalColorMask colorWriteMask;
    PalBlendFactor srcColorBlendFactor;
    PalBlendFactor dstColorBlendFactor;
    PalBlendOp colorBlendOp;
    PalBlendFactor srcAlphaBlendFactor;
    PalBlendFactor dstAlphaBlendFactor;
    PalBlendOp alphaBlendOp;
} PalBlendAttachment;

typedef struct {
    PalFragmentShadingRate rate;
    PalFragmentShadingRateCombinerOp combinerOps[2];
} PalFragmentShadingRateState;

typedef struct {
    Uint32 instanceId;
    Uint32 mask;
    PalAccelerationStructure* blas;
    float transform[12]; // row major (3x4)
} PalAccelerationStructureInstance;

typedef struct {
    Uint32 accelerationStructureSize;
    Uint32 scratchBufferSize;
} PalAccelerationStructureBuildSize;

typedef struct {
    PalVertexType vertexType;
    PalIndexType indexType;
    Uint32 vertexStride;
    Uint32 indexCount;
    Uint32 vertexCount;
    Uint64 vertexOffset;
    Uint64 indexOffset;
    PalBuffer* vertexBuffer;
    PalBuffer* indexBuffer;
} PalGeometryDataTriangle;

typedef struct {
    Uint32 stride;
    Uint64 offset;
    PalBuffer* buffer;
} PalGeometryDataAABBS;

typedef struct {
    Uint64 offset;
    PalBuffer* buffer;
} PalGeometryDataInstance;

typedef struct {
    Uint32 primitiveCount;
    PalGeometryType type;
    void* data; // based on type
} PalGeometry;

typedef struct {
    PalAccelerationStructureType type;
    Uint32 geometryCount;
    Uint64 scratchBufferOffset;
    PalAccelerationStructure* dst;
    PalBuffer* scratchBuffer;
    PalGeometry* geometries;
} PalAccelerationStructureBuildInfo;

typedef struct {
    Uint32 width;
    Uint32 height;
    Uint32 depthOrArraySize;
    Uint32 mipLevelCount;
    PalSampleCount sampleCount;
    PalImageType type;
    PalFormat format;
    PalImageUsages usages;
} PalImageCreateInfo;

typedef struct {
    Uint32 startMipLevel;
    Uint32 mipLevelCount;
    Uint32 startArrayLayer;
    Uint32 layerArrayCount;
    PalImageViewType type;
    PalImageViewUsages usages;
} PalImageViewCreateInfo;

typedef struct {
    bool clipped;
    Uint32 width;
    Uint32 height;
    Uint32 imageCount;
    Uint32 imageArrayLayerCount;
    PalPresentMode presentMode;
    PalCompositeAplha compositeAlpha;
    PalSwapchainFormat format;
} PalSwapchainCreateInfo;

typedef struct {
    Uint32 patchControlPoints;
    PalShaderStage stage;
    void* bytecode;
    Uint64 bytecodeSize;
} PalShaderCreateInfo;

typedef struct {
    PalBufferUsages usages;
    Uint64 size;
} PalBufferCreateInfo;

typedef struct {
    PalAccelerationStructureType type;
    PalBuffer* buffer;
    Uint64 offset;
    Uint64 size;
} PalAccelerationStructureCreateInfo;

typedef struct {
    bool unused;
} PalPipelineLayoutCreateInfo;

typedef struct {
    Uint32 vertexLayoutCount;
    Uint32 blendAttachmentCount;
    Uint32 shaderCount;
    PalPrimitiveTopology topology;
    PalPipelineLayout* pipelineLayout;
    PalShader** shaders;
    PalVertexLayout* vertexLayouts;
    PalBlendAttachment* blendAttachments;
    PalRasterizerState* rasterizerState;
    PalMultisampleState* multisampleState;
    PalDepthStencilState* depthStencilState;
    PalFragmentShadingRateState* fragmentShadingRateState;
    PalRenderingLayoutInfo* renderingLayout;
} PalGraphicsPipelineCreateInfo;

typedef struct {
    PalResult PAL_CALL (*enumerateAdapters)(
        Int32* count,
        PalAdapter** outAdapters);

    PalResult PAL_CALL (*getAdapterInfo)(
        PalAdapter* adapter,
        PalAdapterInfo* info);

    PalResult PAL_CALL (*getAdapterCapabilities)(
        PalAdapter* adapter,
        PalAdapterCapabilities* caps);

    PalAdapterFeatures PAL_CALL (*getAdapterFeatures)(PalAdapter* adapter);

    PalResult PAL_CALL (*createDevice)(
        PalAdapter* adapter,
        PalAdapterFeatures features,
        PalDevice** outDevice);

    void PAL_CALL (*destroyDevice)(PalDevice* device);

    PalResult PAL_CALL (*waitDevice)(PalDevice* device);

    PalResult PAL_CALL (*allocateMemory)(
        PalDevice* device,
        PalMemoryType type,
        Uint64 size,
        PalMemory** outMemory);

    void PAL_CALL (*freeMemory)(
        PalDevice* device,
        PalMemory* memory);

    PalResult PAL_CALL (*mapMemory)(
        PalDevice* device,
        PalMemory* memory,
        Uint64 offset,
        Uint64 size,
        void** outPtr);

    void PAL_CALL (*unmapMemory)(
        PalDevice* device,
        PalMemory* memory);

    PalResult PAL_CALL (*queryDepthStencilCapabilities)(
        PalDevice* device,
        PalDepthStencilCapabilities* caps);

    PalResult PAL_CALL (*queryFragmentShadingRateCapabilities)(
        PalDevice* device,
        PalFragmentShadingRateCapabilities* caps);

    PalResult PAL_CALL (*queryMeshShaderCapabilities)(
        PalDevice* device,
        PalMeshShaderCapabilities* caps);

    PalResult PAL_CALL (*queryRayTracingCapabilities)(
        PalDevice* device,
        PalRayTracingCapabilities* caps);

    PalResult PAL_CALL (*createQueue)(
        PalDevice* device,
        PalQueueType type,
        PalQueue** outQueue);

    void PAL_CALL (*destroyQueue)(PalQueue* queue);

    bool PAL_CALL (*canQueuePresent)(
        PalQueue* queue,
        PalGraphicsWindow* window);

    PalResult PAL_CALL (*waitQueue)(PalQueue* queue);

    PalResult PAL_CALL (*enumerateFormats)(
        PalAdapter* adapter,
        Int32* count,
        PalFormatInfo* outFormats);

    bool PAL_CALL (*isFormatSupported)(
        PalAdapter* adapter,
        PalFormat format);

    PalImageUsages PAL_CALL (*queryFormatImageUsages)(
        PalAdapter* adapter,
        PalFormat format);

    PalImageViewUsages PAL_CALL (*queryFormatImageViewUsages)(
        PalAdapter* adapter,
        PalFormat format);

    PalResult PAL_CALL (*createImage)(
        PalDevice* device,
        const PalImageCreateInfo* info,
        PalImage** outImage);

    void PAL_CALL (*destroyImage)(PalImage* image);

    PalResult PAL_CALL (*getImageInfo)(
        PalImage* image,
        PalImageInfo* info);

    PalResult PAL_CALL (*getImageMemoryRequirements)(
        PalImage* image,
        PalMemoryRequirements* requirements);

    PalResult PAL_CALL (*bindImageMemory)(
        PalImage* image,
        PalMemory* memory,
        Uint64 offset);

    PalResult PAL_CALL (*createImageView)(
        PalDevice* device,
        PalImage* image,
        const PalImageViewCreateInfo* info,
        PalImageView** outImageView);

    void PAL_CALL (*destroyImageView)(PalImageView* imageView);

    PalResult PAL_CALL (*querySwapchainCapabilities)(
        PalDevice* device,
        PalGraphicsWindow* window,
        PalSwapchainCapabilities* caps);

    PalResult PAL_CALL (*createSwapchain)(
        PalDevice* device,
        PalQueue* queue,
        PalGraphicsWindow* window,
        const PalSwapchainCreateInfo* info,
        PalSwapchain** outSwapchain);

    void PAL_CALL (*destroySwapchain)(PalSwapchain* swapchain);

    PalImage* PAL_CALL (*getSwapchainImage)(
        PalSwapchain* swapchain,
        Int32 index);

    PalResult PAL_CALL (*getNextSwapchainImage)(
        PalSwapchain* swapchain,
        PalSwapchainNextImageInfo* info,
        Uint32 *outIndex);

    PalResult PAL_CALL (*presentSwapchain)(
        PalSwapchain* swapchain,
        PalSwapchainPresentInfo* info);

    PalResult PAL_CALL (*createShader)(
        PalDevice* device,
        const PalShaderCreateInfo* info,
        PalShader** outShader);

    void PAL_CALL (*destroyShader)(PalShader* shader);

    PalResult PAL_CALL (*createFence)(
        PalDevice* device,
        bool signaled,
        PalFence** outFence);

    void PAL_CALL (*destroyFence)(PalFence* fence);

    PalResult PAL_CALL (*waitFenceTimeout)(
        PalFence* fence,
        Uint64 timeout);

    PalResult PAL_CALL (*resetFence)(PalFence* fence);

    bool PAL_CALL (*isFenceSignaled)(PalFence* fence);

    PalResult PAL_CALL (*createSemaphore)(
        PalDevice* device,
        PalSemaphore** outSemaphore);

    void PAL_CALL (*destroySemaphore)(PalSemaphore* semaphore);

    PalResult PAL_CALL (*waitSemaphore)(
        PalSemaphore* semaphore,
        PalQueue* queue,
        Uint64 value,
        Uint64 timeout);

    PalResult PAL_CALL (*signalSemaphore)(
        PalSemaphore* semaphore,
        PalQueue* queue,
        Uint64 value);

    PalResult PAL_CALL (*getSemaphoreValue)(
        PalSemaphore* semaphore,
        Uint64* value);

    PalResult PAL_CALL (*createCommandPool)(
        PalDevice* device,
        PalQueue* queue,
        PalCommandPool** outPool);

    void PAL_CALL (*destroyCommandPool)(PalCommandPool* pool);

    PalResult PAL_CALL (*resetCommandPool)(PalCommandPool* pool);

    PalResult PAL_CALL (*createCommandBuffer)(
        PalDevice* device,
        PalCommandPool* pool,
        PalCommandBufferType type,
        PalCommandBuffer** outCmdBuffer);

    void PAL_CALL (*destroyCommandBuffer)(PalCommandBuffer* cmdBuffer);

    PalResult PAL_CALL (*beginCommandBuffer)(
        PalCommandBuffer* cmdBuffer,
        PalRenderingLayoutInfo* info);

    PalResult PAL_CALL (*endCommandBuffer)(PalCommandBuffer* cmdBuffer);

    PalResult PAL_CALL (*resetCommandBuffer)(PalCommandBuffer* cmdBuffer);

    PalResult PAL_CALL (*executeCommandBuffer)(
        PalCommandBuffer* primaryCmdBuffer,
        PalCommandBuffer* secondaryCmdBuffer);

    PalResult PAL_CALL (*setFragmentShadingRate)(
        PalCommandBuffer* cmdBuffer,
        PalFragmentShadingRateState* state);

    PalResult PAL_CALL (*drawMeshTasks)(
        PalCommandBuffer* cmdBuffer,
        Uint32 groupCountX,
        Uint32 groupCountY,
        Uint32 groupCountZ);

    PalResult PAL_CALL (*drawMeshTasksIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        Uint64 offset,
        Uint32 drawCount,
        Uint32 stride);

    PalResult PAL_CALL (*drawMeshTasksIndirectCount)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalBuffer* countBuffer,
        Uint64 offset,
        Uint64 countBufferOffset,
        Uint32 maxDrawCount,
        Uint32 stride);

    PalResult PAL_CALL (*buildAccelerationStructure)(
        PalCommandBuffer* cmdBuffer,
        PalAccelerationStructureBuildInfo* info);

    PalResult PAL_CALL (*beginRendering)(
        PalCommandBuffer* cmdBuffer,
        PalRenderingInfo* info);

    PalResult PAL_CALL (*endRendering)(PalCommandBuffer* cmdBuffer);

    PalResult PAL_CALL (*copyBuffer)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* dst,
        PalBuffer* src,
        Uint64 dstOffset,
        Uint64 srcOffset,
        Uint32 size);

    PalResult PAL_CALL (*bindPipeline)(
        PalCommandBuffer* cmdBuffer,
        PalPipeline* pipeline);

    PalResult PAL_CALL (*setViewport)(
        PalCommandBuffer* cmdBuffer,
        Uint32 count,
        PalViewport* viewports);

    PalResult PAL_CALL (*setScissors)(
        PalCommandBuffer* cmdBuffer,
        Uint32 count,
        PalRect2D* scissors);

    PalResult PAL_CALL (*bindVertexBuffers)(
        PalCommandBuffer* cmdBuffer,
        Uint32 firstSlot,
        Uint32 count,
        PalBuffer** buffers,
        Uint64* offsets);

    PalResult PAL_CALL (*bindIndexBuffer)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        Uint64 offset,
        PalIndexType type);

    PalResult PAL_CALL (*draw)(
        PalCommandBuffer* cmdBuffer,
        PalDrawData* data);

    PalResult PAL_CALL (*drawIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        Uint64 offset,
        Uint32 count);

    PalResult PAL_CALL (*drawIndirectCount)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalBuffer* countBuffer,
        Uint64 offset,
        Uint64 countBufferOffset,
        Uint32 count);

    PalResult PAL_CALL (*drawIndexed)(
        PalCommandBuffer* cmdBuffer,
        PalDrawIndexedData* data);

    PalResult PAL_CALL (*drawIndexedIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        Uint64 offset,
        Uint32 count);

    PalResult PAL_CALL (*drawIndexedIndirectCount)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalBuffer* countBuffer,
        Uint64 offset,
        Uint64 countBufferOffset,
        Uint32 count);

    PalResult PAL_CALL (*imageViewBarrier)(
        PalCommandBuffer* cmdBuffer,
        PalImageView* imageView,
        PalUsageState oldUsageState,
        PalUsageState newUsageState);

    PalResult PAL_CALL (*bufferBarrier)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalUsageState oldUsageState,
        PalUsageState newUsageState);

    PalResult PAL_CALL (*dispatch)(
        PalCommandBuffer* cmdBuffer,
        Uint32 groupCountX,
        Uint32 groupCountY,
        Uint32 groupCountZ);

    PalResult PAL_CALL (*dispatchBase)(
        PalCommandBuffer* cmdBuffer,
        Uint32 baseGroupX,
        Uint32 baseGroupY,
        Uint32 baseGroupZ,
        Uint32 groupCountX,
        Uint32 groupCountY,
        Uint32 groupCountZ);

    PalResult PAL_CALL (*dispatchIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        Uint64 offset);

    PalResult PAL_CALL (*submitCommandBuffer)(
        PalQueue* queue,
        PalCommandBufferSubmitInfo* info);

    PalResult PAL_CALL (*createAccelerationstructure)(
        PalDevice* device,
        const PalAccelerationStructureCreateInfo* info,
        PalAccelerationStructure** outAs);

    void PAL_CALL (*destroyAccelerationstructure)(
        PalAccelerationStructure* as);

    PalResult PAL_CALL (*getAccelerationStructureBuildSize)(
        PalDevice* device,
        PalAccelerationStructureBuildInfo* info,
        PalAccelerationStructureBuildSize* size);

    PalResult PAL_CALL (*createBuffer)(
        PalDevice* device,
        const PalBufferCreateInfo* info,
        PalBuffer** outBuffer);

    void PAL_CALL (*destroyBuffer)(PalBuffer* buffer);

    PalResult PAL_CALL (*getBufferMemoryRequirements)(
        PalBuffer* buffer,
        PalMemoryRequirements* requirements);

    PalResult PAL_CALL (*bindBufferMemory)(
        PalBuffer* buffer,
        PalMemory* memory,
        Uint64 offset);

    PalResult PAL_CALL (*createPipelineLayout)(
        PalDevice* device,
        const PalPipelineLayoutCreateInfo* info,
        PalPipelineLayout** outLayout);

    void PAL_CALL (*destroyPipelineLayout)(PalPipelineLayout* layout);

    PalResult PAL_CALL (*createGraphicsPipeline)(
        PalDevice* device,
        const PalGraphicsPipelineCreateInfo* info,
        PalPipeline** outPipeline);

    void PAL_CALL (*destroyPipeline)(PalPipeline* pipeline);
} PalGraphicsBackend;

PAL_API PalResult PAL_CALL palAddGraphicsBackend(
    const PalGraphicsBackend* backend);

PAL_API PalResult PAL_CALL palInitGraphics(
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator);

PAL_API void PAL_CALL palShutdownGraphics();

PAL_API PalResult PAL_CALL palEnumerateAdapters(
    Int32* count,
    PalAdapter** outAdapters);

PAL_API PalResult PAL_CALL palGetAdapterInfo(
    PalAdapter* adapter,
    PalAdapterInfo* info);

PAL_API PalResult PAL_CALL palGetAdapterCapabilities(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps);

PAL_API PalAdapterFeatures PAL_CALL palGetAdapterFeatures(PalAdapter* adapter);

PAL_API PalResult PAL_CALL palCreateDevice(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice);

PAL_API void PAL_CALL palDestroyDevice(PalDevice* device);

PAL_API PalResult PAL_CALL palWaitDevice(PalDevice* device);

PAL_API PalResult PAL_CALL palAllocateMemory(
    PalDevice* device,
    PalMemoryType type,
    Uint64 size,
    PalMemory** outMemory);

PAL_API void PAL_CALL palFreeMemory(
    PalDevice* device,
    PalMemory* memory);

PAL_API PalResult PAL_CALL palMapMemory(
    PalDevice* device,
    PalMemory* memory,
    Uint64 offset,
    Uint64 size,
    void** outPtr);

PAL_API void PAL_CALL palUnmapMemory(
    PalDevice* device,
    PalMemory* memory);

PAL_API PalResult PAL_CALL palQueryDepthStencilCapabilities(
    PalDevice* device,
    PalDepthStencilCapabilities* caps);

PAL_API PalResult PAL_CALL palQueryFragmentShadingRateCapabilities(
    PalDevice* device,
    PalFragmentShadingRateCapabilities* caps);

PAL_API PalResult PAL_CALL palQueryMeshShaderCapabilities(
    PalDevice* device,
    PalMeshShaderCapabilities* caps);

PAL_API PalResult PAL_CALL palQueryRayTracingCapabilities(
    PalDevice* device,
    PalRayTracingCapabilities* caps);

PAL_API PalResult PAL_CALL palCreateQueue(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue);

PAL_API void PAL_CALL palDestroyQueue(PalQueue* queue);

PAL_API bool PAL_CALL palCanQueuePresent(
    PalQueue* queue,
    PalGraphicsWindow* window);

PAL_API PalResult PAL_CALL palWaitQueue(PalQueue* queue);

PAL_API PalResult PAL_CALL palEnumerateFormats(
    PalAdapter* adapter,
    Int32* count,
    PalFormatInfo* outFormats);

PAL_API bool PAL_CALL palIsFormatSupported(
    PalAdapter* adapter,
    PalFormat format);

PAL_API PalImageUsages PAL_CALL palQueryFormatImageUsages(
    PalAdapter* adapter,
    PalFormat format);

PAL_API PalImageViewUsages PAL_CALL palQueryFormatImageViewUsages(
    PalAdapter* adapter,
    PalFormat format);

PAL_API PalResult PAL_CALL palCreateImage(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage);

PAL_API void PAL_CALL palDestroyImage(PalImage* image);

PAL_API PalResult PAL_CALL palGetImageInfo(
    PalImage* image,
    PalImageInfo* info);

PAL_API PalResult PAL_CALL palGetImageMemoryRequirements(
    PalImage* image,
    PalMemoryRequirements* requirements);

PAL_API PalResult PAL_CALL palBindImageMemory(
    PalImage* image,
    PalMemory* memory,
    Uint64 offset);

PAL_API PalResult PAL_CALL palCreateImageView(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** outImageView);

PAL_API void PAL_CALL palDestroyImageView(PalImageView* imageView);

PAL_API PalResult PAL_CALL palQuerySwapchainCapabilities(
    PalDevice* device,
    PalGraphicsWindow* window,
    PalSwapchainCapabilities* caps);

PAL_API PalResult PAL_CALL palCreateSwapchain(
    PalDevice* device,
    PalQueue* queue,
    PalGraphicsWindow* window,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain);

PAL_API void PAL_CALL palDestroySwapchain(PalSwapchain* swapchain);

PAL_API PalImage* PAL_CALL palGetSwapchainImage(
    PalSwapchain* swapchain,
    Int32 index);

PAL_API PalResult PAL_CALL palGetNextSwapchainImage(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    Uint32 *outIndex);

PAL_API PalResult PAL_CALL palPresentSwapchain(
    PalSwapchain* swapchain,
    PalSwapchainPresentInfo* info);

PAL_API PalResult PAL_CALL palCreateShader(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader);

PAL_API void PAL_CALL palDestroyShader(PalShader* shader);

PAL_API PalResult PAL_CALL palCreateFence(
    PalDevice* device,
    bool signaled,
    PalFence** outFence);

PAL_API void PAL_CALL palDestroyFence(PalFence* fence);

PAL_API PalResult PAL_CALL palWaitFence(
    PalFence* fence,
    Uint64 timeout);

PAL_API PalResult PAL_CALL palResetFence(PalFence* fence);

PAL_API bool PAL_CALL palIsFenceSignaled(PalFence* fence);

PAL_API PalResult PAL_CALL palCreateSemaphore(
    PalDevice* device,
    PalSemaphore** outSemaphore);

PAL_API void PAL_CALL palDestroySemaphore(PalSemaphore* semaphore);

PAL_API PalResult PAL_CALL palWaitSemaphore(
    PalSemaphore* semaphore,
    PalQueue* queue,
    Uint64 value,
    Uint64 timeout);

PAL_API PalResult PAL_CALL palSignalSemaphore(
    PalSemaphore* semaphore,
    PalQueue* queue,
    Uint64 value);

PAL_API PalResult PAL_CALL palGetSemaphoreValue(
    PalSemaphore* semaphore,
    Uint64* value);

PAL_API PalResult PAL_CALL palCreateCommandPool(
    PalDevice* device,
    PalQueue* queue,
    PalCommandPool** outPool);

PAL_API void PAL_CALL palDestroyCommandPool(PalCommandPool* pool);

PAL_API PalResult PAL_CALL palResetCommandPool(PalCommandPool* pool);

PAL_API PalResult PAL_CALL palCreateCommandBuffer(
    PalDevice* device,
    PalCommandPool* pool,
    PalCommandBufferType type,
    PalCommandBuffer** outCmdbuffer);

PAL_API void PAL_CALL palDestroyCommandBuffer(PalCommandBuffer* cmdBuffer);

PAL_API PalResult PAL_CALL palBeginCommandBuffer(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info);

PAL_API PalResult PAL_CALL palEndCommandBuffer(PalCommandBuffer* cmdBuffer);

PAL_API PalResult PAL_CALL palResetCommandBuffer(PalCommandBuffer* cmdBuffer);

PAL_API PalResult PAL_CALL palExecuteCommandBuffer(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer);

PAL_API PalResult PAL_CALL palSetFragmentShadingRate(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state);

PAL_API PalResult PAL_CALL palDrawMeshTasks(
    PalCommandBuffer* cmdBuffer,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ);

PAL_API PalResult PAL_CALL palDrawMeshTasksIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 drawCount,
    Uint32 stride);

PAL_API PalResult PAL_CALL palDrawMeshTasksIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 maxDrawCount,
    Uint32 stride);

PAL_API PalResult PAL_CALL palBuildAccelerationStructure(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info);

PAL_API PalResult PAL_CALL palBeginRendering(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info);

PAL_API PalResult PAL_CALL palEndRendering(PalCommandBuffer* cmdBuffer);

PAL_API PalResult PAL_CALL palCopyBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    Uint64 dstOffset,
    Uint64 srcOffset,
    Uint32 size);

PAL_API PalResult PAL_CALL palBindPipeline(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline);

PAL_API PalResult PAL_CALL palSetViewport(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalViewport* viewports);

PAL_API PalResult PAL_CALL palSetScissors(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalRect2D* scissors);

PAL_API PalResult PAL_CALL palBindVertexBuffers(
    PalCommandBuffer* cmdBuffer,
    Uint32 firstSlot,
    Uint32 count,
    PalBuffer** buffers,
    Uint64* offsets);

PAL_API PalResult PAL_CALL palBindIndexBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    PalIndexType type);

PAL_API PalResult PAL_CALL palDraw(
    PalCommandBuffer* cmdBuffer,
    PalDrawData* data);

PAL_API PalResult PAL_CALL palDrawIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 count);

PAL_API PalResult PAL_CALL palDrawIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 count);

PAL_API PalResult PAL_CALL palDrawIndexed(
    PalCommandBuffer* cmdBuffer,
    PalDrawIndexedData* data);

PAL_API PalResult PAL_CALL palDrawIndexedIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 count);

PAL_API PalResult PAL_CALL palDrawIndexedIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 count);

PAL_API PalResult PAL_CALL palImageViewBarrier(
    PalCommandBuffer* cmdBuffer,
    PalImageView* imageView,
    PalUsageState oldUsageState,
    PalUsageState newUsageState);

PAL_API PalResult PAL_CALL palBufferBarrier(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalUsageState oldUsageState,
    PalUsageState newUsageState);

PAL_API PalResult PAL_CALL palDispatch(
    PalCommandBuffer* cmdBuffer,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ);

PAL_API PalResult PAL_CALL palDispatchBase(
    PalCommandBuffer* cmdBuffer,
    Uint32 baseGroupX,
    Uint32 baseGroupY,
    Uint32 baseGroupZ,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ);

PAL_API PalResult PAL_CALL palDispatchIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset);

PAL_API PalResult PAL_CALL palSubmitCommandBuffer(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info);

PAL_API PalResult PAL_CALL palCreateAccelerationstructure(
    PalDevice* device,
    const PalAccelerationStructureCreateInfo* info,
    PalAccelerationStructure** outAs);

PAL_API void PAL_CALL palDestroyAccelerationstructure(
    PalAccelerationStructure* as);

PAL_API PalResult PAL_CALL palGetAccelerationStructureBuildSize(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size);

PAL_API PalResult PAL_CALL palCreateBuffer(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer);

PAL_API void PAL_CALL palDestroyBuffer(PalBuffer* buffer);

PAL_API PalResult PAL_CALL palGetBufferMemoryRequirements(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements);

PAL_API PalResult PAL_CALL palBindBufferMemory(
    PalBuffer* buffer,
    PalMemory* memory,
    Uint64 offset);

PAL_API PalResult PAL_CALL palCreatePipelineLayout(
    PalDevice* device,
    const PalPipelineLayoutCreateInfo* info,
    PalPipelineLayout** outLayout);

PAL_API void PAL_CALL palDestroyPipelineLayout(PalPipelineLayout* layout);

PAL_API PalResult PAL_CALL palCreateGraphicsPipeline(
    PalDevice* device,
    const PalGraphicsPipelineCreateInfo* info,
    PalPipeline** outPipeline);

PAL_API void PAL_CALL palDestroyPipeline(PalPipeline* pipeline);

PAL_API bool PAL_CALL palBuildWorkGroupInfo(
    const PalWorkGroupBuildData* data,
    Int32* count,
    PalWorkGroupInfo* info);

/** @} */ // end of pal_graphics group

#endif // _PAL_GRAPHICS_H
