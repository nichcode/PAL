
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L
#include "pal/thread/condvar.h"
#include <time.h>
#include <errno.h>
#include <pthread.h>

struct PalCondVar {
    const PalAllocator* allocator;
    pthread_cond_t handle;
};

struct PalMutex {
    const PalAllocator* allocator;
    pthread_mutex_t handle;
};

PalResult PAL_CALL palCreateCondVar(
    const PalAllocator* allocator,
    PalCondVar** outCondVar)
{
    if (!outCondVar) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (allocator) {
        if (!allocator->allocate && !allocator->free) {
            return PAL_RESULT_INVALID_ALLOCATOR;
        }
    }

    PalCondVar* condVar = palAllocate(allocator, sizeof(PalCondVar), 0);
    if (!condVar) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    pthread_cond_init(&condVar->handle, nullptr);
    condVar->allocator = allocator;
    *outCondVar = condVar;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyCondVar(PalCondVar* condVar)
{
    if (condVar) {
        pthread_cond_destroy(&condVar->handle);
        palFree(condVar->allocator, condVar);
    }
}

PalResult PAL_CALL palWaitCondVar(
    PalCondVar* condVar,
    PalMutex* mutex)
{
    if (!condVar || !mutex) {
        return PAL_RESULT_NULL_POINTER;
    }

    int ret = pthread_cond_wait(&condVar->handle, &mutex->handle);
    if (ret == 0) {
        return PAL_RESULT_SUCCESS;
    } else if (ret == ETIMEDOUT) {
        return PAL_RESULT_TIMEOUT;
    } else {
        return PAL_RESULT_PLATFORM_FAILURE;
    }
}

PalResult PAL_CALL palWaitCondVarTimeout(
    PalCondVar* condVar,
    PalMutex* mutex,
    uint64_t milliseconds)
{
    if (!condVar || !mutex) {
        return PAL_RESULT_NULL_POINTER;
    }

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += milliseconds / 1000;
    ts.tv_nsec += (milliseconds % 1000) * 1000000;

    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    if (pthread_cond_timedwait(&condVar->handle, &mutex->handle, &ts) != 0) {
        return PAL_RESULT_TIMEOUT;
    }
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palSignalCondVar(PalCondVar* condVar)
{
    if (condVar) {
        pthread_cond_signal(&condVar->handle);
    }
}

void PAL_CALL palBroadcastCondVar(PalCondVar* condVar)
{
    if (condVar) {
        pthread_cond_broadcast(&condVar->handle);
    }
}