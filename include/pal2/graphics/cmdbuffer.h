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

#include "cmdpool.h"

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
 * @struct PalCommandBuffer
 * @brief Opaque handle to a command buffer.
 *
 * @since Added in version 2.0
 */
typedef struct PalCommandBuffer PalCommandBuffer;

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

#endif // PAL_GRAPHICS_CMDBUFFER_H