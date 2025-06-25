
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

PalAllocator palGetDefaultAllocator()
{
    PalAllocator allocator;
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

bool _palPlatformVideoInit()
{
    HINSTANCE instance = GetModuleHandleW(PAL_NULL);
    WNDCLASSEXW wc = {};
    wc.cbClsExtra = 0;
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.cbWndExtra = 0;
    wc.hbrBackground = NULL;
    wc.hCursor = LoadCursorW(instance, IDC_ARROW);
    wc.hIcon = LoadIconW(instance, IDI_APPLICATION);
    wc.hIconSm = LoadIconW(instance, IDI_APPLICATION);
    wc.hInstance = instance;
    wc.lpfnWndProc = palWin32Proc;
    wc.lpszClassName = WIN32_CLASS;
    wc.lpszMenuName = NULL;
    wc.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

    ATOM success = RegisterClassExW(&wc);
    if (!success) {
        palSetError(PAL_PLATFORM_ERROR);
        return PAL_FALSE;
    }

    s_PALVideo.instance = instance;
    return PAL_TRUE;
}

void _palPlatformVideoShutdown()
{
    if (s_PALVideo.instance) {
        UnregisterClassW(WIN32_CLASS, (HINSTANCE)s_PALVideo.instance);
    }
}