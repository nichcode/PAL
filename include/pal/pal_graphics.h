
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
 * @brief The maximum name size of a shader entry.
 * @since 1.4
 * @ingroup pal_graphics
 */
#define PAL_SHADER_ENTRY_NAME_SIZE 32

#define PAL_MAX_RESOLVE_MODES 8
#define PAL_MAX_COMBINER_OPS 8

/**
 * @brief A Unused shader index. Used to make a shader index invalid.
 * @since 1.4
 * @ingroup pal_graphics
 */
#define PAL_UNUSED_SHADER_INDEX UINT32_MAX

/**
 * @brief An encoding scheme for shader format target versions.
 * @since 1.4
 * @ingroup pal_graphics
 */
#define PAL_MAKE_SHADER_TARGET(major, minor) ((Uint32)((major) << 8) | (minor))

/**
 * @brief Get the major of an encoded shader target.
 * @since 1.4
 * @ingroup pal_graphics
 */
#define PAL_SHADER_TARGET_MAJOR(target) ((Uint32)(target) >> 8);

/**
 * @brief Get the minor of an encoded shader target.
 * @since 1.4
 * @ingroup pal_graphics
 */
#define PAL_SHADER_TARGET_MINOR(target) ((Uint32)(target) & 0xFF);

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
typedef Uint64 PalAdapterFeatures;

#define PAL_ADAPTER_FEATURE_NONE 0;
#define PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY PAL_BIT64(1)
#define PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING PAL_BIT64(2)
#define PAL_ADAPTER_FEATURE_MULTI_VIEWPORT PAL_BIT64(3)
#define PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE PAL_BIT64(4)
#define PAL_ADAPTER_FEATURE_TESSELLATION_SHADER PAL_BIT64(5)
#define PAL_ADAPTER_FEATURE_GEOMETRY_SHADER PAL_BIT64(6)
#define PAL_ADAPTER_FEATURE_SHADER_FLOAT16 PAL_BIT64(7)
#define PAL_ADAPTER_FEATURE_SHADER_FLOAT64 PAL_BIT64(8)
#define PAL_ADAPTER_FEATURE_SHADER_INT16 PAL_BIT64(9)
#define PAL_ADAPTER_FEATURE_SHADER_INT64 PAL_BIT64(10)
#define PAL_ADAPTER_FEATURE_RAY_TRACING PAL_BIT64(11)
#define PAL_ADAPTER_FEATURE_MESH_SHADER PAL_BIT64(12)
#define PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE PAL_BIT64(13)
#define PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING PAL_BIT64(14)
#define PAL_ADAPTER_FEATURE_SWAPCHAIN PAL_BIT64(15)
#define PAL_ADAPTER_FEATURE_MULTI_VIEW PAL_BIT64(16)
#define PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY PAL_BIT64(17)
#define PAL_ADAPTER_FEATURE_FENCE_RESET PAL_BIT64(18)
#define PAL_ADAPTER_FEATURE_POLYGON_MODE_LINE PAL_BIT64(19)
#define PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE PAL_BIT64(20)
#define PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE PAL_BIT64(21)
#define PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY PAL_BIT64(22)
#define PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE PAL_BIT64(23)
#define PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE PAL_BIT64(24)
#define PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP PAL_BIT64(25)
#define PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE PAL_BIT64(26)
#define PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT PAL_BIT64(27)
#define PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS PAL_BIT64(28)
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW PAL_BIT64(29)
#define PAL_ADAPTER_FEATURE_INDIRECT_DISPATCH PAL_BIT64(30)
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT PAL_BIT64(31)
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH PAL_BIT64(32)
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT PAL_BIT64(33)
#define PAL_ADAPTER_FEATURE_DISPATCH_BASE PAL_BIT64(34)

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
 * @struct PalSurface
 * @brief Opaque handle to a surface.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalSurface PalSurface;

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
 * This defines the layout, ordering and the number of descriptors a descriptor set uses.
 * 
 * The layouts should reflect the exact layout of the shaders. Eg. 
 * descriptorBindings[2] = { sampler, sampled image } is different from 
 * descriptorBindings[2] = { sampled image, sampler }. The ordering must be correct.
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
 * @struct PalShaderBindingTable
 * @brief Opaque handle to a shader binding table.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct PalShaderBindingTable PalShaderBindingTable;

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
    PAL_FORMAT_D16_UNORM_S8_UINT,
    PAL_FORMAT_D32_SFLOAT_S8_UINT,
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
 * @enum PalImageAspect
 * @brief Image aspects.
 *
 * All image aspect follow the format `PAL_IMAGE_ASPECT_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_IMAGE_ASPECT_COLOR,
    PAL_IMAGE_ASPECT_DEPTH,
    PAL_IMAGE_ASPECT_STENCIL,
    PAL_IMAGE_ASPECT_DEPTH_STENCIL
} PalImageAspect;

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
 * @enum PalFilterMode
 * @brief Filter modes.
 *
 * All filter modes follow the format `PAL_FILTER_MODE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_FILTER_MODE_NEAREST,
    PAL_FILTER_MODE_LINEAR
} PalFilterMode;

/**
 * @enum PalSamplerMipmapMode
 * @brief Sampler mipmap modes.
 *
 * All sampler mipmap modes follow the format `PAL_SAMPLER_MIPMAP_MODE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_SAMPLER_MIPMAP_MODE_NEAREST,
    PAL_SAMPLER_MIPMAP_MODE_LINEAR
} PalSamplerMipmapMode;

/**
 * @enum PalSamplerAddressMode
 * @brief Sampler address modes.
 *
 * All sampler address modes follow the format `PAL_SAMPLER_ADDRESS_MODE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_SAMPLER_ADDRESS_MODE_REPEAT,
    PAL_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
    PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
} PalSamplerAddressMode;

/**
 * @enum PalBorderColor
 * @brief Border color.
 *
 * All border colors follow the format `PAL_BORDER_COLOR_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
    PAL_BORDER_COLOR_INT_TRANSPARENT_BLACK,
    PAL_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
    PAL_BORDER_COLOR_INT_OPAQUE_BLACK,
    PAL_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
    PAL_BORDER_COLOR_INT_OPAQUE_WHITE
} PalBorderColor;

/**
 * @enum PalSurfaceFormat
 * @brief Surface format types.
 *
 * All surface format types follow the format `PAL_SURFACE_FORMAT_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR,
    PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR,
    PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR,
    PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10, /**< HDR.*/

    PAL_SURFACE_FORMAT_MAX
} PalSurfaceFormat;

/**
 * @enum PalGraphicsWindowDisplayType
 * @brief Display types for a graphics window.
 *
 * All graphics window display types follow the format `PAL_GRAPHICS_WINDOW_DISPLAY_TYPE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_GRAPHICS_WINDOW_DISPLAY_TYPE_WAYLAND,
    PAL_GRAPHICS_WINDOW_DISPLAY_TYPE_X11,
    PAL_GRAPHICS_WINDOW_DISPLAY_TYPE_XCB
} PalGraphicsWindowDisplayType;

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
 * @enum PalStencilFaceFlags
 * @brief Stencil face flags. Multiple stencil face flags can be OR'ed together using bitwise
 * OR operator (`|`).
 *
 * All tencil face flags follow the format `PAL_STENCIL_FACE_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_STENCIL_FACE_FRONT = PAL_BIT(0),
    PAL_STENCIL_FACE_BACK = PAL_BIT(1),
    PAL_STENCIL_FACE_BOTH = PAL_STENCIL_FACE_FRONT | PAL_STENCIL_FACE_BACK
} PalStencilFaceFlags;

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
 * @enum PalVertexSemanticID
 * @brief Vertex semantic id types.
 *
 * All vertex semantic id types follow the format `PAL_VERTEX_SEMANTIC_ID_**` for
 * consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_VERTEX_SEMANTIC_ID_POSITION,
    PAL_VERTEX_SEMANTIC_ID_COLOR,
    PAL_VERTEX_SEMANTIC_ID_TEXCOORD,
    PAL_VERTEX_SEMANTIC_ID_NORMAL,
    PAL_VERTEX_SEMANTIC_ID_TANGENT   
} PalVertexSemanticID;

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
 * @enum PalAccelerationStructureBuildMode
 * @brief Acceleration structure build modes.
 *
 * All acceleration structure build modes follow the format 
 * `PAL_ACCELERATION_STRUCTURE_BUILD_MODE_**` for consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_ACCELERATION_STRUCTURE_BUILD_MODE_BUILD,
    PAL_ACCELERATION_STRUCTURE_BUILD_MODE_UPDATE
} PalAccelerationStructureBuildMode;

/**
 * @enum PalAccelerationStructureBuildHints
 * @brief Acceleration structure build hints. Multiple hints can be OR'ed together using 
 * bitwise OR operator (`|`). Hints can be ignored by the driver.
 *
 * All acceleration structure build hints follow the format 
 * `PAL_ACCELERATION_STRUCTURE_BUILD_HINT_**` for consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_BUILD = PAL_BIT(0),
    PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_TRACE = PAL_BIT(1),
    PAL_ACCELERATION_STRUCTURE_BUILD_HINT_LOW_MEMORY = PAL_BIT(2)
} PalAccelerationStructureBuildHints;

/**
 * @enum PalAccelerationStructureInstanceFlags
 * @brief Acceleration structure instance flags. Multiple flags can be OR'ed together using 
 * bitwise OR operator (`|`). Not all combinations are valid.
 *
 * All acceleration structure instance flags follow the format 
 * `PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_**` for consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_OPAQUE = PAL_BIT(0),
    PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_NO_OPAQUE = PAL_BIT(1),
    PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FACING_CULL_DISABLE = PAL_BIT(2),
    PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE = PAL_BIT(3)
} PalAccelerationStructureInstanceFlags;

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
    PAL_GEOMETRY_TYPE_AABBS
} PalGeometryType;

/**
 * @enum PalGeometryFlags
 * @brief Geometry flags. Multiple flags can be OR'ed together using 
 * bitwise OR operator (`|`). Not all combinations are valid.
 *
 * All geometry flags follow the format `PAL_GEOMETRY_FLAG_**` for consistency and API use.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef enum {
    PAL_GEOMETRY_FLAG_OPAQUE = PAL_BIT(0),
    PAL_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT = PAL_BIT(1)
} PalGeometryFlags;

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
    PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_SCRATCH = PAL_BIT(7),
    PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_READ_ONLY_INPUT = PAL_BIT(8),
    PAL_BUFFER_USAGE_DEVICE_ADDRESS = PAL_BIT(9),
    PAL_BUFFER_USAGE_INDIRECT = PAL_BIT(10)
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
    PAL_USAGE_STATE_INDIRECT_READ,
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
    char name[PAL_ADAPTER_NAME_SIZE];
    char backendName[PAL_ADAPTER_NAME_SIZE]; /**< Adapter backend name (eg. `PAL`, `Custom`).*/
} PalAdapterInfo;

/**
 * @struct PalImageCapabilities
 * @brief Image capabilities of an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 maxWidth;
    Uint32 maxHeight;
    Uint32 maxDepth;
    Uint32 maxArrayLayers;
    Uint32 maxMipLevels;
} PalImageCapabilities;

/**
 * @struct PalResourceCapabilities
 * @brief Resource capabilities of an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    bool sampledImageDynamicArrayIndexing;
    bool storageImageDynamicArrayIndexing;
    bool storageBufferDynamicArrayIndexing;
    bool uniformBufferDynamicArrayIndexing;
    Uint32 maxPerStageSampledImages;
    Uint32 maxPerSetSampledImages;
    Uint32 maxPerStageStorageImages;
    Uint32 maxPerSetStorageImages;
    Uint32 maxPerStageSamplers;
    Uint32 maxPerSetSamplers;
    Uint32 maxPerStageStorageBuffers;
    Uint32 maxPerSetStorageBuffers;
    Uint32 maxPerStageUniformBuffers;
    Uint32 maxPerSetUniformBuffers;
    Uint32 maxPerStageAccelerationStructure;
    Uint32 maxPerSetAccelerationStructure;
    Uint32 maxBoundSets;
} PalResourceCapabilities;

/**
 * @struct PalComputeCapabilities
 * @brief Compute capabilities of an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 maxWorkGroupInvocations;
    Uint32 maxWorkGroupCount[3];
    Uint32 maxWorkGroupSize[3];
} PalComputeCapabilities;

/**
 * @struct PalViewportCapabilities
 * @brief Viewport capabilities of an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 maxWidth;
    Uint32 maxHeight;
    float minBoundsRange;
    float maxBoundsRange;
} PalViewportCapabilities;

/**
 * @struct PalAdapterCapabilities
 * @brief Capabilities of an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 maxComputeQueues;
    Uint32 maxGraphicsQueues;
    Uint32 maxCopyQueues;
    Uint32 maxColorAttachments;
    Uint32 maxUniformBufferSize;
    Uint32 maxStorageBufferSize;
    Uint32 maxPushConstantSize;
    Uint32 maxVertexLayouts;
    Uint32 maxVertexAttributes;
    Uint32 maxTessellationPatchPoint;
    PalViewportCapabilities viewportCaps;
    PalImageCapabilities imageCaps;
    PalResourceCapabilities resourceCaps;
    PalComputeCapabilities computeCaps;
} PalAdapterCapabilities;

/**
 * @struct PalSamplerAnisotropyCapabilities
 * @brief Sampler anisotropy capabilities of an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 maxAnisotropy;
} PalSamplerAnisotropyCapabilities;

/**
 * @struct PalMultiViewCapabilities
 * @brief Multi view capabilities of an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 maxViewCount;
} PalMultiViewCapabilities;

/**
 * @struct PalMultiViewportCapabilities
 * @brief Multi viewport capabilities of an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 maxCount;
} PalMultiViewportCapabilities;

/**
 * @struct PalDepthStencilCapabilities
 * @brief Depth stencil capabilities of an adapter (GPU).
 * 
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    bool independentResolve;
    bool depthResolves[PAL_MAX_RESOLVE_MODES];
    bool stencilResolves[PAL_MAX_RESOLVE_MODES];
} PalDepthStencilCapabilities;

/**
 * @struct PalFragmentShadingRateCapabilities
 * @brief Fragment shading rate capabilities of an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    bool shadingRates[PAL_FRAGMENT_SHADING_RATE_MAX];
    Uint32 minTexelWidth;
    Uint32 minTexelHeight;
    Uint32 maxTexelWidth;
    Uint32 maxTexelHeight;
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
    Uint32 maxOutputPrimitives;
    Uint32 maxOutputVertices;    
    Uint32 maxWorkGroupInvocations;
    Uint32 maxTaskWorkGroupInvocations;
    Uint32 maxWorkGroupCount[3];
    Uint32 maxTaskWorkGroupCount[3];
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
    Uint32 maxHitAttributeSize;
    Uint32 maxInstanceCount;
    Uint32 maxPrimitiveCount;
    Uint32 maxGeometryCount;
    Uint32 maxPayloadSize;
    Uint32 maxDispatchInvocations;
} PalRayTracingCapabilities;

/**
 * @struct PalDescriptorIndexingCapabilities
 * @brief Descriptor indexing capabilities of an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    bool sampledImageNonUniformIndexing;
    bool sampledImageUpdateAfterBind;
    bool storageImageNonUniformIndexing;
    bool storageImageUpdateAfterBind;
    bool storageBufferNonUniformIndexing;
    bool storageBufferUpdateAfterBind;
    bool uniformBufferNonUniformIndexing;
    bool uniformBufferUpdateAfterBind;
    Uint32 maxPerStageSampledImages;
    Uint32 maxPerSetSampledImages;
    Uint32 maxPerStageStorageImages;
    Uint32 maxPerSetStorageImages;
    Uint32 maxPerStageSamplers;
    Uint32 maxPerSetSamplers;
    Uint32 maxPerStageStorageBuffers;
    Uint32 maxPerSetStorageBuffers;
    Uint32 maxPerStageUniformBuffers;
    Uint32 maxPerSetUniformBuffers;
    Uint32 maxPerStageAccelerationStructure;
    Uint32 maxPerSetAccelerationStructure;
} PalDescriptorIndexingCapabilities;

/**
 * @struct PalSurfaceCapabilities
 * @brief surface capabilities of an adapter (GPU).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    bool presentModes[PAL_PRESENT_MODE_MAX];
    bool compositeAlphas[PAL_COMPOSITE_ALPHA_MAX];
    bool formats[PAL_SURFACE_FORMAT_MAX];
    Uint32 minImageCount;
    Uint32 maxImageCount;
    Uint32 minImageWidth;
    Uint32 minImageHeight;
    Uint32 maxImageWidth;
    Uint32 maxImageHeight;
    Uint32 maxImageArrayLayers;
} PalSurfaceCapabilities;

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
    PalGraphicsWindowDisplayType displayType; /**< Will be used only on linux platform.*/
    void* display; /**< Can be nullptr depending on platform (eg. Windows).*/
    void* window;  /**< Must not be nullptr.*/
} PalGraphicsWindow;

/**
 * @struct PalFormatInfo
 * @brief Information about a format. This includes the supported image usages and maximum sample
 * count from the provided format.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalFormat format;
    PalImageUsages usages;
    PalSampleCount sampleCount; /**< Maximum supported multisample count.*/
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
    float color[4]; /**< Color for color attachments only.*/
    float depth;
    Uint32 stencil;
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
    PalLoadOp stencilLoadOp;
    PalStoreOp stencilStoreOp;
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
    Uint32 shaderStageCount;
    PalUsageState usageState;
    PalShaderStage* shaderStages;
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
    bool memoryTypes[PAL_MEMORY_TYPE_MAX];
    Uint64 memoryMask;
    Uint64 size;
    Uint64 alignment;
} PalMemoryRequirements;

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
    Uint64 waitValue;
    Uint64 signalValue;
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
    Uint64 signalValue;
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
    Uint64 waitValue;
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
    Uint32 colorAttachentCount;
    PalAttachmentDesc* colorAttachments;
    PalAttachmentDesc* depthStencilAttachment;
    PalAttachmentDesc* fragmentShadingRateAttachment;
} PalRenderingInfo;

/**
 * @struct PalRenderingLayoutInfo
 * @brief Information about a pre-existing PalRenderingInfo.
 * This is used to reference an already existing PalRenderingInfo.
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
    PalFormat depthStencilAttachmentFormat;
    PalFormat fragmentShadingRateAttachmentFormat;
    PalFormat* colorAttachmentsFormat;
} PalRenderingLayoutInfo;

/**
 * @struct PalWorkGroupBuildData
 * @brief Compute or Mesh(or Task) workgroup input data build helper.
 *
 * Uninitialized fields may result in undefined behavior. `workCount` can be specified in pixels,
 * vertices etc.Eg. an image of 800 x 600 will be [0] = 800, [1] = 600 and [2] = 1.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
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
 * @struct PalDrawIndirectData
 * @brief Draw indirect data of a single draw call.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 vertexCount;
    Uint32 instanceCount;
    Uint32 firstVertex;
    Uint32 firstInstance;
} PalDrawIndirectData;

/**
 * @struct PalDrawIndexedIndirectData
 * @brief Draw indexed indirect data of a single draw call.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 indexCount;
    Uint32 instanceCount;
    Uint32 firstIndex;
    Int32 vertexOffset;
    Uint32 firstInstance;
} PalDrawIndexedIndirectData;

/**
 * @struct PalDispatchIndirectData
 * @brief Draw or dispatch indirect data of a single dispatch call.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 groupCountXOrWidth;  /**< Number of groups on the x axis or width.*/
    Uint32 groupCountXOrHeight; /**< Number of groups on the y axis or height.*/
    Uint32 groupCountXOrDepth;  /**< Number of groups on the z axis or depth.*/
} PalDispatchIndirectData;

/**
 * @struct PalVertexAttribute
 * @brief Vertex attribute.
 *
 * Uninitialized fields may result in undefined behavior. `semanticName` Must not include the index
 * (eg. "position" or "myown"). Set to nullptr to use the default that will be derived from `
 * semanticID` which are (`POSITION`, `COLOR`, `TEXCOORD`, `NORMAL` and `TANGENT`).
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalVertexSemanticID semanticID;
    PalVertexType type;
    const char* semanticName;
} PalVertexAttribute;

/**
 * @struct PalVertexLayout
 * @brief Vertex layout.
 * 
 * This defines the layout, ordering and the number of vertex attributes the layout uses.
 * 
 * The layouts should reflect the exact layout of the shaders. Eg. 
 * attributes[2] = { position, color } is different from 
 * attributes[2] = { color, position }. The ordering must be correct.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalVertexLayoutType type;
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
    bool denyGeneral;
    bool denyValidation;
    bool denyPerformance;
    bool denyInfoSeverity;
    bool denyWarningSeverity;
    bool denyErrorSeverity;
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
    float depthBiasConstant;
    float depthBiasSlope;
    float depthBiasClamp;
    PalPolygonMode polygonMode;
    PalCullMode cullMode;
    PalFrontFace frontFace;
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
    PalFragmentShadingRate rate;
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
    Uint32 instanceId;
    Uint32 mask;
    Uint32 hitGroupOffset;
    PalAccelerationStructureInstanceFlags flags;
    PalAccelerationStructure* blas;
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
    Uint32 accelerationStructureSize;
    Uint32 scratchBufferSize;         
    Uint32 updateScratchBufferSize;   
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
    PalVertexType vertexType;
    PalIndexType indexType;
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
 * @struct PalGeometry
 * @brief Acceleration structure geometry.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalGeometryFlags flags;
    Uint32 primitiveCount;
    PalGeometryType type;
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
    PalAccelerationStructureType type;
    Uint32 geometryCount;
    Uint32 instanceCount;
    PalAccelerationStructureBuildHints buildHints;
    PalAccelerationStructureBuildMode buildMode;
    PalDeviceAddress scratchBufferAddress;
    PalDeviceAddress instanceBufferAddress;
    PalAccelerationStructure* dst;
    PalAccelerationStructure* src;
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
    Uint32 descriptorCount;
    PalDescriptorType descriptorType;
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
    Uint32 bindingCount;
    PalDescriptorType descriptorType;
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
    Uint32 stride; /**< For structured buffers. Will be ignored if not supported. 0 for default.*/
    Uint64 offset; /**< Offset in bytes. If structured, will be divided by `stride`.*/
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
    PalImageView* imageView;
} PalDescriptorImageViewInfo;

/**
 * @struct PalDescriptorSamplerInfo
 * @brief Information about a sampler descriptor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalSampler* sampler;
} PalDescriptorSamplerInfo;

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
    Uint32 layoutBindingIndex; /**< Index into the descriptor set layout bindings.*/
    Uint32 arrayElement;
    Uint32 descriptorCount; 
    PalDescriptorType descriptorType;
    PalDescriptorSet* descriptorSet;
    PalDescriptorBufferInfo* bufferInfos;
    PalDescriptorImageViewInfo* imageViewInfos;
    PalDescriptorSamplerInfo* samplerInfos;
    PalDescriptorTLASInfo* tlasInfos;
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
    PalShaderStage* shaderStages;
} PalPushConstantRange;

/**
 * @struct PalImageSubresourceRange
 * @brief Subresource range for images and image views.
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
    PalImageAspect aspect; /**< Must be compatible with the image format.*/
} PalImageSubresourceRange;

/**
 * @struct PalBufferCopyInfo
 * @brief Information for buffer to buffer copies.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 size;
    Uint64 dstOffset;
    Uint64 srcOffset;
} PalBufferCopyInfo;

/**
 * @struct PalBufferImageCopyInfo
 * @brief Information for image to buffer and vice versa copies.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint64 bufferOffset;
    Uint32 bufferRowLength;
    Uint32 bufferImageHeight;
    Uint32 ImageMipLevel;
    Uint32 ImageStartArrayLayer;
    Uint32 ImageArrayLayerCount;
    Int32 imageOffsetX;
    Int32 imageOffsetY;
    Int32 imageOffsetZ;
    Uint32 imageWidth;
    Uint32 imageHeight;
    Uint32 imageDepth;
    PalImageAspect imageAspect;
} PalBufferImageCopyInfo;

/**
 * @struct PalImageCopyInfo
 * @brief Information for image to image copies.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 dstMipLevel;
    Uint32 srcMipLevel;
    Uint32 dstStartArrayLayer;
    Uint32 srcStartArrayLayer;
    Uint32 arrayLayerCount;
    Int32 dstOffsetX;
    Int32 srcOffsetX;
    Int32 dstOffsetY;
    Int32 srcOffsetY;
    Int32 dstOffsetZ;
    Int32 srcOffsetZ;
    Uint32 width;
    Uint32 height;
    Uint32 depth;
    PalImageAspect aspect;
} PalImageCopyInfo;

/**
 * @struct PalImageCopyInfo
 * @brief Information for image to image copies.
 *
 * Uninitialized fields may result in undefined behavior.
 * 
 * The records array must be in this order [raygen][miss][hitgroup][callable]. 
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 groupIndex; /**< Index into the shader groups used to create the ray tracing pipeline.*/
    Uint32 localDataSize; /**< Must not be greater than the data size of the group.*/
    void* localData;
} PalShaderBindingTableRecordInfo;

/**
 * @struct PalShaderEntryInfo
 * @brief Entry information of a shader.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 patchControlPoints; /**< For tessellation shaders. Will be ignored by other stages.*/
    PalShaderStage stage;
    const char* entryName;
} PalShaderEntryInfo;

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
    Uint32 width;
    Uint32 height;
    Uint32 depthOrArraySize;
    Uint32 mipLevelCount;
    PalSampleCount sampleCount;
    PalImageType type;
    PalFormat format;
    PalImageUsages usages;
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
    PalFormat format; /**< Must be compatible with the image format.*/
    PalImageViewType type;
    PalImageSubresourceRange subresourceRange;
} PalImageViewCreateInfo;

/**
 * @struct PalSamplerCreateInfo
 * @brief Creation parameters for a sampler.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    bool enableCompare;
    bool enableAnisotropy; /**< `PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY` must be supported.*/
    float mipLodBias;
    float minLod;
    float maxLod;
    float maxAnisotropy;
    PalFilterMode minFilterMode;
    PalFilterMode magFilterMode;
    PalSamplerMipmapMode mipmapMode;
    PalSamplerAddressMode addressModeU;
    PalSamplerAddressMode addressModeV;
    PalSamplerAddressMode addressModeW;
    PalCompareOp compareOp;
    PalBorderColor borderColor;
} PalSamplerCreateInfo;

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
    Uint32 width;
    Uint32 height;
    Uint32 imageCount;
    Uint32 imageArrayLayerCount;
    PalPresentMode presentMode;
    PalCompositeAplha compositeAlpha;
    PalSurfaceFormat format;
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
    Uint32 entryCount;
    Uint64 bytecodeSize;
    void* bytecode;
    PalShaderEntryInfo* entries;
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
    PalBufferUsages usages;
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
    PalAccelerationStructureType type;
    PalBuffer* buffer;
    Uint64 offset;
    Uint64 size;
} PalAccelerationStructureCreateInfo;

/**
 * @struct PalDescriptorSetLayoutCreateInfo
 * @brief Creation parameters for a descriptor set layout.
 *
 * Uninitialized fields may result in undefined behavior. Set `enableDescriptorIndexing` to
 * true to enable descriptor indexing for the descriptor set layout.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    bool enableDescriptorIndexing;
    Uint32 bindingCount;
    Uint32 shaderStageCount;
    PalShaderStage* shaderStages;
    PalDescriptorSetLayoutBinding* bindings;
} PalDescriptorSetLayoutCreateInfo;

/**
 * @struct PalDescriptorPoolCreateInfo
 * @brief Creation parameters for a descriptor pool.
 * 
 * Uninitialized fields may result in undefined behavior. Set `enableDescriptorIndexing` to
 * true to enable descriptor indexing for the descriptor pool.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    bool enableDescriptorIndexing;
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
    bool primitiveRestartEnable;
    Uint32 vertexLayoutCount;
    Uint32 colorBlendAttachmentCount;
    Uint32 shaderCount;
    PalIndexType indexType;  /**< Will be used if `primitiveRestartEnable` is true.*/
    PalPrimitiveTopology topology;
    PalPipelineLayout* pipelineLayout;
    PalShader** shaders;
    PalVertexLayout* vertexLayouts;
    PalColorBlendAttachment* colorBlendAttachments;
    PalRasterizerState* rasterizerState;
    PalMultisampleState* multisampleState;
    PalDepthStencilState* depthStencilState;
    PalFragmentShadingRateState* fragmentShadingRateState;
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
 * The shader group array must be in this order [raygen][miss][hitgroup][callable]. 
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    PalRayTracingShaderGroupType type;
    Uint32 anyHitShaderIndex;
    Uint32 anyHitShaderEntryIndex;
    Uint32 closestHitShaderIndex;
    Uint32 closestHitShaderEntryIndex;
    Uint32 generalShaderIndex;
    Uint32 generalShaderEntryIndex;
    Uint32 intersectionShaderIndex;
    Uint32 intersectionShaderEntryIndex;
    Uint32 maxDataSize;    /**< Size of extra data associated with the shader group.*/
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
    Uint32 maxAttributeSize;
    Uint32 maxPayloadSize;
    PalPipelineLayout* pipelineLayout;
    PalRayTracingShaderGroupCreateInfo* shaderGroups;
    PalShader** shaders;
} PalRayTracingPipelineCreateInfo;

/**
 * @struct PalShaderBindingTableCreateInfo
 * @brief Creation parameters for a shader binding table.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
typedef struct {
    Uint32 recordCount;
    PalShaderBindingTableRecordInfo* records;
    PalPipeline* rayTracingPipeline;
} PalShaderBindingTableCreateInfo;

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
     * Must obey the rules and semantics documented in palEnumerateAdapters().
     */
    PalResult (PAL_CALL *enumerateAdapters)(
        Int32* count,
        PalAdapter** outAdapters);

    /**
     * Backend implementation of ::palGetAdapterInfo.
     *
     * Must obey the rules and semantics documented in palGetAdapterInfo().
     */
    PalResult (PAL_CALL *getAdapterInfo)(
        PalAdapter* adapter,
        PalAdapterInfo* info);

    /**
     * Backend implementation of ::palGetAdapterCapabilities.
     *
     * Must obey the rules and semantics documented in palGetAdapterCapabilities().
     */
    PalResult (PAL_CALL *getAdapterCapabilities)(
        PalAdapter* adapter,
        PalAdapterCapabilities* caps);

    /**
     * Backend implementation of ::palGetAdapterFeatures.
     *
     * Must obey the rules and semantics documented in palGetAdapterFeatures().
     */
    PalAdapterFeatures (PAL_CALL *getAdapterFeatures)(PalAdapter* adapter);

    /**
     * Backend implementation of ::palGetHighestSupportedShaderTarget.
     *
     * Must obey the rules and semantics documented in palGetHighestSupportedShaderTarget().
     */
    Uint32 (PAL_CALL *getHighestSupportedShaderTarget)(
        PalAdapter* adapter, 
        PalShaderFormats shaderFormat);

    /**
     * Backend implementation of ::palCreateDevice.
     *
     * Must obey the rules and semantics documented in palCreateDevice().
     */
    PalResult (PAL_CALL *createDevice)(
        PalAdapter* adapter,
        PalAdapterFeatures features,
        PalDevice** outDevice);

    /**
     * Backend implementation of ::palDestroyDevice.
     *
     * Must obey the rules and semantics documented in palDestroyDevice().
     */
    void (PAL_CALL *destroyDevice)(PalDevice* device);

    /**
     * Backend implementation of ::palAllocateMemory.
     *
     * Must obey the rules and semantics documented in palAllocateMemory().
     */
    PalResult (PAL_CALL *allocateMemory)(
        PalDevice* device,
        PalMemoryType type,
        Uint64 memoryMask,
        Uint64 size,
        PalMemory** outMemory);

    /**
     * Backend implementation of ::palFreeMemory.
     *
     * Must obey the rules and semantics documented in palFreeMemory().
     */
    void (PAL_CALL *freeMemory)(
        PalDevice* device,
        PalMemory* memory);

    /**
     * Backend implementation of ::palQuerySamplerAnisotropyCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQuerySamplerAnisotropyCapabilities().
     */
    PalResult (PAL_CALL *querySamplerAnisotropyCapabilities)(
        PalDevice* device,
        PalSamplerAnisotropyCapabilities* caps);

    /**
     * Backend implementation of ::palQueryMultiViewCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryMultiViewCapabilities().
     */
    PalResult (PAL_CALL *queryMultiViewCapabilities)(
        PalDevice* device,
        PalMultiViewCapabilities* caps);

    /**
     * Backend implementation of ::palQueryMultiViewportCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryMultiViewportCapabilities().
     */
    PalResult (PAL_CALL *queryMultiViewportCapabilities)(
        PalDevice* device,
        PalMultiViewportCapabilities* caps);

    /**
     * Backend implementation of ::palQueryDepthStencilCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryDepthStencilCapabilities().
     */
    PalResult (PAL_CALL *queryDepthStencilCapabilities)(
        PalDevice* device,
        PalDepthStencilCapabilities* caps);

    /**
     * Backend implementation of ::palQueryFragmentShadingRateCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryFragmentShadingRateCapabilities().
     */
    PalResult (PAL_CALL *queryFragmentShadingRateCapabilities)(
        PalDevice* device,
        PalFragmentShadingRateCapabilities* caps);

    /**
     * Backend implementation of ::palQueryMeshShaderCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryMeshShaderCapabilities().
     */
    PalResult (PAL_CALL *queryMeshShaderCapabilities)(
        PalDevice* device,
        PalMeshShaderCapabilities* caps);

    /**
     * Backend implementation of ::palQueryRayTracingCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryRayTracingCapabilities().
     */
    PalResult (PAL_CALL *queryRayTracingCapabilities)(
        PalDevice* device,
        PalRayTracingCapabilities* caps);

    /**
     * Backend implementation of ::palQueryDescriptorIndexingCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryDescriptorIndexingCapabilities().
     */
    PalResult (PAL_CALL *queryDescriptorIndexingCapabilities)(
        PalDevice* device,
        PalDescriptorIndexingCapabilities* caps);

    /**
     * Backend implementation of ::palCreateQueue.
     *
     * Must obey the rules and semantics documented in palCreateQueue().
     */
    PalResult (PAL_CALL *createQueue)(
        PalDevice* device,
        PalQueueType type,
        PalQueue** outQueue);

    /**
     * Backend implementation of ::palDestroyQueue.
     *
     * Must obey the rules and semantics documented in palDestroyQueue().
     */
    void (PAL_CALL *destroyQueue)(PalQueue* queue);

    /**
     * Backend implementation of ::palCanQueuePresent.
     *
     * Must obey the rules and semantics documented in palCanQueuePresent().
     */
    bool (PAL_CALL *canQueuePresent)(
        PalQueue* queue,
        PalSurface* surface);

    /**
     * Backend implementation of ::palWaitQueue.
     *
     * Must obey the rules and semantics documented in palWaitQueue().
     */
    PalResult (PAL_CALL *waitQueue)(PalQueue* queue);

    /**
     * Backend implementation of ::palEnumerateFormats.
     *
     * Must obey the rules and semantics documented in palEnumerateFormats().
     */
    PalResult (PAL_CALL *enumerateFormats)(
        PalAdapter* adapter,
        Int32* count,
        PalFormatInfo* outFormats);

    /**
     * Backend implementation of ::palIsFormatSupported.
     *
     * Must obey the rules and semantics documented in palIsFormatSupported().
     */
    bool (PAL_CALL *isFormatSupported)(
        PalAdapter* adapter,
        PalFormat format);

    /**
     * Backend implementation of ::palQueryFormatImageUsages.
     *
     * Must obey the rules and semantics documented in palQueryFormatImageUsages().
     */
    PalImageUsages (PAL_CALL *queryFormatImageUsages)(
        PalAdapter* adapter,
        PalFormat format);

    /**
     * Backend implementation of ::palQueryFormatSampleCount.
     *
     * Must obey the rules and semantics documented in palQueryFormatSampleCount().
     */
    PalSampleCount (PAL_CALL *queryFormatSampleCount)(
        PalAdapter* adapter,
        PalFormat format);

    /**
     * Backend implementation of ::palCreateImage.
     *
     * Must obey the rules and semantics documented in palCreateImage().
     */
    PalResult (PAL_CALL *createImage)(
        PalDevice* device,
        const PalImageCreateInfo* info,
        PalImage** outImage);

    /**
     * Backend implementation of ::palDestroyImage.
     *
     * Must obey the rules and semantics documented in palDestroyImage().
     */
    void (PAL_CALL *destroyImage)(PalImage* image);

    /**
     * Backend implementation of ::palGetImageInfo.
     *
     * Must obey the rules and semantics documented in palGetImageInfo().
     */
    PalResult (PAL_CALL *getImageInfo)(
        PalImage* image,
        PalImageInfo* info);

    /**
     * Backend implementation of ::palGetImageMemoryRequirements.
     *
     * Must obey the rules and semantics documented in palGetImageMemoryRequirements().
     */
    PalResult (PAL_CALL *getImageMemoryRequirements)(
        PalImage* image,
        PalMemoryRequirements* requirements);

    /**
     * Backend implementation of ::palBindImageMemory.
     *
     * Must obey the rules and semantics documented in palBindImageMemory().
     */
    PalResult (PAL_CALL *bindImageMemory)(
        PalImage* image,
        PalMemory* memory,
        Uint64 offset);

    /**
     * Backend implementation of ::palMapImageMemory.
     *
     * Must obey the rules and semantics documented in palMapImageMemory().
     */
    PalResult (PAL_CALL *mapImageMemory)(
        PalImage* image,
        Uint64 offset,
        Uint64 size,
        void** outPtr);

    /**
     * Backend implementation of ::palUnmapImageMemory.
     *
     * Must obey the rules and semantics documented in palUnmapImageMemory().
     */
    void (PAL_CALL *unmapImageMemory)(PalImage* image);

    /**
     * Backend implementation of ::palCreateImageView.
     *
     * Must obey the rules and semantics documented in palCreateImageView().
     */
    PalResult (PAL_CALL *createImageView)(
        PalDevice* device,
        PalImage* image,
        const PalImageViewCreateInfo* info,
        PalImageView** outImageView);

    /**
     * Backend implementation of ::palDestroyImageView.
     *
     * Must obey the rules and semantics documented in palDestroyImageView().
     */
    void (PAL_CALL *destroyImageView)(PalImageView* imageView);

    /**
     * Backend implementation of ::palCreateSampler.
     *
     * Must obey the rules and semantics documented in palCreateSampler().
     */
    PalResult (PAL_CALL *createSampler)(
        PalDevice* device,
        const PalSamplerCreateInfo* info,
        PalSampler** outSampler);

    /**
     * Backend implementation of ::palDestroySampler.
     *
     * Must obey the rules and semantics documented in palDestroySampler().
     */
    void (PAL_CALL *destroySampler)(PalSampler* sampler);

    /**
     * Backend implementation of ::palCreateSurface.
     *
     * Must obey the rules and semantics documented in palCreateSurface().
     */
    PalResult (PAL_CALL *createSurface)(
        PalDevice* device,
        PalGraphicsWindow* window,
        PalSurface** outSurface);

    /**
     * Backend implementation of ::palDestroySurface.
     *
     * Must obey the rules and semantics documented in palDestroySurface().
     */
    void (PAL_CALL *destroySurface)(PalSurface* surface);

    /**
     * Backend implementation of ::palGetSurfaceCapabilities.
     *
     * Must obey the rules and semantics documented in palGetSurfaceCapabilities().
     */
    PalResult (PAL_CALL *getSurfaceCapabilities)(
        PalDevice* device,
        PalSurface* surface,
        PalSurfaceCapabilities* caps);

    /**
     * Backend implementation of ::palCreateSwapchain.
     *
     * Must obey the rules and semantics documented in palCreateSwapchain().
     */
    PalResult (PAL_CALL *createSwapchain)(
        PalDevice* device,
        PalQueue* queue,
        PalSurface* surface,
        const PalSwapchainCreateInfo* info,
        PalSwapchain** outSwapchain);

    /**
     * Backend implementation of ::palDestroySwapchain.
     *
     * Must obey the rules and semantics documented in palDestroySwapchain().
     */
    void (PAL_CALL *destroySwapchain)(PalSwapchain* swapchain);

    /**
     * Backend implementation of ::palGetSwapchainImage.
     *
     * Must obey the rules and semantics documented in palGetSwapchainImage().
     */
    PalImage* (PAL_CALL *getSwapchainImage)(
        PalSwapchain* swapchain,
        Int32 index);

    /**
     * Backend implementation of ::palGetNextSwapchainImage.
     *
     * Must obey the rules and semantics documented in palGetNextSwapchainImage().
     */
    PalResult (PAL_CALL *getNextSwapchainImage)(
        PalSwapchain* swapchain,
        PalSwapchainNextImageInfo* info,
        Uint32* outIndex);

    /**
     * Backend implementation of ::palPresentSwapchain.
     *
     * Must obey the rules and semantics documented in palPresentSwapchain().
     */
    PalResult (PAL_CALL *presentSwapchain)(
        PalSwapchain* swapchain,
        PalSwapchainPresentInfo* info);

    /**
     * Backend implementation of ::palResizeSwapchain.
     *
     * Must obey the rules and semantics documented in palResizeSwapchain().
     */
    PalResult (PAL_CALL *resizeSwapchain)(
        PalSwapchain* swapchain,
        Uint32 newWidth,
        Uint32 newHeight);

    /**
     * Backend implementation of ::palCreateShader.
     *
     * Must obey the rules and semantics documented in palCreateShader().
     */
    PalResult (PAL_CALL *createShader)(
        PalDevice* device,
        const PalShaderCreateInfo* info,
        PalShader** outShader);

    /**
     * Backend implementation of ::palDestroyShader.
     *
     * Must obey the rules and semantics documented in palDestroyShader().
     */
    void (PAL_CALL *destroyShader)(PalShader* shader);

    /**
     * Backend implementation of ::palCreateFence.
     *
     * Must obey the rules and semantics documented in palCreateFence().
     */
    PalResult (PAL_CALL *createFence)(
        PalDevice* device,
        bool signaled,
        PalFence** outFence);

    /**
     * Backend implementation of ::palDestroyFence.
     *
     * Must obey the rules and semantics documented in palDestroyFence().
     */
    void (PAL_CALL *destroyFence)(PalFence* fence);

    /**
     * Backend implementation of ::palWaitFence.
     *
     * Must obey the rules and semantics documented in palWaitFence().
     */
    PalResult (PAL_CALL *waitFence)(
        PalFence* fence,
        Uint64 timeout);

    /**
     * Backend implementation of ::palResetFence.
     *
     * Must obey the rules and semantics documented in palResetFence().
     */
    PalResult (PAL_CALL *resetFence)(PalFence* fence);

    /**
     * Backend implementation of ::palIsFenceSignaled.
     *
     * Must obey the rules and semantics documented in palIsFenceSignaled().
     */
    bool (PAL_CALL *isFenceSignaled)(PalFence* fence);

    /**
     * Backend implementation of ::palCreateSemaphore.
     *
     * Must obey the rules and semantics documented in palCreateSemaphore().
     */
    PalResult (PAL_CALL *createSemaphore)(
        PalDevice* device,
        bool enableTimeline,
        PalSemaphore** outSemaphore);

    /**
     * Backend implementation of ::palDestroySemaphore.
     *
     * Must obey the rules and semantics documented in palDestroySemaphore().
     */
    void (PAL_CALL *destroySemaphore)(PalSemaphore* semaphore);

    /**
     * Backend implementation of ::palWaitSemaphore.
     *
     * Must obey the rules and semantics documented in palWaitSemaphore().
     */
    PalResult (PAL_CALL *waitSemaphore)(
        PalSemaphore* semaphore,
        Uint64 value,
        Uint64 timeout);

    /**
     * Backend implementation of ::palSignalSemaphore.
     *
     * Must obey the rules and semantics documented in palSignalSemaphore().
     */
    PalResult (PAL_CALL *signalSemaphore)(
        PalSemaphore* semaphore,
        PalQueue* queue,
        Uint64 value);

    /**
     * Backend implementation of ::palGetSemaphoreValue.
     *
     * Must obey the rules and semantics documented in palGetSemaphoreValue().
     */
    PalResult (PAL_CALL *getSemaphoreValue)(
        PalSemaphore* semaphore,
        Uint64* outValue);

    /**
     * Backend implementation of ::palCreateCommandPool.
     *
     * Must obey the rules and semantics documented in palCreateCommandPool().
     */
    PalResult (PAL_CALL *createCommandPool)(
        PalDevice* device,
        PalQueue* queue,
        PalCommandPool** outPool);

    /**
     * Backend implementation of ::palDestroyCommandPool.
     *
     * Must obey the rules and semantics documented in palDestroyCommandPool().
     */
    void (PAL_CALL *destroyCommandPool)(PalCommandPool* pool);

    /**
     * Backend implementation of ::palResetCommandPool.
     *
     * Must obey the rules and semantics documented in palResetCommandPool().
     */
    PalResult (PAL_CALL *resetCommandPool)(PalCommandPool* pool);

    /**
     * Backend implementation of ::palAllocateCommandBuffer.
     *
     * Must obey the rules and semantics documented in palAllocateCommandBuffer().
     */
    PalResult (PAL_CALL *allocateCommandBuffer)(
        PalDevice* device,
        PalCommandPool* pool,
        PalCommandBufferType type,
        PalCommandBuffer** outCmdBuffer);

    /**
     * Backend implementation of ::palFreeCommandBuffer.
     *
     * Must obey the rules and semantics documented in palFreeCommandBuffer().
     */
    void (PAL_CALL *freeCommandBuffer)(PalCommandBuffer* cmdBuffer);

    /**
     * Backend implementation of ::palResetCommandBuffer.
     *
     * Must obey the rules and semantics documented in palResetCommandBuffer().
     */
    PalResult (PAL_CALL *resetCommandBuffer)(PalCommandBuffer* cmdBuffer);

    /**
     * Backend implementation of ::palSubmitCommandBuffer.
     *
     * Must obey the rules and semantics documented in palSubmitCommandBuffer().
     */
    PalResult (PAL_CALL *submitCommandBuffer)(
        PalQueue* queue,
        PalCommandBufferSubmitInfo* info);

    /**
     * Backend implementation of ::palCmdBegin.
     *
     * Must obey the rules and semantics documented in palCmdBegin().
     */
    PalResult (PAL_CALL *cmdBegin)(
        PalCommandBuffer* cmdBuffer,
        PalRenderingLayoutInfo* info);

    /**
     * Backend implementation of ::palCmdEnd.
     *
     * Must obey the rules and semantics documented in palCmdEnd().
     */
    PalResult (PAL_CALL *cmdEnd)(PalCommandBuffer* cmdBuffer);

    /**
     * Backend implementation of ::palCmdExecuteCommandBuffer.
     *
     * Must obey the rules and semantics documented in palCmdExecuteCommandBuffer().
     */
    PalResult (PAL_CALL *cmdExecuteCommandBuffer)(
        PalCommandBuffer* primaryCmdBuffer,
        PalCommandBuffer* secondaryCmdBuffer);

    /**
     * Backend implementation of ::palCmdSetFragmentShadingRate.
     *
     * Must obey the rules and semantics documented in palCmdSetFragmentShadingRate().
     */
    PalResult (PAL_CALL *cmdSetFragmentShadingRate)(
        PalCommandBuffer* cmdBuffer,
        PalFragmentShadingRateState* state);

    /**
     * Backend implementation of ::palCmdDrawMeshTasks.
     *
     * Must obey the rules and semantics documented in palCmdDrawMeshTasks().
     */
    PalResult (PAL_CALL *cmdDrawMeshTasks)(
        PalCommandBuffer* cmdBuffer,
        Uint32 groupCountX,
        Uint32 groupCountY,
        Uint32 groupCountZ);

    /**
     * Backend implementation of ::palCmdDrawMeshTasksIndirect.
     *
     * Must obey the rules and semantics documented in palCmdDrawMeshTasksIndirect().
     */
    PalResult (PAL_CALL *cmdDrawMeshTasksIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        Uint32 drawCount);

    /**
     * Backend implementation of ::palCmdDrawMeshTasksIndirectCount.
     *
     * Must obey the rules and semantics documented in palCmdDrawMeshTasksIndirectCount().
     */
    PalResult (PAL_CALL *cmdDrawMeshTasksIndirectCount)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalBuffer* countBuffer,
        Uint32 maxDrawCount);

    /**
     * Backend implementation of ::palCmdBuildAccelerationStructure.
     *
     * Must obey the rules and semantics documented in palCmdBuildAccelerationStructure().
     */
    PalResult (PAL_CALL *cmdBuildAccelerationStructure)(
        PalCommandBuffer* cmdBuffer,
        PalAccelerationStructureBuildInfo* info);

    /**
     * Backend implementation of ::palCmdBeginRendering.
     *
     * Must obey the rules and semantics documented in palCmdBeginRendering().
     */
    PalResult (PAL_CALL *cmdBeginRendering)(
        PalCommandBuffer* cmdBuffer,
        PalRenderingInfo* info);

    /**
     * Backend implementation of ::palCmdEndRendering.
     *
     * Must obey the rules and semantics documented in palCmdEndRendering().
     */
    PalResult (PAL_CALL *cmdEndRendering)(PalCommandBuffer* cmdBuffer);

    /**
     * Backend implementation of ::palCmdCopyBuffer.
     *
     * Must obey the rules and semantics documented in palCmdCopyBuffer().
     */
    PalResult (PAL_CALL *cmdCopyBuffer)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* dst,
        PalBuffer* src,
        PalBufferCopyInfo* copyInfo);

    /**
     * Backend implementation of ::palCmdCopyBufferToImage.
     *
     * Must obey the rules and semantics documented in palCmdCopyBufferToImage().
     */
    PalResult (PAL_CALL *cmdCopyBufferToImage)(
        PalCommandBuffer* cmdBuffer,
        PalImage* dstImage,
        PalBuffer* srcBuffer,
        PalBufferImageCopyInfo* copyInfo);

    /**
     * Backend implementation of ::cmdCopyImage.
     *
     * Must obey the rules and semantics documented in cmdCopyImage().
     */
    PalResult (PAL_CALL *cmdCopyImage)(
        PalCommandBuffer* cmdBuffer,
        PalImage* dst,
        PalImage* src,
        PalImageCopyInfo* copyInfo);

    /**
     * Backend implementation of ::palCmdCopyImageToBuffer.
     *
     * Must obey the rules and semantics documented in palCmdCopyImageToBuffer().
     */
    PalResult (PAL_CALL *cmdCopyImageToBuffer)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* dstBuffer,
        PalImage* srcImage,
        PalBufferImageCopyInfo* copyInfo);

    /**
     * Backend implementation of ::palCmdBindPipeline.
     *
     * Must obey the rules and semantics documented in palCmdBindPipeline().
     */
    PalResult (PAL_CALL *cmdBindPipeline)(
        PalCommandBuffer* cmdBuffer,
        PalPipeline* pipeline);

    /**
     * Backend implementation of ::palCmdSetViewport.
     *
     * Must obey the rules and semantics documented in palCmdSetViewport().
     */
    PalResult (PAL_CALL *cmdSetViewport)(
        PalCommandBuffer* cmdBuffer,
        Uint32 count,
        PalViewport* viewports);

    /**
     * Backend implementation of ::palCmdSetScissors.
     *
     * Must obey the rules and semantics documented in palCmdSetScissors().
     */
    PalResult (PAL_CALL *cmdSetScissors)(
        PalCommandBuffer* cmdBuffer,
        Uint32 count,
        PalRect2D* scissors);

    /**
     * Backend implementation of ::palCmdBindVertexBuffers.
     *
     * Must obey the rules and semantics documented in palCmdBindVertexBuffers().
     */
    PalResult (PAL_CALL *cmdBindVertexBuffers)(
        PalCommandBuffer* cmdBuffer,
        Uint32 firstSlot,
        Uint32 count,
        PalBuffer** buffers,
        Uint64* offsets);

    /**
     * Backend implementation of ::palCmdBindIndexBuffer.
     *
     * Must obey the rules and semantics documented in palCmdBindIndexBuffer().
     */
    PalResult (PAL_CALL *cmdBindIndexBuffer)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        Uint64 offset,
        PalIndexType type);

    /**
     * Backend implementation of ::palCmdDraw.
     *
     * Must obey the rules and semantics documented in palCmdDraw().
     */
    PalResult (PAL_CALL *cmdDraw)(
        PalCommandBuffer* cmdBuffer,
        Uint32 vertexCount,
        Uint32 instanceCount,
        Uint32 firstVertex,
        Uint32 firstInstance);

    /**
     * Backend implementation of ::palCmdDrawIndirect.
     *
     * Must obey the rules and semantics documented in palCmdDrawIndirect().
     */
    PalResult (PAL_CALL *cmdDrawIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        Uint32 count);

    /**
     * Backend implementation of ::palCmdDrawIndirectCount.
     *
     * Must obey the rules and semantics documented in palCmdDrawIndirectCount().
     */
    PalResult (PAL_CALL *cmdDrawIndirectCount)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalBuffer* countBuffer,
        Uint32 count);

    /**
     * Backend implementation of ::palCmdDrawIndexed.
     *
     * Must obey the rules and semantics documented in palCmdDrawIndexed().
     */
    PalResult (PAL_CALL *cmdDrawIndexed)(
        PalCommandBuffer* cmdBuffer,
        Uint32 indexCount,
        Uint32 instanceCount,
        Uint32 firstIndex,
        Int32 vertexOffset,
        Uint32 firstInstance);

    /**
     * Backend implementation of ::palCmdDrawIndexedIndirect.
     *
     * Must obey the rules and semantics documented in palCmdDrawIndexedIndirect().
     */
    PalResult (PAL_CALL *cmdDrawIndexedIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        Uint32 count);

    /**
     * Backend implementation of ::palCmdDrawIndexedIndirectCount.
     *
     * Must obey the rules and semantics documented in palCmdDrawIndexedIndirectCount().
     */
    PalResult (PAL_CALL *cmdDrawIndexedIndirectCount)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalBuffer* countBuffer,
        Uint32 count);

    /**
     * Backend implementation of ::palCmdAccelerationStructureBarrier.
     *
     * Must obey the rules and semantics documented in palCmdAccelerationStructureBarrier().
     */
    PalResult (PAL_CALL *cmdAccelerationStructureBarrier)(
        PalCommandBuffer* cmdBuffer,
        PalAccelerationStructure* as,
        PalUsageStateInfo* oldUsageStateInfo,
        PalUsageStateInfo* newUsageStateInfo);

    /**
     * Backend implementation of ::palCmdImageBarrier.
     *
     * Must obey the rules and semantics documented in palCmdImageBarrier().
     */
    PalResult (PAL_CALL *cmdImageBarrier)(
        PalCommandBuffer* cmdBuffer,
        PalImage* image,
        PalImageSubresourceRange* subresourceRange,
        PalUsageStateInfo* oldUsageStateInfo,
        PalUsageStateInfo* newUsageStateInfo);

    /**
     * Backend implementation of ::palCmdBufferBarrier.
     *
     * Must obey the rules and semantics documented in palCmdBufferBarrier().
     */
    PalResult (PAL_CALL *cmdBufferBarrier)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalUsageStateInfo* oldUsageStateInfo,
        PalUsageStateInfo* newUsageStateInfo);

    /**
     * Backend implementation of ::palCmdDispatch.
     *
     * Must obey the rules and semantics documented in palCmdDispatch().
     */
    PalResult (PAL_CALL *cmdDispatch)(
        PalCommandBuffer* cmdBuffer,
        Uint32 groupCountX,
        Uint32 groupCountY,
        Uint32 groupCountZ);

    /**
     * Backend implementation of ::palCmdDispatchBase.
     *
     * Must obey the rules and semantics documented in palCmdDispatchBase().
     */
    PalResult (PAL_CALL *cmdDispatchBase)(
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
     * Must obey the rules and semantics documented in palCmdDispatchIndirect().
     */
    PalResult (PAL_CALL *cmdDispatchIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer);

    /**
     * Backend implementation of ::palCmdTraceRays.
     *
     * Must obey the rules and semantics documented in palCmdTraceRays().
     */
    PalResult (PAL_CALL *cmdTraceRays)(
        PalCommandBuffer* cmdBuffer,
        PalShaderBindingTable* sbt,
        Uint32 raygenIndex,
        Uint32 width,
        Uint32 height,
        Uint32 depth);

    /**
     * Backend implementation of ::palCmdTraceRaysIndirect.
     *
     * Must obey the rules and semantics documented in palCmdTraceRaysIndirect().
     */
    PalResult (PAL_CALL *cmdTraceRaysIndirect)(
        PalCommandBuffer* cmdBuffer,
        Uint32 raygenIndex,
        PalShaderBindingTable* sbt,
        PalBuffer* buffer);

    /**
     * Backend implementation of ::palCmdBindDescriptorSet.
     *
     * Must obey the rules and semantics documented in palCmdBindDescriptorSet().
     */
    PalResult (PAL_CALL *cmdBindDescriptorSet)(
        PalCommandBuffer* cmdBuffer,
        Uint32 setIndex,
        PalDescriptorSet* set);

    /**
     * Backend implementation of ::palCmdPushConstants.
     *
     * Must obey the rules and semantics documented in palCmdPushConstants().
     */
    PalResult (PAL_CALL *cmdPushConstants)(
        PalCommandBuffer* cmdBuffer,
        Uint32 shaderStageCount,
        PalShaderStage* shaderStages,
        Uint32 offset,
        Uint32 size,
        const void* value);

    /**
     * Backend implementation of ::palCmdSetCullMode.
     *
     * Must obey the rules and semantics documented in palCmdSetCullMode().
     */
    PalResult (PAL_CALL *cmdSetCullMode)(
        PalCommandBuffer* cmdBuffer,
        PalCullMode cullMode);

    /**
     * Backend implementation of ::palCmdSetFrontFace.
     *
     * Must obey the rules and semantics documented in palCmdSetFrontFace().
     */
    PalResult (PAL_CALL *cmdSetFrontFace)(
        PalCommandBuffer* cmdBuffer,
        PalFrontFace frontFace);

    /**
     * Backend implementation of ::palCmdSetPrimitiveTopology.
     *
     * Must obey the rules and semantics documented in palCmdSetPrimitiveTopology().
     */
    PalResult (PAL_CALL *cmdSetPrimitiveTopology)(
        PalCommandBuffer* cmdBuffer,
        PalPrimitiveTopology topology);

    /**
     * Backend implementation of ::palCmdSetDepthTestEnable.
     *
     * Must obey the rules and semantics documented in palCmdSetDepthTestEnable().
     */
    PalResult (PAL_CALL *cmdSetDepthTestEnable)(
        PalCommandBuffer* cmdBuffer,
        bool enable);

    /**
     * Backend implementation of ::palCmdSetDepthWriteEnable.
     *
     * Must obey the rules and semantics documented in palCmdSetDepthWriteEnable().
     */
    PalResult (PAL_CALL *cmdSetDepthWriteEnable)(
        PalCommandBuffer* cmdBuffer,
        bool enable);

    /**
     * Backend implementation of ::palCmdSetStencilOp.
     *
     * Must obey the rules and semantics documented in palCmdSetStencilOp().
     */
    PalResult (PAL_CALL *cmdSetStencilOp)(
        PalCommandBuffer* cmdBuffer,
        PalStencilFaceFlags faceMask,
        PalStencilOp failOp,
        PalStencilOp passOp,
        PalStencilOp depthFailOp,
        PalCompareOp compareOp);

    /**
     * Backend implementation of ::palCreateAccelerationstructure.
     *
     * Must obey the rules and semantics documented in palCreateAccelerationstructure().
     */
    PalResult (PAL_CALL *createAccelerationstructure)(
        PalDevice* device,
        const PalAccelerationStructureCreateInfo* info,
        PalAccelerationStructure** outAs);

    /**
     * Backend implementation of ::palDestroyAccelerationstructure.
     *
     * Must obey the rules and semantics documented in palDestroyAccelerationstructure().
     */
    void (PAL_CALL *destroyAccelerationstructure)(PalAccelerationStructure* as);

    /**
     * Backend implementation of ::palGetAccelerationStructureBuildSize.
     *
     * Must obey the rules and semantics documented in palGetAccelerationStructureBuildSize().
     */
    PalResult (PAL_CALL *getAccelerationStructureBuildSize)(
        PalDevice* device,
        PalAccelerationStructureBuildInfo* info,
        PalAccelerationStructureBuildSize* size);

    /**
     * Backend implementation of ::palCreateBuffer.
     *
     * Must obey the rules and semantics documented in palCreateBuffer().
     */
    PalResult (PAL_CALL *createBuffer)(
        PalDevice* device,
        const PalBufferCreateInfo* info,
        PalBuffer** outBuffer);

    /**
     * Backend implementation of ::palDestroyBuffer.
     *
     * Must obey the rules and semantics documented in palDestroyBuffer().
     */
    void (PAL_CALL *destroyBuffer)(PalBuffer* buffer);

    /**
     * Backend implementation of ::palGetBufferMemoryRequirements.
     *
     * Must obey the rules and semantics documented in palGetBufferMemoryRequirements().
     */
    PalResult (PAL_CALL *getBufferMemoryRequirements)(
        PalBuffer* buffer,
        PalMemoryRequirements* requirements);

    /**
     * Backend implementation of ::palComputeInstanceBufferRequirements.
     *
     * Must obey the rules and semantics documented in palComputeInstanceBufferRequirements().
     */
    PalResult (PAL_CALL *computeInstanceBufferRequirements)(
        PalDevice* device,
        Uint32 instanceCount,
        Uint64* outSize);

    /**
     * Backend implementation of ::palComputeImageCopyStagingBufferRequirements.
     *
     * Must obey the rules and semantics documented in palComputeImageCopyStagingBufferRequirements().
     */
    PalResult (PAL_CALL *computeImageCopyStagingBufferRequirements)(
        PalDevice* device,
        PalFormat imageFormat,
        PalBufferImageCopyInfo* copyInfo,
        Uint32* outBufferRowLength,
        Uint32* outBufferImageHeight,
        Uint64* outSize);

    /**
     * Backend implementation of ::palWriteToInstanceBuffer.
     *
     * Must obey the rules and semantics documented in palWriteToInstanceBuffer().
     */
    PalResult (PAL_CALL *writeToInstanceBuffer)(
        PalDevice* device,
        void* ptr,
        PalAccelerationStructureInstance* instances,
        Uint32 instanceCount);

    /**
     * Backend implementation of ::palWriteToImageCopyStagingBuffer.
     *
     * Must obey the rules and semantics documented in palWriteToImageCopyStagingBuffer().
     */
    PalResult (PAL_CALL *writeToImageCopyStagingBuffer)(
        PalDevice* device,
        void* ptr,
        void* srcData,
        PalFormat imageFormat,
        PalBufferImageCopyInfo* copyInfo);

    /**
     * Backend implementation of ::palBindBufferMemory.
     *
     * Must obey the rules and semantics documented in palBindBufferMemory().
     */
    PalResult (PAL_CALL *bindBufferMemory)(
        PalBuffer* buffer,
        PalMemory* memory,
        Uint64 offset);

    /**
     * Backend implementation of ::palMapBufferMemory.
     *
     * Must obey the rules and semantics documented in palMapBufferMemory().
     */
    PalResult (PAL_CALL *mapBufferMemory)(
        PalBuffer* buffer,
        Uint64 offset,
        Uint64 size,
        void** outPtr);

    /**
     * Backend implementation of ::palUnmapBufferMemory.
     *
     * Must obey the rules and semantics documented in palUnmapBufferMemory().
     */
    void (PAL_CALL *unmapBufferMemory)(PalBuffer* buffer);

    /**
     * Backend implementation of ::palGetBufferDeviceAddress.
     *
     * Must obey the rules and semantics documented in palGetBufferDeviceAddress().
     */
    PalDeviceAddress (PAL_CALL *getBufferDeviceAddress)(PalBuffer* buffer);

    /**
     * Backend implementation of ::palCreateDescriptorSetLayout.
     *
     * Must obey the rules and semantics documented in palCreateDescriptorSetLayout().
     */
    PalResult (PAL_CALL *createDescriptorSetLayout)(
        PalDevice* device,
        const PalDescriptorSetLayoutCreateInfo* info,
        PalDescriptorSetLayout** outLayout);

    /**
     * Backend implementation of ::palDestroyDescriptorSetLayout.
     *
     * Must obey the rules and semantics documented in palDestroyDescriptorSetLayout().
     */
    void (PAL_CALL *destroyDescriptorSetLayout)(PalDescriptorSetLayout* layout);

    /**
     * Backend implementation of ::palCreateDescriptorPool.
     *
     * Must obey the rules and semantics documented in palCreateDescriptorPool().
     */
    PalResult (PAL_CALL *createDescriptorPool)(
        PalDevice* device,
        const PalDescriptorPoolCreateInfo* info,
        PalDescriptorPool** outPool);

    /**
     * Backend implementation of ::palDestroyDescriptorPool.
     *
     * Must obey the rules and semantics documented in palDestroyDescriptorPool().
     */
    void (PAL_CALL *destroyDescriptorPool)(PalDescriptorPool* pool);

    /**
     * Backend implementation of ::palResetDescriptorPool.
     *
     * Must obey the rules and semantics documented in palResetDescriptorPool().
     */
    PalResult (PAL_CALL *resetDescriptorPool)(PalDescriptorPool* pool);

    /**
     * Backend implementation of ::palAllocateDescriptorSet.
     *
     * Must obey the rules and semantics documented in palAllocateDescriptorSet().
     */
    PalResult (PAL_CALL *allocateDescriptorSet)(
        PalDevice* device,
        PalDescriptorPool* pool,
        PalDescriptorSetLayout* layout,
        PalDescriptorSet** outSet);

    /**
     * Backend implementation of ::palUpdateDescriptorSet.
     *
     * Must obey the rules and semantics documented in palUpdateDescriptorSet().
     */
    PalResult (PAL_CALL *updateDescriptorSet)(
        PalDevice* device,
        Uint32 count,
        PalDescriptorSetWriteInfo* infos);

    /**
     * Backend implementation of ::palCreatePipelineLayout.
     *
     * Must obey the rules and semantics documented in palCreatePipelineLayout().
     */
    PalResult (PAL_CALL *createPipelineLayout)(
        PalDevice* device,
        const PalPipelineLayoutCreateInfo* info,
        PalPipelineLayout** outLayout);

    /**
     * Backend implementation of ::palDestroyPipelineLayout.
     *
     * Must obey the rules and semantics documented in palDestroyPipelineLayout().
     */
    void (PAL_CALL *destroyPipelineLayout)(PalPipelineLayout* layout);

    /**
     * Backend implementation of ::palCreateGraphicsPipeline.
     *
     * Must obey the rules and semantics documented in palCreateGraphicsPipeline().
     */
    PalResult (PAL_CALL *createGraphicsPipeline)(
        PalDevice* device,
        const PalGraphicsPipelineCreateInfo* info,
        PalPipeline** outPipeline);

    /**
     * Backend implementation of ::palCreateComputePipeline.
     *
     * Must obey the rules and semantics documented in palCreateComputePipeline().
     */
    PalResult (PAL_CALL *createComputePipeline)(
        PalDevice* device,
        const PalComputePipelineCreateInfo* info,
        PalPipeline** outPipeline);

    /**
     * Backend implementation of ::palCreateRayTracingPipeline.
     *
     * Must obey the rules and semantics documented in palCreateRayTracingPipeline().
     */
    PalResult (PAL_CALL *createRayTracingPipeline)(
        PalDevice* device,
        const PalRayTracingPipelineCreateInfo* info,
        PalPipeline** outPipeline);

    /**
     * Backend implementation of ::palDestroyPipeline.
     *
     * Must obey the rules and semantics documented in palDestroyPipeline().
     */
    void (PAL_CALL *destroyPipeline)(PalPipeline* pipeline);

    /**
     * Backend implementation of ::palCreateShaderBindingTable.
     *
     * Must obey the rules and semantics documented in palCreateShaderBindingTable().
     */
    PalResult (PAL_CALL *createShaderBindingTable)(
        PalDevice* device,
        const PalShaderBindingTableCreateInfo* info,
        PalShaderBindingTable** outSbt);

    /**
     * Backend implementation of ::palDestroyShaderBindingTable.
     *
     * Must obey the rules and semantics documented in palDestroyShaderBindingTable().
     */
    void (PAL_CALL *destroyShaderBindingTable)(PalShaderBindingTable* sbt);

    /**
     * Backend implementation of ::palUpdateShaderBindingTable.
     *
     * Must obey the rules and semantics documented in palUpdateShaderBindingTable().
     */
    PalResult (PAL_CALL *updateShaderBindingTable)(
        PalShaderBindingTable* sbt, 
        Uint32 count,
        PalShaderBindingTableRecordInfo* infos);
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
 * Thread safety: Must only be called from the main thread.
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
 * graphics system. The graphics system must be shutdown with palShutdownGraphics() when no longer
 * needed.
 *
 * The debugger and allocator will not not copied, therefore the pointers must remain valid
 * until the graphics system is shutdown. Set the debugger or PalGraphicsDebugger::callback to
 * nullptr to disable debugging and validation layers.
 * 
 * If `debugger` is not nullptr and there is no debug layers, this function will not fail but 
 * debugging will be disabled.
 *
 * @param[in] debugger Optional debugger. Set to nullptr to disable debugging and validation
 * layers.
 * @param[in] allocator Optional user-provided allocator. Set to nullptr to use default.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
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
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palInitGraphics
 */
PAL_API void PAL_CALL palShutdownGraphics();

/**
 * @brief Returns a list of all adapters (GPU) from custom and internal backends.
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
 * @param[in, out] count Capacity of the PalAdapter array.
 * @param[out] outAdapters User allocated array of PalAdapter.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
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
 * Thread safety: Thread safe if `info` is per thread.
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
 * Thread safety: Thread safe if `caps` is per thread.
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
 * Thread safety: Thread safe.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palEnumerateAdapters
 */
PAL_API PalAdapterFeatures PAL_CALL palGetAdapterFeatures(PalAdapter* adapter);

/**
 * @brief Get the highest supported shader target of an adapter (GPU).
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] adapter Adapter to query.
 * @param[in] shaderFormat The shader format. Must have only a single bit set.
 *
 * @return The highest supported shader target encoded with `PAL_MAKE_SHADER_TARGET` macro 
 * on success otherwise `0` on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palEnumerateAdapters
 */
PAL_API Uint32 PAL_CALL palGetHighestSupportedShaderTarget(
    PalAdapter* adapter, 
    PalShaderFormats shaderFormat);

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
 * @param[in] adapter Adapter that creates the device.
 * @param[in] features Adapter features to enable. Must be supported.
 * @param[out] outDevice Pointer to a PalDevice to recieve the created device. Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `adapter` is externally synchronized.
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
 * Thread safety: Thread safe if the adapter used to create the device is
 * externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateDevice
 */
PAL_API void PAL_CALL palDestroyDevice(PalDevice* device);

/**
 * @brief Allocates GPU memory for the specified device.
 *
 * The graphics system must be initialized before this call. On CPU adapters, there is usually no
 * `PAL_MEMORY_TYPE_GPU_ONLY` memory type available. So it uses shared memory as the vram and set
 * the shared memory to the `PAL_MEMORY_TYPE_GPU_ONLY` for correctness.
 *
 * @param[in] device Pointer to device to allocate memory on.
 * @param[in] type Memory type to allocate. Must be supported by the adapter associated with the
 * device.
 * @param[in] memoryMask Memory mask. Must match memory type.
 * @param[in] size Number of bytes to allocate. Must not be 0.
 * @param[out] outMemory Pointer to a PalMemory to recieved the allocated GPU memory.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized and
 * `outMemory` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palFreeMemory
 */
PAL_API PalResult PAL_CALL palAllocateMemory(
    PalDevice* device,
    PalMemoryType type,
    Uint64 memoryMask,
    Uint64 size,
    PalMemory** outMemory);

/**
 * @brief Free GPU memory allocated by palAllocateMemory.
 *
 * The graphics system must be initialized before this call.
 * If `memory` is nullptr, this function will return silently.
 *
 * @param[in] device Pointer to device to free memory on.
 * @param[in] memory Pointer to memory to free.
 *
 * Thread safety: Thread safe if `device` is externally synchronized and
 * `outMemory` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palAllocateMemory
 */
PAL_API void PAL_CALL palFreeMemory(
    PalDevice* device,
    PalMemory* memory);

/**
 * @brief Get sampler anisotropy feature capabilites or limits about a device.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query sampler anisotropy feature capabilities on.
 * @param[out] caps Pointer to a PalSamplerAnisotropyCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palQuerySamplerAnisotropyCapabilities(
    PalDevice* device,
    PalSamplerAnisotropyCapabilities* caps); 

/**
 * @brief Get multi view feature capabilites or limits about a device.
 *
 * The graphics system must be initialized before this call.
 * 
 * `PAL_ADAPTER_FEATURE_MULTI_VIEW` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query multi view feature capabilities on.
 * @param[out] caps Pointer to a PalMultiViewCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palQueryMultiViewCapabilities(
    PalDevice* device,
    PalMultiViewCapabilities* caps);

/**
 * @brief Get multi viewport feature capabilites or limits about a device.
 *
 * The graphics system must be initialized before this call.
 * 
 * `PAL_ADAPTER_FEATURE_MULTI_VIEWPORT` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query multi viewport feature capabilities on.
 * @param[out] caps Pointer to a PalMultiViewportCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palQueryMultiViewportCapabilities(
    PalDevice* device,
    PalMultiViewportCapabilities* caps);

/**
 * @brief Get depth stencil feature capabilites or limits about a device.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query depth stencil feature capabilities on.
 * @param[out] caps Pointer to a PalDepthStencilCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palQueryDepthStencilCapabilities(
    PalDevice* device,
    PalDepthStencilCapabilities* caps);

/**
 * @brief Get fragment shading rate feature capabilites or limits about a device.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query fragment shading rate feature capabilities on.
 * @param[out] caps Pointer to a PalFragmentShadingRateCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palQueryFragmentShadingRateCapabilities(
    PalDevice* device,
    PalFragmentShadingRateCapabilities* caps);

/**
 * @brief Get mesh shader feature capabilites or limits about a device.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_MESH_SHADER` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query mesh shader feature capabilities on.
 * @param[out] caps Pointer to a PalMeshShaderCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palQueryMeshShaderCapabilities(
    PalDevice* device,
    PalMeshShaderCapabilities* caps);

/**
 * @brief Get ray tracing feature capabilites or limits about a device.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query ray tracing feature capabilities on.
 * @param[out] caps Pointer to a PalRayTracingCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palQueryRayTracingCapabilities(
    PalDevice* device,
    PalRayTracingCapabilities* caps);

/**
 * @brief Get descriptor indexing feature capabilites or limits about a device.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query descriptor indexing feature capabilities on.
 * @param[out] caps Pointer to a PalDescriptorIndexingCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palQueryDescriptorIndexingCapabilities(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps);

/**
 * @brief Create a queue from a device.
 *
 * The graphics system must be initialized before this call.
 *
 * The number of queues of each type which can be created is limited per adapter. check with
 * PalAdapterCapabilities::maxComputeQueues, PalAdapterCapabilities::maxGraphicsQueues and
 * PalAdapterCapabilities::maxCopyQueues respectively for the limit for each queue type.
 * Creating more queues than the supported will fail and return `PAL_RESULT_OUT_OF_QUEUE`.
 * 
 * Not all graphics queues support presentation. Create a graphics queue and then check if
 * its support presentation for the provided surface. see palCanQueuePresent(). Any graphics
 * queue supports offscreen rendering.
 *
 * @param[in] device Device that creates the queue.
 * @param[in] type Queue type. (eg. PAL_QUEUE_TYPE_GRAPHICS).
 * @param[out] outQueue Pointer to a PalQueue to recieve the created queue.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroyQueue
 */
PAL_API PalResult PAL_CALL palCreateQueue(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue);

/**
 * @brief Destroy a queue.
 *
 * The graphics system must be initialized before this call.
 * If the provided queue is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] queue Queue to destroy.
 *
 * Thread safety: Thread safe if the device used to create the queue is
 * externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateQueue
 */
PAL_API void PAL_CALL palDestroyQueue(PalQueue* queue);

/**
 * @brief Check if a queue is presentable to the provided window.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] queue Queue to query.
 * @param[in] surface Surface to check presentation support for.
 *
 * @return True if queue can present otherwise false if queue can not present.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateQueue
 */
PAL_API bool PAL_CALL palCanQueuePresent(
    PalQueue* queue,
    PalSurface* surface);

/**
 * @brief Blocks indefinitely until the queue becomes idle.
 *
 * The graphics system must be initialized before this call.
 *
 * This function blocks indefinitely until all submitted work on the queue has been completetd.
 * Returns `PAL_RESULT_SUCCESS` to indicate all pending operations has been completetd.
 *
 * @param[in] queue Pointer to queue to wait.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `queue` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palWaitQueue(PalQueue* queue);

/**
 * @brief Returns a list of all supported formats of an adapter (GPU).
 *
 * The graphics system must be initialized before this call.
 *
 * This function returns the supported format with the supported image usages
 * associated with the format. This is a handy way of selecting a format based on the image
 * usages. Use palIsFormatSupported() to check for a specific format.
 *
 * Call this function first with PalFormatInfo array set to nullptr to get the number of formats.
 * Allocate memory for the PalFormatInfo array and passed in the count and the allocated array. If
 * the count of the array is less than the number of formats, PAL will write upto that limit.
 *
 * If the count is 0 and the PalFormatInfo array is nullptr, the function fails
 * and returns `PAL_RESULT_INSUFFICIENT_BUFFER`.
 *
 * @param[in] adapter Adapter to query formats on.
 * @param[in, out] count Capacity of the PalFormatInfo array.
 * @param[out] outFormats User allocated array of PalFormatInfo.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `outFormats` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palIsFormatSupported
 */
PAL_API PalResult PAL_CALL palEnumerateFormats(
    PalAdapter* adapter,
    Int32* count,
    PalFormatInfo* outFormats);

/**
 * @brief Check support for a format on an adapter (GPU).
 *
 * The graphics system must be initialized before this call.
 *
 * This is much faster than enumerating all the formats to pick one. You directly check support
 * for the format you want to use. Call palQueryFormatImageUsages() to check for supported image
 * usages if format is supported.
 *
 * @param[in] adapter Adapter to query format on.
 * @param[in] format Format to query support for.
 *
 * @return True if format is supported otherwise false if not supported.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palQueryFormatImageUsages
 * @sa palQueryFormatImageViewUsages
 */
PAL_API bool PAL_CALL palIsFormatSupported(
    PalAdapter* adapter,
    PalFormat format);

/**
 * @brief Checks supported image usages associated with a format.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] adapter Adapter to query format on.
 * @param[in] format Format to query image usages for.
 *
 * @return Supported image usages on success otherwise `0` on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalImageUsages PAL_CALL palQueryFormatImageUsages(
    PalAdapter* adapter,
    PalFormat format);

/**
 * @brief Checks supported sample count associated with a format.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] adapter Adapter to query format on.
 * @param[in] format Format to query sample count for.
 *
 * @return Supported sample count on success otherwise `0` on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalSampleCount PAL_CALL palQueryFormatSampleCount(
    PalAdapter* adapter,
    PalFormat format);

/**
 * @brief Create an image.
 *
 * The graphics system must be initialized before this call.
 *
 * PalImageCreateInfo::width, PalImageCreateInfo::height and PalImageCreateInfo::sampleCount
 * must not be greater than the limits of the adapter used to create the device. Check
 * adapter capabilities for the limits.
 *
 * @param[in] device Device that creates the image.
 * @param[in] info Pointer to a PalImageCreateInfo struct that specifies parameters.
 * Must not be nullptr.
 * @param[out] outImage Pointer to a PalImage to recieve the created image.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroyImage
 */
PAL_API PalResult PAL_CALL palCreateImage(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage);

/**
 * @brief Destroy an image.
 *
 * The graphics system must be initialized before this call.
 * If the provided image is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] image Image to destroy.
 *
 * Thread safety: Thread safe if the device used to create the image is
 * externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateImage
 */
PAL_API void PAL_CALL palDestroyImage(PalImage* image);

/**
 * @brief Get information about an image.
 *
 * The graphics system must be initialized before this call.
 * This function also supports swapchain images.
 *
 * @param[in] image Image to query information on.
 * @param[out] info Pointer to a PalImageInfo to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `info` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateImage
 */
PAL_API PalResult PAL_CALL palGetImageInfo(
    PalImage* image,
    PalImageInfo* info);

/**
 * @brief Get memory requirements for the provided image.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] image Image to query memory requirements on.
 * @param[out] requirements Pointer to a PalMemoryRequirements to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `requirements` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palGetImageMemoryRequirements(
    PalImage* image,
    PalMemoryRequirements* requirements);

/**
 * @brief Bind an allocated memory to an image.
 *
 * The graphics system must be initialized before this call.
 * The memory size and alignment should match the requirements of the image.
 * Get the requirements with palGetImageMemoryRequirements().
 *
 * @param[in] image Image to bind memory to.
 * @param[in] memory Memory to bind. Must not be nullptr.
 * @param[in] offset Starting point within the memory.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `requirements` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palGetImageMemoryRequirements
 */
PAL_API PalResult PAL_CALL palBindImageMemory(
    PalImage* image,
    PalMemory* memory,
    Uint64 offset);

/**
 * @brief Maps image to CPU visible address space.
 *
 * The graphics system must be initialized before this call. The image must have a valid
 * memory bound to it before this call.
 *
 * Only `PAL_MEMORY_TYPE_CPU_UPLOAD` and `PAL_MEMORY_TYPE_CPU_READBACK` can be mapped to
 * CPU visible space. Mapping `PAL_MEMORY_TYPE_GPU_ONLY` will fail and return
 * `PAL_RESULT_MEMORY_MAP_FAILED`.
 *
 * @param[in] image Pointer to image to map. Memory must be bound.
 * @param[in] offset Starting point within the image.
 * @param[in] size Number of bytes to map from the offset. `offset + size` must not be
 * greater than image size.
 * @param[out] outPtr Pointer to a void* to recieved the mapped memory.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `image` is externally synchronized.
 * Mapping with different offsets into the same image is thread safe as long as `image`
 * is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palUnmapImageMemory
 */
PAL_API PalResult PAL_CALL palMapImageMemory(
    PalImage* image,
    Uint64 offset,
    Uint64 size,
    void** outPtr);

/**
 * @brief Unmap image from CPU visible address space.
 *
 * The graphics system must be initialized before this call. The image must be mapped
 * before this call. After this call, the CPU pointer must not be used anymore.
 *
 * @param[in] image Pointer to image to unmap.
 *
 * Thread safety: Thread safe if `image` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palMapImageMemory
 */
PAL_API void PAL_CALL palUnmapImageMemory(PalImage* image);

/**
 * @brief Create an image view.
 *
 * The graphics system must be initialized before this call.
 *
 * PalImageViewCreateInfo::type must be compatible by the type of the base image. Eg. A 2D base
 * image must be have an image view of either `PAL_IMAGE_VIEW_TYPE_2D` or
 * `PAL_IMAGE_VIEW_TYPE_2D_ARRAY`.
 *
 * `PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY` must be supported and enabled by the device
 * used to create the image view if `PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY` will be used.
 *
 * @param[in] device Device that creates the image view.
 * @param[in] image Image to create the image view with.
 * @param[in] info Pointer to a PalImageViewCreateInfo struct that specifies parameters.
 * Must not be nullptr.
 * @param[out] outImageView Pointer to a PalImageView to recieve the created image view.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroyImageView
 */
PAL_API PalResult PAL_CALL palCreateImageView(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** outImageView);

/**
 * @brief Destroy an image view.
 *
 * The graphics system must be initialized before this call.
 * If the provided image view is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] imageView Image view to destroy.
 *
 * Thread safety: Thread safe if the device used to create the image view is
 * externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateImageView
 */
PAL_API void PAL_CALL palDestroyImageView(PalImageView* imageView);

/**
 * @brief Create a sampler.
 *
 * The graphics system must be initialized before this call.
 * Samplers are immutable so any paramter used to create it cannot will be fixed after
 * creation.
 *
 * @param[in] device Device that creates the sampler.
 * @param[in] info Pointer to a PalSamplerCreateInfo struct that specifies parameters.
 * Must not be nullptr.
 * @param[out] outSampler Pointer to a PalSampler to recieve the created sampler.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroySampler
 */
PAL_API PalResult PAL_CALL palCreateSampler(
    PalDevice* device,
    const PalSamplerCreateInfo* info,
    PalSampler** outSampler);

/**
 * @brief Destroy a sampler.
 *
 * The graphics system must be initialized before this call.
 * If the provided sampler is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] sampler Sampler to destroy.
 *
 * Thread safety: Thread safe if the device used to create the sampler is
 * externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateSampler
 */
PAL_API void PAL_CALL palDestroySampler(PalSampler* sampler);

/**
 * @brief Create a surface for a window.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_SWAPCHAIN` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device that creates the surface.
 * @param[in] window Window to create the surface for.
 * @param[out] outSurface Pointer to a PalSurface to recieve the created surface.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroySurface
 */
PAL_API PalResult PAL_CALL palCreateSurface(
    PalDevice* device,
    PalGraphicsWindow* window,
    PalSurface** outSurface);

/**
 * @brief Destroy a surface.
 *
 * The graphics system must be initialized before this call.
 * If the provided surface is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] surface Surface to destroy.
 *
 * Thread safety: Thread safe if the device used to create the surface is
 * externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateSurface
 */
PAL_API void PAL_CALL palDestroySurface(PalSurface* surface);

/**
 * @brief Get surface capabilites about a device.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_SWAPCHAIN` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query surface feature capabilities on.
 * @param[in] surface Surface to query capabilities.
 * @param[out] caps Pointer to a PalSurfaceCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palGetSurfaceCapabilities(
    PalDevice* device,
    PalSurface* surface,
    PalSurfaceCapabilities* caps);

/**
 * @brief Create a swaphain.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_SWAPCHAIN` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device that creates the swapchain.
 * @param[in] queue Queue to create swapchain with. This must be a graphics queue.
 * @param[in] surface Surface to create swapchain with.
 * @param[in] info Pointer to a PalSwapchainCreateInfo struct that specifies parameters.
 * Must not be nullptr.
 * @param[out] outSwapchain Pointer to a PalSwapchain to recieve the created swapchain.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroySwapchain
 */
PAL_API PalResult PAL_CALL palCreateSwapchain(
    PalDevice* device,
    PalQueue* queue,
    PalSurface* surface,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain);

/**
 * @brief Destroy a swapchain.
 *
 * The graphics system must be initialized before this call.
 * If the provided swapchain is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] swapchain Swapchain to destroy.
 *
 * Thread safety: Thread safe if the device used to create the swapchain is
 * externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateSwapchain
 */
PAL_API void PAL_CALL palDestroySwapchain(PalSwapchain* swapchain);

/**
 * @brief Get a swapchain image from the list of images with an index.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] swapchain Swapchain to get image from.
 * @param[in] index Index of image in the list. Must not be greater than the image count.
 *
 * @return A pointer to the image on success otherwise nullptr on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palGetNextSwapchainImage
 */
PAL_API PalImage* PAL_CALL palGetSwapchainImage(
    PalSwapchain* swapchain,
    Int32 index);

/**
 * @brief Get the next available image from the swapchain image list.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] swapchain Swapchain to get image index from.
 * @param[in] info Pointer to a PalSwapchainNextImageInfo struct that specifies parameters.
 * Must not be nullptr.
 * @param[out] outIndex Pointer to a Uint32 to recieve the next image index.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `swapchain` externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palGetSwapchainImage
 */
PAL_API PalResult PAL_CALL palGetNextSwapchainImage(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    Uint32* outIndex);

/**
 * @brief Present the swapchain.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] swapchain Swapchain to present.
 * @param[in] info Pointer to a PalSwapchainPresentInfo struct that specifies parameters.
 * Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palPresentSwapchain(
    PalSwapchain* swapchain,
    PalSwapchainPresentInfo* info);

/**
 * @brief Resize the provided swapchain.
 *
 * The graphics system must be initialized before this call. 
 * 
 * The swapchain images must not be in use before this call. All resources (image views) that 
 * reference the swapchain images must be destroyed and recreated.
 *
 * @param[in] swapchain Swapchain to resize.
 * @param[in] newWidth The new width of the swapchain.
 * @param[in] newHeight The new height of the swapchain.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `swapchain` externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palResizeSwapchain(
    PalSwapchain* swapchain,
    Uint32 newWidth,
    Uint32 newHeight);

/**
 * @brief Create a shader.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_GEOMETRY_SHADER` must be supported and enabled by the device if
 * `PAL_SHADER_STAGE_GEOMETRY` will be used.
 *
 * `PAL_ADAPTER_FEATURE_MESH_SHADER` must be supported and enabled by the device if
 * `PAL_SHADER_STAGE_MESH` or `PAL_SHADER_STAGE_TASK` will be used.
 *
 * `PAL_ADAPTER_FEATURE_TESSELLATION_SHADER` must be supported and enabled by the device if
 * `PAL_SHADER_STAGE_TESSELLATION_CONTROL` or `PAL_SHADER_STAGE_TESSELLATION_EVALUATION` will
 * be used.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if
 * `PAL_SHADER_STAGE_RAYGEN` or `PAL_SHADER_STAGE_CLOSEST_HIT` or `PAL_SHADER_STAGE_ANY_HIT` or
 * `PAL_SHADER_STAGE_MISS` or `PAL_SHADER_STAGE_INTERSECTION` or `PAL_SHADER_STAGE_CALLABLE` will
 * be used.
 *
 * @param[in] device Device that creates the shader.
 * @param[in] info Pointer to a PalShaderCreateInfo struct that specifies parameters.
 * Must not be nullptr.
 * @param[out] outShader Pointer to a PalShader to recieve the created shader.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 * 
 * @note The shader entry name must not be greater than `PAL_SHADER_ENTRY_NAME_SIZE (32)`.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroyShader
 */
PAL_API PalResult PAL_CALL palCreateShader(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader);

/**
 * @brief Destroy a shader.
 *
 * The graphics system must be initialized before this call.
 * If the provided shader is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] shader Shader to destroy.
 *
 * Thread safety: Thread safe if the device used to create the shader is
 * externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateShader
 */
PAL_API void PAL_CALL palDestroyShader(PalShader* shader);

/**
 * @brief Create a fence.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] device Device that creates the fence.
 * @param[in] signaled True if fence should be created signaled. If true, the fence must be reset
 * before waiting for it to prevent waiting forever.
 * @param[out] outFence Pointer to a PalFence to recieve the created fence.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroyFence
 */
PAL_API PalResult PAL_CALL palCreateFence(
    PalDevice* device,
    bool signaled,
    PalFence** outFence);

/**
 * @brief Destroy a fence.
 *
 * The graphics system must be initialized before this call.
 * If the provided fence is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] fence Fence to destroy.
 *
 * Thread safety: Thread safe if the device used to create the fence is
 * externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateFence
 */
PAL_API void PAL_CALL palDestroyFence(PalFence* fence);

/**
 * @brief Wait for a fence.
 *
 * The graphics system must be initialized before this call.
 *
 * This function blocks for `timeout` until the fence is signaled or there is a timeout.
 * Returns `PAL_RESULT_SUCCESS` or `PAL_RESULT_TIMEOUT` respectively.
 *
 * @param[in] fence Fence to wait for.
 * @param[in] timeout Time to wait for in milliseconds. Set to `PAL_INFINITE` for indefintely.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `fence` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palIsFenceSignaled
 */
PAL_API PalResult PAL_CALL palWaitFence(
    PalFence* fence,
    Uint64 timeout);

/**
 * @brief Reset a fence to an unsignaled state.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_FENCE_RESET` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] fence Fence to reset.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `fence` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palIsFenceSignaled
 */
PAL_API PalResult PAL_CALL palResetFence(PalFence* fence);

/**
 * @brief Checks if the provided fence is in a signaled state.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] fence Fence to check.
 *
 * @return True if signaled otherwise false.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palResetFence
 * @sa palWaitFence
 */
PAL_API bool PAL_CALL palIsFenceSignaled(PalFence* fence);

/**
 * @brief Create a semaphore.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] device Device that creates the semaphore.
 * @param[in] enableTimeline If true, `PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE` must be supported
 * and enabled by the device creating the semaphore.
 * @param[out] outSemaphore Pointer to a PalSemaphore to recieve the created semaphore.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroySemaphore
 */
PAL_API PalResult PAL_CALL palCreateSemaphore(
    PalDevice* device,
    bool enableTimeline,
    PalSemaphore** outSemaphore);

/**
 * @brief Destroy a semaphore.
 *
 * The graphics system must be initialized before this call.
 * If the provided semaphore is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] semaphore Semaphore to destroy.
 *
 * Thread safety: Thread safe if the device used to create the semaphore is
 * externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateSemaphore
 */
PAL_API void PAL_CALL palDestroySemaphore(PalSemaphore* semaphore);

/**
 * @brief Waits for a semaphore to reach the provided value.
 *
 * The graphics system must be initialized before this call. 
 * 
 * The provided semaphore must be a timeline semaphore If not, this function fails and returns 
 * `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] semaphore Semaphore to wait on.
 * @param[in] value Value to wait for.
 * @param[in] timeout Time to wait for in milliseconds. Set to `PAL_INFINITE` for indefintely.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `semaphore` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palSignalSemaphore
 * @sa palGetSemaphoreValue
 */
PAL_API PalResult PAL_CALL palWaitSemaphore(
    PalSemaphore* semaphore,
    Uint64 value,
    Uint64 timeout);

/**
 * @brief Signals a semaphore from the provided value.
 *
 * The graphics system must be initialized before this call.
 *
 * The provided semaphore must be a timeline semaphore If not, this function fails and returns 
 * `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] semaphore Semaphore to signal.
 * @param[in] queue Queue used to signal the semaphore.
 * @param[in] value Value to signal.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `queue` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palWaitSemaphore
 * @sa palGetSemaphoreValue
 */
PAL_API PalResult PAL_CALL palSignalSemaphore(
    PalSemaphore* semaphore,
    PalQueue* queue,
    Uint64 value);

/**
 * @brief Get the value of a semaphore.
 *
 * The graphics system must be initialized before this call.
 *
 * The provided semaphore must be a timeline semaphore If not, this function fails and returns 
 * `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] semaphore Semaphore to get its value.
 * @param[out] outValue Pointer to a Uint64 to receive the semaphore value.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `semaphore` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palWaitSemaphore
 * @sa palSignalSemaphore
 */
PAL_API PalResult PAL_CALL palGetSemaphoreValue(
    PalSemaphore* semaphore,
    Uint64* outValue);

/**
 * @brief Create a command pool from a device.
 *
 * The graphics system must be initialized before this call.
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
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroyCommandPool
 */
PAL_API PalResult PAL_CALL palCreateCommandPool(
    PalDevice* device,
    PalQueue* queue,
    PalCommandPool** outPool);

/**
 * @brief Destroy a command pool.
 *
 * The graphics system must be initialized before this call.
 * If the provided command pool is invalid or nullptr, this function returns
 * silently. All command buffers allocated from the command pool must be freed before this
 * function.
 *
 * @param[in] pool Command pool to destroy.
 *
 * Thread safety: Thread safe if the device used to create the command pool is
 * externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateCommandPool
 */
PAL_API void PAL_CALL palDestroyCommandPool(PalCommandPool* pool);

/**
 * @brief Reset all command buffers allocated from the provided command pool.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] pool Command pool to reset its command buffers.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `pool` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palResetCommandPool(PalCommandPool* pool);

/**
 * @brief Allocate a command buffer from the provided command pool.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] device Device to allocate command buffer on.
 * @param[in] pool Command pool to allocate command buffer from.
 * @param[in] type Type of the command buffer (eg. PAL_COMMAND_BUFFER_TYPE_PRIMARY).
 * @param[out] outCmdBuffer Pointer to a PalCommandBuffer to recieve the created command buffer.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` and `pool` are externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palFreeCommandBuffer
 */
PAL_API PalResult PAL_CALL palAllocateCommandBuffer(
    PalDevice* device,
    PalCommandPool* pool,
    PalCommandBufferType type,
    PalCommandBuffer** outCmdbuffer);

/**
 * @brief Free an allocated command buffer.
 *
 * The graphics system must be initialized before this call.
 * If the provided command buffer is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] cmdBuffer Command buffer to free.
 *
 * Thread safety: Thread safe if the command pool used to create the command buffer is
 * externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palAllocateCommandBuffer
 */
PAL_API void PAL_CALL palFreeCommandBuffer(PalCommandBuffer* cmdBuffer);

/**
 * @brief Reset the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer to reset.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palResetCommandBuffer(PalCommandBuffer* cmdBuffer);

/**
 * @brief Submit a command buffer to the provided queue for execution.
 *
 * The graphics system must be initialized before this call. The command buffer must not
 * be in a recording state.
 *
 * @param[in] queue Queue to execute the command buffer.
 * @param[in] info Pointer to a PalCommandBufferSubmitInfo struct that specifies parameters.
 * Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `queue` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palSubmitCommandBuffer(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info);

/**
 * @brief Begin recording commands to the provided command buffer.
 *
 * The graphics system must be initialized before this call. This function must be called
 * before any other `palCmd**` function is used.
 *
 * @param[in] cmdBuffer Command buffer to begin recording.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCmdEnd
 */
PAL_API PalResult PAL_CALL palCmdBegin(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info);

/**
 * @brief End recording commands to the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer to begin recording.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCmdBegin
 */
PAL_API PalResult PAL_CALL palCmdEnd(PalCommandBuffer* cmdBuffer);

/**
 * @brief Execute a secondary command buffer within a primary command buffer.
 *
 * The graphics system must be initialized before this call. The `secondaryCmdBuffer` must
 * be created with the type `PAL_COMMAND_BUFFER_TYPE_SECONDARY`.
 *
 * @param[in] primaryCmdBuffer Primary command buffer. Must be in recording state.
 * @param[in] secondaryCmdBuffer Secondary command buffer.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `primaryCmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdExecuteCommandBuffer(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer);

/**
 * @brief Set the fragment shading rate used for draw calls.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE` must be supported and enabled by the device if not,
 * this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] state Pointer to a PalFragmentShadingRateState struct that specifies parameters.
 * Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdSetFragmentShadingRate(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state);

/**
 * @brief Dispatch mesh shader workgroups.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_MESH_SHADER` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] groupCountX Number of mesh shader groups to dispatch on the x axis.
 * @param[in] groupCountY Number of mesh shader groups to dispatch on the y axis.
 * @param[in] groupCountZ Number of mesh shader groups to dispatch on the z axis.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 * 
 * @note A pipeline must be bound before this call.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palBuildWorkGroupInfo
 */
PAL_API PalResult PAL_CALL palCmdDrawMeshTasks(
    PalCommandBuffer* cmdBuffer,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ);

/**
 * @brief Dispatch mesh shader workgroups using parameters from a buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_MESH_SHADER` and `PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH` must be supported
 * and enabled by the device if not, this function will fail and return
 * `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDispatchIndirectData structs.
 * Can be a single struct.
 * @param[in] drawCount Number of draws to perform.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 * 
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palBuildWorkGroupInfo
 */
PAL_API PalResult PAL_CALL palCmdDrawMeshTasksIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint32 drawCount);

/**
 * @brief Dispatch mesh shader workgroups using parameters from buffers.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_MESH_SHADER` and `PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT` must be
 * supported and enabled by the device if not, this function will fail and return
 * `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDispatchIndirectData structs.
 * Can be a single struct.
 * @param[in] countBuffer Buffer containing a single `Uint32` specifying the number of draws.
 * @param[in] maxDrawCount Maximum Number of draws to perform.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 * 
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palBuildWorkGroupInfo
 */
PAL_API PalResult PAL_CALL palCmdDrawMeshTasksIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint32 maxDrawCount);

/**
 * @brief Build or update an acceleration structure.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] info Pointer to a PalAccelerationStructureBuildInfo struct that specifies parameters.
 * Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdBuildAccelerationStructure(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info);

/**
 * @brief Begin a rendering pass.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] info Pointer to a PalRenderingInfo struct that specifies parameters.
 * Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdBeginRendering(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info);

/**
 * @brief End a rendering pass.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdEndRendering(PalCommandBuffer* cmdBuffer);

/**
 * @brief Copy data from one buffer to the other.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] dst Destination buffer.
 * @param[in] src Source buffer.
 * @param[in] copyInfo Pointer to a PalBufferCopyInfo struct that specifies parameters.
 * Must not be nullptr.
 * 
 * Pointer to a PalImageCreateInfo struct that specifies parameters.
 * Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdCopyBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    PalBufferCopyInfo* copyInfo);

/**
 * @brief Copy data from a buffer to an image.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] dstImage Destination image.
 * @param[in] srcBuffer Source buffer.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 * Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdCopyBufferToImage(
    PalCommandBuffer* cmdBuffer,
    PalImage* dstImage,
    PalBuffer* srcBuffer,
    PalBufferImageCopyInfo* copyInfo);

/**
 * @brief Copy data from one image to the other.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] dst Destination image.
 * @param[in] src Source image.
 * @param[in] copyInfo Pointer to a PalImageCopyInfo struct that specifies parameters.
 * Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdCopyImage(
    PalCommandBuffer* cmdBuffer,
    PalImage* dst,
    PalImage* src,
    PalImageCopyInfo* copyInfo);

/**
 * @brief Copy data from an image to a buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] dstBuffer Destination buffer.
 * @param[in] srcImage Source image.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 * Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdCopyImageToBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dstBuffer,
    PalImage* srcImage,
    PalBufferImageCopyInfo* copyInfo);

/**
 * @brief Bind a pipeline.
 *
 * The graphics system must be initialized before this call. Every pipeline knows it types which is
 * set at the respective creation functions. (`palCreate**Graphics/Compute/RayTracing**Pipeline`).
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] pipeline Pipeline to bind.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdBindPipeline(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline);

/**
 * @brief Set the viewport(s) used in draw commands.
 *
 * The graphics system must be initialized before this call. This always overwrites any previous
 * viewports that were set since the first viewport index is always 0.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] count Capacity of the PalViewport array.
 * @param[in] viewports Pointer to an array of viewports.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdSetViewport(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalViewport* viewports);

/**
 * @brief Set the scissor(s) used in draw commands.
 *
 * The graphics system must be initialized before this call. This always overwrites any previous
 * scissors that were set since the first scissor index is always 0.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] count Capacity of the PalRect2D array.
 * @param[in] scissors Pointer to an array of scissors.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdSetScissors(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalRect2D* scissors);

/**
 * @brief Bind vertex buffer(s) used in draw commands.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] firstSlot Index of the first vertex buffer binding slot.
 * @param[in] count Number of vertex buffers to bind.
 * @param[in] strides Pointer to an array of strides for each vertex buffer.
 * @param[in] buffers Pointer to an array of vertex buffers.
 * @param[in] offsets Pointer to an array of offsets in bytes into each vertex buffer.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 * 
 * @note A pipeline must be bound before this call.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdBindVertexBuffers(
    PalCommandBuffer* cmdBuffer,
    Uint32 firstSlot,
    Uint32 count,
    PalBuffer** buffers,
    Uint64* offsets);

/**
 * @brief Bind index buffer used in draw commands.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Index buffer to bind.
 * @param[in] offset Offset in bytes into the index buffer.
 * @param[in] type Type of indices stored in the index buffer. (eg. `PAL_INDEX_TYPE_UINT32`).
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 * 
 * @note A pipeline must be bound before this call.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdBindIndexBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    PalIndexType type);

/**
 * @brief Issue a non-indexed draw command.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] vertexCount Number of vertices to draw.
 * @param[in] instanceCount Number of instances to draw.
 * @param[in] firstVertex Index of the first vertex to draw.
 * @param[in] firstInstance Index of the first instance to draw.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 * 
 * @note A pipeline must be bound before this call.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDrawIndexed
 */
PAL_API PalResult PAL_CALL palCmdDraw(
    PalCommandBuffer* cmdBuffer,
    Uint32 vertexCount,
    Uint32 instanceCount,
    Uint32 firstVertex,
    Uint32 firstInstance);

/**
 * @brief Issue a non-indexed draw command using buffers.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DRAW` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDrawIndirectData structs.
 * Can be a single struct.
 * @param[in] count Number of draws to perform.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 * 
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDrawIndexedIndirect
 */
PAL_API PalResult PAL_CALL palCmdDrawIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint32 count);

/**
 * @brief Issue a non-indexed draw command using buffers.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT` must be supported and enabled by the device if not,
 * this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDrawIndirectData structs.
 * Can be a single struct.
 * @param[in] countBuffer Buffer containing a single `Uint32` specifying the number of draws.
 * @param[in] maxDrawCount Maximum Number of draws to perform.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 * 
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCmdDrawIndexedIndirectCount
 */
PAL_API PalResult PAL_CALL palCmdDrawIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint32 maxDrawCount);

/**
 * @brief Issue an indexed draw command.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] indexCount Number of indices to draw.
 * @param[in] instanceCount Number of instances to draw.
 * @param[in] firstIndex Index of the first index to draw.
 * @param[in] vertexOffset Added offset to vertex indices.
 * @param[in] firstInstance Index of the first instance to draw.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 * 
 * @note A pipeline must be bound before this call.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDraw
 */
PAL_API PalResult PAL_CALL palCmdDrawIndexed(
    PalCommandBuffer* cmdBuffer,
    Uint32 indexCount,
    Uint32 instanceCount,
    Uint32 firstIndex,
    Int32 vertexOffset,
    Uint32 firstInstance);

/**
 * @brief Issue an indexed draw command using buffers.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DRAW` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDrawIndexedIndirectData structs.
 * Can be a single struct.
 * @param[in] count Number of draws to perform.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 * 
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDrawIndirect
 */
PAL_API PalResult PAL_CALL palCmdDrawIndexedIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint32 count);

/**
 * @brief Issue an indexed draw command using buffers.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT` must be supported and enabled by the device if not,
 * this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDrawIndexedIndirectData structs.
 * Can be a single struct.
 * @param[in] countBuffer Buffer containing a single `Uint32` specifying the number of draws.
 * @param[in] maxDrawCount Maximum Number of draws to perform.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 * 
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCmdDrawIndirectCount
 */
PAL_API PalResult PAL_CALL palCmdDrawIndexedIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint32 maxDrawCount);

/**
 * @brief Transition an acceleration structure from one usage state to another.
 * 
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if not,
 * this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 * 
 * The graphics system must be initialized before this call. This function defines a
 * dependency between `oldUsageStateInfo` and `newUsageStateInfo`. It ensures that all
 * operations performed under `oldUsageStateInfo` are completed and visible before the acceleration
 * structure is accessed under `newUsageStateInfo`.
 * 
 * This function does not modify the acceleration structure, it only exforces execution ordering 
 * and acceleration structure memory visibility.
 * 
 * A barrier is not needed between BLAS and TLAS if there dont shared any resource. If both
 * builds use a different scratch buffer, no barrier is needed.
 * 
 * Example: 
 * 
 * To make sure BLAS builds before TLAS access it and TLAS does not use scratch buffer
 * whilst BLAS is buidling,
 * we put a barrier to transition the BLAS to ensure it has finished building and the scratch
 * buffer is not being used. This is expressed with oldUsageStateInfo.usageState being 
 * `PAL_USAGE_STATE_ACCELERATION_STRUCTURE_WRITE`.
 * 
 * newUsageStateInfo.usageState should be the new usage state we want after the BLAS 
 * has finished wbuilding which is `PAL_USAGE_STATE_ACCELERATION_STRUCTURE_READ`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] as Acceleration structure to set barrier on.
 * @param[in] oldUsageStateInfo Pointer to a PalUsageStateInfo specifying the old usage state.
 * @param[in] newUsageStateInfo Pointer to a PalUsageStateInfo specifying the new usage state.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 * 
 * @note A pipeline must be bound before this call.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCmdImageBarrier
 * @sa palCmdBufferBarrier
 */
PAL_API PalResult PAL_CALL palCmdAccelerationStructureBarrier(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructure* as,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo);

/**
 * @brief Transition an image from one usage state to another.
 *
 * The graphics system must be initialized before this call. This function defines a
 * dependency between `oldUsageStateInfo` and `newUsageStateInfo`. It ensures that all
 * operations performed under `oldUsageStateInfo` are completed and visible before the image 
 * is accessed under `newUsageStateInfo`.
 * 
 * This function does not modify the image, it only exforces execution ordering and image memory 
 * visibility.
 * 
 * Example: 
 * 
 * To make sure the an image is ready for presenting after a render pass,
 * we put a barrier to transition the image to ensure the render pass has finished writing
 * to the image. This is expressed with oldUsageStateInfo.usageState being 
 * `PAL_USAGE_STATE_COLOR_ATTACHMENT` or `PAL_USAGE_STATE_COLOR_ATTACHMENT` or both set 
 * after each other.
 * 
 * newUsageStateInfo.usageState should be the new usage state we want after the render pass 
 * has finished which is `PAL_USAGE_STATE_PRESENT`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] image Image to set barrier on.
 * @param[in] subresourceRange Subresource range of the image.
 * @param[in] oldUsageStateInfo Pointer to a PalUsageStateInfo specifying the old usage state.
 * @param[in] newUsageStateInfo Pointer to a PalUsageStateInfo specifying the new usage state.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCmdAccelerationStructureBarrier
 * @sa palCmdBufferBarrier
 */
PAL_API PalResult PAL_CALL palCmdImageBarrier(
    PalCommandBuffer* cmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo);

/**
 * @brief Transition a buffer from one usage state to another.
 *
 * The graphics system must be initialized before this call. This function defines a
 * dependency between `oldUsageStateInfo` and `newUsageStateInfo`. It ensures that all
 * operations performed under `oldUsageStateInfo` are completed and visible before the buffer 
 * is accessed under `newUsageStateInfo`.
 * 
 * This function does not modify the buffer, it only exforces execution ordering and buffer memory 
 * visibility.
 * 
 * Example: 
 * 
 * To read back data from a buffer that will be written to by a shader,
 * we put a barrier to transition the buffer to ensurethe shader has finished writing to the 
 * buffer. This is expressed with oldUsageStateInfo.usageState being `PAL_USAGE_STATE_SHADER_WRITE`
 * and optional oldUsageStateInfo.shaderStage set to the shader stage that we will be doing the 
 * writing.
 * 
 * newUsageStateInfo.usageState should be the new usage state we want after the write 
 * has finished which is`PAL_USAGE_STATE_TRANSFER_READ`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer to set barrier on.
 * @param[in] oldUsageStateInfo Pointer to a PalUsageStateInfo specifying the old usage state.
 * @param[in] newUsageStateInfo Pointer to a PalUsageStateInfo specifying the new usage state.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCmdAccelerationStructureBarrier
 * @sa palCmdImageBarrier
 */
PAL_API PalResult PAL_CALL palCmdBufferBarrier(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo);

/**
 * @brief Dispatch compute shader workgroups.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] groupCountX Number of compute shader groups to dispatch on the x axis.
 * @param[in] groupCountY Number of compute shader groups to dispatch on the y axis.
 * @param[in] groupCountZ Number of compute shader groups to dispatch on the z axis.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 * 
 * @note A pipeline must be bound before this call.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palBuildWorkGroupInfo
 */
PAL_API PalResult PAL_CALL palCmdDispatch(
    PalCommandBuffer* cmdBuffer,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ);

/**
 * @brief Dispatch compute shader workgroups with base offset.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_DISPATCH_BASE` must be supported and enabled by the device if not, this 
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] baseGroupX Base group offset on the x axis.
 * @param[in] baseGroupY Base group offset on the y axis.
 * @param[in] baseGroupZ Base group offset on the z axis.
 * @param[in] groupCountX Number of compute shader groups to dispatch on the x axis.
 * @param[in] groupCountY Number of compute shader groups to dispatch on the y axis.
 * @param[in] groupCountZ Number of compute shader groups to dispatch on the z axis.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 * 
 * @note A pipeline must be bound before this call.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palBuildWorkGroupInfo
 */
PAL_API PalResult PAL_CALL palCmdDispatchBase(
    PalCommandBuffer* cmdBuffer,
    Uint32 baseGroupX,
    Uint32 baseGroupY,
    Uint32 baseGroupZ,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ);

/**
 * @brief Dispatch compute shader workgroups using parameters from a buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DISPATCH` must be supported and enabled by the device if not, this 
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing the PalDispatchIndirectData struct. Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 * 
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palBuildWorkGroupInfo
 */
PAL_API PalResult PAL_CALL palCmdDispatchIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer);

/**
 * @brief Dispatch rays.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if not,
 * this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] sbt The shader binding table to use.
 * @param[in] raygenIndex Index of the raygen shader to execute.
 * @param[in] width Number of rays to trace on the x axis.
 * @param[in] height Number of rays to trace on the y axis.
 * @param[in] depth Number of rays to trace on the z axis.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 * 
 * @note A pipeline must be bound before this call.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdTraceRays(
    PalCommandBuffer* cmdBuffer,
    PalShaderBindingTable* sbt,
    Uint32 raygenIndex,
    Uint32 width,
    Uint32 height,
    Uint32 depth);

/**
 * @brief Dispatch rays using parameters from a buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if not, 
 * this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] raygenIndex Index of the raygen shader to execute.
 * @param[in] sbt The shader binding table to use.
 * @param[in] buffer Buffer containing the PalDispatchIndirectData struct. Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 * 
 * @note The argument buffer memory must not be `PAL_MEMORY_TYPE_GPU_ONLY`. The implementation
 * internally copies the data into a GPU buffer for execution.
 * 
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdTraceRaysIndirect(
    PalCommandBuffer* cmdBuffer,
    Uint32 raygenIndex,
    PalShaderBindingTable* sbt,
    PalBuffer* buffer);

/**
 * @brief Bind a descriptor set to the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] setIndex Index of the descriptor set to bind.
 * @param[in] set Descriptor set to bind. Must be compatible with `layout`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 * 
 * @note A pipeline must be bound before this call.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdBindDescriptorSet(
    PalCommandBuffer* cmdBuffer,
    Uint32 setIndex,
    PalDescriptorSet* set);

/**
 * @brief Update push constant data for the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] shaderStageCount Capacity of the PalShaderStage array.
 * @param[in] shaderStages Array of shader stages that can access the push constant.
 * @param[in] offset Offset in bytes into the push constant range.
 * @param[in] size Size of `value` in bytes.
 * @param[in] value Pointer to the push constant range data to write.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 * 
 * @note A pipeline must be bound before this call.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdPushConstants(
    PalCommandBuffer* cmdBuffer,
    Uint32 shaderStageCount,
    PalShaderStage* shaderStages,
    Uint32 offset,
    Uint32 size,
    const void* value);

/**
 * @brief Set the cull mode for the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE` must be supported and enabled by the device if not,
 * this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] cullMode Cull mode to set.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdSetCullMode(
    PalCommandBuffer* cmdBuffer,
    PalCullMode cullMode);

/**
 * @brief Set the front face for the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE` must be supported and enabled by the device if not,
 * this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] frontFace Front face to set.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdSetFrontFace(
    PalCommandBuffer* cmdBuffer,
    PalFrontFace frontFace);

/**
 * @brief Set the primitive topology for the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY` must be supported and enabled by the device 
 * if not, this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] topology Topology to set.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdSetPrimitiveTopology(
    PalCommandBuffer* cmdBuffer,
    PalPrimitiveTopology topology);

/**
 * @brief Set depth test enable for the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE` must be supported and enabled by the device 
 * if not, this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] enable True to enable.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdSetDepthTestEnable(
    PalCommandBuffer* cmdBuffer,
    bool enable);

/**
 * @brief Set depth write enable for the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE` must be supported and enabled by the device
 * if not, this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] enable True to enable.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdSetDepthWriteEnable(
    PalCommandBuffer* cmdBuffer,
    bool enable);

/**
 * @brief Set depth stencil operation for the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP` must be supported and enabled by the device
 * if not, this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] faceMask Bitmask specifying faces to apply the stencil to.
 * @param[in] failOp Stencil operation to perform when stencil fails.
 * @param[in] passOp Stencil operation to perform when stencil and depth passes.
 * @param[in] depthFailOp Stencil operation to perform when stencil passes but depth fails.
 * @param[in] compareOp Compare operation for stencil tests.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palCmdSetStencilOp(
    PalCommandBuffer* cmdBuffer,
    PalStencilFaceFlags faceMask,
    PalStencilOp failOp,
    PalStencilOp passOp,
    PalStencilOp depthFailOp,
    PalCompareOp compareOp);

/**
 * @brief Create an acceleration structure.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device that creates the acceleration structure.
 * @param[in] info Pointer to a PalAccelerationStructureCreateInfo struct that specifies parameters.
 * Must not be nullptr.
 * @param[out] outAs Pointer to a PalAccelerationStructure to recieve the created acceleration
 * structure.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 * 
 * @note The memory associated with PalAccelerationStructureCreateInfo::buffer must be 
 * `PAL_MEMORY_TYPE_GPU_ONLY`.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroyAccelerationstructure
 */
PAL_API PalResult PAL_CALL palCreateAccelerationstructure(
    PalDevice* device,
    const PalAccelerationStructureCreateInfo* info,
    PalAccelerationStructure** outAs);

/**
 * @brief Destroy an acceleration structure.
 *
 * The graphics system must be initialized before this call.
 * If the provided acceleration structure is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] as Acceleration structure to destroy.
 *
 * Thread safety: Thread safe if the device used to create the acceleration structure is
 * externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateAccelerationstructure
 */
PAL_API void PAL_CALL palDestroyAccelerationstructure(PalAccelerationStructure* as);

/**
 * @brief Get the build size of an acceleration structure.
 *
 * The graphics system must be initialized before this call.
 * PalAccelerationStructureBuildInfo::dst, PalAccelerationStructureBuildInfo::scratchBufferAddress
 * and PalAccelerationStructureBuildInfo::src must be set to nullptr.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query.
 * @param[in] info Pointer to a PalAccelerationStructureBuildInfo struct that specifies parameters.
 * Must not be nullptr.
 * @param[out] size Pointer to a PalAccelerationStructureBuildSize to recieve the build size.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palGetAccelerationStructureBuildSize(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size);

/**
 * @brief Create a buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS` must be supported and enabled by the device if 
 * `PAL_BUFFER_USAGE_DEVICE_ADDRESS` will be used.
 * 
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if 
 * `PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE` or `PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_SCRATCH`
 * or `PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_READ_ONLY_INPUT` will be used.
 * 
 * `PAL_BUFFER_USAGE_INDIRECT` must be supported and enabled by the device if the buffer will be
 * used as an indirect buffer.
 * 
 * @param[in] device Device that creates the buffer.
 * @param[in] info Pointer to a PalBufferCreateInfo struct that specifies parameters.
 * Must not be nullptr.
 * @param[out] outBuffer Pointer to a PalBuffer to recieve the created buffer.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroyBuffer
 */
PAL_API PalResult PAL_CALL palCreateBuffer(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer);

/**
 * @brief Destroy a buffer.
 *
 * The graphics system must be initialized before this call.
 * If the provided buffer is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] buffer buffer to destroy.
 *
 * Thread safety: Thread safe if the device used to create the buffer is
 * externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateBuffer
 */
PAL_API void PAL_CALL palDestroyBuffer(PalBuffer* buffer);

/**
 * @brief Get memory requirements for the provided buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] buffer Buffer to query memory requirements on.
 * @param[out] requirements Pointer to a PalMemoryRequirements to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `requirements` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palGetBufferMemoryRequirements(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements);

/**
 * @brief Compute size and alignment requirements for an instance buffer.
 *
 * The graphics system must be initialized before this call. This does not allocate memory
 * for the buffer.
 *
 * `outSize` must be the size that is used to create the instance buffer. It will be 
 * computed with regards to the provided `instanceCount`. This function must be used and required
 * for all instance buffers. This is used with acceleration 
 * structure (`TLAS`).
 *
 * @param[in] device Device to compute instance buffer requirements with.
 * @param[in] instanceCount Number of instances the instance buffer will hold.
 * @param[out] outSize Pointer to a Uint64 to recieve the required size.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palComputeInstanceBufferRequirements(
    PalDevice* device,
    Uint32 instanceCount,
    Uint64* outSize);

/**
 * @brief Compute size and alignment requirements for an image copy staging buffer.
 *
 * The graphics system must be initialized before this call. This does not allocate memory
 * for the buffer.
 *
 * `outSize` must be the size that is used to create the image copy staging buffer. It will be 
 * computed with regards to the provided `imageFormat` and `copyInfo`. This function must be 
 * used and required for all image copy staging buffers. This is used with image copy commands.
 * 
 * PalBufferImageCopyInfo::bufferRowLength and PalBufferImageCopyInfo::bufferImageHeight are hints.
 * The driver might used it defaults if the requested is not supported. Check `outBufferRowLength`
 * and `outBufferImageHeight` to see the values the driver used. Set the new values to 
 * `copyInfo` before writing to the buffer with `palWriteToImageCopyStagingBuffer()`.
 *
 * @param[in] device Device to compute image copy staging buffer requirements with.
 * @param[in] imageFormat Destination image format.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 * Must not be nullptr.
 * @param[out] outBufferRowLength Pointer to a Uint32 to recieve the required buffer row length.
 * @param[out] outBufferImageHeight Pointer to a Uint32 to recieve the required buffer imag height.
 * @param[out] outSize Pointer to a Uint64 to recieve the required size.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palComputeImageCopyStagingBufferRequirements(
    PalDevice* device,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo,
    Uint32* outBufferRowLength,
    Uint32* outBufferImageHeight,
    Uint64* outSize);

/**
 * @brief Update or write data to an instance buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] device The device. Must match the one used to create the instance buffer.
 * @param[out] ptr Pointer to the CPU visible memory. Must be mapped.
 * @param[in] instances Array of PalAccelerationStructureInstance struct to write.
 * Can be a single struct.
 * @param[in] instanceCount Number of instances in `instances`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palWriteToInstanceBuffer(
    PalDevice* device,
    void* ptr,
    PalAccelerationStructureInstance* instances,
    Uint32 instanceCount);

/**
 * @brief Update or write data to an image copy staging buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] device The device. Must match the one used to create the image copy staging buffer.
 * @param[out] ptr Pointer to the CPU visible memory. Must be mapped.
 * @param[out] srcData Pointer to the CPU visible memory with the data.
 * @param[in] imageFormat Destination image format.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 * Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palWriteToImageCopyStagingBuffer(
    PalDevice* device,
    void* ptr,
    void* srcData,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo);

/**
 * @brief Bind an allocated memory to a buffer.
 *
 * The graphics system must be initialized before this call.
 * The memory size and alignment should match the requirements of the buffer.
 * Get the requirements with palGetBufferMemoryRequirements().
 *
 * @param[in] buffer Buffer to bind memory to.
 * @param[in] memory Memory to bind. Must not be nullptr.
 * @param[in] offset Starting point within the memory.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `requirements` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palGetBufferMemoryRequirements
 */
PAL_API PalResult PAL_CALL palBindBufferMemory(
    PalBuffer* buffer,
    PalMemory* memory,
    Uint64 offset);

/**
 * @brief Maps buffer to CPU visible address space.
 *
 * The graphics system must be initialized before this call. The buffer must have a valid
 * memory bound to it before this call.
 *
 * Only `PAL_MEMORY_TYPE_CPU_UPLOAD` and `PAL_MEMORY_TYPE_CPU_READBACK` can be mapped to
 * CPU visible space. Mapping `PAL_MEMORY_TYPE_GPU_ONLY` will fail and return
 * `PAL_RESULT_MEMORY_MAP_FAILED`.
 *
 * @param[in] buffer Pointer to buffer to map. Memory must be bound.
 * @param[in] offset Starting point within the buffer.
 * @param[in] size Number of bytes to map from the offset. `offset + size` must not be
 * greater than buffer size.
 * @param[out] outPtr Pointer to a void* to recieved the mapped memory.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `buffer` is externally synchronized.
 * Mapping with different offsets into the same buffer is thread safe as long as `buffer`
 * is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palUnmapBufferMemory
 */
PAL_API PalResult PAL_CALL palMapBufferMemory(
    PalBuffer* buffer,
    Uint64 offset,
    Uint64 size,
    void** outPtr);

/**
 * @brief Unmap buffer from CPU visible address space.
 *
 * The graphics system must be initialized before this call. The buffer must be mapped
 * before this call. After this call, the CPU pointer must not be used anymore.
 *
 * @param[in] buffer Pointer to buffer to unmap.
 *
 * Thread safety: Thread safe if `buffer` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palMapBufferMemory
 */
PAL_API void PAL_CALL palUnmapBufferMemory(PalBuffer* buffer);

/**
 * @brief Get the device address of the provided buffer.
 *
 * The graphics system must be initialized before this call. Buffer must have
 * `PAL_BUFFER_USAGE_DEVICE_ADDRESS` usage flag.
 *
 * @param[in] buffer Buffer to get its device address.
 *
 * @return Buffer device address on success or `0` on failure.
 *
 * Thread safety: Thread safe if `buffer` is per thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalDeviceAddress PAL_CALL palGetBufferDeviceAddress(PalBuffer* buffer);

/**
 * @brief Create a descriptor set layout that defines the bindings used by descriptor sets.
 *
 * The graphics system must be initialized before this call.
 * 
 * Set PalDescriptorSetLayoutCreateInfo::enableDescriptorIndexing to true to enable
 * descriptor indexing on the descriptor set layout. `PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING`
 * must be supported and enabled when creating the device if not, this function will fail and 
 * return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 * 
 * This defines the layout, ordering and the number of descriptors a descriptor set uses.
 * 
 * The layouts should reflect the exact layout of the shaders. Eg. 
 * descriptorBindings[2] = { sampler, sampled image } is different from 
 * descriptorBindings[2] = { sampled image, sampler }. The ordering must be correct.
 *
 * @param[in] device Device that creates the descriptor set layout.
 * @param[in] info Pointer to a PalDescriptorSetLayoutCreateInfo struct that specifies parameters.
 * Must not be nullptr.
 * @param[out] outLayout Pointer to a PalDescriptorSetLayout to recieve the created descriptor
 * set layout.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroyDescriptorSetLayout
 */
PAL_API PalResult PAL_CALL palCreateDescriptorSetLayout(
    PalDevice* device,
    const PalDescriptorSetLayoutCreateInfo* info,
    PalDescriptorSetLayout** outLayout);

/**
 * @brief Destroy a descriptor set layout.
 *
 * The graphics system must be initialized before this call.
 * If the provided descriptor set layout is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] layout Descriptor set layout to destroy.
 *
 * Thread safety: Thread safe if the device used to create the descriptor set layout is
 * externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateDescriptorSetLayout
 */
PAL_API void PAL_CALL palDestroyDescriptorSetLayout(PalDescriptorSetLayout* layout);

/**
 * @brief Create a descriptor pool to allocate descriptor sets.
 *
 * The graphics system must be initialized before this call.
 * 
 * Set PalDescriptorPoolCreateInfo::enableDescriptorIndexing to true to enable
 * descriptor indexing on the descriptor pool. `PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING`
 * must be supported and enabled when creating the device if not, this function will fail and 
 * return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device that creates the descriptor pool.
 * @param[in] info Pointer to a PalDescriptorPoolCreateInfo struct that specifies parameters.
 * Must not be nullptr.
 * @param[out] outPool Pointer to a PalDescriptorPool to recieve the created descriptor pool.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroyDescriptorPool
 */
PAL_API PalResult PAL_CALL palCreateDescriptorPool(
    PalDevice* device,
    const PalDescriptorPoolCreateInfo* info,
    PalDescriptorPool** outPool);

/**
 * @brief Destroy a descriptor pool.
 *
 * The graphics system must be initialized before this call.
 * If the provided descriptor pool is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] pool Descriptor pool to destroy.
 *
 * Thread safety: Thread safe if the device used to create the descriptor pool is
 * externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateDescriptorPool
 */
PAL_API void PAL_CALL palDestroyDescriptorPool(PalDescriptorPool* pool);

/**
 * @brief Reset the provided descriptor pool. This resets all allocated descriptor sets.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] pool Descriptor pool to reset.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `pool` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palResetDescriptorPool(PalDescriptorPool* pool);

/**
 * @brief Allocate a descriptor set from the provided descriptor pool.
 *
 * The graphics system must be initialized before this call. The descriptor set will be
 * allocated uninitialized therefore update it before use except the case where descriptor
 * indexing is enabled.
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
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palAllocateDescriptorSet(
    PalDevice* device,
    PalDescriptorPool* pool,
    PalDescriptorSetLayout* layout,
    PalDescriptorSet** outSet);

/**
 * @brief Update a descriptor set with descriptors (resources).
 *
 * The graphics system must be initialized before this call.
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
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palUpdateDescriptorSet(
    PalDevice* device,
    Uint32 count,
    PalDescriptorSetWriteInfo* infos);

/**
 * @brief Create a pipeline layout. This defines the descriptor set interfaces and push
 * constant ranges.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] device Device that creates the pipeline layout.
 * @param[in] info Pointer to a PalPipelineLayoutCreateInfo struct that specifies parameters.
 * Must not be nullptr.
 * @param[out] outLayout Pointer to a PalPipelineLayout to recieve the created pipeline layout.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroyPipelineLayout
 */
PAL_API PalResult PAL_CALL palCreatePipelineLayout(
    PalDevice* device,
    const PalPipelineLayoutCreateInfo* info,
    PalPipelineLayout** outLayout);

/**
 * @brief Destroy a pipeline layout.
 *
 * The graphics system must be initialized before this call.
 * If the provided pipeline layout is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] layout Pipeline layout to destroy.
 *
 * Thread safety: Thread safe if the device used to create the pipeline layout is
 * externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreatePipelineLayout
 */
PAL_API void PAL_CALL palDestroyPipelineLayout(PalPipelineLayout* layout);

/**
 * @brief Create a graphics pipeline.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] device Device that creates the graphics pipeline.
 * @param[in] info Pointer to a PalGraphicsPipelineCreateInfo struct that specifies parameters.
 * Must not be nullptr.
 * @param[out] outPipeline Pointer to a PalPipeline to recieve the created buffer.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroyPipeline
 */
PAL_API PalResult PAL_CALL palCreateGraphicsPipeline(
    PalDevice* device,
    const PalGraphicsPipelineCreateInfo* info,
    PalPipeline** outPipeline);

/**
 * @brief Create a compute pipeline.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] device Device that creates the compute pipeline.
 * @param[in] info Pointer to a PalComputePipelineCreateInfo struct that specifies parameters.
 * Must not be nullptr.
 * @param[out] outPipeline Pointer to a PalPipeline to recieve the created buffer.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 * 
 * @note The first entry of the compute shader will be used.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroyPipeline
 */
PAL_API PalResult PAL_CALL palCreateComputePipeline(
    PalDevice* device,
    const PalComputePipelineCreateInfo* info,
    PalPipeline** outPipeline);

/**
 * @brief Create a ray tracing pipeline.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device that creates the ray tracing pipeline.
 * @param[in] info Pointer to a PalRayTracingPipelineCreateInfo struct that specifies parameters.
 * Must not be nullptr.
 * @param[out] outPipeline Pointer to a PalPipeline to recieve the created buffer.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 * 
 * @note The shader group array must be in this order [raygen][miss][hitgroup][callable]. 
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroyPipeline
 */
PAL_API PalResult PAL_CALL palCreateRayTracingPipeline(
    PalDevice* device,
    const PalRayTracingPipelineCreateInfo* info,
    PalPipeline** outPipeline);

/**
 * @brief Destroy a pipeline.
 *
 * The graphics system must be initialized before this call.
 * If the provided pipeline is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] pipeline Pipeline to destroy.
 *
 * Thread safety: Thread safe if the device used to create the pipeline is
 * externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateGraphicsPipeline
 * @sa palCreateComputePipeline
 * @sa palCreateRayTracingPipeline
 */
PAL_API void PAL_CALL palDestroyPipeline(PalPipeline* pipeline);

/**
 * @brief Create a shader binding table.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 * 
 * PalShaderBindingTableCreateInfo::recordCount must match the shader group count of 
 * PalShaderBindingTableCreateInfo::rayTracingPipeline.
 *
 * @param[in] device Device that creates the shader binding table.
 * @param[in] info Pointer to a PalShaderBindingTableCreateInfo struct that specifies parameters.
 * Must not be nullptr.
 * @param[out] outSbt Pointer to a PalShaderBindingTable to recieve the created shader binding 
 * table.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 * 
 * @note The records array must be in this order [raygen][miss][hitgroup][callable]. 
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palDestroyShaderBindingTable
 */
PAL_API PalResult PAL_CALL palCreateShaderBindingTable(
    PalDevice* device,
    const PalShaderBindingTableCreateInfo* info,
    PalShaderBindingTable** outSbt);

/**
 * @brief Destroy a shader binding table.
 *
 * The graphics system must be initialized before this call.
 * If the provided shader binding table is invalid or nullptr, this function returns
 * silently.
 *
 * @param[in] sbt Shader binding table to destroy.
 *
 * Thread safety: Thread safe if the device used to create the shader binding table is
 * externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCreateShaderBindingTable
 */
PAL_API void PAL_CALL palDestroyShaderBindingTable(PalShaderBindingTable* sbt);

/**
 * @brief Update a shader binding table record payloads.
 *
 * The graphics system must be initialized before this call.
 * 
 * This call does not update shader handles. It only updates the payload associated
 * with the record. PalShaderBindingTableRecordInfo::groupIndex is the index into
 * the shader groups used to create the ray tracing pipeline.
 *
 * @param[in] sbt The shader binding table to update.
 * @param[in] count Capacity of the PalShaderBindingTableRecordInfo array.
 * @param[in] infos Array of PalShaderBindingTableRecordInfo to update.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `sbt` is externally synchronized.
 *
 * @since 1.4
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palUpdateShaderBindingTable(
    PalShaderBindingTable* sbt, 
    Uint32 count,
    PalShaderBindingTableRecordInfo* infos);

/**
 * @brief Build work group info(s) from work inputs specified in pixels, vertices etc.
 *
 * Call this function first with PalWorkGroupInfo array set to nullptr to get the number of work
 * group infos. Allocate memory for the PalWorkGroupInfo array and passed in the count and the
 * allocated array. If the count of the array is less than the number of work group infos, PAL will
 * write upto that limit.
 *
 * If the count is 0 and the PalWorkGroupInfo array is nullptr, the function fails
 * and returns `false`.
 *
 * This function works the maths for how many work groups to dispatch in each axis and how many
 * times it needs to be dispatch in order for the work to be done. It works well with
 * palCmdDispatchBase() since its also gives the base for each work group.
 *
 * @param[in] data Pointer to a PalWorkGroupBuildData with paramters.
 * @param[in, out] count Capacity of the PalWorkGroupInfo array.
 * @param[out] infos Pointer to an Array of PalWorkGroupInfo.
 *
 * @return True on success otherwise false.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 1.4
 * @ingroup pal_graphics
 * @sa palCmdDrawMeshTasks
 * @sa palCmdDrawMeshTasksIndirect
 * @sa palCmdDrawMeshTasksIndirectCount
 * @sa palCmdDispatch
 * @sa palCmdDispatchBase
 */
PAL_API bool PAL_CALL palBuildWorkGroupInfo(
    const PalWorkGroupBuildData* data,
    Int32* count,
    PalWorkGroupInfo* info);

/** @} */ // end of pal_graphics group

#endif // _PAL_GRAPHICS_H
