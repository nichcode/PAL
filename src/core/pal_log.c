
#include "pal_pch.h"
#include "pal/pal_core.h"

static const char* s_levelStrings[4] = { "[TRACE]: ", "[INFO]: ", "[WARN]: ", "[ERROR]: " };

static PalTLSID s_LogTLSID = 0;
typedef struct LogTLSData
{
    char buffer[1024];
    bool isLogging;

} LogTLSData;

void consoleWrite(PalLogLevel level, const char* msg)
{
#ifdef _WIN32
    HANDLE console = PAL_NULL;
    bool error = level > PAL_LOG_LEVEL_WARN;
    static int levels[4] = { 8, 2, 6, 4 };
    if (error) {
        console = GetStdHandle(STD_ERROR_HANDLE);
    } else {
        console = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    SetConsoleTextAttribute(console, levels[level]);
    Uint64 len = strlen(msg);
    DWORD written = 0;
    WriteConsoleA(console, msg, (DWORD)len, &written, 0);
    OutputDebugStringA(msg);
#endif // _WIN32    
}

void palLogConsoleV(PalLogLevel level, const char* fmt)
{
    LogTLSData* data = palGetTLS(s_LogTLSID);
    if (!data) {
        data = palAlloc(PAL_NULL, sizeof(LogTLSData));
        memset(data, 0, sizeof(LogTLSData));
    }
    palFormat(data->buffer, "%s%s\n", s_levelStrings[level], fmt);
    consoleWrite(level, data->buffer);
}

void palLogV(PalLogger* logger, PalLogLevel level, const char* fmt)
{
    LogTLSData* data = palGetTLS(s_LogTLSID);
    if (!data) {
        data = palAlloc(PAL_NULL, sizeof(LogTLSData));
        memset(data, 0, sizeof(LogTLSData));
    }

    // prevent recursive calls
    if (data->isLogging) {
        return; 
    }

    if (logger && logger->callback) {
        data->isLogging = PAL_TRUE;
        palSetTLS(s_LogTLSID, data);

        logger->callback(logger->userData, level, fmt);
        data->isLogging = PAL_FALSE;
        palSetTLS(s_LogTLSID, data);
    } else {
        palLogConsoleV(level, fmt);
    }
}

void _PCALL palLog(PalLogger* logger, PalLogLevel level, const char* fmt, ...)
{
    char buffer[1024] = {};
    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatArgs(fmt, argPtr, buffer);
    va_end(argPtr);
    palLogV(logger, level, buffer);
}

void _PCALL palLogTrace(PalLogger* logger, const char* fmt, ...)
{
    char buffer[1024] = {};
    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatArgs(fmt, argPtr, buffer);
    va_end(argPtr);
    palLogV(logger, PAL_LOG_LEVEL_TRACE, buffer);
}

void _PCALL palLogInfo(PalLogger* logger, const char* fmt, ...)
{
    char buffer[1024] = {};
    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatArgs(fmt, argPtr, buffer);
    va_end(argPtr);
    palLogV(logger, PAL_LOG_LEVEL_INFO, buffer);
}

void _PCALL palLogWarn(PalLogger* logger, const char* fmt, ...)
{
    char buffer[1024] = {};
    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatArgs(fmt, argPtr, buffer);
    va_end(argPtr);
    palLogV(logger, PAL_LOG_LEVEL_WARN, buffer);
}

void _PCALL palLogError(PalLogger* logger, const char* fmt, ...)
{
    char buffer[1024] = {};
    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatArgs(fmt, argPtr, buffer);
    va_end(argPtr);
    palLogV(logger, PAL_LOG_LEVEL_ERROR, buffer);
}

void _PCALL palLogConsole(PalLogLevel level, const char* fmt, ...)
{
    char buffer[1024] = {};
    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatArgs(fmt, argPtr, buffer);
    va_end(argPtr);
    palLogConsoleV(level, buffer);
}

void _PCALL palLogConsoleTrace(const char* fmt, ...)
{
    char buffer[1024] = {};
    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatArgs(fmt, argPtr, buffer);
    va_end(argPtr);
    palLogConsoleV(PAL_LOG_LEVEL_TRACE, buffer);
}

void _PCALL palLogConsoleInfo(const char* fmt, ...)
{
    char buffer[1024] = {};
    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatArgs(fmt, argPtr, buffer);
    va_end(argPtr);
    palLogConsoleV(PAL_LOG_LEVEL_INFO, buffer);
}

void _PCALL palLogConsoleWarn(const char* fmt, ...)
{
    char buffer[1024] = {};
    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatArgs(fmt, argPtr, buffer);
    va_end(argPtr);
    palLogConsoleV(PAL_LOG_LEVEL_WARN, buffer);
}

void _PCALL palLogConsoleError(const char* fmt, ...)
{
    char buffer[1024] = {};
    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatArgs(fmt, argPtr, buffer);
    va_end(argPtr);
    palLogConsoleV(PAL_LOG_LEVEL_ERROR, buffer);
}

