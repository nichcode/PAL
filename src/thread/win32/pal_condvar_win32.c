
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef _WIN32
#include "pal_thread_win32.h"
#include "pal_shared.h"

PalResult PAL_CALL palCreateCondVar(
    const PalAllocator* allocator,
    PalCondVar** outCondVar)
{
    if (!outCondVar) {
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

    PalCondVar* condVar = palAllocate(allocator, sizeof(PalCondVar), 0);
    if (!condVar) {
        return palMakeResult(
            PAL_RESULT_OUT_OF_MEMORY, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    InitializeConditionVariable(&condVar->cv);
    condVar->allocator = allocator;
    *outCondVar = condVar;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyCondVar(PalCondVar* condVar)
{
    if (condVar) {
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
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    BOOL ret = SleepConditionVariableCS(&condVar->cv, &mutex->sc, INFINITE);
    if (!ret) {
        DWORD error = GetLastError();
        if (error == ERROR_TIMEOUT) {
            return palMakeResult(
                PAL_RESULT_TIMEOUT, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);

        } else {
            return palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);
        }
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palWaitCondVarTimeout(
    PalCondVar* condVar,
    PalMutex* mutex,
    uint64_t milliseconds)
{
    if (!condVar || !mutex) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    BOOL ret = SleepConditionVariableCS(&condVar->cv, &mutex->sc, (DWORD)milliseconds);
    if (!ret) {
        DWORD error = GetLastError();
        if (error == ERROR_TIMEOUT) {
            return palMakeResult(
                PAL_RESULT_TIMEOUT, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);

        } else {
            return palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);
        }
    }
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palSignalCondVar(PalCondVar* condVar)
{
    if (condVar) {
        WakeConditionVariable(&condVar->cv);
    }
}

void PAL_CALL palBroadcastCondVar(PalCondVar* condVar)
{
    if (condVar) {
        WakeAllConditionVariable(&condVar->cv);
    }
}

#endif // _WIN32