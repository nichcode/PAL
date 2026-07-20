
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef _WIN32
#include "pal_thread_win32.h"

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

    InitializeConditionVariable(&condVar->cv);
    condVar->allocator = allocator;
    *outCondVar = condVar;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyCondVar(PalCondVar* condVar)
{
    palFree(condVar->allocator, condVar);
}

PalResult PAL_CALL palWaitCondVar(
    PalCondVar* condVar,
    PalMutex* mutex)
{
    BOOL ret = SleepConditionVariableCS(&condVar->cv, &mutex->sc, INFINITE);
    if (!ret) {
        DWORD error = GetLastError();
        if (error == ERROR_TIMEOUT) {
            return palMakeResult(PAL_RESULT_CODE_TIMEOUT, PAL_RESULT_SOURCE_WIN32, error);

        } else {
            return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, error);
        }
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palWaitCondVarTimeout(
    PalCondVar* condVar,
    PalMutex* mutex,
    uint64_t milliseconds)
{
    BOOL ret = SleepConditionVariableCS(&condVar->cv, &mutex->sc, (DWORD)milliseconds);
    if (!ret) {
        DWORD error = GetLastError();
        if (error == ERROR_TIMEOUT) {
            return palMakeResult(PAL_RESULT_CODE_TIMEOUT, PAL_RESULT_SOURCE_WIN32, error);

        } else {
            return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, error);
        }
    }
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palSignalCondVar(PalCondVar* condVar)
{
    WakeConditionVariable(&condVar->cv);
}

void PAL_CALL palBroadcastCondVar(PalCondVar* condVar)
{
    WakeAllConditionVariable(&condVar->cv);
}

#endif // _WIN32