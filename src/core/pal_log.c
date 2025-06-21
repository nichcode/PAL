
#include "pal_pch.h"
#include "pal_internal.h"
#include "pal_platform.h"

static const char* levels[4] = { "[TRACE]: ", "[INFO]:  ", "[WARN]:  ", "[ERROR]: " };

void PAL_CALL palLog(PAL_LogLevel level, const char* fmt, ...)
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

void PAL_CALL palLogTrace(const char* fmt, ...)
{
    char tmp[PAL_MSG_SIZE] = {};
    char buffer[PAL_MSG_SIZE] = {};

    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatArgs(fmt, argPtr, tmp);
    va_end(argPtr);

    palFormat(buffer, "%s%s\n", levels[PAL_LOG_TRACE], tmp);
    palWriteConsole(PAL_LOG_TRACE, buffer);
}

void PAL_CALL palLogInfo(const char* fmt, ...)
{
    char tmp[PAL_MSG_SIZE] = {};
    char buffer[PAL_MSG_SIZE] = {};

    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatArgs(fmt, argPtr, tmp);
    va_end(argPtr);

    palFormat(buffer, "%s%s\n", levels[PAL_LOG_INFO], tmp);
    palWriteConsole(PAL_LOG_INFO, buffer);
}

void PAL_CALL palLogWarn(const char* fmt, ...)
{
    char tmp[PAL_MSG_SIZE] = {};
    char buffer[PAL_MSG_SIZE] = {};

    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatArgs(fmt, argPtr, tmp);
    va_end(argPtr);

    palFormat(buffer, "%s%s\n", levels[PAL_LOG_WARN], tmp);
    palWriteConsole(PAL_LOG_WARN, buffer);
}

void PAL_CALL palLogError(const char* fmt, ...)
{
    char tmp[PAL_MSG_SIZE] = {};
    char buffer[PAL_MSG_SIZE] = {};

    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatArgs(fmt, argPtr, tmp);
    va_end(argPtr);

    palFormat(buffer, "%s%s\n", levels[PAL_LOG_ERROR], tmp);
    palWriteConsole(PAL_LOG_ERROR, buffer);
}