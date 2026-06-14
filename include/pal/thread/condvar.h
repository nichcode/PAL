
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup thread Condition variable section
 * @ingroup pal_thread
 * @{
 */

#ifndef _THREAD_CONDVAR_H
#define _THREAD_CONDVAR_H

#include "mutex.h"

/**
 * @struct PalCondVar
 * @brief Opaque handle to a condition variable.
 *
 * @since 1.0
 */
typedef struct PalCondVar PalCondVar;

/**
 * @brief Create a condition variable.
 *
 * @param[in] allocator Optional user-provided allocator. Set to nullptr to use
 * default.
 * @param[out] outCondVar Pointer to a PalCondVar to recieve the created
 * condition variable.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if the provided allocator is
 * thread safe and `outCondVar` is per thread.
 *
 * @since 1.0
 * @sa palDestroyCondVar
 */
PAL_API PalResult PAL_CALL palCreateCondVar(
    const PalAllocator* allocator,
    PalCondVar** outCondVar);

/**
 * @brief Destroy a condition variable.
 *
 * If the condition variable is invalid, this function returns silently.
 * Threads must not wait on the condition variable or undefined
 * behaviour.
 *
 * @param[in] condVar Pointer to the condition to destroy
 *
 * Thread safety: Thread safe if the allocator used to create
 * the condition varibale is thread safe and `condVar` is per thread.
 *
 * @since 1.0
 * @sa palCreateCondVar
 */
PAL_API void PAL_CALL palDestroyCondVar(PalCondVar* condVar);

/**
 * @brief Unlock the provided mutex and wait on the condition variable.
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
 * @param[in] condVar Pointer to the condition variable.
 * @param[in] mutex Pointer to the mutex.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palWaitCondVarTimeout
 */
PAL_API PalResult PAL_CALL palWaitCondVar(
    PalCondVar* condVar,
    PalMutex* mutex);

/**
 * @brief Unlock the provided mutex and wait on the condition variable.
 *
 * The mutex must be locked before this call. Spurious wakeups may occur, its
 * best to use a loop. If the condition variable is not signaled but the time to
 * wait is up `PAL_RESULT_TIMEOUT` is returned.
 *
 * @param[in] condVar Pointer to the condition variable.
 * @param[in] mutex Pointer to the mutex.
 * @param[in] milliseconds Timeout in milliseconds.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palWaitCondVar
 */
PAL_API PalResult PAL_CALL palWaitCondVarTimeout(
    PalCondVar* condVar,
    PalMutex* mutex,
    uint64_t milliseconds);

/**
 * @brief Wake a single thread waiting on the condition variable.
 *
 * @param[in] condVar Pointer to the condition variable.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palBroadcastCondVar
 */
PAL_API void PAL_CALL palSignalCondVar(PalCondVar* condVar);

/**
 * @brief Wake all threads waiting on the condition variable.
 *
 * @param[in] condVar Pointer to the condition variable.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palSignalCondVar
 */
PAL_API void PAL_CALL palBroadcastCondVar(PalCondVar* condVar);

#endif // _THREAD_CONDVAR_H

/** @} */