/**
 * @file next.h
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

#ifndef PAL_GRAPHICS_NEXT_H
#define PAL_GRAPHICS_NEXT_H

#include "types.h"

/**
 * @brief Marks a shader index as unused.
 * @ingroup pal_graphics
 */
#define PAL_UNUSED_SHADER_INDEX UINT32_MAX

/**
 * @defgroup pipeline_stages Pipeline Stages
 * @ingroup pal_graphics
 * @{
 */
#define PAL_PIPELINE_STAGE_NONE 0 /**< No pipeline stages. */
#define PAL_PIPELINE_STAGE_VERTEX_SHADER (1U << 1) /**< Vertex shader stage. */
#define PAL_PIPELINE_STAGE_FRAGMENT_SHADER (1U << 2) /**< Fragment shader stage. */
#define PAL_PIPELINE_STAGE_COMPUTE_SHADER (1U << 3) /**< Compute shader stage. */
#define PAL_PIPELINE_STAGE_GEOMETRY_SHADER (1U << 4) /**< Geometry shader stage. */
#define PAL_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER (1U << 5) /**< Tessellation control shader stage. */
#define PAL_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER (1U << 6) /**< Tessellation evaluation shader stage. */
#define PAL_PIPELINE_STAGE_RAY_TRACING_SHADER (1U << 7) /**< Ray tracing shader stages. */
#define PAL_PIPELINE_STAGE_TASK_SHADER (1U << 8) /**< Task shader stage. */
#define PAL_PIPELINE_STAGE_MESH_SHADER (1U << 9) /**< Mesh shader stage. */
#define PAL_PIPELINE_STAGE_VERTEX_INPUT (1U << 10) /**< Vertex input stage. */
#define PAL_PIPELINE_STAGE_INDEX_INPUT (1U << 11) /**< Index input stage. */
#define PAL_PIPELINE_STAGE_EARLY_DEPTH_STENCIL (1U << 12) /**< Early depth/stencil test stage. */
#define PAL_PIPELINE_STAGE_LATE_DEPTH_STENCIL (1U << 13) /**< Late depth/stencil test stage. */
#define PAL_PIPELINE_STAGE_TRANSFER (1U << 14) /**< Transfer (copy) stage. */
#define PAL_PIPELINE_STAGE_HOST (1U << 15) /**< CPU access stage. */
#define PAL_PIPELINE_STAGE_COLOR_ATTACHMENT (1U << 16) /**< Color attachment output stage. */
#define PAL_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT (1U << 17) /**< Fragment shading rate attachment stage. */
#define PAL_PIPELINE_STAGE_INDIRECT_INPUT (1U << 18) /**< Indirect argument input stage. */
#define PAL_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD (1U << 19) /**< Acceleration structure build stage. */
/** @} */

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
 * @result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT,
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY,
 *               @ref PAL_RESULT_CODE_PLATFORM_FAILURE.
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
 * @result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT,
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY,
 *               @ref PAL_RESULT_CODE_PLATFORM_FAILURE.
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
 * @result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT,
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY,
 *               @ref PAL_RESULT_CODE_PLATFORM_FAILURE.
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
 * @result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT,
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY,
 *               @ref PAL_RESULT_CODE_PLATFORM_FAILURE.
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

#endif // PAL_GRAPHICS_NEXT_H