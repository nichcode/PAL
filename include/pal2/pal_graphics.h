/**
 * @brief This is the header file for PAL Graphics API.
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
 */

#ifndef PAL_GRAPHICS_H
#define PAL_GRAPHICS_H

#include "pal_core.h"

#define PAL_SHADER_ENTRY_NAME_SIZE 32

#define PAL_UNUSED_SHADER_INDEX UINT32_MAX
#define PAL_MAX_CUSTOM_BACKENDS 16

#define PAL_MAKE_SHADER_TARGET(major, minor) ((uint32_t)((major) << 8) | (minor))
#define PAL_SHADER_TARGET_MAJOR(target) ((uint32_t)(target) >> 8);
#define PAL_SHADER_TARGET_MINOR(target) ((uint32_t)(target) & 0xFF);

/**
 * @defgroup queue_types Queue Types
 * @brief Queue types
 * 
 * @{
 */
#define PAL_QUEUE_TYPE_GRAPHICS 0
#define PAL_QUEUE_TYPE_COMPUTE 1
#define PAL_QUEUE_TYPE_COPY 2
#define PAL_QUEUE_TYPE_COUNT 3
/** @} */

/**
 * @defgroup present_modes Present Modes
 * @brief Present modes
 * 
 * @{
 */
#define PAL_PRESENT_MODE_FIFO 0
#define PAL_PRESENT_MODE_IMMEDIATE 1
#define PAL_PRESENT_MODE_MAILBOX 2
#define PAL_PRESENT_MODE_COUNT 3
/** @} */

/**
 * @defgroup composite_alphas Composite Alphas
 * @brief Composite alphas
 * 
 * @{
 */
#define PAL_COMPOSITE_ALPHA_OPAQUE 0
#define PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED 1
#define PAL_COMPOSITE_ALPHA_POST_MULTIPLIED 2
#define PAL_COMPOSITE_ALPHA_COUNT 3
/** @} */

/**
 * @defgroup formats Format
 * @brief Format
 * 
 * @{
 */
#define PAL_FORMAT_UNDEFINED 0
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
#define PAL_FORMAT_COUNT 81
/** @} */

/**
 * @defgroup image_usages Image Usages
 * @brief Image usages
 * 
 * @{
 */
#define PAL_IMAGE_USAGE_UNDEFINED 0
#define PAL_IMAGE_USAGE_COLOR_ATTACHEMENT (1U << 0)
#define PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT (1U << 1)
#define PAL_IMAGE_USAGE_TRANSFER_SRC (1U << 2)
#define PAL_IMAGE_USAGE_TRANSFER_DST (1U << 3)
#define PAL_IMAGE_USAGE_STORAGE (1U << 4)
#define PAL_IMAGE_USAGE_SAMPLED (1U << 5)
/** @} */

/**
 * @defgroup load_ops Load Operations
 * @brief Load operations
 * 
 * @{
 */
#define PAL_LOAD_OP_LOAD 0
#define PAL_LOAD_OP_CLEAR 1
#define PAL_LOAD_OP_DONT_CARE 2
#define PAL_LOAD_OP_COUNT 3
/** @} */

/**
 * @defgroup store_operations Store Operations
 * @brief Store operations
 * 
 * @{
 */
#define PAL_STORE_OP_STORE 0
#define PAL_STORE_OP_DONT_CARE 1
#define PAL_STORE_OP_COUNT 2
/** @} */

/**
 * @defgroup memory_types Memory Types
 * @brief Memory types
 * 
 * @{
 */
#define PAL_MEMORY_TYPE_GPU_ONLY 0
#define PAL_MEMORY_TYPE_CPU_UPLOAD 1
#define PAL_MEMORY_TYPE_CPU_READBACK 2
#define PAL_MEMORY_TYPE_COUNT 3
/** @} */

/**
 * @defgroup image_types Image Types
 * @brief Image types
 * 
 * @{
 */
#define PAL_IMAGE_TYPE_1D 0
#define PAL_IMAGE_TYPE_2D 1
#define PAL_IMAGE_TYPE_3D 2
#define PAL_IMAGE_TYPE_COUNT 3
/** @} */

/**
 * @defgroup image_aspects Image Aspects
 * @brief Image aspects
 * 
 * @{
 */
#define PAL_IMAGE_ASPECT_COLOR 0
#define PAL_IMAGE_ASPECT_DEPTH 1
#define PAL_IMAGE_ASPECT_STENCIL 2
#define PAL_IMAGE_ASPECT_DEPTH_STENCIL 3
#define PAL_IMAGE_ASPECT_COUNT 4
/** @} */

/**
 * @defgroup image_view_types Image View Types
 * @brief Image view types
 * 
 * @{
 */
#define PAL_IMAGE_VIEW_TYPE_1D 0
#define PAL_IMAGE_VIEW_TYPE_1D_ARRAY 1
#define PAL_IMAGE_VIEW_TYPE_2D 2
#define PAL_IMAGE_VIEW_TYPE_2D_ARRAY 3
#define PAL_IMAGE_VIEW_TYPE_3D 4
#define PAL_IMAGE_VIEW_TYPE_CUBE 5
#define PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY 6
#define PAL_IMAGE_VIEW_TYPE_COUNT 7
/** @} */

/**
 * @defgroup filter_modes Filter Modes
 * @brief Filter modes
 * 
 * @{
 */
#define PAL_FILTER_MODE_NEAREST 0
#define PAL_FILTER_MODE_LINEAR 1
#define PAL_FILTER_MODE_COUNT 2
/** @} */

/**
 * @defgroup sampler_mipmap_modes Sampler Mipmap Modes
 * @brief Sampler mipmap modes
 * 
 * @{
 */
#define PAL_SAMPLER_MIPMAP_MODE_NEAREST 0
#define PAL_SAMPLER_MIPMAP_MODE_LINEAR 1
#define PAL_SAMPLER_MIPMAP_MODE_COUNT 2
/** @} */

/**
 * @defgroup sampler_address_modes Sampler Address Modes
 * @brief Sampler address modes
 * 
 * @{
 */
#define PAL_SAMPLER_ADDRESS_MODE_REPEAT 0
#define PAL_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT 1
#define PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE 2
#define PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER 3
#define PAL_SAMPLER_ADDRESS_MODE_COUNT 4
/** @} */

/**
 * @defgroup border_color Border Colors
 * @brief Border colors
 * 
 * @{
 */
#define PAL_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK 0
#define PAL_BORDER_COLOR_INT_TRANSPARENT_BLACK 1
#define PAL_BORDER_COLOR_FLOAT_OPAQUE_BLACK 2
#define PAL_BORDER_COLOR_INT_OPAQUE_BLACK 3
#define PAL_BORDER_COLOR_FLOAT_OPAQUE_WHITE 4
#define PAL_BORDER_COLOR_INT_OPAQUE_WHITE 5
#define PAL_BORDER_COLOR_COUNT 6
/** @} */

/**
 * @defgroup surface_formats Surface Formats
 * @brief Surface formats
 * 
 * @{
 */
#define PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR 0
#define PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR 1
#define PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR 2
#define PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10 3
#define PAL_SURFACE_FORMAT_COUNT 4
/** @} */

/**
 * @defgroup window_instance_types Window Instance Types
 * @brief Window instance types
 * 
 * @{
 */
#define PAL_WINDOW_INSTANCE_TYPE_WAYLAND 0
#define PAL_WINDOW_INSTANCE_TYPE_X11 1
#define PAL_WINDOW_INSTANCE_TYPE_XCB 2
#define PAL_WINDOW_INSTANCE_TYPE_WIN32 3
#define PAL_WINDOW_INSTANCE_TYPE_COUNT 4
/** @} */

/**
 * @defgroup shader_stages Shader Stages
 * @brief Shader stages
 * 
 * @{
 */
#define PAL_SHADER_STAGE_UNDEFINED 0
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
#define PAL_SHADER_STAGE_COUNT 15
/** @} */

/**
 * @defgroup sample_counts Sample Counts
 * @brief Sample counts
 * 
 * @{
 */
#define PAL_SAMPLE_COUNT_1 0
#define PAL_SAMPLE_COUNT_2 1
#define PAL_SAMPLE_COUNT_4 2
#define PAL_SAMPLE_COUNT_8 3
#define PAL_SAMPLE_COUNT_16 4
#define PAL_SAMPLE_COUNT_32 5
#define PAL_SAMPLE_COUNT_64 6
#define PAL_SAMPLE_COUNT_COUNT 7
/** @} */

/**
 * @defgroup primitive_topologies Primitive Topologies
 * @brief Primitive topologies
 * 
 * @{
 */
#define PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST 0
#define PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP 1
#define PAL_PRIMITIVE_TOPOLOGY_LINE_LIST 2
#define PAL_PRIMITIVE_TOPOLOGY_LINE_STRIP 3
#define PAL_PRIMITIVE_TOPOLOGY_POINT_LIST 4
#define PAL_PRIMITIVE_TOPOLOGY_PATCH 5
#define PAL_PRIMITIVE_TOPOLOGY_COUNT 6
/** @} */

/**
 * @defgroup cull_modes Cull Modes
 * @brief Cull modes
 * 
 * @{
 */
#define PAL_CULL_MODE_NONE 0
#define PAL_CULL_MODE_FRONT 1
#define PAL_CULL_MODE_BACK 2
#define PAL_CULL_MODE_COUNT 3
/** @} */

/**
 * @defgroup front_faces Front Faces
 * @brief Front faces
 * 
 * @{
 */
#define PAL_FRONT_FACE_CLOCKWISE 0
#define PAL_FRONT_FACE_COUNTER_CLOCKWISE 1
#define PAL_FRONT_FACE_COUNT 2
/** @} */

/**
 * @defgroup polygon_modes Polygon Modes
 * @brief Polygon modes
 * 
 * @{
 */
#define PAL_POLYGON_MODE_FILL 0
#define PAL_POLYGON_MODE_LINE 1
#define PAL_POLYGON_MODE_COUNT 2
/** @} */

/**
 * @defgroup stencil_face_flags Stencil Face Flags
 * @brief Stencil face flags
 * 
 * @{
 */
#define PAL_STENCIL_FACE_FLAG_FRONT (1U << 0)
#define PAL_STENCIL_FACE_FLAG_BACK (1U << 1)
#define PAL_STENCIL_FACE_FLAG_BOTH (PAL_STENCIL_FACE_FLAG_FRONT | \
    PAL_STENCIL_FACE_FLAG_BACK)
/** @} */

/**
 * @defgroup vertex_types Vertex Types
 * @brief Vertex types
 * 
 * @{
 */
#define PAL_VERTEX_TYPE_UNDEFINED 0
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
#define PAL_VERTEX_TYPE_COUNT 31
/** @} */

/**
 * @defgroup vertex_semantic_types Vertex Semantic IDs
 * @brief Vertex semantic ids
 * 
 * @{
 */
#define PAL_VERTEX_SEMANTIC_ID_POSITION 0
#define PAL_VERTEX_SEMANTIC_ID_COLOR 1
#define PAL_VERTEX_SEMANTIC_ID_TEXCOORD 2
#define PAL_VERTEX_SEMANTIC_ID_NORMAL 3
#define PAL_VERTEX_SEMANTIC_ID_TANGENT 4
#define PAL_VERTEX_SEMANTIC_ID_COUNT 5
/** @} */

/**
 * @defgroup command_buffer_types Command Buffer Types
 * @brief Command buffer types
 * 
 * @{
 */
#define PAL_COMMAND_BUFFER_TYPE_PRIMARY 0
#define PAL_COMMAND_BUFFER_TYPE_SECONDARY 1
#define PAL_COMMAND_BUFFER_TYPE_COUNT 2
/** @} */

/**
 * @defgroup vertex_layout_types Vertex Layout Types
 * @brief Vertex layout types
 * 
 * @{
 */
#define PAL_VERTEX_LAYOUT_TYPE_PER_VERTEX 0
#define PAL_VERTEX_LAYOUT_TYPE_PER_INSTANCE 1
#define PAL_VERTEX_LAYOUT_TYPE_COUNT 2
/** @} */

/**
 * @defgroup compare_operations Compare Operations
 * @brief Compare operations
 * 
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
#define PAL_COMPARE_OP_COUNT 8
/** @} */

/**
 * @defgroup stencil_operations Stencil Operations
 * @brief Stencil operations
 * 
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
#define PAL_STENCIL_OP_COUNT 8
/** @} */

/**
 * @defgroup blend_operations Blend Operations
 * @brief Blend operations
 * 
 * @{
 */
#define PAL_BLEND_OP_ADD 0
#define PAL_BLEND_OP_SUBTRACT 1
#define PAL_BLEND_OP_REVERSE_SUBTRACT 2
#define PAL_BLEND_OP_MIN 3
#define PAL_BLEND_OP_MAX 4
#define PAL_BLEND_OP_COUNT 5
/** @} */

/**
 * @defgroup blend_factors Blend Factors
 * @brief Blend factors
 * 
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
#define PAL_BLEND_FACTOR_COUNT 14
/** @} */

/**
 * @defgroup color_masks Color Masks
 * @brief Color masks
 * 
 * @{
 */
#define PAL_COLOR_MASK_NONE 0
#define PAL_COLOR_MASK_RED (1U << 0)
#define PAL_COLOR_MASK_GREEN (1U << 1)
#define PAL_COLOR_MASK_BLUE (1U << 2)
#define PAL_COLOR_MASK_ALPHA (1U << 3)
/** @} */

/**
 * @defgroup resolve_modes Resolve Modes
 * @brief Resolve modes
 * 
 * @{
 */
#define PAL_RESOLVE_MODE_NONE 0
#define PAL_RESOLVE_MODE_SAMPLE_ZERO 1
#define PAL_RESOLVE_MODE_AVERAGE 2
#define PAL_RESOLVE_MODE_MIN 3
#define PAL_RESOLVE_MODE_MAX 4
#define PAL_RESOLVE_MODE_COUNT 5
/** @} */

/**
 * @defgroup fragment_shading_rates Fragment Shading Rates
 * @brief Fragment shading rates
 * 
 * @{
 */
#define PAL_FRAGMENT_SHADING_RATE_1X1 0
#define PAL_FRAGMENT_SHADING_RATE_1X2 1
#define PAL_FRAGMENT_SHADING_RATE_2X1 2
#define PAL_FRAGMENT_SHADING_RATE_2X2 3
#define PAL_FRAGMENT_SHADING_RATE_2X4 4
#define PAL_FRAGMENT_SHADING_RATE_4X2 5
#define PAL_FRAGMENT_SHADING_RATE_4X4 6
#define PAL_FRAGMENT_SHADING_RATE_COUNT 7
/** @} */

/**
 * @defgroup fsr_combiner_operations Fragment Shading Rate Combiner Operations
 * @brief Fragment shading rate combiner operations
 * 
 * @{
 */
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP 0
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE 1
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN 2
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX 3
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL 4
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_COUNT 5
/** @} */

/**
 * @defgroup acceleration_structure_types Acceleration Structure Types
 * @brief Acceleration structure types
 * 
 * @{
 */
#define PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL 0
#define PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL 1
#define PAL_ACCELERATION_STRUCTURE_TYPE_COUNT 2
/** @} */

/**
 * @defgroup as_build_modes Acceleration Structure Build Modes
 * @brief Acceleration structure build modes
 * 
 * @{
 */
#define PAL_ACCELERATION_STRUCTURE_BUILD_MODE_BUILD 0
#define PAL_ACCELERATION_STRUCTURE_BUILD_MODE_UPDATE 1
#define PAL_ACCELERATION_STRUCTURE_BUILD_MODE_COUNT 2
/** @} */

/**
 * @defgroup as_build_hints Acceleration Structure Build Hints
 * @brief Acceleration structure build hints
 * 
 * @{
 */
#define PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_BUILD (1U << 0)
#define PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_TRACE (1U << 1)
#define PAL_ACCELERATION_STRUCTURE_BUILD_HINT_LOW_MEMORY (1U << 2)
/** @} */

/**
 * @defgroup as_instance_flags Acceleration Structure Instance Flags
 * @brief Acceleration structure instance flags
 * 
 * @{
 */
#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_OPAQUE (1U << 0)
#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_NO_OPAQUE (1U << 1)
#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FACING_CULL_DISABLE (1U << 2)
#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE (1U << 3)
/** @} */

/**
 * @defgroup geometry_types Geometry Types
 * @brief Geometry types
 * 
 * @{
 */
#define PAL_GEOMETRY_TYPE_TRIANGLE 0
#define PAL_GEOMETRY_TYPE_AABBS 1
#define PAL_GEOMETRY_TYPE_COUNT 2
/** @} */

/**
 * @defgroup geometry_flags Geometry Flags
 * @brief Geometry flags
 * 
 * @{
 */
#define PAL_GEOMETRY_FLAG_OPAQUE (1U << 0)
#define PAL_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT (1U << 1)
/** @} */

/**
 * @defgroup index_types Index Types
 * @brief Index types
 * 
 * @{
 */
#define PAL_INDEX_TYPE_UINT16 0
#define PAL_INDEX_TYPE_UINT32 1
#define PAL_INDEX_TYPE_COUNT 2
/** @} */

/**
 * @defgroup buffer_usages Buffer Usages
 * @brief Buffer usages
 * 
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
 * @defgroup debug_message_severities Debug Message Severities
 * @brief Debug message severities
 * 
 * @{
 */
#define PAL_DEBUG_MESSAGE_SEVERITY_INFO 0
#define PAL_DEBUG_MESSAGE_SEVERITY_WARNING 1
#define PAL_DEBUG_MESSAGE_SEVERITY_ERROR 2
#define PAL_DEBUG_MESSAGE_SEVERITY_COUNT 3
/** @} */

/**
 * @defgroup debug_message_types Debug Message Types
 * @brief Debug message types
 * 
 * @{
 */
#define PAL_DEBUG_MESSAGE_TYPE_GENERAL 0
#define PAL_DEBUG_MESSAGE_TYPE_VALIDATION 1
#define PAL_DEBUG_MESSAGE_TYPE_PERFORMANCE 2
#define PAL_DEBUG_MESSAGE_TYPE_COUNT 3
/** @} */

/**
 * @defgroup usage_states Usage States
 * @brief Usage states
 * 
 * @{
 */
#define PAL_USAGE_STATE_UNDEFINED 0
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
#define PAL_USAGE_STATE_COUNT 22
/** @} */

/**
 * @defgroup descriptor_types Descriptor Types
 * @brief Descriptor types
 * 
 * @{
 */
#define PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER 0
#define PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER 1
#define PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE 2
#define PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE 3
#define PAL_DESCRIPTOR_TYPE_SAMPLER 4
#define PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE 5
#define PAL_DESCRIPTOR_TYPE_COUNT 6
/** @} */

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
 * @defgroup ray_tracing_shader_group_types Ray Tracing Shader Group Types
 * @brief Ray tracing shader group types
 * 
 * @{
 */
#define PAL_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL 0
#define PAL_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT 1
#define PAL_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT 2
#define PAL_RAY_TRACING_SHADER_GROUP_TYPE_COUNT 3
/** @} */

/**
 * @defgroup buffer_memory_usages Buffer Memory Usages
 * @brief Buffer memory usages
 * 
 * @{
 */
#define PAL_BUFFER_MEMORY_USAGE_MANUAL 0
#define PAL_BUFFER_MEMORY_USAGE_AUTO_GPU_ONLY 1
#define PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_UPLOAD 2
#define PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_READBACK 3
#define PAL_BUFFER_MEMORY_USAGE_COUNT 4
/** @} */

/**
 * @defgroup image_memory_usages Image Memory Usages
 * @brief Image memory usages
 * 
 * @{
 */
#define PAL_IMAGE_MEMORY_USAGE_MANUAL 0
#define PAL_IMAGE_MEMORY_USAGE_AUTO_GPU_ONLY 1
#define PAL_IMAGE_MEMORY_USAGE_COUNT 2
/** @} */

/**
 * @defgroup rendering_flags Rendering Flags
 * @brief Rendering flags
 * 
 * @{
 */
#define PAL_RENDERING_FLAG_NONE 0
#define PAL_RENDERING_FLAG_SUSPENDING (1U << 0)
#define PAL_RENDERING_FLAG_RESUMING (1U << 1)
/** @} */

/**
 * @defgroup pipeline_stages Pipeline Stages
 * @brief Pipeline stages
 * 
 * @{
 */
#define PAL_PIPELINE_STAGE_NONE 0
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
 * @struct PalMemory
 * @brief Opaque handle to a GPU device memory.
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
 * This is the same handle used for all pipeline types
 * (Graphics, Compute and Ray tracing).
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
 * @brief Debugger messages severity types used to filter incoming messages.
 * 
 * All values of this type follow the format `PAL_DEBUG_MESSAGE_SEVERITY_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalDebugMessageSeverity;

/**
 * @typedef PalDebugMessageType
 * @brief Debugger messages types used to filter incoming messages.
 * 
 * All values of this type follow the format `PAL_DEBUG_MESSAGE_TYPE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalDebugMessageType;

/**
 * @typedef PalDeviceAddress
 * @brief The device address of a buffer.
 *
 * @since Added in version 2.0
 */
typedef uint64_t PalDeviceAddress;

/**
 * @typedef PalQueueType
 * @brief Queue types.
 * 
 * All values of this type follow the format `PAL_QUEUE_TYPE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalQueueType;

/**
 * @typedef PalPresentMode
 * @brief Present modes
 * 
 * All values of this type follow the format `PAL_PRESENT_MODE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalPresentMode;

/**
 * @typedef PalCompositeAplha
 * @brief Composite alphas
 * 
 * All values of this type follow the format `PAL_COMPOSITE_ALPHA_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalCompositeAplha;

/**
 * @typedef PalFormat
 * @brief Format types.
 * 
 * All values of this type follow the format `PAL_FORMAT_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalFormat;

/**
 * @typedef PalImageUsages
 * @brief Image usages. 
 * 
 * Multiple image usages can be OR'ed together using bitwise OR operator (`|`).
 * 
 * All values of this type follow the format `PAL_IMAGE_USAGE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalImageUsages;

/**
 * @typedef PalLoadOp
 * @brief Load operations.
 * 
 * All values of this type follow the format `PAL_LOAD_OP_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalLoadOp;

/**
 * @typedef PalStoreOp
 * @brief Store operations.
 * 
 * All values of this type follow the format `PAL_STORE_OP_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalStoreOp;

/**
 * @typedef PalMemoryType
 * @brief Memory types.
 * 
 * All values of this type follow the format `PAL_MEMORY_TYPE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalMemoryType;

/**
 * @typedef PalImageType
 * @brief Image types.
 * 
 * All values of this type follow the format `PAL_IMAGE_TYPE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalImageType;

/**
 * @typedef PalImageAspect
 * @brief Image aspects.
 * 
 * All values of this type follow the format `PAL_IMAGE_ASPECT_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalImageAspect;

/**
 * @typedef PalImageViewType
 * @brief Image view types.
 * 
 * All values of this type follow the format `PAL_IMAGE_VIEW_TYPE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalImageViewType;

/**
 * @typedef PalFilterMode
 * @brief Filter modes.
 * 
 * All values of this type follow the format `PAL_FILTER_MODE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalFilterMode;

/**
 * @typedef PalSamplerMipmapMode
 * @brief Sampler mipmap modes.
 * 
 * All values of this type follow the format `PAL_SAMPLER_MIPMAP_MODE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalSamplerMipmapMode;

/**
 * @typedef PalSamplerAddressMode
 * @brief Sampler address modes.
 * 
 * All values of this type follow the format `PAL_SAMPLER_ADDRESS_MODE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalSamplerAddressMode;

/**
 * @typedef PalBorderColor
 * @brief Border colors.
 * 
 * All values of this type follow the format `PAL_BORDER_COLOR_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalBorderColor;

/**
 * @typedef PalSurfaceFormat
 * @brief Surface format types.
 * 
 * All values of this type follow the format `PAL_SURFACE_FORMAT_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalSurfaceFormat;

/**
 * @typedef WindowInstanceType
 * @brief Instance types of a window instance.
 * 
 * All values of this type follow the format `PAL_WINDOW_INSTANCE_TYPE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalWindowInstanceType;

/**
 * @typedef PalShaderStage
 * @brief shader stage types.
 * 
 * All values of this type follow the format `PAL_SHADER_STAGE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalShaderStage;

/**
 * @typedef PalSampleCount
 * @brief sample count.
 * 
 * All values of this type follow the format `PAL_SAMPLE_COUNT_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalSampleCount;

/**
 * @typedef PalPrimitiveTopology
 * @brief Primitve topology types.
 * 
 * All values of this type follow the format `PAL_PRIMITIVE_TOPOLOGY_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalPrimitiveTopology;

/**
 * @typedef PalCullMode
 * @brief Cull modes.
 * 
 * All values of this type follow the format `PAL_CULL_MODE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalCullMode;

/**
 * @typedef PalFrontFace
 * @brief Front face modes.
 * 
 * All values of this type follow the format `PAL_FRONT_FACE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalFrontFace;

/**
 * @typedef PalPolygonMode
 * @brief Polygon modes.
 * 
 * All values of this type follow the format `PAL_POLYGON_MODE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalPolygonMode;

/**
 * @typedef PalStencilFaceFlags
 * @brief Stencil face flags. 
 * 
 * Multiple stencil face flags can be OR'ed together using bitwise 
 * OR operator (`|`).
 * 
 * All values of this type follow the format `PAL_STENCIL_FACE_FLAG_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalStencilFaceFlags;

/**
 * @typedef PalVertexType
 * @brief Vertex attribute types.
 * 
 * All values of this type follow the format `PAL_VERTEX_TYPE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalVertexType;

/**
 * @typedef PalVertexSemanticID
 * @brief Vertex semantic id types.
 * 
 * All values of this type follow the format `PAL_VERTEX_SEMANTIC_ID_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalVertexSemanticID;

/**
 * @typedef PalCommandBufferType
 * @brief Command buffer types.
 * 
 * All values of this type follow the format `PAL_COMMAND_BUFFER_TYPE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalCommandBufferType;

/**
 * @typedef PalVertexLayoutType
 * @brief Vertex layout types.
 * 
 * All values of this type follow the format `PAL_VERTEX_LAYOUT_TYPE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalVertexLayoutType;

/**
 * @typedef PalCompareOp
 * @brief Compare operation modes.
 * 
 * All values of this type follow the format `PAL_COMPARE_OP_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalCompareOp;

/**
 * @typedef PalStencilOp
 * @brief Stencil operation modes.
 * 
 * All values of this type follow the format `PAL_STENCIL_OP_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalStencilOp;

/**
 * @typedef PalBlendOp
 * @brief Blend operation modes.
 * 
 * All values of this type follow the format `PAL_BLEND_OP_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalBlendOp;

/**
 * @typedef PalBlendFactor
 * @brief Blend factor modes.
 * 
 * All values of this type follow the format `PAL_BLEND_FACTOR_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalBlendFactor;

/**
 * @typedef PalColorMask
 * @brief Color mask flags. 
 * 
 * Multiple color mask flags can be OR'ed together using bitwise
 * OR operator (`|`).
 * 
 * All values of this type follow the format `PAL_COLOR_MASK_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalColorMask;

/**
 * @typedef PalResolveMode
 * @brief Resolve modes.
 * 
 * All values of this type follow the format `PAL_RESOLVE_MODE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalResolveMode;

/**
 * @typedef PalFragmentShadingRate
 * @brief Fragment shading rates.
 * 
 * All values of this type follow the format `PAL_FRAGMENT_SHADING_RATE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalFragmentShadingRate;

/**
 * @typedef PalFragmentShadingRateCombinerOp
 * @brief Fragment shading rate combiner operaton modes.
 * 
 * All values of this type follow the format 
 * `PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_*` for API consistency and 
 * ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalFragmentShadingRateCombinerOp;

/**
 * @typedef PalAccelerationStructureType
 * @brief Acceleration structure types.
 * 
 * All values of this type follow the format 
 * `PAL_ACCELERATION_STRUCTURE_TYPE_*` for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalAccelerationStructureType;

/**
 * @typedef PalAccelerationStructureBuildMode
 * @brief Acceleration structure build modes.
 * 
 * All values of this type follow the format 
 * `PAL_ACCELERATION_STRUCTURE_BUILD_MODE_*` for API consistency and 
 * ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalAccelerationStructureBuildMode;

/**
 * @typedef PalAccelerationStructureBuildHints
 * @brief Acceleration structure build hints. 
 * 
 * Multiple hints can be OR'ed together using bitwise
 * OR operator (`|`). Hints can be ignored by the driver.
 * 
 * All values of this type follow the format 
 * `PAL_ACCELERATION_STRUCTURE_BUILD_HINT_*` for API consistency and 
 * ease of use.
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
 * All values of this type follow the format 
 * `PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_*` for API consistency and 
 * ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalAccelerationStructureInstanceFlags;

/**
 * @typedef PalGeometryType
 * @brief Geometry types.
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
 * 
 * All values of this type follow the format `PAL_GEOMETRY_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalGeometryFlags;

/**
 * @typedef PalIndexType
 * @brief Index types.
 * 
 * All values of this type follow the format `PAL_INDEX_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalIndexType;

/**
 * @typedef PalUsageState
 * @brief Usage states.
 * 
 * All values of this type follow the format `PAL_USAGE_STATE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalUsageState;

/**
 * @typedef PalDescriptorType
 * @brief Descriptor types.
 * 
 * All values of this type follow the format `PAL_DESCRIPTOR_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalDescriptorType;

/**
 * @typedef PalRayTracingShaderGroupType
 * @brief Ray tracing shader group types.
 * 
 * All values of this type follow the format 
 * `PAL_RAY_TRACING_SHADER_GROUP_TYPE_*` for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalRayTracingShaderGroupType;

/**
 * @typedef PalRenderingFlags
 * @brief Rendering flags.
 * 
 * All values of this type follow the format `PAL_RENDERING_FLAG_*` 
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalRenderingFlags;

/**
 * @typedef PalPipelineStages
 * @brief Pipeline stages. 
 * 
 * Multiple pipeline usages can be OR'ed together using bitwise
 * OR operator (`|`).
 * 
 * All values of this type follow the format `PAL_PIPELINE_STAGE_*` 
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalPipelineStages;

/**
 * @typedef PalDebugCallback
 * @brief Function pointer type used for debug callbacks.
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
 * @param userData User data passed from the debugger. Can be `nullptr`.
 * @param severity Severity of the message.
 * @param type Type of the message.
 * @param msg Null-terminated `UTF-8` debug message.
 *
 * @since Added in version 2.0
 * @sa palInitGraphics
 */
typedef void(PAL_CALL* PalDebugCallback)(
    void* userData,
    PalDebugMessageSeverity severity,
    PalDebugMessageType type,
    const char* msg);

/**
 * @struct PalFormatInfo
 * @brief Contains information about a format. 
 *
 * @since Added in version 2.0
 * 
 * @var PalFormatInfo::usages
 * A bitmask of supported image usages of ::format.
 * 
 * @var PalFormatInfo::format
 * The format.
 * 
 * @var PalFormatInfo::sampleCount
 * The `MSAA` samples of ::format.
 */
typedef struct PalFormatInfo
{
    PalImageUsages usages;
    PalFormat format;
    PalSampleCount sampleCount;
} PalFormatInfo;

/**
 * @struct PalImageInfo
 * @brief Contains information about an image.
 *
 * @since Added in version 2.0
 * 
 * @var PalImageInfo::usages
 * A bitmask of image usages.
 * 
 * @var PalImageInfo::width
 * The width of the image in pixels.
 * 
 * @var PalImageInfo::height
 * The height of the image in pixels.
 * 
 * @var PalImageInfo::depth
 * The depth of the image in pixels.
 * 
 * @var PalImageInfo::arrayLayerCount
 * The number of array layers of the image.
 * 
 * @var PalImageInfo::mipLevelCount
 * The number of mipmap levels of the image.
 * 
 * @var PalImageInfo::sampleCount
 * The `MSAA` samples of the image.
 * 
 * @var PalImageInfo::type
 * The type of the image (eg. `PAL_IMAGE_TYPE_2D`).
 * 
 * @var PalImageInfo::format
 * The format of the image (eg. `PAL_FORMAT_R8G8B8A8_UNORM`).
 * 
 * @var PalImageInfo::belongsToSwapchain
 * If `PAL_TRUE`, the image belongs to a swapchain.
 */
typedef struct PalImageInfo
{
    PalImageUsages usages;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t arrayLayerCount;
    uint32_t mipLevelCount; 
    PalSampleCount sampleCount;
    PalImageType type;
    PalFormat format; 
    PalBool belongsToSwapchain;
} PalImageInfo;

/**
 * @struct PalClearValue
 * @brief Contains information about clear values used with rendering.
 *
 * If used with a color attachment, ::color values will be used and 
 * ::depth and ::stencil will be used with depth stencil attachments.
 *
 * @since Added in version 2.0
 * 
 * @var PalClearValue::color
 * The color clear value.
 * 
 * @var PalClearValue::depth
 * The depth clear value.
 * 
 * @var PalClearValue::stencil
 * The stencil clear value.
 */
typedef struct PalClearValue
{
    float color[4];
    float depth;
    uint32_t stencil;
} PalClearValue;

/**
 * @struct PalAttachmentDesc
 * @brief Contains description of an attachment.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalAttachmentDesc::imageView
 * The image view the attachment description is tied to.
 * 
 * @var PalAttachmentDesc::resolveImageView
 * The resolve image view. Can be `nullptr` if ::imageView does not
 * need to be resolved.
 * 
 * @var PalAttachmentDesc::loadOp
 * The load operation for color or depth attachments.
 * 
 * @var PalAttachmentDesc::storeOp
 * The store operation for color or depth attachments.
 * 
 * @var PalAttachmentDesc::stencilLoadOp
 * The load operation for stencil attachments.
 * 
 * @var PalAttachmentDesc::stencilStoreOp
 * The store operation for stencil attachments.
 * 
 * @var PalAttachmentDesc::resolveMode
 * The color or depth resolve mode. 
 * Will be ignored if ::resolveImageView is `nullptr`.
 * 
 * @var PalAttachmentDesc::stencilResolveMode
 * The stencil resolve mode. 
 * Will be ignored if ::resolveImageView is `nullptr`.
 * 
 * @var PalAttachmentDesc::clearValue
 * The clear value for the attachment. The values will be used based
 * on the type of the attachment.
 */
typedef struct PalAttachmentDesc
{
    PalImageView* imageView;
    PalImageView* resolveImageView;
    PalLoadOp loadOp;
    PalStoreOp storeOp;
    PalLoadOp stencilLoadOp;
    PalStoreOp stencilStoreOp;
    PalResolveMode resolveMode;
    PalResolveMode stencilResolveMode;
    PalClearValue clearValue;
} PalAttachmentDesc;

/**
 * @struct PalViewport
 * @brief Contains information about a viewport.
 * 
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalViewport::x
 * The x position of the viewport.
 * 
 * @var PalViewport::y
 * The y position of the viewport.
 * 
 * @var PalViewport::width
 * The width of the viewport in pixels.
 * 
 * @var PalViewport::height
 * The height of the viewport in pixels.
 * 
 * @var PalViewport::minDepth
 * The minimum depth of the viewport.
 * 
 * @var PalViewport::maxDepth
 * The maximum depth of the viewport.
 */
typedef struct PalViewport
{
    float x;
    float y;
    float width;
    float height;
    float minDepth;
    float maxDepth;
} PalViewport;

/**
 * @struct PalRect2D
 * @brief Contains information of a 2D rectangle.
 * 
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalRect2D::x
 * The x position of the rect.
 * 
 * @var PalRect2D::y
 * The y position of the rect.
 * 
 * @var PalRect2D::width
 * The width of the rect in pixels.
 * 
 * @var PalRect2D::height
 * The height of the rect in pixels.
 */
typedef struct PalRect2D
{
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
} PalRect2D;

/**
 * @struct PalMemoryRequirements
 * @brief Memory requirements for a resource (image, buffer etc).
 * 
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalMemoryRequirements::size
 * The required size in bytes of the resource.
 * 
 * @var PalMemoryRequirements::alignment
 * The required alignment in bytes of the resource.
 * This is used for sub allocations.
 * 
 * @var PalMemoryRequirements::memoryMask
 * The memory mask for the driver for the allocation. Must not be changed.
 * 
 * @var PalMemoryRequirements::supportedMemoryTypes
 * A bitmask of supported memory types. A specific memory type
 * should be check like this: @nl
 * palIsSupported(::supportedMemoryTypes, `PAL_MEMORY_TYPE_GPU_ONLY`).
 * 
 * @var PalMemoryRequirements::reserved
 * Not used. Set to `0`.
 */
typedef struct PalMemoryRequirements
{
    uint64_t size;
    uint64_t alignment;
    uint64_t memoryMask;
    uint32_t supportedMemoryTypes;
    uint32_t reserved;
} PalMemoryRequirements;

/**
 * @struct PalCommandBufferSubmitInfo
 * @brief Contains information about submitting a command buffer.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalCommandBufferSubmitInfo::waitValue
 * The wait value for timeline ::waitSemaphore.
 * 
 * @var PalCommandBufferSubmitInfo::signalValue
 * The signal value for timeline ::signalSemaphore.
 * 
 * @var PalCommandBufferSubmitInfo::cmdBuffer
 * The command buffer to submit.
 * 
 * @var PalCommandBufferSubmitInfo::waitSemaphore
 * The wait semaphore. If timeline, ::waitValue will be used.
 * 
 * @var PalCommandBufferSubmitInfo::signalSemaphore
 * The signal semaphore. If timeline, ::signalValue will be used.
 * 
 * @var PalCommandBufferSubmitInfo::fence
 * The fence to signal.
 * 
 * @var PalCommandBufferSubmitInfo::waitStages
 * The wait stages for the ::waitSemaphore.
 * 
 * @var PalCommandBufferSubmitInfo::signalStages
 * The signal stages for the ::signalSemaphore.
 */
typedef struct PalCommandBufferSubmitInfo
{
    uint64_t waitValue; 
    uint64_t signalValue; 
    PalCommandBuffer* cmdBuffer;
    PalSemaphore* waitSemaphore;
    PalSemaphore* signalSemaphore;
    PalFence* fence;
    PalPipelineStages waitStages;
    PalPipelineStages signalStages;
} PalCommandBufferSubmitInfo;

/**
 * @struct PalSwapchainNextImageInfo
 * @brief Contains information about getting the next swapchain image.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalSwapchainNextImageInfo::timeout
 * The timeout in milliseconds to wait.
 * 
 * @var PalSwapchainNextImageInfo::signalSemaphore
 * The semaphore to signal when the image is acquired.
 * 
 * @var PalSwapchainNextImageInfo::fence
 * The fence to signal when the image is acquired.
 */
typedef struct PalSwapchainNextImageInfo
{
    uint64_t timeout;
    PalSemaphore* signalSemaphore;
    PalFence* fence;
} PalSwapchainNextImageInfo;

/**
 * @struct PalRenderingInfo
 * @brief Contains information about how rendering should be done.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalRenderingInfo::colorAttachments
 * The color attachments.
 * 
 * @var PalRenderingInfo::depthStencilAttachment
 * The depth stencil attachment.
 * 
 * @var PalRenderingInfo::fragmentShadingRateImageView
 * The fragment shading rate image view.
 * 
 * @var PalRenderingInfo::renderArea
 * The rendering area. All attachments image views size must be less or 
 * equal to the area.
 * 
 * @var PalRenderingInfo::flags
 * The rendering flags (eg. `PAL_RENDERING_FLAG_NONE`).
 * 
 * @var PalRenderingInfo::fragmentShadingRateTexelWidth
 * The fragment shading rate texel width. Will be ignored if 
 * ::fragmentShadingRateImageView is `nullptr`.
 * 
 * @var PalRenderingInfo::fragmentShadingRateTexelHeight
 * The fragment shading rate texel height. Will be ignored if 
 * ::fragmentShadingRateImageView is `nullptr`.
 * 
 * @var PalRenderingInfo::viewCount
 * The number of views. The default is `1`.
 * 
 * @var PalRenderingInfo::arrayLayerCount
 * The number of array layers.
 * 
 * @var PalRenderingInfo::colorAttachentCount
 * The number of color attachments.
 */
typedef struct PalRenderingInfo
{
    PalAttachmentDesc* colorAttachments;
    PalAttachmentDesc* depthStencilAttachment;
    PalImageView* fragmentShadingRateImageView;
    PalRect2D renderArea;
    PalRenderingFlags flags;
    uint32_t fragmentShadingRateTexelWidth;
    uint32_t fragmentShadingRateTexelHeight;
    uint32_t viewCount;
    uint32_t arrayLayerCount;
    uint32_t colorAttachentCount;
} PalRenderingInfo;

/**
 * @struct PalRenderingLayoutInfo
 * @brief Contains information about a pre-existing PalRenderingInfo.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalRenderingLayoutInfo::colorAttachmentsFormat
 * The color attachmenets formats.
 * 
 * @var PalRenderingLayoutInfo::colorAttachmentsFormat
 * The number of color attachments formats.
 * 
 * @var PalRenderingLayoutInfo::viewCount
 * The number of views. The default is `1`.
 * 
 * @var PalRenderingLayoutInfo::sampleCount
 * The `MSAA` samples.
 * 
 * @var PalRenderingLayoutInfo::flags
 * The rendering flags (eg. `PAL_RENDERING_FLAG_NONE`).
 * 
 * @var PalRenderingLayoutInfo::depthStencilAttachmentFormat
 * The depth stencil attachment format.
 * 
 * @var PalRenderingLayoutInfo::fragmentShadingRateAttachmentFormat
 * The fragment shading rate attachment format.
 */
typedef struct PalRenderingLayoutInfo
{
    PalFormat* colorAttachmentsFormat;
    uint32_t colorAttachentCount;
    uint32_t viewCount;
    PalSampleCount sampleCount;
    PalRenderingFlags flags;
    PalFormat depthStencilAttachmentFormat;
    PalFormat fragmentShadingRateAttachmentFormat;
} PalRenderingLayoutInfo;

/**
 * @struct PalWorkGroupBuildData
 * @brief Contains build information about a workgroup.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalWorkGroupBuildData::workCount
 * The workload per dimension.
 * 
 * @var PalWorkGroupBuildData::workGroupSize
 * The maximum workgroup size per dimension of the adapter.
 * 
 * @var PalWorkGroupBuildData::workGroupCount
 * The maximum number of workgroups per dimension of the adapter.
 */
typedef struct PalWorkGroupBuildData
{
    uint32_t workCount[3];
    uint32_t workGroupSize[3];
    uint32_t workGroupCount[3];
} PalWorkGroupBuildData;

/**
 * @struct PalWorkGroupInfo
 * @brief Contains information about a dispatch tile.
 * 
 * @since Added in version 2.0
 * 
 * @var PalWorkGroupInfo::workGroupBase
 * The offsets per dimension of the dispatch tile.
 * 
 * @var PalWorkGroupInfo::workGroupBase
 * The number of workgroups per dimension of the dispatch tile.
 */
typedef struct PalWorkGroupInfo
{
    uint32_t workGroupBase[3];
    uint32_t workGroupCount[3];
} PalWorkGroupInfo;

/**
 * @struct PalImageStagingRequirements
 * @brief Contains requirements for an image staging buffer.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalImageStagingRequirements::bufferSize
 * The required buffer size.
 * 
 * @var PalImageStagingRequirements::bufferRowLength
 * The required buffer row length.
 * 
 * @var PalImageStagingRequirements::bufferImageHeight
 * The required buffer image height.
 */
typedef struct PalImageStagingRequirements
{
    uint64_t bufferSize; 
    uint32_t bufferRowLength;
    uint32_t bufferImageHeight;
} PalImageStagingRequirements;

/**
 * @struct PalDrawIndirectData
 * @brief Contains indirect data of a draw call.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDrawIndirectData::vertexCount
 * The number of vertices.
 * 
 * @var PalDrawIndirectData::instanceCount
 * The number of instances.
 * 
 * @var PalDrawIndirectData::firstVertex
 * The first vertex.
 * 
 * @var PalDrawIndirectData::firstInstance
 * The first instance.
 */
typedef struct PalDrawIndirectData
{
    uint32_t vertexCount;
    uint32_t instanceCount;
    uint32_t firstVertex;
    uint32_t firstInstance;
} PalDrawIndirectData;

/**
 * @struct PalDrawIndexedIndirectData
 * @brief Contains indirect data of a draw indexed call.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDrawIndexedIndirectData::indexCount
 * The number of indices.
 * 
 * @var PalDrawIndexedIndirectData::instanceCount
 * The number of instances.
 * 
 * @var PalDrawIndexedIndirectData::firstIndex
 * The first index.
 * 
 * @var PalDrawIndexedIndirectData::vertexOffset
 * The vertex offset.
 * 
 * @var PalDrawIndexedIndirectData::firstInstance
 * The first instance.
 */
typedef struct PalDrawIndexedIndirectData
{
    uint32_t indexCount; 
    uint32_t instanceCount;
    uint32_t firstIndex; 
    int32_t vertexOffset;
    uint32_t firstInstance;
} PalDrawIndexedIndirectData;

/**
 * @struct PalDispatchIndirectData
 * @brief Contains indirect data of a dispatch call.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDispatchIndirectData::groupCountXOrWidth
 * The number of groups on the x dimension or the dispatch width.
 * 
 * @var PalDispatchIndirectData::groupCountXOrHeight
 * The number of groups on the y dimension or the dispatch height.
 * 
 * @var PalDispatchIndirectData::groupCountXOrDepth
 * The number of groups on the z dimension or the dispatch depth.
 */
typedef struct PalDispatchIndirectData
{
    uint32_t groupCountXOrWidth; 
    uint32_t groupCountXOrHeight;
    uint32_t groupCountXOrDepth; 
} PalDispatchIndirectData;

/**
 * @struct PalVertexAttribute
 * @brief Contains information about a vertex attribute.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalVertexAttribute::semanticID
 * The semantic id of the vertex (eg. `PAL_VERTEX_SEMANTIC_ID_POSITION`).
 * 
 * @var PalVertexAttribute::type
 * The type of the vertex (eg. `PAL_VERTEX_TYPE_FLOAT3`).
 */
typedef struct PalVertexAttribute
{
    PalVertexSemanticID semanticID;
    PalVertexType type;
} PalVertexAttribute;

/**
 * @struct PalVertexLayout
 * @brief Contains information about a vertex layout.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalVertexLayout::attributes
 * The vertex attributes of the vertex layout.
 * 
 * @var PalVertexLayout::attributeCount
 * The number of vertex attributes.
 * 
 * @var PalVertexLayout::type
 * The type of the vertex layout (eg. `PAL_VERTEX_LAYOUT_TYPE_PER_VERTEX`).
 * 
 * @var PalVertexLayout::binding
 * The vertex binding slot to associate the vertex layout to.
 * 
 * @var PalVertexLayout::reserved
 * Not used. Set to `0`.
 */
typedef struct PalVertexLayout
{
    PalVertexAttribute* attributes;
    uint32_t attributeCount;
    PalVertexLayoutType type;
    uint32_t binding;
    uint32_t reserved;
} PalVertexLayout;

/**
 * @struct PalGraphicsDebugger
 * @brief Contains information about a graphics debugger.
 *
 * The debugger will not be initialized if 
 * PalGraphicsDebugger::callback is not valid.
 *
 * @since Added in version 2.0
 * 
 * @var PalGraphicsDebugger::userData
 * User data passed to ::callback. Can be `nullptr`.
 * 
 * @var PalGraphicsDebugger::callback
 * The function to forward debug messages to. Must not be `nullptr`.
 * 
 * @var PalGraphicsDebugger::enableGPUValidation
 * `PAL_TRUE` to enable GPU-Based Validation.
 * 
 * @var PalGraphicsDebugger::denyGeneral
 * If `PAL_TRUE`, general type debug messages wil be filtered out.
 * 
 * @var PalGraphicsDebugger::denyValidation
 * If `PAL_TRUE`, validation type debug messages wil be filtered out.
 * 
 * @var PalGraphicsDebugger::denyPerformance
 * If `PAL_TRUE`, performance type debug messages wil be filtered out.
 * 
 * @var PalGraphicsDebugger::denyInfoSeverity
 * If `PAL_TRUE`, information severity debug messages wil be filtered out.
 * 
 * @var PalGraphicsDebugger::denyWarningSeverity
 * If `PAL_TRUE`, warning severity debug messages wil be filtered out.
 * 
 * @var PalGraphicsDebugger::denyErrorSeverity
 * If `PAL_TRUE`, error severity debug messages wil be filtered out.
 * 
 * @var PalGraphicsDebugger::reserved
 * Not used. Set to `0`.
 */
typedef struct PalGraphicsDebugger
{
    void* userData;
    PalDebugCallback callback;
    PalBool enableGPUValidation;
    PalBool denyGeneral;
    PalBool denyValidation;
    PalBool denyPerformance;
    PalBool denyInfoSeverity;
    PalBool denyWarningSeverity;
    PalBool denyErrorSeverity;
    uint32_t reserved;
} PalGraphicsDebugger;

/**
 * @struct PalRasterizerState
 * @brief Contains information about the rasterizer state.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalRasterizerState::enableDepthClamp
 * `PAL_TRUE` to enable depth clamp.
 * 
 * @var PalRasterizerState::enableDepthBias
 * `PAL_TRUE` to enable depth bias.
 * 
 * @var PalRasterizerState::depthBiasConstant
 * The depth bias constant. Will be ignored if ::enableDepthBias 
 * is `PAL_FALSE`.
 * 
 * @var PalRasterizerState::depthBiasSlope
 * The depth bias slope. Will be ignored if ::enableDepthBias is `PAL_FALSE`.
 * 
 * @var PalRasterizerState::depthBiasClamp
 * The depth bias clamp. Will be ignored if ::enableDepthBias is `PAL_FALSE`.
 * 
 * @var PalRasterizerState::polygonMode
 * The polygon mode (eg. `PAL_POLYGON_MODE_FILL`).
 * 
 * @var PalRasterizerState::cullMode
 * The cull mode (eg. `PAL_CULL_MODE_NONE`)
 * 
 * @var PalRasterizerState::frontFace
 * The front face (eg. `PAL_FRONT_FACE_CLOCKWISE`).
 */
typedef struct PalRasterizerState
{
    PalBool enableDepthClamp;
    PalBool enableDepthBias;
    float depthBiasConstant;
    float depthBiasSlope;
    float depthBiasClamp;
    PalPolygonMode polygonMode;
    PalCullMode cullMode;
    PalFrontFace frontFace;
} PalRasterizerState;

/**
 * @struct PalMultisampleState
 * @brief Contains information about the multisample state.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalMultisampleState::sampleMask
 * The sample mask. Set to `0` for all samples.
 * 
 * @var PalMultisampleState::enableSampleShading
 * `PAL_TRUE` to enable sample shading.
 * 
 * @var PalMultisampleState::enableAlphaToCoverage
 * `PAL_TRUE` to enable enable alpha to coverage.
 * 
 * @var PalMultisampleState::sampleCount
 * The `MSAA` samples. (eg. `PAL_SAMPLE_COUNT_4`).
 * 
 * @var PalMultisampleState::sampleCount
 * The minimum sample shading.
 */
typedef struct PalMultisampleState
{
    uint64_t sampleMask;
    PalBool enableSampleShading;
    PalBool enableAlphaToCoverage;
    PalSampleCount sampleCount;
    float minSampleShading;
} PalMultisampleState;

/**
 * @struct PalStencilOpState
 * @brief Contains information about stencil operation state.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalStencilOpState::failOp
 * The stencil fail operation.
 * 
 * @var PalStencilOpState::passOp
 * The pass operation.
 * 
 * @var PalStencilOpState::depthFailOp
 * The depth fail operation.
 * 
 * @var PalStencilOpState::compareOp
 * The compare operation.
 */
typedef struct PalStencilOpState
{
    PalStencilOp failOp;
    PalStencilOp passOp;
    PalStencilOp depthFailOp;
    PalCompareOp compareOp;
} PalStencilOpState;

/**
 * @struct PalDepthStencilState
 * @brief Contains information about the depth stencil state.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDepthStencilState::enableDepthTest
 * `PAL_TRUE` to enable depth test.
 * 
 * @var PalDepthStencilState::enableDepthWrite
 * `PAL_TRUE` to enable depth write.
 * 
 * @var PalDepthStencilState::enableStencilTest
 * `PAL_TRUE` to enable stencil test.
 * 
 * @var PalDepthStencilState::compareOp
 * The compare operation.
 * 
 * @var PalDepthStencilState::frontStencilOpState
 * The front stencil operation state.
 * 
 * @var PalDepthStencilState::backStencilOpState
 * The back stencil operation state.
 */
typedef struct PalDepthStencilState
{
    PalBool enableDepthTest;              
    PalBool enableDepthWrite;             
    PalBool enableStencilTest;            
    PalCompareOp compareOp;               
    PalStencilOpState frontStencilOpState;
    PalStencilOpState backStencilOpState; 
} PalDepthStencilState;

/**
 * @struct PalColorBlendAttachment
 * @brief Contains information about a color blend attachment.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalColorBlendAttachment::enableBlend
 * `PAL_TRUE` to enable blending.
 * 
 * @var PalColorBlendAttachment::colorWriteMask
 * A bitmask of blend color masks 
 * (eg. `PAL_COLOR_MASK_RED` | `PAL_COLOR_MASK_BLUE`).
 * 
 * @var PalColorBlendAttachment::dstColorBlendFactor
 * The destination color blend factor. 
 * 
 * @var PalColorBlendAttachment::srcColorBlendFactor
 * The source color blend factor. 
 * 
 * @var PalColorBlendAttachment::colorBlendOp
 * The color blend operation.
 * 
 * @var PalColorBlendAttachment::dstAlphaBlendFactor
 * The destination alpha blend factor.
 * 
 * @var PalColorBlendAttachment::srcAlphaBlendFactor
 * The source alpha blend factor.
 * 
 * @var PalColorBlendAttachment::alphaBlendOp
 * The alpha blend operation.
 */
typedef struct PalColorBlendAttachment
{
    PalBool enableBlend;
    PalColorMask colorWriteMask;
    PalBlendFactor dstColorBlendFactor;
    PalBlendFactor srcColorBlendFactor;
    PalBlendOp colorBlendOp;           
    PalBlendFactor dstAlphaBlendFactor;
    PalBlendFactor srcAlphaBlendFactor;
    PalBlendOp alphaBlendOp;           
} PalColorBlendAttachment;

/**
 * @struct PalFragmentShadingRateState
 * @brief Contains information about the fragment shading rate state.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalFragmentShadingRateState::rate
 * The shading rate (eg. `PAL_FRAGMENT_SHADING_RATE_2X2`).
 * 
 * @var PalFragmentShadingRateState::combinerOps
 * The fragment shading rate combiner operations.
 */
typedef struct PalFragmentShadingRateState
{
    PalFragmentShadingRate rate;
    PalFragmentShadingRateCombinerOp combinerOps[2];
} PalFragmentShadingRateState;

/**
 * @struct PalAccelerationStructureInstance
 * @brief Contains acceleration structure instance base data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalAccelerationStructureInstance::blas
 * The bottom level acceleration structure.
 * 
 * @var PalAccelerationStructureInstance::flags
 * The instance flags.
 * 
 * @var PalAccelerationStructureInstance::mask
 * The mask. Only the lower `8-bits` are used (0x00 - 0xFF).
 * 
 * @var PalAccelerationStructureInstance::instanceId
 * The id used to identify the instance.
 * 
 * @var PalAccelerationStructureInstance::hitGroupOffset
 * The hitgroup offset.
 * 
 * @var PalAccelerationStructureInstance::transform
 * The instance trasnform. Must be row major.
 */
typedef struct PalAccelerationStructureInstance
{
    PalAccelerationStructure* blas; 
    PalAccelerationStructureInstanceFlags flags; 
    uint32_t mask;
    uint32_t instanceId;
    uint32_t hitGroupOffset;
    float transform[12];
} PalAccelerationStructureInstance;

/**
 * @struct PalAccelerationStructureBuildSize
 * @brief Contains acceleration structure build size.
 *
 * @since Added in version 2.0
 * 
 * @var PalAccelerationStructureBuildSize::accelerationStructureSize
 * The required acceleration structure size in bytes.
 * 
 * @var PalAccelerationStructureBuildSize::scratchBufferSize
 * The required scratch buffer size in bytes.
 * 
 * @var PalAccelerationStructureBuildSize::updateScratchBufferSize
 * The required scratch buffer update size in bytes.
 */
typedef struct PalAccelerationStructureBuildSize
{
    uint64_t accelerationStructureSize;
    uint64_t scratchBufferSize;        
    uint64_t updateScratchBufferSize;  
} PalAccelerationStructureBuildSize;

/**
 * @struct PalGeometryDataTriangle
 * @brief Contains acceleration structure triangle geometry data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalGeometryDataTriangle::vertexBufferAddress
 * The address of the vertex buffer.
 * 
 * @var PalGeometryDataTriangle::indexBufferAddress
 * The address of the index buffer.
 * 
 * @var PalGeometryDataTriangle::transformBufferAddress
 * The address of the transform buffer.
 * 
 * @var PalGeometryDataTriangle::vertexType
 * The vertex type of ::vertexBufferAddress.
 * 
 * @var PalGeometryDataTriangle::indexType
 * The index type of ::indexBufferAddress.
 * 
 * @var PalGeometryDataTriangle::vertexCount
 * The number of vertices in ::vertexBufferAddress.
 * 
 * @var PalGeometryDataTriangle::vertexStride
 * The size of each vertex in bytes in ::vertexBufferAddress.
 */
typedef struct PalGeometryDataTriangle
{
    PalDeviceAddress vertexBufferAddress;
    PalDeviceAddress indexBufferAddress;
    PalDeviceAddress transformBufferAddress;
    PalVertexType vertexType;
    PalIndexType indexType;
    uint32_t vertexCount;
    uint32_t vertexStride;
} PalGeometryDataTriangle;

/**
 * @struct PalGeometryDataAABBS
 * @brief Contains acceleration structure AABBS geometry data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalGeometryDataAABBS::bufferAddress
 * The address of the `AABBS` buffer.
 * 
 * @var PalGeometryDataAABBS::stride
 * The size of each `AABBS` in bytes in ::bufferAddress.
 */
typedef struct PalGeometryDataAABBS
{
    PalDeviceAddress bufferAddress;
    uint64_t stride;               
} PalGeometryDataAABBS;

/**
 * @struct PalGeometry
 * @brief Contains acceleration structure geometry.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalGeometry::data
 * The geometry data. This is based on ::type.
 * 
 * @var PalGeometry::primitiveCount
 * The number of primitives in ::data.
 * 
 * @var PalGeometry::flags
 * The geometry flags.
 * 
 * @var PalGeometry::type
 * The geometry type (eg. `PAL_GEOMETRY_TYPE_TRIANGLE`).
 */
typedef struct PalGeometry
{
    const void* data;
    uint64_t primitiveCount;
    PalGeometryFlags flags;
    PalGeometryType type;  
} PalGeometry;

/**
 * @struct PalAccelerationStructureBuildInfo
 * @brief Contains build information of an acceleration structure.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalAccelerationStructureBuildInfo::dst
 * The destination acceleration structure.
 * 
 * @var PalAccelerationStructureBuildInfo::src
 * The Source acceleration structure. This is used update builds.
 * 
 * @var PalAccelerationStructureBuildInfo::geometries
 * The Bottom level acceleration structure geometries.
 * Set to `nullptr` if ::type is `PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL`.
 * 
 * @var PalAccelerationStructureBuildInfo::scratchBufferAddress
 * The address of the scratch buffer.
 * 
 * @var PalAccelerationStructureBuildInfo::instanceBufferAddress
 * The address of the instance buffer.
 * Set to `nullptr` if ::type is `PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL`
 * 
 * @var PalAccelerationStructureBuildInfo::buildHints
 * Driver build hints. These are hints and might be ignored.
 * 
 * @var PalAccelerationStructureBuildInfo::type
 * The type of the acceleration structure.
 * 
 * @var PalAccelerationStructureBuildInfo::buildMode
 * The build mode of the acceleration structure.
 * (eg. `PAL_ACCELERATION_STRUCTURE_BUILD_MODE_BUILD`).
 * 
 * @var PalAccelerationStructureBuildInfo::count
 * If ::type is `PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL`, this is
 * the number of instances in ::instanceBufferAddress. @nl
 * If ::type is `PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL`, this is @nl
 * the number of geometries in ::geometries.
 */
typedef struct PalAccelerationStructureBuildInfo
{
    PalAccelerationStructure* dst;
    PalAccelerationStructure* src;
    PalGeometry* geometries;
    PalDeviceAddress scratchBufferAddress;
    PalDeviceAddress instanceBufferAddress;
    PalAccelerationStructureBuildHints buildHints;
    PalAccelerationStructureType type;
    PalAccelerationStructureBuildMode buildMode;
    uint32_t count;
} PalAccelerationStructureBuildInfo;

/**
 * @struct PalDescriptorSetLayoutBinding
 * @brief Contains information about a descriptor set layout binding.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDescriptorSetLayoutBinding::descriptorCount
 * The number of descriptors of ::descriptorType.
 * 
 * @var PalDescriptorSetLayoutBinding::descriptorType
 * The type of the descriptor. (eg. `PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE`).
 */
typedef struct PalDescriptorSetLayoutBinding
{
    uint32_t descriptorCount;
    PalDescriptorType descriptorType;
} PalDescriptorSetLayoutBinding;

/**
 * @struct PalDescriptorPoolBindingSize
 * @brief Contains information about descriptor pool binding size.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDescriptorPoolBindingSize::bindingCount
 * The number of bindings of ::descriptorType.
 * 
 * @var PalDescriptorPoolBindingSize::descriptorType
 * The type of the descriptor.
 */
typedef struct PalDescriptorPoolBindingSize
{
    uint32_t bindingCount;
    PalDescriptorType descriptorType;
} PalDescriptorPoolBindingSize;

/**
 * @struct PalDescriptorBufferInfo
 * @brief Contains information about a buffer descriptor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDescriptorBufferInfo::buffer
 * The buffer associated with the descriptor.
 * 
 * @var PalDescriptorBufferInfo::offset
 * The offset of ::buffer in bytes. This will be divided by ::stride 
 * if the buffer is marked as structured.
 * 
 * @var PalDescriptorBufferInfo::size
 * The size of ::buffer in bytes.
 * 
 * @var PalDescriptorBufferInfo::stride
 * This will be ignored if ::buffer is not marked as structured.
 */
typedef struct PalDescriptorBufferInfo
{
    PalBuffer* buffer;
    uint64_t offset;
    uint64_t size;  
    uint64_t stride;
} PalDescriptorBufferInfo;

/**
 * @struct PalDescriptorImageViewInfo
 * @brief Contains information about an image view descriptor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDescriptorBufferInfo::imageView
 * The image view associated with the descriptor.
 */
typedef struct PalDescriptorImageViewInfo
{
    PalImageView* imageView;
} PalDescriptorImageViewInfo;

/**
 * @struct PalDescriptorSamplerInfo
 * @brief Contains information about a sampler descriptor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDescriptorSamplerInfo::sampler
 * The sampler associated with the descriptor.
 */
typedef struct PalDescriptorSamplerInfo
{
    PalSampler* sampler;
} PalDescriptorSamplerInfo;

/**
 * @struct PalDescriptorTLASInfo
 * @brief Contains information about a TLAS descriptor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDescriptorTLASInfo::tlas
 * The top level acceleration structure associated with the descriptor.
 */
typedef struct PalDescriptorTLASInfo
{
    PalAccelerationStructure* tlas;
} PalDescriptorTLASInfo;

/**
 * @struct PalDescriptorSetWriteInfo
 * @brief Contains write information of a descriptor set.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDescriptorSetWriteInfo::descriptorSet
 * The descriptor set to update. Must not be `nullptr`.
 * 
 * @var PalDescriptorSetWriteInfo::bufferInfos
 * The buffer infos. This will be used if ::descriptorType is
 * `PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER`  or 
 * `PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER`
 * 
 * @var PalDescriptorSetWriteInfo::imageViewInfos
 * The image view infos. This will be used if ::descriptorType is
 * `PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE`  or 
 * `PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE`
 * 
 * @var PalDescriptorSetWriteInfo::samplerInfos
 * The sampler infos. This will be used if ::descriptorType is
 * `PAL_DESCRIPTOR_TYPE_SAMPLER`.
 * 
 * @var PalDescriptorSetWriteInfo::tlasInfos
 * The top level acceleration structure infos. This will be used if
 * ::descriptorType is `PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE`.
 * 
 * @var PalDescriptorSetWriteInfo::descriptorType
 * The type of the descriptor.
 * 
 * @var PalDescriptorSetWriteInfo::layoutBindingIndex
 * The index into the descriptor set layout (bindings) that was used
 * to create ::descriptorSet.
 * 
 * @var PalDescriptorSetWriteInfo::arrayElement
 * The first index in the descriptor set layout (bindings) that was used
 * to create ::descriptorSet.
 * 
 * @var PalDescriptorSetWriteInfo::descriptorCount
 * The number of descriptors to write.
 */
typedef struct PalDescriptorSetWriteInfo
{
    PalDescriptorSet* descriptorSet;
    PalDescriptorBufferInfo* bufferInfos;
    PalDescriptorImageViewInfo* imageViewInfos;
    PalDescriptorSamplerInfo* samplerInfos;
    PalDescriptorTLASInfo* tlasInfos;
    PalDescriptorType descriptorType;
    uint32_t layoutBindingIndex;
    uint32_t arrayElement;
    uint32_t descriptorCount;
} PalDescriptorSetWriteInfo;

/**
 * @struct PalBarrierInfo
 * @brief Contains information about a barrier.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalBarrierInfo::oldState
 * The old usage state of the resource.
 * 
 * @var PalBarrierInfo::newState
 * The new usage state of the resource.
 * 
 * @var PalBarrierInfo::srcStages
 * The source pipeline stages of the resource.
 * 
 * @var PalBarrierInfo::dstStages
 * The destination pipeline stages of the resource.
 */
typedef struct PalBarrierInfo
{
    PalUsageState oldState;
    PalUsageState newState;
    PalPipelineStages srcStages;
    PalPipelineStages dstStages;
} PalBarrierInfo;

/**
 * @struct PalPushConstantInfo
 * @brief Conatains information about a push constant.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalPushConstantInfo::offset
 * The offset in bytes of the push constant.
 * 
 * @var PalPushConstantInfo::size
 * The size in bytes of the push constant.
 */
typedef struct PalPushConstantInfo
{
    uint32_t offset;
    uint32_t size;
} PalPushConstantInfo;

/**
 * @struct PalImageSubresourceRange
 * @brief Contains subresource range for images.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalImageSubresourceRange::aspect
 * The image aspect. (eg. `PAL_IMAGE_ASPECT_COLOR`).
 * 
 * @var PalImageSubresourceRange::startMipLevel
 * The start mipmap level of the image.
 * 
 * @var PalImageSubresourceRange::mipLevelCount
 * The number of mipmap levels of the image.
 * 
 * @var PalImageSubresourceRange::startArrayLayer
 * The start array layer of the image.
 * 
 * @var PalImageSubresourceRange::layerArrayCount
 * The number of array layers of the image.
 */
typedef struct PalImageSubresourceRange
{
    PalImageAspect aspect;
    uint32_t startMipLevel;
    uint32_t mipLevelCount;
    uint32_t startArrayLayer;
    uint32_t layerArrayCount;
} PalImageSubresourceRange;

/**
 * @struct PalBufferCopyInfo
 * @brief Contains information for buffer to buffer copies.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalBufferCopyInfo::size
 * The size in bytes to copy from the source buffer.
 * 
 * @var PalBufferCopyInfo::dstOffset
 * The offset in bytes of the destination buffer.
 * 
 * @var PalBufferCopyInfo::srcOffset
 * The offset in bytes of the source buffer.
 */
typedef struct PalBufferCopyInfo
{
    uint64_t size;
    uint64_t dstOffset;
    uint64_t srcOffset;
} PalBufferCopyInfo;

/**
 * @struct PalBufferImageCopyInfo
 * @brief Contains information for image to buffer and vice versa copies.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalBufferImageCopyInfo::bufferOffset
 * The offset in bytes into the buffer.
 * 
 * @var PalBufferImageCopyInfo::imageAspect
 * The aspect of the image.
 * 
 * @var PalBufferImageCopyInfo::bufferRowLength
 * The buffer row length in texels.
 * 
 * @var PalBufferImageCopyInfo::bufferImageHeight
 * The buffer image height in texels.
 * 
 * @var PalBufferImageCopyInfo::ImageMipLevel
 * The mipmap of the image.
 * 
 * @var PalBufferImageCopyInfo::ImageStartArrayLayer
 * The start array layer of the image.
 * 
 * @var PalBufferImageCopyInfo::ImageArrayLayerCount
 * The number of array layers of the image.
 * 
 * @var PalBufferImageCopyInfo::imageOffsetX
 * The x offset in bytes of the image.
 * 
 * @var PalBufferImageCopyInfo::imageOffsetY
 * The y offset in bytes of the image.
 * 
 * @var PalBufferImageCopyInfo::imageOffsetZ
 * The z offset in bytes of the image.
 * 
 * @var PalBufferImageCopyInfo::imageWidth
 * The width in bytes of the image.
 * 
 * @var PalBufferImageCopyInfo::imageHeight
 * The height in bytes of the image.
 * 
 * @var PalBufferImageCopyInfo::imageDepth
 * The depth in bytes of the image.
 */
typedef struct PalBufferImageCopyInfo
{
    uint64_t bufferOffset;
    PalImageAspect imageAspect;
    uint32_t bufferRowLength;
    uint32_t bufferImageHeight;
    uint32_t ImageMipLevel;
    uint32_t ImageStartArrayLayer;
    uint32_t ImageArrayLayerCount;
    int32_t imageOffsetX;
    int32_t imageOffsetY;
    int32_t imageOffsetZ;
    uint32_t imageWidth;
    uint32_t imageHeight;
    uint32_t imageDepth;
} PalBufferImageCopyInfo;

/**
 * @struct PalImageCopyInfo
 * @brief Contains information for image to image copies.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalImageCopyInfo::aspect
 * The aspect of the image.
 * 
 * @var PalImageCopyInfo::dstMipLevel
 * The destination mipmap level of the image.
 * 
 * @var PalImageCopyInfo::srcMipLevel
 * The source mipmap level of the image.
 * 
 * @var PalImageCopyInfo::dstStartArrayLayer
 * The start array layer of the destination image.
 * 
 * @var PalImageCopyInfo::srcStartArrayLayer
 * The start array layer of the source image.
 * 
 * @var PalImageCopyInfo::arrayLayerCount
 * The number of array layers of the destination and source images.
 * 
 * @var PalImageCopyInfo::dstOffsetX
 * The x offset in bytes of the destination image.
 * 
 * @var PalImageCopyInfo::srcOffsetX
 * The x offset in bytes of the source image.
 * 
 * @var PalImageCopyInfo::dstOffsetY
 * The y offset in bytes of the destination image.
 * 
 * @var PalImageCopyInfo::srcOffsetY
 * The y offset in bytes of the source image.
 * 
 * @var PalImageCopyInfo::dstOffsetZ
 * The z offset in bytes of the destination image.
 * 
 * @var PalImageCopyInfo::srcOffsetZ
 * The z offset in bytes of the source image.
 * 
 * @var PalImageCopyInfo::width
 * The width of the copy region into the source image.
 * 
 * @var PalImageCopyInfo::height
 * The height of the copy region into the source image.
 * 
 * @var PalImageCopyInfo::depth
 * The depth of the copy region into the source image.
 */
typedef struct PalImageCopyInfo
{
    PalImageAspect aspect;
    uint32_t dstMipLevel;
    uint32_t srcMipLevel;
    uint32_t dstStartArrayLayer;
    uint32_t srcStartArrayLayer;
    uint32_t arrayLayerCount;
    int32_t dstOffsetX;
    int32_t srcOffsetX;
    int32_t dstOffsetY;
    int32_t srcOffsetY;
    int32_t dstOffsetZ;
    int32_t srcOffsetZ;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
} PalImageCopyInfo;

/**
 * @struct PalShaderBindingTableRecordInfo
 * @brief Contains information about a shader binding record.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * The records array must be in this order [raygen][miss][hitgroup][callable].
 *
 * @since Added in version 2.0
 * 
 * @var PalShaderBindingTableRecordInfo::localData
 * The local data of the record. This will be ignored of ::localDataSize is `0`
 * 
 * @var PalShaderBindingTableRecordInfo::groupIndex
 * The index into the shader groups used to create the ray tracing pipeline.
 * 
 * @var PalShaderBindingTableRecordInfo::localDataSize
 * The local data size.
 */
typedef struct PalShaderBindingTableRecordInfo
{
    void* localData;
    uint32_t groupIndex;
    uint32_t localDataSize;
} PalShaderBindingTableRecordInfo;

/**
 * @struct PalShaderEntryInfo
 * @brief Contains information about a shader entry.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalShaderEntryInfo::entryName
 * The shader entry name.
 * 
 * @var PalShaderEntryInfo::stage
 * The shader stage of the entry (eg. `PAL_SHADER_STAGE_VERTEX`).
 * 
 * @var PalShaderEntryInfo::patchControlPoints
 * The tessellation patch point. This will be used for tessellation shaders.
 */
typedef struct PalShaderEntryInfo
{
    const char* entryName;
    PalShaderStage stage;
    uint32_t patchControlPoints;
} PalShaderEntryInfo;

/**
 * @struct PalImageCreateInfo
 * @brief Contains creation parameters of an image.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalImageCreateInfo::usages
 * A bitmask of the usages of the image.
 * The usages must be supported by ::format.
 * 
 * @var PalImageCreateInfo::width
 * The width of the image in bytes.
 * 
 * @var PalImageCreateInfo::height
 * The height of the image in bytes.
 * 
 * @var PalImageCreateInfo::depth
 * The depth of the image in bytes.
 * 
 * @var PalImageCreateInfo::arrayLayerCount
 * The number of array layers of the image.
 * 
 * @var PalImageCreateInfo::arrayLayerCount
 * The number of mipmap levels of the image.
 * 
 * @var PalImageCreateInfo::sampleCount
 * The `MSAA` samples of the image.
 * 
 * @var PalImageCreateInfo::type
 * The type of the image (eg. `PAL_IMAGE_TYPE_2D`).
 * 
 * @var PalImageCreateInfo::format
 * The format of the image (eg. `PAL_FORMAT_B8G8R8A8_UNORM`).
 * 
 * @var PalImageCreateInfo::memoryUsage
 * The memory usage model of the image (eg. `PAL_IMAGE_MEMORY_USAGE_MANUAL`).
 */
typedef struct PalImageCreateInfo
{
    PalImageUsages usages;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t arrayLayerCount;
    uint32_t mipLevelCount;
    PalSampleCount sampleCount;
    PalImageType type;
    PalFormat format;
    PalImageMemoryUsage memoryUsage;
} PalImageCreateInfo;

/**
 * @struct PalImageViewCreateInfo
 * @brief Contains creation parameters of an image view.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalImageViewCreateInfo::format
 * The format of the image view. It must be compatible with the image format.
 * 
 * @var PalImageViewCreateInfo::type
 * The type of the image view. It must be compatible with the image format.
 * 
 * @var PalImageViewCreateInfo::subresourceRange
 * The subresource range of the image to create the image view from.
 */
typedef struct PalImageViewCreateInfo
{
    PalFormat format;
    PalImageViewType type;
    PalImageSubresourceRange subresourceRange;
} PalImageViewCreateInfo;

/**
 * @struct PalSamplerCreateInfo
 * @brief Contains creation parameters of a sampler.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalSamplerCreateInfo::enableCompare
 * `PAL_TRUE` to enable compare operations.
 * 
 * @var PalSamplerCreateInfo::enableAnisotropy
 * `PAL_TRUE` to enable texture filtering.
 * 
 * @var PalSamplerCreateInfo::mipLodBias
 * The offset applied to the selected mipmap level.
 * 
 * @var PalSamplerCreateInfo::minLod
 * The minimum mipmap level allowed.
 * 
 * @var PalSamplerCreateInfo::maxLod
 * The maximum mipmap level allowed.
 * 
 * @var PalSamplerCreateInfo::maxAnisotropy
 * The maximum texture filtering level.
 * 
 * @var PalSamplerCreateInfo::minFilterMode
 * The filter mode used for minification.
 * 
 * @var PalSamplerCreateInfo::magFilterMode
 * The filter mode used for magnification.
 * 
 * @var PalSamplerCreateInfo::mipmapMode
 * The mipmap filtering mode.
 * 
 * @var PalSamplerCreateInfo::addressModeU
 * The address mode for the u texture coordinate.
 * 
 * @var PalSamplerCreateInfo::addressModeV
 * The address mode for the v texture coordinate.
 * 
 * @var PalSamplerCreateInfo::addressModeW
 * The address mode for the w texture coordinate.
 * 
 * @var PalSamplerCreateInfo::compareOp
 * The compare operation.
 * 
 * @var PalSamplerCreateInfo::borderColor
 * The color used outside the texture.
 */
typedef struct PalSamplerCreateInfo
{
    PalBool enableCompare;
    PalBool enableAnisotropy;
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
 * @brief Contains creation parameters of a swapchain.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalSwapchainCreateInfo::clipped
 * `PAL_TRUE` to discard pixels that are not visible.
 * 
 * @var PalSwapchainCreateInfo::width
 * The width of the swapchain in pixels.
 * 
 * @var PalSwapchainCreateInfo::height
 * The height of the swapchain in pixels.
 * 
 * @var PalSwapchainCreateInfo::imageCount
 * The number of images or back buffers of the swapchain.
 * 
 * @var PalSwapchainCreateInfo::imageArrayLayerCount
 * The number of array layers of the swapchain.
 * 
 * @var PalSwapchainCreateInfo::presentMode
 * The present mode of the swapchain.
 * 
 * @var PalSwapchainCreateInfo::compositeAlpha
 * The composite alpha of the swapchain.
 * 
 * @var PalSwapchainCreateInfo::format
 * The format of the swapchain. This is the format and colorspace.
 * (eg. `PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR`).
 */
typedef struct PalSwapchainCreateInfo
{
    PalBool clipped; 
    uint32_t width;   
    uint32_t height;  
    uint32_t imageCount;
    uint32_t imageArrayLayerCount;
    PalPresentMode presentMode;
    PalCompositeAplha compositeAlpha;
    PalSurfaceFormat format;
} PalSwapchainCreateInfo;

/**
 * @struct PalShaderCreateInfo
 * @brief Contains creation parameters of a swapchain.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalShaderCreateInfo::code
 * The shader source or bytecode.
 * 
 * @var PalShaderCreateInfo::entries
 * The shader entries.
 * 
 * @var PalShaderCreateInfo::codeSize
 * The size of the shader source or bytecode.
 * 
 * @var PalShaderCreateInfo::entryCount
 * The number of shader entries.
 */
typedef struct PalShaderCreateInfo
{
    void* code;
    PalShaderEntryInfo* entries;
    uint32_t codeSize;
    uint32_t entryCount;
} PalShaderCreateInfo;



/**
 * @struct PalAccelerationStructureCreateInfo
 * @brief Contains creation parameters of an acceleration structure.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalAccelerationStructureCreateInfo::buffer
 * The acceleration structure buffer. It must be valid.
 * 
 * @var PalAccelerationStructureCreateInfo::offset
 * The offset in bytes into the buffer.
 * 
 * @var PalAccelerationStructureCreateInfo::size
 * The size in bytes of the buffer.
 * 
 * @var PalAccelerationStructureCreateInfo::type
 * The type of the acceleration structure.
 * (eg. `PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL`).
 * 
 * @var PalAccelerationStructureCreateInfo::reserved
 * Not used. Set to `0`.
 */
typedef struct PalAccelerationStructureCreateInfo
{
    PalBuffer* buffer;
    uint64_t offset;
    uint64_t size;   
    PalAccelerationStructureType type;
    uint32_t reserved;
} PalAccelerationStructureCreateInfo;

/**
 * @struct PalDescriptorSetLayoutCreateInfo
 * @brief Contains creation parameters of a descriptor set layout.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDescriptorSetLayoutCreateInfo::bindings
 * The layout bindings.
 * 
 * @var PalDescriptorSetLayoutCreateInfo::flags
 * A bitmask of the descriptor indexing flags to create the 
 * descriptor set layout with.
 * 
 * @var PalDescriptorSetLayoutCreateInfo::bindingCount
 * The number of layout bindings.
 */
typedef struct PalDescriptorSetLayoutCreateInfo
{
    PalDescriptorSetLayoutBinding* bindings;
    PalDescriptorIndexingFlags flags;
    uint32_t bindingCount;
} PalDescriptorSetLayoutCreateInfo;

/**
 * @struct PalDescriptorPoolCreateInfo
 * @brief Contains creation parameters of a descriptor pool.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalDescriptorPoolCreateInfo::bindingSizes
 * The binding sizes.
 * 
 * @var PalDescriptorPoolCreateInfo::bindingSizeCount
 * The number of binding sizes.
 * 
 * @var PalDescriptorPoolCreateInfo::maxDescriptorSets
 * The maximum number of descriptor sets that can be allocated.
 * 
 * @var PalDescriptorPoolCreateInfo::flags
 * A bitmask of the descriptor indexing flags to create the 
 * descriptor pool with.
 * 
 * @var PalDescriptorPoolCreateInfo::reserved
 * Not used. Set to `0`.
 */
typedef struct PalDescriptorPoolCreateInfo
{
    PalDescriptorPoolBindingSize* bindingSizes;
    uint32_t bindingSizeCount;
    uint32_t maxDescriptorSets;
    PalDescriptorIndexingFlags flags; 
    uint32_t reserved;
} PalDescriptorPoolCreateInfo;

/**
 * @struct PalPipelineLayoutCreateInfo
 * @brief Contains creation parameters of a pipeline layout.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalPipelineLayoutCreateInfo::descriptorSetLayouts
 * The descriptor set layouts.
 * 
 * @var PalPipelineLayoutCreateInfo::pushConstantInfo
 * The push constant info of the pipeline layout.
 * 
 * @var PalPipelineLayoutCreateInfo::descriptorSetLayoutCount
 * The number of descriptor set layouts.
 * 
 * @var PalPipelineLayoutCreateInfo::usePushConstant
 * If `PAL_TRUE`, ::pushConstantInfo will be read and used.
 */
typedef struct PalPipelineLayoutCreateInfo
{
    PalDescriptorSetLayout** descriptorSetLayouts;
    PalPushConstantInfo pushConstantInfo;
    uint32_t descriptorSetLayoutCount;
    PalBool usePushConstant;
} PalPipelineLayoutCreateInfo;

/**
 * @struct PalGraphicsPipelineCreateInfo
 * @brief Contains creation parameters of a graphics pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalGraphicsPipelineCreateInfo::pipelineLayout
 * The pipeline layout.
 * 
 * @var PalGraphicsPipelineCreateInfo::shaders
 * The shaders.
 * 
 * @var PalGraphicsPipelineCreateInfo::vertexLayouts
 * The vertex layouts.
 * 
 * @var PalGraphicsPipelineCreateInfo::colorBlendAttachments
 * The color blend attachments.
 * 
 * @var PalGraphicsPipelineCreateInfo::rasterizerState
 * The rasterizer state. Set to `nullptr` to use the default.
 * 
 * @var PalGraphicsPipelineCreateInfo::multisampleState
 * The multisample state. Set to `nullptr` to use the default.
 * 
 * @var PalGraphicsPipelineCreateInfo::depthStencilState
 * The depth/stencil state. Set to `nullptr` to use the default.
 * 
 * @var PalGraphicsPipelineCreateInfo::fragmentShadingRateState
 * The fragment shading rate state.
 * 
 * @var PalGraphicsPipelineCreateInfo::renderingLayout
 * The rendering layout.
 * 
 * @var PalGraphicsPipelineCreateInfo::primitiveRestartEnable
 * `PAL_TRUE` to enable primitive restart for indexed draw.
 * 
 * @var PalGraphicsPipelineCreateInfo::vertexLayoutCount
 * The number of vertex layouts.
 * 
 * @var PalGraphicsPipelineCreateInfo::colorBlendAttachmentCount
 * The number of color blend attachments.
 * 
 * @var PalGraphicsPipelineCreateInfo::shaderCount
 * The number of shaders.
 * 
 * @var PalGraphicsPipelineCreateInfo::indexType
 * Will be used if ::primitiveRestartEnable is `PAL_TRUE`.
 * 
 * @var PalGraphicsPipelineCreateInfo::topology
 * The primitive topology type (eg. `PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST`).
 */
typedef struct PalGraphicsPipelineCreateInfo
{
    PalPipelineLayout* pipelineLayout; 
    PalShader** shaders;               
    PalVertexLayout* vertexLayouts; 
    PalColorBlendAttachment* colorBlendAttachments;
    PalRasterizerState* rasterizerState;   
    PalMultisampleState* multisampleState; 
    PalDepthStencilState* depthStencilState;
    PalFragmentShadingRateState* fragmentShadingRateState;
    PalRenderingLayoutInfo* renderingLayout;
    PalBool primitiveRestartEnable;
    uint32_t vertexLayoutCount;
    uint32_t colorBlendAttachmentCount;
    uint32_t shaderCount;
    PalIndexType indexType;
    PalPrimitiveTopology topology;
} PalGraphicsPipelineCreateInfo;

/**
 * @struct PalComputePipelineCreateInfo
 * @brief Contains creation parameters of a compute pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalComputePipelineCreateInfo::pipelineLayout
 * The pipeline layout.
 * 
 * @var PalComputePipelineCreateInfo::computeShader
 * The compute shader.
 */
typedef struct PalComputePipelineCreateInfo
{
    PalPipelineLayout* pipelineLayout;
    PalShader* computeShader;
} PalComputePipelineCreateInfo;

/**
 * @struct PalRayTracingShaderGroupCreateInfo
 * @brief Contains creation parameters of a ray tracing shader group.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * The shader group array must be in this order 
 * [raygen][miss][hitgroup][callable].
 *
 * @since Added in version 2.0
 * 
 * @var PalRayTracingShaderGroupCreateInfo::type
 * The type of the ray tracing shader group.
 * (eg. `PAL_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL`).
 * 
 * @var PalRayTracingShaderGroupCreateInfo::anyHitShaderIndex
 * The index of the anyhit shader.
 * 
 * @var PalRayTracingShaderGroupCreateInfo::anyHitShaderEntryIndex
 * The index of the anyhit shader entry.
 * 
 * @var PalRayTracingShaderGroupCreateInfo::closestHitShaderIndex
 * The index of the closest hit shader.
 * 
 * @var PalRayTracingShaderGroupCreateInfo::closestHitShaderEntryIndex
 * The index of the closest hit shader entry.
 * 
 * @var PalRayTracingShaderGroupCreateInfo::generalShaderIndex
 * The index of the general shader.
 * 
 * @var PalRayTracingShaderGroupCreateInfo::generalShaderEntryIndex
 * The index of the general shader entry.
 * 
 * @var PalRayTracingShaderGroupCreateInfo::intersectionShaderIndex
 * The index of the intersection shader.
 * 
 * @var PalRayTracingShaderGroupCreateInfo::intersectionShaderEntryIndex
 * The index of the intersection shader entry.
 * 
 * @var PalRayTracingShaderGroupCreateInfo::maxDataSize
 * The maximum size in bytes of the data associated with the shader group.
 */
typedef struct PalRayTracingShaderGroupCreateInfo
{
    PalRayTracingShaderGroupType type;
    uint32_t anyHitShaderIndex; 
    uint32_t anyHitShaderEntryIndex; 
    uint32_t closestHitShaderIndex;
    uint32_t closestHitShaderEntryIndex;
    uint32_t generalShaderIndex;
    uint32_t generalShaderEntryIndex;
    uint32_t intersectionShaderIndex; 
    uint32_t intersectionShaderEntryIndex;
    uint32_t maxDataSize;
} PalRayTracingShaderGroupCreateInfo;

/**
 * @struct PalRayTracingPipelineCreateInfo
 * @brief Contains creation parameters of a ray tracing pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalRayTracingPipelineCreateInfo::pipelineLayout
 * The pipeline layout.
 * 
 * @var PalRayTracingPipelineCreateInfo::shaderGroups
 * The ray tracing shader groups.
 * 
 * @var PalRayTracingPipelineCreateInfo::shaders
 * The shaders
 * 
 * @var PalRayTracingPipelineCreateInfo::shaderGroupCount
 * The number of ray tracing shader groups.
 * 
 * @var PalRayTracingPipelineCreateInfo::shaderCount
 * The number of shaders.
 * 
 * @var PalRayTracingPipelineCreateInfo::maxRecursionDepth
 * The maximum recursion depth of the pipeline.
 * 
 * @var PalRayTracingPipelineCreateInfo::maxAttributeSize
 * The maximum ray attribute size in bytes of the pipeline.
 * 
 * @var PalRayTracingPipelineCreateInfo::maxPayloadSize
 * The maximum payload size in bytes of the pipeline.
 * 
 * @var PalRayTracingPipelineCreateInfo::reserved
 * Not used. Set to `0`.
 */
typedef struct PalRayTracingPipelineCreateInfo
{
    PalPipelineLayout* pipelineLayout;
    PalRayTracingShaderGroupCreateInfo* shaderGroups;
    PalShader** shaders;
    uint32_t shaderGroupCount;
    uint32_t shaderCount;
    uint32_t maxRecursionDepth;
    uint32_t maxAttributeSize;
    uint32_t maxPayloadSize;
    uint32_t reserved;
} PalRayTracingPipelineCreateInfo;

/**
 * @struct PalShaderBindingTableCreateInfo
 * @brief Contains creation parameters of a shader binding table.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalShaderBindingTableCreateInfo::records
 * The shader binding table records.
 * 
 * @var PalShaderBindingTableCreateInfo::rayTracingPipeline
 * The ray tracing pipeline.
 * 
 * @var PalShaderBindingTableCreateInfo::recordCount
 * The number of shader binding table records.
 * 
 * @var PalShaderBindingTableCreateInfo::reserved
 * Not used. Set to `0`.
 */
typedef struct PalShaderBindingTableCreateInfo
{
    PalShaderBindingTableRecordInfo* records;
    PalPipeline* rayTracingPipeline;
    uint32_t recordCount;
    uint32_t reserved;
} PalShaderBindingTableCreateInfo;

/**
 * @struct PalGraphicsBackendInfo
 * @brief Contains information of a graphics backend.
 *
 * All backend handles implementation (eg. struct CustomBuffer) must reserve
 * its first field as a `void*`. This will be used by the graphics layer.
 *
 * Each backend Vtable version (eg. `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_1`) 
 * has required functions that must be present implemented. This will be 
 * validated at initialization. See version constant for the required 
 * functions. Optional functions have their own requirements.
 * 
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalGraphicsBackendInfo::vtable
 * The graphics backend vtable.
 * 
 * @var PalGraphicsBackendInfo::version
 * The version of the graphics backend vtable.
 * 
 * @var PalGraphicsBackendInfo::version
 * Not used. Set to `0`.
 */
typedef struct PalGraphicsBackendInfo
{
    const void* vtable;
    PalGraphicsBackendVtableVersion version;
    uint32_t reserved;
} PalGraphicsBackendInfo;

/**
 * @brief Initialize the graphics system.
 *
 * The debugger, allocator and custom backends will not not copied, therefore the pointers must
 * remain valid until the graphics system is shutdown. Set the debugger to `nullptr` to disable
 * debugging and validation layers.
 *
 * If `debugger` is not `nullptr` and there is no debug layers, this function will not fail but
 * debugging will be disabled.
 *
 * All backends must have their vtable functions fully set according to the version requirements.
 * All required pointers must be set. If an optional feature is not supported, `nullptr` must be
 * set and its appropriate feature bit (eg. `PAL_ADAPTER_FEATURE_RAY_TRACING`) must not be set.
 *
 * @param[in] debugger Optional debugger. Set to `nullptr` to disable debugging and validation
 * layers.
 * @param[in] allocator Optional user-provided allocator. Set to `nullptr` to use default.
 * @param[in] customBackendCount The number of custom backends in `customBackends`.
 * @param[in] customBackends Pointer to an array of custom backends.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palShutdownGraphics
 */
PAL_API PalResult PAL_CALL palInitGraphics(
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator,
    uint32_t customBackendCount,
    const PalGraphicsBackendInfo* customBackends);

/**
 * @brief Shutdown the graphics system.
 *
 * If the graphics system has not been initialized, the function returns silently.
 * All created devices, queues, images, swapchains etc must be destroyed before this call.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since Added in version 2.0
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
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palEnumerateAdapters(
    uint32_t* count,
    PalAdapter** outAdapters);

/**
 * @brief Get information about an adapter (GPU).
 *
 * @param[in] adapter Adapter to query information on.
 * @param[out] info Pointer to a PalAdapterInfo to fill.
 *
 * Thread safety: Thread safe if `info` is per thread.
 *
 * @since Added in version 2.0
 * @sa palEnumerateAdapters
 */
PAL_API void PAL_CALL palGetAdapterInfo(
    PalAdapter* adapter,
    PalAdapterInfo* info);

/**
 * @brief Get capabilites or limits about an adapter (GPU).
 *
 * @param[in] adapter Adapter to query capabilities on.
 * @param[out] caps Pointer to a PalAdapterCapabilities to fill.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since Added in version 2.0
 * @sa palEnumerateAdapters
 */
PAL_API void PAL_CALL palGetAdapterCapabilities(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps);

/**
 * @brief Get the supported features of an adapter (GPU).
 *
 * @param[in] adapter Adapter to query features on.
 *
 * @return adapter features on success or `0` on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
 * @sa palEnumerateAdapters
 */
PAL_API PalAdapterFeatures PAL_CALL palGetAdapterFeatures(PalAdapter* adapter);

/**
 * @brief Get the highest supported shader target of an adapter (GPU).
 *
 * @param[in] adapter Adapter to query.
 * @param[in] shaderFormat The shader format. Must have only a single bit set.
 *
 * @return The highest supported shader target encoded with `PAL_MAKE_SHADER_TARGET` macro
 * on success otherwise `0` on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
 * @sa palEnumerateAdapters
 */
PAL_API uint32_t PAL_CALL palGetHighestSupportedShaderTarget(
    PalAdapter* adapter,
    PalShaderFormats shaderFormat);

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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized and
 * `outMemory` is per thread.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `device` is externally synchronized and
 * `outMemory` is per thread.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if the device used to create the queue is
 * externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Must only be called from the main thread.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.1
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
 * Thread safety: Thread safe.
 *
 * @sa palCanQueueUsePipelineStages
 * @since Added in version 2.1
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
 * Thread safety: Thread safe.
 *
 * @sa palCanQueueUseUsageState
 * @since Added in version 2.1
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `queue` is externally synchronized.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palWaitQueue(PalQueue* queue);

/**
 * @brief Returns a list of all supported formats of an adapter (GPU).
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
 * Thread safety: Thread safe if `outFormats` is per thread.
 *
 * @since Added in version 2.0
 * @sa palIsFormatSupported
 */
PAL_API void PAL_CALL palEnumerateFormats(
    PalAdapter* adapter,
    uint32_t* count,
    PalFormatInfo* outFormats);

/**
 * @brief Check support for a format on an adapter (GPU).
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
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if the device used to create the image is
 * externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `info` is per thread.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `requirements` is per thread.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `requirements` is per thread.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if the device used to create the image view is
 * externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if the device used to create the sampler is
 * externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if the device used to create the surface is
 * externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if the device used to create the swapchain is
 * externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `swapchain` externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `swapchain` externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @note The shader entry name must not be greater than `PAL_SHADER_ENTRY_NAME_SIZE (32)`.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if the device used to create the shader is
 * externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if the device used to create the fence is
 * externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `fence` is externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `fence` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if the device used to create the semaphore is
 * externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `semaphore` is externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `queue` is externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `semaphore` is externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if the device used to create the command pool is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateCommandPool
 */
PAL_API void PAL_CALL palDestroyCommandPool(PalCommandPool* pool);

/**
 * @brief Reset all command buffers allocated from the provided command pool.
 *
 * @param[in] pool Command pool to reset its command buffers.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `pool` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` and `pool` are externally synchronized.
 *
 * @since Added in version 2.0
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
 * @param[in] cmdBuffer Command buffer to free.
 *
 * Thread safety: Thread safe if the command pool used to create the command buffer is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palAllocateCommandBuffer
 */
PAL_API void PAL_CALL palFreeCommandBuffer(PalCommandBuffer* cmdBuffer);

/**
 * @brief Reset the provided command buffer.
 *
 * @param[in] cmdBuffer Command buffer to reset.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palResetCommandBuffer(PalCommandBuffer* cmdBuffer);

/**
 * @brief Submit a command buffer to the provided queue for execution.
 *
 * The command buffer must not be in a recording state.
 *
 * @param[in] queue Queue to execute the command buffer.
 * @param[in] info Pointer to a PalCommandBufferSubmitInfo struct that specifies parameters.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `queue` is externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCmdEnd
 */
PAL_API PalResult PAL_CALL palCmdBegin(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info);

/**
 * @brief End recording commands to the provided command buffer.
 *
 * @param[in] cmdBuffer Command buffer to begin recording.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `primaryCmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palCmdBeginRendering(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info);

/**
 * @brief End a rendering pass.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `srcCmdBuffer` and `dstCmdBuffer` are externally synchronized.
 *
 * @since Added in version 2.1
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `srcCmdBuffer` and `dstCmdBuffer` are externally synchronized.
 *
 * @since Added in version 2.1
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note The argument buffer memory must not be `PAL_MEMORY_TYPE_CPU_UPLOAD`. The implementation
 * internally copies the data into a GPU buffer for execution.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if the device used to create the acceleration structure is
 * externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetAccelerationStructureBuildSize(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size);



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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if the device used to create the descriptor set layout is
 * externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if the device used to create the descriptor pool is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateDescriptorPool
 */
PAL_API void PAL_CALL palDestroyDescriptorPool(PalDescriptorPool* pool);

/**
 * @brief Reset the provided descriptor pool. This resets all allocated descriptor sets.
 *
 * @param[in] pool Descriptor pool to reset.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `pool` is externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` and `pool` are externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if the device used to create the pipeline layout is
 * externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @note The first entry of the compute shader will be used.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @note The shader group array must be in this order
 * [raygen][miss][hitgroup][callable].
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if the device used to create the pipeline is
 * externally synchronized.
 *
 * @since Added in version 2.0
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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @note The records array must be in this order [raygen][miss][hitgroup][callable].
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if the device used to create the shader binding table is
 * externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Thread safe if `sbt` is externally synchronized.
 *
 * @since Added in version 2.0
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
 * Thread safety: Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palCmdDrawMeshTasks
 * @sa palCmdDrawMeshTasksIndirect
 * @sa palCmdDrawMeshTasksIndirectCount
 * @sa palCmdDispatch
 * @sa palCmdDispatchBase
 */
PAL_API void PAL_CALL palBuildWorkGroupInfo(
    const PalWorkGroupBuildData* data,
    uint32_t* count,
    PalWorkGroupInfo* info);

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
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
 */
static inline PalBool PAL_CALL palIsSupported(
    uint32_t mask,
    uint32_t value)
{
    return (mask & (1U << value)) != 0;
}

/** @} */

#endif // PAL_GRAPHICS_H