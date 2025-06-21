
#include "pal_pch.h"
#include "pal_win32platform.h"
#include "pal_internal.h"

void* palPlatformAlloc(Uint64 size)
{
    return malloc(size);
}

void palPlatformFree(void* memory)
{
    free(memory);
}

void palPlatformMemZero(void* memory, Uint64 size)
{
    memset(memory, 0, size);
}

void palPlatformMemSet(void* memory, int value, Uint64 size)
{
    memset(memory, value, size);
}

void palPlatformMemCpy(void* destMemory, void* srcMemory, Uint64 size)
{
    memcpy(destMemory, srcMemory, size);
}

void palWriteConsole(PAL_LogLevel level, const char* msg)
{
    wchar_t buffer[PAL_MSG_SIZE] = {};
    bool success = palToWstrUTF8Win32(buffer, msg);
    if (!success) { return; }

    bool error = level > PAL_LOG_WARN;
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
    OutputDebugStringW(buffer); // debug console on window VS
}

//************************internal API************************

bool palToWstrUTF8Win32(wchar_t* buffer, const char* string)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, string, -1, PAL_NULL, 0);
    if (len == 0) { return PAL_FALSE; }

    MultiByteToWideChar(CP_UTF8, 0, string, -1, buffer, len);
    return PAL_TRUE;
}