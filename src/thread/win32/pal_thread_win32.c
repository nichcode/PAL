
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef _WIN32
#include "pal_thread_win32.h"

typedef HRESULT(WINAPI* SetThreadDescriptionFn)(
    HANDLE,
    PCWSTR);

typedef HRESULT(WINAPI* GetThreadDescriptionFn)(
    HANDLE,
    PWSTR*);

typedef struct {
    const PalAllocator* allocator;
    PalThreadFn func;
    void* arg;
} ThreadData;

static DWORD WINAPI threadEntryToWin32(LPVOID arg)
{
    ThreadData* data = arg;
    void* ret = data->func(data->arg);
    palFree(data->allocator, data);
    return (DWORD)(uintptr_t)ret;
}

PalResult PAL_CALL palCreateThread(
    const PalThreadCreateInfo* info,
    PalThread** outThread)
{
    if (!info || !outThread) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (info->allocator) {
        if (!info->allocator->allocate && !info->allocator->free) {
            return PAL_RESULT_CODE_INVALID_ARGUMENT;
        }
    }

    // create thread
    ThreadData* data = palAllocate(info->allocator, sizeof(ThreadData), 0);
    if (!data) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    data->arg = info->arg;
    data->func = info->entry;
    data->allocator = info->allocator;

    HANDLE thread = CreateThread(nullptr, info->stackSize, threadEntryToWin32, data, 0, nullptr);
    if (!thread) {
        // error
        DWORD error = GetLastError();
        if (error == ERROR_NOT_ENOUGH_MEMORY) {
            return palMakeResult(
                PAL_RESULT_CODE_OUT_OF_MEMORY, 
                PAL_RESULT_SOURCE_WIN32, 
                error);

        } else if (error == ERROR_INVALID_PARAMETER) {
            return palMakeResult(
                PAL_RESULT_CODE_INVALID_ARGUMENT, 
                PAL_RESULT_SOURCE_WIN32, 
                error);

        } else if (error == ERROR_ACCESS_DENIED) {
            return palMakeResult(
                PAL_RESULT_CODE_INVALID_OPERATION, 
                PAL_RESULT_SOURCE_WIN32, 
                error);

        } else {
            return palMakeResult(
                PAL_RESULT_CODE_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WIN32, 
                error);
        }
    }

    *outThread = (PalThread*)thread;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palJoinThread(
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
        // thread is done destroy the HANDLE
        CloseHandle((HANDLE)thread);

    } else if (wait == WAIT_FAILED) {
        return palMakeResult(
            PAL_RESULT_CODE_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_WIN32, 
            GetLastError());
    }

    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDetachThread(PalThread* thread)
{
    CloseHandle((HANDLE)thread);
}

void PAL_CALL palSleep(uint64_t milliseconds)
{
    Sleep((DWORD)milliseconds);
}

void PAL_CALL palYield()
{
    SwitchToThread();
}

PalThread* PAL_CALL palGetCurrentThread()
{
    return (PalThread*)GetCurrentThread();
}

PalThreadFeatures PAL_CALL palGetThreadFeatures()
{
    PalThreadFeatures features = 0;
    features |= PAL_THREAD_FEATURE_STACK_SIZE;
    features |= PAL_THREAD_FEATURE_PRIORITY;
    features |= PAL_THREAD_FEATURE_AFFINITY;

    // check support for PAL_THREAD_FEATURE_NAME feature
    HINSTANCE kernel32 = GetModuleHandleW(L"kernel32.dll");
    if (kernel32) {
        FARPROC setThreadDesc = GetProcAddress(kernel32, "SetThreadDescription");

        if (setThreadDesc) {
            features |= PAL_THREAD_FEATURE_NAME;
        }
    }
    return features;
}

PalThreadPriority PAL_CALL palGetThreadPriority(PalThread* thread)
{
    int priority = GetThreadPriority((HANDLE)thread);
    switch (priority) {
        case THREAD_PRIORITY_LOWEST:
            return PAL_THREAD_PRIORITY_LOW;
            break;

        case THREAD_PRIORITY_NORMAL:
            return PAL_THREAD_PRIORITY_NORMAL;
            break;

        case THREAD_PRIORITY_HIGHEST:
            return PAL_THREAD_PRIORITY_HIGH;
            break;
    }

    return 0;
}

uint64_t PAL_CALL palGetThreadAffinity(PalThread* thread)
{
    DWORD_PTR mask = SetThreadAffinityMask((HANDLE)thread, ~0ull);
    if (mask == 0) {
        return 0;
    }

    SetThreadAffinityMask((HANDLE)thread, mask);
    return mask;
}

void PAL_CALL palGetThreadName(
    PalThread* thread,
    uint64_t bufferSize,
    uint64_t* outSize,
    char* outBuffer)
{
    HINSTANCE kernel32 = GetModuleHandleW(L"kernel32.dll");
    GetThreadDescriptionFn getThreadDesc = nullptr;
    getThreadDesc = (GetThreadDescriptionFn)GetProcAddress(kernel32, "GetThreadDescription");

    wchar_t* buffer = nullptr;
    getThreadDesc((HANDLE)thread, &buffer);
    int len = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, nullptr, 0, 0, 0);
    if (outSize) {
        *outSize = len - 1;
    }

    // see if user provided a buffer and write to it
    if (outBuffer && bufferSize > 0) {
        int write = (int)bufferSize - 1;
        WideCharToMultiByte(CP_UTF8, 0, buffer, -1, outBuffer, write + 1, 0, 0);
        outBuffer[write < len - 1 ? write : len - 1] = '\0';
        LocalFree(buffer);
    }
}

PalResult PAL_CALL palSetThreadPriority(
    PalThread* thread,
    PalThreadPriority priority)
{
    int _priority = 0;
    switch (priority) {
        case PAL_THREAD_PRIORITY_LOW:
            _priority = THREAD_PRIORITY_LOWEST;
            break;

        case PAL_THREAD_PRIORITY_NORMAL:
            _priority = THREAD_PRIORITY_NORMAL;
            break;

        case PAL_THREAD_PRIORITY_HIGH:
            _priority = THREAD_PRIORITY_HIGHEST;
            break;
    }

    if (!SetThreadPriority((HANDLE)thread, _priority)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return palMakeResult(
                PAL_RESULT_CODE_INVALID_HANDLE, 
                PAL_RESULT_SOURCE_WIN32, 
                error);

        } else if (error == ERROR_ACCESS_DENIED) {
            return palMakeResult(
                PAL_RESULT_CODE_INVALID_OPERATION, 
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

PalResult PAL_CALL palSetThreadAffinity(
    PalThread* thread,
    uint64_t mask)
{
    if (!SetThreadAffinityMask((HANDLE)thread, mask)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE || error == ERROR_INVALID_PARAMETER) {
            return palMakeResult(
                PAL_RESULT_CODE_INVALID_ARGUMENT, 
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

PalResult PAL_CALL palSetThreadName(
    PalThread* thread,
    const char* name)
{
    HINSTANCE kernel32 = GetModuleHandleW(L"kernel32.dll");
    SetThreadDescriptionFn setThreadDesc = nullptr;
    setThreadDesc = (SetThreadDescriptionFn)GetProcAddress(kernel32, "SetThreadDescription");

    wchar_t buffer[128] = {0};
    MultiByteToWideChar(CP_UTF8, 0, name, -1, buffer, 128);
    HRESULT hr = setThreadDesc((HANDLE)thread, buffer);

    if (SUCCEEDED(hr)) {
        return PAL_RESULT_SUCCESS;

    } else {
        if (hr == E_INVALIDARG) {
            return palMakeResult(
                PAL_RESULT_CODE_INVALID_HANDLE, 
                PAL_RESULT_SOURCE_WIN32, 
                hr);

        } else if (hr == E_OUTOFMEMORY) {
            return palMakeResult(
                PAL_RESULT_CODE_OUT_OF_MEMORY, 
                PAL_RESULT_SOURCE_WIN32, 
                hr);

        } else if (hr == E_ACCESSDENIED) {
            return palMakeResult(
                PAL_RESULT_CODE_INVALID_OPERATION, 
                PAL_RESULT_SOURCE_WIN32, 
                hr);

        } else {
            return palMakeResult(
                PAL_RESULT_CODE_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WIN32, 
                hr);
        }
    }
}

#endif // _WIN32