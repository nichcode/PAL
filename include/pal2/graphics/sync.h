/**
 * @file sync.h
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

#ifndef PAL_GRAPHICS_SYNC_H
#define PAL_GRAPHICS_SYNC_H

#include "pal2/core/base.h"
#include "pal2/core/result.h"
#include "device.h"
#include "queue.h"

/**
 * @struct PalFence
 * @brief Opaque handle to a fence.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalFence PalFence;

/**
 * @struct PalSemaphore
 * @brief Opaque handle to a semaphore.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalSemaphore PalSemaphore;

/**
 * @brief Creates a fence.
 *
 * The created fence must be destroyed using @ref palDestroyFence().
 *
 * @param[in] device Device that creates the fence.
 * @param[in] signaled True if fence should be created signaled. If true, the fence must be reset
 * before waiting for it to prevent waiting forever.
 * @param[out] fence Output handle to receive the created fence.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `device` must be externally synchronized.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palDestroyFence
 */
PAL_API PalResult PAL_CALL palCreateFence(
    PalDevice* device,
    PalBool signaled,
    PalFence** fence);

/**
 * @brief Destroys a fence.
 *
 * @param[in] fence Fence to destroy.
 *
 * @thread-safety The device used to create `fence` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateFence
 */
PAL_API void PAL_CALL palDestroyFence(PalFence* fence);

/**
 * @brief Waits for a fence.
 *
 * This function blocks until the fence is signaled or `timeout` expires.
 * It returns `PAL_RESULT_SUCCESS` when the fence is signaled.
 *
 * @param[in] fence Fence to wait for.
 * @param[in] timeout Maximum time to wait in milliseconds. Set to `PAL_INFINITE` to wait indefinitely.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `fence` must be externally synchronized.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_TIMEOUT
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palIsFenceSignaled
 */
PAL_API PalResult PAL_CALL palWaitFence(
    PalFence* fence,
    uint64_t timeout);

/**
 * @brief Resets a fence to an unsignaled state.
 *
 * @ref PAL_ADAPTER_FEATURE_FENCE_RESET feature must be supported and enabled when
 * creating the device or this function results in undefined behavior.
 *
 * @param[in] fence Fence to reset.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `fence` must be externally synchronized.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palIsFenceSignaled
 */
PAL_API PalResult PAL_CALL palResetFence(PalFence* fence);

/**
 * @brief Checks whether the specified fence is in a signaled state.
 *
 * @param[in] fence Fence to check.
 * @return `PAL_TRUE` if signaled otherwise `PAL_FALSE`.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palResetFence
 * @sa palWaitFence
 */
PAL_API PalBool PAL_CALL palIsFenceSignaled(PalFence* fence);

/**
 * @brief Creates a semaphore.
 *
 * The created semaphore must be destroyed using @ref palDestroySemaphore().
 *
 * @param[in] device Device that creates the semaphore.
 * @param[in] enableTimeline If true, `PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE` must be supported
 * and enabled by the device creating the semaphore.
 * @param[out] semaphore Output handle to receive the created semaphore.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `device` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palDestroySemaphore
 */
PAL_API PalResult PAL_CALL palCreateSemaphore(
    PalDevice* device,
    PalBool enableTimeline,
    PalSemaphore** semaphore);

/**
 * @brief Destroys a semaphore.
 *
 * @param[in] semaphore Semaphore to destroy.
 *
 * @thread-safety The device used to create `semaphore` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palCreateSemaphore
 */
PAL_API void PAL_CALL palDestroySemaphore(PalSemaphore* semaphore);

/**
 * @brief Waits for a timeline semaphore to reach a value.
 *
 * The specified semaphore must be a timeline semaphore. Otherwise undefined behavior.
 *
 * @param[in] semaphore Semaphore to wait on.
 * @param[in] value Value to wait for.
 * @param[in] timeout Time to wait for in milliseconds. Set to `PAL_INFINITE` for indefinitely.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `semaphore` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_TIMEOUT
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palSignalSemaphore
 * @sa palGetSemaphoreValue
 */
PAL_API PalResult PAL_CALL palWaitSemaphore(
    PalSemaphore* semaphore,
    uint64_t value,
    uint64_t timeout);

/**
 * @brief Signals a timeline semaphore from a queue.
 *
 * The specified semaphore must be a timeline semaphore. Otherwise undefined behavior.
 *
 * @param[in] semaphore Semaphore to signal.
 * @param[in] queue Queue used to signal the semaphore.
 * @param[in] value Value to signal.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `queue` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palWaitSemaphore
 * @sa palGetSemaphoreValue
 */
PAL_API PalResult PAL_CALL palSignalSemaphore(
    PalSemaphore* semaphore,
    PalQueue* queue,
    uint64_t value);

/**
 * @brief Gets the current value of a timeline semaphore.
 *
 * The provided semaphore must be a timeline semaphore. Otherwise undefined behavior.
 *
 * @param[in] semaphore Semaphore to get its value.
 * @param[out] value Output to receive the semaphore value.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `semaphore` must be externally synchronized.
 * 
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palWaitSemaphore
 * @sa palSignalSemaphore
 */
PAL_API PalResult PAL_CALL palGetSemaphoreValue(
    PalSemaphore* semaphore,
    uint64_t* value);

#endif // PAL_GRAPHICS_SYNC_H