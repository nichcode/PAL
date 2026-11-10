
/**
 * @file types.h
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

#ifndef PAL_THREAD_TYPES_H
#define PAL_THREAD_TYPES_H

#include "pal2/core/types.h"

/**
 * @typedef PalThreadFeatures
 * @brief Thread system features.
 *
 * This is a bitmask of the features supported by the thread system.
 *
 * All values of this type follow the format `PAL_THREAD_FEATURE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
typedef uint32_t PalThreadFeatures;

/**
 * @typedef PalThreadPriority
 * @brief Thread priority.
 *
 * Identifies the scheduling priority of a thread.
 *
 * All values of this type follow the format `PAL_THREAD_PRIORITY_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
typedef uint32_t PalThreadPriority;

/**
 * @struct PalThread
 * @brief Opaque handle to a thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
typedef struct PalThread PalThread;

/**
 * @typedef PalTLSId
 * @brief Opaque handle to a thread-local storage (TLS) slot.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
typedef uint32_t PalTLSId;

/**
 * @struct PalMutex
 * @brief Opaque handle to a mutex.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
typedef struct PalMutex PalMutex;

/**
 * @struct PalCondVar
 * @brief Opaque handle to a condition variable.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
typedef struct PalCondVar PalCondVar;

#endif // PAL_THREAD_TYPES_H