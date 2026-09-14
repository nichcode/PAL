
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

#include "shared.h"
#include "thread_platform.h"

PalResult PAL_CALL palCreateCondVar(
    const PalAllocator* allocator,
    PalCondVar** condVar)
{
    if (!condVar) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (allocator) {
        if (!allocator->allocate && !allocator->free) {
            return PAL_RESULT_CODE_INVALID_ARGUMENT;
        }
    }

    return platformCreateCondVar(allocator, condVar);
}

void PAL_CALL palDestroyCondVar(PalCondVar* condVar)
{
    ASSERT(condVar != nullptr, "The specified condvar is null");

    platformDestroyCondVar(condVar);
}

PalResult PAL_CALL palWaitCondVar(
    PalCondVar* condVar,
    PalMutex* mutex)
{
    ASSERT(condVar != nullptr, "The specified condvar is null");
    ASSERT(mutex != nullptr, "The specified mutex is null");

    return platformWaitCondVar(condVar, mutex);
}

PalResult PAL_CALL palWaitCondVarTimeout(
    PalCondVar* condVar,
    PalMutex* mutex,
    uint64_t milliseconds)
{
    ASSERT(condVar != nullptr, "The specified condvar is null");
    ASSERT(mutex != nullptr, "The specified mutex is null");

    return platformWaitCondVarTimeout(condVar, mutex, milliseconds);
}

void PAL_CALL palSignalCondVar(PalCondVar* condVar)
{
    ASSERT(condVar != nullptr, "The specified condvar is null");

    platformSignalCondVar(condVar);
}

void PAL_CALL palBroadcastCondVar(PalCondVar* condVar)
{
    ASSERT(condVar != nullptr, "The specified condvar is null");

    platformBroadcastCondVar(condVar);
}