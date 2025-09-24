
/**

Copyright (C) 2025 Nicholas Agbo

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

// ==================================================
// Includes
// ==================================================

#include "pal/pal_thread.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

// set unicode
#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include <windows.h>

// ==================================================
// Typedefs, enums and structs
// ==================================================

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

struct PalMutex {
    const PalAllocator* allocator;
    CRITICAL_SECTION sc;
};

struct PalCondVar {
    const PalAllocator* allocator;
    CONDITION_VARIABLE cv;
};

// ==================================================
// Internal API
// ==================================================

static DWORD WINAPI threadEntryToWin32(LPVOID arg)
{
    ThreadData* data = arg;
    void* ret = data->func(data->arg);
    palFree(data->allocator, data);
    return (DWORD)(uintptr_t)ret;
}

// ==================================================
// Public API
// ==================================================

// ==================================================
// Thread
// ==================================================

PalResult PAL_CALL palCreateThread(
    const PalThreadCreateInfo* info,
    PalThread** outThread)
{
    if (!info || !outThread) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (info->allocator) {
        if (!info->allocator->allocate && !info->allocator->free) {
            return PAL_RESULT_INVALID_ALLOCATOR;
        }
    }

    // create thread
    ThreadData* data = palAllocate(info->allocator, sizeof(ThreadData), 0);
    if (!data) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    data->arg = info->arg;
    data->func = info->entry;
    data->allocator = info->allocator;

    HANDLE thread = CreateThread(
        nullptr,
        info->stackSize,
        threadEntryToWin32,
        data,
        0,
        nullptr);

    if (!thread) {
        // error
        DWORD error = GetLastError();
        if (error == ERROR_NOT_ENOUGH_MEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;

        } else if (error == ERROR_INVALID_PARAMETER) {
            return PAL_RESULT_OUT_OF_MEMORY;

        } else if (error == ERROR_ACCESS_DENIED) {
            return PAL_RESULT_ACCESS_DENIED;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    *outThread = thread;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palJoinThread(
    PalThread* thread,
    void* retval)
{
    if (!thread) {
        return PAL_RESULT_NULL_POINTER;
    }

    DWORD wait = WaitForSingleObject((HANDLE)thread, INFINITE);
    if (wait == WAIT_OBJECT_0 && retval) {
        uintptr_t ret;
        GetExitCodeThread((HANDLE)thread, (LPDWORD)&ret);
        retval = (void*)ret;

    } else if (wait == WAIT_FAILED) {
        return PAL_RESULT_INVALID_THREAD;
    }

    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDetachThread(PalThread* thread)
{
    if (thread) {
        CloseHandle((HANDLE)thread);
    }
}

void PAL_CALL palSleep(Uint64 milliseconds)
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
        FARPROC setThreadDesc =
            GetProcAddress(kernel32, "SetThreadDescription");
        if (setThreadDesc) {
            features |= PAL_THREAD_FEATURE_NAME;
        }
    }
    return features;
}

PalThreadPriority PAL_CALL palGetThreadPriority(PalThread* thread)
{
    if (!thread) {
        return 0;
    }

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

Uint64 PAL_CALL palGetThreadAffinity(PalThread* thread)
{
    if (!thread) {
        return 0;
    }

    DWORD_PTR mask = SetThreadAffinityMask((HANDLE)thread, ~0ull);
    if (mask == 0) {
        return 0;
    }

    SetThreadAffinityMask((HANDLE)thread, mask);
    return mask;
}

void PAL_CALL palGetThreadName(
    PalThread* thread,
    Uint64 bufferSize,
    Uint64* outSize,
    char* outBuffer)
{
    if (!thread) {
        return;
    }

    HINSTANCE kernel32 = GetModuleHandleW(L"kernel32.dll");
    GetThreadDescriptionFn getThreadDescription = nullptr;
    if (kernel32) {
        getThreadDescription = (GetThreadDescriptionFn)GetProcAddress(
            kernel32,
            "GetThreadDescription");
    }

    if (!getThreadDescription) {
        // not supported
        return;
    }

    wchar_t* buffer = nullptr;
    HRESULT hr = getThreadDescription((HANDLE)thread, &buffer);

    if (!SUCCEEDED(hr)) {
        return;
    }

    int len = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, nullptr, 0, 0, 0);
    if (outSize) {
        *outSize = len - 1;
    }

    // see if user provided a buffer and write to it
    if (outBuffer && bufferSize > 0) {
        int write = bufferSize - 1;
        WideCharToMultiByte(CP_UTF8, 0, buffer, -1, outBuffer, write + 1, 0, 0);
        outBuffer[write < len - 1 ? write : len - 1] = '\0';
        LocalFree(buffer);
    }
}

PalResult PAL_CALL palSetThreadPriority(
    PalThread* thread,
    PalThreadPriority priority)
{
    if (!thread) {
        return PAL_RESULT_NULL_POINTER;
    }

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
            return PAL_RESULT_INVALID_THREAD;

        } else if (error == ERROR_ACCESS_DENIED) {
            return PAL_RESULT_ACCESS_DENIED;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetThreadAffinity(
    PalThread* thread,
    Uint64 mask)
{
    if (!thread) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!SetThreadAffinityMask((HANDLE)thread, mask)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_THREAD;

        } else if (error == ERROR_INVALID_PARAMETER) {
            return PAL_RESULT_INVALID_ARGUMENT;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetThreadName(
    PalThread* thread,
    const char* name)
{
    if (!thread || !name) {
        return PAL_RESULT_NULL_POINTER;
    }

    HINSTANCE kernel32 = GetModuleHandleW(L"kernel32.dll");
    SetThreadDescriptionFn setThreadDescription = nullptr;
    if (kernel32) {
        setThreadDescription = (SetThreadDescriptionFn)GetProcAddress(
            kernel32,
            "SetThreadDescription");
    }

    if (!setThreadDescription) {
        // not supported
        return PAL_RESULT_THREAD_FEATURE_NOT_SUPPORTED;
    }

    wchar_t buffer[128] = {0};
    MultiByteToWideChar(CP_UTF8, 0, name, -1, buffer, 128);
    HRESULT hr = setThreadDescription((HANDLE)thread, buffer);

    if (SUCCEEDED(hr)) {
        return PAL_RESULT_SUCCESS;

    } else {
        if (hr == E_INVALIDARG) {
            return PAL_RESULT_INVALID_THREAD;

        } else if (hr == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;

        } else if (hr == E_ACCESSDENIED) {
            return PAL_RESULT_ACCESS_DENIED;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }
}

// ==================================================
// TLS
// ==================================================

PalTLSId PAL_CALL palCreateTLS(PaTlsDestructorFn destructor)
{
    DWORD tlsid = FlsAlloc(destructor);
    if (tlsid == FLS_OUT_OF_INDEXES) {
        return 0;
    }
    return tlsid;
}

void PAL_CALL palDestroyTLS(PalTLSId id)
{
    FlsFree((DWORD)id);
}

void* PAL_CALL palGetTLS(PalTLSId id)
{
    return FlsGetValue((DWORD)id);
}

void PAL_CALL palSetTLS(
    PalTLSId id,
    void* data)
{
    FlsSetValue((DWORD)id, data);
}

// ==================================================
// Mutex
// ==================================================

PalResult PAL_CALL palCreateMutex(
    const PalAllocator* allocator,
    PalMutex** outMutex)
{
    if (!outMutex) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (allocator) {
        if (!allocator->allocate && !allocator->free) {
            return PAL_RESULT_INVALID_ALLOCATOR;
        }
    }

    PalMutex* mutex = palAllocate(allocator, sizeof(PalMutex), 0);
    if (!mutex) {
        return PAL_RESULT_OUT_OF_MEMORY;
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

// ==================================================
// Condition Variable
// ==================================================

PalResult PAL_CALL palCreateCondVar(
    const PalAllocator* allocator,
    PalCondVar** outCondVar)
{
    if (!outCondVar) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (allocator) {
        if (!allocator->allocate && !allocator->free) {
            return PAL_RESULT_INVALID_ALLOCATOR;
        }
    }

    PalCondVar* condVar = palAllocate(allocator, sizeof(PalCondVar), 0);
    if (!condVar) {
        return PAL_RESULT_OUT_OF_MEMORY;
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
        return PAL_RESULT_NULL_POINTER;
    }

    BOOL ret = SleepConditionVariableCS(&condVar->cv, &mutex->sc, INFINITE);
    if (!ret) {
        DWORD error = GetLastError();
        if (error == ERROR_TIMEOUT) {
            return PAL_RESULT_TIMEOUT;
        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palWaitCondVarTimeout(
    PalCondVar* condVar,
    PalMutex* mutex,
    Uint64 milliseconds)
{
    if (!condVar || !mutex) {
        return PAL_RESULT_NULL_POINTER;
    }

    BOOL ret = SleepConditionVariableCS(
        &condVar->cv,
        &mutex->sc,
        (DWORD)milliseconds);

    if (!ret) {
        DWORD error = GetLastError();
        if (error == ERROR_TIMEOUT) {
            return PAL_RESULT_TIMEOUT;
        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
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