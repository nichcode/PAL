
#include "pal_pch.h"
#include "pal/pal_core.h"

void _PCALL palFormatArgs(
    const char* fmt, 
    va_list argsList, 
    char* buffer)
{
    if (!buffer || !fmt) {
        return;
    }

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

void _PCALL palFormat(
    char* buffer, 
    const char* fmt, ...)
{
    if (!buffer || !fmt) {
        return;
    }
    
    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatArgs(fmt, argPtr, buffer);
    va_end(argPtr);
}
