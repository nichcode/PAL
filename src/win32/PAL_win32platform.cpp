
#include "PAL_pch.h"
#include "PAL_win32platform.h"

PAL_BOOL PAL_ToWideStringUTF8Win32(wchar_t* buffer, const char* string)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, string, -1, nullptr, 0);
    if (len == 0) { return PAL_FALSE; }

    MultiByteToWideChar(CP_UTF8, 0, string, -1, buffer, len);
    return PAL_TRUE;
}

PAL_Allocator PAL_PlatformGetAllocator()
{
    PAL_Allocator allocator;
    allocator.alloc = ::operator new;
    allocator.free = ::operator delete;

    return allocator;
}

void PAL_WriteConsole(Uint32 level, const char* msg)
{
    wchar_t buffer[PAL_MESSAGE_SIZE] = {};
    PAL_BOOL success = PAL_ToWideStringUTF8Win32(buffer, msg);
    if (!success) { return; }

    PAL_BOOL error = level > PAL_LOG_WARN;
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
}