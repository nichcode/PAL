
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef _WIN32
#include "pal_thread_win32.h"

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
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_WIN32, 
            GetLastError());
    }

    InitializeCriticalSection(&mutex->sc);
    mutex->allocator = allocator;
    *outMutex = mutex;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyMutex(PalMutex* mutex)
{
    DeleteCriticalSection(&mutex->sc);
    palFree(mutex->allocator, mutex);
}

void PAL_CALL palLockMutex(PalMutex* mutex)
{
    EnterCriticalSection(&mutex->sc);
}

void PAL_CALL palUnlockMutex(PalMutex* mutex)
{
    LeaveCriticalSection(&mutex->sc);
}

#endif // _WIN32