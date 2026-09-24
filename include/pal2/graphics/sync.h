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

#ifndef PAL_GRAPHICS_SYNC_H
#define PAL_GRAPHICS_SYNC_H

#include "device.h"

/**
 * @struct PalFence
 * @brief Opaque handle to a fence.
 *
 * @since Added in version 2.0
 */
typedef struct PalFence PalFence;

/**
 * @struct PalSemaphore
 * @brief Opaque handle to a semaphore.
 *
 * @since Added in version 2.0
 */
typedef struct PalSemaphore PalSemaphore;

/**
 * @brief Create a fence.
 *
 * The created fence must be destroyed using `palDestroyFence()`.
 *
 * @param[in] device Device that creates the fence.
 * @param[in] signaled True if fence should be created signaled. If true, the fence must be reset
 * before waiting for it to prevent waiting forever.
 * @param[out] outFence Pointer to a PalFence to recieve the created fence.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palDestroyFence
 */
PAL_API PalResult PAL_CALL palCreateFence(
    PalDevice* device,
    PalBool signaled,
    PalFence** outFence);

/**
 * @brief Destroy a fence.
 *
 * @param[in] fence Fence to destroy.
 *
 * Thread safety: Thread safe if the device used to create the fence is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateFence
 */
PAL_API void PAL_CALL palDestroyFence(PalFence* fence);

/**
 * @brief Wait for a fence.
 *
 * This function blocks for `timeout` until the fence is signaled or there is a timeout.
 * Returns `PAL_RESULT_SUCCESS` or `PAL_RESULT_TIMEOUT` respectively.
 *
 * @param[in] fence Fence to wait for.
 * @param[in] timeout Time to wait for in milliseconds. Set to `PAL_INFINITE` for indefintely.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `fence` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palIsFenceSignaled
 */
PAL_API PalResult PAL_CALL palWaitFence(
    PalFence* fence,
    uint64_t timeout);

/**
 * @brief Reset a fence to an unsignaled state.
 *
 * `PAL_ADAPTER_FEATURE_FENCE_RESET` must be supported and enabled when creating the
 * device. Otherwise behavior is undefined.
 *
 * @param[in] fence Fence to reset.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `fence` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palIsFenceSignaled
 */
PAL_API PalResult PAL_CALL palResetFence(PalFence* fence);

/**
 * @brief Checks if the provided fence is in a signaled state.
 *
 * @param[in] fence Fence to check.
 *
 * @return True if signaled otherwise `PAL_FALSE`.
 *
 * Thread safety: Thread safe.
 *
 * @since Added in version 2.0
 * @sa palResetFence
 * @sa palWaitFence
 */
PAL_API PalBool PAL_CALL palIsFenceSignaled(PalFence* fence);

/**
 * @brief Create a semaphore.
 *
 * The created semaphore must be destroyed using `palDestroySemaphore()`.
 *
 * @param[in] device Device that creates the semaphore.
 * @param[in] enableTimeline If true, `PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE` must be supported
 * and enabled by the device creating the semaphore.
 * @param[out] outSemaphore Pointer to a PalSemaphore to recieve the created semaphore.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palDestroySemaphore
 */
PAL_API PalResult PAL_CALL palCreateSemaphore(
    PalDevice* device,
    PalBool enableTimeline,
    PalSemaphore** outSemaphore);

/**
 * @brief Destroy a semaphore.
 *
 * @param[in] semaphore Semaphore to destroy.
 *
 * Thread safety: Thread safe if the device used to create the semaphore is
 * externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateSemaphore
 */
PAL_API void PAL_CALL palDestroySemaphore(PalSemaphore* semaphore);

/**
 * @brief Waits for a semaphore to reach the provided value.
 *
 * The provided semaphore must be a timeline semaphore. Otherwise undefined behavior.
 *
 * @param[in] semaphore Semaphore to wait on.
 * @param[in] value Value to wait for.
 * @param[in] timeout Time to wait for in milliseconds. Set to `PAL_INFINITE` for indefintely.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `semaphore` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palSignalSemaphore
 * @sa palGetSemaphoreValue
 */
PAL_API PalResult PAL_CALL palWaitSemaphore(
    PalSemaphore* semaphore,
    uint64_t value,
    uint64_t timeout);

/**
 * @brief Signals a semaphore from the provided value.
 *
 * The provided semaphore must be a timeline semaphore. Otherwise undefined behavior.
 *
 * @param[in] semaphore Semaphore to signal.
 * @param[in] queue Queue used to signal the semaphore.
 * @param[in] value Value to signal.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `queue` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palWaitSemaphore
 * @sa palGetSemaphoreValue
 */
PAL_API PalResult PAL_CALL palSignalSemaphore(
    PalSemaphore* semaphore,
    PalQueue* queue,
    uint64_t value);

/**
 * @brief Get the value of a semaphore.
 *
 * The provided semaphore must be a timeline semaphore. Otherwise undefined behavior.
 *
 * @param[in] semaphore Semaphore to get its value.
 * @param[out] value The semaphore value.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `semaphore` is externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palWaitSemaphore
 * @sa palSignalSemaphore
 */
PAL_API PalResult PAL_CALL palGetSemaphoreValue(
    PalSemaphore* semaphore,
    uint64_t* value);

#endif // PAL_GRAPHICS_SYNC_H