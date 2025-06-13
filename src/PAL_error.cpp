
#include "PAL_pch.h"
#include "PAL_platform.h"

const char* getErrorString(Uint32 code)
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

void PAL_SetErrorCallback(PAL_ErrorFun callback)
{
    s_PAL.errorCallback = callback;
}

PAL_BOOL PAL_SetError(Uint32 code, const char* fmt, ...)
{
    s_PAL.errorCode = code;
    if (!s_PAL.debug) { return PAL_FALSE; }

    PAL_Format(s_PAL.errorString, "%s: %s", getErrorString(code), fmt);
    if (s_PAL.errorCallback) {
        s_PAL.errorCallback(code, s_PAL.errorString);
    }

    return PAL_TRUE;
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