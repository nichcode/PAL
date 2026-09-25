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

#ifndef PAL_GRAPHICS_QUEUE_H
#define PAL_GRAPHICS_QUEUE_H

#include "device.h"

/**
 * @defgroup queue_types Queue Types
 * @brief Queue types
 * 
 * @{
 */
#define PAL_QUEUE_TYPE_GRAPHICS 0
#define PAL_QUEUE_TYPE_COMPUTE 1
#define PAL_QUEUE_TYPE_COPY 2
#define PAL_QUEUE_TYPE_COUNT 3
/** @} */

/**
 * @typedef PalQueueType
 * @brief Queue types.
 * 
 * All values of this type follow the format `PAL_QUEUE_TYPE_*`
 * for API consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalQueueType;

/**
 * @struct PalQueue
 * @brief Opaque handle to a queue.
 *
 * @since Added in version 2.0
 */
typedef struct PalQueue PalQueue;

/**
 * @brief Create a queue from a device.
 *
 * The created queue must be destroyed using `palDestroyQueue()`.
 *
 * The number of queues of each type which can be created is limited per adapter. check with
 * PalAdapterCapabilities::maxComputeQueues, PalAdapterCapabilities::maxGraphicsQueues and
 * PalAdapterCapabilities::maxCopyQueues respectively for the limit for each queue type.
 * Creating more queues than the supported will fail and return `PAL_RESULT_OUT_OF_QUEUE`.
 *
 * Not all graphics queues support presentation. Create a graphics queue and then check if
 * its support presentation for the provided surface. see `palCanQueuePresent()`. Any graphics
 * queue supports offscreen rendering.
 *
 * @param[in] device Device that creates the queue.
 * @param[in] type Queue type. (eg. PAL_QUEUE_TYPE_GRAPHICS).
 * @param[out] outQueue Pointer to a PalQueue to recieve the created queue.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palDestroyQueue
 */
PAL_API PalResult PAL_CALL palCreateQueue(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue);

/**
 * @brief Destroy a queue.
 *
 * @param[in] queue Queue to destroy.
 *
 * Thread safety: Thread safe if the device used to create the queue is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateQueue
 */
PAL_API void PAL_CALL palDestroyQueue(PalQueue* queue);

/**
 * @brief Check if a queue is presentable to the provided window.
 *
 * @param[in] queue Queue to query.
 * @param[in] surface Surface to check presentation support for.
 *
 * @return `PAL_TRUE` if queue can present otherwise `PAL_FALSE`.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palCreateQueue
 */
PAL_API PalBool PAL_CALL palCanQueuePresent(
    PalQueue* queue,
    PalSurface* surface);

/**
 * @brief Check if two queues can share resources without requiring ownership transfer.
 *
 * The adapter used to create the queue's device must support
 * `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2` or later.
 *
 * @param[in] a First queue
 * @param[in] b Second queue.
 *
 * @return `PAL_TRUE` if both queues can share resources otherwise `PAL_FALSE`.
 *
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.1
 */
PAL_API PalBool PAL_CALL palCanQueueShareOwnership(
    PalQueue* a,
    PalQueue* b);

/**
 * @brief Check if a queue can use the provided usage state.
 *
 * The adapter used to create the queue's device must support
 * `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2` or later.
 *
 * @param[in] queue The queue to query.
 * @param[in] state The usage state.
 *
 * @return `PAL_TRUE` if the queue can use the usage state otherwise `PAL_FALSE`.
 *
 * Thread safety: Thread safe.
 *
 * @sa palCanQueueUsePipelineStages
 * @since Added in version 2.1
 */
PAL_API PalBool PAL_CALL palCanQueueUseUsageState(
    PalQueue* queue,
    PalUsageState state);

/**
 * @brief Check if a queue can use the provided pipeline stages.
 *
 * The adapter used to create the queue's device must support
 * `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2` or later.
 *
 * @param[in] queue The queue to query.
 * @param[in] stages The pipeline stages.
 *
 * @return `PAL_TRUE` if the queue can use the pipeline stages otherwise `PAL_FALSE`.
 *
 * Thread safety: Thread safe.
 *
 * @sa palCanQueueUseUsageState
 * @since Added in version 2.1
 */
PAL_API PalBool PAL_CALL palCanQueueUsePipelineStages(
    PalQueue* queue,
    PalPipelineStages stages);

/**
 * @brief Blocks indefinitely until the queue becomes idle.
 *
 * This function blocks indefinitely until all submitted work on the queue has been completetd.
 * Returns `PAL_RESULT_SUCCESS` to indicate all pending operations has been completetd.
 *
 * @param[in] queue Pointer to queue to wait.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `queue` is externally synchronized.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palWaitQueue(PalQueue* queue);

#endif // PAL_GRAPHICS_QUEUE_H