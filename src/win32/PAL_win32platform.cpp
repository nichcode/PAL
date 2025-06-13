
#include "PAL_pch.h"
#include "PAL_win32platform.h"

PAL_BOOL PAL_ToWideStringUTF8Win32(wchar_t* buffer, const char* string)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, string, -1, nullptr, 0);
    if (len == 0) { return PAL_FALSE; }

    MultiByteToWideChar(CP_UTF8, 0, string, -1, buffer, len);
    return PAL_TRUE;
}

void PAL_SetErrorWin32(const char* fmt, ...)
{
    s_PAL.errorCode = PAL_PLATFORM_ERROR;
    if (!s_PAL.debug) { return; }

    DWORD error = GetLastError();
    //if (error == 0) { return; } // no error

    wchar_t tmp[PAL_MESSAGE_SIZE] = {};
    char buffer[PAL_MESSAGE_SIZE] = {};
    char fmtBuffer[PAL_MESSAGE_SIZE] = {};

    DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM;
    flags |= FORMAT_MESSAGE_IGNORE_INSERTS;
    flags |= FORMAT_MESSAGE_MAX_WIDTH_MASK;

    FormatMessageW(
        flags,
        NULL,
        error & 0xffff,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        tmp,
        PAL_MESSAGE_SIZE,
        NULL
    );

    WideCharToMultiByte(CP_UTF8, 0, tmp, -1, buffer, PAL_MESSAGE_SIZE, NULL, NULL);
    
    // Get arguments
    va_list argPtr;
    va_start(argPtr, fmt);
    PAL_FormatArgs(fmt, argPtr, fmtBuffer);
    va_end(argPtr);

    PAL_Format(s_PAL.errorString, "%s: %s", PAL_GetErrorString(PAL_PLATFORM_ERROR), fmtBuffer);
    if (s_PAL.errorCallback) {
        s_PAL.errorCallback(PAL_PLATFORM_ERROR, s_PAL.errorString);
    }
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
    OutputDebugStringW(buffer);
}