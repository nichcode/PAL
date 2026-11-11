/**
 * @file types.h
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

#ifndef PAL_GRAPHICS_TYPES_H
#define PAL_GRAPHICS_TYPES_H

#include "pal2/core/types.h"

/**
 * @struct PalAdapter
 * @brief Opaque handle to an adapter.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalAdapter PalAdapter;

/**
 * @struct PalDevice
 * @brief Opaque handle to a graphics device.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDevice PalDevice;

/**
 * @struct PalMemory
 * @brief Opaque handle to a graphics device memory.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalMemory PalMemory;

/**
 * @struct PalQueue
 * @brief Opaque handle to a queue.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalQueue PalQueue;

/**
 * @struct PalSurface
 * @brief Opaque handle to a surface.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalSurface PalSurface;

/**
 * @struct PalSwapchain
 * @brief Opaque handle to a swapchain.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalSwapchain PalSwapchain;

/**
 * @struct PalImage
 * @brief Opaque handle to an image.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalImage PalImage;

/**
 * @struct PalImageView
 * @brief Opaque handle to an image view.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalImageView PalImageView;

/**
 * @struct PalShader
 * @brief Opaque handle to a shader.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalShader PalShader;

/**
 * @struct PalBuffer
 * @brief Opaque handle to a buffer.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalBuffer PalBuffer;

/**
 * @struct PalFence
 * @brief Opaque handle to a fence.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalFence PalFence;

/**
 * @struct PalSemaphore
 * @brief Opaque handle to a semaphore.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalSemaphore PalSemaphore;

/**
 * @struct PalCommandPool
 * @brief Opaque handle to a command pool.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalCommandPool PalCommandPool;

/**
 * @struct PalCommandBuffer
 * @brief Opaque handle to a command buffer.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalCommandBuffer PalCommandBuffer;

/**
 * @struct PalDescriptorSetLayout
 * @brief Opaque handle to a descriptor set layout.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorSetLayout PalDescriptorSetLayout;

/**
 * @struct PalDescriptorPool
 * @brief Opaque handle to a descriptor pool.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorPool PalDescriptorPool;

/**
 * @struct PalDescriptorSet
 * @brief Opaque handle to a descriptor set.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorSet PalDescriptorSet;

/**
 * @struct PalSampler
 * @brief Opaque handle to a sampler.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalSampler PalSampler;

/**
 * @struct PalPipelineLayout
 * @brief Opaque handle to a pipeline layout.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalPipelineLayout PalPipelineLayout;

/**
 * @struct PalPipeline
 * @brief Opaque handle to a pipeline.
 *
 * This is the handle used for all pipeline types (Graphics, Compute, Ray tracing, ...).
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalPipeline PalPipeline;

/**
 * @struct PalShaderBindingTable
 * @brief Opaque handle to a shader binding table.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalShaderBindingTable PalShaderBindingTable;

/**
 * @struct PalAccelerationStructure
 * @brief Opaque handle to an acceleration structure.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalAccelerationStructure PalAccelerationStructure;

/**
 * @typedef PalDebugMessageSeverity
 * @brief Debug message severity.
 *
 * Identifies how severe a debug message is.
 *
 * All values of this type follow the format `PAL_DEBUG_MESSAGE_SEVERITY_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalDebugMessageSeverity;

/**
 * @typedef PalDebugMessageType
 * @brief Debug message type.
 *
 * Identifies the category of a debug message.
 *
 * All values of this type follow the format `PAL_DEBUG_MESSAGE_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalDebugMessageType;

/**
 * @typedef PalDeviceAddress
 * @brief Device address.
 *
 * Identifies the address of a buffer in GPU memory.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint64_t PalDeviceAddress;

/**
 * @typedef PalAdapterFeatures
 * @brief Adapter features.
 *
 * Identifies the optional features of an adapter.
 *
 * This is a bitmask of all supported features of an adapter.
 *
 * All values of this type follow the format `PAL_ADAPTER_FEATURE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint64_t PalAdapterFeatures;

/**
 * @typedef PalAdapterType
 * @brief Adapter type.
 *
 * Identifies the kind of an adapter.
 *
 * All values of this type follow the format `PAL_ADAPTER_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalAdapterType;

/**
 * @typedef PalAdapterApiType
 * @brief Adapter API type.
 *
 * Identifies the graphics API behind an adapter.
 *
 * Custom backends that do not fit the declared API types may use
 * `PAL_ADAPTER_API_TYPE_CUSTOM`.
 *
 * All values of this type follow the format `PAL_ADAPTER_API_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalAdapterApiType;

/**
 * @typedef PalQueueType
 * @brief Queue type.
 *
 * Identifies the kind of work a queue accepts.
 *
 * All values of this type follow the format `PAL_QUEUE_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalQueueType;

/**
 * @typedef PalPresentMode
 * @brief Swapchain present mode.
 *
 * Identifies how swapchain images are presented to a surface.
 *
 * All values of this type follow the format `PAL_PRESENT_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalPresentMode;

/**
 * @typedef PalCompositeAplha
 * @brief Swapchain composite alpha.
 *
 * Identifies how a swapchain is composited with other surfaces.
 *
 * All values of this type follow the format `PAL_COMPOSITE_ALPHA_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalCompositeAplha;

/**
 * @typedef PalFormat
 * @brief Image format.
 *
 * Identifies the texel format of an image.
 *
 * All values of this type follow the format `PAL_FORMAT_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalFormat;

/**
 * @typedef PalImageUsages
 * @brief Image usages.
 *
 * Identifies how an image may be used.
 *
 * Multiple image usages may be OR'ed together using bitwise OR operator (`|`).
 *
 * All values of this type follow the format `PAL_IMAGE_USAGE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalImageUsages;

/**
 * @typedef PalShaderFormats
 * @brief Shader formats.
 *
 * Identifies the code formats of a shader.
 *
 * This is a bitmask of all supported shader formats of an adapter.
 *
 * All values of this type follow the format `PAL_SHADER_FORMAT_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalShaderFormats;

/**
 * @typedef PalLoadOp
 * @brief Load operation.
 *
 * Identifies what happens to the contents of an attachment when rendering begins.
 *
 * All values of this type follow the format `PAL_LOAD_OP_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalLoadOp;

/**
 * @typedef PalStoreOp
 * @brief Store operation.
 *
 * Identifies what happens to the contents of an attachment when rendering ends.
 *
 * All values of this type follow the format `PAL_STORE_OP_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalStoreOp;

/**
 * @typedef PalMemoryType
 * @brief Memory type.
 *
 * Identifies the kind of device memory.
 *
 * All values of this type follow the format `PAL_MEMORY_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalMemoryType;

/**
 * @typedef PalImageType
 * @brief Image type.
 *
 * Identifies the dimensionality of an image.
 *
 * All values of this type follow the format `PAL_IMAGE_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalImageType;

/**
 * @typedef PalImageAspect
 * @brief Image aspect.
 *
 * Identifies which aspects of an image are accessed.
 *
 * All values of this type follow the format `PAL_IMAGE_ASPECT_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalImageAspect;

/**
 * @typedef PalImageViewType
 * @brief Image view type.
 *
 * Identifies how an image view interprets its image.
 *
 * All values of this type follow the format `PAL_IMAGE_VIEW_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalImageViewType;

/**
 * @typedef PalFilterMode
 * @brief Filter mode.
 *
 * Identifies how texels are filtered when sampling.
 *
 * All values of this type follow the format `PAL_FILTER_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalFilterMode;

/**
 * @typedef PalSamplerMipmapMode
 * @brief Sampler mipmap mode.
 *
 * Identifies how mipmap levels are selected when sampling.
 *
 * All values of this type follow the format `PAL_SAMPLER_MIPMAP_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalSamplerMipmapMode;

/**
 * @typedef PalSamplerAddressMode
 * @brief Sampler address mode.
 *
 * Identifies how coordinates outside an image are handled when sampling.
 *
 * All values of this type follow the format `PAL_SAMPLER_ADDRESS_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalSamplerAddressMode;

/**
 * @typedef PalBorderColor
 * @brief Sampler border color.
 *
 * Identifies the color used outside an image with clamp-to-border addressing.
 *
 * All values of this type follow the format `PAL_BORDER_COLOR_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalBorderColor;

/**
 * @typedef PalSurfaceFormat
 * @brief Surface format.
 *
 * Identifies the format and color space of a swapchain.
 *
 * All values of this type follow the format `PAL_SURFACE_FORMAT_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalSurfaceFormat;

/**
 * @typedef PalWindowInstanceType
 * @brief Window instance type.
 *
 * Identifies the windowing system a surface is created for.
 *
 * All values of this type follow the format `PAL_WINDOW_INSTANCE_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalWindowInstanceType;

/**
 * @typedef PalShaderStage
 * @brief Shader stage.
 *
 * Identifies the pipeline stage a shader runs in.
 *
 * All values of this type follow the format `PAL_SHADER_STAGE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalShaderStage;

/**
 * @typedef PalSampleCount
 * @brief Sample count.
 *
 * Identifies the number of samples per pixel.
 *
 * All values of this type follow the format `PAL_SAMPLE_COUNT_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalSampleCount;

/**
 * @typedef PalPrimitiveTopology
 * @brief Primitive topology.
 *
 * Identifies how vertices are assembled into primitives.
 *
 * All values of this type follow the format `PAL_PRIMITIVE_TOPOLOGY_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalPrimitiveTopology;

/**
 * @typedef PalCullMode
 * @brief Cull mode.
 *
 * Identifies which faces are culled.
 *
 * All values of this type follow the format `PAL_CULL_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalCullMode;

/**
 * @typedef PalFrontFace
 * @brief Front face.
 *
 * Identifies the winding order of front-facing triangles.
 *
 * All values of this type follow the format `PAL_FRONT_FACE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalFrontFace;

/**
 * @typedef PalPolygonMode
 * @brief Polygon mode.
 *
 * Identifies how polygons are rasterized.
 *
 * All values of this type follow the format `PAL_POLYGON_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalPolygonMode;

/**
 * @typedef PalStencilFaceFlags
 * @brief Stencil face flags.
 *
 * Identifies the faces a stencil operation applies to.
 *
 * Multiple stencil face flags can be OR'ed together using bitwise OR operator (`|`).
 *
 * All values of this type follow the format `PAL_STENCIL_FACE_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalStencilFaceFlags;

/**
 * @typedef PalVertexType
 * @brief Vertex attribute type.
 *
 * Identifies the data type of a vertex attribute.
 *
 * All values of this type follow the format `PAL_VERTEX_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalVertexType;

/**
 * @typedef PalVertexSemanticID
 * @brief Vertex semantic identifier.
 *
 * Identifies the meaning of a vertex attribute.
 *
 * All values of this type follow the format `PAL_VERTEX_SEMANTIC_ID_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalVertexSemanticID;

/**
 * @typedef PalCommandBufferType
 * @brief Command buffer type.
 *
 * Identifies the level of a command buffer.
 *
 * All values of this type follow the format `PAL_COMMAND_BUFFER_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalCommandBufferType;

/**
 * @typedef PalVertexLayoutType
 * @brief Vertex layout type.
 *
 * Identifies the rate at which vertex attributes advance.
 *
 * All values of this type follow the format `PAL_VERTEX_LAYOUT_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalVertexLayoutType;

/**
 * @typedef PalCompareOp
 * @brief Compare operation.
 *
 * Identifies how two values are compared.
 *
 * All values of this type follow the format `PAL_COMPARE_OP_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalCompareOp;

/**
 * @typedef PalStencilOp
 * @brief Stencil operation.
 *
 * Identifies the action taken on a stencil value.
 *
 * All values of this type follow the format `PAL_STENCIL_OP_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalStencilOp;

/**
 * @typedef PalBlendOp
 * @brief Blend operation.
 *
 * Identifies how source and destination colors are combined.
 *
 * All values of this type follow the format `PAL_BLEND_OP_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalBlendOp;

/**
 * @typedef PalBlendFactor
 * @brief Blend factor.
 *
 * Identifies the factor applied to a color during blending.
 *
 * All values of this type follow the format `PAL_BLEND_FACTOR_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalBlendFactor;

/**
 * @typedef PalColorMask
 * @brief Color mask.
 *
 * Identifies the color channels that are written.
 *
 * Multiple color masks can be OR'ed together using bitwise OR operator (`|`).
 *
 * All values of this type follow the format `PAL_COLOR_MASK_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalColorMask;

/**
 * @typedef PalResolveMode
 * @brief Resolve mode.
 *
 * Identifies how multisampled attachments are resolved.
 *
 * All values of this type follow the format `PAL_RESOLVE_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalResolveMode;

/**
 * @typedef PalFragmentShadingRate
 * @brief Fragment shading rate.
 *
 * Identifies the size of the pixel area shaded by one fragment.
 *
 * All values of this type follow the format `PAL_FRAGMENT_SHADING_RATE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalFragmentShadingRate;

/**
 * @typedef PalFragmentShadingRateCombinerOp
 * @brief Fragment shading rate combiner operation.
 *
 * Identifies how fragment shading rates are combined.
 *
 * All values of this type follow the format `PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalFragmentShadingRateCombinerOp;

/**
 * @typedef PalAccelerationStructureType
 * @brief Acceleration structure type.
 *
 * Identifies the level of an acceleration structure.
 *
 * All values of this type follow the format `PAL_ACCELERATION_STRUCTURE_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalAccelerationStructureType;

/**
 * @typedef PalAccelerationStructureBuildMode
 * @brief Acceleration structure build mode.
 *
 * Identifies whether an acceleration structure is built or updated.
 *
 * All values of this type follow the format `PAL_ACCELERATION_STRUCTURE_BUILD_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalAccelerationStructureBuildMode;

/**
 * @typedef PalAccelerationStructureBuildHints
 * @brief Acceleration structure build hints.
 *
 * Identifies the optimization goals of an acceleration structure build.
 *
 * Multiple hints can be OR'ed together using bitwise OR operator (`|`).
 * Hints can be ignored by the driver.
 *
 * All values of this type follow the format `PAL_ACCELERATION_STRUCTURE_BUILD_HINT_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalAccelerationStructureBuildHints;

/**
 * @typedef PalAccelerationStructureInstanceFlags
 * @brief Acceleration structure instance flags.
 *
 * Identifies the behavior of an acceleration structure instance.
 *
 * Multiple flags can be OR'ed together using bitwise OR operator (`|`).
 *
 * All values of this type follow the format `PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalAccelerationStructureInstanceFlags;

/**
 * @typedef PalGeometryType
 * @brief Acceleration structure geometry type.
 *
 * Identifies the kind of acceleration structure geometry.
 *
 * All values of this type follow the format `PAL_GEOMETRY_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalGeometryType;

/**
 * @typedef PalGeometryFlags
 * @brief Acceleration structure geometry flags.
 *
 * Identifies the behavior of acceleration structure geometry.
 *
 * Multiple flags can be OR'ed together using bitwise OR operator (`|`).
 * Not all combinations are valid.
 *
 * All values of this type follow the format `PAL_GEOMETRY_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalGeometryFlags;

/**
 * @typedef PalIndexType
 * @brief Index type.
 *
 * Identifies the size of the indices in an index buffer.
 *
 * All values of this type follow the format `PAL_INDEX_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalIndexType;

/**
 * @typedef PalBufferUsages
 * @brief Buffer usages.
 *
 * Identifies how a buffer may be used.
 *
 * Multiple buffer usages can be OR'ed together using bitwise OR operator (`|`).
 *
 * All values of this type follow the format `PAL_BUFFER_USAGE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalBufferUsages;

/**
 * @typedef PalUsageState
 * @brief Resource usage state.
 *
 * Identifies how a resource is used at a point in a command buffer.
 *
 * All values of this type follow the format `PAL_USAGE_STATE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalUsageState;

/**
 * @typedef PalDescriptorType
 * @brief Descriptor type.
 *
 * Identifies the kind of resource a descriptor refers to.
 *
 * All values of this type follow the format `PAL_DESCRIPTOR_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalDescriptorType;

/**
 * @typedef PalRayTracingShaderGroupType
 * @brief Ray tracing shader group type.
 *
 * Identifies the kind of a ray tracing shader group.
 *
 * All values of this type follow the format `PAL_RAY_TRACING_SHADER_GROUP_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalRayTracingShaderGroupType;

/**
 * @typedef PalDescriptorIndexingFlags
 * @brief Descriptor indexing flags.
 *
 * Identifies the descriptor indexing behaviors of descriptors.
 *
 * All values of this type follow the format `PAL_DESCRIPTOR_INDEXING_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalDescriptorIndexingFlags;

/**
 * @typedef PalBufferMemoryUsage
 * @brief Buffer memory usage.
 *
 * Identifies how memory is provided for a buffer.
 *
 * All values of this type follow the format `PAL_BUFFER_MEMORY_USAGE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalBufferMemoryUsage;

/**
 * @typedef PalImageMemoryUsage
 * @brief Image memory usage.
 *
 * Identifies how memory is provided for an image.
 *
 * All values of this type follow the format `PAL_IMAGE_MEMORY_USAGE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalImageMemoryUsage;

/**
 * @typedef PalRenderingFlags
 * @brief Rendering flags.
 *
 * Identifies the behavior of a rendering pass.
 *
 * All values of this type follow the format `PAL_RENDERING_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalRenderingFlags;

/**
 * @typedef PalPipelineStages
 * @brief Pipeline stages.
 *
 * Identifies the pipeline stages used for synchronization.
 *
 * Multiple pipeline usages can be OR'ed together using bitwise OR operator (`|`).
 *
 * All values of this type follow the format `PAL_PIPELINE_STAGE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalPipelineStages;

/**
 * @typedef PalGraphicsBackendVtableVersion
 * @brief Graphics backend vtable version.
 *
 * Identifies the version of a graphics backend vtable.
 *
 * All values of this type follow the format `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalGraphicsBackendVtableVersion;

#endif // PAL_GRAPHICS_TYPES_H