
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

#ifndef PAL_THREAD_MUTEX_H
#define PAL_THREAD_MUTEX_H

#include "pal2/core/defines.h"
#include "pal2/core/memory.h"
#include "pal2/core/result.h"

/**
 * @struct PalMutex
 * @brief Opaque handle to a mutex.
 *
 * @since Added in version 2.0
 */
typedef struct PalMutex PalMutex;

/**
 * @brief Creates a mutex.
 *
 * @param[in] allocator The allocator the thread should use. Set to `nullptr`
 * to use the thread-safe default.
 * @param[out] mutex The output handle to recieve the created mutex.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety `mutex` must be per thread and `allocator` must be 
 * thread safe.
 *
 * @since Added in version 2.0
 * @sa palDestroyMutex
 */
PAL_API PalResult PAL_CALL palCreateMutex(
    const PalAllocator* allocator,
    PalMutex** mutex);

/**
 * @brief Destroys a mutex.
 * 
 * The mutex must be unlocked before destroying if it was locked.
 *
 * @param[in] mutex The mutex.
 *
 * @Thread-safety `mutex` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateMutex
 */
PAL_API void PAL_CALL palDestroyMutex(PalMutex* mutex);

/**
 * @brief Locks a mutex. 
 * 
 * Blocks if the mutex is already locked by another thread.
 *
 * @param[in] mutex The mutex.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palUnlockMutex
 */
PAL_API void PAL_CALL palLockMutex(PalMutex* mutex);

/**
 * @brief Unlocks a mutex.
 *
 * The function must be called by the thread that first locked the mutex.
 *
 * @param[in] mutex The mutex.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palLockMutex
 */
PAL_API void PAL_CALL palUnlockMutex(PalMutex* mutex);

#endif // PAL_THREAD_MUTEX_H