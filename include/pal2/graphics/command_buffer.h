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
 * @defgroup cmdbuffer_types Command Buffer Types
 * @ingroup pal_graphics
 * @{
 */
#define PAL_COMMAND_BUFFER_TYPE_PRIMARY 0 /**< Primary command buffer, submitted to a queue. */
#define PAL_COMMAND_BUFFER_TYPE_SECONDARY 1 /**< Secondary command buffer, executed from a primary command buffer. */
#define PAL_COMMAND_BUFFER_TYPE_COUNT 2 /**< Number of command buffer types. */
/** @} */

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
    uint64_t waitValue;             /**< Timeline semaphore value to wait on. */
    uint64_t signalValue;           /**< Timeline semaphore value to signal. */
    PalCommandBuffer* cmdBuffer;    /**< Command buffer to submit. */
    PalSemaphore* waitSemaphore;    /**< Semaphore to wait on. */
    PalSemaphore* signalSemaphore;  /**< Semaphore to signal. */
    PalFence* fence;                /**< Fence to signal. */
    PalPipelineStages waitStages;   /**< Wait semaphore pipeline stages. */
    PalPipelineStages signalStages; /**< Signal semaphore pipeline stages. */
} PalCommandBufferSubmitInfo;

/**
 * @brief Creates a command pool from a device.
 *
 * The created command pool must be destroyed using @ref palDestroyCommandPool().
 *
 * @param[in] device Device that creates the command pool.
 * @param[in] queue Queue the command pool buffers will be submitted to.
 * @param[out] outPool Pointer to a PalCommandPool to receive the created command pool.
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
 * @sa palDestroyCommandPool
 */
PAL_API PalResult PAL_CALL palCreateCommandPool(
    PalDevice* device,
    PalQueue* queue,
    PalCommandPool** outPool);

/**
 * @brief Destroys a command pool.
 *
 * All command buffers allocated from the pool must be freed before this call,
 * otherwise undefined behavior.
 *
 * @param[in] pool Command pool to destroy.
 *
 * @thread-safety The device used to create `pool` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateCommandPool
 */
PAL_API void PAL_CALL palDestroyCommandPool(PalCommandPool* pool);

/**
 * @brief Resets all command buffers allocated from the provided command pool.
 *
 * @param[in] pool Command pool to reset its command buffers.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `pool` must be externally synchronized.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palResetCommandPool(PalCommandPool* pool);

/**
 * @brief Allocates a command buffer from the provided command pool.
 *
 * @param[in] device Device to allocate command buffer on.
 * @param[in] pool Command pool to allocate command buffer from.
 * @param[in] type Type of the command buffer (eg. PAL_COMMAND_BUFFER_TYPE_PRIMARY).
 * @param[out] outCmdBuffer Pointer to a PalCommandBuffer to receive the created command buffer.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `device` and `pool` must be externally synchronized.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_PLATFORM_FAILURE
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
 * @brief Frees an allocated command buffer.
 *
 * @param[in] cmdBuffer Command buffer to free.
 *
 * @thread-safety The command pool used to create `cmdBuffer` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palAllocateCommandBuffer
 */
PAL_API void PAL_CALL palFreeCommandBuffer(PalCommandBuffer* cmdBuffer);

/**
 * @brief Resets the provided command buffer.
 *
 * @param[in] cmdBuffer Command buffer to reset.
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
 */
PAL_API PalResult PAL_CALL palResetCommandBuffer(PalCommandBuffer* cmdBuffer);

/**
 * @brief Submits a command buffer to the provided queue for execution.
 *
 * The command buffer must not be in a recording state.
 *
 * @param[in] queue Queue to execute the command buffer.
 * @param[in] info Pointer to a PalCommandBufferSubmitInfo struct that specifies parameters.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `queue` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_PLATFORM_FAILURE
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palSubmitCommandBuffer(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info);

#endif // PAL_GRAPHICS_NEXT_H