
#include "pal_pch.h"
#include "pal_internal.h"

void _PCALL _palFormatArgs(const char* fmt, va_list argsList, char* buffer)
{
    va_list listCopy;
#ifdef _MSC_VER
        listCopy = argsList;
#elif defined(P_PLATFORM_APPLE)
        listCopy = argsList;
#else
        __builtin_va_copy(listCopy, argsList);
#endif // _MSC_VER

    int len = vsnprintf(0, 0, fmt, listCopy);
    vsnprintf(buffer, len + 1, fmt, listCopy);
    buffer[len] = 0;
}

void _PCALL _palFormat(char* buffer, const char* fmt, ...)
{
    va_list argPtr;
    va_start(argPtr, fmt);
    _palFormatArgs(fmt, argPtr, buffer);
    va_end(argPtr);
}