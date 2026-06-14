
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

// set unicode
#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include "core/pal_format.h"
#include <string.h>
#include <windows.h>

#define MSG_SIZE 4096

typedef struct {
    char tmp[MSG_SIZE];
    char buffer[MSG_SIZE];
    wchar_t wideBuffer[MSG_SIZE];
    PalBool isLogging;
} LogTLSData;

static volatile LONG s_TlsID = 0;

static void destroyTlsData(void* data)
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
    if (!fmt) {
        return;
    }

    LogTLSData* data = FlsGetValue((DWORD)s_TlsID);
    if (!data) {
        data = palAllocate(nullptr, sizeof(LogTLSData),0);
        memset(data, 0, sizeof(LogTLSData));

        // create TLS if it has not been created
        if (s_TlsID == 0) {
            DWORD TLSIndex = FlsAlloc(destroyTlsData);
            // update the TLS using atomic operations to avoid thread race
            LONG prev = InterlockedCompareExchange((volatile LONG*)&s_TlsID, (LONG)TLSIndex, 0);
            if (prev != 0) {
                // Another thread has already set this,
                // destroy the tls index
                FlsFree(TLSIndex);
            }
        }
        FlsSetValue(s_TlsID, data);
    }

    va_list argPtr;
    va_start(argPtr, fmt);
    formatArgs(fmt, argPtr, data->tmp);
    va_end(argPtr);

    // check to see if a user supplied a logger
    if (logger && logger->callback) {
        if (data->isLogging) {
            // block recursion
            return;
        }

        // update the tls to stop recursive calls
        memcpy(data->buffer, data->tmp, PAL_LOG_MSG_SIZE);
        data->isLogging = true;
        FlsSetValue(s_TlsID, data);
        logger->callback(logger->userData, data->buffer);

    } else {
        // add newline character to the string
        format(data->buffer, "%s\n", data->tmp);

        // write to console
        HANDLE console = GetStdHandle(STD_ERROR_HANDLE);
        int len = MultiByteToWideChar(CP_UTF8, 0, data->buffer, -1, nullptr, 0);
        if (!len) {
            return;
        }

        MultiByteToWideChar(CP_UTF8, 0, data->buffer, -1, data->wideBuffer, len);
        if (console) {
            WriteConsoleW(console, data->wideBuffer, (DWORD)len - 1, NULL, 0);
        } else {
            OutputDebugStringW(data->wideBuffer);
        }
    }

    data->isLogging = false;
    FlsSetValue(s_TlsID, data);
}

#endif // _WIN32
