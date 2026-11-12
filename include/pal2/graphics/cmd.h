/**
 * @file cmd.h
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

#ifndef PAL_GRAPHICS_CMD_H
#define PAL_GRAPHICS_CMD_H

#include "pal2/core/base.h"
#include "pal2/core/result.h"
#include "device.h"
#include "cmdbuffer.h"
#include "image.h"
#include "buffer.h"
#include "buffer.h"
#include "pipeline.h"
#include "queue.h"

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
 * @defgroup load_ops Load Operations
 * @ingroup pal_graphics
 * @{
 */
#define PAL_LOAD_OP_LOAD 0 /**< Preserve the existing contents of the attachment. */
#define PAL_LOAD_OP_CLEAR 1 /**< Clear the attachment to the clear value. */
#define PAL_LOAD_OP_DONT_CARE 2 /**< The existing contents of the attachment are undefined. */
#define PAL_LOAD_OP_COUNT 3 /**< Number of load operations. */
/** @} */

/**
 * @defgroup store_ops Store Operations
 * @ingroup pal_graphics
 * @{
 */
#define PAL_STORE_OP_STORE 0 /**< Store the rendered contents of the attachment. */
#define PAL_STORE_OP_DONT_CARE 1 /**< The rendered contents of the attachment may be discarded. */
#define PAL_STORE_OP_COUNT 2 /**< Number of store operations. */
/** @} */

/**
 * @struct PalClearValue
 * @brief Clear values.
 *
 * If used with a color attachment, the color values will be used and depth
 * and stencil will be used with depth stencil attachments.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalClearValue {
    float color[4];   /**< Color clear value. */
    float depth;      /**< Depth clear value. */
    uint32_t stencil; /**< Stencil clear value. */
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
    PalImageView* imageView;           /**< Image view to render to. */
    PalImageView* resolveImageView;    /**< Resolve image view, or `nullptr` if the attachment is not resolved. */
    PalLoadOp loadOp;                  /**< Color or depth load operation. */
    PalStoreOp storeOp;                /**< Color or depth store operation. */
    PalLoadOp stencilLoadOp;           /**< Stencil load operation. */
    PalStoreOp stencilStoreOp;         /**< Stencil store operation. */
    PalResolveMode resolveMode;        /**< Color or depth resolve mode. */
    PalResolveMode stencilResolveMode; /**< Stencil resolve mode. */
    PalClearValue clearValue;          /**< Attachment clear value. */
} PalAttachmentDesc;

/**
 * @struct PalViewport
 * @brief Viewport.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalViewport {
    float x;        /**< X position in pixels. */
    float y;        /**< Y position in pixels. */
    float width;    /**< Width in pixels. */
    float height;   /**< Height in pixels. */
    float minDepth; /**< Minimum depth value. */
    float maxDepth; /**< Maximum depth value. */
} PalViewport;

/**
 * @struct PalRect2D
 * @brief Rectangle.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalRect2D {
    int32_t x;       /**< X position in pixels. */
    int32_t y;       /**< Y position in pixels. */
    uint32_t width;  /**< Width in pixels. */
    uint32_t height; /**< Height in pixels. */
} PalRect2D;

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
    PalAttachmentDesc* colorAttachments;        /**< Color attachments. */
    PalAttachmentDesc* depthStencilAttachment;  /**< Depth/stencil attachment. */
    PalImageView* fragmentShadingRateImageView; /**< Fragment shading rate image view. */
    PalRect2D renderArea;                       /**< Rendering area. */
    PalRenderingFlags flags;                    /**< Rendering flags. */
    uint32_t fragmentShadingRateTexelWidth;     /**< Texel width for fragment shading rate. */
    uint32_t fragmentShadingRateTexelHeight;    /**< Texel height for fragment shading rate. */
    uint32_t viewCount;                         /**< Number of views. */
    uint32_t arrayLayerCount;                   /**< Number of array layers. */
    uint32_t colorAttachentCount;               /**< Number of color attachments. */
} PalRenderingInfo;

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
    uint32_t vertexCount;   /**< Vertex count. */
    uint32_t instanceCount; /**< Instance count. */
    uint32_t firstVertex;   /**< First vertex. */
    uint32_t firstInstance; /**< First instance. */
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
    uint32_t indexCount;    /**< Index count. */
    uint32_t instanceCount; /**< Instance count. */
    uint32_t firstIndex;    /**< First index. */
    int32_t vertexOffset;   /**< Vertex offset. */
    uint32_t firstInstance; /**< First instance. */
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
    uint32_t groupCountXOrWidth;  /**< Number of groups on the x dimension or dispatch width. */
    uint32_t groupCountXOrHeight; /**< Number of groups on the y dimension or dispatch height. */
    uint32_t groupCountXOrDepth;  /**< Number of groups on the z dimension or dispatch depth. */
} PalDispatchIndirectData;

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
    PalUsageState oldState;      /**< Old usage state of resource. */
    PalUsageState newState;      /**< New usage state of resource. */
    PalPipelineStages srcStages; /**< Source pipeline stages of resource. */
    PalPipelineStages dstStages; /**< Destination pipeline stages of resource. */
} PalBarrierInfo;

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
    uint32_t workCount[3];      /**< Workload per dimension. */
    uint32_t workGroupSize[3];  /**< Threads per workgroup per axis of the adapter. */
    uint32_t workGroupCount[3]; /**< Workgroups per axis of the adapter. */
} PalWorkGroupBuildData;

/**
 * @struct PalWorkGroupInfo
 * @brief Workgroup dispatch information.
 *
 * Filled in by @ref palBuildWorkGroupInfo().
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalWorkGroupInfo {
    uint32_t workGroupBase[3];  /**< Offset per dimension of a dispatch tile. */
    uint32_t workGroupCount[3]; /**< Workgroup count per dimension of a dispatch tile. */
} PalWorkGroupInfo;

/**
 * @brief Begins recording commands to the provided command buffer.
 *
 * This function must be called before any other `palCmd**` function is used.
 *
 * @param[in] cmdBuffer Command buffer to begin recording.
 * @param[in] info Rendering layout information for the command buffer.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_PLATFORM_FAILURE
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
 * @brief Ends recording commands to the provided command buffer.
 *
 * @param[in] cmdBuffer Command buffer to begin recording.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCmdBegin
 */
PAL_API PalResult PAL_CALL palCmdEnd(PalCommandBuffer* cmdBuffer);

/**
 * @brief Executes a secondary command buffer within a primary command buffer.
 *
 * The `secondaryCmdBuffer` must be created with the type `PAL_COMMAND_BUFFER_TYPE_SECONDARY`.
 *
 * @param[in] primaryCmdBuffer Primary command buffer. Must be in recording state.
 * @param[in] secondaryCmdBuffer Secondary command buffer.
 *
 * @thread-safety `primaryCmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdExecuteCommandBuffer(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer);

/**
 * @brief Sets the fragment shading rate used for draw calls.
 *
 * `PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] state Pointer to a PalFragmentShadingRateState struct that specifies parameters.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdSetFragmentShadingRate(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state);

/**
 * @brief Dispatches mesh shader workgroups.
 *
 * `PAL_ADAPTER_FEATURE_MESH_SHADER` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] groupCountX Number of mesh shader groups to dispatch on the x axis.
 * @param[in] groupCountY Number of mesh shader groups to dispatch on the y axis.
 * @param[in] groupCountZ Number of mesh shader groups to dispatch on the z axis.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Dispatches mesh shader workgroups using parameters from a buffer.
 *
 * `PAL_ADAPTER_FEATURE_MESH_SHADER` and `PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH` must be supported
 * and enabled by the device. Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDispatchIndirectData structs.
 * @param[in] drawCount Number of draws to perform.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Dispatches mesh shader workgroups using parameters from buffers.
 *
 * `PAL_ADAPTER_FEATURE_MESH_SHADER` and `PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT` must be
 * supported and enabled by the device. Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDispatchIndirectData structs.
 * @param[in] countBuffer Buffer containing a single `uint32_t` specifying the number of draws.
 * @param[in] maxDrawCount Maximum Number of draws to perform.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Builds or update an acceleration structure.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] info Pointer to a PalAccelerationStructureBuildInfo struct that specifies parameters.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdBuildAccelerationStructure(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info);

/**
 * @brief Begins a rendering pass.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] info Pointer to a PalRenderingInfo struct that specifies parameters.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdBeginRendering(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info);

/**
 * @brief Ends a rendering pass.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdEndRendering(PalCommandBuffer* cmdBuffer);

/**
 * @brief Copies data from one buffer to the other.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] dst Destination buffer.
 * @param[in] src Source buffer.
 * @param[in] copyInfo Pointer to a PalBufferCopyInfo struct that specifies parameters.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Copies data from a buffer to an image.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] dstImage Destination image.
 * @param[in] srcBuffer Source buffer.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Copies data from one image to the other.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] dst Destination image.
 * @param[in] src Source image.
 * @param[in] copyInfo Pointer to a PalImageCopyInfo struct that specifies parameters.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Copies data from an image to a buffer.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] dstBuffer Destination buffer.
 * @param[in] srcImage Source image.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Binds a pipeline.
 *
 * Every pipeline knows it types which is set at the respective creation functions.
 * (`palCreate**Graphics/Compute/RayTracing**Pipeline`).
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] pipeline Pipeline to bind.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdBindPipeline(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline);

/**
 * @brief Sets the viewport(s) used in draw commands.
 *
 * This always overwrites any previous viewports that were set since the first viewport
 * index is always 0.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] count Capacity of the PalViewport array.
 * @param[in] viewports Pointer to an array of viewports.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdSetViewport(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalViewport* viewports);

/**
 * @brief Sets the scissor(s) used in draw commands.
 *
 * This always overwrites any previous scissors that were set since the first
 * scissor index is always 0.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] count Capacity of the PalRect2D array.
 * @param[in] scissors Pointer to an array of scissors.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdSetScissors(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalRect2D* scissors);

/**
 * @brief Binds vertex buffer(s) used in draw commands.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] firstSlot Index of the first vertex buffer binding slot.
 * @param[in] count Number of vertex buffers to bind.
 * @param[in] buffers Pointer to an array of vertex buffers.
 * @param[in] offsets Pointer to an array of offsets in bytes into each vertex buffer.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Binds index buffer used in draw commands.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Index buffer to bind.
 * @param[in] offset Offset in bytes into the index buffer.
 * @param[in] type Type of indices stored in the index buffer. (eg. `PAL_INDEX_TYPE_UINT32`).
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Issues a non-indexed draw command.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] vertexCount Number of vertices to draw.
 * @param[in] instanceCount Number of instances to draw.
 * @param[in] firstVertex Index of the first vertex to draw.
 * @param[in] firstInstance Index of the first instance to draw.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCmdDrawIndexed
 */
PAL_API void PAL_CALL palCmdDraw(
    PalCommandBuffer* cmdBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance);

/**
 * @brief Issues a non-indexed draw command using buffers.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DRAW` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDrawIndirectData structs.
 * Can be a single struct.
 * @param[in] count Number of draws to perform.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCmdDrawIndexedIndirect
 */
PAL_API void PAL_CALL palCmdDrawIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count);

/**
 * @brief Issues a non-indexed draw command using buffers.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDrawIndirectData structs.
 * @param[in] countBuffer Buffer containing a single `uint32_t` specifying the number of draws.
 * @param[in] maxDrawCount Maximum Number of draws to perform.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Issues an indexed draw command.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] indexCount Number of indices to draw.
 * @param[in] instanceCount Number of instances to draw.
 * @param[in] firstIndex Index of the first index to draw.
 * @param[in] vertexOffset Added offset to vertex indices.
 * @param[in] firstInstance Index of the first instance to draw.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Issues an indexed draw command using buffers.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DRAW` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDrawIndexedIndirectData structs.
 * @param[in] count Number of draws to perform.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Issues an indexed draw command using buffers.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDrawIndexedIndirectData structs.
 * @param[in] countBuffer Buffer containing a single `uint32_t` specifying the number of draws.
 * @param[in] maxDrawCount Maximum Number of draws to perform.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Transitions an acceleration structure from one usage state to another.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * This function defines a dependency between `PalBarrierInfo::oldState` and
 * `PalBarrierInfo::newState`. It ensures that all operations performed under the old
 * `PalBarrierInfo::oldState` are completed and visible before the acceleration structure
 * is accessed under `PalBarrierInfo::newState`.
 *
 * This function does not modify the acceleration structure, it only enforces execution ordering
 * and acceleration structure memory visibility.
 *
 * A barrier is not needed between a BLAS and a TLAS if they do not share any resources. If both
 * builds use a different scratch buffer, no barrier is needed.
 *
 * Example:
 *
 * To make sure BLAS builds before TLAS access it and TLAS does not use scratch buffer
 * while BLAS is building, we put a barrier to transition the BLAS to ensure it has finished
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
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Transitions an image from one usage state to another.
 *
 * This function defines a dependency between `PalBarrierInfo::oldState` and
 * `PalBarrierInfo::newState`. It ensures that all operations performed under
 * `PalBarrierInfo::oldState` are completed and visible before the image is accessed under
 * `PalBarrierInfo::newState`.
 *
 * This function does not modify the image, it only enforces execution ordering and image memory
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
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Transitions an image from one usage state to another across queues,
 * optionally transferring ownership.
 *
 * The adapter used to create the command buffer's device must support
 * `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2` or later.
 *
 * The source command buffer releases ownership of the image after `srcUsageState` and optionally
 * `srcPipelineStages` operations are completed. The destination command buffer acquires the
 * image and transitions it to `PAL_USAGE_STATE_UNDEFINED` and `PAL_PIPELINE_STAGE_NONE` as the
 * default state. The image must be transitioned to the proper state before it is used by
 * the destination command buffer after this call.
 *
 * Both command buffers must not be able to share resource ownership otherwise, this function
 * sets a normal barrier on the source command buffer ignoring the destination buffer. Call
 * @ref palCanQueueShareOwnership() to check if both command buffer queues can share resources.
 *
 * @param[in] srcCmdBuffer Source command buffer being recorded.
 * @param[in] dstCmdBuffer Destination command buffer being recorded.
 * @param[in] image Image to set barrier on and transfer ownership.
 * @param[in] subresourceRange Subresource range of the image.
 * @param[in] srcUsageState Usage state of the image on the source command buffer.
 * @param[in] srcPipelineStages Source pipeline stages.
 *
 * @thread-safety `srcCmdBuffer` and `dstCmdBuffer` must be externally synchronized.
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
 * @brief Transitions a buffer from one usage state to another.
 *
 * This function defines a dependency between `PalBarrierInfo::oldState` and
 * `PalBarrierInfo::newState`. It ensures that all operations performed under
 * `PalBarrierInfo::oldState` are completed and visible before the buffer is accessed
 * under `PalBarrierInfo::newState`.
 *
 * This function does not modify the buffer, it only enforces execution ordering and buffer memory
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
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Transitions a buffer from one usage state to another across queues,
 * optionally transferring ownership.
 *
 * The adapter used to create the command buffer's device must support
 * `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2` or later.
 *
 * The source command buffer releases ownership of the buffer after `srcUsageState` and optionally
 * `srcPipelineStages` operations are completed. The destination command buffer acquires the
 * buffer and transitions it to `PAL_USAGE_STATE_UNDEFINED` and `PAL_PIPELINE_STAGE_NONE` as the
 * default state. The buffer must be transitioned to the proper state before it is used by
 * the destination command buffer after this call.
 *
 * Both command buffers must not be able to share resource ownership otherwise, this function
 * sets a normal barrier on the source command buffer ignoring the destination buffer. Call
 * @ref palCanQueueShareOwnership() to check if both command buffer queues can share resources.
 *
 * @param[in] srcCmdBuffer Source command buffer being recorded.
 * @param[in] dstCmdBuffer Destination command buffer being recorded.
 * @param[in] buffer Buffer to set barrier on and transfer ownership.
 * @param[in] srcUsageState Usage state of the buffer on the source command buffer.
 * @param[in] srcPipelineStages Source pipeline stages.
 *
 * @thread-safety `srcCmdBuffer` and `dstCmdBuffer` must be externally synchronized.
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
 * @brief Dispatches compute shader workgroups.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] groupCountX Number of compute shader groups to dispatch on the x axis.
 * @param[in] groupCountY Number of compute shader groups to dispatch on the y axis.
 * @param[in] groupCountZ Number of compute shader groups to dispatch on the z axis.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Dispatches compute shader workgroups with base offset.
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
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Dispatches compute shader workgroups using parameters from a buffer.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DISPATCH` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing the PalDispatchIndirectData struct.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Dispatches rays.
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
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Dispatches rays using parameters from a buffer.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_RAY_TRACING` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] raygenIndex Index of the raygen shader to execute.
 * @param[in] sbt The shader binding table to use.
 * @param[in] buffer Buffer containing the PalDispatchIndirectData struct.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @note The argument buffer memory must not be `PAL_MEMORY_TYPE_CPU_UPLOAD`. The implementation
 * internally copies the data into a GPU buffer for execution.
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
 * @brief Binds a descriptor set to the provided command buffer.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] setIndex Index of the descriptor set to bind.
 * @param[in] set Descriptor set to bind. Must be compatible with the pipeline layout of the bound pipeline.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Updates push constant data for the provided command buffer.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] offset Offset in bytes into the push constant range.
 * @param[in] size Size of `value` in bytes.
 * @param[in] value Pointer to the push constant range data to write.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Sets the cull mode for the provided command buffer.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] cullMode Cull mode to set.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdSetCullMode(
    PalCommandBuffer* cmdBuffer,
    PalCullMode cullMode);

/**
 * @brief Sets the front face for the provided command buffer.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] frontFace Front face to set.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdSetFrontFace(
    PalCommandBuffer* cmdBuffer,
    PalFrontFace frontFace);

/**
 * @brief Sets the primitive topology for the provided command buffer.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] topology Topology to set.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdSetPrimitiveTopology(
    PalCommandBuffer* cmdBuffer,
    PalPrimitiveTopology topology);

/**
 * @brief Sets depth test enable for the provided command buffer.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] enable True to enable.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdSetDepthTestEnable(
    PalCommandBuffer* cmdBuffer,
    PalBool enable);

/**
 * @brief Sets depth write enable for the provided command buffer.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE` must be supported and enabled by the device.
 * Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] enable True to enable.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API void PAL_CALL palCmdSetDepthWriteEnable(
    PalCommandBuffer* cmdBuffer,
    PalBool enable);

/**
 * @brief Sets depth stencil operation for the provided command buffer.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP` must be supported and enabled by the device.
 *  Otherwise behavior is undefined.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] faceMask Bitmask specifying faces to apply the stencil to.
 * @param[in] failOp Stencil operation to perform when stencil fails.
 * @param[in] passOp Stencil operation to perform when stencil and depth passes.
 * @param[in] depthFailOp Stencil operation to perform when stencil passes but depth fails.
 * @param[in] compareOp Compare operation for stencil tests.
 *
 * @thread-safety `cmdBuffer` must be externally synchronized.
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
 * @brief Builds work group info(s) from work inputs specified in pixels, vertices etc.
 *
 * This function does the math for how many work groups to dispatch on each axis, and how many times it needs
 * to be dispatched for the work to be done. It works well with @ref palCmdDispatchBase() since it also gives
 * the base of each work group.
 *
 * This is a two-call function, set `infos` to `nullptr` to get the number of work group infos. Allocate the
 * array and call this function again with `infos` set to the allocated array and `count` set to the capacity
 * of the array.
 *
 * If the specified count is less than the number of work group infos, only the work group infos that fit in
 * the array will be written. If `count` is `0` and `infos` is `nullptr`, the function fails.
 *
 * @param[in] data Pointer to a PalWorkGroupBuildData with parameters.
 * @param[in, out] count Capacity of the work group info array.
 * @param[out] infos Output array to receive the work group infos, or `nullptr` to get the number of work group infos.
 *
 * @thread-safety Must be called from the main thread.
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

#endif // PAL_GRAPHICS_CMD_H