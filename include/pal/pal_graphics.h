
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

/**
 * @brief The maximum name size of an adapter (GPU).
 * @since 1.4
 * @ingroup pal_graphics
 */
#define PAL_ADAPTER_NAME_SIZE 128

/**
 * @brief The maximum version string size of an adapter.
 * @since 1.4
 * @ingroup pal_graphics
 */
#define PAL_ADAPTER_VERSION_SIZE 16

#define PAL_MAX_RESOLVE_MODES 8
#define PAL_MAX_COMBINER_OPS 8

/**
 * @brief A Unused shader index. Used to make a shader index invalid.
 * @since 1.4
 * @ingroup pal_graphics
 */
#define PAL_UNUSED_SHADER_INDEX UINT32_MAX

/**
 * @struct PalAdapter
 * @brief Opaque handle to an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalAdapter PalAdapter;

/**
 * @struct PalDevice
 * @brief Opaque handle to a device. Devices are created from an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalDevice PalDevice;

/**
 * @struct PalMemory
 * @brief Opaque handle to a device memory. This is not `CPU` memory.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalMemory PalMemory;

/**
 * @struct PalQueue
 * @brief Opaque handle to a queue.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalQueue PalQueue;

/**
 * @struct PalSwapchain
 * @brief Opaque handle to a swapchain.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalSwapchain PalSwapchain;

/**
 * @struct PalImage
 * @brief Opaque handle to an image.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalImage PalImage;

/**
 * @struct PalImageView
 * @brief Opaque handle to an image view.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalImageView PalImageView;

/**
 * @struct PalShader
 * @brief Opaque handle to a shader.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalShader PalShader;

/**
 * @struct PalBuffer
 * @brief Opaque handle to a buffer.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalBuffer PalBuffer;

/**
 * @struct PalFence
 * @brief Opaque handle to a fence.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalFence PalFence;

/**
 * @struct PalSemaphore
 * @brief Opaque handle to a semaphore.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalSemaphore PalSemaphore;

/**
 * @struct PalCommandPool
 * @brief Opaque handle to a command pool.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalCommandPool PalCommandPool;

/**
 * @struct PalCommandBuffer
 * @brief Opaque handle to a command buffer.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalCommandBuffer PalCommandBuffer;

/**
 * @struct PalDescriptorSetLayout
 * @brief Opaque handle to a descriptor set layout.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorSetLayout PalDescriptorSetLayout;

/**
 * @struct PalDescriptorPool
 * @brief Opaque handle to a descriptor pool.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorPool PalDescriptorPool;

/**
 * @struct PalDescriptorSet
 * @brief Opaque handle to a descriptor set.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorSet PalDescriptorSet;

/**
 * @struct PalSampler
 * @brief Opaque handle to a sampler.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalSampler PalSampler;

/**
 * @struct PalPipelineLayout
 * @brief Opaque handle to a pipeline layout.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalPipelineLayout PalPipelineLayout;

/**
 * @struct PalPipeline
 * @brief Opaque handle to a pipeline. This is the same handle used for all pipeline types
 * (Graphics, Compute and Ray tracing).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalPipeline PalPipeline;

/**
 * @struct PalAccelerationStructure
 * @brief Opaque handle to an acceleration structure.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalAccelerationStructure PalAccelerationStructure;

/**
 * @enum PalDebugMessageSeverity
 * @brief Debugger messages severity types used to filter incoming messages.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum PalDebugMessageSeverity PalDebugMessageSeverity;

/**
 * @enum PalDebugMessageType
 * @brief Debugger messages types used to filter incoming messages.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum PalDebugMessageType PalDebugMessageType;

/**
 * @typedef PalDeviceAddress
 * @brief Adapter address. Used to get adapter (GPU) address of mostly buffers.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef Uint64 PalDeviceAddress;

/**
 * @typedef PalDebugCallback
 * @brief Function pointer type used for debug callbacks.
 *
 * @param userData Optional pointer to user data passed from ::PalGraphicsDebugger. Can be nullptr.
 * @param severity Severity of the message. (`PAL_DEBUG_MESSAGE_SEVERITY_INFO`,
 * `PAL_DEBUG_MESSAGE_SEVERITY_WARNING` and `PAL_DEBUG_MESSAGE_SEVERITY_ERROR`).
 * @param type Type of the message. (`PAL_DEBUG_MESSAGE_TYPE_GENERAL`,
 * `PAL_DEBUG_MESSAGE_TYPE_VALIDATION` and `PAL_DEBUG_MESSAGE_TYPE_PERFORMANCE`).
 * @param msg Null-terminated UTF-8 debug message.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palInitGraphics
 */
typedef void(PAL_CALL* PalDebugCallback)(
    void* userData,
    PalDebugMessageSeverity severity,
    PalDebugMessageType type,
    const char* msg);

/**
 * @enum PalAdapterType
 * @brief Adapter (GPU) types.
 *
 * All adapter types follow the format `PAL_ADAPTER_TYPE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_ADAPTER_TYPE_UNKNOWN,
    PAL_ADAPTER_TYPE_DISCRETE,
    PAL_ADAPTER_TYPE_INTEGRATED,
    PAL_ADAPTER_TYPE_VIRTUAL,
    PAL_ADAPTER_TYPE_CPU
} PalAdapterType;

/**
 * @enum PalAdapterApiType
 * @brief Adapter API types.
 *
 * All adapter api types follow the format `PAL_ADAPTER_API_TYPE_**` for
 * consistency and API use.
 *
 * (`PAL_ADAPTER_API_TYPE_OPENGL`, `PAL_ADAPTER_API_TYPE_GLES`, `PAL_ADAPTER_API_TYPE_D3D11`
 * `PAL_ADAPTER_API_TYPE_D3D9`, `PAL_ADAPTER_API_TYPE_PPM`, etc) will not be supported by the
 * core graphics system. These are custom backends that can be use to extend the graphics systems.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
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

/**
 * @enum PalQueueType
 * @brief Queue types.
 *
 * All queue types follow the format `PAL_QUEUE_TYPE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_QUEUE_TYPE_GRAPHICS,
    PAL_QUEUE_TYPE_COMPUTE,
    PAL_QUEUE_TYPE_COPY
} PalQueueType;

/**
 * @enum PalPresentMode
 * @brief Present modes
 *
 * All present modes follow the format `PAL_PRESENT_MODE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_PRESENT_MODE_FIFO, /**< V-Sync.*/
    PAL_PRESENT_MODE_IMMEDIATE,
    PAL_PRESENT_MODE_MAILBOX,

    PAL_PRESENT_MODE_MAX
} PalPresentMode;

/**
 * @enum PalCompositeAplha
 * @brief Composite alphas
 *
 * All composite alphas follow the format `PAL_COMPOSITE_ALPHA_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_COMPOSITE_ALPHA_OPAQUE, /**< Default behavior.*/
    PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED,
    PAL_COMPOSITE_ALPHA_POST_MULTIPLIED,

    PAL_COMPOSITE_ALPHA_MAX
} PalCompositeAplha;

/**
 * @enum PalFormat
 * @brief Format types.
 *
 * All format types follow the format `PAL_FORMAT_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
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

/**
 * @enum PalImageUsages
 * @brief Image usages. Multiple image usages can be OR'ed together using bitwise
 * OR operator (`|`).
 *
 * All image usages follow the format `PAL_IMAGE_USAGE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_IMAGE_USAGE_UNDEFINED = 0,

    PAL_IMAGE_USAGE_COLOR_ATTACHEMENT = PAL_BIT(0),
    PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT = PAL_BIT(1),
    PAL_IMAGE_USAGE_TRANSFER_SRC = PAL_BIT(2),
    PAL_IMAGE_USAGE_TRANSFER_DST = PAL_BIT(3),
    PAL_IMAGE_USAGE_STORAGE = PAL_BIT(4),
    PAL_IMAGE_USAGE_SAMPLED = PAL_BIT(5)
} PalImageUsages;

/**
 * @enum PalImageViewUsages
 * @brief Image view usages. Multiple image view usages can be OR'ed together using bitwise
 * OR operator (`|`).
 *
 * All image view usages follow the format `PAL_IMAGE_VIEW_USAGE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_IMAGE_VIEW_USAGE_UNDEFINED = 0,

    PAL_IMAGE_VIEW_USAGE_COLOR = PAL_BIT(0),
    PAL_IMAGE_VIEW_USAGE_DEPTH = PAL_BIT(1),
    PAL_IMAGE_VIEW_USAGE_STENCIL = PAL_BIT(2),
    PAL_IMAGE_VIEW_USAGE_FRAGMENT_SHADING_RATE = PAL_BIT(3)
} PalImageViewUsages;

/**
 * @enum PalShaderFormats
 * @brief Shader formats. This is a bitmask.
 *
 * All shader formats follow the format `PAL_SHADER_FORMAT_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_SHADER_FORMAT_SPIRV = PAL_BIT(0),
    PAL_SHADER_FORMAT_DXIL = PAL_BIT(1),
    PAL_SHADER_FORMAT_DXBC = PAL_BIT(2),
    PAL_SHADER_FORMAT_GLSL = PAL_BIT(3),
    PAL_SHADER_FORMAT_MSL = PAL_BIT(4),
    PAL_SHADER_FORMAT_PPM = PAL_BIT(5)
} PalShaderFormats;

/**
 * @enum PalAdapterFeatures
 * @brief Adapter features. This is a bitmask.
 *
 * All adapter features follow the format `PAL_ADAPTER_FEATURE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
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

/**
 * @enum PalLoadOp
 * @brief Load operation type.
 *
 * All load operation type follow the format `PAL_LOAD_OP_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_LOAD_OP_LOAD,
    PAL_LOAD_OP_CLEAR,
    PAL_LOAD_OP_DONT_CARE,
} PalLoadOp;

/**
 * @enum PalStoreOp
 * @brief Store operation type.
 *
 * All store operation type follow the format `PAL_STORE_OP_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_STORE_OP_STORE,
    PAL_STORE_OP_DONT_CARE
} PalStoreOp;

/**
 * @enum PalMemoryType
 * @brief Memory types.
 *
 * All memory types follow the format `PAL_MEMORY_TYPE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_MEMORY_TYPE_GPU_ONLY,
    PAL_MEMORY_TYPE_CPU_UPLOAD,
    PAL_MEMORY_TYPE_CPU_READBACK,

    PAL_MEMORY_TYPE_MAX
} PalMemoryType;

/**
 * @enum PalImageType
 * @brief Image types.
 *
 * All image types follow the format `PAL_IMAGE_TYPE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_IMAGE_TYPE_1D,
    PAL_IMAGE_TYPE_2D,
    PAL_IMAGE_TYPE_3D
} PalImageType;

/**
 * @enum PalImageViewType
 * @brief Image view types.
 *
 * All image view types follow the format `PAL_IMAGE_VIEW_TYPE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_IMAGE_VIEW_TYPE_1D,
    PAL_IMAGE_VIEW_TYPE_1D_ARRAY,
    PAL_IMAGE_VIEW_TYPE_2D,
    PAL_IMAGE_VIEW_TYPE_2D_ARRAY,
    PAL_IMAGE_VIEW_TYPE_3D,
    PAL_IMAGE_VIEW_TYPE_CUBE,
    PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY,
} PalImageViewType;

/**
 * @enum PalSwapchainFormat
 * @brief swapchain format types.
 *
 * All swapchain format types follow the format `PAL_SWAPCHAIN_FORMAT_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_SWAPCHAIN_FORMAT_BGRA8_UNORM_SRGB,
    PAL_SWAPCHAIN_FORMAT_BGRA8_SRGB_SRGB,
    PAL_SWAPCHAIN_FORMAT_RGBA8_UNORM_SRGB,
    PAL_SWAPCHAIN_FORMAT_RGBA16_FLOAT_HDR10, /**< HDR.*/

    PAL_SWAPCHAIN_FORMAT_MAX
} PalSwapchainFormat;

/**
 * @enum PalShaderStage
 * @brief shader stage types.
 *
 * All shader stage types follow the format `PAL_SHADER_STAGE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_SHADER_STAGE_UNDEFINED,

    PAL_SHADER_STAGE_VERTEX,
    PAL_SHADER_STAGE_FRAGMENT,
    PAL_SHADER_STAGE_COMPUTE,
    PAL_SHADER_STAGE_GEOMETRY,
    PAL_SHADER_STAGE_MESH,
    PAL_SHADER_STAGE_TASK,
    PAL_SHADER_STAGE_TESSELLATION_CONTROL,
    PAL_SHADER_STAGE_TESSELLATION_EVALUATION,
    PAL_SHADER_STAGE_RAYGEN,
    PAL_SHADER_STAGE_CLOSEST_HIT,
    PAL_SHADER_STAGE_ANY_HIT,
    PAL_SHADER_STAGE_MISS,
    PAL_SHADER_STAGE_INTERSECTION,
    PAL_SHADER_STAGE_CALLABLE
} PalShaderStage;

/**
 * @enum PalSampleCount
 * @brief sample count.
 *
 * All sample count follow the format `PAL_SAMPLE_COUNT_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_SAMPLE_COUNT_1,
    PAL_SAMPLE_COUNT_2,
    PAL_SAMPLE_COUNT_4,
    PAL_SAMPLE_COUNT_8,
    PAL_SAMPLE_COUNT_16,
    PAL_SAMPLE_COUNT_32,
    PAL_SAMPLE_COUNT_64
} PalSampleCount;

/**
 * @enum PalPrimitiveTopology
 * @brief Primitve topology types.
 *
 * All primitve topology types follow the format `PAL_PRIMITIVE_TOPOLOGY_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
    PAL_PRIMITIVE_TOPOLOGY_LINE_LIST,
    PAL_PRIMITIVE_TOPOLOGY_LINE_STRIP,
    PAL_PRIMITIVE_TOPOLOGY_POINT_LIST,
    PAL_PRIMITIVE_TOPOLOGY_PATCH
} PalPrimitiveTopology;

/**
 * @enum PalCullMode
 * @brief Cull modes.
 *
 * All cull modes follow the format `PAL_CULL_MODE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_CULL_MODE_NONE,
    PAL_CULL_MODE_FRONT,
    PAL_CULL_MODE_BACK
} PalCullMode;

/**
 * @enum PalFrontFace
 * @brief Front face modes.
 *
 * All front face modes follow the format `PAL_FRONT_FACE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_FRONT_FACE_CLOCKWISE,
    PAL_FRONT_FACE_COUNTER_CLOCKWISE
} PalFrontFace;

/**
 * @enum PalPolygonMode
 * @brief Polygon modes.
 *
 * All polygon modes follow the format `PAL_POLYGON_MODE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_POLYGON_MODE_FILL,
    PAL_POLYGON_MODE_LINE
} PalPolygonMode;

/**
 * @enum PalVertexType
 * @brief Vertex attribute types.
 *
 * All vertex attribute types follow the format `PAL_VERTEX_TYPE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_VERTEX_TYPE_UNDEFINED,

    PAL_VERTEX_TYPE_INT32,   /**< Int32.*/
    PAL_VERTEX_TYPE_INT32_2, /**< Int32 vec2 or array[2].*/
    PAL_VERTEX_TYPE_INT32_3, /**< Int32 vec3 or array[3].*/
    PAL_VERTEX_TYPE_INT32_4, /**< Int32 vec4 or array[4].*/

    PAL_VERTEX_TYPE_UINT32,   /**< Uint32.*/
    PAL_VERTEX_TYPE_UINT32_2, /**< Uint32 vec2 or array[2].*/
    PAL_VERTEX_TYPE_UINT32_3, /**< Uint32 vec3 or array[3].*/
    PAL_VERTEX_TYPE_UINT32_4, /**< Uint32 vec4 or array[4].*/

    PAL_VERTEX_TYPE_INT8_2,  /**< Int8 vec2 or array[2].*/
    PAL_VERTEX_TYPE_INT8_4,  /**< Int8 vec4 or array[4].*/
    PAL_VERTEX_TYPE_UINT8_2, /**< Uint8 vec2 or array[2].*/
    PAL_VERTEX_TYPE_UINT8_4, /**< Uint8 vec4 or array[4].*/

    PAL_VERTEX_TYPE_INT8_2NORM,  /**< Int8 vec2 or array[2] normalized.*/
    PAL_VERTEX_TYPE_INT8_4NORM,  /**< Int8 vec4 or array[4] normalized.*/
    PAL_VERTEX_TYPE_UINT8_2NORM, /**< Uint8 vec2 or array[2] normalized.*/
    PAL_VERTEX_TYPE_UINT8_4NORM, /**< Uint8 vec4 or array[4] normalized.*/

    PAL_VERTEX_TYPE_INT16_2,  /**< Int16 vec2 or array[2].*/
    PAL_VERTEX_TYPE_INT16_4,  /**< Int16 vec4 or array[4].*/
    PAL_VERTEX_TYPE_UINT16_2, /**< Uint16 vec2 or array[2].*/
    PAL_VERTEX_TYPE_UINT16_4, /**< Uint16 vec4 or array[4].*/

    PAL_VERTEX_TYPE_INT16_2NORM,  /**< Int16 vec2 or array[2] normalized.*/
    PAL_VERTEX_TYPE_INT16_4NORM,  /**< Int16 vec4 or array[4] normalized.*/
    PAL_VERTEX_TYPE_UINT16_2NORM, /**< Uint16 vec2 or array[2] normalized.*/
    PAL_VERTEX_TYPE_UINT16_4NORM, /**< Uint16 vec4 or array[4] normalized.*/

    PAL_VERTEX_TYPE_FLOAT,  /**< float*/
    PAL_VERTEX_TYPE_FLOAT2, /**< float vec2 or array[2].*/
    PAL_VERTEX_TYPE_FLOAT3, /**< float vec3 or array[3].*/
    PAL_VERTEX_TYPE_FLOAT4, /**< float vec4 or array[4].*/

    PAL_VERTEX_TYPE_HALF_FLOAT16_2, /**< float16 vec2 or array[2].*/
    PAL_VERTEX_TYPE_HALF_FLOAT16_4  /**< float16 vec4 or array[4].*/
} PalVertexType;

/**
 * @enum PalCommandBufferType
 * @brief Command buffer types.
 *
 * All command buffer types follow the format `PAL_COMMAND_BUFFER_TYPE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_COMMAND_BUFFER_TYPE_PRIMARY,
    PAL_COMMAND_BUFFER_TYPE_SECONDARY
} PalCommandBufferType;

/**
 * @enum PalVertexLayoutType
 * @brief Vertex layout types.
 *
 * All vertex layout types follow the format `PAL_VERTEX_LAYOUT_TYPE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_VERTEX_LAYOUT_TYPE_PER_VERTEX,
    PAL_VERTEX_LAYOUT_TYPE_PER_INSTANCE
} PalVertexLayoutType;

/**
 * @enum PalCompareOp
 * @brief Compare operation modes.
 *
 * All compare operation modes follow the format `PAL_COMPARE_OP_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
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

/**
 * @enum PalStencilOp
 * @brief Stencil operation modes.
 *
 * All stencil operation modes follow the format `PAL_STENCIL_OP_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
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

/**
 * @enum PalBlendOp
 * @brief Blend operation modes.
 *
 * All blend operation modes follow the format `PAL_BLEND_OP_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_BLEND_OP_ADD,
    PAL_BLEND_OP_SUBTRACT,
    PAL_BLEND_OP_REVERSE_SUBTRACT,
    PAL_BLEND_OP_MIN,
    PAL_BLEND_OP_MAX
} PalBlendOp;

/**
 * @enum PalBlendOp
 * @brief Blend factor modes.
 *
 * All blend factor modes follow the format `PAL_BLEND_FACTOR_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
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

/**
 * @enum PalColorMask
 * @brief Color mask flags. Multiple color mask flags can be OR'ed together using bitwise
 * OR operator (`|`).
 *
 * `PAL_COLOR_MASK_NONE` is not a bit and must not be combined with other bits.
 *
 * All color mask flags follow the format `PAL_BLEND_FACTOR_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_COLOR_MASK_NONE = 0,

    PAL_COLOR_MASK_RED = PAL_BIT(0),
    PAL_COLOR_MASK_GREEN = PAL_BIT(1),
    PAL_COLOR_MASK_BLUE = PAL_BIT(2),
    PAL_COLOR_MASK_ALPHA = PAL_BIT(3),
} PalColorMask;

/**
 * @enum PalResolveMode
 * @brief Resolve modes.
 *
 * All resolve modes follow the format `PAL_RESOLVE_MODE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_RESOLVE_MODE_NONE = 0,

    PAL_RESOLVE_MODE_SAMPLE_ZERO,
    PAL_RESOLVE_MODE_AVERAGE,
    PAL_RESOLVE_MODE_MIN,
    PAL_RESOLVE_MODE_MAX
} PalResolveMode;

/**
 * @enum PalFragmentShadingRate
 * @brief Fragment shading rates.
 *
 * All fragment shading rates follow the format `PAL_FRAGMENT_SHADING_RATE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
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

/**
 * @enum PalFragmentShadingRateCombinerOp
 * @brief Fragment shading rate combiner operaton modes.
 *
 * All fragment shading rate combiner operation modes follow the format
 * `PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_**` for consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP,
    PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE,
    PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN,
    PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX,
    PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL
} PalFragmentShadingRateCombinerOp;

/**
 * @enum PalAccelerationStructureType
 * @brief Acceleration structure types.
 *
 * All acceleration structure types follow the format `PAL_ACCELERATION_STRUCTURE_TYPE_**`
 * for consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL,
    PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL
} PalAccelerationStructureType;

/**
 * @enum PalGeometryType
 * @brief Geometry types.
 *
 * All geometry types follow the format `PAL_GEOMETRY_TYPE_**` for consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_GEOMETRY_TYPE_TRIANGLE,
    PAL_GEOMETRY_TYPE_AABBS,
    PAL_GEOMETRY_TYPE_INSTANCE
} PalGeometryType;

/**
 * @enum PalIndexType
 * @brief Index types.
 *
 * All index types follow the format `PAL_INDEX_TYPE_**` for consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_INDEX_TYPE_UINT16,
    PAL_INDEX_TYPE_UINT32
} PalIndexType;

/**
 * @enum PalBufferUsages
 * @brief Buffer usages. Multiple buffer usages can be OR'ed together using bitwise
 * OR operator (`|`).
 *
 * All buffer usages follow the format `PAL_BUFFER_USAGE_**` for consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_BUFFER_USAGE_VERTEX = PAL_BIT(0),
    PAL_BUFFER_USAGE_INDEX = PAL_BIT(1),
    PAL_BUFFER_USAGE_UNIFORM = PAL_BIT(2),
    PAL_BUFFER_USAGE_STORAGE = PAL_BIT(3),
    PAL_BUFFER_USAGE_TRANSFER_SRC = PAL_BIT(4),
    PAL_BUFFER_USAGE_TRANSFER_DST = PAL_BIT(5),
    PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE = PAL_BIT(6),
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

/**
 * @enum PalUsageState
 * @brief Usage states.
 *
 * All usage states follow the format `PAL_USAGE_STATE_**` for consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_USAGE_STATE_UNDEFINED,

    PAL_USAGE_STATE_PRESENT,
    PAL_USAGE_STATE_COLOR_ATTACHMENT_READ,
    PAL_USAGE_STATE_COLOR_ATTACHMENT_WRITE,
    PAL_USAGE_STATE_DEPTH_ATTACHMENT_READ,
    PAL_USAGE_STATE_DEPTH_ATTACHMENT_WRITE,
    PAL_USAGE_STATE_STENCIL_ATTACHMENT_READ,
    PAL_USAGE_STATE_STENCIL_ATTACHMENT_WRITE,
    PAL_USAGE_STATE_FRAGMENT_SHADING_RATE_ATTACHMENT_READ,
    PAL_USAGE_STATE_TRANSFER_READ,
    PAL_USAGE_STATE_TRANSFER_WRITE,
    PAL_USAGE_STATE_VERTEX_READ,
    PAL_USAGE_STATE_INDEX_READ,
    PAL_USAGE_STATE_UNIFORM_READ,
    PAL_USAGE_STATE_SHADER_READ,
    PAL_USAGE_STATE_SHADER_WRITE,
    PAL_USAGE_STATE_STORAGE_READ,
    PAL_USAGE_STATE_STORAGE_WRITE,
    PAL_USAGE_STATE_HOST_READ,
    PAL_USAGE_STATE_HOST_WRITE,
    PAL_USAGE_STATE_ACCELERATION_STRUCTURE_READ,
    PAL_USAGE_STATE_ACCELERATION_STRUCTURE_WRITE
} PalUsageState;

/**
 * @enum PalDescriptorType
 * @brief Descriptor types.
 *
 * All descriptor types follow the format `PAL_DESCRIPTOR_TYPE_**` for consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
    PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    PAL_DESCRIPTOR_TYPE_SAMPLER,
    PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE
} PalDescriptorType;

/**
 * @enum PalRayTracingShaderGroupType
 * @brief Ray tracing shader group types.
 *
 * All ray tracing shader group types follow the format `PAL_RAY_TRACING_SHADER_GROUP_TYPE_**`
 * for consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL,
    PAL_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT,
    PAL_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT
} PalRayTracingShaderGroupType;

/**
 * @struct PalAdapterInfo
 * @brief Information about an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 vendorId;
    Uint32 deviceId;
    PalAdapterType type;            /**< Discrete, Integrated, etc.*/
    PalAdapterApiType apiType;      /**< Vulkan, D3D12, etc.*/
    PalShaderFormats shaderFormats; /**< Supported shader formats mask (eg. Spirv, DXIL, ect).*/
    Uint64 vram;
    Uint64 sharedMemory;
    Uint64 version;                               /**< Adapter version.*/
    char versionString[PAL_ADAPTER_VERSION_SIZE]; /**< Adapter version in string.*/
    char name[PAL_ADAPTER_NAME_SIZE];
    char backendName[PAL_ADAPTER_NAME_SIZE]; /**< Adapter backend name (eg. `PAL`, `Custom`).*/
} PalAdapterInfo;

/**
 * @struct PalAdapterCapabilities
 * @brief Capabilities of an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 maxComputeQueues;  /**< Number of compute queues that can be created.*/
    Uint32 maxGraphicsQueues; /**< Number of graphics queues that can be created.*/
    Uint32 maxCopyQueues;     /**< Number of copy queues that can be created.*/
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
    Uint32 maxComputeWorkGroupInvocations; /**< Max compute threads per workgroup across all axis.*/
    Uint32 maxComputeWorkGroupCount[3];    /**< Max compute workgroups per axis.*/
    Uint32 maxComputeWorkGroupSize[3];     /**< Max compute threads per workgroup per axis.*/
} PalAdapterCapabilities;

/**
 * @struct PalDepthStencilCapabilities
 * @brief Depth stencil capabilities of an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    /** If false, depth and stencil resolve modes must be the same.*/
    bool independentDepthStencilResolve;

    /** Bool array of supported depth resolve modes.
     * (eg. depthResolveModes[`PAL_RESOLVE_MODE_SAMPLE_ZERO`]).*/
    bool depthResolveModes[PAL_MAX_RESOLVE_MODES];

    /** Bool array of supported stencil resolve modes.
     * (eg. stencilResolveModes[`PAL_RESOLVE_MODE_SAMPLE_ZERO`]).*/
    bool stencilResolveModes[PAL_MAX_RESOLVE_MODES];
} PalDepthStencilCapabilities;

/**
 * @struct PalFragmentShadingRateCapabilities
 * @brief Fragment shading rate capabilities of an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    /** Bool array of supported fragment shading rates.
     * (eg. shadingRates[`PAL_FRAGMENT_SHADING_RATE_2X1`]).*/
    bool shadingRates[PAL_FRAGMENT_SHADING_RATE_MAX];
    Uint32 minTexelWidth;
    Uint32 minTexelHeight;
    Uint32 maxTexelWidth;
    Uint32 maxTexelHeight;

    /** Bool array of supported fragment shading rate combiner operations.
     * (eg. combinerOps[`PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP`]).*/
    bool combinerOps[PAL_MAX_COMBINER_OPS];
} PalFragmentShadingRateCapabilities;

/**
 * @struct PalMeshShaderCapabilities
 * @brief Mesh shader capabilities of an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 maxMeshOutputPrimitives;     /**< Max mesh primitives per workgroup.*/
    Uint32 maxMeshOutputVertices;       /**< Max mesh vertices per workgroup.*/
    Uint32 maxTaskWorkGroupInvocations; /**< Max task threads per workgroup.*/
    Uint32 maxMeshWorkGroupInvocations; /**< Max mesh threads per workgroup.*/
    Uint32 maxTaskWorkGroupCount[3];    /**< Max task workgroups per axis.*/
    Uint32 maxMeshWorkGroupCount[3];    /**< Max mesh workgroups per axis.*/
} PalMeshShaderCapabilities;

/**
 * @struct PalRayTracingCapabilities
 * @brief Ray tracing capabilities of an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 maxRecursionDepth;
    Uint32 maxHitAttributeSize; /**< Max memory per intersection attributes.*/
    Uint32 maxInstanceCount;
    Uint32 maxPrimitiveCount;
    Uint32 maxGeometryCount;
    Uint32 maxPayloadSize;         /**< Max memory per ray.*/
    Uint32 maxDispatchInvocations; /**< Max ray threads per dispatch.*/
} PalRayTracingCapabilities;

/**
 * @struct PalDescriptorIndexingCapabilities
 * @brief Descriptor indexing capabilities of an adapter (GPU).
 *
 * Bindless images are always supported if Descriptor indexing is supported.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    bool bindlessStorageBuffers; /**< If true, bindless storage buffers are supported.*/
    bool bindlessUniformBuffers; /**< If true, bindless uniform buffers are supported.*/
    Uint32 maxImagesPerShaderStage;
    Uint32 maxImagesPerDescriptorSet;
    Uint32 maxStorageBuffersPerShaderStage;
    Uint32 maxStorageBuffersPerDescriptorSet;
    Uint32 maxUniformBuffersPerShaderStage;
    Uint32 maxUniformBuffersPerDescriptorSet;
    Uint32 maxDescriptors;
} PalDescriptorIndexingCapabilities;

/**
 * @struct PalSwapchainCapabilities
 * @brief swapchain capabilities of an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    /** Bool array of supported present modes.
     * (eg. presentModes[`PAL_PRESENT_MODE_FIFO`]).*/
    bool presentModes[PAL_PRESENT_MODE_MAX];

    /** Bool array of supported composite alphas.
     * (eg. compositeAlphas[`PAL_COMPOSITE_ALPHA_OPAQUE`]).*/
    bool compositeAlphas[PAL_COMPOSITE_ALPHA_MAX];

    /** Bool array of supported swapchain formats.
     * (eg. formats[`PAL_COMPOSITE_ALPHA_OPAQUE`]).*/
    bool formats[PAL_SWAPCHAIN_FORMAT_MAX];
    Uint32 minImageCount;
    Uint32 maxImageCount;
    Uint32 minImageWidth;
    Uint32 minImageHeight;
    Uint32 maxImageWidth;
    Uint32 maxImageHeight;
    Uint32 maxImageArrayLayers;
} PalSwapchainCapabilities;

/**
 * @struct PalGraphicsWindow
 * @brief Information about a graphics window.
 *
 * This can be allocated statically or dynamically since its used for
 * holding native handles. The handles will not be copied.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    void* display; /**< Can be nullptr depending on platform (eg. Windows).*/
    void* window;  /**< Must not be nullptr.*/
} PalGraphicsWindow;

/**
 * @struct PalFormatInfo
 * @brief Information about a format. This includes the supported image and image view usages
 * from the provided format.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalFormat format;              /**< The format.*/
    PalImageUsages usages;         /**< Supported image usages of the format.*/
    PalImageViewUsages viewUsages; /**< Supported image view usages of the format.*/
} PalFormatInfo;

/**
 * @struct PalImageInfo
 * @brief Information about an image. This can be a swapchain image.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 width;            /**< Width of the image in pixels.*/
    Uint32 height;           /**< Height of the image in pixels.*/
    Uint32 depthOrArraySize; /**< Depth for 3D image and array size for 2D image.*/
    Uint32 mipLevelCount;
    PalSampleCount sampleCount;
    PalImageType type; /**< 1D, 2D, 3D.*/
    PalFormat format;
    PalImageUsages usages;
} PalImageInfo;

/**
 * @struct PalClearValue
 * @brief Clear values used with rendering.
 *
 * If used with a color attachment, the color values will be used and depth and stencil
 * will be used with depth stencil attachments.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    float color[4]; /**< Color for color attachments.*/
    float depth;    /**< Depth for depth stencil attachments.*/
    Uint32 stencil; /**< Stencil for depth stencil attachments.*/
} PalClearValue;

/**
 * @struct PalAttachmentDesc
 * @brief An attachment description.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalLoadOp loadOp;
    PalStoreOp storeOp;
    PalResolveMode resolveMode;     /**< Used if resolveImageView is set.*/
    Uint32 texelWidth;              /**< Texel width for fragment shading rate attachment.*/
    Uint32 texelHeight;             /**< Texel height for fragment shading rate attachment.*/
    PalImageView* imageView;        /**< Image view. Must not be nullptr.*/
    PalImageView* resolveImageView; /**< Optional resolve image view.*/
    PalClearValue clearValue;
} PalAttachmentDesc;

/**
 * @struct PalUsageStateInfo
 * @brief Information about resource usage state. This is used with barrier commands.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalUsageState usageState;
    PalShaderStage shaderStage;
} PalUsageStateInfo;

/**
 * @struct PalViewport
 * @brief A viewport in pixels (float).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    float x;
    float y;
    float width;
    float height;
    float minDepth;
    float maxDepth;
} PalViewport;

/**
 * @struct PalRect2D
 * @brief A 2D rectangle in pixels.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Int32 x;
    Int32 y;
    Uint32 width;
    Uint32 height;
} PalRect2D;

/**
 * @struct PalMemoryRequirements
 * @brief Memory requirements for a resource (image, buffer etc).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    /** Bool array of supported memory types.
     * (eg. memoryTypes[`PAL_MEMORY_TYPE_GPU_ONLY`]).*/
    bool memoryTypes[PAL_MEMORY_TYPE_MAX];

    Uint64 memoryMask;
    Uint64 size;
    Uint32 alignment;
} PalMemoryRequirements;

/**
 * @struct PalInstanceBufferRequirements
 * @brief Instance buffer requirements.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint64 size;
    Uint32 alignment;
} PalInstanceBufferRequirements;

/**
 * @struct PalCommandBufferSubmitInfo
 * @brief Submit information of a command buffer.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint64 waitValue;   /**< Used if `PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE` is supported.*/
    Uint64 signalValue; /**< Used if `PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE` is supported.*/
    PalCommandBuffer* cmdBuffer;
    PalSemaphore* waitSemaphore;
    PalSemaphore* signalSemaphore;
    PalFence* fence;
} PalCommandBufferSubmitInfo;

/**
 * @struct PalSwapchainNextImageInfo
 * @brief Next image information of a swapchain.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint64 timeout;     /**< Timeout in milliseconds.*/
    Uint64 signalValue; /**< Used if `PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE` is supported.*/
    PalSemaphore* signalSemaphore;
    PalFence* fence;
} PalSwapchainNextImageInfo;

/**
 * @struct PalSwapchainPresentInfo
 * @brief Present information of a swapchain.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 imageIndex; /**< Image index to present. Must be 0 and less than max images.*/
    Uint64 waitValue;  /**< Used if `PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE` is supported.*/
    PalSemaphore* waitSemaphore;
} PalSwapchainPresentInfo;

/**
 * @struct PalRenderingInfo
 * @brief Information about how rendering should be done in graphics pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 viewCount; /**< If > 1 `PAL_ADAPTER_FEATURE_MULTI_VIEW` must be supported.*/
    Uint32 layerCount;
    Uint32 colorAttachentCount;
    PalSampleCount multisampleCount;
    PalAttachmentDesc* colorAttachments;
    PalAttachmentDesc* depthAttachment;
    PalAttachmentDesc* stencilAttachment;
    PalAttachmentDesc* fragmentShadingRateAttachment;
    PalRect2D renderArea;
} PalRenderingInfo;

/**
 * @struct PalRenderingLayoutInfo
 * @brief Information about a pre-existing PalRenderingInfo.
 * This is used to reference the already existing PalRenderingInfo.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 viewCount; /**< If > 1 `PAL_ADAPTER_FEATURE_MULTI_VIEW` must be supported.*/
    Uint32 colorAttachentCount;
    PalSampleCount multisampleCount;
    PalFormat depthAttachmentFormat;
    PalFormat stencilAttachmentFormat;
    PalFormat fragmentShadingRateAttachmentFormat;
    PalFormat* colorAttachmentsFormat;
} PalRenderingLayoutInfo;

/**
 * @struct PalWorkGroupBuildData
 * @brief Compute or Mesh(or Task) workgroup input data build helper.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    /** Workcount can be specified in pixels, vertices etc.
     * eg. an image of 800 x 600 will have a workcount of workCount[0] = 800,
     * workCount[1] = 600, workCount[2] = 1.*/
    Uint32 workCount[3];
    Uint32 workGroupSize[3];  /**< Threads per workgroup per axis of the adapter (GPU).*/
    Uint32 workGroupCount[3]; /**< Workgroups per axis of the adapter (GPU).*/
} PalWorkGroupBuildData;

/**
 * @struct PalWorkGroupInfo
 * @brief Information about compute or mesh(or task) dispatch data or a dispatch tile.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 workGroupBase[3];  /**< Offset per axis of a dispatch tile.*/
    Uint32 workGroupCount[3]; /**< Workgroup count per axis of a dispatch tile.*/
} PalWorkGroupInfo;

/**
 * @struct PalDrawData
 * @brief Draw data of a single draw call.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 vertexCount;   /**< Number of vertices to draw.*/
    Uint32 instanceCount; /**< Number of instances to draw. Set to 1 if not using instanced draw.*/
    Uint32 firstVertex;   /**< First vertex. Set to 0 for default behavior.*/
    Uint32 firstInstance; /**< First instance. Set to 0 for default behavior.*/
} PalDrawData;

/**
 * @struct PalDrawData
 * @brief Draw indexed data of a single draw call.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 indexCount;    /**< Number of indices to draw.*/
    Uint32 instanceCount; /**< Number of instances to draw. Set to 1 if not using instanced draw.*/
    Uint32 firstIndex;    /**< First index. Set to 0 for default behavior.*/
    Int32 vertexOffset;   /**< Vertex offset. Set to 0 for default behavior.*/
    Uint32 firstInstance; /**< First instance. Set to 0 for default behavior.*/
} PalDrawIndexedData;

/**
 * @struct PalVertexAttribute
 * @brief Vertex attribute.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalVertexType type; /**< (eg. PAL_VERTEX_TYPE_FLOAT).*/
    Uint32 location;
} PalVertexAttribute;

/**
 * @struct PalVertexLayout
 * @brief Vertex layout.
 * This defines the layout and the number of vertex attributes the layout uses.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalVertexLayoutType type; /**< (eg. PAL_VERTEX_LAYOUT_TYPE_PER_VERTEX).*/
    Uint32 binding;
    Uint32 attributeCount;
    PalVertexAttribute* attributes;
} PalVertexLayout;

/**
 * @struct PalGraphicsDebugger
 * @brief Graphics debugger.
 *
 * The debugger will not be initialized if PalGraphicsDebugger::callback is set and valid.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    void* userData;
    PalDebugCallback callback;
} PalGraphicsDebugger;

/**
 * @struct PalRasterizerState
 * @brief Rasterizer state. This is used with a graphics pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    bool enableDepthClamp;
    bool enableDepthBias;
    PalPolygonMode polygonMode; /**< (eg. PAL_POLYGON_MODE_FILL).*/
    PalCullMode cullMode;       /**< (eg. PAL_CULL_MODE_BACK).*/
    PalFrontFace frontFace;     /**< (eg. PAL_FRONT_FACE_CLOCKWISE).*/
} PalRasterizerState;

/**
 * @struct PalMultisampleState
 * @brief Multisample state. This is used with a graphics pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    bool enableSampleShading;
    bool enableAlphaToCoverage;
    PalSampleCount sampleCount;
    Uint64 sampleMask;
    float minSampleShading;
} PalMultisampleState;

/**
 * @struct PalStencilOpState
 * @brief Stencil operation state. This is used with a graphics pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalStencilOp failOp;
    PalStencilOp passOp;
    PalStencilOp depthFailOp;
    PalCompareOp compareOp;
} PalStencilOpState;

/**
 * @struct PalDepthStencilState
 * @brief Depth stencil state. This is used with a graphics pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    bool enableDepthTest;
    bool enableDepthWrite;
    bool enableStencilTest;
    PalCompareOp compareOp;
    PalStencilOpState frontStencilOpState;
    PalStencilOpState backStencilOpState;
} PalDepthStencilState;

/**
 * @struct PalColorBlendAttachment
 * @brief Color blend attachmeent. This is used with a graphics pipeline.
 *
 * Every rendering attachment (color, etc) must have a color blend attachment to
 * describe how blending is applied to the attachment.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    bool enableBlend;
    PalColorMask colorWriteMask;
    PalBlendFactor srcColorBlendFactor;
    PalBlendFactor dstColorBlendFactor;
    PalBlendOp colorBlendOp;
    PalBlendFactor srcAlphaBlendFactor;
    PalBlendFactor dstAlphaBlendFactor;
    PalBlendOp alphaBlendOp;
} PalColorBlendAttachment;

/**
 * @struct PalFragmentShadingRateState
 * @brief Fragment shading rate state. This is used with a graphics pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalFragmentShadingRate rate; /**< (eg. PAL_FRAGMENT_SHADING_RATE_2X1).*/
    PalFragmentShadingRateCombinerOp combinerOps[2];
} PalFragmentShadingRateState;

/**
 * @struct PalAccelerationStructureInstance
 * @brief Acceleration structure instance base data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 instanceId; /**< User defined id.*/
    Uint32 mask;
    PalAccelerationStructure* blas; /**< BLAS to use.*/
    float transform[12];            /**< row major (3x4).*/
} PalAccelerationStructureInstance;

/**
 * @struct PalAccelerationStructureBuildSize
 * @brief Acceleration structure build size.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 accelerationStructureSize; /**< Required acceleration structure size.*/
    Uint32 scratchBufferSize;         /**< Required scratch buffer size.*/
} PalAccelerationStructureBuildSize;

/**
 * @struct PalGeometryDataTriangle
 * @brief Acceleration structure triangle geometry data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalVertexType vertexType; /**< (eg. PAL_VERTEX_TYPE_FLOAT3).*/
    PalIndexType indexType;   /**< (eg. PAL_INDEX_TYPE_UINT32). If indexBufferAddress is not 0.*/
    Uint32 vertexStride;
    Uint32 indexCount;
    Uint32 vertexCount;
    PalDeviceAddress vertexBufferAddress;
    PalDeviceAddress indexBufferAddress;
} PalGeometryDataTriangle;

/**
 * @struct PalGeometryDataAABBS
 * @brief Acceleration structure AABBS geometry data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 stride;
    PalDeviceAddress bufferAddress;
} PalGeometryDataAABBS;

/**
 * @struct PalGeometryDataInstance
 * @brief Acceleration structure instance geometry data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalDeviceAddress bufferAddress;
} PalGeometryDataInstance;

/**
 * @struct PalGeometryDataInstance
 * @brief Acceleration structure geometry.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 primitiveCount;
    PalGeometryType type; /**< (eg. PAL_GEOMETRY_TYPE_TRIANGLE).*/
    void* data; /**< Pointer to geometry data. This will be casted based on the geometry type.*/
} PalGeometry;

/**
 * @struct PalAccelerationStructureBuildInfo
 * @brief Build information of an acceleration structure.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalAccelerationStructureType type; /**< (eg. PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL).*/
    Uint32 geometryCount;
    PalAccelerationStructure* dst;
    PalDeviceAddress scratchBufferAddress;
    PalGeometry* geometries;
} PalAccelerationStructureBuildInfo;

/**
 * @struct PalDescriptorSetLayoutBinding
 * @brief Single descriptor set layout binding.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 binding;
    Uint32 descriptorCount;
    Uint32 shaderStageCount;
    PalShaderStage* shaderStages;     /**< Array of shader stages that can access the descriptor.*/
    PalDescriptorType descriptorType; /**< (eg. PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER).*/
} PalDescriptorSetLayoutBinding;

/**
 * @struct PalDescriptorPoolBindingSize
 * @brief Descriptor pool binding size.
 * Describes the sizes of each descriptor type in the descriptor pool.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 bindingCount; /**< Number of descriptors of `descriptorType` that will be used.*/
    PalDescriptorType descriptorType; /**< (eg. PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER).*/
} PalDescriptorPoolBindingSize;

/**
 * @struct PalDescriptorBufferInfo
 * @brief Information about a buffer descriptor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 size;
    Uint64 offset;
    PalBuffer* buffer;
} PalDescriptorBufferInfo;

/**
 * @struct PalDescriptorImageViewInfo
 * @brief Information about an image view descriptor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalSampler* sampler;
    PalImageView* imageView;
} PalDescriptorImageViewInfo;

/**
 * @struct PalDescriptorTLASInfo
 * @brief Information about a TLAS descriptor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalAccelerationStructure* tlas;
} PalDescriptorTLASInfo;

/**
 * @struct PalDescriptorSetWriteInfo
 * @brief Write information of a descriptor set.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 binding;
    Uint32 arrayElement;    /**< 0 If not using PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING.*/
    Uint32 descriptorCount; /**< 1 If not using PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING.*/
    PalDescriptorType descriptorType;
    PalDescriptorSet* descriptorSet;
    PalDescriptorBufferInfo* bufferInfo; /**< If PAL_DESCRIPTOR_TYPE* uniform or storage buffer.*/
    PalDescriptorImageViewInfo* imageViewInfo; /**< If PAL_DESCRIPTOR_TYPE* sampler or image.*/
    PalDescriptorTLASInfo* tlasInfo;           /**< If PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE.*/
} PalDescriptorSetWriteInfo;

/**
 * @struct PalPushConstantRange
 * @brief Push constant range.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint64 offset;
    Uint64 size;
    Uint32 shaderStageCount;
    PalShaderStage* shaderStages; /**< Array of shader stages that can access the push constant.*/
} PalPushConstantRange;

/**
 * @struct PalImageCreateInfo
 * @brief Creation parameters for an image.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 width;  /**< Width of the image in pixels.*/
    Uint32 height; /**< Height of the image in pixels.*/
    Uint32 depthOrArraySize;
    Uint32 mipLevelCount;
    PalSampleCount sampleCount;
    PalImageType type;     /**< (eg. PAL_IMAGE_TYPE_2D).*/
    PalFormat format;      /**< Format of the image.*/
    PalImageUsages usages; /**< (eg. PAL_IMAGE_USAGE_COLOR_ATTACHEMENT).*/
} PalImageCreateInfo;

/**
 * @struct PalImageCreateInfo
 * @brief Creation parameters for an image view.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 startMipLevel;
    Uint32 mipLevelCount;
    Uint32 startArrayLayer;
    Uint32 layerArrayCount;
    PalImageViewType type;     /**< (eg. PAL_IMAGE_VIEW_TYPE_2D).*/
    PalImageViewUsages usages; /**< (eg. PAL_IMAGE_VIEW_USAGE_COLOR).*/
} PalImageViewCreateInfo;

/**
 * @struct PalSwapchainCreateInfo
 * @brief Creation parameters for a swapchain.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    bool clipped;
    Uint32 width;                     /**< Width of the swapchain in pixels.*/
    Uint32 height;                    /**< Height of the swapchain in pixels.*/
    Uint32 imageCount;                /**< Number of swapchain images.*/
    Uint32 imageArrayLayerCount;      /**< Set to 1 for default.*/
    PalPresentMode presentMode;       /**< (eg. PAL_PRESENT_MODE_FIFO).*/
    PalCompositeAplha compositeAlpha; /**< (eg. PAL_COMPOSITE_ALPHA_OPAQUE).*/
    PalSwapchainFormat format;        /**< (eg. PAL_SWAPCHAIN_FORMAT_BGRA8_UNORM_SRGB).*/
} PalSwapchainCreateInfo;

/**
 * @struct PalShaderCreateInfo
 * @brief Creation parameters for a shader.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 patchControlPoints; /**< For tessellation shaders. Will be ignored by other stages.*/
    PalShaderStage stage;
    void* bytecode;
    Uint64 bytecodeSize;
} PalShaderCreateInfo;

/**
 * @struct PalBufferCreateInfo
 * @brief Creation parameters for a buffer.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalBufferUsages usages; /**< (eg. PAL_BUFFER_USAGE_STORAGE).*/
    Uint64 size;
} PalBufferCreateInfo;

/**
 * @struct PalAccelerationStructureCreateInfo
 * @brief Creation parameters for an acceleration structure.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalAccelerationStructureType type; /**< (eg. PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL).*/
    PalBuffer* buffer;
    Uint64 offset;
    Uint64 size;
} PalAccelerationStructureCreateInfo;

/**
 * @struct PalDescriptorSetLayoutCreateInfo
 * @brief Creation parameters for a descriptor set layout.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 bindingCount;
    PalDescriptorSetLayoutBinding* bindings;
} PalDescriptorSetLayoutCreateInfo;

/**
 * @struct PalDescriptorPoolCreateInfo
 * @brief Creation parameters for a descriptor pool.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 maxDescriptorSets;
    Uint32 maxDescriptorBindingSizes;
    PalDescriptorPoolBindingSize* bindingSizes;
} PalDescriptorPoolCreateInfo;

/**
 * @struct PalPipelineLayoutCreateInfo
 * @brief Creation parameters for a pipeline layout.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 descriptorSetLayoutCount;
    Uint32 pushConstantRangeCount;
    PalDescriptorSetLayout** descriptorSetLayouts;
    PalPushConstantRange* pushConstantRanges;
} PalPipelineLayoutCreateInfo;

/**
 * @struct PalGraphicsPipelineCreateInfo
 * @brief Creation parameters for a graphics pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 vertexLayoutCount;
    Uint32 colorBlendAttachmentCount;
    Uint32 shaderCount;
    PalPrimitiveTopology topology;
    PalPipelineLayout* pipelineLayout;
    PalShader** shaders;
    PalVertexLayout* vertexLayouts;
    PalColorBlendAttachment* colorBlendAttachments;        /**< Must not be nullptr.*/
    PalRasterizerState* rasterizerState;                   /**< Set to nullptr for default.*/
    PalMultisampleState* multisampleState;                 /**< Set to nullptr for default.*/
    PalDepthStencilState* depthStencilState;               /**< Set to nullptr for default.*/
    PalFragmentShadingRateState* fragmentShadingRateState; /**< Set to nullptr for default.*/
    PalRenderingLayoutInfo* renderingLayout;
} PalGraphicsPipelineCreateInfo;

/**
 * @struct PalComputePipelineCreateInfo
 * @brief Creation parameters for a compute pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalPipelineLayout* pipelineLayout;
    PalShader* computeShader;
} PalComputePipelineCreateInfo;

/**
 * @struct PalRayTracingShaderGroupCreateInfo
 * @brief Creation parameters for a ray tracing pipeline shader group.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalRayTracingShaderGroupType type; /**< (eg. PAL_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL).*/
    Uint32 anyHitShaderIndex;          /**< Index of any hit shader from shader array.*/
    Uint32 closestHitShaderIndex;      /**< Index of closest hit shader from shader array.*/
    Uint32 generalShaderIndex;         /**< Index of general hit shader from shader array.*/
    Uint32 intersectionShaderIndex;    /**< Index of intersection hit shader from shader array.*/
} PalRayTracingShaderGroupCreateInfo;

/**
 * @struct PalRayTracingPipelineCreateInfo
 * @brief Creation parameters for a ray tracing pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 shaderCount;
    Uint32 shaderGroupCount;
    Uint32 maxRecursionDepth;
    PalPipelineLayout* pipelineLayout;
    PalRayTracingShaderGroupCreateInfo* shaderGroups; /**< Array of shader group create info.*/
    PalShader** shaders; /**< Array of shader stage. This is used by `shaderGroups`.*/
} PalRayTracingPipelineCreateInfo;

/**
 * @struct PalGraphicsBackend
 * @brief Dispatch table for PAL graphics system backends.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    /**
     * Backend implementation of ::palEnumerateAdapters.
     *
     * This function must obey the rules and semantics documented in palEnumerateAdapters().
     */
    PalResult PAL_CALL (*enumerateAdapters)(
        Int32* count,
        PalAdapter** outAdapters);

    /**
     * Backend implementation of ::palGetAdapterInfo.
     *
     * This function must obey the rules and semantics documented in palGetAdapterInfo().
     */
    PalResult PAL_CALL (*getAdapterInfo)(
        PalAdapter* adapter,
        PalAdapterInfo* info);

    /**
     * Backend implementation of ::palGetAdapterCapabilities.
     *
     * This function must obey the rules and semantics documented in palGetAdapterCapabilities().
     */
    PalResult PAL_CALL (*getAdapterCapabilities)(
        PalAdapter* adapter,
        PalAdapterCapabilities* caps);

    /**
     * Backend implementation of ::palGetAdapterFeatures.
     *
     * This function must obey the rules and semantics documented in palGetAdapterFeatures().
     */
    PalAdapterFeatures PAL_CALL (*getAdapterFeatures)(PalAdapter* adapter);

    /**
     * Backend implementation of ::palCreateDevice.
     *
     * This function must obey the rules and semantics documented in palCreateDevice().
     */
    PalResult PAL_CALL (*createDevice)(
        PalAdapter* adapter,
        PalAdapterFeatures features,
        PalDevice** outDevice);

    /**
     * Backend implementation of ::palDestroyDevice.
     *
     * This function must obey the rules and semantics documented in palDestroyDevice().
     */
    void PAL_CALL (*destroyDevice)(PalDevice* device);

    /**
     * Backend implementation of ::palWaitDevice.
     *
     * This function must obey the rules and semantics documented in palWaitDevice().
     */
    PalResult PAL_CALL (*waitDevice)(PalDevice* device);

    /**
     * Backend implementation of ::palAllocateMemory.
     *
     * This function must obey the rules and semantics documented in palAllocateMemory().
     */
    PalResult PAL_CALL (*allocateMemory)(
        PalDevice* device,
        PalMemoryType type,
        Uint64 memoryMask,
        Uint64 size,
        PalMemory** outMemory);

    /**
     * Backend implementation of ::palFreeMemory.
     *
     * This function must obey the rules and semantics documented in palFreeMemory().
     */
    void PAL_CALL (*freeMemory)(
        PalDevice* device,
        PalMemory* memory);

    /**
     * Backend implementation of ::palMapMemory.
     *
     * This function must obey the rules and semantics documented in palMapMemory().
     */
    PalResult PAL_CALL (*mapMemory)(
        PalDevice* device,
        PalMemory* memory,
        Uint64 offset,
        Uint64 size,
        void** outPtr);

    /**
     * Backend implementation of ::palUnmapMemory.
     *
     * This function must obey the rules and semantics documented in palUnmapMemory().
     */
    void PAL_CALL (*unmapMemory)(
        PalDevice* device,
        PalMemory* memory);

    /**
     * Backend implementation of ::palQueryDepthStencilCapabilities.
     *
     * This function must obey the rules and semantics documented in
     * palQueryDepthStencilCapabilities().
     */
    PalResult PAL_CALL (*queryDepthStencilCapabilities)(
        PalDevice* device,
        PalDepthStencilCapabilities* caps);

    /**
     * Backend implementation of ::palQueryFragmentShadingRateCapabilities.
     *
     * This function must obey the rules and semantics documented in
     * palQueryFragmentShadingRateCapabilities().
     */
    PalResult PAL_CALL (*queryFragmentShadingRateCapabilities)(
        PalDevice* device,
        PalFragmentShadingRateCapabilities* caps);

    /**
     * Backend implementation of ::palQueryMeshShaderCapabilities.
     *
     * This function must obey the rules and semantics documented in
     * palQueryMeshShaderCapabilities().
     */
    PalResult PAL_CALL (*queryMeshShaderCapabilities)(
        PalDevice* device,
        PalMeshShaderCapabilities* caps);

    /**
     * Backend implementation of ::palQueryRayTracingCapabilities.
     *
     * This function must obey the rules and semantics documented in
     * palQueryRayTracingCapabilities().
     */
    PalResult PAL_CALL (*queryRayTracingCapabilities)(
        PalDevice* device,
        PalRayTracingCapabilities* caps);

    /**
     * Backend implementation of ::palQueryDescriptorIndexingCapabilities.
     *
     * This function must obey the rules and semantics documented in
     * palQueryDescriptorIndexingCapabilities().
     */
    PalResult PAL_CALL (*queryDescriptorIndexingCapabilities)(
        PalDevice* device,
        PalDescriptorIndexingCapabilities* caps);

    /**
     * Backend implementation of ::palCreateQueue.
     *
     * This function must obey the rules and semantics documented in palCreateQueue().
     */
    PalResult PAL_CALL (*createQueue)(
        PalDevice* device,
        PalQueueType type,
        PalQueue** outQueue);

    /**
     * Backend implementation of ::palDestroyQueue.
     *
     * This function must obey the rules and semantics documented in palDestroyQueue().
     */
    void PAL_CALL (*destroyQueue)(PalQueue* queue);

    /**
     * Backend implementation of ::palCanQueuePresent.
     *
     * This function must obey the rules and semantics documented in palCanQueuePresent().
     */
    bool PAL_CALL (*canQueuePresent)(
        PalQueue* queue,
        PalGraphicsWindow* window);

    /**
     * Backend implementation of ::palWaitQueue.
     *
     * This function must obey the rules and semantics documented in palWaitQueue().
     */
    PalResult PAL_CALL (*waitQueue)(PalQueue* queue);

    /**
     * Backend implementation of ::palEnumerateFormats.
     *
     * This function must obey the rules and semantics documented in palEnumerateFormats().
     */
    PalResult PAL_CALL (*enumerateFormats)(
        PalAdapter* adapter,
        Int32* count,
        PalFormatInfo* outFormats);

    /**
     * Backend implementation of ::palIsFormatSupported.
     *
     * This function must obey the rules and semantics documented in palIsFormatSupported().
     */
    bool PAL_CALL (*isFormatSupported)(
        PalAdapter* adapter,
        PalFormat format);

    /**
     * Backend implementation of ::palQueryFormatImageUsages.
     *
     * This function must obey the rules and semantics documented in palQueryFormatImageUsages().
     */
    PalImageUsages PAL_CALL (*queryFormatImageUsages)(
        PalAdapter* adapter,
        PalFormat format);

    /**
     * Backend implementation of ::palQueryFormatImageViewUsages.
     *
     * This function must obey the rules and semantics documented in palQueryFormatImageViewUsages().
     */
    PalImageViewUsages PAL_CALL (*queryFormatImageViewUsages)(
        PalAdapter* adapter,
        PalFormat format);

    /**
     * Backend implementation of ::palCreateImage.
     *
     * This function must obey the rules and semantics documented in palCreateImage().
     */
    PalResult PAL_CALL (*createImage)(
        PalDevice* device,
        const PalImageCreateInfo* info,
        PalImage** outImage);

    /**
     * Backend implementation of ::palDestroyImage.
     *
     * This function must obey the rules and semantics documented in palDestroyImage().
     */
    void PAL_CALL (*destroyImage)(PalImage* image);

    /**
     * Backend implementation of ::palGetImageInfo.
     *
     * This function must obey the rules and semantics documented in palGetImageInfo().
     */
    PalResult PAL_CALL (*getImageInfo)(
        PalImage* image,
        PalImageInfo* info);

    /**
     * Backend implementation of ::palGetImageMemoryRequirements.
     *
     * This function must obey the rules and semantics documented in palGetImageMemoryRequirements().
     */
    PalResult PAL_CALL (*getImageMemoryRequirements)(
        PalImage* image,
        PalMemoryRequirements* requirements);

    /**
     * Backend implementation of ::palBindImageMemory.
     *
     * This function must obey the rules and semantics documented in palBindImageMemory().
     */
    PalResult PAL_CALL (*bindImageMemory)(
        PalImage* image,
        PalMemory* memory,
        Uint64 offset);

    /**
     * Backend implementation of ::palCreateImageView.
     *
     * This function must obey the rules and semantics documented in palCreateImageView().
     */
    PalResult PAL_CALL (*createImageView)(
        PalDevice* device,
        PalImage* image,
        const PalImageViewCreateInfo* info,
        PalImageView** outImageView);

    /**
     * Backend implementation of ::palDestroyImageView.
     *
     * This function must obey the rules and semantics documented in palDestroyImageView().
     */
    void PAL_CALL (*destroyImageView)(PalImageView* imageView);

    /**
     * Backend implementation of ::palQuerySwapchainCapabilities.
     *
     * This function must obey the rules and semantics documented in palQuerySwapchainCapabilities().
     */
    PalResult PAL_CALL (*querySwapchainCapabilities)(
        PalDevice* device,
        PalGraphicsWindow* window,
        PalSwapchainCapabilities* caps);

    /**
     * Backend implementation of ::palCreateSwapchain.
     *
     * This function must obey the rules and semantics documented in palCreateSwapchain().
     */
    PalResult PAL_CALL (*createSwapchain)(
        PalDevice* device,
        PalQueue* queue,
        PalGraphicsWindow* window,
        const PalSwapchainCreateInfo* info,
        PalSwapchain** outSwapchain);

    /**
     * Backend implementation of ::palDestroySwapchain.
     *
     * This function must obey the rules and semantics documented in palDestroySwapchain().
     */
    void PAL_CALL (*destroySwapchain)(PalSwapchain* swapchain);

    /**
     * Backend implementation of ::palGetSwapchainImage.
     *
     * This function must obey the rules and semantics documented in palGetSwapchainImage().
     */
    PalImage* PAL_CALL (*getSwapchainImage)(
        PalSwapchain* swapchain,
        Int32 index);

    /**
     * Backend implementation of ::palGetNextSwapchainImage.
     *
     * This function must obey the rules and semantics documented in palGetNextSwapchainImage().
     */
    PalResult PAL_CALL (*getNextSwapchainImage)(
        PalSwapchain* swapchain,
        PalSwapchainNextImageInfo* info,
        Uint32* outIndex);

    /**
     * Backend implementation of ::palPresentSwapchain.
     *
     * This function must obey the rules and semantics documented in palPresentSwapchain().
     */
    PalResult PAL_CALL (*presentSwapchain)(
        PalSwapchain* swapchain,
        PalSwapchainPresentInfo* info);

    /**
     * Backend implementation of ::palCreateShader.
     *
     * This function must obey the rules and semantics documented in palCreateShader().
     */
    PalResult PAL_CALL (*createShader)(
        PalDevice* device,
        const PalShaderCreateInfo* info,
        PalShader** outShader);

    /**
     * Backend implementation of ::palDestroyShader.
     *
     * This function must obey the rules and semantics documented in palDestroyShader().
     */
    void PAL_CALL (*destroyShader)(PalShader* shader);

    /**
     * Backend implementation of ::palCreateFence.
     *
     * This function must obey the rules and semantics documented in palCreateFence().
     */
    PalResult PAL_CALL (*createFence)(
        PalDevice* device,
        bool signaled,
        PalFence** outFence);

    /**
     * Backend implementation of ::palDestroyFence.
     *
     * This function must obey the rules and semantics documented in palDestroyFence().
     */
    void PAL_CALL (*destroyFence)(PalFence* fence);

    /**
     * Backend implementation of ::palWaitFenceTimeout.
     *
     * This function must obey the rules and semantics documented in palWaitFenceTimeout().
     */
    PalResult PAL_CALL (*waitFenceTimeout)(
        PalFence* fence,
        Uint64 timeout);

    /**
     * Backend implementation of ::palResetFence.
     *
     * This function must obey the rules and semantics documented in palResetFence().
     */
    PalResult PAL_CALL (*resetFence)(PalFence* fence);

    /**
     * Backend implementation of ::palIsFenceSignaled.
     *
     * This function must obey the rules and semantics documented in palIsFenceSignaled().
     */
    bool PAL_CALL (*isFenceSignaled)(PalFence* fence);

    /**
     * Backend implementation of ::palCreateSemaphore.
     *
     * This function must obey the rules and semantics documented in palCreateSemaphore().
     */
    PalResult PAL_CALL (*createSemaphore)(
        PalDevice* device,
        PalSemaphore** outSemaphore);

    /**
     * Backend implementation of ::palDestroySemaphore.
     *
     * This function must obey the rules and semantics documented in palDestroySemaphore().
     */
    void PAL_CALL (*destroySemaphore)(PalSemaphore* semaphore);

    /**
     * Backend implementation of ::palWaitSemaphore.
     *
     * This function must obey the rules and semantics documented in palWaitSemaphore().
     */
    PalResult PAL_CALL (*waitSemaphore)(
        PalSemaphore* semaphore,
        PalQueue* queue,
        Uint64 value,
        Uint64 timeout);

    /**
     * Backend implementation of ::palSignalSemaphore.
     *
     * This function must obey the rules and semantics documented in palSignalSemaphore().
     */
    PalResult PAL_CALL (*signalSemaphore)(
        PalSemaphore* semaphore,
        PalQueue* queue,
        Uint64 value);

    /**
     * Backend implementation of ::palGetSemaphoreValue.
     *
     * This function must obey the rules and semantics documented in palGetSemaphoreValue().
     */
    PalResult PAL_CALL (*getSemaphoreValue)(
        PalSemaphore* semaphore,
        Uint64* value);

    /**
     * Backend implementation of ::palCreateCommandPool.
     *
     * This function must obey the rules and semantics documented in palCreateCommandPool().
     */
    PalResult PAL_CALL (*createCommandPool)(
        PalDevice* device,
        PalQueue* queue,
        PalCommandPool** outPool);

    /**
     * Backend implementation of ::palDestroyCommandPool.
     *
     * This function must obey the rules and semantics documented in palDestroyCommandPool().
     */
    void PAL_CALL (*destroyCommandPool)(PalCommandPool* pool);

    /**
     * Backend implementation of ::palResetCommandPool.
     *
     * This function must obey the rules and semantics documented in palResetCommandPool().
     */
    PalResult PAL_CALL (*resetCommandPool)(PalCommandPool* pool);

    /**
     * Backend implementation of ::palAllocateCommandBuffer.
     *
     * This function must obey the rules and semantics documented in palAllocateCommandBuffer().
     */
    PalResult PAL_CALL (*allocateCommandBuffer)(
        PalDevice* device,
        PalCommandPool* pool,
        PalCommandBufferType type,
        PalCommandBuffer** outCmdBuffer);

    /**
     * Backend implementation of ::palFreeCommandBuffer.
     *
     * This function must obey the rules and semantics documented in palFreeCommandBuffer().
     */
    void PAL_CALL (*freeCommandBuffer)(PalCommandBuffer* cmdBuffer);

    /**
     * Backend implementation of ::palResetCommandBuffer.
     *
     * This function must obey the rules and semantics documented in palResetCommandBuffer().
     */
    PalResult PAL_CALL (*resetCommandBuffer)(PalCommandBuffer* cmdBuffer);

    /**
     * Backend implementation of ::palCmdBegin.
     *
     * This function must obey the rules and semantics documented in palCmdBegin().
     */
    PalResult PAL_CALL (*cmdBegin)(
        PalCommandBuffer* cmdBuffer,
        PalRenderingLayoutInfo* info);

    /**
     * Backend implementation of ::palCmdEnd.
     *
     * This function must obey the rules and semantics documented in palCmdEnd().
     */
    PalResult PAL_CALL (*cmdEnd)(PalCommandBuffer* cmdBuffer);

    /**
     * Backend implementation of ::palCmdExecuteCommandBuffer.
     *
     * This function must obey the rules and semantics documented in palCmdExecuteCommandBuffer().
     */
    PalResult PAL_CALL (*cmdExecuteCommandBuffer)(
        PalCommandBuffer* primaryCmdBuffer,
        PalCommandBuffer* secondaryCmdBuffer);

    /**
     * Backend implementation of ::palCmdSetFragmentShadingRate.
     *
     * This function must obey the rules and semantics documented in palCmdSetFragmentShadingRate().
     */
    PalResult PAL_CALL (*cmdSetFragmentShadingRate)(
        PalCommandBuffer* cmdBuffer,
        PalFragmentShadingRateState* state);

    /**
     * Backend implementation of ::palCmdDrawMeshTasks.
     *
     * This function must obey the rules and semantics documented in palCmdDrawMeshTasks().
     */
    PalResult PAL_CALL (*cmdDrawMeshTasks)(
        PalCommandBuffer* cmdBuffer,
        Uint32 groupCountX,
        Uint32 groupCountY,
        Uint32 groupCountZ);

    /**
     * Backend implementation of ::palCmdDrawMeshTasksIndirect.
     *
     * This function must obey the rules and semantics documented in palCmdDrawMeshTasksIndirect().
     */
    PalResult PAL_CALL (*cmdDrawMeshTasksIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        Uint64 offset,
        Uint32 drawCount,
        Uint32 stride);

    /**
     * Backend implementation of ::palCmdDrawMeshTasksIndirectCount.
     *
     * This function must obey the rules and semantics documented in palCmdDrawMeshTasksIndirectCount().
     */
    PalResult PAL_CALL (*cmdDrawMeshTasksIndirectCount)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalBuffer* countBuffer,
        Uint64 offset,
        Uint64 countBufferOffset,
        Uint32 maxDrawCount,
        Uint32 stride);

    /**
     * Backend implementation of ::palCmdBuildAccelerationStructure.
     *
     * This function must obey the rules and semantics documented in palCmdBuildAccelerationStructure().
     */
    PalResult PAL_CALL (*cmdBuildAccelerationStructure)(
        PalCommandBuffer* cmdBuffer,
        PalAccelerationStructureBuildInfo* info);

    /**
     * Backend implementation of ::palCmdBeginRendering.
     *
     * This function must obey the rules and semantics documented in palCmdBeginRendering().
     */
    PalResult PAL_CALL (*cmdBeginRendering)(
        PalCommandBuffer* cmdBuffer,
        PalRenderingInfo* info);

    /**
     * Backend implementation of ::palCmdEndRendering.
     *
     * This function must obey the rules and semantics documented in palCmdEndRendering().
     */
    PalResult PAL_CALL (*cmdEndRendering)(PalCommandBuffer* cmdBuffer);

    /**
     * Backend implementation of ::palCmdCopyBuffer.
     *
     * This function must obey the rules and semantics documented in palCmdCopyBuffer().
     */
    PalResult PAL_CALL (*cmdCopyBuffer)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* dst,
        PalBuffer* src,
        Uint64 dstOffset,
        Uint64 srcOffset,
        Uint32 size);

    /**
     * Backend implementation of ::palCmdBindPipeline.
     *
     * This function must obey the rules and semantics documented in palCmdBindPipeline().
     */
    PalResult PAL_CALL (*cmdBindPipeline)(
        PalCommandBuffer* cmdBuffer,
        PalPipeline* pipeline);

    /**
     * Backend implementation of ::palCmdSetViewport.
     *
     * This function must obey the rules and semantics documented in palCmdSetViewport().
     */
    PalResult PAL_CALL (*cmdSetViewport)(
        PalCommandBuffer* cmdBuffer,
        Uint32 count,
        PalViewport* viewports);

    /**
     * Backend implementation of ::palCmdSetScissors.
     *
     * This function must obey the rules and semantics documented in palCmdSetScissors().
     */
    PalResult PAL_CALL (*cmdSetScissors)(
        PalCommandBuffer* cmdBuffer,
        Uint32 count,
        PalRect2D* scissors);

    /**
     * Backend implementation of ::palCmdBindVertexBuffers.
     *
     * This function must obey the rules and semantics documented in palCmdBindVertexBuffers().
     */
    PalResult PAL_CALL (*cmdBindVertexBuffers)(
        PalCommandBuffer* cmdBuffer,
        Uint32 firstSlot,
        Uint32 count,
        PalBuffer** buffers,
        Uint64* offsets);

    /**
     * Backend implementation of ::palCmdBindIndexBuffer.
     *
     * This function must obey the rules and semantics documented in palCmdBindIndexBuffer().
     */
    PalResult PAL_CALL (*cmdBindIndexBuffer)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        Uint64 offset,
        PalIndexType type);

    /**
     * Backend implementation of ::palCmdDraw.
     *
     * This function must obey the rules and semantics documented in palCmdDraw().
     */
    PalResult PAL_CALL (*cmdDraw)(
        PalCommandBuffer* cmdBuffer,
        PalDrawData* data);

    /**
     * Backend implementation of ::palCmdDrawIndirect.
     *
     * This function must obey the rules and semantics documented in palCmdDrawIndirect().
     */
    PalResult PAL_CALL (*cmdDrawIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        Uint64 offset,
        Uint32 count);

    /**
     * Backend implementation of ::palCmdDrawIndirectCount.
     *
     * This function must obey the rules and semantics documented in palCmdDrawIndirectCount().
     */
    PalResult PAL_CALL (*cmdDrawIndirectCount)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalBuffer* countBuffer,
        Uint64 offset,
        Uint64 countBufferOffset,
        Uint32 count);

    /**
     * Backend implementation of ::palCmdDrawIndexed.
     *
     * This function must obey the rules and semantics documented in palCmdDrawIndexed().
     */
    PalResult PAL_CALL (*cmdDrawIndexed)(
        PalCommandBuffer* cmdBuffer,
        PalDrawIndexedData* data);

    /**
     * Backend implementation of ::palCmdDrawIndexedIndirect.
     *
     * This function must obey the rules and semantics documented in palCmdDrawIndexedIndirect().
     */
    PalResult PAL_CALL (*cmdDrawIndexedIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        Uint64 offset,
        Uint32 count);

    /**
     * Backend implementation of ::palCmdDrawIndexedIndirectCount.
     *
     * This function must obey the rules and semantics documented in palCmdDrawIndexedIndirectCount().
     */
    PalResult PAL_CALL (*cmdDrawIndexedIndirectCount)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalBuffer* countBuffer,
        Uint64 offset,
        Uint64 countBufferOffset,
        Uint32 count);

    /**
     * Backend implementation of ::palCmdMemoryBarrier.
     *
     * This function must obey the rules and semantics documented in palCmdMemoryBarrier().
     */
    PalResult PAL_CALL (*cmdMemoryBarrier)(
        PalCommandBuffer* cmdBuffer,
        PalUsageStateInfo* oldUsageStateInfo,
        PalUsageStateInfo* newUsageStateInfo);

    /**
     * Backend implementation of ::palCmdImageViewBarrier.
     *
     * This function must obey the rules and semantics documented in palCmdImageViewBarrier().
     */
    PalResult PAL_CALL (*cmdImageViewBarrier)(
        PalCommandBuffer* cmdBuffer,
        PalImageView* imageView,
        PalUsageStateInfo* oldUsageStateInfo,
        PalUsageStateInfo* newUsageStateInfo);

    /**
     * Backend implementation of ::palCmdBufferBarrier.
     *
     * This function must obey the rules and semantics documented in palCmdBufferBarrier().
     */
    PalResult PAL_CALL (*cmdBufferBarrier)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalUsageStateInfo* oldUsageStateInfo,
        PalUsageStateInfo* newUsageStateInfo);

    /**
     * Backend implementation of ::palCmdDispatch.
     *
     * This function must obey the rules and semantics documented in palCmdDispatch().
     */
    PalResult PAL_CALL (*cmdDispatch)(
        PalCommandBuffer* cmdBuffer,
        Uint32 groupCountX,
        Uint32 groupCountY,
        Uint32 groupCountZ);

    /**
     * Backend implementation of ::palCmdDispatchBase.
     *
     * This function must obey the rules and semantics documented in palCmdDispatchBase().
     */
    PalResult PAL_CALL (*cmdDispatchBase)(
        PalCommandBuffer* cmdBuffer,
        Uint32 baseGroupX,
        Uint32 baseGroupY,
        Uint32 baseGroupZ,
        Uint32 groupCountX,
        Uint32 groupCountY,
        Uint32 groupCountZ);

    /**
     * Backend implementation of ::palCmdDispatchIndirect.
     *
     * This function must obey the rules and semantics documented in palCmdDispatchIndirect().
     */
    PalResult PAL_CALL (*cmdDispatchIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        Uint64 offset);

    /**
     * Backend implementation of ::palCmdTraceRays.
     *
     * This function must obey the rules and semantics documented in palCmdTraceRays().
     */
    PalResult PAL_CALL (*cmdTraceRays)(
        PalCommandBuffer* cmdBuffer,
        Uint32 width,
        Uint32 height,
        Uint32 depth);

    /**
     * Backend implementation of ::palCmdTraceRaysIndirect.
     *
     * This function must obey the rules and semantics documented in palCmdTraceRaysIndirect().
     */
    PalResult PAL_CALL (*cmdTraceRaysIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalDeviceAddress bufferAddress);

    /**
     * Backend implementation of ::palCmdBindDescriptorSet.
     *
     * This function must obey the rules and semantics documented in palCmdBindDescriptorSet().
     */
    PalResult PAL_CALL (*cmdBindDescriptorSet)(
        PalCommandBuffer* cmdBuffer,
        PalPipeline* pipeline,
        PalPipelineLayout* layout,
        Uint32 setIndex,
        PalDescriptorSet* set);

    /**
     * Backend implementation of ::palCmdPushConstants.
     *
     * This function must obey the rules and semantics documented in palCmdPushConstants().
     */
    PalResult PAL_CALL (*cmdPushConstants)(
        PalCommandBuffer* cmdBuffer,
        PalPipelineLayout* layout,
        Uint32 shaderStageCount,
        PalShaderStage* shaderStages,
        Uint32 offset,
        Uint32 size,
        const void* value);

    /**
     * Backend implementation of ::palSubmitCommandBuffer.
     *
     * This function must obey the rules and semantics documented in palSubmitCommandBuffer().
     */
    PalResult PAL_CALL (*submitCommandBuffer)(
        PalQueue* queue,
        PalCommandBufferSubmitInfo* info);

    /**
     * Backend implementation of ::palCreateAccelerationstructure.
     *
     * This function must obey the rules and semantics documented in palCreateAccelerationstructure().
     */
    PalResult PAL_CALL (*createAccelerationstructure)(
        PalDevice* device,
        const PalAccelerationStructureCreateInfo* info,
        PalAccelerationStructure** outAs);

    /**
     * Backend implementation of ::palDestroyAccelerationstructure.
     *
     * This function must obey the rules and semantics documented in palDestroyAccelerationstructure().
     */
    void PAL_CALL (*destroyAccelerationstructure)(PalAccelerationStructure* as);

    /**
     * Backend implementation of ::palGetAccelerationStructureBuildSize.
     *
     * This function must obey the rules and semantics documented in palGetAccelerationStructureBuildSize().
     */
    PalResult PAL_CALL (*getAccelerationStructureBuildSize)(
        PalDevice* device,
        PalAccelerationStructureBuildInfo* info,
        PalAccelerationStructureBuildSize* size);

    /**
     * Backend implementation of ::palCreateBuffer.
     *
     * This function must obey the rules and semantics documented in palCreateBuffer().
     */
    PalResult PAL_CALL (*createBuffer)(
        PalDevice* device,
        const PalBufferCreateInfo* info,
        PalBuffer** outBuffer);

    /**
     * Backend implementation of ::palDestroyBuffer.
     *
     * This function must obey the rules and semantics documented in palDestroyBuffer().
     */
    void PAL_CALL (*destroyBuffer)(PalBuffer* buffer);

    /**
     * Backend implementation of ::palGetBufferMemoryRequirements.
     *
     * This function must obey the rules and semantics documented in palGetBufferMemoryRequirements().
     */
    PalResult PAL_CALL (*getBufferMemoryRequirements)(
        PalBuffer* buffer,
        PalMemoryRequirements* requirements);

    /**
     * Backend implementation of ::palComputeInstanceBufferRequirements.
     *
     * This function must obey the rules and semantics documented in palComputeInstanceBufferRequirements().
     */
    PalResult PAL_CALL (*computeInstanceBufferRequirements)(
        PalDevice* device,
        PalInstanceBufferRequirements* requirements,
        Uint32 instanceCount);

    /**
     * Backend implementation of ::palWriteInstancesToMappedMemory.
     *
     * This function must obey the rules and semantics documented in palWriteInstancesToMappedMemory().
     */
    PalResult PAL_CALL (*writeInstancesToMappedMemory)(
        PalDevice* device,
        void* ptr,
        PalAccelerationStructureInstance* instances,
        Uint32 instanceCount);

    /**
     * Backend implementation of ::palBindBufferMemory.
     *
     * This function must obey the rules and semantics documented in palBindBufferMemory().
     */
    PalResult PAL_CALL (*bindBufferMemory)(
        PalBuffer* buffer,
        PalMemory* memory,
        Uint64 offset);

    /**
     * Backend implementation of ::palGetBufferDeviceAddress.
     *
     * This function must obey the rules and semantics documented in palGetBufferDeviceAddress().
     */
    PalDeviceAddress PAL_CALL (*getBufferDeviceAddress)(PalBuffer* buffer);

    /**
     * Backend implementation of ::palCreateDescriptorSetLayout.
     *
     * This function must obey the rules and semantics documented in palCreateDescriptorSetLayout().
     */
    PalResult PAL_CALL (*createDescriptorSetLayout)(
        PalDevice* device,
        const PalDescriptorSetLayoutCreateInfo* info,
        PalDescriptorSetLayout** outLayout);

    /**
     * Backend implementation of ::palDestroyDescriptorSetLayout.
     *
     * This function must obey the rules and semantics documented in palDestroyDescriptorSetLayout().
     */
    void PAL_CALL (*destroyDescriptorSetLayout)(PalDescriptorSetLayout* layout);

    /**
     * Backend implementation of ::palCreateDescriptorPool.
     *
     * This function must obey the rules and semantics documented in palCreateDescriptorPool().
     */
    PalResult PAL_CALL (*createDescriptorPool)(
        PalDevice* device,
        const PalDescriptorPoolCreateInfo* info,
        PalDescriptorPool** outPool);

    /**
     * Backend implementation of ::palDestroyDescriptorPool.
     *
     * This function must obey the rules and semantics documented in palDestroyDescriptorPool().
     */
    void PAL_CALL (*destroyDescriptorPool)(PalDescriptorPool* pool);

    /**
     * Backend implementation of ::palResetDescriptorPool.
     *
     * This function must obey the rules and semantics documented in palResetDescriptorPool().
     */
    PalResult PAL_CALL (*resetDescriptorPool)(PalDescriptorPool* pool);

    /**
     * Backend implementation of ::palAllocateDescriptorSet.
     *
     * This function must obey the rules and semantics documented in palAllocateDescriptorSet().
     */
    PalResult PAL_CALL (*allocateDescriptorSet)(
        PalDevice* device,
        PalDescriptorPool* pool,
        PalDescriptorSetLayout* layout,
        PalDescriptorSet** outSet);

    /**
     * Backend implementation of ::palUpdateDescriptorSet.
     *
     * This function must obey the rules and semantics documented in palUpdateDescriptorSet().
     */
    PalResult PAL_CALL (*updateDescriptorSet)(
        PalDevice* device,
        Uint32 count,
        PalDescriptorSetWriteInfo* infos);

    /**
     * Backend implementation of ::palCreatePipelineLayout.
     *
     * This function must obey the rules and semantics documented in palCreatePipelineLayout().
     */
    PalResult PAL_CALL (*createPipelineLayout)(
        PalDevice* device,
        const PalPipelineLayoutCreateInfo* info,
        PalPipelineLayout** outLayout);

    /**
     * Backend implementation of ::palDestroyPipelineLayout.
     *
     * This function must obey the rules and semantics documented in palDestroyPipelineLayout().
     */
    void PAL_CALL (*destroyPipelineLayout)(PalPipelineLayout* layout);

    /**
     * Backend implementation of ::palCreateGraphicsPipeline.
     *
     * This function must obey the rules and semantics documented in palCreateGraphicsPipeline().
     */
    PalResult PAL_CALL (*createGraphicsPipeline)(
        PalDevice* device,
        const PalGraphicsPipelineCreateInfo* info,
        PalPipeline** outPipeline);

    /**
     * Backend implementation of ::palCreateComputePipeline.
     *
     * This function must obey the rules and semantics documented in palCreateComputePipeline().
     */
    PalResult PAL_CALL (*createComputePipeline)(
        PalDevice* device,
        const PalComputePipelineCreateInfo* info,
        PalPipeline** outPipeline);

    /**
     * Backend implementation of ::palCreateRayTracingPipeline.
     *
     * This function must obey the rules and semantics documented in palCreateRayTracingPipeline().
     */
    PalResult PAL_CALL (*createRayTracingPipeline)(
        PalDevice* device,
        const PalRayTracingPipelineCreateInfo* info,
        PalPipeline** outPipeline);

    /**
     * Backend implementation of ::palDestroyPipeline.
     *
     * This function must obey the rules and semantics documented in palDestroyPipeline().
     */
    void PAL_CALL (*destroyPipeline)(PalPipeline* pipeline);
} PalGraphicsBackend;

/**
 * @brief Add a custom graphics backend to the graphics system.
 * 
 * The graphics system must not be initialized before this call. If already initialized, 
 * the system should be shutdown and re-initialized after this call. 
 * The graphics system supports 16 custom backends.
 * 
 * The `backend` dispatch table must have its function pointers all set even if a 
 * function will not be used. If a feature is not supported by the backend, 
 * `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED` must be returned by the appropriate function.
 * If any of the function pointers are not set, this function will fail and 
 * `PAL_RESULT_INVALID_BACKEND` will be returned.
 * 
 * The backend will not not copied, therefore the pointer must remain valid
 * until the graphics system is shutdown.
 *
 * @param[in] backend Pointer to the backend dispatch table to add.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palInitGraphics
 * @sa palShutdownGraphics
 */
PAL_API PalResult PAL_CALL palAddGraphicsBackend(const PalGraphicsBackend* backend);

/**
 * @brief Initialize the graphics system.
 * 
 * Any custom backends added with palAddGraphicsBackend() will be registered with the 
 * graphics system. The graphics system must be shutdown with palShutdownGraphics() when no longer needed.
 * 
 * The debugger and allocator will not not copied, therefore the pointers must remain valid
 * until the graphics system is shutdown. Set the debugger or PalGraphicsDebugger::callback to
 * nullptr to disable debugging and validation layers.
 *
 * @param[in] debugger Optional debugger. Set to nullptr to disable debugging and validation 
 * layers.
 * @param[in] allocator Optional user-provided allocator. Set to nullptr to use default.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 * 
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palAddGraphicsBackend
 * @sa palShutdownGraphics
 */
PAL_API PalResult PAL_CALL palInitGraphics(
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator);

/**
 * @brief Shutdown the graphics system.
 * 
 * If the graphics system has not been initialized, the function returns silently.
 * All created devices, queues, images, swapchains etc must be destroyed before this call.
 * 
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palInitGraphics
 */
PAL_API void PAL_CALL palShutdownGraphics();

/**
 * @brief Return a list of all adapters (GPU) from custom and internal backends.
 * 
 * The graphics system must be initialized before this call.
 * 
 * If a custom backend which implements an adapter with its API type of Vulkan, and there is an 
 * adapter from the internal backends with the same specifications, this function will return both
 * of them in the list. Use PalAdapterInfo::backendName to differentiate between custom and 
 * internal backend. the backend name for internal backend is `PAL`.
 * 
 * Call this function first with PalAdapter array set to nullptr to get the number of adapters. 
 * Allocate memory for the PalAdapter array and passed in the count and the allocated array. If 
 * the count of the array is less than the number of adapters, PAL will write upto that limit.
 * 
 * If the count is 0 and the PalAdapter array is nullptr, the function fails
 * and returns `PAL_RESULT_INSUFFICIENT_BUFFER`.
 *
 * The adapter handles must not be freed by the user, they are managed by the
 * graphics system. Users are required to cache this, and call this function again
 * if adapters are added or removed which is rare except for virtual ones.
 *
 * @param[in] count Capacity of the PalAdapter array.
 * @param[out] outAdapters User allocated array of PalAdapter.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 * 
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palEnumerateAdapters(
    Int32* count,
    PalAdapter** outAdapters);

/**
 * @brief Get information about an adapter (GPU).
 * 
 * The graphics system must be initialized before this call.
 *
 * @param[in] adapter Adapter to query information on.
 * @param[out] info Pointer to a PalAdapterInfo to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 * 
 * Thread safety: This function is thread safe if `info` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palEnumerateAdapters
 */
PAL_API PalResult PAL_CALL palGetAdapterInfo(
    PalAdapter* adapter,
    PalAdapterInfo* info);

/**
 * @brief Get capabilites or limits about an adapter (GPU).
 * 
 * The graphics system must be initialized before this call.
 *
 * @param[in] adapter Adapter to query capabilities on.
 * @param[out] caps Pointer to a PalAdapterCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 * 
 * Thread safety: This function is thread safe if `caps` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palEnumerateAdapters
 */
PAL_API PalResult PAL_CALL palGetAdapterCapabilities(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps);

/**
 * @brief Get the supported features of an adapter (GPU).
 * 
 * The graphics system must be initialized before this call.
 *
 * @param[in] adapter Adapter to query features on.
 *
 * @return adapter features on success or `0` on failure.
 * 
 * Thread safety: This function is thread safe.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palEnumerateAdapters
 */
PAL_API PalAdapterFeatures PAL_CALL palGetAdapterFeatures(PalAdapter* adapter);

/**
 * @brief Create a device from an adapter (GPU).
 * 
 * The graphics system must be initialized before this call. PAL does not enable any features
 * implicitly not even common ones like `PAL_ADAPTER_FEATURE_SWAPCHAIN`.
 * 
 * Every requested feature must be supported by the adapter. Use palGetAdapterFeatures to check
 * the supported features of the adapter that can be enabled. Using a feature which is not 
 * supported will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] adapter Adapter to create the device with.
 * @param[in] features Adapter features to enable. Must be supported.
 * @param[out] outDevice Pointer to a PalDevice to recieve the created device. Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 * 
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroyDevice
 */
PAL_API PalResult PAL_CALL palCreateDevice(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice);

/**
 * @brief Destroy a device.
 * 
 * The graphics system must be initialized before this call.
 * If the provided device is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] device Pointer to the device to destroy.
 * 
 * Thread safety: This function must only be called from the main thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateDevice
 */
PAL_API void PAL_CALL palDestroyDevice(PalDevice* device);

/**
 * @brief Blocks indefinitely untill the device becomes idle.
 * 
 * The graphics system must be initialized before this call.
 * 
 * This function blocks indefinitely untill all submitted work on the device has been completetd.
 * Returns `PAL_RESULT_SUCCESS` to indicate all pending operations has been completetd.
 *
 * @param[in] device Pointer to device to wait.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 * 
 * Thread safety: This function is thread safe if `device` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palWaitDevice(PalDevice* device);

PAL_API PalResult PAL_CALL palAllocateMemory(
    PalDevice* device,
    PalMemoryType type,
    Uint64 memoryMask,
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

PAL_API PalResult PAL_CALL palQueryDescriptorIndexingCapabilities(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps);

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
    Uint32* outIndex);

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

PAL_API PalResult PAL_CALL palAllocateCommandBuffer(
    PalDevice* device,
    PalCommandPool* pool,
    PalCommandBufferType type,
    PalCommandBuffer** outCmdbuffer);

PAL_API void PAL_CALL palFreeCommandBuffer(PalCommandBuffer* cmdBuffer);

PAL_API PalResult PAL_CALL palResetCommandBuffer(PalCommandBuffer* cmdBuffer);

PAL_API PalResult PAL_CALL palCmdBegin(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info);

PAL_API PalResult PAL_CALL palCmdEnd(PalCommandBuffer* cmdBuffer);

PAL_API PalResult PAL_CALL palCmdExecuteCommandBuffer(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer);

PAL_API PalResult PAL_CALL palCmdSetFragmentShadingRate(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state);

PAL_API PalResult PAL_CALL palCmdDrawMeshTasks(
    PalCommandBuffer* cmdBuffer,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ);

PAL_API PalResult PAL_CALL palCmdDrawMeshTasksIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 drawCount,
    Uint32 stride);

PAL_API PalResult PAL_CALL palCmdDrawMeshTasksIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 maxDrawCount,
    Uint32 stride);

PAL_API PalResult PAL_CALL palCmdBuildAccelerationStructure(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info);

PAL_API PalResult PAL_CALL palCmdBeginRendering(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info);

PAL_API PalResult PAL_CALL palCmdEndRendering(PalCommandBuffer* cmdBuffer);

PAL_API PalResult PAL_CALL palCmdCopyBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    Uint64 dstOffset,
    Uint64 srcOffset,
    Uint32 size);

PAL_API PalResult PAL_CALL palCmdBindPipeline(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline);

PAL_API PalResult PAL_CALL palCmdSetViewport(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalViewport* viewports);

PAL_API PalResult PAL_CALL palCmdSetScissors(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalRect2D* scissors);

PAL_API PalResult PAL_CALL palCmdBindVertexBuffers(
    PalCommandBuffer* cmdBuffer,
    Uint32 firstSlot,
    Uint32 count,
    PalBuffer** buffers,
    Uint64* offsets);

PAL_API PalResult PAL_CALL palCmdBindIndexBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    PalIndexType type);

PAL_API PalResult PAL_CALL palCmdDraw(
    PalCommandBuffer* cmdBuffer,
    PalDrawData* data);

PAL_API PalResult PAL_CALL palCmdDrawIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 count);

PAL_API PalResult PAL_CALL palCmdDrawIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 count);

PAL_API PalResult PAL_CALL palCmdDrawIndexed(
    PalCommandBuffer* cmdBuffer,
    PalDrawIndexedData* data);

PAL_API PalResult PAL_CALL palCmdDrawIndexedIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 count);

PAL_API PalResult PAL_CALL palCmdDrawIndexedIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 count);

PAL_API PalResult PAL_CALL palCmdMemoryBarrier(
    PalCommandBuffer* cmdBuffer,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo);

PAL_API PalResult PAL_CALL palCmdImageViewBarrier(
    PalCommandBuffer* cmdBuffer,
    PalImageView* imageView,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo);

PAL_API PalResult PAL_CALL palCmdBufferBarrier(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo);

PAL_API PalResult PAL_CALL palCmdDispatch(
    PalCommandBuffer* cmdBuffer,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ);

PAL_API PalResult PAL_CALL palCmdDispatchBase(
    PalCommandBuffer* cmdBuffer,
    Uint32 baseGroupX,
    Uint32 baseGroupY,
    Uint32 baseGroupZ,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ);

PAL_API PalResult PAL_CALL palCmdDispatchIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset);

PAL_API PalResult PAL_CALL palCmdTraceRays(
    PalCommandBuffer* cmdBuffer,
    Uint32 width,
    Uint32 height,
    Uint32 depth);

PAL_API PalResult PAL_CALL palCmdTraceRaysIndirect(
    PalCommandBuffer* cmdBuffer,
    PalDeviceAddress bufferAddress);

PAL_API PalResult PAL_CALL palCmdBindDescriptorSet(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline,
    PalPipelineLayout* layout,
    Uint32 setIndex,
    PalDescriptorSet* set);

PAL_API PalResult PAL_CALL palCmdPushConstants(
    PalCommandBuffer* cmdBuffer,
    PalPipelineLayout* layout,
    Uint32 shaderStageCount,
    PalShaderStage* shaderStages,
    Uint32 offset,
    Uint32 size,
    const void* value);

PAL_API PalResult PAL_CALL palSubmitCommandBuffer(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info);

PAL_API PalResult PAL_CALL palCreateAccelerationstructure(
    PalDevice* device,
    const PalAccelerationStructureCreateInfo* info,
    PalAccelerationStructure** outAs);

PAL_API void PAL_CALL palDestroyAccelerationstructure(PalAccelerationStructure* as);

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

PAL_API PalResult PAL_CALL palComputeInstanceBufferRequirements(
    PalDevice* device,
    PalInstanceBufferRequirements* requirements,
    Uint32 instanceCount);

PAL_API PalResult PAL_CALL palWriteInstancesToMappedMemory(
    PalDevice* device,
    void* ptr,
    PalAccelerationStructureInstance* instances,
    Uint32 instanceCount);

PAL_API PalResult PAL_CALL palBindBufferMemory(
    PalBuffer* buffer,
    PalMemory* memory,
    Uint64 offset);

PAL_API PalDeviceAddress PAL_CALL palGetBufferDeviceAddress(PalBuffer* buffer);

PAL_API PalResult PAL_CALL palCreateDescriptorSetLayout(
    PalDevice* device,
    const PalDescriptorSetLayoutCreateInfo* info,
    PalDescriptorSetLayout** outLayout);

PAL_API void PAL_CALL palDestroyDescriptorSetLayout(PalDescriptorSetLayout* layout);

PAL_API PalResult PAL_CALL palCreateDescriptorPool(
    PalDevice* device,
    const PalDescriptorPoolCreateInfo* info,
    PalDescriptorPool** outPool);

PAL_API void PAL_CALL palDestroyDescriptorPool(PalDescriptorPool* pool);

PAL_API PalResult PAL_CALL palResetDescriptorPool(PalDescriptorPool* pool);

PAL_API PalResult PAL_CALL palAllocateDescriptorSet(
    PalDevice* device,
    PalDescriptorPool* pool,
    PalDescriptorSetLayout* layout,
    PalDescriptorSet** outSet);

PAL_API PalResult PAL_CALL palUpdateDescriptorSet(
    PalDevice* device,
    Uint32 count,
    PalDescriptorSetWriteInfo* infos);

PAL_API PalResult PAL_CALL palCreatePipelineLayout(
    PalDevice* device,
    const PalPipelineLayoutCreateInfo* info,
    PalPipelineLayout** outLayout);

PAL_API void PAL_CALL palDestroyPipelineLayout(PalPipelineLayout* layout);

PAL_API PalResult PAL_CALL palCreateGraphicsPipeline(
    PalDevice* device,
    const PalGraphicsPipelineCreateInfo* info,
    PalPipeline** outPipeline);

PAL_API PalResult PAL_CALL palCreateComputePipeline(
    PalDevice* device,
    const PalComputePipelineCreateInfo* info,
    PalPipeline** outPipeline);

PAL_API PalResult PAL_CALL palCreateRayTracingPipeline(
    PalDevice* device,
    const PalRayTracingPipelineCreateInfo* info,
    PalPipeline** outPipeline);

PAL_API void PAL_CALL palDestroyPipeline(PalPipeline* pipeline);

PAL_API bool PAL_CALL palBuildWorkGroupInfo(
    const PalWorkGroupBuildData* data,
    Int32* count,
    PalWorkGroupInfo* info);

/** @} */ // end of pal_graphics group

#endif // _PAL_GRAPHICS_H
