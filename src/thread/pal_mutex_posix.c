
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L
#include "pal/thread/mutex.h"
#include <pthread.h>

struct PalMutex {
    const PalAllocator* allocator;
    pthread_mutex_t handle;
};

PalResult PAL_CALL palCreateMutex(
    const PalAllocator* allocator,
    PalMutex** outMutex)
{
    if (!outMutex) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (allocator) {
        if (!allocator->allocate && !allocator->free) {
            return PAL_RESULT_INVALID_ALLOCATOR;
        }
    }

    PalMutex* mutex = palAllocate(allocator, sizeof(PalMutex), 0);
    if (!mutex) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    pthread_mutex_init(&mutex->handle, nullptr);
    mutex->allocator = allocator;
    *outMutex = mutex;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyMutex(PalMutex* mutex)
{
    if (mutex) {
        pthread_mutex_destroy(&mutex->handle);
        palFree(mutex->allocator, mutex);
    }
}

void PAL_CALL palLockMutex(PalMutex* mutex)
{
    if (mutex) {
        pthread_mutex_lock(&mutex->handle);
    }
}

void PAL_CALL palUnlockMutex(PalMutex* mutex)
{
    if (mutex) {
        pthread_mutex_unlock(&mutex->handle);
    }
}