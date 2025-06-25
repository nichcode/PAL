
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

void palToWstrUTF8Win32(wchar_t* buffer, const char* string);

#endif // _PAL_WIN32_PLATFORM_H