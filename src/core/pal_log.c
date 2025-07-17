
#include "pal_pch.h"
#include "pal_console.h"

#define LOG_SIZE 1024

static PalTlsID s_TlsID = 0;
static const char* s_levelStrings[4] = { "[TRACE]: ", "[INFO]: ", "[WARN]: ", "[ERROR]: " };

typedef struct LogTLSData {
    char tmpBuffer[LOG_SIZE];
    char buffer[LOG_SIZE];
    wchar_t wideBuffer[LOG_SIZE];
    bool isLogging;
} LogTLSData;

static LogTLSData* getLogTls();

static void logV(
    LogTLSData* data, 
    PalLogger* logger, 
    PalLogLevel level);

void _PCALL palLog(
    PalLogger* logger, 
    PalLogLevel level, 
    const char* fmt, ...) {

    Uint64 size = LOG_SIZE;
    LogTLSData* data = getLogTls();
    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatV(fmt, argPtr, &size, data->tmpBuffer);
    va_end(argPtr);
    logV(data, logger, level);
}

void _PCALL palLogTrace(
    PalLogger* logger,  
    const char* fmt, ...) {

    Uint64 size = LOG_SIZE;
    LogTLSData* data = getLogTls();
    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatV(fmt, argPtr, &size, data->tmpBuffer);
    va_end(argPtr);
    logV(data, logger, PAL_LOG_LEVEL_TRACE);
}

void _PCALL palLogInfo(
    PalLogger* logger,  
    const char* fmt, ...) {

    Uint64 size = LOG_SIZE;
    LogTLSData* data = getLogTls();
    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatV(fmt, argPtr, &size, data->tmpBuffer);
    va_end(argPtr);
    logV(data, logger, PAL_LOG_LEVEL_INFO);
}

void _PCALL palLogWarn(
    PalLogger* logger,  
    const char* fmt, ...) {

    Uint64 size = LOG_SIZE;
    LogTLSData* data = getLogTls();
    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatV(fmt, argPtr, &size, data->tmpBuffer);
    va_end(argPtr);
    logV(data, logger, PAL_LOG_LEVEL_WARN);
}

void _PCALL palLogError(
    PalLogger* logger,  
    const char* fmt, ...) {

    Uint64 size = LOG_SIZE;
    LogTLSData* data = getLogTls();
    va_list argPtr;
    va_start(argPtr, fmt);
    palFormatV(fmt, argPtr, &size, data->tmpBuffer);
    va_end(argPtr);
    logV(data, logger, PAL_LOG_LEVEL_ERROR);
}

static LogTLSData* getLogTls() {

    LogTLSData* data = palGetTls(s_TlsID);
    if (!data) {
        data = palAllocate(PAL_NULL, sizeof(LogTLSData));
        palZeroMemory(data, sizeof(LogTLSData));

        // create TLS
        s_TlsID = palCreateTls();
        palSetTls(s_TlsID, data);
    }

    return data;
}

static void logV(
    LogTLSData* data, 
    PalLogger* logger, 
    PalLogLevel level) {

    if (logger && logger->callback) {
        // prevent recursive calls
        if (data->isLogging) {
            return; 
        }

        data->isLogging = PAL_TRUE;
        palSetTls(s_TlsID, data);

        logger->callback(logger->userData, level, data->tmpBuffer);
        data->isLogging = PAL_FALSE;
        palSetTls(s_TlsID, data);
        
    } else {
        // log to console
        Uint64 size = LOG_SIZE;
        palFormat(data->buffer, &size, "%s%s\n", s_levelStrings[level], data->tmpBuffer);
        palWriteConsole(level, data->wideBuffer, data->buffer);
    }
}