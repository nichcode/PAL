
/**
 * PAL - Prime Abstraction Layer (PAL)
 * A cross platform abstraction layer over graphics and windowing APIs
 * -------------------------------------------------------------------
 *
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "shared.h"
#include "pal2/pal_core.h"

#include <stdio.h>
#include <stdlib.h>

void handleAssert(
    const char* file, 
    uint32_t line, 
    const char* expr, 
    const char* msg)
{
    palLog(
        nullptr, 
        "ASSERTION:\n File:    %s\n Line:    %d\n Expr:    %s\n Message: %s",
        file, 
        line, 
        expr, 
        msg);

    abort();
}

void formatMsgArgs(
    const char* fmt,
    va_list argsList,
    char* buffer, 
    uint64_t bufferSize)
{
    va_list argsListCopy;
    va_copy(argsListCopy, argsList);
    (void)vsnprintf(nullptr, 0, fmt, argsListCopy);
    va_end(argsListCopy);

    va_copy(argsListCopy, argsList);
    (void)vsnprintf(buffer, bufferSize, fmt, argsListCopy);
    va_end(argsListCopy);
}

void formatMsg(
    char* buffer,
    uint64_t bufferSize,
    const char* fmt,
    ...)
{
    va_list argPtr;
    va_start(argPtr, fmt);
    formatMsgArgs(fmt, argPtr, buffer, bufferSize);
    va_end(argPtr);
}