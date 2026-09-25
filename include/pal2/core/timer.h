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

#ifndef PAL_CORE_TIMER_H
#define PAL_CORE_TIMER_H

#include "defines.h"

/**
 * @brief Retrieve the current high-resolution performance counter value.
 *
 * @return Current monotonically increasing performance counter value.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palGetPerformanceFrequency
 */
PAL_API uint64_t PAL_CALL palGetPerformanceCounter(void);

/**
 * @brief Retrieve the frequency of the high-resolution performance counter.
 *
 * @return Performance counter frequency, in counts per second.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palGetPerformanceCounter
 */
PAL_API uint64_t PAL_CALL palGetPerformanceFrequency(void);

#endif // PAL_CORE_TIMER_H