
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

#include "core_platform.h"
#include "shared.h"
#include <stdio.h>
#include <string.h>

void PAL_CALL palLog(
    const PalLogger* logger,
    const char* fmt,
    ...)
{
    LogTLSData* data = corePlatformGetLogTLSData();
    if (!data) {
        data = palAllocate(nullptr, sizeof(LogTLSData), 0);
        memset(data, 0, sizeof(LogTLSData));
        corePlatformCreateLogTLS();
        corePlatformSetLogTLSData(data);
    }

    va_list argPtr;
    va_start(argPtr, fmt);
    formatMsgArgs(fmt, argPtr, data->tmp, PAL_LOG_MSG_SIZE);
    va_end(argPtr);

    if (logger && logger->callback) {
        if (data->isLogging) {
            return;
        }

        (void)memcpy(data->buffer, data->tmp, PAL_LOG_MSG_SIZE);
        data->isLogging = PAL_TRUE;
        corePlatformSetLogTLSData(data);
        logger->callback(logger->userData, data->buffer);

    } else {
        formatMsg(data->buffer, PAL_LOG_MSG_SIZE, "%s\n", data->tmp);
        (void)fprintf(stdout, "%s", data->buffer);
        (void)fflush(stdout);
    }

    data->isLogging = PAL_FALSE;
    corePlatformSetLogTLSData(data);
}