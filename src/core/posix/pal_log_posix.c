
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal_posix.h"

#if _PAL_ON_POSIX
#include "core/pal_format.h"
#include <string.h>
#include <pthread.h>

#define MSG_SIZE 4096

typedef struct {
    char tmp[MSG_SIZE];
    char buffer[MSG_SIZE];
    PalBool isLogging;
} LogTLSData;

pthread_key_t s_TLSID = 0;
static pthread_once_t s_TLSCreation = PTHREAD_ONCE_INIT;

static void destroyTlsData(void* data)
{
    LogTLSData* tlsData = data;
    if (tlsData) {
        palFree(nullptr, tlsData);
    }
}

void createTLSID()
{
    if (pthread_key_create(&s_TLSID, destroyTlsData) != 0) {
        return;
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

    LogTLSData* data = pthread_getspecific(s_TLSID);
    if (!data) {
        data = palAllocate(nullptr, sizeof(LogTLSData), 0);
        memset(data, 0, sizeof(LogTLSData));

        // create TLS if it has not been created
        pthread_once(&s_TLSCreation, createTLSID);
        pthread_setspecific(s_TLSID, data);
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
        memcpy(data->buffer, data->tmp, MSG_SIZE);
        data->isLogging = PAL_TRUE;
        pthread_setspecific(s_TLSID, data);
        logger->callback(logger->userData, data->buffer);

    } else {
        // add newline character to the string
        format(data->buffer, "%s\n", data->tmp);

        // write to console
        fprintf(stdout, "%s", data->buffer);
        fflush(stdout);
    }

    data->isLogging = PAL_FALSE;
    pthread_setspecific(s_TLSID, data);
}

#endif // _PAL_ON_POSIX