
/**
 * @file pal_graphics.h
 * @brief Header file for the PAL Graphics API.
 *
 * Defines the types, constants and functions of the module. The module provides a low-level, Vulkan-style API
 * for adapters, devices, queues, swapchains, resources, pipelines and command recording, and supports custom
 * backends.
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
 * @ingroup pal_graphics
 * @{
 */

#ifndef PAL_GRAPHICS_H
#define PAL_GRAPHICS_H

/**
 * @defgroup primitive_topologies Primitive Topologies
 * @ingroup pal_graphics
 * @{
 */
#define PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST 0 /**< Separate triangles. */
#define PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP 1 /**< Connected triangle strip. */
#define PAL_PRIMITIVE_TOPOLOGY_LINE_LIST 2 /**< Separate lines. */
#define PAL_PRIMITIVE_TOPOLOGY_LINE_STRIP 3 /**< Connected line strip. */
#define PAL_PRIMITIVE_TOPOLOGY_POINT_LIST 4 /**< Separate points. */
#define PAL_PRIMITIVE_TOPOLOGY_PATCH 5 /**< Patch list for tessellation. */
#define PAL_PRIMITIVE_TOPOLOGY_COUNT 6 /**< Number of primitive topologies. */
/** @} */

/**
 * @defgroup cull_modes Cull Modes
 * @ingroup pal_graphics
 * @{
 */
#define PAL_CULL_MODE_NONE 0 /**< No faces are culled. */
#define PAL_CULL_MODE_FRONT 1 /**< Front faces are culled. */
#define PAL_CULL_MODE_BACK 2 /**< Back faces are culled. */
#define PAL_CULL_MODE_COUNT 3 /**< Number of cull modes. */
/** @} */

/**
 * @defgroup front_faces Front Faces
 * @ingroup pal_graphics
 * @{
 */
#define PAL_FRONT_FACE_CLOCKWISE 0 /**< Clockwise triangles are front facing. */
#define PAL_FRONT_FACE_COUNTER_CLOCKWISE 1 /**< Counter-clockwise triangles are front facing. */
#define PAL_FRONT_FACE_COUNT 2 /**< Number of front faces. */
/** @} */

/**
 * @defgroup polygon_modes Polygon Modes
 * @ingroup pal_graphics
 * @{
 */
#define PAL_POLYGON_MODE_FILL 0 /**< Fill polygons. */
#define PAL_POLYGON_MODE_LINE 1 /**< Draw polygon edges as lines (wireframe). */
#define PAL_POLYGON_MODE_COUNT 2 /**< Number of polygon modes. */
/** @} */

/**
 * @defgroup stencil_face_flags Stencil Face Flags
 * @ingroup pal_graphics
 * @{
 */
#define PAL_STENCIL_FACE_FLAG_FRONT (1U << 0) /**< Front faces. */
#define PAL_STENCIL_FACE_FLAG_BACK (1U << 1) /**< Back faces. */
#define PAL_STENCIL_FACE_FLAG_BOTH (PAL_STENCIL_FACE_FLAG_FRONT | PAL_STENCIL_FACE_FLAG_BACK) /**< Both faces. */
/** @} */

/**
 * @defgroup vertex_types Vertex Types
 * @ingroup pal_graphics
 * @{
 */
#define PAL_VERTEX_TYPE_UNDEFINED 0 /**< Invalid vertex type. */
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
#define PAL_VERTEX_TYPE_COUNT 31 /**< Number of vertex types. */
/** @} */

/**
 * @defgroup vertex_semantic_ids Vertex Semantic IDs
 * @ingroup pal_graphics
 * @{
 */
#define PAL_VERTEX_SEMANTIC_ID_POSITION 0 /**< Vertex position. */
#define PAL_VERTEX_SEMANTIC_ID_COLOR 1 /**< Vertex color. */
#define PAL_VERTEX_SEMANTIC_ID_TEXCOORD 2 /**< Texture coordinates. */
#define PAL_VERTEX_SEMANTIC_ID_NORMAL 3 /**< Vertex normal. */
#define PAL_VERTEX_SEMANTIC_ID_TANGENT 4 /**< Vertex tangent. */
#define PAL_VERTEX_SEMANTIC_ID_COUNT 5 /**< Number of vertex semantic identifiers. */
/** @} */

/**
 * @defgroup vertex_layout_types Vertex Layout Types
 * @ingroup pal_graphics
 * @{
 */
#define PAL_VERTEX_LAYOUT_TYPE_PER_VERTEX 0 /**< Attributes advance once per vertex. */
#define PAL_VERTEX_LAYOUT_TYPE_PER_INSTANCE 1 /**< Attributes advance once per instance. */
#define PAL_VERTEX_LAYOUT_TYPE_COUNT 2 /**< Number of vertex layout types. */
/** @} */

/**
 * @defgroup compare_ops Compare Operations
 * @ingroup pal_graphics
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
#define PAL_COMPARE_OP_COUNT 8 /**< Number of compare operations. */
/** @} */

/**
 * @defgroup stencil_ops Stencil Operations
 * @ingroup pal_graphics
 * @{
 */
#define PAL_STENCIL_OP_KEEP 0 /**< Keep the current value. */
#define PAL_STENCIL_OP_ZERO 1 /**< Set the value to `0`. */
#define PAL_STENCIL_OP_REPLACE 2 /**< Replace the value with the reference value. */
#define PAL_STENCIL_OP_INCREMENT_AND_CLAMP 3 /**< Increment the value and clamp it to the maximum. */
#define PAL_STENCIL_OP_DECREMENT_AND_CLAMP 4 /**< Decrement the value and clamp it to `0`. */
#define PAL_STENCIL_OP_INVERT 5 /**< Bitwise invert the value. */
#define PAL_STENCIL_OP_INCREMENT_AND_WRAP 6 /**< Increment the value and wrap to `0` on overflow. */
#define PAL_STENCIL_OP_DECREMENT_AND_WRAP 7 /**< Decrement the value and wrap to the maximum on underflow. */
#define PAL_STENCIL_OP_COUNT 8 /**< Number of stencil operations. */
/** @} */

/**
 * @defgroup blend_ops Blend Operations
 * @ingroup pal_graphics
 * @{
 */
#define PAL_BLEND_OP_ADD 0
#define PAL_BLEND_OP_SUBTRACT 1
#define PAL_BLEND_OP_REVERSE_SUBTRACT 2
#define PAL_BLEND_OP_MIN 3
#define PAL_BLEND_OP_MAX 4
#define PAL_BLEND_OP_COUNT 5 /**< Number of blend operations. */
/** @} */

/**
 * @defgroup blend_factors Blend Factors
 * @ingroup pal_graphics
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
#define PAL_BLEND_FACTOR_COUNT 14 /**< Number of blend factors. */
/** @} */

/**
 * @defgroup color_masks Color Masks
 * @ingroup pal_graphics
 * @{
 */
#define PAL_COLOR_MASK_NONE 0 /**< No color mask. */
#define PAL_COLOR_MASK_RED (1U << 0) /**< Red channel. */
#define PAL_COLOR_MASK_GREEN (1U << 1) /**< Green channel. */
#define PAL_COLOR_MASK_BLUE (1U << 2) /**< Blue channel. */
#define PAL_COLOR_MASK_ALPHA (1U << 3) /**< Alpha channel. */
/** @} */

/**
 * @defgroup resolve_modes Resolve Modes
 * @ingroup pal_graphics
 * @{
 */
#define PAL_RESOLVE_MODE_NONE 0 /**< No resolve mode. */
#define PAL_RESOLVE_MODE_SAMPLE_ZERO 1 /**< Use sample zero. */
#define PAL_RESOLVE_MODE_AVERAGE 2 /**< Use the average of the samples. */
#define PAL_RESOLVE_MODE_MIN 3 /**< Use the minimum of the samples. */
#define PAL_RESOLVE_MODE_MAX 4 /**< Use the maximum of the samples. */
#define PAL_RESOLVE_MODE_COUNT 5 /**< Number of resolve modes. */
/** @} */

/**
 * @defgroup fsrs Fragment Shading Rates
 * @ingroup pal_graphics
 * @{
 */
#define PAL_FRAGMENT_SHADING_RATE_1X1 0
#define PAL_FRAGMENT_SHADING_RATE_1X2 1
#define PAL_FRAGMENT_SHADING_RATE_2X1 2
#define PAL_FRAGMENT_SHADING_RATE_2X2 3
#define PAL_FRAGMENT_SHADING_RATE_2X4 4
#define PAL_FRAGMENT_SHADING_RATE_4X2 5
#define PAL_FRAGMENT_SHADING_RATE_4X4 6
#define PAL_FRAGMENT_SHADING_RATE_COUNT 7 /**< Number of fragment shading rates. */
/** @} */

/**
 * @defgroup fsr_combiner_ops Fragment Shading Rate Combiner Operations
 * @ingroup pal_graphics
 * @{
 */
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP 0 /**< Keep the first operand. */
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE 1 /**< Use the second operand. */
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN 2 /**< Use the minimum of the operands. */
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX 3 /**< Use the maximum of the operands. */
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL 4 /**< Use the product of the operands. */
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_COUNT 5 /**< Number of fragment shading rate combiner operations. */
/** @} */

/**
 * @defgroup index_types Index Types
 * @ingroup pal_graphics
 * @{
 */
#define PAL_INDEX_TYPE_UINT16 0 /**< 16-bit indices. */
#define PAL_INDEX_TYPE_UINT32 1 /**< 32-bit indices. */
#define PAL_INDEX_TYPE_COUNT 2 /**< Number of index types. */
/** @} */

/**
 * @defgroup usage_states Usage States
 * @ingroup pal_graphics
 * @{
 */
#define PAL_USAGE_STATE_UNDEFINED 0 /**< Invalid usage state. */
#define PAL_USAGE_STATE_PRESENT 1 /**< Presenting to a surface. */
#define PAL_USAGE_STATE_COLOR_ATTACHMENT_WRITE 2 /**< Writing as a color attachment. */
#define PAL_USAGE_STATE_DEPTH_ATTACHMENT_READ 3 /**< Reading as a depth attachment. */
#define PAL_USAGE_STATE_DEPTH_ATTACHMENT_WRITE 4 /**< Writing as a depth attachment. */
#define PAL_USAGE_STATE_STENCIL_ATTACHMENT_READ 5 /**< Reading as a stencil attachment. */
#define PAL_USAGE_STATE_STENCIL_ATTACHMENT_WRITE 6 /**< Writing as a stencil attachment. */
#define PAL_USAGE_STATE_FRAGMENT_SHADING_RATE_ATTACHMENT_READ 7 /**< Reading as a fragment shading rate attachment. */
#define PAL_USAGE_STATE_TRANSFER_READ 8 /**< Reading in a transfer operation. */
#define PAL_USAGE_STATE_TRANSFER_WRITE 9 /**< Writing in a transfer operation. */
#define PAL_USAGE_STATE_VERTEX_READ 10 /**< Reading as a vertex buffer. */
#define PAL_USAGE_STATE_INDEX_READ 11 /**< Reading as an index buffer. */
#define PAL_USAGE_STATE_INDIRECT_READ 12 /**< Reading as indirect arguments. */
#define PAL_USAGE_STATE_UNIFORM_READ 13 /**< Reading as a uniform buffer. */
#define PAL_USAGE_STATE_SHADER_READ 14 /**< Reading in a shader. */
#define PAL_USAGE_STATE_SHADER_WRITE 15 /**< Writing in a shader. */
#define PAL_USAGE_STATE_STORAGE_READ 16 /**< Reading as a storage resource. */
#define PAL_USAGE_STATE_STORAGE_WRITE 17 /**< Writing as a storage resource. */
#define PAL_USAGE_STATE_HOST_READ 18 /**< Reading from the CPU. */
#define PAL_USAGE_STATE_HOST_WRITE 19 /**< Writing from the CPU. */
#define PAL_USAGE_STATE_ACCELERATION_STRUCTURE_READ 20 /**< Reading as an acceleration structure. */
#define PAL_USAGE_STATE_ACCELERATION_STRUCTURE_WRITE 21 /**< Writing as an acceleration structure. */
#define PAL_USAGE_STATE_COUNT 22 /**< Number of usage states. */
/** @} */

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
 * @defgroup rendering_flags Rendering Flags
 * @ingroup pal_graphics
 * @{
 */
#define PAL_RENDERING_FLAG_NONE 0 /**< No rendering flags. */
#define PAL_RENDERING_FLAG_SUSPENDING (1U << 0) /**< The rendering pass is suspended at its end. */
#define PAL_RENDERING_FLAG_RESUMING (1U << 1) /**< The rendering pass resumes a suspended pass. */
/** @} */

#endif // PAL_GRAPHICS_H