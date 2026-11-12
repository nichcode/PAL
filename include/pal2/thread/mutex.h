
/**
 * @file mutex.h
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

#ifndef PAL_THREAD_MUTEX_H
#define PAL_THREAD_MUTEX_H

#include "pal2/core/base.h"
#include "pal2/core/result.h"
#include "pal2/core/memory.h"

/**
 * @struct PalMutex
 * @brief Opaque handle to a mutex.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
typedef struct PalMutex PalMutex;

/**
 * @brief Creates a mutex.
 *
 * The created mutex must be destroyed using @ref palDestroyMutex().
 *
 * `allocator` is not copied. The allocator and any state referenced by it must
 * remain valid until the mutex is destroyed.
 *
 * @param[in] allocator Allocator to use, or `nullptr` for the default.
 * @param[out] mutex Output handle to receive the created mutex.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `mutex` must be per thread.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palDestroyMutex
 */
PAL_API PalResult PAL_CALL palCreateMutex(
    const PalAllocator* allocator,
    PalMutex** mutex);

/**
 * @brief Destroys a mutex.
 *
 * A locked mutex must be unlocked before it is destroyed.
 *
 * @param[in] mutex Mutex to destroy.
 *
 * @thread-safety `mutex` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palCreateMutex
 */
PAL_API void PAL_CALL palDestroyMutex(PalMutex* mutex);

/**
 * @brief Locks a mutex.
 *
 * Blocks until the mutex is available if it is already locked by another thread.
 *
 * @param[in] mutex Mutex to lock.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palUnlockMutex
 */
PAL_API void PAL_CALL palLockMutex(PalMutex* mutex);

/**
 * @brief Unlocks a mutex.
 *
 * This function must be called by the thread that locked the mutex.
 *
 * @param[in] mutex Mutex to unlock.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palLockMutex
 */
PAL_API void PAL_CALL palUnlockMutex(PalMutex* mutex);

#endif // PAL_THREAD_MUTEX_H