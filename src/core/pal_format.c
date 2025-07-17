
#include "pal_pch.h"
#include "pal/pal_core.h"

bool _PCALL palFormatV(
    const char* fmt, 
    va_list argsList, 
    Uint64* size, 
    char* buffer) {

    if (!size || !fmt) {
        return PAL_FALSE;
    }

    va_list listCopy;
#ifdef _MSC_VER
    listCopy = argsList;
#else
    __builtin_va_copy(listCopy, argsList);
#endif // _MSC_VER

    int len = vsnprintf(0, 0, fmt, listCopy);
    if (!buffer || *size < len + 1) {
        *size = len + 1;
        return PAL_FALSE;
    }

    vsnprintf(buffer, len + 1, fmt, listCopy);
    buffer[len] = 0;
    return PAL_TRUE;
}

bool _PCALL palFormat(
    char* buffer, 
    Uint64* size, 
    const char* fmt, ...) {

    if (!buffer || !fmt) {
        return PAL_FALSE;
    }

    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatV(fmt, argPtr, size, buffer);
    va_end(argPtr);
}