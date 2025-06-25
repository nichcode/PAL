
#include "pal_pch.h"
#include "pal_internal.h"

static const char* levels[4] = { "[TRACE]: ", "[INFO]:  ", "[WARN]:  ", "[ERROR]: " };

void _PCALL palLog(PalLogLevel level, const char* fmt, ...)
{
    char tmp[_PAL_MSG_SIZE] = {};
    va_list argPtr;
    va_start(argPtr, fmt);
    _palFormatArgs(fmt, argPtr, tmp);
    va_end(argPtr);

    char buffer[_PAL_MSG_SIZE] = {};
    _palFormat(buffer, "%s%s\n", levels[level], tmp);
    _palPlatformWriteConsole(level, buffer);
}