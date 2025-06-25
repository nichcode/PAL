
#ifndef _PAL_WIN32_PLATFORM_H
 #define _PAL_WIN32_PLATFORM_H

#ifndef WIN32_LEAN_AND_MEAN
 #define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
 #define NOMINMAX
#endif // NOMINMAX

#include <windows.h>

#include "pal_internal.h"

#define WIN32_CLASS L"PALClass"
#define WIN32_PROP L"PAL"

void palToWstrUTF8Win32(wchar_t* buffer, const char* string);

LRESULT CALLBACK palWin32Proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif // _PAL_WIN32_PLATFORM_H