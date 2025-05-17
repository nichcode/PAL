
#pragma once

#include "PAL_pch.h"
#include "PAL_win32_platform.h"
#include "PAL/PAL.h"

i32 PAL_MultibyteToWchar(const char* str, u32 str_len, wchar_t* wstr)
{
    return MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, str_len);
}

i32 PAL_WcharToMultibyte(const wchar_t* wstr, u32 wstr_len, char* str)
{
    return WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, wstr_len, 0, 0);
}

void PAL_WriteConsole(u32 log_level, const char* msg)
{
    b8 error = log_level > PAL_LOG_LEVEL_WARN;
    HANDLE console = NULL;
    static u8 levels[4] = { 8, 2, 6, 4 };

    if (error) {
        console = GetStdHandle(STD_ERROR_HANDLE);
    }
    else {
        console = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    SetConsoleTextAttribute(console, levels[log_level]);
    wchar_t* wstring = PAL_ToWideString(msg);
    auto len = wcslen(wstring);
    DWORD number_written = 0;

    WriteConsoleW(console, wstring, (DWORD)len, &number_written, 0);
    SetConsoleTextAttribute(console, 15);
    delete[] wstring;
}