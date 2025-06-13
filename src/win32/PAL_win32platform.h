
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

PAL_BOOL PAL_ToWideStringUTF8Win32(wchar_t* buffer, const char* string);

#endif // PAL_WIN32_PLATFORM_H