
#ifndef _PAL_WIN32_PLATFORM_H
 #define _PAL_WIN32_PLATFORM_H

#ifndef WIN32_LEAN_AND_MEAN
 #define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
 #define NOMINMAX
#endif // NOMINMAX

#include <windows.h>
#include <winnt.h>

#include "pal_internal.h"

#define WIN32_CLASS L"PALClass"
#define WIN32_PROP L"PAL"

typedef LONG (WINAPI *RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

void palToWstrUTF8Win32(wchar_t* buffer, const char* string);
bool _palIsVersionWin32(int major, int minor, int servicePack);
Uint32 _palGetBuildWin32();

LRESULT CALLBACK palProcWin32(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK palMonitorProcWin32(HMONITOR monitor, HDC, LPRECT, LPARAM);

#define PAL_WINDOWS_XP                 (_palIsVersionWin32(5, 1, 0))
#define PAL_WINDOWS_VISTA              (_palIsVersionWin32(6, 0, 0))
#define PAL_WINDOWS_7                  (_palIsVersionWin32(6, 1, 0))
#define PAL_WINDOWS_8                  (_palIsVersionWin32(6, 2, 0))
#define PAL_WINDOWS_8_1                (_palIsVersionWin32(6, 3, 0))
#define PAL_WINDOWS_10                 (_palIsVersionWin32(10, 0, 0))
#define PAL_WINDOWS_11                 (PAL_WINDOWS_10 && _palGetBuildWin32() >= 22000)

#endif // _PAL_WIN32_PLATFORM_H