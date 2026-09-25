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

#ifndef PAL_GRAPHICS_PIPELINE_H
#define PAL_GRAPHICS_PIPELINE_H

#include "pipeline_layout.h"
#include "shader.h"
#include "sampler.h"

#define PAL_UNUSED_SHADER_INDEX UINT32_MAX

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

#endif // PAL_GRAPHICS_PIPELINE_H