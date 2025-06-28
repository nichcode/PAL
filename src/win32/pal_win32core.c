
#include "pal_pch.h"
#include "pal_win32.h"
#include "pal_shared.h"

void palToWstrUTF8Win32(wchar_t* buffer, const char* string)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, string, -1, PAL_NULL, 0);
    MultiByteToWideChar(CP_UTF8, 0, string, -1, buffer, len);
}

bool palIsVersionWin32(int major, int minor, int servicePack)
{
    OSVERSIONINFOEXW osvi = { 0 };
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
    osvi.dwMajorVersion = major;
    osvi.dwMinorVersion = minor;
    osvi.wServicePackMajor = servicePack;

    DWORDLONG mask = 0;
    mask = VerSetConditionMask(mask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    mask = VerSetConditionMask(mask, VER_MINORVERSION, VER_GREATER_EQUAL);
    mask = VerSetConditionMask(mask, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
}

Uint32 palGetBuildWin32()
{
    RtlGetVersionPtr RtlGetVersion = PAL_NULL;
    RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(s_Win32.ntdll, "RtlGetVersion");
    if (!RtlGetVersion) {
        return 0;
    }

    RTL_OSVERSIONINFOW ver = { 0 };
    ver.dwOSVersionInfoSize = sizeof(ver);
    if (RtlGetVersion(&ver) == 0) {
        return ver.dwBuildNumber;
    }

    return 0;
}

const char* _PCALL palGetPlatformString()
{
    if (PAL_WINDOWS_11) {
        return "Windows 11";

    } else if (PAL_WINDOWS_10) {
        return "Windows 10";

    } else if (PAL_WINDOWS_8_1) {
        return "Windows 8.1";

    } else if (PAL_WINDOWS_8) {
        return "Windows 8";

    } else if (PAL_WINDOWS_7) {
        return "Windows 7";

    } else if (PAL_WINDOWS_VISTA) {
        return "Windows Vista";

    } else if (PAL_WINDOWS_XP) {
        return "Windows XP";
        
    } else {
        return PAL_NULL;
    }
}

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

bool _PCALL palSetTLS(PalTLSID id, void* data)
{
    return TlsSetValue((DWORD)id, data);
}

void* _PCALL palAllocate(Uint64 size)
{
    return malloc(size);
}

void* _PCALL palAlignAllocate(Uint64 size, Uint64 alignment)
{
    return _aligned_malloc(size, alignment);
}

void _PCALL palFree(void* memory)
{
    free(memory);
}

void _PCALL palAlignFree(void* memory)
{
    _aligned_free(memory);
}

void _PCALL palCopyString(char* dest, const char* src)
{
    strcpy(dest, src);
}

void _PCALL palSetMemory(void* memory, int value, Uint64 size)
{
    if (memory) {
        memset(memory, value, size);
    }
}

void _PCALL palCopyMemory(void* dest, void* src, Uint64 size)
{
    if (dest && src) {
        memcpy(dest, src, size);
    }
}

void palWriteConsole(Uint32 level, const char* msg)
{
    wchar_t buffer[PAL_MSG_SIZE] = {};
    HANDLE console = PAL_NULL;
    bool error = level > PAL_LOG_LEVEL_WARN;
    static int levels[4] = { 8, 2, 6, 4 };

    if (error) {
        console = GetStdHandle(STD_ERROR_HANDLE);
    } else {
        console = GetStdHandle(STD_OUTPUT_HANDLE);
    }


    palToWstrUTF8Win32(buffer, msg);
    SetConsoleTextAttribute(console, levels[level]);
    Uint64 len = wcslen(buffer);
    DWORD written = 0;
    WriteConsoleW(console, buffer, (DWORD)len, &written, 0);
    OutputDebugStringW(buffer);
}