
#include "pal_pch.h"
#include "pal_shared.h"

static PalTLSID s_ErrorTLSID = 0;
static const char* levels[4] = { "[TRACE]: ", "[INFO]:  ", "[WARN]:  ", "[ERROR]: " };

typedef struct ErrorTLSData
{
    PalError error;

} ErrorTLSData;

void _PCALL palSetError(PalError error)
{
    ErrorTLSData* errorData = palGetTLS(s_ErrorTLSID);
    if (!errorData) {
        errorData = palAllocate(sizeof(ErrorTLSData));
        errorData->error = error;
    }
    palSetTLS(s_ErrorTLSID, errorData);
}

PalError _PCALL palGetError()
{
    ErrorTLSData* errorData = palGetTLS(s_ErrorTLSID);
    if (!errorData) {
        return PAL_ERROR_NONE;
    }
    return errorData->error;
}

PalVersion _PCALL palGetVersion()
{
    PalVersion version;
    version.major = PAL_VERSION_MAJOR;
    version.minor = PAL_VERSION_MINOR;
    version.patch = PAL_VERSION_PATCH;
    return version;
}

const char* _PCALL palFormatError(PalError error)
{
    switch (error) {
        case PAL_ERROR_NONE: 
        return "The operation was successful";

        case PAL_NULL_POINTER: 
        return "The pointer is invalid.";

        case PAL_INVALID_ALLOCATOR: 
        return "Allocator is not fully defined";

        case PAL_VIDEO_NOT_INITIALIZED:
        return "Video system is not intialized";

        case PAL_EVENT_NOT_INITIALIZED:
        return "Events system is not intialized";

        case PAL_OUT_OF_MEMORY: 
        return "Windows has run out of memory";

        case PAL_DISPLAY_ERROR: 
        return "The display index is invalid. Use 0 for primary display";

        case PAL_PLATFORM_ERROR: 
        return "An operation failed on Windows";

        case PAL_INVALID_ARG:
        return "The argument provided is invalid. eg an unsigned int set to -1";

        case PAL_INVALID_EVENT:
        return "The event or event type is invalid";
    }

    return PAL_NULL;
}

void _PCALL palLog(PalLogLevel level, const char* fmt, ...)
{
    char tmp[PAL_MSG_SIZE] = {};
    char buffer[PAL_MSG_SIZE] = {};
    
    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatArgs(fmt, argPtr, tmp);
    va_end(argPtr);
    palFormat(buffer, "%s%s\n", levels[level], tmp);
    palWriteConsole(level, buffer);
}