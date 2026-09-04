
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

#include "format.h"
#include "log_priv.h"
#include <string.h>

void destroyTlsData_(void* data)
{
    LogTLSData* tlsData = data;
    if (tlsData) {
        palFree(nullptr, tlsData);
    }
}

void PAL_CALL palLog(
    const PalLogger* logger,
    const char* fmt,
    ...)
{
    LogTLSData* data = getLogTLSData_();
    if (!data) {
        data = palAllocate(nullptr, sizeof(LogTLSData), 0);
        memset(data, 0, sizeof(LogTLSData));

        // create TLS if it has not been created
        createLogTLS_();
        setLogTLSData_(data);
    }

    va_list argPtr;
    va_start(argPtr, fmt);
    formatArgs_(fmt, argPtr, data->tmp);
    va_end(argPtr);

    // check to see if a user supplied a logger
    if (logger && logger->callback) {
        if (data->isLogging) {
            // block recursion
            return;
        }

        // update the tls to stop recursive calls
        memcpy(data->buffer, data->tmp, LOG_MSG_SIZE_);
        data->isLogging = PAL_TRUE;
        setLogTLSData_(data);
        logger->callback(logger->userData, data->buffer);

    } else {
        // add newline character to the string
        format_(data->buffer, "%s\n", data->tmp);

        // write to console
        fprintf(stdout, "%s", data->buffer);
        fflush(stdout);
    }

    data->isLogging = PAL_FALSE;
    setLogTLSData_(data);
}