
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef __linux__
#include "pal_thread_common_linux.h"
#include "pal_shared.h"

PalResult PAL_CALL palCreateMutex(
    const PalAllocator* allocator,
    PalMutex** outMutex)
{
    if (!outMutex) {
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

    PalMutex* mutex = palAllocate(allocator, sizeof(PalMutex), 0);
    if (!mutex) {
        return palMakeResult(
            PAL_RESULT_OUT_OF_MEMORY, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
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

#endif // __linux__