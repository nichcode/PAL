
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

#define NAME_SUPPORTED 2
#define NAME_NOT_SUPPORTED 3

typedef HRESULT (WINAPI *SetThreadDescriptionFn)(HANDLE, PCWSTR);
typedef HRESULT (WINAPI *GetThreadDescriptionFn)(HANDLE, PWSTR*);

typedef struct {
    PalThreadFn func;
    void* arg;
} ThreadData;

typedef struct PalMutex {
    CRITICAL_SECTION sc;
} PalMutex;

typedef struct PalCondVar {
    CONDITION_VARIABLE cv;
} PalCondVar;

static Uint8 s_Init = false;
static SetThreadDescriptionFn s_SetThreadDescription;
static GetThreadDescriptionFn s_GetThreadDescription;
static const PalAllocator* s_Allocator = nullptr;

// ==================================================
// Internal API
// ==================================================

static DWORD WINAPI threadEntryToWin32(
    LPVOID arg) {

    ThreadData* data = arg;
    void* ret = data->func(data->arg);
    palFree(s_Allocator, data);
    return (uintptr_t)ret;
}

// ==================================================
// Public API
// ==================================================

void PAL_CALL palSetThreadAllocator(
    const PalAllocator* allocator) {

    if (allocator && (allocator->allocate || allocator->free)) {
        s_Allocator = allocator;
    }
}

const PalAllocator* PAL_CALL palGetThreadAllocator() {

    return s_Allocator;
}

// ==================================================
// Thread
// ==================================================

PalResult PAL_CALL palCreateThread(
    const PalThreadCreateInfo* info,
    PalThread** outThread) {

    if (!info || !outThread) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (s_Init == 0) {
        HINSTANCE kernel32 = GetModuleHandleW(L"kernel32.dll");
        if (kernel32) {
            s_GetThreadDescription = (GetThreadDescriptionFn)GetProcAddress(
                kernel32, 
                "GetThreadDescription"
            );

            s_SetThreadDescription = (SetThreadDescriptionFn)GetProcAddress(
                kernel32, 
                "SetThreadDescription"
            );

            if (!s_GetThreadDescription && !s_SetThreadDescription) {
                s_Init = NAME_NOT_SUPPORTED;
            } else {
                s_Init = NAME_SUPPORTED;
            }

        } else {
            s_Init = NAME_NOT_SUPPORTED;
        }
    }

    // create thread
    ThreadData* data = palAllocate(s_Allocator, sizeof(ThreadData), 0);
    if (!data) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    data->arg = info->arg;
    data->func = info->entry;

    HANDLE thread = CreateThread(
        nullptr,
        info->stackSize,
        threadEntryToWin32,
        data,
        0,
        nullptr
    );

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
    void* retval) {

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

void PAL_CALL palDetachThread(
    PalThread* thread) {

    if (thread) {
        CloseHandle((HANDLE)thread);
    }
}

void PAL_CALL palSleep(
    Uint64 milliseconds) {

    Sleep((DWORD)milliseconds);
}

void PAL_CALL palYield() {

    SwitchToThread();
}

PalThread* PAL_CALL palGetCurrentThread() {

    return (PalThread*)GetCurrentThread();
}

PalThreadFeatures PAL_CALL palGetThreadFeatures() {

    PalThreadFeatures features;
    features |= PAL_THREAD_FEATURE_STACK_SIZE;
    features |= PAL_THREAD_FEATURE_DETACHED;
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

PalThreadPriority PAL_CALL palGetThreadPriority(
    PalThread* thread) {

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

Uint64 PAL_CALL palGetThreadAffinity(
    PalThread* thread) {

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

char* PAL_CALL palGetThreadName(
    PalThread* thread) {

    if (!thread || !s_GetThreadDescription) {
        return nullptr;
    }

    wchar_t* buffer = nullptr;
    HRESULT hr = s_GetThreadDescription((HANDLE)thread, &buffer);

    if (!SUCCEEDED(hr)) {
        return nullptr;
    } 

    // convert to char string
    int len = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, nullptr, 0, 0, 0);
    char* stringBuffer = palAllocate(s_Allocator, len + 1, 0);
    if (!stringBuffer) {
        return nullptr;
    }

    WideCharToMultiByte(CP_UTF8, 0, buffer, -1, stringBuffer, len, 0, 0);
    LocalFree(buffer);
    return stringBuffer;
}

PalResult PAL_CALL palSetThreadPriority(
    PalThread* thread, 
    PalThreadPriority priority) {
    
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
    Uint64 mask) {

    if (!thread) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!SetThreadAffinityMask((HANDLE)thread, mask)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_THREAD;

        } else if (error == ERROR_INVALID_PARAMETER) {
            return PAL_RESULT_INVALID_PARAMETER;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetThreadName(
    PalThread* thread, 
    const char* name) {

    if (!thread || !name) {
        return PAL_RESULT_NULL_POINTER;
    }

    // check if the thread has been created
    if (s_Init == 0) {
        return PAL_RESULT_INVALID_THREAD;
    }

    if (!s_SetThreadDescription) {
        return PAL_RESULT_THREAD_FEATURE_NOT_SUPPORTED;
    }

    wchar_t buffer[512] = {};
    MultiByteToWideChar(CP_UTF8, 0, name, -1, buffer, 512);
    HRESULT hr = s_SetThreadDescription((HANDLE)thread, buffer);

    if (SUCCEEDED(hr)) {
        return PAL_RESULT_SUCCESS;

    } else {
        // error resolving
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

PalTLSId PAL_CALL palCreateTLS(
    PaTlsDestructorFn destructor) {

    DWORD tlsid = tlsid = FlsAlloc(destructor);
    if (tlsid == FLS_OUT_OF_INDEXES) {
        return 0;
    }
    return tlsid;
}

void PAL_CALL palDestroyTLS(
    PalTLSId id) {

    FlsFree((DWORD)id);
}

void* PAL_CALL palGetTLS(
    PalTLSId id) {

    return FlsGetValue((DWORD)id);
}

void PAL_CALL palSetTLS(
    PalTLSId id, 
    void* data) {

    FlsSetValue((DWORD)id, data);
}


// ==================================================
// Mutex
// ==================================================

PalResult PAL_CALL palCreateMutex(
    PalMutex** outMutex) {

    if (!outMutex) {
        return PAL_RESULT_NULL_POINTER;
    }

    PalMutex* mutex = palAllocate(s_Allocator, sizeof(PalMutex), 0);
    if (!mutex) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    InitializeCriticalSection(&mutex->sc);
    *outMutex = mutex;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyMutex(
    PalMutex* mutex) {

    if (mutex) {
        DeleteCriticalSection(&mutex->sc);
        palFree(s_Allocator, mutex);
    }
}

void PAL_CALL palLockMutex(
    PalMutex* mutex) {

    if (mutex) {
        EnterCriticalSection(&mutex->sc);
    }
}

void PAL_CALL palUnlockMutex(
    PalMutex* mutex) {

    if (mutex) {
        LeaveCriticalSection(&mutex->sc);
    }
}

PalResult PAL_CALL palCreateCondVar(
    PalCondVar** outCondition) {

    if (!outCondition) {
        return PAL_RESULT_NULL_POINTER;
    }

    PalCondVar* condition = palAllocate(s_Allocator, sizeof(PalCondVar), 0);
    if (!condition) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    InitializeConditionVariable(&condition->cv);
    *outCondition = condition;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyCondVar(
    PalCondVar* condition) {

    if (condition) {
        palFree(s_Allocator, condition);
    }
}

PalResult PAL_CALL palWaitCondVar(
    PalCondVar* condition,
    PalMutex* mutex) {
    
    if (!condition || !mutex) {
        return PAL_RESULT_NULL_POINTER;
    }

    BOOL ret = SleepConditionVariableCS(&condition->cv, &mutex->sc, INFINITE);
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
    PalCondVar* condition,
    PalMutex* mutex,
    Uint64 milliseconds) {
    
    if (!condition || !mutex) {
        return PAL_RESULT_NULL_POINTER;
    }

    BOOL ret = SleepConditionVariableCS(&condition->cv, &mutex->sc, milliseconds);
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

void PAL_CALL palSignalCondVar(
    PalCondVar* condition) {

    if (condition) {
        WakeConditionVariable(&condition->cv);
    }
}

void PAL_CALL palBroadcastCondVar(
    PalCondVar* condition) {

    if (condition) {
        WakeAllConditionVariable(&condition->cv);
    }
}

