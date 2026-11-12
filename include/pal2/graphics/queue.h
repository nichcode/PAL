
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

#include "pal2/core/base.h"
#include "pal2/core/result.h"
#include "device.h"
#include "surface.h"

/**
 * @typedef PalPipelineStages
 * @brief Pipeline stages.
 *
 * Identifies the pipeline stages used for synchronization.
 *
 * Multiple pipeline usages can be OR'ed together using bitwise OR operator (`|`).
 *
 * All values of this type follow the format `PAL_PIPELINE_STAGE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalPipelineStages;

/**
 * @typedef PalUsageState
 * @brief Resource usage state.
 *
 * Identifies how a resource is used at a point in a command buffer.
 *
 * All values of this type follow the format `PAL_USAGE_STATE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalUsageState;

/**
 * @typedef PalQueueType
 * @brief Queue type.
 *
 * Identifies the kind of work a queue accepts.
 *
 * All values of this type follow the format `PAL_QUEUE_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef uint32_t PalQueueType;

/**
 * @struct PalQueue
 * @brief Opaque handle to a queue.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalQueue PalQueue;

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
 * @defgroup usage_states Usage States
 * @ingroup pal_graphics
 * @{
 */
#define PAL_USAGE_STATE_UNDEFINED 0 /**< Invalid usage state. */
#define PAL_USAGE_STATE_PRESENT 1 /**< Presenting to a surface. */
#define PAL_USAGE_STATE_COLOR_ATTACHMENT_WRITE 2 /**< Writing as a color attachment. */
#define PAL_USAGE_STATE_DEPTH_ATTACHMENT_READ 3 /**< Reading as a depth attachment. */
#define PAL_USAGE_STATE_DEPTH_ATTACHMENT_WRITE 4 /**< Writing as a depth attachment. */
#define PAL_USAGE_STATE_STENCIL_ATTACHMENT_READ 5 /**< Reading as a stencil attachment. */
#define PAL_USAGE_STATE_STENCIL_ATTACHMENT_WRITE 6 /**< Writing as a stencil attachment. */
#define PAL_USAGE_STATE_FRAGMENT_SHADING_RATE_ATTACHMENT_READ 7 /**< Reading as a fragment shading rate attachment. */
#define PAL_USAGE_STATE_TRANSFER_READ 8 /**< Reading in a transfer operation. */
#define PAL_USAGE_STATE_TRANSFER_WRITE 9 /**< Writing in a transfer operation. */
#define PAL_USAGE_STATE_VERTEX_READ 10 /**< Reading as a vertex buffer. */
#define PAL_USAGE_STATE_INDEX_READ 11 /**< Reading as an index buffer. */
#define PAL_USAGE_STATE_INDIRECT_READ 12 /**< Reading as indirect arguments. */
#define PAL_USAGE_STATE_UNIFORM_READ 13 /**< Reading as a uniform buffer. */
#define PAL_USAGE_STATE_SHADER_READ 14 /**< Reading in a shader. */
#define PAL_USAGE_STATE_SHADER_WRITE 15 /**< Writing in a shader. */
#define PAL_USAGE_STATE_STORAGE_READ 16 /**< Reading as a storage resource. */
#define PAL_USAGE_STATE_STORAGE_WRITE 17 /**< Writing as a storage resource. */
#define PAL_USAGE_STATE_HOST_READ 18 /**< Reading from the CPU. */
#define PAL_USAGE_STATE_HOST_WRITE 19 /**< Writing from the CPU. */
#define PAL_USAGE_STATE_ACCELERATION_STRUCTURE_READ 20 /**< Reading as an acceleration structure. */
#define PAL_USAGE_STATE_ACCELERATION_STRUCTURE_WRITE 21 /**< Writing as an acceleration structure. */
#define PAL_USAGE_STATE_COUNT 22 /**< Number of usage states. */
/** @} */

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