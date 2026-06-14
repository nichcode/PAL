
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal/core/log.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

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
#include <windows.h>

static volatile LONG s_TlsID = 0;

#else
#include <pthread.h>

pthread_key_t s_TLSID = 0;
static pthread_once_t s_TLSCreation = PTHREAD_ONCE_INIT;

static void destroyTlsData(void* data);
void createTLSID()
{
    if (pthread_key_create(&s_TLSID, destroyTlsData) != 0) {
        return;
    }
}

#endif // _WIN32

#define PAL_LOG_MSG_SIZE 4096

typedef struct {
    char tmp[PAL_LOG_MSG_SIZE];
    char buffer[PAL_LOG_MSG_SIZE];
    wchar_t wideBuffer[PAL_LOG_MSG_SIZE];
    PalBool isLogging;
} LogTLSData;

static void destroyTlsData(void* data)
{
    LogTLSData* tlsData = data;
    if (tlsData) {
        free(tlsData);
    }
}

static inline LogTLSData* getLogTlsData()
{
#ifdef _WIN32
    LogTLSData* data = FlsGetValue((DWORD)s_TlsID);
#else
    LogTLSData* data = pthread_getspecific(s_TLSID);
#endif // _WIN32

    if (!data) {
        data = malloc(sizeof(LogTLSData));
        memset(data, 0, sizeof(LogTLSData));
        // create TLS if it has not been created
#ifdef _WIN32
        if (s_TlsID == 0) {
            DWORD TLSIndex = FlsAlloc(destroyTlsData);
            if (TLSIndex == TLS_OUT_OF_INDEXES) {
                return nullptr;
            } else {
                // update the TLS using atomic operations to avoid thread race
                LONG prev = InterlockedCompareExchange((volatile LONG*)&s_TlsID, (LONG)TLSIndex, 0);
                if (prev != 0) {
                    // Another thread has already set this,
                    // destroy the tls index
                    FlsFree(TLSIndex);
                }
            }
        }
        FlsSetValue(s_TlsID, data);
#else
        pthread_once(&s_TLSCreation, createTLSID);
        pthread_setspecific(s_TLSID, data);
#endif // _WIN32
    }
    return data;
}

static inline void updateLogTlsData(LogTLSData* data)
{
#ifdef _WIN32
    FlsSetValue(s_TlsID, data);
#else
    pthread_setspecific(s_TLSID, data);
#endif // _WIN32
}

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

static inline void writeToConsole(LogTLSData* data)
{
#ifdef _WIN32
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
#else
    fprintf(stdout, "%s", data->buffer);
    fflush(stdout);
#endif // _WIN32
}

void PAL_CALL palLog(
    const PalLogger* logger,
    const char* fmt,
    ...)
{
    if (!fmt) {
        return;
    }

    LogTLSData* data = getLogTlsData();
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
        updateLogTlsData(data);
        logger->callback(logger->userData, data->buffer);

    } else {
        // add newline character to the string
        format(data->buffer, "%s\n", data->tmp);
        writeToConsole(data);
    }

    data->isLogging = false;
    updateLogTlsData(data);
}