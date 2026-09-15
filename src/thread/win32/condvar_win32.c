
/**
 * PAL - Prime Abstraction Layer (PAL)
 * A cross platform abstraction layer over graphics and windowing APIs
 * -------------------------------------------------------------------
 *
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "shared.h"
#include "thread/thread_platform.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include "thread_win32.h"

PalResult platformCreateCondVar(
    const PalAllocator* allocator,
    PalCondVar** condVar)
{
    PalCondVar* condVarImpl = palAllocate(allocator, sizeof(PalCondVar), 0);
    if (!condVarImpl) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    InitializeConditionVariable(&condVarImpl->handle);
    condVarImpl->allocator = allocator;
    
    *condVar = condVarImpl;

    return PAL_RESULT_SUCCESS;
}

void platformDestroyCondVar(PalCondVar* condVar)
{
    palFree(condVar->allocator, condVar);
}

PalResult platformWaitCondVar(
    PalCondVar* condVar,
    PalMutex* mutex)
{
    BOOL ret = SleepConditionVariableCS(
        &condVar->handle, 
        &mutex->handle, 
        INFINITE);

    if (!ret) {
        DWORD error = GetLastError();

        if (error == ERROR_TIMEOUT) {
            return palMakeResult(
                PAL_RESULT_CODE_TIMEOUT, 
                PAL_RESULT_SOURCE_WIN32, 
                error);

        } else {
            return palMakeResult(
                PAL_RESULT_CODE_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WIN32, 
                error);
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult platformWaitCondVarTimeout(
    PalCondVar* condVar,
    PalMutex* mutex,
    uint64_t milliseconds)
{
    BOOL ret = SleepConditionVariableCS(
        &condVar->handle, 
        &mutex->handle, 
        (DWORD)milliseconds);

    if (!ret) {
        DWORD error = GetLastError();

        if (error == ERROR_TIMEOUT) {
            return palMakeResult(
                PAL_RESULT_CODE_TIMEOUT, 
                PAL_RESULT_SOURCE_WIN32, 
                error);

        } else {
            return palMakeResult(
                PAL_RESULT_CODE_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WIN32, 
                error);
        }
    }

    return PAL_RESULT_SUCCESS;
}

void platformSignalCondVar(PalCondVar* condVar)
{
    WakeConditionVariable(&condVar->handle);
}

void platformBroadcastCondVar(PalCondVar* condVar)
{
    WakeAllConditionVariable(&condVar->handle);
}

#endif // _WIN32