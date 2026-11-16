/**
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

#include "platform.h"
#include "pal2/core/memory.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void PAL_CALL palLog(
    const PalLogger* logger,
    const char* fmt,
    ...)
{
    if (!fmt) {
        return;
    }

    platformCreateLogTLS();

    LogTLSData* data = platformGetLogTLSData();
    if (!data) {
        data = palAllocate(nullptr, sizeof(LogTLSData), 0);
        if (!data) {
            return;
        }
        memset(data, 0, sizeof(LogTLSData));
        platformSetLogTLSData(data);
    }

    if (data->isLogging) {
        return;
    }

    va_list argPtr;
    va_start(argPtr, fmt);
    formatMsgArgs(fmt, argPtr, data->tmp, PAL_LOG_MSG_SIZE);
    va_end(argPtr);

    if (logger && logger->callback) {
        data->isLogging = true;
        logger->callback(logger->userData, data->tmp);
        data->isLogging = false;

    } else {
        (void)fprintf(stdout, "%s\n", data->tmp);
        (void)fflush(stdout);
    }
}