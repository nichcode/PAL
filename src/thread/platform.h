
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

#ifndef THREAD_PLATFORM_H
#define THREAD_PLATFORM_H

#include "shared.h"
#include "pal2/thread/thread.h"
#include "pal2/thread/condvar.h"
#include "pal2/thread/mutex.h"
#include "pal2/thread/tls.h"

/**
 * Platform contract for the thread module.
 *
 * Every platform must implement every function declared here. A function
 * that says it has the same semantics and rules as a public `pal*` function
 * has only the platform requirements. A function with no public counterpart
 * is documented in full. These rules apply to all of them:
 *
 * - Arguments are validated by the caller. A platform function can assume
 *   every pointer it receives is valid and must not validate it again.
 *   Any exception will be documented by the function.
 */


/* =========================================================================
 * Thread
 * ========================================================================= */

/**
 * This function has the same semantics and rules as palCreateThread().
 *
 * Additional rules:
 * - The value returned by the entry function must be kept so
 * `platformJoinThread()` can return it.
 */
PalResult platformCreateThread(
    const PalThreadCreateInfo* info,
    PalThread** thread);

/**
 * This function has the same semantics and rules as palJoinThread().
 *
 * Additional rules:
 * - On success the platform releases the native thread resources.
 */
PalResult platformJoinThread(
    PalThread* thread,
    void** retval);

/** This function has the same semantics and rules as palDetachThread(). */
void platformDetachThread(PalThread* thread);

/**
 * This function has the same semantics and rules as palSleep().
 *
 * Additional rules:
 * - A sleep can return early because of a signal or an interrupt.
 *   Resume with the remaining time (for example, on `EINTR`).
 */
void platformSleep(uint64_t milliseconds);

/**
 * This function has the same semantics and rules as palYield().
 *
 * Additional rules:
 * - It can do nothing if the platform has no implementation.
 */
void platformYield();

/**
 * This function has the same semantics and rules as palGetCurrentThread().
 *
 * Additional rules:
 * - The returned handle must be valid and one that other threads can use.
 */
PalThread* platformGetCurrentThread();

/** This function has the same semantics and rules as palGetThreadFeatures(). */
PalThreadFeatures platformGetThreadFeatures();

/** This function has the same semantics and rules as palGetThreadPriority(). */
PalThreadPriority platformGetThreadPriority(PalThread* thread);

/** This function has the same semantics and rules as palGetThreadAffinity(). */
uint64_t platformGetThreadAffinity(PalThread* thread);

/** This function has the same semantics and rules as palGetThreadName(). */
void platformGetThreadName(
    PalThread* thread,
    uint64_t bufferSize,
    uint64_t* size,
    char* buffer);

/** This function has the same semantics and rules as palSetThreadPriority(). */
PalResult platformSetThreadPriority(
    PalThread* thread,
    PalThreadPriority priority);

/** This function has the same semantics and rules as palSetThreadAffinity(). */
PalResult platformSetThreadAffinity(
    PalThread* thread,
    uint64_t mask);

/**
 * This function has the same semantics and rules as palSetThreadName().
 *
 * Additional rules:
 * - `name` must be truncated to the required or maximum size of the name
 *   for the platform.
 */
PalResult platformSetThreadName(
    PalThread* thread,
    const char* name);


/* =========================================================================
 * Thread Local Storage (TLS)
 * ========================================================================= */

/**
 * This function has the same semantics and rules as palCreateTLS().
 *
 * Additional rules:
 * - A platform without native destructors must use an alternative that has them
 *   or any other way to avoid memory leaks.
 */
PalTLSId platformCreateTLS(PaTlsDestructorFn destructor);

/** This function has the same semantics and rules as palDestroyTLS(). */
void platformDestroyTLS(PalTLSId Tls);

/** This function has the same semantics and rules as palGetTLS(). */
void* platformGetTLS(PalTLSId Tls);

/** This function has the same semantics and rules as palSetTLS(). */
void platformSetTLS(
    PalTLSId Tls,
    void* data);


/* =========================================================================
 * Mutex
 * ========================================================================= */

/**
 * This function has the same semantics and rules as palCreateMutex().
 * 
 * Additional rules:
 * - The native mutex type may be resursive or not, the caller assumes non-resursive.
 * - The mutex and the condition variable must work together. The native
 *   mutex must be usable by `platformWaitCondVar()`.
 */
PalResult platformCreateMutex(
    const PalAllocator* allocator,
    PalMutex** mutex);

/** This function has the same semantics and rules as palDestroyMutex(). */
void platformDestroyMutex(PalMutex* mutex);

/** This function has the same semantics and rules as palLockMutex(). */
void platformLockMutex(PalMutex* mutex);

/** This function has the same semantics and rules as palUnlockMutex(). */
void platformUnlockMutex(PalMutex* mutex);


/* =========================================================================
 * Condition Variable
 * ========================================================================= */

/**
 * This function has the same semantics and rules as palCreateCondVar().
 *
 * Additional rules:
 * - Use a monotonic native source; a system clock must not be used.
 */
PalResult platformCreateCondVar(
    const PalAllocator* allocator,
    PalCondVar** condVar);

/** This function has the same semantics and rules as palDestroyCondVar(). */
void platformDestroyCondVar(PalCondVar* condVar);

/** This function has the same semantics and rules as palWaitCondVar(). */
PalResult platformWaitCondVar(
    PalCondVar* condVar,
    PalMutex* mutex);

/** This function has the same semantics and rules as palWaitCondVarTimeout(). */
PalResult platformWaitCondVarTimeout(
    PalCondVar* condVar,
    PalMutex* mutex,
    uint64_t milliseconds);

/** This function has the same semantics and rules as palSignalCondVar(). */
void platformSignalCondVar(PalCondVar* condVar);

/** This function has the same semantics and rules as palBroadcastCondVar(). */
void platformBroadcastCondVar(PalCondVar* condVar);

#endif // THREAD_PLATFORM_H