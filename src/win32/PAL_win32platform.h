
#ifndef PAL_WIN32_PLATFORM_H
#define PAL_WIN32_PLATFORM_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

// mo min max
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#include <windows.h>

#include "PAL_platform.h"

void PAL_SetErrorWin32(const char* fmt, ...);
PAL_BOOL PAL_ToWideStringUTF8Win32(wchar_t* buffer, const char* string);

struct PAL_Win32Data
{
    HINSTANCE instance;
    const wchar_t* className = L"PALWindowClass";
    const wchar_t* propName = L"PALWindowData";
};

static PAL_Win32Data s_PALWin32;

struct PAL_Win32Monitor
{
    HMONITOR handle = nullptr;
    Uint32 w, h = 0;
};

struct PAL_Window
{
    HWND handle = nullptr;
    PAL_Win32Monitor monitor;
    int x, y = 0;
    Uint32 w, h = 0;
};

LRESULT CALLBACK PAL_Win32Proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif // PAL_WIN32_PLATFORM_H