
#include "PAL_pch.h"
#include "PAL_platform.h"

static const char* levels[4] = { "[TRACE]: ", "[INFO]:  ", "[WARN]:  ", "[ERROR]: " };

void PAL_Log(Uint32 level, const char* fmt, ...)
{
    char tmp[PAL_MESSAGE_SIZE] = {};
    char buffer[PAL_MESSAGE_SIZE] = {};

    va_list argPtr;
    va_start(argPtr, fmt);
    PAL_FormatArgs(fmt, argPtr, tmp);
    va_end(argPtr);

    PAL_Format(buffer, "%s%s\n", levels[level], tmp);
    PAL_WriteConsole(level, buffer);
}

void PAL_LogTrace(const char* fmt, ...)
{
    char tmp[PAL_MESSAGE_SIZE] = {};
    char buffer[PAL_MESSAGE_SIZE] = {};

    va_list argPtr;
    va_start(argPtr, fmt);
    PAL_FormatArgs(fmt, argPtr, tmp);
    va_end(argPtr);

    PAL_Format(buffer, "%s%s\n", levels[PAL_LOG_TRACE], tmp);
    PAL_WriteConsole(PAL_LOG_TRACE, buffer);
}

void PAL_LogInfo(const char* fmt, ...)
{
    char tmp[PAL_MESSAGE_SIZE] = {};
    char buffer[PAL_MESSAGE_SIZE] = {};

    va_list argPtr;
    va_start(argPtr, fmt);
    PAL_FormatArgs(fmt, argPtr, tmp);
    va_end(argPtr);

    PAL_Format(buffer, "%s%s\n", levels[PAL_LOG_INFO], tmp);
    PAL_WriteConsole(PAL_LOG_INFO, buffer);
}

void PAL_LogWarn(const char* fmt, ...)
{
    char tmp[PAL_MESSAGE_SIZE] = {};
    char buffer[PAL_MESSAGE_SIZE] = {};

    va_list argPtr;
    va_start(argPtr, fmt);
    PAL_FormatArgs(fmt, argPtr, tmp);
    va_end(argPtr);

    PAL_Format(buffer, "%s%s\n", levels[PAL_LOG_WARN], tmp);
    PAL_WriteConsole(PAL_LOG_WARN, buffer);
}

void PAL_LogError(const char* fmt, ...)
{
    char tmp[PAL_MESSAGE_SIZE] = {};
    char buffer[PAL_MESSAGE_SIZE] = {};

    va_list argPtr;
    va_start(argPtr, fmt);
    PAL_FormatArgs(fmt, argPtr, tmp);
    va_end(argPtr);

    PAL_Format(buffer, "%s%s\n", levels[PAL_LOG_ERROR], tmp);
    PAL_WriteConsole(PAL_LOG_ERROR, buffer);
}