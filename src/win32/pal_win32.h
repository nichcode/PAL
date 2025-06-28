
#ifndef _PAL_WIN32_H
 #define _PAL_WIN32_H

#ifndef WIN32_LEAN_AND_MEAN
 #define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
 #define NOMINMAX
#endif // NOMINMAX

#include <windows.h>
#include <windowsx.h>
#include <winnt.h>

#include "pal/pal_core.h"

#define WIN32_CLASS L"PALClass"
#define WIN32_PROP L"PAL"
#define WIN32_DPI 0
#define WIN32_DPI_AWARE 2

// osversioninfo
typedef LONG (WINAPI *RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

// monitor, dpiType, &dpiX, &dpiY
typedef HRESULT (WINAPI* GetDpiForMonitorFn)(HMONITOR, int, UINT*, UINT*);

// PROCESS_DPI_AWARENESS
typedef HRESULT (WINAPI* SetProcessAwarenessFn)(int);

// libraries
typedef struct PalWin32
{
    HINSTANCE hInstance;
    HINSTANCE ntdll;
    HINSTANCE shcore;

    GetDpiForMonitorFn getDpiForMonitor;
    SetProcessAwarenessFn setProcessAwareness;

} PalWin32;

static PalWin32 s_Win32;

void palToWstrUTF8Win32(wchar_t* buffer, const char* string);
bool palIsVersionWin32(int major, int minor, int servicePack);
Uint32 palGetBuildWin32();

LRESULT CALLBACK palProcWin32(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK palMonitorProcWin32(HMONITOR monitor, HDC, LPRECT, LPARAM);

#define PAL_WINDOWS_XP                 (palIsVersionWin32(5, 1, 0))
#define PAL_WINDOWS_VISTA              (palIsVersionWin32(6, 0, 0))
#define PAL_WINDOWS_7                  (palIsVersionWin32(6, 1, 0))
#define PAL_WINDOWS_8                  (palIsVersionWin32(6, 2, 0))
#define PAL_WINDOWS_8_1                (palIsVersionWin32(6, 3, 0))
#define PAL_WINDOWS_10                 (palIsVersionWin32(10, 0, 0))
#define PAL_WINDOWS_11                 (PAL_WINDOWS_10 && palGetBuildWin32() >= 22000)

#endif // _PAL_WIN32_H