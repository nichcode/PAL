/**
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

#include "pal2/core/library.h"
#include "pal2/core/memory.h"
#include "pal2/thread/thread.h"
#include "thread/platform.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

typedef HRESULT(WINAPI* SetThreadDescriptionFn)(
    HANDLE handle,
    PCWSTR description);

typedef HRESULT(WINAPI* GetThreadDescriptionFn)(
    HANDLE handle,
    PWSTR* description);

typedef struct ThreadData {
    const PalAllocator* allocator;
    PalThreadFn func;
    void* arg;
    void* retval;
    HANDLE handle;
    volatile LONG refCount;
} ThreadData;

static volatile LONG s_CurrentThreadTLS = (LONG)FLS_OUT_OF_INDEXES;

/* =========================================================================
 * Helper
 * ========================================================================= */

static void releaseThreadData(ThreadData* data)
{
    if (InterlockedDecrement(&data->refCount) == 0) {
        CloseHandle(data->handle);
        palFree(data->allocator, data);
    }
}

static void WINAPI destroyCurrentThreadTLS(void* ptr)
{
    if (ptr) {
        releaseThreadData((ThreadData*)ptr);
    }
}

static DWORD getCurrentThreadTLS(void)
{
    LONG current = InterlockedCompareExchange(&s_CurrentThreadTLS, 0, 0);
    if (current == (LONG)FLS_OUT_OF_INDEXES) {
        DWORD index = FlsAlloc(destroyCurrentThreadTLS);
        if (index == FLS_OUT_OF_INDEXES) {
            return FLS_OUT_OF_INDEXES;
        }

        LONG prev = InterlockedCompareExchange(&s_CurrentThreadTLS, (LONG)index, (LONG)FLS_OUT_OF_INDEXES);

        if (prev != (LONG)FLS_OUT_OF_INDEXES) {
            FlsFree(index);
            return (DWORD)prev;
        }

        return index;
    }

    return (DWORD)current;
}

static PalResult makeResult(DWORD error)
{
    switch (error) {
    case ERROR_NOT_ENOUGH_MEMORY:
        return palMakeResult(PAL_RESULT_CODE_OUT_OF_MEMORY, PAL_RESULT_SOURCE_WIN32, error);

    case ERROR_INVALID_PARAMETER:
        return palMakeResult(PAL_RESULT_CODE_INVALID_ARGUMENT, PAL_RESULT_SOURCE_WIN32, error);

    case ERROR_ACCESS_DENIED:
        return palMakeResult(PAL_RESULT_CODE_INVALID_OPERATION, PAL_RESULT_SOURCE_WIN32, error);

    case ERROR_INVALID_HANDLE:
        return palMakeResult(PAL_RESULT_CODE_INVALID_HANDLE, PAL_RESULT_SOURCE_WIN32, error);
    }

    return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, error);
}

static DWORD WINAPI threadEntryWrapper(LPVOID arg)
{
    ThreadData* data = arg;
    DWORD tls = getCurrentThreadTLS();

    if (tls != FLS_OUT_OF_INDEXES) {
        FlsSetValue(tls, data);
    }

    data->retval = data->func(data->arg);

    // If TLS wasn't available, release manually; otherwise FLS destructor handles it
    if (tls == FLS_OUT_OF_INDEXES) {
        releaseThreadData(data);
    }

    return 0;
}

/* =========================================================================
 * API
 * ========================================================================= */

PalResult platformCreateThread(
    const PalThreadCreateInfo* info,
    PalThread** thread)
{
    ThreadData* data = palAllocate(info->allocator, sizeof(ThreadData), 0);
    if (!data) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    data->allocator = info->allocator;
    data->func = info->entry;
    data->arg = info->arg;
    data->retval = nullptr;
    data->refCount = 2; // 1 for worker thread, 1 for join/detach

    data->handle = CreateThread(nullptr, info->stackSize, threadEntryWrapper, data, CREATE_SUSPENDED, nullptr);

    if (!data->handle) {
        DWORD error = GetLastError();
        palFree(info->allocator, data);
        return makeResult(error);
    }

    *thread = (PalThread*)data;
    ResumeThread(data->handle);

    return PAL_RESULT_SUCCESS;
}

PalResult platformJoinThread(
    PalThread* thread,
    void** retval)
{
    ThreadData* data = (ThreadData*)thread;
    if (WaitForSingleObject(data->handle, INFINITE) != WAIT_OBJECT_0) {
        return makeResult(GetLastError());
    }

    if (retval) {
        *retval = data->retval;
    }

    releaseThreadData(data);
    return PAL_RESULT_SUCCESS;
}

void platformDetachThread(PalThread* thread)
{
    releaseThreadData((ThreadData*)thread);
}

void platformSleep(uint64_t milliseconds)
{
    while (milliseconds > 0) {
        DWORD chunk = (milliseconds >= INFINITE) ? (INFINITE - 1) : (DWORD)milliseconds;
        Sleep(chunk);
        milliseconds -= chunk;
    }
}

void platformYield(void)
{
    SwitchToThread();
}

PalThread* platformGetCurrentThread(void)
{
    DWORD tls = getCurrentThreadTLS();
    if (tls == FLS_OUT_OF_INDEXES) {
        return nullptr;
    }

    return FlsGetValue(tls);
}

PalThreadFeatures platformGetThreadFeatures(void)
{
    PalThreadFeatures features = PAL_THREAD_FEATURE_STACK_SIZE | PAL_THREAD_FEATURE_PRIORITY |
                                 PAL_THREAD_FEATURE_AFFINITY;

    PalLibrary* kernel32 = palLoadLibrary("kernel32.dll");
    if (kernel32) {
        if (palGetSymbol(kernel32, "SetThreadDescription")) {
            features |= PAL_THREAD_FEATURE_NAME;
        }
        palFreeLibrary(kernel32);
    }

    return features;
}

PalThreadPriority platformGetThreadPriority(PalThread* thread)
{
    ThreadData* data = (ThreadData*)thread;
    switch (GetThreadPriority(data->handle)) {
    case THREAD_PRIORITY_LOWEST:
    case THREAD_PRIORITY_BELOW_NORMAL:
    case THREAD_PRIORITY_IDLE:
        return PAL_THREAD_PRIORITY_LOW;

    case THREAD_PRIORITY_HIGHEST:
    case THREAD_PRIORITY_ABOVE_NORMAL:
    case THREAD_PRIORITY_TIME_CRITICAL:
        return PAL_THREAD_PRIORITY_HIGH;

    default:
        return PAL_THREAD_PRIORITY_NORMAL;
    }
}

uint64_t platformGetThreadAffinity(PalThread* thread)
{
    DWORD_PTR processMask = 0;
    DWORD_PTR systemMask = 0;
    if (!GetProcessAffinityMask(GetCurrentProcess(), &processMask, &systemMask) || processMask == 0) {
        return 0;
    }

    ThreadData* data = (ThreadData*)thread;
    DWORD_PTR mask = SetThreadAffinityMask(data->handle, processMask);
    if (mask == 0) {
        return 0;
    }

    SetThreadAffinityMask(data->handle, mask);
    return (uint64_t)mask;
}

void platformGetThreadName(
    PalThread* thread,
    size_t bufferSize,
    size_t* size,
    char* buffer)
{
    if (size) {
        *size = 0;
    }
    if (buffer && bufferSize > 0) {
        buffer[0] = '\0';
    }

    PalLibrary* kernel32 = palLoadLibrary("kernel32.dll");
    if (!kernel32) {
        return;
    }

    GetThreadDescriptionFn getThreadDesc = (GetThreadDescriptionFn)palGetSymbol(kernel32, "GetThreadDescription");
    if (!getThreadDesc) {
        palFreeLibrary(kernel32);
        return;
    }

    ThreadData* data = (ThreadData*)thread;
    wchar_t* tmp = nullptr;
    if (FAILED(getThreadDesc(data->handle, &tmp)) || !tmp) {
        palFreeLibrary(kernel32);
        return;
    }

    int len = WideCharToMultiByte(CP_UTF8, 0, tmp, -1, nullptr, 0, nullptr, nullptr);
    if (len > 0) {
        if (size) {
            *size = (size_t)len;
        }
        if (buffer && bufferSize > 0) {
            WideCharToMultiByte(CP_UTF8, 0, tmp, -1, buffer, (int)bufferSize, nullptr, nullptr);
            buffer[bufferSize - 1] = '\0';
        }
    }

    LocalFree(tmp);
    palFreeLibrary(kernel32);
}

PalResult platformSetThreadPriority(
    PalThread* thread,
    PalThreadPriority priority)
{
    int nativePriority = THREAD_PRIORITY_NORMAL;
    switch (priority) {
    case PAL_THREAD_PRIORITY_LOW:
        nativePriority = THREAD_PRIORITY_LOWEST;
        break;

    case PAL_THREAD_PRIORITY_NORMAL:
        nativePriority = THREAD_PRIORITY_NORMAL;
        break;

    case PAL_THREAD_PRIORITY_HIGH:
        nativePriority = THREAD_PRIORITY_HIGHEST;
        break;
    }

    ThreadData* data = (ThreadData*)thread;
    if (!SetThreadPriority(data->handle, nativePriority)) {
        return makeResult(GetLastError());
    }

    return PAL_RESULT_SUCCESS;
}

PalResult platformSetThreadAffinity(
    PalThread* thread,
    uint64_t mask)
{
    ThreadData* data = (ThreadData*)thread;
    if (!SetThreadAffinityMask(data->handle, (DWORD_PTR)mask)) {
        return makeResult(GetLastError());
    }

    return PAL_RESULT_SUCCESS;
}

PalResult platformSetThreadName(
    PalThread* thread,
    const char* name)
{
    PalLibrary* kernel32 = palLoadLibrary("kernel32.dll");
    if (!kernel32) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    SetThreadDescriptionFn setThreadDesc = (SetThreadDescriptionFn)palGetSymbol(
        kernel32, 
        "SetThreadDescription");

    if (!setThreadDesc) {
        palFreeLibrary(kernel32);
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    wchar_t buffer[128] = {0};
    MultiByteToWideChar(CP_UTF8, 0, name, -1, buffer, 127);
    ThreadData* data = (ThreadData*)thread;
    
    HRESULT hr = setThreadDesc(data->handle, buffer);
    palFreeLibrary(kernel32);

    if (FAILED(hr)) {
        switch (hr) {
        case E_INVALIDARG:
            return makeResult(ERROR_INVALID_PARAMETER);
        case E_OUTOFMEMORY:
            return makeResult(ERROR_NOT_ENOUGH_MEMORY);
        case E_ACCESSDENIED:
            return makeResult(ERROR_ACCESS_DENIED);
        default:
            return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, (DWORD)hr);
        }
    }

    return PAL_RESULT_SUCCESS;
}

#endif // _WIN32
