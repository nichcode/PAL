
#include "pal_win32platform.h"
#include "pal/pal_thread.h"
#include "pal/pal_log.h"

//************************************************************
//********************Win32 platform API**********************
//************************************************************

void palToWstrUTF8Win32(wchar_t* buffer, const char* string)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, string, -1, PAL_NULL, 0);
    MultiByteToWideChar(CP_UTF8, 0, string, -1, buffer, len);
}

// memory

PAlAllocator palGetDefaultAllocator()
{
    PAlAllocator allocator;
    allocator.alloc = malloc;
    allocator.free = free;
    allocator.alignedAlloc = _aligned_malloc;
    allocator.alignedFree = _aligned_free;
    return allocator;
}

void _PCALL palSetMemory(void* memory, int value, Uint64 size)
{
    if (memory) {
        memset(memory, value, size);
    }
}

// thread

PalTLSID _PCALL palCreateTLS()
{
    DWORD slot = TlsAlloc();
    if (slot == TLS_OUT_OF_INDEXES) {
        return 0;
    }

    return slot;
}

bool _PCALL palDestroyTLS(PalTLSID id)
{
    return TlsFree((DWORD)id);
}

void* _PCALL palGetTLS(PalTLSID id)
{
    return TlsGetValue((DWORD)id);
}

bool _PCALL palSetTLS(PalTLSID id, void* data, void (*destructor)(void*))
{
    return TlsSetValue((DWORD)id, data);
}

// platform

void _palPlatformWriteConsole(Uint32 level, const char* msg)
{
    wchar_t buffer[_PAL_MSG_SIZE] = {};
    palToWstrUTF8Win32(buffer, msg);

    bool error = level > PAL_LOG_LEVEL_WARN;
    HANDLE console = PAL_NULL;
    static int levels[4] = { 8, 2, 6, 4 };

    if (error) {
        console = GetStdHandle(STD_ERROR_HANDLE);
    } else {
        console = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    SetConsoleTextAttribute(console, levels[level]);
    Uint64 len = wcslen(buffer);
    DWORD written = 0;
    WriteConsoleW(console, buffer, (DWORD)len, &written, 0);
    OutputDebugStringW(buffer);
}