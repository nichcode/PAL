
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal_platform.h"

#if _PAL_HAS_POSIX
#include "pal_thread_posix.h"

PalResult PAL_CALL palCreateCondVar(
    const PalAllocator* allocator,
    PalCondVar** outCondVar)
{
    if (!outCondVar) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (allocator) {
        if (!allocator->allocate && !allocator->free) {
            return PAL_RESULT_CODE_INVALID_ARGUMENT;
        }
    }

    PalCondVar* condVar = palAllocate(allocator, sizeof(PalCondVar), 0);
    if (!condVar) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
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
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    int ret = pthread_cond_wait(&condVar->handle, &mutex->handle);
    if (ret == 0) {
        return PAL_RESULT_SUCCESS;

    } else if (ret == ETIMEDOUT) {
        return palMakeResult(PAL_RESULT_CODE_TIMEOUT, PAL_RESULT_SOURCE_POSIX, errno);

    } else {
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_POSIX, errno);
    }
}

PalResult PAL_CALL palWaitCondVarTimeout(
    PalCondVar* condVar,
    PalMutex* mutex,
    uint64_t milliseconds)
{
    if (!condVar || !mutex) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
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
        return palMakeResult(PAL_RESULT_CODE_TIMEOUT, PAL_RESULT_SOURCE_POSIX, errno);
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

#endif // _PAL_HAS_POSIX