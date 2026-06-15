
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef _WIN32
#include "pal_thread_common_win32.h"
#include "pal_shared.h"

PalResult PAL_CALL palCreateMutex(
    const PalAllocator* allocator,
    PalMutex** outMutex)
{
    if (!outMutex) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (allocator) {
        if (!allocator->allocate && !allocator->free) {
            return palMakeResult(
                PAL_RESULT_INVALID_ARGUMENT, 
                PAL_RESULT_SOURCE_WINDOWS, 
                GetLastError());
        }
    }

    PalMutex* mutex = palAllocate(allocator, sizeof(PalMutex), 0);
    if (!mutex) {
        return palMakeResult(
            PAL_RESULT_OUT_OF_MEMORY, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    InitializeCriticalSection(&mutex->sc);
    mutex->allocator = allocator;
    *outMutex = mutex;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyMutex(PalMutex* mutex)
{
    if (mutex) {
        DeleteCriticalSection(&mutex->sc);
        palFree(mutex->allocator, mutex);
    }
}

void PAL_CALL palLockMutex(PalMutex* mutex)
{
    if (mutex) {
        EnterCriticalSection(&mutex->sc);
    }
}

void PAL_CALL palUnlockMutex(PalMutex* mutex)
{
    if (mutex) {
        LeaveCriticalSection(&mutex->sc);
    }
}

#endif // _WIN32