
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

#include <windows.h>

typedef HRESULT(WINAPI* SetThreadDescriptionFn)(
    HANDLE handle,
    PCWSTR description);

typedef HRESULT(WINAPI* GetThreadDescriptionFn)(
    HANDLE handle,
    PWSTR* description);

typedef struct ThreadData
{
    const PalAllocator* allocator;
    PalThreadFn func;
    void* arg;
} ThreadData;

static DWORD WINAPI threadEntryWrapper(LPVOID arg)
{
    ThreadData* data = arg;

    void* ret = data->func(data->arg);
    palFree(data->allocator, data);

    return (DWORD)(uintptr_t)ret;
}

static PalResult makeResult(DWORD error)
{
    switch (error) {
    case ERROR_NOT_ENOUGH_MEMORY:
    {
        return palMakeResult(
            PAL_RESULT_CODE_OUT_OF_MEMORY, 
            PAL_RESULT_SOURCE_WIN32, 
            error);
    }

    case ERROR_INVALID_PARAMETER:
    {
        return palMakeResult(
            PAL_RESULT_CODE_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WIN32, 
            error);
    }

    case ERROR_ACCESS_DENIED:
    {
        return palMakeResult(
            PAL_RESULT_CODE_INVALID_OPERATION, 
            PAL_RESULT_SOURCE_WIN32, 
            error);
    }

    case ERROR_INVALID_HANDLE:
    {
        return palMakeResult(
            PAL_RESULT_CODE_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_WIN32, 
            error);
    }
    }

    return palMakeResult(
        PAL_RESULT_CODE_PLATFORM_FAILURE, 
        PAL_RESULT_SOURCE_WIN32, 
        error);

}

PalResult platformCreateThread(
    const PalThreadCreateInfo* info,
    PalThread** thread)
{
    ThreadData* data = palAllocate(info->allocator, sizeof(ThreadData), 0);
    if (!data) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    data->arg = info->arg;
    data->func = info->entry;
    data->allocator = info->allocator;

    HANDLE handle = CreateThread(
        nullptr, 
        info->stackSize, 
        threadEntryWrapper, 
        data, 
        0, 
        nullptr);

    if (!thread) {
        return makeResult(GetLastError());
    }

    *thread = (PalThread*)handle;

    return PAL_RESULT_SUCCESS;
}

PalResult platformJoinThread(
    PalThread* thread,
    void** retval)
{
    DWORD wait = WaitForSingleObject((HANDLE)thread, INFINITE);

    if (wait == WAIT_OBJECT_0) {
        if (retval) {
            uintptr_t ret;
            GetExitCodeThread((HANDLE)thread, (LPDWORD)&ret);
            *retval = (void*)ret;
        }

        CloseHandle((HANDLE)thread);

    } else if (wait == WAIT_FAILED) {
        return palMakeResult(
            PAL_RESULT_CODE_INVALID_HANDLE,
            PAL_RESULT_SOURCE_WIN32,
            GetLastError());
    }

    return PAL_RESULT_SUCCESS;
}

void platformDetachThread(PalThread* thread)
{
    CloseHandle((HANDLE)thread);
}

void platformSleep(uint64_t milliseconds)
{
    Sleep((DWORD)milliseconds);
}

void platformYield()
{
    SwitchToThread();
}

PalThread* platformGetCurrentThread()
{
    return (PalThread*)GetCurrentThread();
}

PalThreadFeatures platformGetThreadFeatures()
{
    PalThreadFeatures features = 0;
    features |= PAL_THREAD_FEATURE_STACK_SIZE;
    features |= PAL_THREAD_FEATURE_PRIORITY;
    features |= PAL_THREAD_FEATURE_AFFINITY;

    HINSTANCE kernel32 = GetModuleHandleW(L"kernel32.dll");
    if (kernel32) {
        FARPROC setThreadDesc = GetProcAddress(kernel32, "SetThreadDescription");
        if (setThreadDesc) {
            features |= PAL_THREAD_FEATURE_NAME;
        }
    }

    return features;
}

PalThreadPriority platformGetThreadPriority(PalThread* thread)
{
    int priority = GetThreadPriority((HANDLE)thread);

    switch (priority) {
    case THREAD_PRIORITY_LOWEST:
        return PAL_THREAD_PRIORITY_LOW;

    case THREAD_PRIORITY_NORMAL:
        return PAL_THREAD_PRIORITY_NORMAL;

    case THREAD_PRIORITY_HIGHEST:
        return PAL_THREAD_PRIORITY_HIGH;
    }

    return 0;
}

uint64_t platformGetThreadAffinity(PalThread* thread)
{
    /** We set an invalid mask which gives us the correct mask, store it and
     * set it back. This is done because win32 has no way to get the affinity.
    */
    DWORD_PTR mask = SetThreadAffinityMask((HANDLE)thread, ~0ull);
    if (mask == 0) {
        return 0;
    }

    SetThreadAffinityMask((HANDLE)thread, mask);

    return mask;
}

void platformGetThreadName(
    PalThread* thread,
    uint64_t bufferSize,
    uint64_t* size,
    char* buffer)
{
    GetThreadDescriptionFn getThreadDesc = nullptr;
    HINSTANCE kernel32 = GetModuleHandleW(L"kernel32.dll");
    FARPROC proc = GetProcAddress(kernel32, "GetThreadDescription");
    getThreadDesc = (GetThreadDescriptionFn)proc;

    wchar_t* tmp = nullptr;
    getThreadDesc((HANDLE)thread, &tmp);

    int len = WideCharToMultiByte(CP_UTF8, 0, tmp, -1, nullptr, 0, 0, 0);
    if (size) {
        *size = len - 1;
    }

    if (buffer && bufferSize > 0) {
        int write = (int)bufferSize - 1;
        WideCharToMultiByte(CP_UTF8, 0, tmp, -1, buffer, write + 1, 0, 0);
        buffer[write < len - 1 ? write : len - 1] = '\0';

        LocalFree(tmp);
    }
}

PalResult platformSetThreadPriority(
    PalThread* thread,
    PalThreadPriority priority)
{
    int tmp = 0;

    switch (priority) {
    case PAL_THREAD_PRIORITY_LOW: {
        tmp = THREAD_PRIORITY_LOWEST;
        break;
    }
    
    case PAL_THREAD_PRIORITY_NORMAL: {
        tmp = THREAD_PRIORITY_NORMAL;
        break;
    }
        
    case PAL_THREAD_PRIORITY_HIGH: {
        tmp = THREAD_PRIORITY_HIGHEST;
        break;
    }
    }

    if (!SetThreadPriority((HANDLE)thread, tmp)) {
        return makeResult(GetLastError());
    }

    return PAL_RESULT_SUCCESS;
}

PalResult platformSetThreadAffinity(
    PalThread* thread,
    uint64_t mask)
{
    if (!SetThreadAffinityMask((HANDLE)thread, mask)) {
        return makeResult(GetLastError());
    }

    return PAL_RESULT_SUCCESS;
}

PalResult platformSetThreadName(
    PalThread* thread,
    const char* name)
{
    HINSTANCE kernel32 = GetModuleHandleW(L"kernel32.dll");
    SetThreadDescriptionFn setThreadDesc = nullptr;
    FARPROC proc = GetProcAddress(kernel32, "SetThreadDescription");
    setThreadDesc = (SetThreadDescriptionFn)proc;

    wchar_t buffer[128] = {0};
    MultiByteToWideChar(CP_UTF8, 0, name, -1, buffer, 128);
    HRESULT hr = setThreadDesc((HANDLE)thread, buffer);

    if (FAILED(hr)) {
        switch (hr) {
        case E_INVALIDARG: {
            return palMakeResult(
                PAL_RESULT_CODE_INVALID_HANDLE, 
                PAL_RESULT_SOURCE_WIN32, 
                hr);
        }

        case E_OUTOFMEMORY: {
            return palMakeResult(
                PAL_RESULT_CODE_OUT_OF_MEMORY, 
                PAL_RESULT_SOURCE_WIN32, 
                hr);
        }

        case E_ACCESSDENIED: {
            return palMakeResult(
                PAL_RESULT_CODE_INVALID_OPERATION, 
                PAL_RESULT_SOURCE_WIN32, 
                hr);
        }
        }

        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_WIN32, 
            hr);
    }

    return PAL_RESULT_SUCCESS;
}

#endif // _WIN32