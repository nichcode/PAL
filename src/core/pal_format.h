
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_FORMAT_H
#define _PAL_FORMAT_H

#include "pal/pal_core.h"
#include <stdarg.h>
#include <stdio.h>

static inline void formatArgs(
    const char* fmt,
    va_list argsList,
    char* buffer)
{
    va_list argsListCopy;
    va_copy(argsListCopy, argsList);
    int len = vsnprintf(nullptr, 0, fmt, argsListCopy);
    va_end(argsListCopy);

    va_copy(argsListCopy, argsList);
    vsnprintf(buffer, len + 1, fmt, argsListCopy);
    va_end(argsListCopy);
    buffer[len] = 0;
}

static inline void format(
    char* buffer,
    const char* fmt,
    ...)
{
    va_list argPtr;
    va_start(argPtr, fmt);
    formatArgs(fmt, argPtr, buffer);
    va_end(argPtr);
}

#endif // _PAL_FORMAT_H