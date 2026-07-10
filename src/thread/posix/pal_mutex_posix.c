
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal_platform.h"

#if _PAL_HAS_POSIX
#include "pal_thread_posix.h"

PalResult PAL_CALL palCreateMutex(
    const PalAllocator* allocator,
    PalMutex** outMutex)
{
    if (!outMutex) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (allocator) {
        if (!allocator->allocate && !allocator->free) {
            return PAL_RESULT_CODE_INVALID_ARGUMENT;
        }
    }

    PalMutex* mutex = palAllocate(allocator, sizeof(PalMutex), 0);
    if (!mutex) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    pthread_mutex_init(&mutex->handle, nullptr);
    mutex->allocator = allocator;
    *outMutex = mutex;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyMutex(PalMutex* mutex)
{
    pthread_mutex_destroy(&mutex->handle);
    palFree(mutex->allocator, mutex);
}

void PAL_CALL palLockMutex(PalMutex* mutex)
{
    pthread_mutex_lock(&mutex->handle);
}

void PAL_CALL palUnlockMutex(PalMutex* mutex)
{
    pthread_mutex_unlock(&mutex->handle);
}

#endif // _PAL_HAS_POSIX