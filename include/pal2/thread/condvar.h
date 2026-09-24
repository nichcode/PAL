
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

#ifndef PAL_THREAD_CONDVAR_H
#define PAL_THREAD_CONDVAR_H

#include "mutex.h"

/**
 * @struct PalCondVar
 * @brief Opaque handle to a condition variable.
 *
 * @since Added in version 2.0
 */
typedef struct PalCondVar PalCondVar;

/**
 * @brief Creates a condition variable.
 *
 * @param[in] allocator The allocator the thread should use. Set to `nullptr`
 * to use the thread-safe default.
 * @param[out] condVar The output handle to recieve the created
 * condition variable.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety `condVar` must be per thread and `allocator` must be 
 * thread safe.
 *
 * @since Added in version 2.0
 * @sa palDestroyCondVar
 */
PAL_API PalResult PAL_CALL palCreateCondVar(
    const PalAllocator* allocator,
    PalCondVar** condVar);

/**
 * @brief Destroys a condition variable.
 *
 * Threads must not wait on the condition variable or undefined
 * behaviour.
 *
 * @param[in] condVar The condition variable.
 *
 * @Thread-safety `condVar` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateCondVar
 */
PAL_API void PAL_CALL palDestroyCondVar(PalCondVar* condVar);

/**
 * @brief Unlocks the mutex and wait on the condition variable.
 *
 * The mutex must be locked before this call. Spurious wakeups may occur, its
 * best to use a loop. This waits until the condition varibale is signaled.
 *
 * Example:
 *
 * @code
 * while (!ready) { palWaitCondVar(condition, mutex); }
 * @endcode
 *
 * @param[in] condVar The condition variable.
 * @param[in] mutex The mutex.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palWaitCondVarTimeout
 */
PAL_API PalResult PAL_CALL palWaitCondVar(
    PalCondVar* condVar,
    PalMutex* mutex);

/**
 * @brief Unlocks the mutex and wait on the condition variable for
 * the specified duration.
 *
 * The mutex must be locked before this call. Spurious wakeups may occur, its
 * best to use a loop. If the condition variable is not signaled but the time to
 * wait is up `PAL_RESULT_TIMEOUT` is returned.
 *
 * @param[in] condVar The condition variable.
 * @param[in] mutex The mutex.
 * @param[in] milliseconds Timeout in milliseconds.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palWaitCondVar
 */
PAL_API PalResult PAL_CALL palWaitCondVarTimeout(
    PalCondVar* condVar,
    PalMutex* mutex,
    uint64_t milliseconds);

/**
 * @brief Wakes a thread waiting on the condition variable.
 *
 * @param[in] condVar The condition variable.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palBroadcastCondVar
 */
PAL_API void PAL_CALL palSignalCondVar(PalCondVar* condVar);

/**
 * @brief Wakes all threads waiting on the condition variable.
 *
 * @param[in] condVar The condition variable.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palSignalCondVar
 */
PAL_API void PAL_CALL palBroadcastCondVar(PalCondVar* condVar);

#endif // PAL_THREAD_CONDVAR_H