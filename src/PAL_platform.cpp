
#include "PAL_pch.h"
#include "PAL_platform.h"

void PAL_FormatArgs(const char* fmt, va_list argsList, char* buffer)
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

void PAL_Format(char* buffer, const char* fmt, ...)
{
    va_list argPtr;
    va_start(argPtr, fmt);
    PAL_FormatArgs(fmt, argPtr, buffer);
    va_end(argPtr);
}

const char* PAL_GetErrorString(Uint32 code)
{
    const char* name = NULL;
    switch (code) {
    case PAL_NO_ERROR: {
        name = "PAL_NO_ERROR";
        break;
    } // PAL_NO_ERROR

    case PAL_ALLOCATION_ERROR: {
        name = "PAL_ALLOCATION_ERROR";
        break;
    } // PAL_ALLOCATION_ERROR

    case PAL_PLATFORM_ERROR: {
        name = "PAL_PLATFORM_ERROR";
        break;
    } // PAL_PLATFORM_ERROR

    case PAL_INVALID_ERROR: {
        name = "PAL_INVALID_ERROR";
        break;
    } // PAL_INVALID_ERROR

    } // end of switch
    return name;
}