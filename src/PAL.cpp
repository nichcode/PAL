
#include "PAL_pch.h"
#include "PAL_internal.h"

#ifdef PAL_PLATFORM_WINDOWS
#include "win32/PAL_win32_platform.h"
#endif // PAL_PLATFORM_WINDOWS

void PAL_Init()
{
#ifdef PAL_PLATFORM_WINDOWS
    PAL_Win32Init();
#endif // PAL_PLATFORM_WINDOWS

    PAL_LOG_INFO("PAL Platform Initialized");
}

void PAL_Terminate()
{
#ifdef PAL_PLATFORM_WINDOWS
    PAL_Win32Terminate();
#endif // PAL_PLATFORM_WINDOWS

    PAL_LOG_INFO("PAL Platform Terminated");
}

char* PAL_ToString(const wchar_t* wide_string)
{
    if (!wide_string) {
        PAL_ERROR(PAL_INVALID_POINTER);
        return nullptr;
    }

    int len = PAL_WcharToMultibyte(wide_string, 0, nullptr);
    if (!len) {
        PAL_ERROR(PAL_INVALID_POINTER);
        return nullptr;
    }

    char* string = new char[len + 1];
    if (!string) {
        PAL_ERROR(PAL_OUT_OF_MEMORY);
        return nullptr;
    }
    
    PAL_WcharToMultibyte(wide_string, len, string);
    return string;
}

wchar_t* PAL_ToWideString(const char* string)
{
    if (!string) {
        PAL_ERROR(PAL_INVALID_POINTER);
        return nullptr;
    }

    int len = PAL_MultibyteToWchar(string, 0, nullptr);
    if (!len) {
        PAL_ERROR(PAL_INVALID_POINTER);
        return nullptr;
    }

    wchar_t* wstring = new wchar_t[sizeof(wchar_t) + len];
    if (!string) {
        PAL_ERROR(PAL_OUT_OF_MEMORY);
        return nullptr;
    }

    PAL_MultibyteToWchar(string, len, wstring);
    return wstring;
}

char* PAL_FormatArgs(const char* fmt, va_list args_list)
{
    if (!fmt) {
        PAL_ERROR(PAL_INVALID_POINTER);
        return nullptr;
    }

    va_list list_copy;
    __builtin_va_copy(list_copy, args_list);
    
    i32 len = vsnprintf(0, 0, fmt, list_copy);
    va_end(list_copy);

    char* string = new char[len + 1];
    if (!string) {
        PAL_ERROR(PAL_OUT_OF_MEMORY);
        return nullptr;
    }

    vsnprintf(string, len + 1, fmt, args_list);
    string[len] = 0;
    return string;
}

char* PAL_Format(const char* fmt, ...)
{
    if (!fmt) {
        PAL_ERROR(PAL_INVALID_POINTER);
        return nullptr;
    }

    va_list arg_ptr;
    va_start(arg_ptr, fmt);
    char* result = PAL_FormatArgs(fmt, arg_ptr);
    va_end(arg_ptr);
    return result;
}