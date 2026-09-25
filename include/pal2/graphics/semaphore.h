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

#ifndef PAL_GRAPHICS_SEMAPHORE_H
#define PAL_GRAPHICS_SEMAPHORE_H

#include "queue.h"

/**
 * @struct PalSemaphore
 * @brief Opaque handle to a semaphore.
 *
 * @since Added in version 2.0
 */
typedef struct PalSemaphore PalSemaphore;

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

#endif // PAL_GRAPHICS_SEMAPHORE_H