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

#ifndef PAL_GRAPHICS_CMDBUFFER_H
#define PAL_GRAPHICS_CMDBUFFER_H

#include "descriptor.h"
#include "pipeline.h"
#include "SBT.h"

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
 * @brief Set the front face for the provided command buffer.
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

#endif // PAL_GRAPHICS_CMDBUFFER_H