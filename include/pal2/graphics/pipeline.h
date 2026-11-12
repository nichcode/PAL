/**
 * @file pipeline.h
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

#ifndef PAL_GRAPHICS_PIPELINE_H
#define PAL_GRAPHICS_PIPELINE_H

#include "pal2/core/base.h"
#include "pal2/core/result.h"
#include "sampler.h"
#include "shader.h"
#include "queue.h"

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
 * @brief Marks a shader index as unused.
 * @ingroup pal_graphics
 */
#define PAL_UNUSED_SHADER_INDEX UINT32_MAX

/**
 * @defgroup rt_shader_group_types Ray Tracing Shader Group Types
 * @ingroup pal_graphics
 * @{
 */
#define PAL_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL 0 /**< General group (raygen, miss or callable shader). */
#define PAL_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT 1 /**< Procedural hit group (with an intersection shader). */
#define PAL_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT 2 /**< Triangles hit group. */
#define PAL_RAY_TRACING_SHADER_GROUP_TYPE_COUNT 3 /**< Number of ray tracing shader group types. */
/** @} */

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
 * @defgroup rendering_flags Rendering Flags
 * @ingroup pal_graphics
 * @{
 */
#define PAL_RENDERING_FLAG_NONE 0 /**< No rendering flags. */
#define PAL_RENDERING_FLAG_SUSPENDING (1U << 0) /**< The rendering pass is suspended at its end. */
#define PAL_RENDERING_FLAG_RESUMING (1U << 1) /**< The rendering pass resumes a suspended pass. */
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
    PalFormat* colorAttachmentsFormat;             /**< Color attachments formats. */
    uint32_t colorAttachentCount;                  /**< Number of color attachment formats. */
    uint32_t viewCount;                            /**< Number of views. */
    PalSampleCount sampleCount;                    /**< Number of samples per pixel. */
    PalRenderingFlags flags;                       /**< Rendering flags. */
    PalFormat depthStencilAttachmentFormat;        /**< Depth/stencil attachment format. */
    PalFormat fragmentShadingRateAttachmentFormat; /**< Fragment shading rate attachment format. */
} PalRenderingLayoutInfo;

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
    PalVertexSemanticID semanticID; /**< Vertex attribute semantic identifier. */
    PalVertexType type;             /**< Vertex attribute type. */
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
    PalVertexAttribute* attributes; /**< Vertex attributes. */
    uint32_t attributeCount;        /**< Number of vertex attributes. */
    PalVertexLayoutType type;       /**< Vertex layout type. */
    uint32_t binding;               /**< Vertex buffer binding slot. */
    uint32_t reserved;              /**< Must be set to `0`. */
} PalVertexLayout;

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
    PalBool enableDepthClamp;   /**< Whether to enable depth clamp. */
    PalBool enableDepthBias;    /**< Whether to enable depth bias. */
    float depthBiasConstant;    /**< Depth bias constant. */
    float depthBiasSlope;       /**< Depth bias slope. */
    float depthBiasClamp;       /**< Depth bias clamp. */
    PalPolygonMode polygonMode; /**< Polygon mode. */
    PalCullMode cullMode;       /**< Cull mode. */
    PalFrontFace frontFace;     /**< Front face. */
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
    uint64_t sampleMask;           /**< Sample mask, or `0` for the default. */
    PalBool enableSampleShading;   /**< Whether to enable sample shading. */
    PalBool enableAlphaToCoverage; /**< Whether to enable alpha to coverage. */
    PalSampleCount sampleCount;    /**< Number of samples per pixel. */
    float minSampleShading;        /**< Minimum sample shading. */
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
    PalStencilOp failOp;      /**< Stencil fail operation. */
    PalStencilOp passOp;      /**< Pass operation. */
    PalStencilOp depthFailOp; /**< Depth fail operation. */
    PalCompareOp compareOp;   /**< Compare operation. */
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
    PalBool enableDepthTest;               /**< Whether to enable depth test. */
    PalBool enableDepthWrite;              /**< Whether to enable depth write. */
    PalBool enableStencilTest;             /**< Whether to enable stencil test. */
    PalCompareOp compareOp;                /**< Compare operation. */
    PalStencilOpState frontStencilOpState; /**< Front stencil operation state. */
    PalStencilOpState backStencilOpState;  /**< Back stencil operation state. */
} PalDepthStencilState;

/**
 * @struct PalColorBlendAttachment
 * @brief Color blend attachment.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalColorBlendAttachment {
    PalBool enableBlend;                /**< Whether to enable blending. */
    PalColorMask colorWriteMask;        /**< Color write mask. */
    PalBlendFactor dstColorBlendFactor; /**< Destination color blend factor. */
    PalBlendFactor srcColorBlendFactor; /**< Source color blend factor. */
    PalBlendOp colorBlendOp;            /**< Color blend operation. */
    PalBlendFactor dstAlphaBlendFactor; /**< Destination alpha blend factor. */
    PalBlendFactor srcAlphaBlendFactor; /**< Source alpha blend factor. */
    PalBlendOp alphaBlendOp;            /**< Alpha blend operation. */
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
    PalFragmentShadingRate rate;                     /**< Shading rate. */
    PalFragmentShadingRateCombinerOp combinerOps[2]; /**< Combiner operations. */
} PalFragmentShadingRateState;

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
    uint32_t offset; /**< Offset in bytes. */
    uint32_t size;   /**< Size in bytes. */
} PalPushConstantInfo;

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
    PalDescriptorSetLayout** descriptorSetLayouts; /**< Descriptor set layouts. */
    PalPushConstantInfo pushConstantInfo;          /**< Push constant information. */
    uint32_t descriptorSetLayoutCount;             /**< Number of descriptor set layouts. */
    PalBool usePushConstant;                       /**< Whether to use push constants. */
} PalPipelineLayoutCreateInfo;

/**
 * @struct PalGraphicsPipelineCreateInfo
 * @brief Graphics pipeline creation parameters.
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
    PalPipelineLayout* pipelineLayout;                     /**< Pipeline layout. */
    PalShader** shaders;                                   /**< Shaders. */
    PalVertexLayout* vertexLayouts;                        /**< Vertex layouts. */
    PalColorBlendAttachment* colorBlendAttachments;        /**< Color blend attachments. */
    PalRasterizerState* rasterizerState;                   /**< Rasterizer state. */
    PalMultisampleState* multisampleState;                 /**< Multisample state. */
    PalDepthStencilState* depthStencilState;               /**< Depth stencil state. */
    PalFragmentShadingRateState* fragmentShadingRateState; /**< Fragment shading rate state. */
    PalRenderingLayoutInfo* renderingLayout;               /**< Rendering layout. */
    PalBool primitiveRestartEnable; /**< Whether to enable primitive restart for indexed draw. */
    uint32_t vertexLayoutCount;     /**< Number of vertex layouts. */
    uint32_t colorBlendAttachmentCount; /**< Number of color blend attachments. */
    uint32_t shaderCount;               /**< Number of shaders. */
    PalIndexType indexType;        /**< Primitive restart index type. */
    PalPrimitiveTopology topology; /**< Primitive topology. */
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
    PalPipelineLayout* pipelineLayout; /**< Pipeline layout. */
    PalShader* computeShader;          /**< Compute shader. */
} PalComputePipelineCreateInfo;

/**
 * @struct PalRayTracingShaderGroupCreateInfo
 * @brief Ray tracing shader group creation parameters.
 *
 * This struct is used only during @ref palCreateRayTracingPipeline() and may be
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
    PalRayTracingShaderGroupType type;     /**< Shader group type. */
    uint32_t anyHitShaderIndex;            /**< Index of the anyhit shader. */
    uint32_t anyHitShaderEntryIndex;       /**< Index of the anyhit shader entry. */
    uint32_t closestHitShaderIndex;        /**< Index of the closest hit shader. */
    uint32_t closestHitShaderEntryIndex;   /**< Index of the closest hit shader entry. */
    uint32_t generalShaderIndex;           /**< Index of the general shader. */
    uint32_t generalShaderEntryIndex;      /**< Index of the general shader entry. */
    uint32_t intersectionShaderIndex;      /**< Index of the intersection shader. */
    uint32_t intersectionShaderEntryIndex; /**< Index of the intersection shader entry. */
    uint32_t maxDataSize; /**< Size in bytes of extra data associated with the shader group. */
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
    PalPipelineLayout* pipelineLayout;                /**< Pipeline layout. */
    PalRayTracingShaderGroupCreateInfo* shaderGroups; /**< Shader groups. */
    PalShader** shaders;                              /**< Shaders. */
    uint32_t shaderGroupCount;                        /**< Number of shader groups. */
    uint32_t shaderCount;                             /**< Number of shaders. */
    uint32_t maxRecursionDepth;                       /**< Maximum number of ray recursion. */
    uint32_t maxAttributeSize;                        /**< Maximum attributes size in bytes. */
    uint32_t maxPayloadSize;                          /**< Maximum payload size in bytes. */
    uint32_t reserved;                                /**< Must be set to `0`. */
} PalRayTracingPipelineCreateInfo;

/**
 * @brief Creates a pipeline layout. This defines the descriptor set interfaces and push
 * constant info.
 *
 * The created pipeline layout must be destroyed using @ref palDestroyPipelineLayout().
 *
 * @param[in] device Device that creates the pipeline layout.
 * @param[in] info Pointer to a PalPipelineLayoutCreateInfo struct that specifies parameters.
 * @param[out] outLayout Pointer to a PalPipelineLayout to receive the created pipeline layout.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `device` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_PLATFORM_FAILURE
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
 * @brief Destroys a pipeline layout.
 *
 * @param[in] layout Pipeline layout to destroy.
 *
 * @thread-safety The device used to create `layout` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreatePipelineLayout
 */
PAL_API void PAL_CALL palDestroyPipelineLayout(PalPipelineLayout* layout);

/**
 * @brief Creates a graphics pipeline.
 *
 * The created pipeline must be destroyed using @ref palDestroyPipeline().
 *
 * @param[in] device Device that creates the graphics pipeline.
 * @param[in] info Pointer to a PalGraphicsPipelineCreateInfo struct that specifies parameters.
 * @param[out] outPipeline Pointer to a PalPipeline to receive the created buffer.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `device` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_PLATFORM_FAILURE
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
 * @brief Creates a compute pipeline.
 *
 * The created pipeline must be destroyed using @ref palDestroyPipeline().
 *
 * @param[in] device Device that creates the compute pipeline.
 * @param[in] info Pointer to a PalComputePipelineCreateInfo struct that specifies parameters.
 * @param[out] outPipeline Pointer to a PalPipeline to receive the created buffer.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `device` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_PLATFORM_FAILURE
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
 * @brief Creates a ray tracing pipeline.
 *
 * The created pipeline must be destroyed using @ref palDestroyPipeline().
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] device Device that creates the ray tracing pipeline.
 * @param[in] info Pointer to a PalRayTracingPipelineCreateInfo struct that specifies parameters.
 * @param[out] outPipeline Pointer to a PalPipeline to receive the created buffer.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `device` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_PLATFORM_FAILURE
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
 * @brief Destroys a pipeline.
 *
 * @param[in] pipeline Pipeline to destroy.
 *
 * @thread-safety The device used to create `pipeline` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateGraphicsPipeline
 * @sa palCreateComputePipeline
 * @sa palCreateRayTracingPipeline
 */
PAL_API void PAL_CALL palDestroyPipeline(PalPipeline* pipeline);

#endif // PAL_GRAPHICS_PIPELINE_H