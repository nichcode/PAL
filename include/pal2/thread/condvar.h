
/**
 * @file condvar.h
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

#ifndef PAL_THREAD_CONDVAR_H
#define PAL_THREAD_CONDVAR_H

#include "types.h"
#include "pal2/core/memory.h"

/**
 * @brief Creates a condition variable.
 *
 * The created condition variable must be destroyed using @ref palDestroyCondVar().
 *
 * `allocator` is not copied. The allocator and any state referenced by it must
 * remain valid until the condition variable is destroyed.
 *
 * @param[in] allocator Allocator to use, or `nullptr` for the default.
 * @param[out] condVar Output handle to receive the created condition variable.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `condVar` must be per thread.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palDestroyCondVar
 */
PAL_API PalResult PAL_CALL palCreateCondVar(
    const PalAllocator* allocator,
    PalCondVar** condVar);

/**
 * @brief Destroys a condition variable.
 *
 * Destroying a condition variable while threads are waiting on it is
 * undefined behavior, since those threads may be blocked with no way to
 * signal them.
 *
 * @param[in] condVar Condition variable to destroy.
 *
 * @thread-safety `condVar` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palCreateCondVar
 */
PAL_API void PAL_CALL palDestroyCondVar(PalCondVar* condVar);

/**
 * @brief Unlocks the mutex and waits on the condition variable.
 *
 * The mutex must be locked by the calling thread before this call. It is
 * unlocked while waiting and locked again before the function returns.
 * Spurious wakeups may occur, so always wait inside a loop that checks the
 * condition. This function waits until the condition variable is signaled.
 *
 * Example Code:
 * @code
 * while (!ready) { palWaitCondVar(condition, mutex); }
 * @endcode
 *
 * @param[in] condVar Condition variable to wait on.
 * @param[in] mutex Locked mutex to unlock while waiting.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety Thread safe.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_HANDLE
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palWaitCondVarTimeout
 */
PAL_API PalResult PAL_CALL palWaitCondVar(
    PalCondVar* condVar,
    PalMutex* mutex);

/**
 * @brief Unlocks the mutex and waits on the condition variable for the specified duration.
 *
 * The mutex must be locked by the calling thread before this call. It is
 * unlocked while waiting and locked again before the function returns.
 * Spurious wakeups may occur, so always wait inside a loop that checks the
 * condition. This function waits for at most `milliseconds` and fails with
 * @ref PAL_RESULT_CODE_TIMEOUT if the condition variable was not signaled in
 * that time.
 *
 * Example Code:
 * @code
 * while (!ready) {
 *     if (palWaitCondVarTimeout(condition, mutex, 100) != PAL_RESULT_SUCCESS) {
 *         break; // timed out or failed
 *     }
 * }
 * @endcode
 *
 * @param[in] condVar Condition variable to wait on.
 * @param[in] mutex Locked mutex to unlock while waiting.
 * @param[in] milliseconds Timeout in milliseconds.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety Thread safe.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_HANDLE
 * @result-code @ref PAL_RESULT_CODE_TIMEOUT
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palWaitCondVar
 */
PAL_API PalResult PAL_CALL palWaitCondVarTimeout(
    PalCondVar* condVar,
    PalMutex* mutex,
    uint64_t milliseconds);

/**
 * @brief Wakes a thread waiting on the condition variable.
 *
 * If multiple threads are waiting on the condition variable, which thread is
 * awakened is implementation-defined. Does nothing if no thread is waiting.
 *
 * @param[in] condVar Condition variable to signal.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palBroadcastCondVar
 */
PAL_API void PAL_CALL palSignalCondVar(PalCondVar* condVar);

/**
 * @brief Wakes all threads waiting on the condition variable.
 *
 * @param[in] condVar Condition variable to broadcast to.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palSignalCondVar
 */
PAL_API void PAL_CALL palBroadcastCondVar(PalCondVar* condVar);

#endif // PAL_THREAD_CONDVAR_H