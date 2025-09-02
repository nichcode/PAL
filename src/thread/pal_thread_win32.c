
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
    const PalAllocator* allocator;
} ThreadWin32;

// ==================================================
// Internal API
// ==================================================

static DWORD WINAPI threadEntryToWin32(LPVOID arg) {

    ThreadWin32* data = arg;
    void* ret = data->func(data->arg);

    const PalAllocator* allocator = data->allocator;
    palFree(allocator, data);
    return (uintptr_t)ret;
}

static Uint8 s_Init = false;
static SetThreadDescriptionFn s_SetThreadDescription;
static GetThreadDescriptionFn s_GetThreadDescription;

// ==================================================
// Public API
// ==================================================

// ==================================================
// Thread
// ==================================================

PalResult _PCALL palCreateThread(
    const PalThreadCreateInfo* info,
    PalThread** outThread) {

    if (!info || !outThread) {
        return PAL_RESULT_NULL_POINTER;
    }

    // check invalid allocator
    if (info->allocator && (!info->allocator->allocate || !info->allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
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
    ThreadWin32* data = palAllocate(info->allocator, sizeof(ThreadWin32), 0);
    if (!data) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    data->allocator = info->allocator;
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

    DWORD err = GetLastError();

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

void _PCALL palJoinThread(PalThread* thread, void* retval) {

    if (thread) {
        WaitForSingleObject(thread, INFINITE);

        if (retval) {
            uintptr_t ret;
            GetExitCodeThread(thread, (LPDWORD)&ret);
            retval = (void*)ret;
        }
    }
}

void _PCALL palDetachThread(PalThread* thread) {

    if (thread) {
        CloseHandle(thread);
    }
}

void _PCALL palSleep(Uint64 milliseconds) {

    Sleep((DWORD)milliseconds);
}

PalThread* _PCALL palGetCurrentThread() {

    return GetCurrentThread();
}

PalThreadFeatures _PCALL palGetThreadFeatures() {

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

// ==================================================
// TLS
// ==================================================

PalTlsId _PCALL palCreateTls(PaTlsDestructorFn destructor) {

    DWORD tlsid = tlsid = FlsAlloc(destructor);
    if (tlsid == FLS_OUT_OF_INDEXES) {
        return 0;
    }
    return tlsid;
}

void _PCALL palDestroyTls(PalTlsId id) {

    FlsFree((DWORD)id);
}

void* _PCALL palGetTls(PalTlsId id) {

    return FlsGetValue((DWORD)id);
}

void _PCALL palSetTls(PalTlsId id, void* data) {

    FlsSetValue((DWORD)id, data);
}
