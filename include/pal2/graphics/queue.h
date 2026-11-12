/**
 * @file queue.h
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

#ifndef PAL_GRAPHICS_QUEUE_H
#define PAL_GRAPHICS_QUEUE_H

#include "types.h"

/**
 * @defgroup queue_types Queue Types
 * @ingroup pal_graphics
 * @{
 */
#define PAL_QUEUE_TYPE_GRAPHICS 0 /**< Graphics, compute and transfer work. */
#define PAL_QUEUE_TYPE_COMPUTE 1 /**< Compute and transfer work. */
#define PAL_QUEUE_TYPE_COPY 2 /**< Transfer work only. */
#define PAL_QUEUE_TYPE_COUNT 3 /**< Number of queue types. */
/** @} */

/**
 * @brief Creates a queue from a device.
 *
 * The created queue must be destroyed using @ref palDestroyQueue().
 *
 * The number of queues of each type which can be created is limited per adapter. check with
 * PalAdapterCapabilities::maxComputeQueues, PalAdapterCapabilities::maxGraphicsQueues and
 * PalAdapterCapabilities::maxCopyQueues respectively for the limit for each queue type.
 *
 * Not all graphics queues support presentation. Create a graphics queue and then check if
 * its support presentation for the provided surface. see @ref palCanQueuePresent(). All graphics
 * queue supports offscreen rendering.
 *
 * @param[in] device Device that creates the queue.
 * @param[in] type Queue type.
 * @param[out] queue Output to receive the created queue.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `device` must be externally synchronized and `queue` must be per thread.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_INVALID_OPERATION
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palDestroyQueue
 */
PAL_API PalResult PAL_CALL palCreateQueue(
    PalDevice* device,
    PalQueueType type,
    PalQueue** queue);

/**
 * @brief Destroys a queue.
 *
 * @param[in] queue Queue to destroy.
 *
 * @thread-safety The device used to create `queue` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateQueue
 */
PAL_API void PAL_CALL palDestroyQueue(PalQueue* queue);

/**
 * @brief Checks whether a queue supports presentation to the provided surface.
 *
 * @param[in] queue Queue to query.
 * @param[in] surface Surface to check presentation support for.
 * @return `PAL_TRUE` if the queue supports presentation to `surface` otherwise `PAL_FALSE`.
 *
 * @thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateQueue
 */
PAL_API PalBool PAL_CALL palCanQueuePresent(
    PalQueue* queue,
    PalSurface* surface);

/**
 * @brief Checks whether two queues can access the same resources without a
 * queue-family ownership transfer.
 *
 * The adapter used to create the queue's device must support
 * `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2` or later.
 *
 * @param[in] a First queue.
 * @param[in] b Second queue.
 * @return `PAL_TRUE` if resources can be shared without an ownership transfer otherwise `PAL_FALSE`.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.1
 * @ingroup pal_graphics
 */
PAL_API PalBool PAL_CALL palCanQueueShareOwnership(
    PalQueue* a,
    PalQueue* b);

/**
 * @brief Checks whether a queue supports the provided resource usage state.
 *
 * The adapter used to create the queue's device must support
 * `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2` or later.
 *
 * @param[in] queue The queue to query.
 * @param[in] state The usage state.
 * @return `PAL_TRUE` if the queue supports the usage state otherwise `PAL_FALSE`.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.1
 * @ingroup pal_graphics
 *
 * @sa palCanQueueUsePipelineStages
 */
PAL_API PalBool PAL_CALL palCanQueueUseUsageState(
    PalQueue* queue,
    PalUsageState state);

/**
 * @brief Checks whether a queue supports the provided pipeline stages.
 *
 * The adapter used to create the queue's device must support
 * `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2` or later.
 *
 * @param[in] queue The queue to query.
 * @param[in] stages The pipeline stages.
 * @return `PAL_TRUE` if the queue supports the pipeline stages otherwise `PAL_FALSE`.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.1
 * @ingroup pal_graphics
 *
 * @sa palCanQueueUseUsageState
 */
PAL_API PalBool PAL_CALL palCanQueueUsePipelineStages(
    PalQueue* queue,
    PalPipelineStages stages);

/**
 * @brief Waits until the queue becomes idle.
 *
 * This function blocks until all previously submitted work on the queue has completed.
 * Returns `PAL_RESULT_SUCCESS` when all previously submitted work has completed.
 *
 * @param[in] queue Queue to wait.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `queue` must be externally synchronized.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalResult PAL_CALL palWaitQueue(PalQueue* queue);

#endif // PAL_GRAPHICS_QUEUE_H