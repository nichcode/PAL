
#include "pal_pch.h"
#include "pal_internal.h"

const char* palGetResultString(PAL_Result code)
{
    const char* name = NULL;
    switch (code) {
    case PAL_PLATFORM_ERROR: {
        name = "PLATFORM_ERROR";
        break;
    }

    case PAL_PLATFORM_WARN: {
        name = "PLATFORM_WARN";
        break;
    }

    case PAL_DEVICE_ERROR: {
        name = "DEVICE_ERROR";
        break;
    }

    case PAL_DEVICE_WARN: {
        name = "DEVICE_WARN";
        break;
    }

    case PAL_INVALID_ERROR: {
        name = "INVALID_ERROR";
        break;
    }

    case PAL_OUT_OF_MEMORY: {
        name = "OUT_OF_MEMORY";
        break;
    }

    }
    return name;
}

void palFormatArgs(const char* fmt, va_list argsList, char* buffer)
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

void palFormat(char* buffer, const char* fmt, ...)
{
    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatArgs(fmt, argPtr, buffer);
    va_end(argPtr);
}

void palSetError(PAL_Result code, const char* msg, ...)
{
    char tmp[PAL_MSG_SIZE] = {};
    char buffer[PAL_MSG_SIZE] = {};
    va_list argPtr;
    va_start(argPtr, msg);
    palFormatArgs(msg, argPtr, tmp);
    va_end(argPtr);

    palFormat(
        buffer, 
        "[%s]: %s",
        palGetResultString(code),
        tmp
    );
    
    if (s_PAL.debugCallback) {
        s_PAL.debugCallback(code, buffer);
    }
}