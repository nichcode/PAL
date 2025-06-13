
#include "PAL_pch.h"
#include "PAL_platform.h"

void PAL_SetErrorCallback(PAL_ErrorFun callback)
{
    s_PAL.errorCallback = callback;
}

void PAL_SetError(Uint32 code, const char* fmt, ...)
{
    s_PAL.errorCode = code;
    if (!s_PAL.debug) { return; }

    char tmp[PAL_MESSAGE_SIZE] = {};
    va_list argPtr;
    va_start(argPtr, fmt);
    PAL_FormatArgs(fmt, argPtr, tmp);
    va_end(argPtr);

    PAL_Format(s_PAL.errorString, "%s: %s", PAL_GetErrorString(code), tmp);
    if (s_PAL.errorCallback) {
        s_PAL.errorCallback(code, s_PAL.errorString);
    }
}

PAL_ErrorFun PAL_GetErrorCallback()
{
    return s_PAL.errorCallback;
}

Uint32 PAL_GetError()
{
    return s_PAL.errorCode;
}

const char* PAL_GetErrorMessage()
{
    return s_PAL.errorString;
}