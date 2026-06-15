
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef __linux__
#include "pal_thread_common_linux.h"
#include "pal_shared.h"

PalResult PAL_CALL palCreateCondVar(
    const PalAllocator* allocator,
    PalCondVar** outCondVar)
{
    if (!outCondVar) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (allocator) {
        if (!allocator->allocate && !allocator->free) {
            return palMakeResult(
                PAL_RESULT_INVALID_ARGUMENT, 
                PAL_RESULT_SOURCE_LINUX, 
                errno);
        }
    }

    PalCondVar* condVar = palAllocate(allocator, sizeof(PalCondVar), 0);
    if (!condVar) {
        return palMakeResult(
            PAL_RESULT_OUT_OF_MEMORY, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
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
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    int ret = pthread_cond_wait(&condVar->handle, &mutex->handle);
    if (ret == 0) {
        return PAL_RESULT_SUCCESS;

    } else if (ret == ETIMEDOUT) {
        return palMakeResult(
            PAL_RESULT_TIMEOUT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);

    } else {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }
}

PalResult PAL_CALL palWaitCondVarTimeout(
    PalCondVar* condVar,
    PalMutex* mutex,
    uint64_t milliseconds)
{
    if (!condVar || !mutex) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
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
        return palMakeResult(
            PAL_RESULT_TIMEOUT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
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

#endif // __linux__