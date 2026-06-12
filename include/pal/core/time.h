

/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup time Time System
 * @ingroup pal_core
 * @{
 */

#ifndef _CORE_TIME_H
#define _CORE_TIME_H

#include "defines.h"

/**
 * Query a high-resolution performance counter value.
 *
 * @return Current performance counter value.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palGetPerformanceFrequency
 */
PAL_API Uint64 PAL_CALL palGetPerformanceCounter();

/**
 * Query the frequency of the high-resolution performance counter.
 *
 * @return Performance counter frequency, in counts per second.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palGetPerformanceCounter
 */
PAL_API Uint64 PAL_CALL palGetPerformanceFrequency();

#endif // _CORE_TIME_H

/** @} */