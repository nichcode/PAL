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

#ifndef PAL_GRAPHICS_TYPES_H
#define PAL_GRAPHICS_TYPES_H

#include "pal2/core/defines.h"

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

#endif // PAL_GRAPHICS_TYPES_H