
/**
 * PAL - Prime Abstraction Layer (PAL)
 * A cross platform abstraction layer over graphics and windowing APIs
 * -------------------------------------------------------------------
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

#ifndef THREAD_PLATFORM_H
#define THREAD_PLATFORM_H

#include "pal2/pal_thread.h"
#include <stdbool.h>

/** This function has the same semantics and rules as 
 * palCreateThread()
 */
PalResult platformCreateThread(
    const PalThreadCreateInfo* info,
    PalThread** thread);

/** This function has the same semantics and rules as 
 * palJoinThread()
 */
PalResult platformJoinThread(
    PalThread* thread,
    void** retval);

/** This function has the same semantics and rules as 
 * palDetachThread()
 */
void platformDetachThread(PalThread* thread);

/** This function has the same semantics and rules as 
 * palSleep()
 */
void platformSleep(uint64_t milliseconds);

/** This function has the same semantics and rules as 
 * palYield()
 */
void platformYield();

/** This function has the same semantics and rules as 
 * palGetCurrentThread()
 */
PalThread* platformGetCurrentThread();

/** This function has the same semantics and rules as 
 * palGetThreadFeatures()
 */
PalThreadFeatures platformGetThreadFeatures();

/** This function has the same semantics and rules as 
 * palGetThreadPriority()
 */
PalThreadPriority platformGetThreadPriority(PalThread* thread);

/** This function has the same semantics and rules as 
 * palGetThreadAffinity()
 */
uint64_t platformGetThreadAffinity(PalThread* thread);

/** This function has the same semantics and rules as 
 * palGetThreadName()
 */
void platformGetThreadName(
    PalThread* thread,
    uint64_t bufferSize,
    uint64_t* size,
    char* buffer);

/** This function has the same semantics and rules as 
 * palSetThreadPriority()
 */
PalResult platformSetThreadPriority(
    PalThread* thread,
    PalThreadPriority priority);

/** This function has the same semantics and rules as 
 * palSetThreadAffinity()
 */
PalResult platformSetThreadAffinity(
    PalThread* thread,
    uint64_t mask);

/** This function has the same semantics and rules as 
 * palSetThreadName()
 */
PalResult platformSetThreadName(
    PalThread* thread,
    const char* name);

#endif // THREAD_PLATFORM_H