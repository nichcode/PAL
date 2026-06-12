
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup thread Mutex section
 * @ingroup pal_thread
 * @{
 */

#ifndef _THREAD_MUTEX_H
#define _THREAD_MUTEX_H

#include "pal/core/defines.h"
#include "pal/core/result.h"
#include "pal/core/memory.h"

/**
 * @struct PalMutex
 * @brief Opaque handle to a mutex.
 *
 * @since 1.0
 */
typedef struct PalMutex PalMutex;

/**
 * @brief Create a mutex.
 *
 * @param[in] allocator Optional user-provided allocator. Set to nullptr to use
 * default.
 * @param[out] outMutex Pointer to a PalMutex to recieve the created mutex.
 * Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if the provided allocator is
 * thread safe and `outMutex` is per thread.
 *
 * @since 1.0
 * @sa palDestroyMutex
 */
PAL_API PalResult PAL_CALL palCreateMutex(
    const PalAllocator* allocator,
    PalMutex** outMutex);

/**
 * @brief Destroy a mutex
 *
 * If `mutex` is invalid, this function returns silently.
 * The mutex must be unlocked before destroying if it was locked.
 *
 * @param[in] mutex Pointer to the mutex.
 *
 * Thread safety: Thread safe if the allocator used to create
 * the mutex is thread safe and `mutex` is per thread.
 *
 * @since 1.0
 * @sa palCreateMutex
 */
PAL_API void PAL_CALL palDestroyMutex(PalMutex* mutex);

/**
 * @brief Lock a mutex. Blocks if the mutex is already locked by another thread.
 *
 * @param[in] mutex Pointer to the mutex to lock.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palUnlockMutex
 */
PAL_API void PAL_CALL palLockMutex(PalMutex* mutex);

/**
 * @brief Unlock a mutex.
 *
 * The function must be called by the thread that first locked the mutex.
 *
 * @param[in] mutex Pointer to the mutex to unlock.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palLockMutex
 */
PAL_API void PAL_CALL palUnlockMutex(PalMutex* mutex);

#endif // _THREAD_MUTEX_H

/** @} */