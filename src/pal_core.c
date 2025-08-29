
// ==================================================
// Includes
// ==================================================

#include "pal/pal_core.h"

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
#endif // _WIN32

#if defined(_MSC_VER) || defined(__MINGW32__)
#include <malloc.h>
#endif // _MSC_VER

#include <stdarg.h>
#include <stdio.h>

// ==================================================
// Typedefs, enums and structs
// ==================================================

#define PAL_DEFAULT_ALIGNMENT 16
#define PAL_VERSION_MAJOR 1
#define PAL_VERSION_MINOR 0
#define PAL_VERSION_BUILD 0
#define PAL_VERSION_STRING "1.0.0"
#define PAL_LOG_MSG_SIZE 1024

static Uint32 s_TlsID = 0;

typedef struct {
    char buffer[PAL_LOG_MSG_SIZE];
    wchar_t wideBuffer[PAL_LOG_MSG_SIZE];
    bool isLogging;
} LogTLSData;

// ==================================================
// Internal API
// ==================================================

static inline void* alignedAlloc(
    Uint64 size,
    Uint64 alignment) { 

#if defined(_MSC_VER) || defined(__MINGW32__)
    return _aligned_malloc(size, alignment);
#elif defined(_ISOC11_SOURCE) || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    return aligned_alloc(alignment, size);
#else
    void* ptr = nullptr;
    posix_memalign(&ptr, alignment);
    return ptr;
#endif // _MSC_VER
}

static inline void alignedFree(void* ptr) { 

#if defined(_MSC_VER) || defined(__MINGW32__)
    _aligned_free(ptr);
#else
    free(ptr);
#endif // _MSC_VER
}

static inline LogTLSData* getLogTlsData() {

#ifdef _WIN32
    LogTLSData* data = TlsGetValue((DWORD)s_TlsID);
#endif // _WIN32

    if (!data) {
        data = palAllocate(nullptr, sizeof(LogTLSData), 16);
        memset(data, 0, sizeof(LogTLSData));

        // create TLS
#ifdef _WIN32
        s_TlsID = TlsAlloc();
        TlsSetValue(s_TlsID, data);
#endif // _WIN32
    }
    return data;
}

static inline void updateLogTlsData(LogTLSData* data) {

#ifdef _WIN32
    TlsSetValue(s_TlsID, data);
#endif // _WIN32
}

static inline void formatArgs(
    const char* fmt, 
    va_list argsList, 
    char* buffer) {

    va_list listCopy;
#ifdef _MSC_VER
    listCopy = argsList;
#else
    __builtin_va_copy(listCopy, argsList);
#endif // _MSC_VER

    int len = vsnprintf(0, 0, fmt, listCopy);
    vsnprintf(buffer, len + 1, fmt, listCopy);
    buffer[len] = 0;
}

static inline void format(
    char* buffer, 
    const char* fmt, ...) {

    va_list argPtr;
    va_start(argPtr, fmt);
    formatArgs(fmt, argPtr, buffer);
    va_end(argPtr);
}

static inline void writeToConsole(LogTLSData* data) {

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

// ==================================================
// Public API
// ==================================================

PalVersion _PCALL palGetVersion() {

    return (PalVersion) {
        .major = PAL_VERSION_MAJOR,
        .minor = PAL_VERSION_MINOR,
        .build = PAL_VERSION_BUILD
    };
}

const char* _PCALL palGetVersionString() {

    return PAL_VERSION_STRING;
}

const char* _PCALL palFormatResult(PalResult result) {

    switch (result) {
        case PAL_RESULT_SUCCESS: 
        return "The operation completed successfully";

        case PAL_RESULT_NULL_POINTER: 
        return "The pointer is invalid";

        case PAL_RESULT_INVALID_PARAMETER: 
        return "The parameter is invalid.";

        case PAL_RESULT_OUT_OF_MEMORY: 
        return "The platform(OS) has no free memory";

        case PAL_RESULT_PLATFORM_FAILURE: 
        return "An error occured on the platform(OS)";

        case PAL_RESULT_INVALID_ALLOCATOR:
        return "The provided allocator's function pointers are not fully set";

    }
    return "Unknown";
}

void* _PCALL palAllocate(
    const PalAllocator* allocator,
    Uint64 size,
    Uint64 alignment) {
    
    Uint64 align = alignment;
    if (align == 0) {
        align = PAL_DEFAULT_ALIGNMENT;
    }

    if (allocator && allocator->allocate) {
        return allocator->allocate(allocator->userData, size, align);
    }
    return alignedAlloc(size, alignment);
}

void _PCALL palFree(const PalAllocator* allocator, void* ptr) {

    if (allocator && allocator->free && ptr) {
        allocator->free(allocator->userData, ptr);

    } else {
        alignedFree(ptr);
    }
}

void _PCALL palLog(const PalLogger* logger, const char* fmt, ...) {
    
    if (!fmt) {
        return;
    }

    char tmp[PAL_LOG_MSG_SIZE];
    va_list argPtr;
    va_start(argPtr, fmt);
    formatArgs(fmt, argPtr, tmp);
    va_end(argPtr);

    // check to see if a user supplied a logger
    LogTLSData* data = getLogTlsData();
    if (logger && logger->callback) {
        if (data->isLogging) {
            // block recursion
            return;
        }

        // update the tls to stop recursive calls
        memcpy(data->buffer, tmp, PAL_LOG_MSG_SIZE);
        data->isLogging = true;
        updateLogTlsData(data);
        logger->callback(logger->userData, data->buffer);

    } else {
        // add newline character to the string
        format(data->buffer, "%s\n", tmp);
        writeToConsole(data);
    }

    data->isLogging = false;
    updateLogTlsData(data);
}

const char* _PCALL palGetLastLogMessage() {

    LogTLSData* data = getLogTlsData();
    return data->buffer;
}