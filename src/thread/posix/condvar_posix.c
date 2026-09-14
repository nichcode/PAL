
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
#include "thread/thread_platform.h"

#if PLATFORM_POSIX
#include "thread_posix.h"
#include <errno.h>

PalResult platformCreateCondVar(
    const PalAllocator* allocator,
    PalCondVar** condVar)
{
    PalCondVar* pcondVar = palAllocate(allocator, sizeof(PalCondVar), 0);
    if (!pcondVar) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    pthread_cond_init(&pcondVar->handle, nullptr);
    pcondVar->allocator = allocator;
    
    *condVar = pcondVar;
    
    return PAL_RESULT_SUCCESS;
}

void platformDestroyCondVar(PalCondVar* condVar)
{
    pthread_cond_destroy(&condVar->handle);
    palFree(condVar->allocator, condVar);
}

PalResult platformWaitCondVar(
    PalCondVar* condVar,
    PalMutex* mutex)
{
    int ret = pthread_cond_wait(&condVar->handle, &mutex->handle);
    if (ret == 0) {
        return PAL_RESULT_SUCCESS;
    }

    if (ret == ETIMEDOUT) {
        return palMakeResult(
            PAL_RESULT_CODE_TIMEOUT, 
            PAL_RESULT_SOURCE_POSIX, 
            (uint32_t)errno);

    } else {
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_POSIX, 
            (uint32_t)errno);
    }
}

PalResult platformWaitCondVarTimeout(
    PalCondVar* condVar,
    PalMutex* mutex,
    uint64_t milliseconds)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += (time_t)milliseconds / 1000;
    ts.tv_nsec += (long)(milliseconds % 1000) * 1000000;

    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    if (pthread_cond_timedwait(&condVar->handle, &mutex->handle, &ts) != 0) {
        return palMakeResult(
            PAL_RESULT_CODE_TIMEOUT, 
            PAL_RESULT_SOURCE_POSIX, 
            (uint32_t)errno);
    }

    return PAL_RESULT_SUCCESS;
}

void platformSignalCondVar(PalCondVar* condVar)
{
    pthread_cond_signal(&condVar->handle);
}

void platformBroadcastCondVar(PalCondVar* condVar)
{
    pthread_cond_broadcast(&condVar->handle);
}

#endif // PLATFORM_POSIX