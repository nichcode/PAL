
/**
 * @brief This is the header file for PAL Graphics Module API.
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
 * @{
 */

#ifndef PAL_GRAPHICS_H
#define PAL_GRAPHICS_H

#include "pal_core.h"

#define PAL_ADAPTER_NAME_SIZE 128 /**< maximum adapter name size */
#define PAL_ADAPTER_BACKEND_NAME_SIZE 32 /**< maximum adapter backend name */
#define PAL_SHADER_ENTRY_NAME_SIZE 32 /*<< maximum shader entry name size */
#define PAL_UNUSED_SHADER_INDEX UINT32_MAX /**< unused shader index */
#define PAL_MAX_CUSTOM_BACKENDS 16 /*<< maximum number of custom backends */

#define PAL_MAKE_SHADER_TARGET(major, minor) ((uint32_t)((major) << 8) | (minor))
#define PAL_SHADER_TARGET_MAJOR(target) ((uint32_t)(target) >> 8);
#define PAL_SHADER_TARGET_MINOR(target) ((uint32_t)(target) & 0xFF);

/**
 * @defgroup adapter_features Adapter Features
 * @{
 */
#define PAL_ADAPTER_FEATURE_NONE 0 /**< no adapter features */
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
#define PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE (1ULL << 14) /**< variable shading rate */
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
#define PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT (1ULL << 28) /**< variable shading rate attachment */
#define PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS (1ULL << 29)
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW (1ULL << 30)
#define PAL_ADAPTER_FEATURE_INDIRECT_DISPATCH (1ULL << 31)
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT (1ULL << 32)
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH (1ULL << 33)
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT (1ULL << 34)
#define PAL_ADAPTER_FEATURE_DISPATCH_BASE (1ULL << 35)
#define PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS (1ULL << 36) /**< empty descriptors */
#define PAL_ADAPTER_FEATURE_RAY_QUERY (1ULL << 37)
/** @} */

/**
 * @defgroup adapter_types Adapter Types
 * @{
 */
#define PAL_ADAPTER_TYPE_UNKNOWN 0 /**< unknown adapter type to PAL */
#define PAL_ADAPTER_TYPE_DISCRETE 1
#define PAL_ADAPTER_TYPE_INTEGRATED 2
#define PAL_ADAPTER_TYPE_VIRTUAL 3
#define PAL_ADAPTER_TYPE_CPU 4
#define PAL_ADAPTER_TYPE_COUNT 5 /**< number of adapter types */
/** @} */

/**
 * @defgroup adapter_api_types Adapter API Types
 * @{
 */
#define PAL_ADAPTER_API_TYPE_UNKNOWN 0 /**< unknown adapter API type to PAL */
#define PAL_ADAPTER_API_TYPE_VULKAN 1
#define PAL_ADAPTER_API_TYPE_D3D12 2
#define PAL_ADAPTER_API_TYPE_METAL 3
#define PAL_ADAPTER_API_TYPE_D3D11 4
#define PAL_ADAPTER_API_TYPE_D3D9 5
#define PAL_ADAPTER_API_TYPE_OPENGL 6
#define PAL_ADAPTER_API_TYPE_OPENGLES 7
#define PAL_ADAPTER_API_TYPE_WEBGPU 8
#define PAL_ADAPTER_API_TYPE_COUNT 9 /**< number of adapter API types */
/** @} */

/**
 * @defgroup queue_types Queue Types
 * @{
 */
#define PAL_QUEUE_TYPE_GRAPHICS 0
#define PAL_QUEUE_TYPE_COMPUTE 1
#define PAL_QUEUE_TYPE_COPY 2
#define PAL_QUEUE_TYPE_COUNT 3 /**< number of queue types */
/** @} */

/**
 * @defgroup present_modes Swapchain Present Modes
 * @{
 */
#define PAL_PRESENT_MODE_FIFO 0 /**< v-sync present mode */
#define PAL_PRESENT_MODE_IMMEDIATE 1
#define PAL_PRESENT_MODE_MAILBOX 2
#define PAL_PRESENT_MODE_COUNT 3 /**< number of swapchain present modes */
/** @} */

/**
 * @defgroup composite_alphas Swapchain Composite Alphas
 * @{
 */
#define PAL_COMPOSITE_ALPHA_OPAQUE 0
#define PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED 1
#define PAL_COMPOSITE_ALPHA_POST_MULTIPLIED 2
#define PAL_COMPOSITE_ALPHA_COUNT 3 /**< number of swapchain composite alphas */
/** @} */

/**
 * @defgroup formats Formats
 * @{
 */
#define PAL_FORMAT_UNDEFINED 0 /**< invalid format */
#define PAL_FORMAT_R8_UNORM 1
#define PAL_FORMAT_R8_SNORM 2
#define PAL_FORMAT_R8_UINT 3
#define PAL_FORMAT_R8_SINT 4
#define PAL_FORMAT_R8_SRGB 5
#define PAL_FORMAT_R16_UNORM 6
#define PAL_FORMAT_R16_SNORM 7
#define PAL_FORMAT_R16_UINT 8
#define PAL_FORMAT_R16_SINT 9
#define PAL_FORMAT_R16_SFLOAT 10
#define PAL_FORMAT_R32_UINT 11
#define PAL_FORMAT_R32_SINT 12
#define PAL_FORMAT_R32_SFLOAT 13
#define PAL_FORMAT_R64_UINT 14
#define PAL_FORMAT_R64_SINT 15
#define PAL_FORMAT_R64_SFLOAT 16
#define PAL_FORMAT_R8G8_UNORM 17
#define PAL_FORMAT_R8G8_SNORM 18
#define PAL_FORMAT_R8G8_UINT 19
#define PAL_FORMAT_R8G8_SINT 20
#define PAL_FORMAT_R8G8_SRGB 21
#define PAL_FORMAT_R16G16_UNORM 22
#define PAL_FORMAT_R16G16_SNORM 23
#define PAL_FORMAT_R16G16_UINT 24
#define PAL_FORMAT_R16G16_SINT 25
#define PAL_FORMAT_R16G16_SFLOAT 26
#define PAL_FORMAT_R32G32_UINT 27
#define PAL_FORMAT_R32G32_SINT 28
#define PAL_FORMAT_R32G32_SFLOAT 29
#define PAL_FORMAT_R64G64_UINT 30
#define PAL_FORMAT_R64G64_SINT 31
#define PAL_FORMAT_R64G64_SFLOAT 32
#define PAL_FORMAT_R8G8B8_UNORM 33
#define PAL_FORMAT_R8G8B8_SNORM 34
#define PAL_FORMAT_R8G8B8_UINT 35
#define PAL_FORMAT_R8G8B8_SINT 36
#define PAL_FORMAT_R8G8B8_SRGB 37
#define PAL_FORMAT_R16G16B16_UNORM 38
#define PAL_FORMAT_R16G16B16_SNORM 39
#define PAL_FORMAT_R16G16B16_UINT 40
#define PAL_FORMAT_R16G16B16_SINT 41
#define PAL_FORMAT_R16G16B16_SFLOAT 42
#define PAL_FORMAT_R32G32B32_UINT 43
#define PAL_FORMAT_R32G32B32_SINT 44
#define PAL_FORMAT_R32G32B32_SFLOAT 45
#define PAL_FORMAT_R64G64B64_UINT 46
#define PAL_FORMAT_R64G64B64_SINT 47
#define PAL_FORMAT_R64G64B64_SFLOAT 48
#define PAL_FORMAT_B8G8R8_UNORM 49
#define PAL_FORMAT_B8G8R8_SNORM 50
#define PAL_FORMAT_B8G8R8_UINT 51
#define PAL_FORMAT_B8G8R8_SINT 52
#define PAL_FORMAT_B8G8R8_SRGB 53
#define PAL_FORMAT_R8G8B8A8_UNORM 54
#define PAL_FORMAT_R8G8B8A8_SNORM 55
#define PAL_FORMAT_R8G8B8A8_UINT 56
#define PAL_FORMAT_R8G8B8A8_SINT 57
#define PAL_FORMAT_R8G8B8A8_SRGB 58
#define PAL_FORMAT_R16G16B16A16_UNORM 59
#define PAL_FORMAT_R16G16B16A16_SNORM 60
#define PAL_FORMAT_R16G16B16A16_UINT 61
#define PAL_FORMAT_R16G16B16A16_SINT 62
#define PAL_FORMAT_R16G16B16A16_SFLOAT 63
#define PAL_FORMAT_R32G32B32A32_UINT 64
#define PAL_FORMAT_R32G32B32A32_SINT 65
#define PAL_FORMAT_R32G32B32A32_SFLOAT 66
#define PAL_FORMAT_R64G64B64A64_UINT 67
#define PAL_FORMAT_R64G64B64A64_SINT 68
#define PAL_FORMAT_R64G64B64A64_SFLOAT 69
#define PAL_FORMAT_B8G8R8A8_UNORM 70
#define PAL_FORMAT_B8G8R8A8_SNORM 71
#define PAL_FORMAT_B8G8R8A8_UINT 72
#define PAL_FORMAT_B8G8R8A8_SINT 73
#define PAL_FORMAT_B8G8R8A8_SRGB 74
#define PAL_FORMAT_S8_UINT 75
#define PAL_FORMAT_D16_UNORM 76
#define PAL_FORMAT_D32_SFLOAT 77
#define PAL_FORMAT_D16_UNORM_S8_UINT 78
#define PAL_FORMAT_D32_SFLOAT_S8_UINT 79
#define PAL_FORMAT_D24_UNORM_S8_UINT 80
#define PAL_FORMAT_COUNT 81 /**< number of formats */
/** @} */

/**
 * @defgroup image_usages Image Usages
 * @{
 */
#define PAL_IMAGE_USAGE_UNDEFINED 0 /**< invalid image usage */
#define PAL_IMAGE_USAGE_COLOR_ATTACHEMENT (1U << 0)
#define PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT (1U << 1)
#define PAL_IMAGE_USAGE_TRANSFER_SRC (1U << 2)
#define PAL_IMAGE_USAGE_TRANSFER_DST (1U << 3)
#define PAL_IMAGE_USAGE_STORAGE (1U << 4)
#define PAL_IMAGE_USAGE_SAMPLED (1U << 5)
/** @} */

/**
 * @defgroup shader_formats Adapter Shader Formats
 * @{
 */
#define PAL_SHADER_FORMAT_UNKNOWN 0 /**< unknown shader format to PAL */
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
 * @defgroup load_ops Load Operations
 * @{
 */
#define PAL_LOAD_OP_LOAD 0
#define PAL_LOAD_OP_CLEAR 1
#define PAL_LOAD_OP_DONT_CARE 2
#define PAL_LOAD_OP_COUNT 3 /**< number of load operations */
/** @} */

/**
 * @defgroup store_ops Store Operations
 * @{
 */
#define PAL_STORE_OP_STORE 0
#define PAL_STORE_OP_DONT_CARE 1
#define PAL_STORE_OP_COUNT 2 /**< number of store operations */
/** @} */

/**
 * @defgroup memory_types Memory Types
 * @{
 */
#define PAL_MEMORY_TYPE_GPU_ONLY 0
#define PAL_MEMORY_TYPE_CPU_UPLOAD 1
#define PAL_MEMORY_TYPE_CPU_READBACK 2
#define PAL_MEMORY_TYPE_COUNT 3 /**< number of memory types */
/** @} */

/**
 * @defgroup image_types Image Types
 * @{
 */
#define PAL_IMAGE_TYPE_1D 0
#define PAL_IMAGE_TYPE_2D 1
#define PAL_IMAGE_TYPE_3D 2
#define PAL_IMAGE_TYPE_COUNT 3 /**< number of image types */
/** @} */

/**
 * @defgroup image_aspect Image Aspects
 * @{
 */
#define PAL_IMAGE_ASPECT_COLOR 0
#define PAL_IMAGE_ASPECT_DEPTH 1
#define PAL_IMAGE_ASPECT_STENCIL 2
#define PAL_IMAGE_ASPECT_DEPTH_STENCIL 3
#define PAL_IMAGE_ASPECT_COUNT 4 /**< number of image aspects */
/** @} */

/**
 * @defgroup image_view_types Image View Types
 * @{
 */
#define PAL_IMAGE_VIEW_TYPE_1D 0
#define PAL_IMAGE_VIEW_TYPE_1D_ARRAY 1
#define PAL_IMAGE_VIEW_TYPE_2D 2
#define PAL_IMAGE_VIEW_TYPE_2D_ARRAY 3
#define PAL_IMAGE_VIEW_TYPE_3D 4
#define PAL_IMAGE_VIEW_TYPE_CUBE 5
#define PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY 6
#define PAL_IMAGE_VIEW_TYPE_COUNT 7 /**< number of image view types */
/** @} */

/**
 * @defgroup filter_modes Filer Modes
 * @{
 */
#define PAL_FILTER_MODE_NEAREST 0
#define PAL_FILTER_MODE_LINEAR 1
#define PAL_FILTER_MODE_COUNT 2 /**< number of filter modes */
/** @} */

/**
 * @defgroup sampler_mipmap_modes Sampler Mipmap Modes
 * @{
 */
#define PAL_SAMPLER_MIPMAP_MODE_NEAREST 0
#define PAL_SAMPLER_MIPMAP_MODE_LINEAR 1
#define PAL_SAMPLER_MIPMAP_MODE_COUNT 2 /**< number of sampler mipmap modes */
/** @} */

/**
 * @defgroup sampler_address_modes Sampler Address Modes
 * @{
 */
#define PAL_SAMPLER_ADDRESS_MODE_REPEAT 0
#define PAL_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT 1
#define PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE 2
#define PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER 3
#define PAL_SAMPLER_ADDRESS_MODE_COUNT 4 /**< number of sampler address modes */
/** @} */

/**
 * @defgroup sampler_border_colors Sampler Border Colors
 * @{
 */
#define PAL_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK 0
#define PAL_BORDER_COLOR_INT_TRANSPARENT_BLACK 1
#define PAL_BORDER_COLOR_FLOAT_OPAQUE_BLACK 2
#define PAL_BORDER_COLOR_INT_OPAQUE_BLACK 3
#define PAL_BORDER_COLOR_FLOAT_OPAQUE_WHITE 4
#define PAL_BORDER_COLOR_INT_OPAQUE_WHITE 5
#define PAL_BORDER_COLOR_COUNT 6 /**< number of sampler border colors */
/** @} */

/**
 * @defgroup surface_formats Surface Formats
 * @{
 */
#define PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR 0
#define PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR 1
#define PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR 2
#define PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10 3
#define PAL_SURFACE_FORMAT_COUNT 4 /**< number of surface formats */
/** @} */

/**
 * @defgroup window_instance_types Window Instance Types
 * @{
 */
#define PAL_WINDOW_INSTANCE_TYPE_WAYLAND 0
#define PAL_WINDOW_INSTANCE_TYPE_X11 1
#define PAL_WINDOW_INSTANCE_TYPE_XCB 2
#define PAL_WINDOW_INSTANCE_TYPE_WIN32 3
#define PAL_WINDOW_INSTANCE_TYPE_COUNT 4 /**< number of window instance types */
/** @} */

/**
 * @defgroup shader_stages Shader Stages
 * @{
 */
#define PAL_SHADER_STAGE_UNDEFINED 0 /**< invalid shader stage */
#define PAL_SHADER_STAGE_VERTEX 1
#define PAL_SHADER_STAGE_FRAGMENT 2
#define PAL_SHADER_STAGE_COMPUTE 3
#define PAL_SHADER_STAGE_GEOMETRY 4
#define PAL_SHADER_STAGE_MESH 5
#define PAL_SHADER_STAGE_TASK 6
#define PAL_SHADER_STAGE_TESSELLATION_CONTROL 7
#define PAL_SHADER_STAGE_TESSELLATION_EVALUATION 8
#define PAL_SHADER_STAGE_RAYGEN 9
#define PAL_SHADER_STAGE_CLOSEST_HIT 10
#define PAL_SHADER_STAGE_ANY_HIT 11
#define PAL_SHADER_STAGE_MISS 12
#define PAL_SHADER_STAGE_INTERSECTION 13
#define PAL_SHADER_STAGE_CALLABLE 14
#define PAL_SHADER_STAGE_COUNT 15 /**< number of shader stages */
/** @} */

/**
 * @defgroup sample_counts Sampler Counts
 * @{
 */
#define PAL_SAMPLE_COUNT_1 0
#define PAL_SAMPLE_COUNT_2 1
#define PAL_SAMPLE_COUNT_4 2
#define PAL_SAMPLE_COUNT_8 3
#define PAL_SAMPLE_COUNT_16 4
#define PAL_SAMPLE_COUNT_32 5
#define PAL_SAMPLE_COUNT_64 6
#define PAL_SAMPLE_COUNT_COUNT 7 /**< number of sampler counts */
/** @} */

/**
 * @defgroup primitive_topologies Primitive Topologies
 * @{
 */
#define PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST 0
#define PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP 1
#define PAL_PRIMITIVE_TOPOLOGY_LINE_LIST 2
#define PAL_PRIMITIVE_TOPOLOGY_LINE_STRIP 3
#define PAL_PRIMITIVE_TOPOLOGY_POINT_LIST 4
#define PAL_PRIMITIVE_TOPOLOGY_PATCH 5
#define PAL_PRIMITIVE_TOPOLOGY_COUNT 6 /**< number of primitive topologies */
/** @} */

/**
 * @defgroup cull_modes Cull Modes
 * @{
 */
#define PAL_CULL_MODE_NONE 0
#define PAL_CULL_MODE_FRONT 1
#define PAL_CULL_MODE_BACK 2
#define PAL_CULL_MODE_COUNT 3 /**< number of cull modes */
/** @} */

/**
 * @defgroup front_faces Front Faces
 * @{
 */
#define PAL_FRONT_FACE_CLOCKWISE 0
#define PAL_FRONT_FACE_COUNTER_CLOCKWISE 1
#define PAL_FRONT_FACE_COUNT 2 /**< number of front faces */
/** @} */

/**
 * @defgroup polygon_modes Polygon Modes
 * @{
 */
#define PAL_POLYGON_MODE_FILL 0
#define PAL_POLYGON_MODE_LINE 1
#define PAL_POLYGON_MODE_COUNT 2 /**< number of polygon modes */
/** @} */

/**
 * @defgroup stencil_face_flags Stencil Face Flags
 * @{
 */
#define PAL_STENCIL_FACE_FLAG_FRONT (1U << 0)
#define PAL_STENCIL_FACE_FLAG_BACK (1U << 1)
#define PAL_STENCIL_FACE_FLAG_BOTH (PAL_STENCIL_FACE_FLAG_FRONT | PAL_STENCIL_FACE_FLAG_BACK)
/** @} */

/**
 * @defgroup vertex_types Vertex Types
 * @{
 */
#define PAL_VERTEX_TYPE_UNDEFINED 0 /**< invalid vertex type */
#define PAL_VERTEX_TYPE_INT32 1
#define PAL_VERTEX_TYPE_INT32_2 2
#define PAL_VERTEX_TYPE_INT32_3 3
#define PAL_VERTEX_TYPE_INT32_4 4
#define PAL_VERTEX_TYPE_UINT32 5
#define PAL_VERTEX_TYPE_UINT32_2 6
#define PAL_VERTEX_TYPE_UINT32_3 7
#define PAL_VERTEX_TYPE_UINT32_4 8
#define PAL_VERTEX_TYPE_INT8_2 9
#define PAL_VERTEX_TYPE_INT8_4 10
#define PAL_VERTEX_TYPE_UINT8_2 11
#define PAL_VERTEX_TYPE_UINT8_4 12
#define PAL_VERTEX_TYPE_INT8_2NORM 13
#define PAL_VERTEX_TYPE_INT8_4NORM 14
#define PAL_VERTEX_TYPE_UINT8_2NORM 15
#define PAL_VERTEX_TYPE_UINT8_4NORM 16
#define PAL_VERTEX_TYPE_INT16_2 17
#define PAL_VERTEX_TYPE_INT16_4 18
#define PAL_VERTEX_TYPE_UINT16_2 19
#define PAL_VERTEX_TYPE_UINT16_4 20
#define PAL_VERTEX_TYPE_INT16_2NORM 21
#define PAL_VERTEX_TYPE_INT16_4NORM 22
#define PAL_VERTEX_TYPE_UINT16_2NORM 23
#define PAL_VERTEX_TYPE_UINT16_4NORM 24
#define PAL_VERTEX_TYPE_FLOAT 25
#define PAL_VERTEX_TYPE_FLOAT2 26
#define PAL_VERTEX_TYPE_FLOAT3 27
#define PAL_VERTEX_TYPE_FLOAT4 28
#define PAL_VERTEX_TYPE_HALF_FLOAT16_2 29
#define PAL_VERTEX_TYPE_HALF_FLOAT16_4 30
#define PAL_VERTEX_TYPE_COUNT 31 /**< number of vertex types */
/** @} */

/**
 * @defgroup vertex_semantic_ids Vertex Semantic IDs
 * @{
 */
#define PAL_VERTEX_SEMANTIC_ID_POSITION 0
#define PAL_VERTEX_SEMANTIC_ID_COLOR 1
#define PAL_VERTEX_SEMANTIC_ID_TEXCOORD 2
#define PAL_VERTEX_SEMANTIC_ID_NORMAL 3
#define PAL_VERTEX_SEMANTIC_ID_TANGENT 4
#define PAL_VERTEX_SEMANTIC_ID_COUNT 5 /**< number of vertex semantic ids */
/** @} */

/**
 * @defgroup cmdbuffer_types Command Buffer Types
 * @{
 */
#define PAL_COMMAND_BUFFER_TYPE_PRIMARY 0
#define PAL_COMMAND_BUFFER_TYPE_SECONDARY 1
#define PAL_COMMAND_BUFFER_TYPE_COUNT 2 /**< number of command buffer types */
/** @} */

/**
 * @defgroup vertex_layout_types Vertex Layout Types
 * @{
 */
#define PAL_VERTEX_LAYOUT_TYPE_PER_VERTEX 0
#define PAL_VERTEX_LAYOUT_TYPE_PER_INSTANCE 1
#define PAL_VERTEX_LAYOUT_TYPE_COUNT 2 /**< number of vertex layout types */
/** @} */

/**
 * @defgroup compare_ops Compare Operations
 * @{
 */
#define PAL_COMPARE_OP_NEVER 0
#define PAL_COMPARE_OP_LESS 1
#define PAL_COMPARE_OP_EQUAL 2
#define PAL_COMPARE_OP_LESS_OR_EQUAL 3
#define PAL_COMPARE_OP_GREATER 4
#define PAL_COMPARE_OP_NOT_EQUAL 5
#define PAL_COMPARE_OP_GREATER_OR_EQUAL 6
#define PAL_COMPARE_OP_ALWAYS 7
#define PAL_COMPARE_OP_COUNT 8 /**< number of compare operations */
/** @} */

/**
 * @defgroup stencil_ops Stencil Operations
 * @{
 */
#define PAL_STENCIL_OP_KEEP 0
#define PAL_STENCIL_OP_ZERO 1
#define PAL_STENCIL_OP_REPLACE 2
#define PAL_STENCIL_OP_INCREMENT_AND_CLAMP 3
#define PAL_STENCIL_OP_DECREMENT_AND_CLAMP 4
#define PAL_STENCIL_OP_INVERT 5
#define PAL_STENCIL_OP_INCREMENT_AND_WRAP 6
#define PAL_STENCIL_OP_DECREMENT_AND_WRAP 7
#define PAL_STENCIL_OP_COUNT 8 /**< number of stencil operations */
/** @} */

/**
 * @defgroup blend_ops Blend Operations
 * @{
 */
#define PAL_BLEND_OP_ADD 0
#define PAL_BLEND_OP_SUBTRACT 1
#define PAL_BLEND_OP_REVERSE_SUBTRACT 2
#define PAL_BLEND_OP_MIN 3
#define PAL_BLEND_OP_MAX 4
#define PAL_BLEND_OP_COUNT 5 /**< number of blend operations */
/** @} */

/**
 * @defgroup blend_factors Blend Factors
 * @{
 */
#define PAL_BLEND_FACTOR_ZERO 0
#define PAL_BLEND_FACTOR_ONE 1
#define PAL_BLEND_FACTOR_SRC_COLOR 2
#define PAL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR 3
#define PAL_BLEND_FACTOR_DST_COLOR 4
#define PAL_BLEND_FACTOR_ONE_MINUS_DST_COLOR 5
#define PAL_BLEND_FACTOR_SRC_ALPHA 6
#define PAL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA 7
#define PAL_BLEND_FACTOR_DST_ALPHA 8
#define PAL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA 9
#define PAL_BLEND_FACTOR_CONSTANT_COLOR 10
#define PAL_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR 11
#define PAL_BLEND_FACTOR_CONSTANT_ALPHA 12
#define PAL_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA 13
#define PAL_BLEND_FACTOR_COUNT 14 /**< number of blend factors */
/** @} */

/**
 * @defgroup color_masks Color Masks
 * @{
 */
#define PAL_COLOR_MASK_NONE 0 /**< no color mask */
#define PAL_COLOR_MASK_RED (1U << 0)
#define PAL_COLOR_MASK_GREEN (1U << 1)
#define PAL_COLOR_MASK_BLUE (1U << 2)
#define PAL_COLOR_MASK_ALPHA (1U << 3)
/** @} */

/**
 * @defgroup resolve_modes Resolve Modes
 * @{
 */
#define PAL_RESOLVE_MODE_NONE 0 /**< no resolve mode */
#define PAL_RESOLVE_MODE_SAMPLE_ZERO 1
#define PAL_RESOLVE_MODE_AVERAGE 2
#define PAL_RESOLVE_MODE_MIN 3
#define PAL_RESOLVE_MODE_MAX 4
#define PAL_RESOLVE_MODE_COUNT 5 /**< number of resolve modes */
/** @} */

/**
 * @defgroup fsrs Fragment Shading Rates
 * @{
 */
#define PAL_FRAGMENT_SHADING_RATE_1X1 0
#define PAL_FRAGMENT_SHADING_RATE_1X2 1
#define PAL_FRAGMENT_SHADING_RATE_2X1 2
#define PAL_FRAGMENT_SHADING_RATE_2X2 3
#define PAL_FRAGMENT_SHADING_RATE_2X4 4
#define PAL_FRAGMENT_SHADING_RATE_4X2 5
#define PAL_FRAGMENT_SHADING_RATE_4X4 6
#define PAL_FRAGMENT_SHADING_RATE_COUNT 7 /**< number of fragment shading rates */
/** @} */

/**
 * @defgroup fsr_combiner_ops Fragment Shading Rate Combiner Operations
 * @{
 */
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP 0
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE 1
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN 2
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX 3
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL 4
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_COUNT 5 /**< number of fragment shading rate combiner operations */
/** @} */

/**
 * @defgroup as_types Acceleration Structure Types
 * @{
 */
#define PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL 0
#define PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL 1
#define PAL_ACCELERATION_STRUCTURE_TYPE_COUNT 2 /**< number of acceleration structure types */
/** @} */

/**
 * @defgroup as_build_modes Acceleration Structure Build Modes
 * @{
 */
#define PAL_ACCELERATION_STRUCTURE_BUILD_MODE_BUILD 0
#define PAL_ACCELERATION_STRUCTURE_BUILD_MODE_UPDATE 1
#define PAL_ACCELERATION_STRUCTURE_BUILD_MODE_COUNT 2 /**< number of acceleration structure build modes */
/** @} */

/**
 * @defgroup as_build_hints Acceleration Structure Build Hints
 * @{
 */
#define PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_BUILD (1U << 0)
#define PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_TRACE (1U << 1)
#define PAL_ACCELERATION_STRUCTURE_BUILD_HINT_LOW_MEMORY (1U << 2)
/** @} */

/**
 * @defgroup as_instance_flags Acceleration Structure Instance Flags
 * @{
 */
#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_OPAQUE (1U << 0)
#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_NO_OPAQUE (1U << 1)
#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FACING_CULL_DISABLE (1U << 2)
#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE (1U << 3)
/** @} */

/**
 * @defgroup as_geometry_types Acceleration Structure Geometry Types
 * @{
 */
#define PAL_GEOMETRY_TYPE_TRIANGLE 0
#define PAL_GEOMETRY_TYPE_AABBS 1
#define PAL_GEOMETRY_TYPE_COUNT 2 /**< number of acceleration structure geometry types */
/** @} */

/**
 * @defgroup as_geometry_flags Acceleration Structure Geometry Flags
 * @{
 */
#define PAL_GEOMETRY_FLAG_OPAQUE (1U << 0)
#define PAL_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT (1U << 1)
/** @} */

/**
 * @defgroup index_types Index Types
 * @{
 */
#define PAL_INDEX_TYPE_UINT16 0
#define PAL_INDEX_TYPE_UINT32 1
#define PAL_INDEX_TYPE_COUNT 2 /**< number of index types */
/** @} */

/**
 * @defgroup buffer_usages Buffer Usages
 * @{
 */
#define PAL_BUFFER_USAGE_VERTEX (1U << 0)
#define PAL_BUFFER_USAGE_INDEX (1U << 1)
#define PAL_BUFFER_USAGE_UNIFORM (1U << 2)
#define PAL_BUFFER_USAGE_STORAGE (1U << 3)
#define PAL_BUFFER_USAGE_TRANSFER_SRC (1U << 4)
#define PAL_BUFFER_USAGE_TRANSFER_DST (1U << 5)
#define PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE (1U << 6)
#define PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_SCRATCH (1U << 7)
#define PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_READ_ONLY_INPUT (1U << 8)
#define PAL_BUFFER_USAGE_DEVICE_ADDRESS (1U << 9)
#define PAL_BUFFER_USAGE_INDIRECT (1U << 10)
/** @} */

/**
 * @defgroup debug_msg_severities Debug Message Severities
 * @{
 */
#define PAL_DEBUG_MESSAGE_SEVERITY_INFO 0
#define PAL_DEBUG_MESSAGE_SEVERITY_WARNING 1
#define PAL_DEBUG_MESSAGE_SEVERITY_ERROR 2
#define PAL_DEBUG_MESSAGE_SEVERITY_COUNT 3 /**< number of debug message severities */
/** @} */

/**
 * @defgroup debug_msg_types Debug Message Types
 * @{
 */
#define PAL_DEBUG_MESSAGE_TYPE_GENERAL 0
#define PAL_DEBUG_MESSAGE_TYPE_VALIDATION 1
#define PAL_DEBUG_MESSAGE_TYPE_PERFORMANCE 2
#define PAL_DEBUG_MESSAGE_TYPE_COUNT 3 /**< number of debug message typs */
/** @} */

/**
 * @defgroup usage_states Usage States
 * @{
 */
#define PAL_USAGE_STATE_UNDEFINED 0 /**< invalid usage state */
#define PAL_USAGE_STATE_PRESENT 1
#define PAL_USAGE_STATE_COLOR_ATTACHMENT_WRITE 2
#define PAL_USAGE_STATE_DEPTH_ATTACHMENT_READ 3
#define PAL_USAGE_STATE_DEPTH_ATTACHMENT_WRITE 4
#define PAL_USAGE_STATE_STENCIL_ATTACHMENT_READ 5
#define PAL_USAGE_STATE_STENCIL_ATTACHMENT_WRITE 6
#define PAL_USAGE_STATE_FRAGMENT_SHADING_RATE_ATTACHMENT_READ 7
#define PAL_USAGE_STATE_TRANSFER_READ 8
#define PAL_USAGE_STATE_TRANSFER_WRITE 9
#define PAL_USAGE_STATE_VERTEX_READ 10
#define PAL_USAGE_STATE_INDEX_READ 11
#define PAL_USAGE_STATE_INDIRECT_READ 12
#define PAL_USAGE_STATE_UNIFORM_READ 13
#define PAL_USAGE_STATE_SHADER_READ 14
#define PAL_USAGE_STATE_SHADER_WRITE 15
#define PAL_USAGE_STATE_STORAGE_READ 16
#define PAL_USAGE_STATE_STORAGE_WRITE 17
#define PAL_USAGE_STATE_HOST_READ 18
#define PAL_USAGE_STATE_HOST_WRITE 19
#define PAL_USAGE_STATE_ACCELERATION_STRUCTURE_READ 20
#define PAL_USAGE_STATE_ACCELERATION_STRUCTURE_WRITE 21
#define PAL_USAGE_STATE_COUNT 22 /**< number of usage states */
/** @} */

/**
 * @defgroup descriptor_types Descriptor Types
 * @{
 */
#define PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER 0
#define PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER 1
#define PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE 2
#define PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE 3
#define PAL_DESCRIPTOR_TYPE_SAMPLER 4
#define PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE 5
#define PAL_DESCRIPTOR_TYPE_COUNT 6 /**< number of descriptor types */
/** @} */

/**
 * @defgroup descriptor_indexing_flags Descriptor Indexing Flags
 * @{
 */
#define PAL_DESCRIPTOR_INDEXING_FLAG_NONE 0
#define PAL_DESCRIPTOR_INDEXING_FLAG_UPDATE_AFTER_BIND (1U << 0)
#define PAL_DESCRIPTOR_INDEXING_FLAG_PARTIALLY_BOUND (1U << 1)
#define PAL_DESCRIPTOR_INDEXING_FLAG_NON_UNIFORM_INDEXING (1U << 2)
/** @} */

/**
 * @defgroup rt_shader_group_types Ray Tracing Shader Group Types
 * @{
 */
#define PAL_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL 0
#define PAL_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT 1
#define PAL_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT 2
#define PAL_RAY_TRACING_SHADER_GROUP_TYPE_COUNT 3 /**< number of ray tracing shader group types */
/** @} */

/**
 * @defgroup buffer_memory_usages Buffer Memory Usages
 * @{
 */
#define PAL_BUFFER_MEMORY_USAGE_MANUAL 0 /**< pAL should not allocate memory for the buffer */
#define PAL_BUFFER_MEMORY_USAGE_AUTO_GPU_ONLY 1
#define PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_UPLOAD 2
#define PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_READBACK 3
#define PAL_BUFFER_MEMORY_USAGE_COUNT 4 /**< number of buffer memory usages */
/** @} */

/**
 * @defgroup image_memory_usages Image Memory Usages
 * @{
 */
#define PAL_IMAGE_MEMORY_USAGE_MANUAL 0 /**< pAL should not allocate memory for the image */
#define PAL_IMAGE_MEMORY_USAGE_AUTO_GPU_ONLY 1
#define PAL_IMAGE_MEMORY_USAGE_COUNT 2 /**< number of image memory usages */
/** @} */

/**
 * @defgroup rendering_flags Rendering Flags
 * @{
 */
#define PAL_RENDERING_FLAG_NONE 0 /**< no rendering flags */
#define PAL_RENDERING_FLAG_SUSPENDING (1U << 0)
#define PAL_RENDERING_FLAG_RESUMING (1U << 1)
/** @} */

/**
 * @defgroup pipeline_stages Pipeline Stages
 * @{
 */
#define PAL_PIPELINE_STAGE_NONE 0 /**< no pipeline stages */
#define PAL_PIPELINE_STAGE_VERTEX_SHADER (1U << 1)
#define PAL_PIPELINE_STAGE_FRAGMENT_SHADER (1U << 2)
#define PAL_PIPELINE_STAGE_COMPUTE_SHADER (1U << 3)
#define PAL_PIPELINE_STAGE_GEOMETRY_SHADER (1U << 4)
#define PAL_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER (1U << 5)
#define PAL_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER (1U << 6)
#define PAL_PIPELINE_STAGE_RAY_TRACING_SHADER (1U << 7)
#define PAL_PIPELINE_STAGE_TASK_SHADER (1U << 8)
#define PAL_PIPELINE_STAGE_MESH_SHADER (1U << 9)
#define PAL_PIPELINE_STAGE_VERTEX_INPUT (1U << 10)
#define PAL_PIPELINE_STAGE_INDEX_INPUT (1U << 11)
#define PAL_PIPELINE_STAGE_EARLY_DEPTH_STENCIL (1U << 12)
#define PAL_PIPELINE_STAGE_LATE_DEPTH_STENCIL (1U << 13)
#define PAL_PIPELINE_STAGE_TRANSFER (1U << 14)
#define PAL_PIPELINE_STAGE_HOST (1U << 15)
#define PAL_PIPELINE_STAGE_COLOR_ATTACHMENT (1U << 16)
#define PAL_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT (1U << 17)
#define PAL_PIPELINE_STAGE_INDIRECT_INPUT (1U << 18)
#define PAL_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD (1U << 19)
/** @} */

/**
 * @defgroup gfx_backend_table_version Graphics Backend Vtable Version
 * @{
 */
#define PAL_GRAPHICS_BACKEND_VTABLE_VERSION_1 0
#define PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2 1
#define PAL_GRAPHICS_BACKEND_VTABLE_VERSION_COUNT 2 /**< number of graphics backend vtable versions */
/** @} */

/**
 * @struct PalAdapter
 * @brief Opaque handle to an adapter.
 *
 * @since Added in version 2.0
 */
typedef struct PalAdapter PalAdapter;

/**
 * @struct PalDevice
 * @brief Opaque handle to a graphics device.
 *
 * @since Added in version 2.0
 */
typedef struct PalDevice PalDevice;

/**
 * @struct PalMemory
 * @brief Opaque handle to a graphics device memory.
 *
 * @since Added in version 2.0
 */
typedef struct PalMemory PalMemory;

/**
 * @struct PalQueue
 * @brief Opaque handle to a queue.
 *
 * @since Added in version 2.0
 */
typedef struct PalQueue PalQueue;

/**
 * @struct PalSurface
 * @brief Opaque handle to a surface.
 *
 * @since Added in version 2.0
 */
typedef struct PalSurface PalSurface;

/**
 * @struct PalSwapchain
 * @brief Opaque handle to a swapchain.
 *
 * @since Added in version 2.0
 */
typedef struct PalSwapchain PalSwapchain;

/**
 * @struct PalImage
 * @brief Opaque handle to an image.
 *
 * @since Added in version 2.0
 */
typedef struct PalImage PalImage;

/**
 * @struct PalImageView
 * @brief Opaque handle to an image view.
 *
 * @since Added in version 2.0
 */
typedef struct PalImageView PalImageView;

/**
 * @struct PalShader
 * @brief Opaque handle to a shader.
 *
 * @since Added in version 2.0
 */
typedef struct PalShader PalShader;

/**
 * @struct PalBuffer
 * @brief Opaque handle to a buffer.
 *
 * @since Added in version 2.0
 */
typedef struct PalBuffer PalBuffer;

/**
 * @struct PalFence
 * @brief Opaque handle to a fence.
 *
 * @since Added in version 2.0
 */
typedef struct PalFence PalFence;

/**
 * @struct PalSemaphore
 * @brief Opaque handle to a semaphore.
 *
 * @since Added in version 2.0
 */
typedef struct PalSemaphore PalSemaphore;

/**
 * @struct PalCommandPool
 * @brief Opaque handle to a command pool.
 *
 * @since Added in version 2.0
 */
typedef struct PalCommandPool PalCommandPool;

/**
 * @struct PalCommandBuffer
 * @brief Opaque handle to a command buffer.
 *
 * @since Added in version 2.0
 */
typedef struct PalCommandBuffer PalCommandBuffer;

/**
 * @struct PalDescriptorSetLayout
 * @brief Opaque handle to a descriptor set layout.
 *
 * @since Added in version 2.0
 */
typedef struct PalDescriptorSetLayout PalDescriptorSetLayout;

/**
 * @struct PalDescriptorPool
 * @brief Opaque handle to a descriptor pool.
 *
 * @since Added in version 2.0
 */
typedef struct PalDescriptorPool PalDescriptorPool;

/**
 * @struct PalDescriptorSet
 * @brief Opaque handle to a descriptor set.
 *
 * @since Added in version 2.0
 */
typedef struct PalDescriptorSet PalDescriptorSet;

/**
 * @struct PalSampler
 * @brief Opaque handle to a sampler.
 *
 * @since Added in version 2.0
 */
typedef struct PalSampler PalSampler;

/**
 * @struct PalPipelineLayout
 * @brief Opaque handle to a pipeline layout.
 *
 * @since Added in version 2.0
 */
typedef struct PalPipelineLayout PalPipelineLayout;

/**
 * @struct PalPipeline
 * @brief Opaque handle to a pipeline. 
 * 
 * This is the handle used for all pipeline types (Graphics, Compute, Ray tracing, ...).
 *
 * @since Added in version 2.0
 */
typedef struct PalPipeline PalPipeline;

/**
 * @struct PalShaderBindingTable
 * @brief Opaque handle to a shader binding table.
 *
 * @since Added in version 2.0
 */
typedef struct PalShaderBindingTable PalShaderBindingTable;

/**
 * @struct PalAccelerationStructure
 * @brief Opaque handle to an acceleration structure.
 *
 * @since Added in version 2.0
 */
typedef struct PalAccelerationStructure PalAccelerationStructure;

/**
 * @typedef PalDebugMessageSeverity
 * @brief Debugger messages severity.
 * 
 * All values of this type follow the format `PAL_DEBUG_MESSAGE_SEVERITY_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalDebugMessageSeverity;

/**
 * @typedef PalDebugMessageType
 * @brief Debugger messages type.
 * 
 * All values of this type follow the format `PAL_DEBUG_MESSAGE_TYPE_*` for API
 * consistency and ease of use.
 * 
 * @since Added in version 2.0
 */
typedef uint32_t PalDebugMessageType;

/**
 * @typedef PalDeviceAddress
 * @brief Device address.
 *
 * @since Added in version 2.0
 */
typedef uint64_t PalDeviceAddress;

/**
 * @typedef PalAdapterFeatures
 * @brief Adapter features.
 * 
 * This is a bitmask of all supported features of an adapter.
 * 
 * All values of this type follow the format `PAL_ADAPTER_FEATURE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint64_t PalAdapterFeatures;

/**
 * @typedef PalAdapterType
 * @brief Adapter type.
 * 
 * All values of this type follow the format `PAL_ADAPTER_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalAdapterType;

/**
 * @typedef PalAdapterApiType
 * @brief Adapter API type.
 * 
 * Customs backends that dont fit the already declared API types may use
 * `PAL_ADAPTER_API_TYPE_CUSTOM`.
 * 
 * All values of this type follow the format `PAL_ADAPTER_API_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalAdapterApiType;

/**
 * @typedef PalQueueType
 * @brief Queue type.
 * 
 * All values of this type follow the format `PAL_QUEUE_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalQueueType;

/**
 * @typedef PalPresentMode
 * @brief Present mode.
 * 
 * All values of this type follow the format `PAL_PRESENT_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalPresentMode;

/**
 * @typedef PalCompositeAplha
 * @brief Composite alpha.
 * 
 * All values of this type follow the format `PAL_COMPOSITE_ALPHA_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalCompositeAplha;

/**
 * @typedef PalFormat
 * @brief Format type.
 * 
 * All values of this type follow the format `PAL_FORMAT_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalFormat;

/**
 * @typedef PalImageUsages
 * @brief Image usages. 
 * 
 * Multiple image usages may be OR'ed together using bitwise OR operator (`|`).
 * 
 * All values of this type follow the format `PAL_IMAGE_USAGE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalImageUsages;

/**
 * @typedef PalShaderFormats
 * @brief Shader formats. 
 * 
 * This is a bitmask of all supported shader formats of an adapter.
 * 
 * All values of this type follow the format `PAL_SHADER_FORMAT_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalShaderFormats;

/**
 * @typedef PalLoadOp
 * @brief Load operation.
 * 
 * All values of this type follow the format `PAL_LOAD_OP_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalLoadOp;

/**
 * @typedef PalStoreOp
 * @brief Store operation.
 * 
 * All values of this type follow the format `PAL_STORE_OP_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalStoreOp;

/**
 * @typedef PalMemoryType
 * @brief Memory type.
 * 
 * All values of this type follow the format `PAL_MEMORY_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalMemoryType;

/**
 * @typedef PalImageType
 * @brief Image type.
 * 
 * All values of this type follow the format `PAL_IMAGE_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalImageType;

/**
 * @typedef PalImageAspect
 * @brief Image aspect.
 * 
 * All values of this type follow the format `PAL_IMAGE_ASPECT_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalImageAspect;

/**
 * @typedef PalImageViewType
 * @brief Image view type.
 * 
 * All values of this type follow the format `PAL_IMAGE_VIEW_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalImageViewType;

/**
 * @typedef PalFilterMode
 * @brief Filter mode.
 * 
 * All values of this type follow the format `PAL_FILTER_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalFilterMode;

/**
 * @typedef PalSamplerMipmapMode
 * @brief Sampler mipmap mode.
 * 
 * All values of this type follow the format `PAL_SAMPLER_MIPMAP_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalSamplerMipmapMode;

/**
 * @typedef PalSamplerAddressMode
 * @brief Sampler address mode.
 * 
 * All values of this type follow the format `PAL_SAMPLER_ADDRESS_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalSamplerAddressMode;

/**
 * @typedef PalBorderColor
 * @brief Border color.
 * 
 * All values of this type follow the format `PAL_BORDER_COLOR_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalBorderColor;

/**
 * @typedef PalSurfaceFormat
 * @brief Surface format type.
 * 
 * All values of this type follow the format `PAL_SURFACE_FORMAT_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalSurfaceFormat;

/**
 * @typedef PalWindowInstanceType
 * @brief Window instance type.
 * 
 * All values of this type follow the format `PAL_WINDOW_INSTANCE_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalWindowInstanceType;

/**
 * @typedef PalShaderStage
 * @brief shader stage.
 * 
 * All values of this type follow the format `PAL_SHADER_STAGE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalShaderStage;

/**
 * @typedef PalSampleCount
 * @brief sample count.
 * 
 * All values of this type follow the format `PAL_SAMPLE_COUNT_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalSampleCount;

/**
 * @typedef PalPrimitiveTopology
 * @brief Primitve topology type.
 * 
 * All values of this type follow the format `PAL_PRIMITIVE_TOPOLOGY_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalPrimitiveTopology;

/**
 * @typedef PalCullMode
 * @brief Cull mode.
 * 
 * All values of this type follow the format `PAL_CULL_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalCullMode;

/**
 * @typedef PalFrontFace
 * @brief Front face.
 * 
 * All values of this type follow the format `PAL_FRONT_FACE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalFrontFace;

/**
 * @typedef PalPolygonMode
 * @brief Polygon mode.
 * 
 * All values of this type follow the format `PAL_POLYGON_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalPolygonMode;

/**
 * @typedef PalStencilFaceFlags
 * @brief Stencil face flags. 
 * 
 * Multiple stencil face flags can be OR'ed together using bitwise OR operator (`|`).
 * 
 * All values of this type follow the format `PAL_STENCIL_FACE_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalStencilFaceFlags;

/**
 * @typedef PalVertexType
 * @brief Vertex attribute type.
 * 
 * All values of this type follow the format `PAL_VERTEX_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalVertexType;

/**
 * @typedef PalVertexSemanticID
 * @brief Vertex semantic id.
 * 
 * All values of this type follow the format `PAL_VERTEX_SEMANTIC_ID_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalVertexSemanticID;

/**
 * @typedef PalCommandBufferType
 * @brief Command buffer type.
 * 
 * All values of this type follow the format `PAL_COMMAND_BUFFER_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalCommandBufferType;

/**
 * @typedef PalVertexLayoutType
 * @brief Vertex layout type.
 * 
 * All values of this type follow the format `PAL_VERTEX_LAYOUT_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalVertexLayoutType;

/**
 * @typedef PalCompareOp
 * @brief Compare operation.
 * 
 * All values of this type follow the format `PAL_COMPARE_OP_*` for API
 * consistency and ease of use.
 * 
 * @since Added in version 2.0
 */
typedef uint32_t PalCompareOp;

/**
 * @typedef PalStencilOp
 * @brief Stencil operation.
 * 
 * All values of this type follow the format `PAL_STENCIL_OP_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalStencilOp;

/**
 * @typedef PalBlendOp
 * @brief Blend operation.
 * 
 * All values of this type follow the format `PAL_BLEND_OP_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalBlendOp;

/**
 * @typedef PalBlendFactor
 * @brief Blend factor.
 * 
 * All values of this type follow the format `PAL_BLEND_FACTOR_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalBlendFactor;

/**
 * @typedef PalColorMask
 * @brief Color mask. 
 * 
 * Multiple color masks can be OR'ed together using bitwise OR operator (`|`).
 * 
 * All values of this type follow the format `PAL_COLOR_MASK_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalColorMask;

/**
 * @typedef PalResolveMode
 * @brief Resolve mode.
 * 
 * All values of this type follow the format `PAL_RESOLVE_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalResolveMode;

/**
 * @typedef PalFragmentShadingRate
 * @brief Fragment shading rate.
 * 
 * All values of this type follow the format `PAL_FRAGMENT_SHADING_RATE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalFragmentShadingRate;

/**
 * @typedef PalFragmentShadingRateCombinerOp
 * @brief Fragment shading rate combiner operaton.
 * 
 * All values of this type follow the format `PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalFragmentShadingRateCombinerOp;

/**
 * @typedef PalAccelerationStructureType
 * @brief Acceleration structure type.
 * 
 * All values of this type follow the format `PAL_ACCELERATION_STRUCTURE_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalAccelerationStructureType;

/**
 * @typedef PalAccelerationStructureBuildMode
 * @brief Acceleration structure build mode.
 * 
 * All values of this type follow the format `PAL_ACCELERATION_STRUCTURE_BUILD_MODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalAccelerationStructureBuildMode;

/**
 * @typedef PalAccelerationStructureBuildHints
 * @brief Acceleration structure build hints. 
 * 
 * Multiple hints can be OR'ed together using bitwise OR operator (`|`).
 * Hints can be ignored by the driver.
 * 
 * All values of this type follow the format `PAL_ACCELERATION_STRUCTURE_BUILD_HINT_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalAccelerationStructureBuildHints;

/**
 * @typedef PalAccelerationStructureInstanceFlags
 * @brief Acceleration structure instance flags. 
 * 
 * Multiple flags can be OR'ed together using bitwise OR operator (`|`).
 * 
 * All values of this type follow the format `PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalAccelerationStructureInstanceFlags;

/**
 * @typedef PalGeometryType
 * @brief Geometry type.
 * 
 * All values of this type follow the format `PAL_GEOMETRY_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalGeometryType;

/**
 * @typedef PalGeometryFlags
 * @brief Geometry flags. 
 * 
 * Multiple flags can be OR'ed together using bitwise OR operator (`|`).
 * Not all combinations are valid.
 * 
 * All values of this type follow the format `PAL_GEOMETRY_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalGeometryFlags;

/**
 * @typedef PalIndexType
 * @brief Index type.
 * 
 * All values of this type follow the format `PAL_INDEX_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalIndexType;

/**
 * @typedef PalBufferUsages
 * @brief Buffer usages. 
 * 
 * Multiple buffer usages can be OR'ed together using bitwise OR operator (`|`).
 * 
 * All values of this type follow the format `PAL_BUFFER_USAGE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalBufferUsages;

/**
 * @typedef PalUsageState
 * @brief Usage state.
 * 
 * All values of this type follow the format `PAL_USAGE_STATE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalUsageState;

/**
 * @typedef PalDescriptorType
 * @brief Descriptor type.
 * 
 * All values of this type follow the format `PAL_DESCRIPTOR_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalDescriptorType;

/**
 * @typedef PalRayTracingShaderGroupType
 * @brief Ray tracing shader group type.
 * 
 * All values of this type follow the format `PAL_RAY_TRACING_SHADER_GROUP_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalRayTracingShaderGroupType;

/**
 * @typedef PalDescriptorIndexingFlags
 * @brief Descriptor indexing subfeature flags.
 * 
 * All values of this type follow the format `PAL_DESCRIPTOR_INDEXING_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalDescriptorIndexingFlags;

/**
 * @typedef PalBufferMemoryUsage
 * @brief Buffer memory usages.
 * 
 * All values of this type follow the format `PAL_BUFFER_MEMORY_USAGE_*` for API
 * consistency and ease of use.
 * 
 * @since Added in version 2.0
 */
typedef uint32_t PalBufferMemoryUsage;

/**
 * @typedef PalImageMemoryUsage
 * @brief Image memory usages.
 * 
 * All values of this type follow the format `PAL_IMAGE_MEMORY_USAGE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalImageMemoryUsage;

/**
 * @typedef PalRenderingFlags
 * @brief Rendering flags.
 * 
 * All values of this type follow the format `PAL_RENDERING_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalRenderingFlags;

/**
 * @typedef PalPipelineStages
 * @brief Pipeline stages.
 * 
 * Multiple pipeline usages can be OR'ed together using bitwise OR operator (`|`).
 * 
 * All values of this type follow the format `PAL_PIPELINE_STAGE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalPipelineStages;

/**
 * @typedef PalGraphicsBackendVtableVersion
 * @brief Graphics backend vtable version.
 * 
 * All values of this type follow the format `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalGraphicsBackendVtableVersion;

/**
 * @typedef PalDebugCallback
 * @brief Debug callback function.
 * 
 * `msg` is only valid for the duration of the callback and must not be modified or freed
 * by the callback, the memory is owned by PAL.
 * 
 * The callback may be called concurrently from multiple threads, the implementation
 * must be it will be used by multiple threads.
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
 * @param userData User-defined data passed to the callback or `nullptr`.
 * @param severity Message severity.
 * @param type Message Type.
 * @param msg Null-terminated UTF-8 string containing the debug message.
 *
 * @since Added in version 2.0
 * 
 * @sa palInitGraphics
 */
typedef void(PAL_CALL* PalDebugCallback)(
    void* userData,
    PalDebugMessageSeverity severity,
    PalDebugMessageType type,
    const char* msg);
/** @} */

/**
 * @struct PalAdapterInfo
 * @brief Adapter information.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalAdapterInfo {
    uint64_t vram;                                   /**< total video memory in bytes */
    uint64_t sharedMemory;                           /**< total shared memory in bytes */
    uint64_t driverVersion;                          /**< adapter version */
    uint32_t vendorId;                               /**< adapter vendor id */
    uint32_t deviceId;                               /**< adapter device id */
    PalShaderFormats shaderFormats;                  /**< bitmask of supported shader formats */
    PalAdapterType type;                             /**< adapter type */
    PalAdapterApiType apiType;                       /**< adapter API type */
    char name[PAL_ADAPTER_NAME_SIZE];                /**< adapter name */
    char backendName[PAL_ADAPTER_BACKEND_NAME_SIZE]; /**< adapter backend name */
    PalGraphicsBackendVtableVersion vtableVersion;   /**< backend version */
} PalAdapterInfo;

/**
 * @struct PalImageCapabilities
 * @brief Adapter image capabilities.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalImageCapabilities {
    uint32_t maxWidth;       /**< maximum width in pixels */
    uint32_t maxHeight;      /**< maximum height in pixels */
    uint32_t maxDepth;       /**< maximum depth in pixels */
    uint32_t maxArrayLayers; /**< maximum array layers */
    uint32_t maxMipLevels;   /**< maximum mipmap levels */
} PalImageCapabilities;

/**
 * @struct PalResourceCapabilities
 * @brief Adapter resource capabilities.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalResourceCapabilities {
    uint32_t maxPerStageSampledImages;         /**< maximum sampled images per shader stage */
    uint32_t maxPerSetSampledImages;           /**< maximum sampled images per descriptor set */
    uint32_t maxPerStageStorageImages;         /**< maximum storage images per shader stage */
    uint32_t maxPerSetStorageImages;           /**< maximum storage images per descriptor set */
    uint32_t maxPerStageSamplers;              /**< maximum samplers per shader stage */
    uint32_t maxPerSetSamplers;                /**< maximum samplers per descriptor set */
    uint32_t maxPerStageStorageBuffers;        /**< maximum storage buffers per shader stage */
    uint32_t maxPerSetStorageBuffers;          /**< maximum storage buffers per descriptor set */
    uint32_t maxPerStageUniformBuffers;        /**< maximum uniform buffers per shader stage */
    uint32_t maxPerSetUniformBuffers;          /**< maximum uniform buffers per descriptor set */
    uint32_t maxPerStageAccelerationStructure; /**< maximum acceleration structures per shader stage */
    uint32_t maxPerSetAccelerationStructure; /**< maximum acceleration structures per descriptor set */
    uint32_t maxBoundSets;                   /**< maximum bound descriptor sets */
} PalResourceCapabilities;

/**
 * @struct PalComputeCapabilities
 * @brief Adapter compute capabilities.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalComputeCapabilities {
    uint32_t maxWorkGroupInvocations; /**< maximum invocations across all workgroups */
    uint32_t maxWorkGroupCount[3];    /**< maximum workgroups per dimension */
    uint32_t maxWorkGroupSize[3];     /**< maximum workgroup size per dimension */
} PalComputeCapabilities;

/**
 * @struct PalViewportCapabilities
 * @brief Adapter viewport capabilities.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalViewportCapabilities {
    uint32_t maxWidth;    /**< maximum width in pixels */
    uint32_t maxHeight;   /**< maximum height in pixels */
    float minBoundsRange; /**< minimum coordinate range */
    float maxBoundsRange; /**< maximum coordinate range */
} PalViewportCapabilities;

/**
 * @struct PalAdapterCapabilities
 * @brief Adapter capabilities.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalAdapterCapabilities {
    uint32_t maxComputeQueues;            /**< maximum compute queues that can be created */
    uint32_t maxGraphicsQueues;           /**< maximum graphics queues that can be created */
    uint32_t maxCopyQueues;               /**< maximum copy queues that can be created */
    uint32_t maxColorAttachments;         /**< maximum number of simultaneous color attachments */
    uint32_t maxUniformBufferSize;        /**< maximum uniform buffer size in bytes */
    uint32_t maxStorageBufferSize;        /**< maximum storage buffer size in bytes */
    uint32_t maxPushConstantSize;         /**< maximum push constants size in bytes */
    uint32_t maxVertexLayouts;            /**< maximum vertex layouts */
    uint32_t maxVertexAttributes;         /**< maximum vertex attributes across all vertex layouts */
    uint32_t maxTessellationPatchPoint;   /**< maximum tessellation patch point */
    PalViewportCapabilities viewportCaps; /**< viewport capabilities */
    PalImageCapabilities imageCaps;       /**< image capabilities */
    PalResourceCapabilities resourceCaps; /**< resource capabilities */
    PalComputeCapabilities computeCaps;   /**< compute capabilities */
} PalAdapterCapabilities;

/**
 * @struct PalSamplerAnisotropyCapabilities
 * @brief Adapter sampler anisotropy capabilities.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalSamplerAnisotropyCapabilities {
    uint32_t maxAnisotropy; /**< maximum texture filtering level */
} PalSamplerAnisotropyCapabilities;

/**
 * @struct PalMultiViewCapabilities
 * @brief Adapter multi view capabilities.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalMultiViewCapabilities {
    uint32_t maxViewCount; /**< maximum number views of an image */
} PalMultiViewCapabilities;

/**
 * @struct PalMultiViewportCapabilities
 * @brief Adapter multi viewport capabilities.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalMultiViewportCapabilities {
    uint32_t maxCount; /**< maximum number of simultaneous viewports */
} PalMultiViewportCapabilities;

/**
 * @struct PalDepthStencilCapabilities
 * @brief Adapter depth stencil capabilities.
 * 
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDepthStencilCapabilities {
    uint32_t supportedDepthResolveModes;   /**< mask of supported depth resolve modes */
    uint32_t supportedStencilResolveModes; /**< mask of supported stencil resolve modes */
    PalBool supportsIndependentResolve; /**< whether independent resolve modes are supported */
    PalBool supportsIndependentResolveNone; /**< whether independent none resolve mode is supported */
} PalDepthStencilCapabilities;

/**
 * @struct PalFragmentShadingRateCapabilities
 * @brief Adapter fragment shading rate capabilities.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalFragmentShadingRateCapabilities {
    uint32_t supportedShadingRates; /**< mask of supported shading rates */
    uint32_t supportedCombinerOps;  /**< mask of supported combiner operations */
    uint32_t minTexelWidth;         /**< minimum texel width in pixels */
    uint32_t minTexelHeight;        /**< minimum texel height in pixels */
    uint32_t maxTexelWidth;         /**< maximum texel width in pixels */
    uint32_t maxTexelHeight;        /**< maximum texel height in pixels */
} PalFragmentShadingRateCapabilities;

/**
 * @struct PalMeshShaderCapabilities
 * @brief Adapter mesh shader capabilities.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalMeshShaderCapabilities {
    uint32_t maxOutputPrimitives;         /**< maximum number of primitives per mesh workgroup */
    uint32_t maxOutputVertices;           /**< maximum number of vertices per mesh workgroup */
    uint32_t maxWorkGroupInvocations;     /**< maximum mesh invocations across all workgroups */
    uint32_t maxTaskWorkGroupInvocations; /**< maximum task invocations across all workgroups */
    uint32_t maxWorkGroupCount[3];        /**< maximum mesh workgroups per dimension */
    uint32_t maxTaskWorkGroupCount[3];    /**< maximum task workgroups per dimension */
} PalMeshShaderCapabilities;

/**
 * @struct PalRayTracingCapabilities
 * @brief Adapter ray tracing capabilities.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalRayTracingCapabilities {
    uint32_t maxRecursionDepth;      /**< maximum number of ray recursion */
    uint32_t maxHitAttributeSize;    /**< maximum attributes size in bytes */
    uint32_t maxInstanceCount;       /**< maximum number of instances */
    uint32_t maxPrimitiveCount;      /**< maximum number of primitives */
    uint32_t maxGeometryCount;       /**< maximum number of geometries */
    uint32_t maxPayloadSize;         /**< maximum payload size in bytes */
    uint32_t maxDispatchInvocations; /**< maximum number of dispatch threads */
} PalRayTracingCapabilities;

/**
 * @struct PalDescriptorIndexingCapabilities
 * @brief Adapter descriptor indexing capabilities.
 * 
 * `flags` shows the sub-features or capabilities of the descriptor indexing feature.
 * Each bit represents a supported operation. See below for more information:
 * 
 * - PAL_DESCRIPTOR_INDEXING_FLAG_UPDATE_AFTER_BIND - Descriptors can be updated after
 *   the descriptor set been bound.
 * 
 * - PAL_DESCRIPTOR_INDEXING_FLAG_PARTIALLY_BOUND - Unused descriptors can be
 *   left uninitialized if shaders never accesses them.
 * 
 * - PAL_DESCRIPTOR_INDEXING_FLAG_NON_UNIFORM_INDEXING - Different threads
 *   can access different descriptors.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorIndexingCapabilities {
    PalDescriptorIndexingFlags flags;   /**< descriptor indexing flags */
    uint32_t maxPerStageSampledImages;  /**< maximum sampled images per shader stage  */
    uint32_t maxPerSetSampledImages;    /**< maximum sampled images per descriptor set */
    uint32_t maxPerStageStorageImages;  /**< maximum storage images per shader stage */
    uint32_t maxPerSetStorageImages;    /**< maximum storage images per descriptor set */
    uint32_t maxPerStageSamplers;       /**< maximum samplers per shader stage */
    uint32_t maxPerSetSamplers;         /**< maximum samplers per descriptor set */
    uint32_t maxPerStageStorageBuffers; /**< maximum storage buffers per shader stage */
    uint32_t maxPerSetStorageBuffers;   /**< maximum storage buffers per descriptor set */
    uint32_t maxPerStageUniformBuffers; /**< maximum uniform buffers per shader stage */
    uint32_t maxPerSetUniformBuffers;   /**< maximum uniform buffers per descriptor set */
    uint32_t maxPerStageAccelerationStructure; /**< maximum acceleration structures per shader stage */
    uint32_t maxPerSetAccelerationStructure; /**< maximum acceleration structures per descriptor set */
} PalDescriptorIndexingCapabilities;

/**
 * @struct PalSurfaceCapabilities
 * @brief Adapter surface capabilities.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalSurfaceCapabilities {
    uint32_t supportedPresentModes;    /**< mask of supported present modes */
    uint32_t supportedCompositeAlphas; /**< mask of supported composite alphas */
    uint32_t supportedFormats;         /**< mask of supported surface formats */
    uint32_t minImageCount;            /**< minimum image or back buffer count */
    uint32_t maxImageCount;            /**< maximum image or back buffer count */
    uint32_t minImageWidth;            /**< minimum image width in pixels */
    uint32_t minImageHeight;           /**< minimum image height in pixels */
    uint32_t maxImageWidth;            /**< maximum image width in pixels */
    uint32_t maxImageHeight;           /**< maximum image height in pixels */
    uint32_t maxImageArrayLayers;      /**< maximum number of image layers */
} PalSurfaceCapabilities;

/**
 * @struct PalFormatInfo
 * @brief Format information.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalFormatInfo {
    PalImageUsages usages; /**< bitmask of supported image usages */
    PalFormat format; /**< format */
    PalSampleCount sampleCount; /**< number of samples per pixel */
} PalFormatInfo;

/**
 * @struct PalImageInfo
 * @brief Image information.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalImageInfo {
    PalImageUsages usages; /**< bitmask of image usages */
    uint32_t width;             /**< image width in pixels */
    uint32_t height;            /**< image height in pixels */
    uint32_t depth;             /**< image depth in pixels */
    uint32_t arrayLayerCount;   /**< number of array layers */
    uint32_t mipLevelCount;     /**< number of mipmap levels */
    PalSampleCount sampleCount; /**< number of samples per pixel */
    PalImageType type;          /**< image type */
    PalFormat format;           /**< image format */
    PalBool belongsToSwapchain; /**< whether image belongs to a swapchain */
} PalImageInfo;

/**
 * @struct PalClearValue
 * @brief Clear values.
 *
 * If used with a color attachment, the color values will be used and depth 
 * and stencil will be used with depth stencil attachments.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalClearValue {
    float color[4];   /**< color clear value */
    float depth;      /**< depth clear value */
    uint32_t stencil; /**< stencil clear value */
} PalClearValue;

/**
 * @struct PalAttachmentDesc
 * @brief Attachment description.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalAttachmentDesc {
    PalImageView* imageView;           /**< image view */
    PalImageView* resolveImageView;    /**< resolve image view or `nullptr` */
    PalLoadOp loadOp;                  /**< color or depth load operation */
    PalStoreOp storeOp;                /**< color or depth stencil operation */
    PalLoadOp stencilLoadOp;           /**< stencil load operation */
    PalStoreOp stencilStoreOp;         /**< stencil store operation */
    PalResolveMode resolveMode;        /**< color or depth resolve mode */
    PalResolveMode stencilResolveMode; /**< stencil resolve mode */
    PalClearValue clearValue;          /**< attachment clear value */
} PalAttachmentDesc;

/**
 * @struct PalViewport
 * @brief Viewport.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalViewport {
    float x;        /**< x position in pixels */
    float y;        /**< y position in pixels */
    float width;    /**< width in pixels */
    float height;   /**< height in pixels */
    float minDepth; /**< minimum depth value */
    float maxDepth; /**< maximum depth value */
} PalViewport;

/**
 * @struct PalRect2D
 * @brief Rectangle.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalRect2D {
    int32_t x;       /**< x position in pixels */
    int32_t y;       /**< y position in pixels */
    uint32_t width;  /**< width in pixels */
    uint32_t height; /**< height in pixels */
} PalRect2D;

/**
 * @struct PalMemoryRequirements
 * @brief Memory requirements.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalMemoryRequirements {
    uint64_t size;                 /**< required size in bytes */
    uint64_t alignment;            /**< required alignment in bytes */
    uint64_t memoryMask;           /**< memory mask. */
    uint32_t supportedMemoryTypes; /**< mask of supported memory types */
    uint32_t reserved;             /**< must be set to `0` */
} PalMemoryRequirements;

/**
 * @struct PalCommandBufferSubmitInfo
 * @brief Command buffer submit information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalCommandBufferSubmitInfo {
    uint64_t waitValue;             /**< timeline semaphore value to wait on */
    uint64_t signalValue;           /**< timeline semaphore value to signal */
    PalCommandBuffer* cmdBuffer;    /**< command buffer to submit */
    PalSemaphore* waitSemaphore;    /**< wait semaphore */
    PalSemaphore* signalSemaphore;  /**< signal semaphore */
    PalFence* fence;                /**< fence to signal */
    PalPipelineStages waitStages;   /**< wait semaphore pipeline stages */
    PalPipelineStages signalStages; /**< signal semaphore pipeline stages */
} PalCommandBufferSubmitInfo;

/**
 * @struct PalSwapchainNextImageInfo
 * @brief Swapchain next image information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalSwapchainNextImageInfo {
    uint64_t timeout;              /**< timeout in milliseconds */
    PalSemaphore* signalSemaphore; /**< timeline semaphore value to signal */
    PalFence* fence;               /**< fence to signal */
} PalSwapchainNextImageInfo;

/**
 * @struct PalRenderingInfo
 * @brief Rendering information.
 * 
 * This struct is used for graphics pipelines.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalRenderingInfo {
    PalAttachmentDesc* colorAttachments;        /**< color attachments */
    PalAttachmentDesc* depthStencilAttachment;  /**< depth/stencil attachment */
    PalImageView* fragmentShadingRateImageView; /**< fragment shading rate image view */
    PalRect2D renderArea;                       /**< rendering area */
    PalRenderingFlags flags;                    /**< rendering flags */
    uint32_t fragmentShadingRateTexelWidth;     /**< texel width for fragment shading rate */
    uint32_t fragmentShadingRateTexelHeight;    /**< texel height for fragment shading rate */
    uint32_t viewCount;                         /**< nuumber of views */
    uint32_t arrayLayerCount;                   /**< number of array layers */
    uint32_t colorAttachentCount;               /**< number of color attachments */
} PalRenderingInfo;

/**
 * @struct PalRenderingLayoutInfo
 * @brief Rendering layout information.
 * 
 * This struct is used to reference an already existing @ref PalRenderingInfo.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalRenderingLayoutInfo {
    PalFormat* colorAttachmentsFormat;             /**< color attachments formats */
    uint32_t colorAttachentCount;                  /**< number of color attachment formats */
    uint32_t viewCount;                            /**< number of views */
    PalSampleCount sampleCount;                    /**< number of samples per pixel */
    PalRenderingFlags flags;                       /**< rendering flags */
    PalFormat depthStencilAttachmentFormat;        /**< depth/stencil attachment format */
    PalFormat fragmentShadingRateAttachmentFormat; /**< fragment shading rate attachment format */
} PalRenderingLayoutInfo;

/**
 * @struct PalWorkGroupBuildData
 * @brief Workgroup build data.
 * 
 * This struct is used to build compute or mesh workgroup dispatch data
 * from workload specified in vertices, pixels etc.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalWorkGroupBuildData {
    uint32_t workCount[3];      /**< workload per dimension */
    uint32_t workGroupSize[3];  /**< threads per workgroup per axis of the adapter */
    uint32_t workGroupCount[3]; /**< workgroups per axis of the adapter */
} PalWorkGroupBuildData;

/**
 * @struct PalWorkGroupInfo
 * @brief Workgroup dispatch information.
 * 
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalWorkGroupInfo {
    uint32_t workGroupBase[3];  /**< offset per dimension of a dispatch tile */
    uint32_t workGroupCount[3]; /**< workgroup count per dimension of a dispatch tile */
} PalWorkGroupInfo;

/**
 * @struct PalImageStagingRequirements
 * @brief Image staging buffer requirements.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalImageStagingRequirements {
    uint64_t bufferSize;        /**< required buffer size */
    uint32_t bufferRowLength;   /**< required buffer row length */
    uint32_t bufferImageHeight; /**< required buffer image height */
} PalImageStagingRequirements;

/**
 * @struct PalDrawIndirectData
 * @brief Draw indirect data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDrawIndirectData {
    uint32_t vertexCount;   /**< vertex count */
    uint32_t instanceCount; /**< instance count */
    uint32_t firstVertex;   /**< first vertex */
    uint32_t firstInstance; /**< first instance */
} PalDrawIndirectData;

/**
 * @struct PalDrawIndexedIndirectData
 * @brief Draw indexed indirect data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDrawIndexedIndirectData {
    uint32_t indexCount;    /**< index count */
    uint32_t instanceCount; /**< instance count */
    uint32_t firstIndex;    /**< first index */
    int32_t vertexOffset;   /**< vertex offset */
    uint32_t firstInstance; /**< first instance */
} PalDrawIndexedIndirectData;

/**
 * @struct PalDispatchIndirectData
 * @brief Draw or dispatch indirect data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDispatchIndirectData {
    uint32_t groupCountXOrWidth;  /**< number of groups on the x dimension or dispatch width */
    uint32_t groupCountXOrHeight; /**< number of groups on the y dimension or dispatch height */
    uint32_t groupCountXOrDepth;  /**< number of groups on the z dimension or dispatch depth */
} PalDispatchIndirectData;

/**
 * @struct PalVertexAttribute
 * @brief Vertex attribute.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalVertexAttribute {
    PalVertexSemanticID semanticID; /**< vertex atribute semantic id */
    PalVertexType type;             /**< vertex attribute type */
} PalVertexAttribute;

/**
 * @struct PalVertexLayout
 * @brief Vertex layout.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalVertexLayout {
    PalVertexAttribute* attributes; /**< vertex attributes */
    uint32_t attributeCount;        /**< number of vertex attributes */
    PalVertexLayoutType type;       /**< vertex layout type */
    uint32_t binding;               /**< vertex buffer binding slot */
    uint32_t reserved;              /**< must be set to `0` */
} PalVertexLayout;

/**
 * @struct PalGraphicsDebugger
 * @brief Graphics debugger.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalGraphicsDebugger {
    void* userData;              /**< user-defined data passed to callback or `nullptr`. */
    PalDebugCallback callback;   /**< debug callback function */
    PalBool enableGPUValidation; /**< enable GPU-Based validation */
    PalBool denyGeneral;         /**< do not recieve general messages */
    PalBool denyValidation;      /**< do not recieve validation messages */
    PalBool denyPerformance;     /**< do not recieve performance messages */
    PalBool denyInfoSeverity;    /**< do not recieve info severity messages */
    PalBool denyWarningSeverity; /**< do not recieve warning severity messages */
    PalBool denyErrorSeverity;   /**< do not recieve error severity messages */
    uint32_t reserved;           /**< must be set to `0` */
} PalGraphicsDebugger;

/**
 * @struct PalRasterizerState
 * @brief Rasterizer state.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalRasterizerState {
    PalBool enableDepthClamp;   /**< whether to enable depth clamp */
    PalBool enableDepthBias;    /**< whether to enable depth bias */
    float depthBiasConstant;    /**< depth bias constant */
    float depthBiasSlope;       /**< depth bias slope */
    float depthBiasClamp;       /**< depth bias clamp */
    PalPolygonMode polygonMode; /**< polygon mode */
    PalCullMode cullMode;       /**< cull mode */
    PalFrontFace frontFace;     /**< front face */
} PalRasterizerState;

/**
 * @struct PalMultisampleState
 * @brief Multisample state.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalMultisampleState {
    uint64_t sampleMask;           /**< sample mask or `0` for default */
    PalBool enableSampleShading;   /**< whether to enable sample shading */
    PalBool enableAlphaToCoverage; /**< whether to enable alpha to coverage */
    PalSampleCount sampleCount;    /**< number of samples per pixel */
    float minSampleShading;        /**< minimum sample shading */
} PalMultisampleState;

/**
 * @struct PalStencilOpState
 * @brief Stencil operation state.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalStencilOpState {
    PalStencilOp failOp;      /**< stencil fail operation */
    PalStencilOp passOp;      /**< pass operation */
    PalStencilOp depthFailOp; /**< depth fail operation */
    PalCompareOp compareOp;   /**< compare operation */
} PalStencilOpState;

/**
 * @struct PalDepthStencilState
 * @brief Depth stencil state.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDepthStencilState {
    PalBool enableDepthTest;               /**< whether to enable depth test */
    PalBool enableDepthWrite;              /**< whether to enable depth write */
    PalBool enableStencilTest;             /**< whether to enable stencil test */
    PalCompareOp compareOp;                /**< compare operation */
    PalStencilOpState frontStencilOpState; /**< front stencil operation state */
    PalStencilOpState backStencilOpState;  /**< back stencil operation state */
} PalDepthStencilState;

/**
 * @struct PalColorBlendAttachment
 * @brief Color blend attachmeent.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalColorBlendAttachment {
    PalBool enableBlend;                /**< whether to enable blending */
    PalColorMask colorWriteMask;        /**< color write mask */
    PalBlendFactor dstColorBlendFactor; /**< destination color blend factor */
    PalBlendFactor srcColorBlendFactor; /**< source color blend factor */
    PalBlendOp colorBlendOp;            /**< color blend operation */
    PalBlendFactor dstAlphaBlendFactor; /**< destination alpha blend factor */
    PalBlendFactor srcAlphaBlendFactor; /**< source alpha blend factor */
    PalBlendOp alphaBlendOp;            /**< alpha blend operation */
} PalColorBlendAttachment;

/**
 * @struct PalFragmentShadingRateState
 * @brief Fragment shading rate state.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalFragmentShadingRateState {
    PalFragmentShadingRate rate;                     /**< shading rate */
    PalFragmentShadingRateCombinerOp combinerOps[2]; /**< combiner operations */
} PalFragmentShadingRateState;

/**
 * @struct PalAccelerationStructureInstance
 * @brief Acceleration structure instance data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalAccelerationStructureInstance {
    PalAccelerationStructure* blas;              /**< bottom level acceleration structure */
    PalAccelerationStructureInstanceFlags flags; /**< instance flags */
    uint32_t mask;           /**< only the lower 8-bits are used (0x00 - 0xFF) */
    uint32_t instanceId;     /**< user-defined identifier */
    uint32_t hitGroupOffset; /**< offset added to hitgroup index in the Shader Binding Table */
    float transform[12];     /**< transform (row major 3x4) */
} PalAccelerationStructureInstance;

/**
 * @struct PalAccelerationStructureBuildSize
 * @brief Acceleration structure build size.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalAccelerationStructureBuildSize {
    uint64_t accelerationStructureSize; /**< required acceleration structure size in bytes */
    uint64_t scratchBufferSize;         /**< required scratch buffer size in bytes */
    uint64_t updateScratchBufferSize;   /**< required scratch buffer update size in bytes */
} PalAccelerationStructureBuildSize;

/**
 * @struct PalGeometryDataTriangle
 * @brief Acceleration structure triangle geometry data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalGeometryDataTriangle {
    PalDeviceAddress vertexBufferAddress;    /**< address of the vertex buffer */
    PalDeviceAddress indexBufferAddress;     /**< address of the index buffer */
    PalDeviceAddress transformBufferAddress; /**< address of the transform buffer */
    PalVertexType vertexType;                /**< vertex attribute type */
    PalIndexType indexType;                  /**< index type */
    uint32_t vertexCount;                    /**< number of vertices */
    uint32_t vertexStride;                   /**< size of each vertex in bytes */
} PalGeometryDataTriangle;

/**
 * @struct PalGeometryDataAABBS
 * @brief Acceleration structure AABBS geometry data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalGeometryDataAABBS {
    PalDeviceAddress bufferAddress; /**< address of the AABBS buffer */
    uint64_t stride;                /**< size of each AABBS in bytes */
} PalGeometryDataAABBS;

/**
 * @struct PalGeometry
 * @brief Acceleration structure geometry.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalGeometry {
    const void* data;        /**< geometry data */
    uint64_t primitiveCount; /**< number of primitives in `data` */
    PalGeometryFlags flags;  /**< geometry flags */
    PalGeometryType type;    /**< geometry type, must match `data` */
} PalGeometry;

/**
 * @struct PalAccelerationStructureBuildInfo
 * @brief Acceleration structure build information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalAccelerationStructureBuildInfo {
    PalAccelerationStructure* dst;          /**< destination aceleration structure */
    PalAccelerationStructure* src;          /**< source aceleration structure for updates */
    PalGeometry* geometries;                /**< BLAS geometries  or `nullptr` for TLAS */
    PalDeviceAddress scratchBufferAddress;  /**< address of scratch buffer */
    PalDeviceAddress instanceBufferAddress; /**< address of instance buffer or `nullptr` for BLAS */
    PalAccelerationStructureBuildHints buildHints; /**< build hints */
    PalAccelerationStructureType type; /**< acceleration structure type */
    PalAccelerationStructureBuildMode buildMode; /**< acceleration structure build mode */
    uint32_t count; /**< number of elements in `geometries` or `instanceBufferAddress` */
} PalAccelerationStructureBuildInfo;

/**
 * @struct PalDescriptorSetLayoutBinding
 * @brief Descriptor set layout binding.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorSetLayoutBinding {
    uint32_t descriptorCount;         /**< number of descriptors */
    PalDescriptorType descriptorType; /**< descriptor type */
} PalDescriptorSetLayoutBinding;

/**
 * @struct PalDescriptorPoolBindingSize
 * @brief Descriptor pool binding size.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorPoolBindingSize {
    uint32_t bindingCount;            /**< number of bindings */
    PalDescriptorType descriptorType; /**< descriptor type */
} PalDescriptorPoolBindingSize;

/**
 * @struct PalDescriptorBufferInfo
 * @brief Buffer descriptor information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorBufferInfo {
    PalBuffer* buffer; /**< buffer associated with the descriptor */
    uint64_t offset;   /**< offset in bytes, will be divided by `stride` if structured */
    uint64_t size;     /**< size in bytes */
    uint64_t stride;   /**< stride of structured buffers */
} PalDescriptorBufferInfo;

/**
 * @struct PalDescriptorImageViewInfo
 * @brief Image view descriptor information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorImageViewInfo {
    PalImageView* imageView; /**< image view associated with the descriptor */
} PalDescriptorImageViewInfo;

/**
 * @struct PalDescriptorSamplerInfo
 * @brief Sampler descriptor information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorSamplerInfo {
    PalSampler* sampler; /**< sampler associated with the descriptor */
} PalDescriptorSamplerInfo;

/**
 * @struct PalDescriptorTLASInfo
 * @brief TLAS descriptor information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorTLASInfo {
    PalAccelerationStructure* tlas; /**< TLAS associated with the descriptor */
} PalDescriptorTLASInfo;

/**
 * @struct PalDescriptorSetWriteInfo
 * @brief Descriptor set write information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorSetWriteInfo {
    PalDescriptorSet* descriptorSet;            /**< descriptor set to write into */
    PalDescriptorBufferInfo* bufferInfos;       /**< used with buffer descriptors */
    PalDescriptorImageViewInfo* imageViewInfos; /**< used with image descriptors */
    PalDescriptorSamplerInfo* samplerInfos;     /**< used with sampler descriptors */
    PalDescriptorTLASInfo* tlasInfos;           /**< used with TLAS descriptors */
    PalDescriptorType descriptorType;           /**< descriptor type */
    uint32_t layoutBindingIndex; /**< index into the descriptor set layout bindings array */
    uint32_t arrayElement;       /**< first index within the descriptor set layout bindings array */
    uint32_t descriptorCount;    /**< number of descriptors to write */
} PalDescriptorSetWriteInfo;

/**
 * @struct PalBarrierInfo
 * @brief Barrier information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalBarrierInfo {
    PalUsageState oldState;      /**< old usage state of resource */
    PalUsageState newState;      /**< new usage state of resource */
    PalPipelineStages srcStages; /**< source pipeline stages of resource */
    PalPipelineStages dstStages; /**< destination pipeline stages of resource */
} PalBarrierInfo;

/**
 * @struct PalPushConstantInfo
 * @brief Push constant information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalPushConstantInfo {
    uint32_t offset; /**< offset in bytes */
    uint32_t size;   /**< size in bytes */
} PalPushConstantInfo;

/**
 * @struct PalImageSubresourceRange
 * @brief Image subresource range.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalImageSubresourceRange {
    PalImageAspect aspect;    /**< image aspect */
    uint32_t startMipLevel;   /**< image start mipmap level. */
    uint32_t mipLevelCount;   /**< number of image mipmap levels */
    uint32_t startArrayLayer; /**< image start array layer */
    uint32_t layerArrayCount; /**< number of image array layers */
} PalImageSubresourceRange;

/**
 * @struct PalBufferCopyInfo
 * @brief Buffer copy Information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalBufferCopyInfo {
    uint64_t size;      /**< size in bytes to copy from source buffer */
    uint64_t dstOffset; /**< offset in bytes in destination buffer */
    uint64_t srcOffset; /**< offset in bytes in source buffer */
} PalBufferCopyInfo;

/**
 * @struct PalBufferImageCopyInfo
 * @brief Buffer-image copy information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalBufferImageCopyInfo {
    uint64_t bufferOffset;         /**< offset in bytes into the buffer */
    PalImageAspect imageAspect;    /**< image aspect */
    uint32_t bufferRowLength;      /**< buffer row length in texels */
    uint32_t bufferImageHeight;    /**< buffer image height in texels */
    uint32_t ImageMipLevel;        /**< mipmap level of the image */
    uint32_t ImageStartArrayLayer; /**< starting array layer of the image */
    uint32_t ImageArrayLayerCount; /**< number of array layers of the image */
    int32_t imageOffsetX;          /**< x image offset in bytes */
    int32_t imageOffsetY;          /**< y image offset in bytes */
    int32_t imageOffsetZ;          /**< z image offset in bytes */
    uint32_t imageWidth;           /**< image width in bytes */
    uint32_t imageHeight;          /**< image height in bytes */
    uint32_t imageDepth;           /**< image depth in bytes */
} PalBufferImageCopyInfo;

/**
 * @struct PalImageCopyInfo
 * @brief Image copy information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalImageCopyInfo {
    PalImageAspect aspect;       /**< image aspect */
    uint32_t dstMipLevel;        /**< mipmap level of destination image */
    uint32_t srcMipLevel;        /**< mipmap level of source image */
    uint32_t dstStartArrayLayer; /**< starting array layer of destination image */
    uint32_t srcStartArrayLayer; /**< starting array layer of source image */
    uint32_t arrayLayerCount;    /**< number of array layers of destination and source images */
    int32_t dstOffsetX;          /**< x destination image offset in bytes */
    int32_t srcOffsetX;          /**< x source image offset in bytes */
    int32_t dstOffsetY;          /**< y destination image offset in bytes */
    int32_t srcOffsetY;          /**< y source image offset in bytes */
    int32_t dstOffsetZ;          /**< z destination image offset in bytes */
    int32_t srcOffsetZ;          /**< z source image offset in bytes */
    uint32_t width;              /**< width of the region to copy from source image */
    uint32_t height;             /**< height of the region to copy from source image */
    uint32_t depth;              /**< depth of the region to copy from source image */
} PalImageCopyInfo;

/**
 * @struct PalShaderBindingTableRecordInfo
 * @brief Shader binding table record information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalShaderBindingTableRecordInfo {
    void* localData;        /**< local data */
    uint32_t groupIndex;    /**< index into the shader groups array used to create the ray tracing pipeline */
    uint32_t localDataSize; /**< size of the localData or `0` */
} PalShaderBindingTableRecordInfo;

/**
 * @struct PalShaderEntryInfo
 * @brief Shader entry information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalShaderEntryInfo {
    const char* entryName;       /**< shader entry name */
    PalShaderStage stage;        /**< shader entry stage */
    uint32_t patchControlPoints; /**< patch control points for tessellation shaders */
} PalShaderEntryInfo;

/**
 * @struct PalImageCreateInfo
 * @brief Image creation parameters.
 * 
 * `memoryUsage` may be one of the following:
 * 
 * - PAL_IMAGE_MEMORY_USAGE_MANUAL - The graphics system does not allocate
 *   memory for the image. Call @ref palGetImageMemoryRequirements() to get the
 *   memory requirement of the image and allocate memory with @ref palAllocateMemory()
 *   after the image has been created. Users are required to free the allocated
 *   memory with @ref palFreeMemory() when no longer needed.
 * 
 * - PAL_IMAGE_MEMORY_USAGE_AUTO_GPU_ONLY - The graphics system allocates GPU
 *   only memory for the image after creation. This is recommended if a custom
 *   allocator or sub-allocations allocator is used.
 * 
 * This struct is used only during @ref palCreateImage() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalImageCreateInfo {
    PalImageUsages usages;    /**< bitmask of supported image usages */
    uint32_t width;           /**< image width in pixels */
    uint32_t height;          /**< image height in pixels */
    uint32_t depth;           /**< image depth in pixels */
    uint32_t arrayLayerCount; /**< number of array layers */
    uint32_t mipLevelCount;   /**< number of mipmap levels */
    PalSampleCount sampleCount;      /**< number of samples per pixel */
    PalImageType type;               /**< image type */
    PalFormat format;                /**< image format */
    PalImageMemoryUsage memoryUsage; /**< image memory usage */
} PalImageCreateInfo;

/**
 * @struct PalImageViewCreateInfo
 * @brief Image view creation parameters.
 * 
 * `format` and `type` must be compatible with the specified image the 
 * image view is being created from.
 * 
 * This struct is used only during @ref palCreateImageView() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalImageViewCreateInfo {
    PalFormat format;                          /**< image view format */
    PalImageViewType type;                     /**< image view type */
    PalImageSubresourceRange subresourceRange; /**< image view range */
} PalImageViewCreateInfo;

/**
 * @struct PalSamplerCreateInfo
 * @brief Sampler creation parameters.
 * 
 * This struct is used only during @ref palCreateSampler() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalSamplerCreateInfo {
    PalBool enableCompare;              /**< whether to enable compare operations */
    PalBool enableAnisotropy;           /**< whether to enable texture filtering */
    float mipLodBias;                   /**< mipmap level bias */
    float minLod;                       /**< minimum Mipmap level allowed */
    float maxLod;                       /**< maximum Mipmap level allowed */
    float maxAnisotropy;                /**< texture filtering level */
    PalFilterMode minFilterMode;        /**< minification filter mode */
    PalFilterMode magFilterMode;        /**< magnification filter mode */
    PalSamplerMipmapMode mipmapMode;    /**< sampler mipmap mode */
    PalSamplerAddressMode addressModeU; /**< u address mode */
    PalSamplerAddressMode addressModeV; /**< v address mode */
    PalSamplerAddressMode addressModeW; /**< w address mode */
    PalCompareOp compareOp;             /**< compare operation */
    PalBorderColor borderColor;         /**< border color */
} PalSamplerCreateInfo;

/**
 * @struct PalSwapchainCreateInfo
 * @brief Swapchain creation parameters.
 * 
 * This struct is used only during @ref palCreateSwapchain() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalSwapchainCreateInfo {
    PalBool clipped;                  /**< whether to discard pixels that are not visible */
    uint32_t width;                   /**< width in pixels */
    uint32_t height;                  /**< height in pixels */
    uint32_t imageCount;              /**< number of images or back buffers */
    uint32_t imageArrayLayerCount;    /**< number of array layers */
    PalPresentMode presentMode;       /**< present mode */
    PalCompositeAplha compositeAlpha; /**< composite alpha */
    PalSurfaceFormat format;          /**< swapchain format */
} PalSwapchainCreateInfo;

/**
 * @struct PalShaderCreateInfo
 * @brief Shader creation parameters.
 * 
 * This struct is used only during @ref palCreateShader() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalShaderCreateInfo {
    void* code;                  /**< shader code or bytecode */
    PalShaderEntryInfo* entries; /**< shader entries */
    uint32_t codeSize;           /**< size of shader code or bytecode in bytes */
    uint32_t entryCount;         /**< number of shader entries */
} PalShaderCreateInfo;

/**
 * @struct PalBufferCreateInfo
 * @brief Buffer creation parameters.
 * 
 * `memoryUsage` may be one of the following:
 * 
 * - PAL_BUFFER_MEMORY_USAGE_MANUAL - The graphics system does not allocate
 *   memory for the buffer. Call @ref palGetBufferMemoryRequirements() to get the
 *   memory requirement of the buffer and allocate memory with @ref palAllocateMemory()
 *   after the buffer has been created. Users are required to free the allocated
 *   memory with @ref palFreeMemory() when no longer needed.
 * 
 * - PAL_BUFFER_MEMORY_USAGE_AUTO_GPU_ONLY - The graphics system allocates GPU
 *   only memory for the buffer after creation.
 * 
 * - PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_UPLOAD - The graphics system allocates CPU
 *   upload memory for the buffer after creation.
 * 
 * - PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_READBACK - The graphics system allocates CPU
 *   readback memory for the buffer after creation.
 * 
 * This struct is used only during @ref palCreateBuffer() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalBufferCreateInfo {
    uint64_t size;                    /**< size in bytes */
    PalBufferUsages usages;           /**< bitmask of supported buffer usages */
    PalBufferMemoryUsage memoryUsage; /**< buffer memory usage */
} PalBufferCreateInfo;

/**
 * @struct PalAccelerationStructureCreateInfo
 * @brief Acceleration structure creation parameters.
 * 
 * This struct is used only during @ref palCreateAccelerationstructure() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalAccelerationStructureCreateInfo {
    PalBuffer* buffer;                 /**< acceleration structure buffer */
    uint64_t offset;                   /**< size in bytes */
    uint64_t size;                     /**< offset in bytes */
    PalAccelerationStructureType type; /**< acceleration structure type */
    uint32_t reserved;                 /**< must be set to `0` */
} PalAccelerationStructureCreateInfo;

/**
 * @struct PalDescriptorSetLayoutCreateInfo
 * @brief Descriptor set layout creation parameters.
 * 
 * This struct is used only during @ref palCreateDescriptorSetLayout() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorSetLayoutCreateInfo {
    PalDescriptorSetLayoutBinding* bindings; /**< bindings */
    PalDescriptorIndexingFlags flags;        /**< descriptor indexing flags */
    uint32_t bindingCount;                   /**< number of bindings */
} PalDescriptorSetLayoutCreateInfo;

/**
 * @struct PalDescriptorPoolCreateInfo
 * @brief Descriptor pool creation parameters.
 * 
 * This struct is used only during @ref palCreateDescriptorPool() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalDescriptorPoolCreateInfo {
    PalDescriptorPoolBindingSize* bindingSizes; /**< binding sizes */
    uint32_t bindingSizeCount;                  /**< number of bindings sizes */
    uint32_t maxDescriptorSets; /**< maximum number of descriptor sets that can be allocated */
    PalDescriptorIndexingFlags flags; /**< descriptor indexing flags */
    uint32_t reserved;                /**< must be set to `0` */
} PalDescriptorPoolCreateInfo;

/**
 * @struct PalPipelineLayoutCreateInfo
 * @brief Pipeline layout creation parameters.
 * 
 * This struct is used only during @ref palCreatePipelineLayout() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalPipelineLayoutCreateInfo {
    PalDescriptorSetLayout** descriptorSetLayouts; /**< descriptor set layouts */
    PalPushConstantInfo pushConstantInfo;          /**< push constant info */
    uint32_t descriptorSetLayoutCount;             /**< number of descriptor set layouts */
    PalBool usePushConstant;                       /**< whether to use push constant */
} PalPipelineLayoutCreateInfo;

/**
 * @struct PalGraphicsPipelineCreateInfo
 * @brief GRaphics pipeline creation parameters.
 * 
 * This struct is used only during @ref palCreateGraphicsPipeline() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalGraphicsPipelineCreateInfo {
    PalPipelineLayout* pipelineLayout;                     /**< pipeline layout */
    PalShader** shaders;                                   /**< shaders */
    PalVertexLayout* vertexLayouts;                        /**< vertex layouts */
    PalColorBlendAttachment* colorBlendAttachments;        /**< color blend attachments */
    PalRasterizerState* rasterizerState;                   /**< rasterizer state */
    PalMultisampleState* multisampleState;                 /**< multisample state */
    PalDepthStencilState* depthStencilState;               /**< depth stencil state */
    PalFragmentShadingRateState* fragmentShadingRateState; /**< fragment shading rate state */
    PalRenderingLayoutInfo* renderingLayout;               /**< rendering layout */
    PalBool primitiveRestartEnable; /**< whether to enable primitive restart for indexed draw */
    uint32_t vertexLayoutCount;     /**< number of vertex layouts */
    uint32_t colorBlendAttachmentCount; /**< number of color attachments */
    uint32_t shaderCount;               /**< number of shaders */
    PalIndexType indexType;        /**< primitive restart index type */
    PalPrimitiveTopology topology; /**< primitive topology */
} PalGraphicsPipelineCreateInfo;

/**
 * @struct PalComputePipelineCreateInfo
 * @brief Compute pipeline creation parameters.
 * 
 * This struct is used only during @ref palCreateComputePipeline() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalComputePipelineCreateInfo {
    PalPipelineLayout* pipelineLayout; /**< pipeline layout */
    PalShader* computeShader;          /**< compute shader */
} PalComputePipelineCreateInfo;

/**
 * @struct PalRayTracingShaderGroupCreateInfo
 * @brief Ray tracing shader group creation parameters.
 * 
 * This struct is used only during @ref PalRayTracingPipelineCreateInfo and may be
 * discarded after the function returns.
 * 
 * The shader group array must be in this order [raygen][miss][hitgroup][callable].
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalRayTracingShaderGroupCreateInfo {
    PalRayTracingShaderGroupType type;     /**< shader group type */
    uint32_t anyHitShaderIndex;            /**< index of the anyhit shader */
    uint32_t anyHitShaderEntryIndex;       /**< index of the anyhit shader entry */
    uint32_t closestHitShaderIndex;        /**< index of the closest hit shader */
    uint32_t closestHitShaderEntryIndex;   /**< index of the closest hit shader entry */
    uint32_t generalShaderIndex;           /**< index of the general shader */
    uint32_t generalShaderEntryIndex;      /**< index of the general shader entry */
    uint32_t intersectionShaderIndex;      /**< index of the intersection shader */
    uint32_t intersectionShaderEntryIndex; /**< index of the intersection shader entry */
    uint32_t maxDataSize; /**< size in bytes of extra data associated with the shader group */
} PalRayTracingShaderGroupCreateInfo;

/**
 * @struct PalRayTracingPipelineCreateInfo
 * @brief Ray tracing pipeline creation parameters.
 * 
 * This struct is used only during @ref palCreateRayTracingPipeline() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalRayTracingPipelineCreateInfo {
    PalPipelineLayout* pipelineLayout;                /**< pipeline layout */
    PalRayTracingShaderGroupCreateInfo* shaderGroups; /**< shader groups */
    PalShader** shaders;                              /**< shaders */
    uint32_t shaderGroupCount;                        /**< number of shader groups */
    uint32_t shaderCount;                             /**< number of shaders */
    uint32_t maxRecursionDepth;                       /**< maximum number of ray recursion */
    uint32_t maxAttributeSize;                        /**< maximum attributes size in bytes */
    uint32_t maxPayloadSize;                          /**< maximum payload size in bytes */
    uint32_t reserved;                                /**< must be set to `0` */
} PalRayTracingPipelineCreateInfo;

/**
 * @struct PalShaderBindingTableCreateInfo
 * @brief Shader binding table creation parameters.
 * 
 * This struct is used only during @ref palCreateShaderBindingTable() and may be
 * discarded after the function returns.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalShaderBindingTableCreateInfo {
    PalShaderBindingTableRecordInfo* records; /**< shader binding table records */
    PalPipeline* rayTracingPipeline;          /**< ray tracing pipeline */
    uint32_t recordCount;                     /**< number of shader binding table records */
    uint32_t reserved;                        /**< must be set to `0` */
} PalShaderBindingTableCreateInfo;

/**
 * @struct PalGraphicsBackendInfo
 * @brief Graphics backend information.
 * 
 * The backend will not be copied, so it must remain valid for as long
 * as PAL may use it. All backend handle implementation must reserve the first field
 * as `void*`. The field will be used by the graphics system.
 * 
 * all required function implementation of `version` must be valid, this will be validated
 * when @ref palInitGraphics() is called. See [Custom Graphics Backend Guide](@ref custom_gfx_backend)
 * for more information.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalGraphicsBackendInfo {
    const void* vtable;                      /**< backend vtable */
    PalGraphicsBackendVtableVersion version; /**< backend vtable version */
    uint32_t reserved;                       /**< must be set to `0` */
} PalGraphicsBackendInfo;

/**
 * @struct PalGraphicsBackendVtable1
 * @brief Graphics backend dispatch table version 1.
 * 
 * This struct contains the required function implementations of
 * `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_1`. This struct must remain valid
 * until @ref palShutdownGraphics() is called.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalGraphicsBackendVtable1 {
    //clang-format off
    PalResult(PAL_CALL* enumerateAdapters)(uint32_t*, PalAdapter**);
    void(PAL_CALL* getAdapterInfo)(PalAdapter*, PalAdapterInfo*);
    void(PAL_CALL* getAdapterCapabilities)(PalAdapter*, PalAdapterCapabilities*);
    PalAdapterFeatures(PAL_CALL* getAdapterFeatures)(PalAdapter*);
    uint32_t(PAL_CALL* getHighestSupportedShaderTarget)(PalAdapter*, PalShaderFormats);
    PalResult(PAL_CALL* createDevice)(PalAdapter*, PalAdapterFeatures, PalDevice**);
    void(PAL_CALL* destroyDevice)(PalDevice*);
    uint32_t(PAL_CALL* getDeviceLostReason)(PalDevice*);
    PalResult(PAL_CALL* allocateMemory)(PalDevice*, PalMemoryType, uint64_t, uint64_t, PalMemory**);
    void(PAL_CALL* freeMemory)(PalMemory*);
    void(PAL_CALL* querySamplerAnisotropyCapabilities)(PalDevice*, PalSamplerAnisotropyCapabilities*);
    void(PAL_CALL* queryMultiViewCapabilities)(PalDevice*, PalMultiViewCapabilities*);
    void(PAL_CALL* queryMultiViewportCapabilities)( PalDevice*, PalMultiViewportCapabilities*);
    void(PAL_CALL* queryDepthStencilCapabilities)(PalDevice*, PalDepthStencilCapabilities*);
    void(PAL_CALL* queryFragmentShadingRateCapabilities)(PalDevice*, PalFragmentShadingRateCapabilities*);
    void(PAL_CALL* queryMeshShaderCapabilities)(PalDevice*, PalMeshShaderCapabilities*);
    void(PAL_CALL* queryRayTracingCapabilities)(PalDevice*, PalRayTracingCapabilities*);
    void(PAL_CALL* queryDescriptorIndexingCapabilities)(PalDevice*, PalDescriptorIndexingCapabilities*);
    PalResult(PAL_CALL* createQueue)(PalDevice*, PalQueueType, PalQueue**);
    void(PAL_CALL* destroyQueue)(PalQueue*);
    PalBool(PAL_CALL* canQueuePresent)(PalQueue*, PalSurface*);
    PalResult(PAL_CALL* waitQueue)(PalQueue*);
    void(PAL_CALL* enumerateFormats)(PalAdapter*, uint32_t*, PalFormatInfo*);
    PalBool(PAL_CALL* isFormatSupported)(PalAdapter*, PalFormat);
    PalImageUsages(PAL_CALL* queryFormatImageUsages)(PalAdapter*, PalFormat);
    PalSampleCount(PAL_CALL* queryFormatSampleCount)(PalAdapter*, PalFormat);
    PalResult(PAL_CALL* createImage)(PalDevice*, const PalImageCreateInfo*, PalImage**);
    void(PAL_CALL* destroyImage)(PalImage*);
    void(PAL_CALL* getImageInfo)(PalImage*, PalImageInfo*);
    void(PAL_CALL* getImageMemoryRequirements)(PalImage*, PalMemoryRequirements*);
    PalResult(PAL_CALL* bindImageMemory)(PalImage*, PalMemory*, uint64_t);
    PalResult(PAL_CALL* createImageView)(PalDevice*, PalImage*, const PalImageViewCreateInfo*, PalImageView**);
    void(PAL_CALL* destroyImageView)(PalImageView*);
    PalResult(PAL_CALL* createSampler)(PalDevice*, const PalSamplerCreateInfo*, PalSampler**);
    void(PAL_CALL* destroySampler)(PalSampler*);
    PalResult(PAL_CALL* createSurface)(PalDevice*, void*, void*, PalWindowInstanceType, PalSurface**);
    void(PAL_CALL* destroySurface)(PalSurface*);
    void(PAL_CALL* getSurfaceCapabilities)(PalDevice*, PalSurface*, PalSurfaceCapabilities*);
    PalResult(PAL_CALL* createSwapchain)(PalDevice*, PalQueue*, PalSurface*, const PalSwapchainCreateInfo*, PalSwapchain**);
    void(PAL_CALL* destroySwapchain)(PalSwapchain*);
    PalImage*(PAL_CALL* getSwapchainImage)(PalSwapchain*, uint32_t);
    PalResult(PAL_CALL* getNextSwapchainImage)(PalSwapchain*, PalSwapchainNextImageInfo*, uint32_t*);
    PalResult(PAL_CALL* presentSwapchain)(PalSwapchain*, uint32_t, PalSemaphore*);
    PalResult(PAL_CALL* resizeSwapchain)(PalSwapchain*, uint32_t, uint32_t);
    PalResult(PAL_CALL* createShader)(PalDevice*, const PalShaderCreateInfo*, PalShader**);
    void(PAL_CALL* destroyShader)(PalShader*);
    PalResult(PAL_CALL* createFence)(PalDevice*, PalBool, PalFence**);
    void(PAL_CALL* destroyFence)(PalFence*);
    PalResult(PAL_CALL* waitFence)(PalFence*, uint64_t);
    PalResult(PAL_CALL* resetFence)(PalFence*);
    PalBool(PAL_CALL* isFenceSignaled)(PalFence*);
    PalResult(PAL_CALL* createSemaphore)(PalDevice*, PalBool, PalSemaphore**);
    void(PAL_CALL* destroySemaphore)(PalSemaphore*);
    PalResult(PAL_CALL* waitSemaphore)(PalSemaphore*, uint64_t, uint64_t);
    PalResult(PAL_CALL* signalSemaphore)(PalSemaphore*, PalQueue*, uint64_t);
    PalResult(PAL_CALL* getSemaphoreValue)(PalSemaphore*, uint64_t*);
    PalResult(PAL_CALL* createCommandPool)(PalDevice*, PalQueue*, PalCommandPool**);
    void(PAL_CALL* destroyCommandPool)(PalCommandPool*);
    PalResult(PAL_CALL* allocateCommandBuffer)(PalDevice*, PalCommandPool*, PalCommandBufferType, PalCommandBuffer**);
    void(PAL_CALL* freeCommandBuffer)(PalCommandBuffer*);
    PalResult(PAL_CALL* resetCommandBuffer)(PalCommandBuffer*);
    PalResult(PAL_CALL* submitCommandBuffer)(PalQueue*, PalCommandBufferSubmitInfo*);
    PalResult(PAL_CALL* cmdBegin)(PalCommandBuffer*, PalRenderingLayoutInfo*);
    PalResult(PAL_CALL* cmdEnd)(PalCommandBuffer*);
    void(PAL_CALL* cmdExecuteCommandBuffer)(PalCommandBuffer*, PalCommandBuffer*);
    void(PAL_CALL* cmdSetFragmentShadingRate)(PalCommandBuffer*, PalFragmentShadingRateState*);
    void(PAL_CALL* cmdDrawMeshTasks)(PalCommandBuffer*, uint32_t, uint32_t, uint32_t);
    void(PAL_CALL* cmdDrawMeshTasksIndirect)(PalCommandBuffer*, PalBuffer*, uint32_t);
    void(PAL_CALL* cmdDrawMeshTasksIndirectCount)(PalCommandBuffer*, PalBuffer*, PalBuffer*, uint32_t);
    void(PAL_CALL* cmdBuildAccelerationStructure)(PalCommandBuffer*, PalAccelerationStructureBuildInfo*);
    void(PAL_CALL* cmdBeginRendering)(PalCommandBuffer*, PalRenderingInfo*);
    void(PAL_CALL* cmdEndRendering)(PalCommandBuffer*);
    void(PAL_CALL* cmdCopyBuffer)(PalCommandBuffer*, PalBuffer*, PalBuffer*, PalBufferCopyInfo*);
    void(PAL_CALL* cmdCopyBufferToImage)(PalCommandBuffer*, PalImage*, PalBuffer*, PalBufferImageCopyInfo*);
    void(PAL_CALL* cmdCopyImage)(PalCommandBuffer*, PalImage*, PalImage*, PalImageCopyInfo*);
    void(PAL_CALL* cmdCopyImageToBuffer)(PalCommandBuffer*, PalBuffer*, PalImage*, PalBufferImageCopyInfo*);
    void(PAL_CALL* cmdBindPipeline)(PalCommandBuffer*, PalPipeline*);
    void(PAL_CALL* cmdSetViewport)(PalCommandBuffer*, uint32_t, PalViewport*);
    void(PAL_CALL* cmdSetScissors)(PalCommandBuffer*, uint32_t, PalRect2D*);
    void(PAL_CALL* cmdBindVertexBuffers)(PalCommandBuffer*, uint32_t, uint32_t, PalBuffer**, uint64_t*);
    void(PAL_CALL* cmdBindIndexBuffer)(PalCommandBuffer*, PalBuffer*, uint64_t, PalIndexType);
    void(PAL_CALL* cmdDraw)(PalCommandBuffer*, uint32_t, uint32_t, uint32_t, uint32_t);
    void(PAL_CALL* cmdDrawIndirect)(PalCommandBuffer*, PalBuffer*, uint32_t);
    void(PAL_CALL* cmdDrawIndirectCount)(PalCommandBuffer*, PalBuffer*, PalBuffer*, uint32_t);
    void(PAL_CALL* cmdDrawIndexed)(PalCommandBuffer*, uint32_t, uint32_t, uint32_t, int32_t, uint32_t);
    void(PAL_CALL* cmdDrawIndexedIndirect)(PalCommandBuffer*, PalBuffer*, uint32_t);
    void(PAL_CALL* cmdDrawIndexedIndirectCount)(PalCommandBuffer*, PalBuffer*, PalBuffer*, uint32_t);
    void(PAL_CALL* cmdAccelerationStructureBarrier)(PalCommandBuffer*, PalAccelerationStructure*, PalBarrierInfo*);
    void(PAL_CALL* cmdImageBarrier)(PalCommandBuffer*, PalImage*, PalImageSubresourceRange*, PalBarrierInfo*);
    void(PAL_CALL* cmdBufferBarrier)(PalCommandBuffer*, PalBuffer*, PalBarrierInfo*);
    void(PAL_CALL* cmdDispatch)(PalCommandBuffer*, uint32_t, uint32_t, uint32_t);
    void(PAL_CALL* cmdDispatchBase)(PalCommandBuffer*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
    void(PAL_CALL* cmdDispatchIndirect)(PalCommandBuffer*, PalBuffer*);
    void(PAL_CALL* cmdTraceRays)(PalCommandBuffer*, PalShaderBindingTable*, uint32_t, uint32_t, uint32_t, uint32_t);
    void(PAL_CALL* cmdTraceRaysIndirect)(PalCommandBuffer*, uint32_t, PalShaderBindingTable*, PalBuffer*);
    void(PAL_CALL* cmdBindDescriptorSet)(PalCommandBuffer*, uint32_t, PalDescriptorSet*);
    void(PAL_CALL* cmdPushConstants)(PalCommandBuffer*, uint32_t, uint32_t, const void*);
    void(PAL_CALL* cmdSetCullMode)(PalCommandBuffer*, PalCullMode);
    void(PAL_CALL* cmdSetFrontFace)(PalCommandBuffer*, PalFrontFace);
    void(PAL_CALL* cmdSetPrimitiveTopology)(PalCommandBuffer*, PalPrimitiveTopology);
    void(PAL_CALL* cmdSetDepthTestEnable)(PalCommandBuffer*, PalBool);
    void(PAL_CALL* cmdSetDepthWriteEnable)(PalCommandBuffer*, PalBool);
    void(PAL_CALL* cmdSetStencilOp)(PalCommandBuffer*, PalStencilFaceFlags, PalStencilOp, PalStencilOp, PalStencilOp, PalCompareOp);
    PalResult(PAL_CALL* createAccelerationstructure)(PalDevice*, const PalAccelerationStructureCreateInfo*, PalAccelerationStructure**);
    void(PAL_CALL* destroyAccelerationstructure)(PalAccelerationStructure*);
    void(PAL_CALL* getAccelerationStructureBuildSize)(PalDevice*, PalAccelerationStructureBuildInfo*, PalAccelerationStructureBuildSize*);
    PalResult(PAL_CALL* createBuffer)(PalDevice*, const PalBufferCreateInfo*, PalBuffer**);
    void(PAL_CALL* destroyBuffer)(PalBuffer*);
    void(PAL_CALL* getBufferMemoryRequirements)(PalBuffer*, PalMemoryRequirements*);
    void(PAL_CALL* computeInstanceStagingSize)(PalDevice*, uint32_t, uint64_t*);
    void(PAL_CALL* computeImageStagingRequirements)(PalDevice*, PalFormat, const PalBufferImageCopyInfo*, PalImageStagingRequirements*);
    void(PAL_CALL* writeInstanceStaging)(PalDevice*, uint32_t, PalAccelerationStructureInstance*, void*);
    void(PAL_CALL* writeImageStaging)(PalDevice*, PalFormat, PalBufferImageCopyInfo*, void*, void*);
    PalResult(PAL_CALL* bindBufferMemory)(PalBuffer*, PalMemory*, uint64_t);
    PalResult(PAL_CALL* mapBuffer)(PalBuffer*, uint64_t, uint64_t, void**);
    void(PAL_CALL* unmapBuffer)(PalBuffer*);
    PalDeviceAddress(PAL_CALL* getBufferDeviceAddress)(PalBuffer*);
    PalResult(PAL_CALL* createDescriptorSetLayout)(PalDevice*, const PalDescriptorSetLayoutCreateInfo*, PalDescriptorSetLayout**);
    void(PAL_CALL* destroyDescriptorSetLayout)(PalDescriptorSetLayout*);
    PalResult(PAL_CALL* createDescriptorPool)(PalDevice*, const PalDescriptorPoolCreateInfo*, PalDescriptorPool**);
    void(PAL_CALL* destroyDescriptorPool)(PalDescriptorPool*);
    PalResult(PAL_CALL* resetDescriptorPool)(PalDescriptorPool*);
    PalResult(PAL_CALL* allocateDescriptorSet)(PalDevice*, PalDescriptorPool*, PalDescriptorSetLayout*, PalDescriptorSet**);
    PalResult(PAL_CALL* updateDescriptorSet)(PalDevice*, uint32_t, PalDescriptorSetWriteInfo*);
    PalResult(PAL_CALL* createPipelineLayout)(PalDevice*, const PalPipelineLayoutCreateInfo*, PalPipelineLayout**);
    void(PAL_CALL* destroyPipelineLayout)(PalPipelineLayout*);
    PalResult(PAL_CALL* createGraphicsPipeline)(PalDevice*, const PalGraphicsPipelineCreateInfo*, PalPipeline**);
    PalResult(PAL_CALL* createComputePipeline)(PalDevice*, const PalComputePipelineCreateInfo*, PalPipeline**);
    PalResult(PAL_CALL* createRayTracingPipeline)(PalDevice*, const PalRayTracingPipelineCreateInfo*, PalPipeline**);
    void(PAL_CALL* destroyPipeline)(PalPipeline*);
    PalResult(PAL_CALL* createShaderBindingTable)(PalDevice*, const PalShaderBindingTableCreateInfo*, PalShaderBindingTable**);
    void(PAL_CALL* destroyShaderBindingTable)(PalShaderBindingTable*);
    void(PAL_CALL* updateShaderBindingTable)(PalShaderBindingTable*, uint32_t, PalShaderBindingTableRecordInfo*);
    //clang-format on
} PalGraphicsBackendVtable1;

/**
 * @struct PalGraphicsBackendVtable2
 * @brief Graphics backend dispatch table version 2.
 * 
 * This struct contains the required function implementations of
 * `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2`. This struct must remain valid
 * until @ref palShutdownGraphics() is called.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.1
 * @ingroup pal_graphics
 */
typedef struct PalGraphicsBackendVtable2 {
    //clang-format off
    const PalGraphicsBackendVtable1* vtable1;
    PalBool(PAL_CALL* canQueueShareOwnership)(PalQueue*, PalQueue*);
    PalBool(PAL_CALL* canQueueUseUsageState)(PalQueue*, PalUsageState);
    PalBool(PAL_CALL* canQueueUsePipelineStages)(PalQueue*, PalPipelineStages);
    void(PAL_CALL* cmdImageOwnershipTransfer)(PalCommandBuffer*, PalCommandBuffer*, PalImage*, PalImageSubresourceRange*, PalUsageState, PalPipelineStages);
    void(PAL_CALL* cmdBufferOwnershipTransfer)(PalCommandBuffer*, PalCommandBuffer*, PalBuffer*, PalUsageState, PalPipelineStages);
    //clang-format on
} PalGraphicsBackendVtable2;

/**
 * @brief Initialize the graphics system.
 * 
 * The function initializes the graphics system. If the graphics system has been
 * initialized, the function does nothing.
 *
 * `allocator` and `debugger` are not copied. The allocator and the debugger with any state
 * referenced by them must remain valid until @ref palShutdownGraphics() is called.
 * If `debugger->callback` is `nullptr`, the function will succeed but debugging will be
 * disabled.
 * 
 * This function will fail if a custom backend vtable does not meet its version requirements.
 * The backends vtable are not copied and must remain valid until @ref palShutdownGraphics()
 * is called.
 *
 * @param[in] debugger Debugger to use or `nullptr` to disable debugging.
 * @param[in] allocator Allocator to use or `nullptr` for default.
 * @param[in] backendCount Number of custom backends.
 * @param[in] backends Custom backends array.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palShutdownGraphics
 */
PAL_API PalResult PAL_CALL palInitGraphics(
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator,
    uint32_t backendCount,
    const PalGraphicsBackendInfo* backends);

/**
 * @brief Shutdown the graphics system.
 *
 * If the graphics system has not been initialized, the function returns silently.
 * All created devices, queues, images, swapchains etc must be destroyed before this call.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palInitGraphics
 */
PAL_API void PAL_CALL palShutdownGraphics();

/**
 * @brief Returns a list of all adapters from custom and internal backends.
 *
 * The graphics system must be initialized before this call.
 *
 * If a custom backend which implements an adapter with its API type of Vulkan, and there is an
 * adapter from the internal backends with the same specifications, this function will return both
 * of them in the list. Use PalAdapterInfo::backendName to differentiate between custom and
 * internal backend. the backend name for internal backend is `PAL`.
 *
 * Call this function first with PalAdapter array set to `nullptr` to get the number of adapters.
 * Allocate memory for the PalAdapter array and passed in the count and the allocated array. If
 * the count of the array is less than the number of adapters, PAL will write upto that limit.
 *
 * The adapter handles must not be freed by the user, they are managed by the
 * graphics system. Users are required to cache this, and call this function again
 * if adapters are added or removed which is rare except for virtual ones.
 *
 * @param[in, out] count Capacity of the PalAdapter array.
 * @param[out] outAdapters User allocated array of PalAdapter.
 *
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palEnumerateAdapters(
    uint32_t* count,
    PalAdapter** outAdapters);

/**
 * @brief Get information about an adapter.
 *
 * @param[in] adapter Adapter to query information on.
 * @param[out] info Pointer to a PalAdapterInfo to fill.
 *
 * @Thread-safety `info` is per thread.
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
 * @brief Get capabilites or limits about an adapter.
 *
 * @param[in] adapter Adapter to query capabilities on.
 * @param[out] caps Pointer to a PalAdapterCapabilities to fill.
 *
 * @Thread-safety `caps` is per thread.
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
 * @brief Get the supported features of an adapter.
 *
 * @param[in] adapter Adapter to query features on.
 *
 * @return adapter features on success or `0` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palEnumerateAdapters
 */
PAL_API PalAdapterFeatures PAL_CALL palGetAdapterFeatures(PalAdapter* adapter);

/**
 * @brief Get the highest supported shader target of an adapter.
 *
 * @param[in] adapter Adapter to query.
 * @param[in] shaderFormat The shader format. Must have only a single bit set.
 *
 * @return The highest supported shader target encoded with `PAL_MAKE_SHADER_TARGET` macro
 * on success otherwise `0` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palEnumerateAdapters
 */
PAL_API uint32_t PAL_CALL palGetHighestSupportedShaderTarget(
    PalAdapter* adapter,
    PalShaderFormats shaderFormat);

/**
 * @brief Create a device from an adapter.
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
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `adapter` must be externally synchronized.
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
 * @brief Destroy a device.
 *
 * All resources created with the device must be destroyed before this call.
 *
 * @param[in] device Pointer to the device to destroy.
 *
 * @Thread-safety the adapter used to create the device is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
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
 * @return Tmp
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API uint32_t PAL_CALL palGetDeviceLostReason(PalDevice* device);

/**
 * @brief Allocates GPU memory for the specified device.
 *
 * On CPU adapters, there is usually no
 * `PAL_MEMORY_TYPE_GPU_ONLY` memory type available. So it uses shared memory as the vram and set
 * the shared memory to the `PAL_MEMORY_TYPE_GPU_ONLY` for correctness.
 *
 * @param[in] device Pointer to device to allocate memory on.
 * @param[in] type Memory type to allocate. Must be supported by the adapter associated with the
 * device.
 * @param[in] memoryMask Memory mask. Must match memory type.
 * @param[in] size Number of bytes to allocate. Must not be 0.
 * @param[out] outMemory Pointer to a PalMemory to recieved the allocated GPU memory.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized and
 * `outMemory` is per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palFreeMemory
 */
PAL_API PalResult PAL_CALL palAllocateMemory(
    PalDevice* device,
    PalMemoryType type,
    uint64_t memoryMask,
    uint64_t size,
    PalMemory** outMemory);

/**
 * @brief Free GPU memory allocated by palAllocateMemory.
 *
 * If `memory` is `nullptr`, this function will return silently.
 *
 * @param[in] memory Pointer to memory to free.
 *
 * @Thread-safety `device` must be externally synchronized and
 * `outMemory` is per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palAllocateMemory
 */
PAL_API void PAL_CALL palFreeMemory(PalMemory* memory);

/**
 * @brief Get sampler anisotropy feature capabilites or limits about a device.
 *
 * `PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY` must be supported and enabled when creating the
 * device. Otherwise behavior is undefined.
 *
 * @param[in] device Device to query sampler anisotropy feature capabilities on.
 * @param[out] caps Pointer to a PalSamplerAnisotropyCapabilities to fill.
 *
 * @Thread-safety `caps` is per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
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
 * @Thread-safety `caps` is per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
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
 * @Thread-safety `caps` is per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
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
 * @Thread-safety `caps` is per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
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
 * @Thread-safety `caps` is per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
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
 * @Thread-safety `caps` is per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
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
 * @Thread-safety `caps` is per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
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
 * @Thread-safety `caps` is per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palQueryDescriptorIndexingCapabilities(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps);

/**
 * @brief Create a queue from a device.
 *
 * The created queue must be destroyed using `palDestroyQueue()`.
 *
 * The number of queues of each type which can be created is limited per adapter. check with
 * PalAdapterCapabilities::maxComputeQueues, PalAdapterCapabilities::maxGraphicsQueues and
 * PalAdapterCapabilities::maxCopyQueues respectively for the limit for each queue type.
 * Creating more queues than the supported will fail and return `PAL_RESULT_OUT_OF_QUEUE`.
 *
 * Not all graphics queues support presentation. Create a graphics queue and then check if
 * its support presentation for the provided surface. see `palCanQueuePresent()`. Any graphics
 * queue supports offscreen rendering.
 *
 * @param[in] device Device that creates the queue.
 * @param[in] type Queue type. (eg. PAL_QUEUE_TYPE_GRAPHICS).
 * @param[out] outQueue Pointer to a PalQueue to recieve the created queue.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palDestroyQueue
 */
PAL_API PalResult PAL_CALL palCreateQueue(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue);

/**
 * @brief Destroy a queue.
 *
 * @param[in] queue Queue to destroy.
 *
 * @Thread-safety the device used to create the queue is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCreateQueue
 */
PAL_API void PAL_CALL palDestroyQueue(PalQueue* queue);

/**
 * @brief Check if a queue is presentable to the provided window.
 *
 * @param[in] queue Queue to query.
 * @param[in] surface Surface to check presentation support for.
 *
 * @return `PAL_TRUE` if queue can present otherwise `PAL_FALSE`.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCreateQueue
 */
PAL_API PalBool PAL_CALL palCanQueuePresent(
    PalQueue* queue,
    PalSurface* surface);

/**
 * @brief Check if two queues can share resources without requiring ownership transfer.
 *
 * The adapter used to create the queue's device must support
 * `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2` or later.
 *
 * @param[in] a First queue
 * @param[in] b Second queue.
 *
 * @return `PAL_TRUE` if both queues can share resources otherwise `PAL_FALSE`.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.1
 * @ingroup pal_graphics
 */
PAL_API PalBool PAL_CALL palCanQueueShareOwnership(
    PalQueue* a,
    PalQueue* b);

/**
 * @brief Check if a queue can use the provided usage state.
 *
 * The adapter used to create the queue's device must support
 * `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2` or later.
 *
 * @param[in] queue The queue to query.
 * @param[in] state The usage state.
 *
 * @return `PAL_TRUE` if the queue can use the usage state otherwise `PAL_FALSE`.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.1
 * @ingroup pal_graphics
 * 
 * @sa palCanQueueUsePipelineStages
 */
PAL_API PalBool PAL_CALL palCanQueueUseUsageState(
    PalQueue* queue,
    PalUsageState state);

/**
 * @brief Check if a queue can use the provided pipeline stages.
 *
 * The adapter used to create the queue's device must support
 * `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2` or later.
 *
 * @param[in] queue The queue to query.
 * @param[in] stages The pipeline stages.
 *
 * @return `PAL_TRUE` if the queue can use the pipeline stages otherwise `PAL_FALSE`.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.1
 * @ingroup pal_graphics
 * 
 * @sa palCanQueueUseUsageState
 */
PAL_API PalBool PAL_CALL palCanQueueUsePipelineStages(
    PalQueue* queue,
    PalPipelineStages stages);

/**
 * @brief Blocks indefinitely until the queue becomes idle.
 *
 * This function blocks indefinitely until all submitted work on the queue has been completetd.
 * Returns `PAL_RESULT_SUCCESS` to indicate all pending operations has been completetd.
 *
 * @param[in] queue Pointer to queue to wait.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `queue` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palWaitQueue(PalQueue* queue);

/**
 * @brief Returns a list of all supported formats of an adapter.
 *
 * This function returns the supported format with the supported image usages
 * associated with the format. This is a handy way of selecting a format based on the image
 * usages. Use palIsFormatSupported() to check for a specific format.
 *
 * Call this function first with PalFormatInfo array set to `nullptr` to get the number of formats.
 * Allocate memory for the PalFormatInfo array and passed in the count and the allocated array. If
 * the count of the array is less than the number of formats, PAL will write upto that limit.
 *
 * @param[in] adapter Adapter to query formats on.
 * @param[in, out] count Capacity of the PalFormatInfo array.
 * @param[out] outFormats User allocated array of PalFormatInfo.
 *
 * @Thread-safety `outFormats` is per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palIsFormatSupported
 */
PAL_API void PAL_CALL palEnumerateFormats(
    PalAdapter* adapter,
    uint32_t* count,
    PalFormatInfo* outFormats);

/**
 * @brief Check support for a format on an adapter.
 *
 * This is much faster than enumerating all the formats to pick one. You directly check support
 * for the format you want to use. Call palQueryFormatImageUsages() to check for supported image
 * usages if format is supported.
 *
 * @param[in] adapter Adapter to query format on.
 * @param[in] format Format to query support for.
 *
 * @return True if format is supported otherwise `PAL_FALSE` if not supported.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palQueryFormatImageUsages
 * @sa palQueryFormatImageViewUsages
 */
PAL_API PalBool PAL_CALL palIsFormatSupported(
    PalAdapter* adapter,
    PalFormat format);

/**
 * @brief Checks supported image usages associated with a format.
 *
 * @param[in] adapter Adapter to query format on.
 * @param[in] format Format to query image usages for.
 *
 * @return Supported image usages on success otherwise `0` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalImageUsages PAL_CALL palQueryFormatImageUsages(
    PalAdapter* adapter,
    PalFormat format);

/**
 * @brief Checks supported sample count associated with a format.
 *
 * @param[in] adapter Adapter to query format on.
 * @param[in] format Format to query sample count for.
 *
 * @return Supported sample count on success otherwise 0 on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalSampleCount PAL_CALL palQueryFormatSampleCount(
    PalAdapter* adapter,
    PalFormat format);

/**
 * @brief Create an image.
 *
 * The created image must be destroyed using `palDestroyImage()`.
 *
 * PalImageCreateInfo::width, PalImageCreateInfo::height and PalImageCreateInfo::sampleCount
 * must not be greater than the limits of the adapter used to create the device. Check
 * adapter capabilities for the limits.
 *
 * @param[in] device Device that creates the image.
 * @param[in] info Pointer to a PalImageCreateInfo struct that specifies parameters.
 * @param[out] outImage Pointer to a PalImage to recieve the created image.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palDestroyImage
 */
PAL_API PalResult PAL_CALL palCreateImage(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage);

/**
 * @brief Destroy an image.
 *
 * @param[in] image Image to destroy.
 *
 * @Thread-safety the device used to create the image is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCreateImage
 */
PAL_API void PAL_CALL palDestroyImage(PalImage* image);

/**
 * @brief Get information about an image.
 *
 * This function also supports swapchain images.
 *
 * @param[in] image Image to query information on.
 * @param[out] info Pointer to a PalImageInfo to fill.
 *
 * @Thread-safety `info` is per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCreateImage
 */
PAL_API void PAL_CALL palGetImageInfo(
    PalImage* image,
    PalImageInfo* info);

/**
 * @brief Get memory requirements for the provided image.
 *
 * @param[in] image Image to query memory requirements on.
 * @param[out] requirements Pointer to a PalMemoryRequirements to fill.
 *
 * @Thread-safety `requirements` is per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palGetImageMemoryRequirements(
    PalImage* image,
    PalMemoryRequirements* requirements);

/**
 * @brief Bind an allocated memory to an image.
 *
 * The memory size and alignment should match the requirements of the image.
 * Get the requirements with palGetImageMemoryRequirements().
 *
 * @param[in] image Image to bind memory to.
 * @param[in] memory Memory to bind.
 * @param[in] offset Starting point within the memory.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `requirements` is per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palGetImageMemoryRequirements
 */
PAL_API PalResult PAL_CALL palBindImageMemory(
    PalImage* image,
    PalMemory* memory,
    uint64_t offset);

/**
 * @brief Create an image view.
 *
 * The created image view must be destroyed using `palDestroyImageView()`.
 *
 * PalImageViewCreateInfo::type must be compatible by the type of the base image. Eg. A 2D base
 * image must be have an image view of either `PAL_IMAGE_VIEW_TYPE_2D` or
 * `PAL_IMAGE_VIEW_TYPE_2D_ARRAY`.
 *
 * `PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY` must be supported and enabled by the device
 * used to create the image view if `PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY` will be used.
 * Otherwise behavior is undefined.
 *
 * @param[in] device Device that creates the image view.
 * @param[in] image Image to create the image view with.
 * @param[in] info Pointer to a PalImageViewCreateInfo struct that specifies parameters.
 * @param[out] outImageView Pointer to a PalImageView to recieve the created image view.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILUREon
 * failure. Call palFormatResult() for more information.
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
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
 * @param[in] imageView Image view to destroy.
 *
 * @Thread-safety the device used to create the image view is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCreateImageView
 */
PAL_API void PAL_CALL palDestroyImageView(PalImageView* imageView);

/**
 * @brief Create a sampler.
 *
 * The created sampler must be destroyed using `palDestroySampler()`.
 *
 * @param[in] device Device that creates the sampler.
 * @param[in] info Pointer to a PalSamplerCreateInfo struct that specifies parameters.
 * @param[out] outSampler Pointer to a PalSampler to recieve the created sampler.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palDestroySampler
 */
PAL_API PalResult PAL_CALL palCreateSampler(
    PalDevice* device,
    const PalSamplerCreateInfo* info,
    PalSampler** outSampler);

/**
 * @brief Destroy a sampler.
 *
 * @param[in] sampler Sampler to destroy.
 *
 * @Thread-safety the device used to create the sampler is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCreateSampler
 */
PAL_API void PAL_CALL palDestroySampler(PalSampler* sampler);

/**
 * @brief Create a surface for a window.
 *
 * The created surface must be destroyed using `palDestroySurface()`.
 *
 * `PAL_ADAPTER_FEATURE_SWAPCHAIN` must be supported and enabled when creating the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] device Device that creates the surface.
 * @param[in] window Window to create the surface for.
 * @param[in] windowInstance The instance of the window.
 * @param[in] instanceType The instance type of the window.
 * @param[out] outSurface Pointer to a PalSurface to recieve the created surface.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palDestroySurface
 */
PAL_API PalResult PAL_CALL palCreateSurface(
    PalDevice* device,
    void* window,
    void* windowInstance,
    PalWindowInstanceType instanceType,
    PalSurface** outSurface);

/**
 * @brief Destroy a surface.
 *
 * @param[in] surface Surface to destroy.
 *
 * @Thread-safety the device used to create the surface is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCreateSurface
 */
PAL_API void PAL_CALL palDestroySurface(PalSurface* surface);

/**
 * @brief Get surface capabilites about a device.
 *
 * `PAL_ADAPTER_FEATURE_SWAPCHAIN` must be supported and enabled when creating the
 * device. Otherwise behavior is undefined.
 *
 * @param[in] device Device to query surface feature capabilities on.
 * @param[in] surface Surface to query capabilities.
 * @param[out] caps Pointer to a PalSurfaceCapabilities to fill.
 *
 * @Thread-safety `caps` is per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palGetSurfaceCapabilities(
    PalDevice* device,
    PalSurface* surface,
    PalSurfaceCapabilities* caps);

/**
 * @brief Create a swaphain.
 *
 * The created swapchain must be destroyed using `palDestroySwapchain()`.
 *
 * `PAL_ADAPTER_FEATURE_SWAPCHAIN` must be supported and enabled when creating the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] device Device that creates the swapchain.
 * @param[in] queue Queue to create swapchain with. This must be a graphics queue.
 * @param[in] surface Surface to create swapchain with.
 * @param[in] info Pointer to a PalSwapchainCreateInfo struct that specifies parameters.
 * @param[out] outSwapchain Pointer to a PalSwapchain to recieve the created swapchain.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
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
 * @param[in] swapchain Swapchain to destroy.
 *
 * @Thread-safety the device used to create the swapchain is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCreateSwapchain
 */
PAL_API void PAL_CALL palDestroySwapchain(PalSwapchain* swapchain);

/**
 * @brief Get a swapchain image from the list of images with an index.
 *
 * @param[in] swapchain Swapchain to get image from.
 * @param[in] index Index of image in the list. Must not be greater than the image count.
 *
 * @return A pointer to the image on success otherwise `nullptr` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palGetNextSwapchainImage
 */
PAL_API PalImage* PAL_CALL palGetSwapchainImage(
    PalSwapchain* swapchain,
    uint32_t index);

/**
 * @brief Get the next available image from the swapchain image list.
 *
 * @param[in] swapchain Swapchain to get image index from.
 * @param[in] info Pointer to a PalSwapchainNextImageInfo struct that specifies parameters.
 * @param[out] outIndex Pointer to a uint32_t to recieve the next image index.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `swapchain` externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palGetSwapchainImage
 */
PAL_API PalResult PAL_CALL palGetNextSwapchainImage(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    uint32_t* outIndex);

/**
 * @brief Present the swapchain.
 *
 * @param[in] swapchain Swapchain to present.
 * @param[in] imageIndex Swapchain image index to present.
 * @param[in] waitSemaphore The semaphore to wait for.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palPresentSwapchain(
    PalSwapchain* swapchain,
    uint32_t imageIndex,
    PalSemaphore* waitSemaphore);

/**
 * @brief Resize the provided swapchain.
 *
 * The swapchain images must not be in use before this call. All resources (image views) that
 * reference the swapchain images must be destroyed and recreated.
 *
 * @param[in] swapchain Swapchain to resize.
 * @param[in] newWidth The new width of the swapchain.
 * @param[in] newHeight The new height of the swapchain.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `swapchain` externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palResizeSwapchain(
    PalSwapchain* swapchain,
    uint32_t newWidth,
    uint32_t newHeight);

/**
 * @brief Create a shader.
 *
 * The created shader must be destroyed using `palDestroyShader()`.
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
 * @param[out] outShader Pointer to a PalShader to recieve the created shader.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @note The shader entry name must not be greater than `PAL_SHADER_ENTRY_NAME_SIZE (32)`.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palDestroyShader
 */
PAL_API PalResult PAL_CALL palCreateShader(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader);

/**
 * @brief Destroy a shader.
 *
 * @param[in] shader Shader to destroy.
 *
 * @Thread-safety the device used to create the shader is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCreateShader
 */
PAL_API void PAL_CALL palDestroyShader(PalShader* shader);

/**
 * @brief Create a fence.
 *
 * The created fence must be destroyed using `palDestroyFence()`.
 *
 * @param[in] device Device that creates the fence.
 * @param[in] signaled True if fence should be created signaled. If true, the fence must be reset
 * before waiting for it to prevent waiting forever.
 * @param[out] outFence Pointer to a PalFence to recieve the created fence.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palDestroyFence
 */
PAL_API PalResult PAL_CALL palCreateFence(
    PalDevice* device,
    PalBool signaled,
    PalFence** outFence);

/**
 * @brief Destroy a fence.
 *
 * @param[in] fence Fence to destroy.
 *
 * @Thread-safety the device used to create the fence is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCreateFence
 */
PAL_API void PAL_CALL palDestroyFence(PalFence* fence);

/**
 * @brief Wait for a fence.
 *
 * This function blocks for `timeout` until the fence is signaled or there is a timeout.
 * Returns `PAL_RESULT_SUCCESS` or `PAL_RESULT_TIMEOUT` respectively.
 *
 * @param[in] fence Fence to wait for.
 * @param[in] timeout Time to wait for in milliseconds. Set to `PAL_INFINITE` for indefintely.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `fence` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palIsFenceSignaled
 */
PAL_API PalResult PAL_CALL palWaitFence(
    PalFence* fence,
    uint64_t timeout);

/**
 * @brief Reset a fence to an unsignaled state.
 *
 * `PAL_ADAPTER_FEATURE_FENCE_RESET` must be supported and enabled when creating the
 * device. Otherwise behavior is undefined.
 *
 * @param[in] fence Fence to reset.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `fence` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palIsFenceSignaled
 */
PAL_API PalResult PAL_CALL palResetFence(PalFence* fence);

/**
 * @brief Checks if the provided fence is in a signaled state.
 *
 * @param[in] fence Fence to check.
 *
 * @return True if signaled otherwise `PAL_FALSE`.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palResetFence
 * @sa palWaitFence
 */
PAL_API PalBool PAL_CALL palIsFenceSignaled(PalFence* fence);

/**
 * @brief Create a semaphore.
 *
 * The created semaphore must be destroyed using `palDestroySemaphore()`.
 *
 * @param[in] device Device that creates the semaphore.
 * @param[in] enableTimeline If true, `PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE` must be supported
 * and enabled by the device creating the semaphore.
 * @param[out] outSemaphore Pointer to a PalSemaphore to recieve the created semaphore.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palDestroySemaphore
 */
PAL_API PalResult PAL_CALL palCreateSemaphore(
    PalDevice* device,
    PalBool enableTimeline,
    PalSemaphore** outSemaphore);

/**
 * @brief Destroy a semaphore.
 *
 * @param[in] semaphore Semaphore to destroy.
 *
 * @Thread-safety the device used to create the semaphore is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCreateSemaphore
 */
PAL_API void PAL_CALL palDestroySemaphore(PalSemaphore* semaphore);

/**
 * @brief Waits for a semaphore to reach the provided value.
 *
 * The provided semaphore must be a timeline semaphore. Otherwise undefined behavior.
 *
 * @param[in] semaphore Semaphore to wait on.
 * @param[in] value Value to wait for.
 * @param[in] timeout Time to wait for in milliseconds. Set to `PAL_INFINITE` for indefintely.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `semaphore` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palSignalSemaphore
 * @sa palGetSemaphoreValue
 */
PAL_API PalResult PAL_CALL palWaitSemaphore(
    PalSemaphore* semaphore,
    uint64_t value,
    uint64_t timeout);

/**
 * @brief Signals a semaphore from the provided value.
 *
 * The provided semaphore must be a timeline semaphore. Otherwise undefined behavior.
 *
 * @param[in] semaphore Semaphore to signal.
 * @param[in] queue Queue used to signal the semaphore.
 * @param[in] value Value to signal.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `queue` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palWaitSemaphore
 * @sa palGetSemaphoreValue
 */
PAL_API PalResult PAL_CALL palSignalSemaphore(
    PalSemaphore* semaphore,
    PalQueue* queue,
    uint64_t value);

/**
 * @brief Get the value of a semaphore.
 *
 * The provided semaphore must be a timeline semaphore. Otherwise undefined behavior.
 *
 * @param[in] semaphore Semaphore to get its value.
 * @param[out] value The semaphore value.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `semaphore` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palWaitSemaphore
 * @sa palSignalSemaphore
 */
PAL_API PalResult PAL_CALL palGetSemaphoreValue(
    PalSemaphore* semaphore,
    uint64_t* value);

/**
 * @brief Create a command pool from a device.
 *
 * The created command pool must be destroyed using `palDestroyCommandPool()`.
 *
 * @param[in] device Device that creates the command pool.
 * @param[in] queue Queue the command pool buffers will be submitted to.
 * @param[out] outPool Pointer to a PalCommandPool to recieve the created command pool.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
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
 * @Thread-safety the device used to create the command pool is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCreateCommandPool
 */
PAL_API void PAL_CALL palDestroyCommandPool(PalCommandPool* pool);

/**
 * @brief Reset all command buffers allocated from the provided command pool.
 *
 * @param[in] pool Command pool to reset its command buffers.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `pool` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palResetCommandPool(PalCommandPool* pool);

/**
 * @brief Allocate a command buffer from the provided command pool.
 *
 * @param[in] device Device to allocate command buffer on.
 * @param[in] pool Command pool to allocate command buffer from.
 * @param[in] type Type of the command buffer (eg. PAL_COMMAND_BUFFER_TYPE_PRIMARY).
 * @param[out] outCmdBuffer Pointer to a PalCommandBuffer to recieve the created command buffer.
 *
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` and `pool` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palFreeCommandBuffer
 */
PAL_API PalResult PAL_CALL palAllocateCommandBuffer(
    PalDevice* device,
    PalCommandPool* pool,
    PalCommandBufferType type,
    PalCommandBuffer** outCmdBuffer);

/**
 * @brief Free an allocated command buffer.
 *
 * @param[in] cmdBuffer Command buffer to free.
 *
 * @Thread-safety the command pool used to create the command buffer is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palAllocateCommandBuffer
 */
PAL_API void PAL_CALL palFreeCommandBuffer(PalCommandBuffer* cmdBuffer);

/**
 * @brief Reset the provided command buffer.
 *
 * @param[in] cmdBuffer Command buffer to reset.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palResetCommandBuffer(PalCommandBuffer* cmdBuffer);

/**
 * @brief Submit a command buffer to the provided queue for execution.
 *
 * The command buffer must not be in a recording state.
 *
 * @param[in] queue Queue to execute the command buffer.
 * @param[in] info Pointer to a PalCommandBufferSubmitInfo struct that specifies parameters.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `queue` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palSubmitCommandBuffer(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info);

/**
 * @brief Begin recording commands to the provided command buffer.
 *
 * This function must be called before any other `palCmd**` function is used.
 *
 * @param[in] cmdBuffer Command buffer to begin recording.
 * @param[in] info Tmp
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCmdEnd
 */
PAL_API PalResult PAL_CALL palCmdBegin(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info);

/**
 * @brief End recording commands to the provided command buffer.
 *
 * @param[in] cmdBuffer Command buffer to begin recording.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCmdBegin
 */
PAL_API PalResult PAL_CALL palCmdEnd(PalCommandBuffer* cmdBuffer);

/**
 * @brief Execute a secondary command buffer within a primary command buffer.
 *
 * The `secondaryCmdBuffer` must be created with the type `PAL_COMMAND_BUFFER_TYPE_SECONDARY`.
 *
 * @param[in] primaryCmdBuffer Primary command buffer. Must be in recording state.
 * @param[in] secondaryCmdBuffer Secondary command buffer.
 *
 * @Thread-safety `primaryCmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdExecuteCommandBuffer(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer);

/**
 * @brief Set the fragment shading rate used for draw calls.
 *
 * `PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] state Pointer to a PalFragmentShadingRateState struct that specifies parameters.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdSetFragmentShadingRate(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state);

/**
 * @brief Dispatch mesh shader workgroups.
 *
 * `PAL_ADAPTER_FEATURE_MESH_SHADER` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] groupCountX Number of mesh shader groups to dispatch on the x axis.
 * @param[in] groupCountY Number of mesh shader groups to dispatch on the y axis.
 * @param[in] groupCountZ Number of mesh shader groups to dispatch on the z axis.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palBuildWorkGroupInfo
 */
PAL_API void PAL_CALL palCmdDrawMeshTasks(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ);

/**
 * @brief Dispatch mesh shader workgroups using parameters from a buffer.
 *
 * `PAL_ADAPTER_FEATURE_MESH_SHADER` and `PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH` must be supported
 * and enabled by the device. Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDispatchIndirectData structs.
 * @param[in] drawCount Number of draws to perform.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palBuildWorkGroupInfo
 */
PAL_API void PAL_CALL palCmdDrawMeshTasksIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t drawCount);

/**
 * @brief Dispatch mesh shader workgroups using parameters from buffers.
 *
 * `PAL_ADAPTER_FEATURE_MESH_SHADER` and `PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT` must be
 * supported and enabled by the device. Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDispatchIndirectData structs.
 * @param[in] countBuffer Buffer containing a single `uint32_t` specifying the number of draws.
 * @param[in] maxDrawCount Maximum Number of draws to perform.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palBuildWorkGroupInfo
 */
PAL_API void PAL_CALL palCmdDrawMeshTasksIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount);

/**
 * @brief Build or update an acceleration structure.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] info Pointer to a PalAccelerationStructureBuildInfo struct that specifies parameters.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdBuildAccelerationStructure(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info);

/**
 * @brief Begin a rendering pass.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] info Pointer to a PalRenderingInfo struct that specifies parameters.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdBeginRendering(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info);

/**
 * @brief End a rendering pass.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdEndRendering(PalCommandBuffer* cmdBuffer);

/**
 * @brief Copy data from one buffer to the other.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] dst Destination buffer.
 * @param[in] src Source buffer.
 * @param[in] copyInfo Pointer to a PalBufferCopyInfo struct that specifies parameters.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdCopyBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    PalBufferCopyInfo* copyInfo);

/**
 * @brief Copy data from a buffer to an image.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] dstImage Destination image.
 * @param[in] srcBuffer Source buffer.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdCopyBufferToImage(
    PalCommandBuffer* cmdBuffer,
    PalImage* dstImage,
    PalBuffer* srcBuffer,
    PalBufferImageCopyInfo* copyInfo);

/**
 * @brief Copy data from one image to the other.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] dst Destination image.
 * @param[in] src Source image.
 * @param[in] copyInfo Pointer to a PalImageCopyInfo struct that specifies parameters.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdCopyImage(
    PalCommandBuffer* cmdBuffer,
    PalImage* dst,
    PalImage* src,
    PalImageCopyInfo* copyInfo);

/**
 * @brief Copy data from an image to a buffer.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] dstBuffer Destination buffer.
 * @param[in] srcImage Source image.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdCopyImageToBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dstBuffer,
    PalImage* srcImage,
    PalBufferImageCopyInfo* copyInfo);

/**
 * @brief Bind a pipeline.
 *
 * Every pipeline knows it types which is set at the respective creation functions.
 * (`palCreate**Graphics/Compute/RayTracing**Pipeline`).
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] pipeline Pipeline to bind.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdBindPipeline(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline);

/**
 * @brief Set the viewport(s) used in draw commands.
 *
 * This always overwrites any previous viewports that were set since the first viewport
 * index is always 0.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] count Capacity of the PalViewport array.
 * @param[in] viewports Pointer to an array of viewports.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdSetViewport(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalViewport* viewports);

/**
 * @brief Set the scissor(s) used in draw commands.
 *
 * This always overwrites any previous scissors that were set since the first
 * scissor index is always 0.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] count Capacity of the PalRect2D array.
 * @param[in] scissors Pointer to an array of scissors.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdSetScissors(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalRect2D* scissors);

/**
 * @brief Bind vertex buffer(s) used in draw commands.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] firstSlot Index of the first vertex buffer binding slot.
 * @param[in] count Number of vertex buffers to bind.
 * @param[in] buffers Pointer to an array of vertex buffers.
 * @param[in] offsets Pointer to an array of offsets in bytes into each vertex buffer.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdBindVertexBuffers(
    PalCommandBuffer* cmdBuffer,
    uint32_t firstSlot,
    uint32_t count,
    PalBuffer** buffers,
    uint64_t* offsets);

/**
 * @brief Bind index buffer used in draw commands.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Index buffer to bind.
 * @param[in] offset Offset in bytes into the index buffer.
 * @param[in] type Type of indices stored in the index buffer. (eg. `PAL_INDEX_TYPE_UINT32`).
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdBindIndexBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint64_t offset,
    PalIndexType type);

/**
 * @brief Issue a non-indexed draw command.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] vertexCount Number of vertices to draw.
 * @param[in] instanceCount Number of instances to draw.
 * @param[in] firstVertex Index of the first vertex to draw.
 * @param[in] firstInstance Index of the first instance to draw.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palDrawIndexed
 */
PAL_API void PAL_CALL palCmdDraw(
    PalCommandBuffer* cmdBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance);

/**
 * @brief Issue a non-indexed draw command using buffers.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DRAW` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDrawIndirectData structs.
 * Can be a single struct.
 * @param[in] count Number of draws to perform.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palDrawIndexedIndirect
 */
PAL_API void PAL_CALL palCmdDrawIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count);

/**
 * @brief Issue a non-indexed draw command using buffers.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDrawIndirectData structs.
 * @param[in] countBuffer Buffer containing a single `uint32_t` specifying the number of draws.
 * @param[in] maxDrawCount Maximum Number of draws to perform.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCmdDrawIndexedIndirectCount
 */
PAL_API void PAL_CALL palCmdDrawIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount);

/**
 * @brief Issue an indexed draw command.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] indexCount Number of indices to draw.
 * @param[in] instanceCount Number of instances to draw.
 * @param[in] firstIndex Index of the first index to draw.
 * @param[in] vertexOffset Added offset to vertex indices.
 * @param[in] firstInstance Index of the first instance to draw.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCmdDraw
 */
PAL_API void PAL_CALL palCmdDrawIndexed(
    PalCommandBuffer* cmdBuffer,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance);

/**
 * @brief Issue an indexed draw command using buffers.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DRAW` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDrawIndexedIndirectData structs.
 * @param[in] count Number of draws to perform.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCmdDrawIndirect
 */
PAL_API void PAL_CALL palCmdDrawIndexedIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count);

/**
 * @brief Issue an indexed draw command using buffers.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDrawIndexedIndirectData structs.
 * @param[in] countBuffer Buffer containing a single `uint32_t` specifying the number of draws.
 * @param[in] maxDrawCount Maximum Number of draws to perform.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCmdDrawIndirectCount
 */
PAL_API void PAL_CALL palCmdDrawIndexedIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount);

/**
 * @brief Transition an acceleration structure from one usage state to another.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * This function defines a dependency between `PalBarrierInfo::oldState` and
 * `PalBarrierInfo::newState`. It ensures that all operations performed under the old
 * `PalBarrierInfo::oldState` are completed and visible before the acceleration structure
 * is accessed under `PalBarrierInfo::newState`.
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
 * whilst BLAS is buidling, we put a barrier to transition the BLAS to ensure it has finished
 * building and the scratch buffer is not being used. This is expressed with
 * `PalBarrierInfo::oldState` being set to `PAL_USAGE_STATE_ACCELERATION_STRUCTURE_WRITE` and
 * `PalBarrierInfo::srcStages` being set to `PAL_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD`.
 *
 * `PalBarrierInfo::newState` being `PAL_USAGE_STATE_ACCELERATION_STRUCTURE_READ` and
 * `PalBarrierInfo::dstStages` being `PAL_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] as Acceleration structure to set barrier on.
 * @param[in] info Pointer to a PalBarrierInfo struct that specifies parameters.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCmdImageBarrier
 * @sa palCmdBufferBarrier
 * @sa palCanQueueUseUsageState
 * @sa palCanQueueUsePipelineStages
 */
PAL_API void PAL_CALL palCmdAccelerationStructureBarrier(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructure* as,
    PalBarrierInfo* info);

/**
 * @brief Transition an image from one usage state to another.
 *
 * This function defines a dependency between `PalBarrierInfo::oldState` and
 * `PalBarrierInfo::newState`. It ensures that all operations performed under
 * `PalBarrierInfo::oldState` are completed and visible before the image is accessed under
 * `PalBarrierInfo::newState`.
 *
 * This function does not modify the image, it only exforces execution ordering and image memory
 * visibility.
 *
 * Example:
 *
 * To make sure the an image is ready for presenting after a render pass,
 * we put a barrier to transition the image to ensure the render pass has finished writing
 * to the image. This is expressed with `PalBarrierInfo::oldState` being
 * `PAL_USAGE_STATE_COLOR_ATTACHMENT` and `PalBarrierInfo::srcStages` being
 * `PAL_PIPELINE_STAGE_COLOR_ATTACHMENT`.
 *
 * `PalBarrierInfo::newState` being  `PAL_USAGE_STATE_PRESENT` and
 * `PalBarrierInfo::dstStages` being `PAL_PIPELINE_STAGE_NONE`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] image Image to set barrier on.
 * @param[in] subresourceRange Subresource range of the image.
 * @param[in] info Pointer to a PalBarrierInfo struct that specifies parameters.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCmdAccelerationStructureBarrier
 * @sa palCmdBufferBarrier
 * @sa palCanQueueUseUsageState
 * @sa palCanQueueUsePipelineStages
 */
PAL_API void PAL_CALL palCmdImageBarrier(
    PalCommandBuffer* cmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalBarrierInfo* info);

/**
 * @brief Transition an image from one usage state to another across queues,
 * optionally transferring ownership.
 *
 * The adapter used to create the command buffer's device must support
 * `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2` or later.
 *
 * The source command buffer releases ownership of the image after `srcUsageState` and optionally
 * `srcPipelineStages` operations are completed. The destination command buffer acquires the
 * image and transition it to `PAL_USAGE_STATE_UNDEFINED` and `PAL_PIPELINE_STAGE_NONE` as the
 * default state. The image must the transitioned to the proper state before it is used by
 * the destination command buffer after this call.
 *
 * Both command buffers must not be able to share resource ownership otherwise, this function
 * sets a normal barrier on the source command buffer ignoring the destination buffer. Call
 * `palCanQueueShareOwnership()` to check if both command buffer queues can share resources.
 *
 * @param[in] srcCmdBuffer Source command buffer being recorded.
 * @param[in] dstCmdBuffer Destination command buffer being recorded.
 * @param[in] image Image to set barrier on and transfer ownership.
 * @param[in] subresourceRange Subresource range of the image.
 * @param[in] srcUsageState Usage state of the image on the source command buffer.
 * @param[in] srcPipelineStages Source pipeline stages.
 *
 * @Thread-safety `srcCmdBuffer` and `dstCmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.1
 * @ingroup pal_graphics
 * 
 * @sa palCmdBufferOwnershipTransfer
 * @sa palCanQueueUseUsageState
 * @sa palCanQueueUsePipelineStages
 */
PAL_API void PAL_CALL palCmdImageOwnershipTransfer(
    PalCommandBuffer* srcCmdBuffer,
    PalCommandBuffer* dstCmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalUsageState srcUsageState,
    PalPipelineStages srcPipelineStages);

/**
 * @brief Transition a buffer from one usage state to another.
 *
 * This function defines a dependency between `PalBarrierInfo::oldState` and
 * `PalBarrierInfo::newState`. It ensures that all operations performed under
 * `PalBarrierInfo::oldState` are completed and visible before the buffer is accessed
 * under `PalBarrierInfo::newState`.
 *
 * This function does not modify the buffer, it only exforces execution ordering and buffer memory
 * visibility.
 *
 * Example:
 *
 * To read back data from a buffer that will be written to by a shader,
 * we put a barrier to transition the buffer to ensure the shader has finished writing to the
 * buffer. This is expressed with `PalBarrierInfo::oldState` being `PAL_USAGE_STATE_SHADER_WRITE`
 * and `PalBarrierInfo::srcStages` being the shader stage that wrote to the buffer
 * (eg. `PAL_PIPELINE_STAGE_COMPUTE_SHADER`).
 *
 * `PalBarrierInfo::newState` being `PAL_USAGE_STATE_TRANSFER_READ` and
 * `PalBarrierInfo::dstStages` being `PAL_PIPELINE_STAGE_TRANSFER`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer to set barrier on.
 * @param[in] info Pointer to a PalBarrierInfo struct that specifies parameters.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCmdAccelerationStructureBarrier
 * @sa palCmdImageBarrier
 * @sa palCanQueueUseUsageState
 * @sa palCanQueueUsePipelineStages
 */
PAL_API void PAL_CALL palCmdBufferBarrier(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBarrierInfo* info);

/**
 * @brief Transition a buffer from one usage state to another across queues,
 * optionally transferring ownership.
 *
 * The adapter used to create the command buffer's device must support
 * `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2` or later.
 *
 * The source command buffer releases ownership of the buffer after `srcUsageState` and optionally
 * `srcPipelineStages` operations are completed. The destination command buffer acquires the
 * buffer and transition it to `PAL_USAGE_STATE_UNDEFINED` and `PAL_PIPELINE_STAGE_NONE` as the
 * default state. The buffer must the transitioned to the proper state before it is used by
 * the destination command buffer after this call.
 *
 * Both command buffers must not be able to share resource ownership otherwise, this function
 * sets a normal barrier on the source command buffer ignoring the destination buffer. Call
 * `palCanQueueShareOwnership()` to check if both command buffer queues can share resources.
 *
 * @param[in] srcCmdBuffer Source command buffer being recorded.
 * @param[in] dstCmdBuffer Destination command buffer being recorded.
 * @param[in] buffer Buffer to set barrier on and transfer ownership.
 * @param[in] srcUsageState Usage state of the buffer on the source command buffer.
 * @param[in] srcPipelineStages Source pipeline stages.
 *
 * @Thread-safety `srcCmdBuffer` and `dstCmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.1
 * @ingroup pal_graphics
 * 
 * @sa palCmdImageOwnershipTransfer
 * @sa palCanQueueUseUsageState
 * @sa palCanQueueUsePipelineStages
 */
PAL_API void PAL_CALL palCmdBufferOwnershipTransfer(
    PalCommandBuffer* srcCmdBuffer,
    PalCommandBuffer* dstCmdBuffer,
    PalBuffer* buffer,
    PalUsageState srcUsageState,
    PalPipelineStages srcPipelineStages);

/**
 * @brief Dispatch compute shader workgroups.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] groupCountX Number of compute shader groups to dispatch on the x axis.
 * @param[in] groupCountY Number of compute shader groups to dispatch on the y axis.
 * @param[in] groupCountZ Number of compute shader groups to dispatch on the z axis.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palBuildWorkGroupInfo
 */
PAL_API void PAL_CALL palCmdDispatch(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ);

/**
 * @brief Dispatch compute shader workgroups with base offset.
 *
 * `PAL_ADAPTER_FEATURE_DISPATCH_BASE` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] baseGroupX Base group offset on the x axis.
 * @param[in] baseGroupY Base group offset on the y axis.
 * @param[in] baseGroupZ Base group offset on the z axis.
 * @param[in] groupCountX Number of compute shader groups to dispatch on the x axis.
 * @param[in] groupCountY Number of compute shader groups to dispatch on the y axis.
 * @param[in] groupCountZ Number of compute shader groups to dispatch on the z axis.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palBuildWorkGroupInfo
 */
PAL_API void PAL_CALL palCmdDispatchBase(
    PalCommandBuffer* cmdBuffer,
    uint32_t baseGroupX,
    uint32_t baseGroupY,
    uint32_t baseGroupZ,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ);

/**
 * @brief Dispatch compute shader workgroups using parameters from a buffer.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DISPATCH` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing the PalDispatchIndirectData struct.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palBuildWorkGroupInfo
 */
PAL_API void PAL_CALL palCmdDispatchIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer);

/**
 * @brief Dispatch rays.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] sbt The shader binding table to use.
 * @param[in] raygenIndex Index of the raygen shader to execute.
 * @param[in] width Number of rays to trace on the x axis.
 * @param[in] height Number of rays to trace on the y axis.
 * @param[in] depth Number of rays to trace on the z axis.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdTraceRays(
    PalCommandBuffer* cmdBuffer,
    PalShaderBindingTable* sbt,
    uint32_t raygenIndex,
    uint32_t width,
    uint32_t height,
    uint32_t depth);

/**
 * @brief Dispatch rays using parameters from a buffer.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] raygenIndex Index of the raygen shader to execute.
 * @param[in] sbt The shader binding table to use.
 * @param[in] buffer Buffer containing the PalDispatchIndirectData struct.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note The argument buffer memory must not be `PAL_MEMORY_TYPE_CPU_UPLOAD`. The implementation
 * internally copies the data into a GPU buffer for execution.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdTraceRaysIndirect(
    PalCommandBuffer* cmdBuffer,
    uint32_t raygenIndex,
    PalShaderBindingTable* sbt,
    PalBuffer* buffer);

/**
 * @brief Bind a descriptor set to the provided command buffer.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] setIndex Index of the descriptor set to bind.
 * @param[in] set Descriptor set to bind. Must be compatible with `layout`.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdBindDescriptorSet(
    PalCommandBuffer* cmdBuffer,
    uint32_t setIndex,
    PalDescriptorSet* set);

/**
 * @brief Update push constant data for the provided command buffer.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] offset Offset in bytes into the push constant range.
 * @param[in] size Size of `value` in bytes.
 * @param[in] value Pointer to the push constant range data to write.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdPushConstants(
    PalCommandBuffer* cmdBuffer,
    uint32_t offset,
    uint32_t size,
    const void* value);

/**
 * @brief Set the cull mode for the provided command buffer.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] cullMode Cull mode to set.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdSetCullMode(
    PalCommandBuffer* cmdBuffer,
    PalCullMode cullMode);

/**
 * @brief Set the front face for the provided command buffer.s
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] frontFace Front face to set.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdSetFrontFace(
    PalCommandBuffer* cmdBuffer,
    PalFrontFace frontFace);

/**
 * @brief Set the primitive topology for the provided command buffer.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] topology Topology to set.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdSetPrimitiveTopology(
    PalCommandBuffer* cmdBuffer,
    PalPrimitiveTopology topology);

/**
 * @brief Set depth test enable for the provided command buffer.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] enable True to enable.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdSetDepthTestEnable(
    PalCommandBuffer* cmdBuffer,
    PalBool enable);

/**
 * @brief Set depth write enable for the provided command buffer.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] enable True to enable.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdSetDepthWriteEnable(
    PalCommandBuffer* cmdBuffer,
    PalBool enable);

/**
 * @brief Set depth stencil operation for the provided command buffer.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP` must be supported and enabled by the device.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] faceMask Bitmask specifying faces to apply the stencil to.
 * @param[in] failOp Stencil operation to perform when stencil fails.
 * @param[in] passOp Stencil operation to perform when stencil and depth passes.
 * @param[in] depthFailOp Stencil operation to perform when stencil passes but depth fails.
 * @param[in] compareOp Compare operation for stencil tests.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdSetStencilOp(
    PalCommandBuffer* cmdBuffer,
    PalStencilFaceFlags faceMask,
    PalStencilOp failOp,
    PalStencilOp passOp,
    PalStencilOp depthFailOp,
    PalCompareOp compareOp);

/**
 * @brief Create an acceleration structure.
 *
 * The created acceleration structure must be destroyed using `palDestroyAccelerationStructure()`.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] device Device that creates the acceleration structure.
 * @param[in] info Pointer to a PalAccelerationStructureCreateInfo struct that specifies parameters
 * @param[out] outAs Pointer to a PalAccelerationStructure to recieve the created acceleration
 * structure.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palDestroyAccelerationStructure
 */
PAL_API PalResult PAL_CALL palCreateAccelerationstructure(
    PalDevice* device,
    const PalAccelerationStructureCreateInfo* info,
    PalAccelerationStructure** outAs);

/**
 * @brief Destroy an acceleration structure.
 *
 * @param[in] as Acceleration structure to destroy.
 *
 * @Thread-safety the device used to create the acceleration structure is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCreateAccelerationstructure
 */
PAL_API void PAL_CALL palDestroyAccelerationStructure(PalAccelerationStructure* as);

/**
 * @brief Get the build size of an acceleration structure.
 *
 * PalAccelerationStructureBuildInfo::dst, PalAccelerationStructureBuildInfo::scratchBufferAddress
 * and PalAccelerationStructureBuildInfo::src must be set to `nullptr`.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] device Device to query.
 * @param[in] info Pointer to a PalAccelerationStructureBuildInfo struct that specifies parameters.
 * @param[out] size Pointer to a PalAccelerationStructureBuildSize to recieve the build size.
 *
 * @Thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palGetAccelerationStructureBuildSize(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size);

/**
 * @brief Create a buffer.
 *
 * The created buffer must be destroyed using `palDestroyBuffer()`.
 *
 * `PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS` must be supported and enabled by the devic.
 * Otherwise behavior is undefined.
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
 * @param[out] outBuffer Pointer to a PalBuffer to recieve the created buffer.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palDestroyBuffer
 */
PAL_API PalResult PAL_CALL palCreateBuffer(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer);

/**
 * @brief Destroy a buffer.
 *
 * @param[in] buffer buffer to destroy.
 *
 * @Thread-safety the device used to create the buffer is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCreateBuffer
 */
PAL_API void PAL_CALL palDestroyBuffer(PalBuffer* buffer);

/**
 * @brief Get memory requirements for the provided buffer.
 *
 * @param[in] buffer Buffer to query memory requirements on.
 * @param[out] requirements Pointer to a PalMemoryRequirements to fill.
 *
 * @Thread-safety `requirements` is per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palGetBufferMemoryRequirements(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements);

/**
 * @brief Compute size for an acceleration structure instance buffer.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * This does not allocate memory for the buffer. This function must is required for all
 * acceleration structure instance buffers.
 *
 * @param[in] device The device to use.
 * @param[in] instanceCount Number of instances the instance buffer will hold.
 * @param[out] outSize Pointer to a uint64_t to recieve the required size.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palWriteInstanceStaging
 */
PAL_API void PAL_CALL palComputeInstanceStagingSize(
    PalDevice* device,
    uint32_t instanceCount,
    uint64_t* outSize);

/**
 * @brief Compute requirements for an image staging buffer.
 *
 * This does not allocate memory for the buffer. This function is required for all
 * image copy staging buffers.
 *
 * `PalBufferImageCopyInfo::bufferRowLength` and `PalBufferImageCopyInfo::bufferImageHeight`
 * are hints. The driver might used it defaults if the requested is not supported. After this call,
 * set those values to the required ones from `requirements`.
 * If the driver supports the proivded, the values will be the same.
 *
 * @param[in] device The device to use.
 * @param[in] imageFormat Destination image format.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 * @param[out] requirements Pointer to a PalImageStagingRequirements to recieve the requirements
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palWriteImageStaging
 */
PAL_API void PAL_CALL palComputeImageStagingRequirements(
    PalDevice* device,
    PalFormat imageFormat,
    const PalBufferImageCopyInfo* copyInfo,
    PalImageStagingRequirements* requirements);

/**
 * @brief Write data to an instance staging buffer.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] device The device to use.
 * @param[in] instanceCount Number of instances.
 * @param[in] instances Array of PalAccelerationStructureInstance struct to write.
 * @param[out] ptr Pointer to the CPU visible memory. Must be mapped.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palComputeInstanceStagingSize
 */
PAL_API void PAL_CALL palWriteInstanceStaging(
    PalDevice* device,
    uint32_t instanceCount,
    PalAccelerationStructureInstance* instances,
    void* ptr);

/**
 * @brief Write data to an image staging buffer.
 *
 * @param[in] device The device to use.
 * @param[in] imageFormat Destination image format.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 * @param[out] srcData Pointer to the CPU visible memory with the data.
 * @param[out] ptr Pointer to the CPU visible memory. Must be mapped.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palComputeImageStagingRequirements
 */
PAL_API void PAL_CALL palWriteImageStaging(
    PalDevice* device,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo,
    void* srcData,
    void* ptr);

/**
 * @brief Bind an allocated memory to a buffer.
 *
 * The memory size and alignment should match the requirements of the buffer.
 * Get the requirements with palGetBufferMemoryRequirements().
 *
 * @param[in] buffer Buffer to bind memory to.
 * @param[in] memory Memory to bind.
 * @param[in] offset Starting point within the memory.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `requirements` is per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palGetBufferMemoryRequirements
 */
PAL_API PalResult PAL_CALL palBindBufferMemory(
    PalBuffer* buffer,
    PalMemory* memory,
    uint64_t offset);

/**
 * @brief Maps buffer to CPU visible address space.
 *
 * The buffer must have a valid memory bound to it before this call.
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
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `buffer` must be externally synchronized.
 * Mapping with different offsets into the same buffer is thread safe as long as `buffer`
 * must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palUnmapBuffer
 */
PAL_API PalResult PAL_CALL palMapBuffer(
    PalBuffer* buffer,
    uint64_t offset,
    uint64_t size,
    void** outPtr);

/**
 * @brief Unmap buffer from CPU visible address space.
 *
 * The buffer must be mapped before this call. After this call, the CPU pointer must not
 * be used anymore.
 *
 * @param[in] buffer Pointer to buffer to unmap.
 *
 * @Thread-safety `buffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palMapBuffer
 */
PAL_API void PAL_CALL palUnmapBuffer(PalBuffer* buffer);

/**
 * @brief Get the device address of the provided buffer.
 *
 * Buffer must have `PAL_BUFFER_USAGE_DEVICE_ADDRESS` usage flag.
 *
 * @param[in] buffer Buffer to get its device address.
 *
 * @return Buffer device address on success or `0` on failure.
 *
 * @Thread-safety `buffer` is per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalDeviceAddress PAL_CALL palGetBufferDeviceAddress(PalBuffer* buffer);

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
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
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
 * @Thread-safety the device used to create the descriptor set layout is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
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
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
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
 * @Thread-safety the device used to create the descriptor pool is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCreateDescriptorPool
 */
PAL_API void PAL_CALL palDestroyDescriptorPool(PalDescriptorPool* pool);

/**
 * @brief Reset the provided descriptor pool. This resets all allocated descriptor sets.
 *
 * @param[in] pool Descriptor pool to reset.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `pool` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
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
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` and `pool` must be externally synchronized.
 *
 * @since Added in version 2.0
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
 * If the write info has no valid resource handle, then `PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS`
 * must be supported and enabled when creating the device. Otherwise behavior is undefined.
 *
 * @param[in] device The Device. Must match the one used to allocate descriptor set.
 * @param[in] count Capacity of the PalDescriptorSetWriteInfo array.
 * @param[in] infos Array of PalDescriptorSetWriteInfo to write.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palUpdateDescriptorSet(
    PalDevice* device,
    uint32_t count,
    PalDescriptorSetWriteInfo* infos);

/**
 * @brief Create a pipeline layout. This defines the descriptor set interfaces and push
 * constant info.
 *
 * The created pipeline layout must be destroyed using `palDestroyPipelineLayout()`.
 *
 * @param[in] device Device that creates the pipeline layout.
 * @param[in] info Pointer to a PalPipelineLayoutCreateInfo struct that specifies parameters.
 * @param[out] outLayout Pointer to a PalPipelineLayout to recieve the created pipeline layout.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palDestroyPipelineLayout
 */
PAL_API PalResult PAL_CALL palCreatePipelineLayout(
    PalDevice* device,
    const PalPipelineLayoutCreateInfo* info,
    PalPipelineLayout** outLayout);

/**
 * @brief Destroy a pipeline layout.
 *
 * @param[in] layout Pipeline layout to destroy.
 *
 * @Thread-safety the device used to create the pipeline layout is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCreatePipelineLayout
 */
PAL_API void PAL_CALL palDestroyPipelineLayout(PalPipelineLayout* layout);

/**
 * @brief Create a graphics pipeline.
 *
 * The created pipeline must be destroyed using `palDestroyPipeline()`.
 *
 * @param[in] device Device that creates the graphics pipeline.
 * @param[in] info Pointer to a PalGraphicsPipelineCreateInfo struct that specifies parameters.
 * @param[out] outPipeline Pointer to a PalPipeline to recieve the created buffer.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palDestroyPipeline
 */
PAL_API PalResult PAL_CALL palCreateGraphicsPipeline(
    PalDevice* device,
    const PalGraphicsPipelineCreateInfo* info,
    PalPipeline** outPipeline);

/**
 * @brief Create a compute pipeline.
 *
 * The created pipeline must be destroyed using `palDestroyPipeline()`.
 *
 * @param[in] device Device that creates the compute pipeline.
 * @param[in] info Pointer to a PalComputePipelineCreateInfo struct that specifies parameters.
 * @param[out] outPipeline Pointer to a PalPipeline to recieve the created buffer.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @note The first entry of the compute shader will be used.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palDestroyPipeline
 */
PAL_API PalResult PAL_CALL palCreateComputePipeline(
    PalDevice* device,
    const PalComputePipelineCreateInfo* info,
    PalPipeline** outPipeline);

/**
 * @brief Create a ray tracing pipeline.
 *
 * The created pipeline must be destroyed using `palDestroyPipeline()`.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] device Device that creates the ray tracing pipeline.
 * @param[in] info Pointer to a PalRayTracingPipelineCreateInfo struct that specifies parameters.
 * @param[out] outPipeline Pointer to a PalPipeline to recieve the created buffer.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palDestroyPipeline
 */
PAL_API PalResult PAL_CALL palCreateRayTracingPipeline(
    PalDevice* device,
    const PalRayTracingPipelineCreateInfo* info,
    PalPipeline** outPipeline);

/**
 * @brief Destroy a pipeline.
 *
 * @param[in] pipeline Pipeline to destroy.
 *
 * @Thread-safety the device used to create the pipeline is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCreateGraphicsPipeline
 * @sa palCreateComputePipeline
 * @sa palCreateRayTracingPipeline
 */
PAL_API void PAL_CALL palDestroyPipeline(PalPipeline* pipeline);

/**
 * @brief Create a shader binding table.
 *
 * The created shader binding table must be destroyed using `palDestroyShaderBindingTable()`.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * PalShaderBindingTableCreateInfo::recordCount must match the shader group count of
 * PalShaderBindingTableCreateInfo::rayTracingPipeline.
 *
 * @param[in] device Device that creates the shader binding table.
 * @param[in] info Pointer to a PalShaderBindingTableCreateInfo struct that specifies parameters.
 * @param[out] outSbt Pointer to a PalShaderBindingTable to recieve the created shader binding
 * table.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult() for more information.
 *
 * @Thread-safety Must only be called from the main thread.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @Thread-safety `device` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palDestroyShaderBindingTable
 */
PAL_API PalResult PAL_CALL palCreateShaderBindingTable(
    PalDevice* device,
    const PalShaderBindingTableCreateInfo* info,
    PalShaderBindingTable** outSbt);

/**
 * @brief Destroy a shader binding table.
 *
 * @param[in] sbt Shader binding table to destroy.
 *
 * @Thread-safety the device used to create the shader binding table is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCreateShaderBindingTable
 */
PAL_API void PAL_CALL palDestroyShaderBindingTable(PalShaderBindingTable* sbt);

/**
 * @brief Update a shader binding table record payloads.
 *
 * This call does not update shader handles. It only updates the payload associated
 * with the record. PalShaderBindingTableRecordInfo::groupIndex is the index into
 * the shader groups used to create the ray tracing pipeline.
 *
 * @param[in] sbt The shader binding table to update.
 * @param[in] count Capacity of the PalShaderBindingTableRecordInfo array.
 * @param[in] infos Array of PalShaderBindingTableRecordInfo to update.
 *
 * @Thread-safety `sbt` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palUpdateShaderBindingTable(
    PalShaderBindingTable* sbt,
    uint32_t count,
    PalShaderBindingTableRecordInfo* infos);

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
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 * 
 * @sa palCmdDrawMeshTasks
 * @sa palCmdDrawMeshTasksIndirect
 * @sa palCmdDrawMeshTasksIndirectCount
 * @sa palCmdDispatch
 * @sa palCmdDispatchBase
 */
PAL_API void PAL_CALL palBuildWorkGroupInfo(
    const PalWorkGroupBuildData* data,
    uint32_t* count,
    PalWorkGroupInfo* infos);

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
 * @Thread-safety Thread safe.
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

#endif // PAL_GRAPHICS_H
