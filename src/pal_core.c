
#include "pal_pch.h"
#include "pal_platform.h"
#include "pal_internal.h"

static PalTLSID s_ErrorTLSID = 0;
static const char* levels[4] = { "[TRACE]: ", "[INFO]:  ", "[WARN]:  ", "[ERROR]: " };

struct LogData
{
    PalError code;
    PalLogCallback callback;
};

void _PCALL palSetError(PalError error)
{
    struct LogData* LogData = PAL_NULL;
    LogData = palGetTLS(s_ErrorTLSID);
    if (!LogData) {
        // create TLS
        PAlAllocator allocator = palPlatformGetAllocator();
        LogData = allocator.alloc(sizeof(struct LogData));
        LogData->code = error;
        LogData->callback = PAL_NULL;
        palSetTLS(s_ErrorTLSID, LogData, allocator.free);
    }
}

PalError _PCALL palGetError()
{
    struct LogData* LogData = palGetTLS(s_ErrorTLSID);
    if (!LogData) {
        return PAL_ERROR_NONE;
    }
    return LogData->code;
}

void _PCALL palSetLogCallback(PalLogCallback callback)
{
    struct LogData* LogData = PAL_NULL;
    LogData = palGetTLS(s_ErrorTLSID);
    if (!LogData) {
        // create TLS
        PAlAllocator allocator = palPlatformGetAllocator();
        LogData = allocator.alloc(sizeof(struct LogData));
        LogData->callback = callback;
        LogData->code = 0;
        palSetTLS(s_ErrorTLSID, LogData, allocator.free);
    }
}

void _PCALL palLog(PalLogLevel level, const char* fmt, ...)
{
    char tmp[MAX_LOG_SIZE] = {};
    bool result = PAL_FALSE;

    va_list argPtr;
    va_start(argPtr, fmt);
    result = palFormatArgs(fmt, argPtr, tmp);
    va_end(argPtr);

    if (!result) {
        return;
    }

    char buffer[MAX_LOG_SIZE] = {};
    palFormat(buffer, "%s%s\n", levels[level], tmp);
    palWriteConsole(level, buffer);
}

// this can reroute messages
void _PCALL palLogInternal(PalLogLevel level, const char* fmt, ...)
{
    char tmp[MAX_LOG_SIZE] = {};
    bool result = PAL_FALSE;

    va_list argPtr;
    va_start(argPtr, fmt);
    result = palFormatArgs(fmt, argPtr, tmp);
    va_end(argPtr);

    struct LogData* LogData = palGetTLS(s_ErrorTLSID);
    if (!LogData || (LogData && !LogData->callback)) {
        char buffer[MAX_LOG_SIZE] = {};
        palFormat(buffer, "%s%s\n", levels[level], tmp);
        palWriteConsole(level, buffer);

    } else {
        LogData->callback(LogData->code, level, tmp);
    }
}

bool _PCALL palFormatArgs(const char* fmt, va_list argsList, char* buffer)
{
    if (!buffer) {
        return PAL_FALSE;
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
    if (!len) {
        return PAL_FALSE;
    }

    vsnprintf(buffer, len + 1, fmt, listCopy);
    buffer[len] = 0;
    return PAL_TRUE;
}

bool _PCALL palFormat(char* buffer, const char* fmt, ...)
{
    va_list argPtr;
    va_start(argPtr, fmt);
    bool success = palFormatArgs(fmt, argPtr, buffer);
    va_end(argPtr);

    return success;
}