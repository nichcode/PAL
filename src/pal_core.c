
/**

Copyright (C) 2025 Nicholas Agbo

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

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
#define PAL_LOG_MSG_SIZE 4096

static volatile LONG s_TlsID = 0;

typedef struct {
    char tmp[PAL_LOG_MSG_SIZE];
    char buffer[PAL_LOG_MSG_SIZE];
    wchar_t wideBuffer[PAL_LOG_MSG_SIZE];
    bool isLogging;
} LogTLSData;

// ==================================================
// Internal API
// ==================================================

static inline void* alignedAlloc(
    Uint64 size,
    Uint64 alignment)
{
#if defined(_MSC_VER) || defined(__MINGW32__)
    return _aligned_malloc(size, alignment);
#elif defined(_ISOC11_SOURCE) ||                                               \
    defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    return aligned_alloc(alignment, size);
#else
    void* ptr = nullptr;
    posix_memalign(&ptr, alignment);
    return ptr;
#endif // _MSC_VER
}

static inline void alignedFree(void* ptr)
{
#if defined(_MSC_VER) || defined(__MINGW32__)
    _aligned_free(ptr);
#else
    free(ptr);
#endif // _MSC_VER
}

static void destroyTlsData(void* data)
{
    LogTLSData* tlsData = data;
    if (tlsData) {
        palFree(nullptr, tlsData);
    }
}

static inline LogTLSData* getLogTlsData()
{
#ifdef _WIN32
    LogTLSData* data = FlsGetValue((DWORD)s_TlsID);
#endif // _WIN32

    if (!data) {
        data = palAllocate(nullptr, sizeof(LogTLSData), 0);
        memset(data, 0, sizeof(LogTLSData));

        // create TLS if it has not been created
#ifdef _WIN32
        if (s_TlsID == 0) {
            DWORD TLSIndex = FlsAlloc(destroyTlsData);
            if (TLSIndex == TLS_OUT_OF_INDEXES) {
                // FIXME: Use a global log buffer with a mutex
                return nullptr;
            } else {
                // update the TLS using atomic operations to avoid thread race
                LONG prev = InterlockedCompareExchange(
                    (volatile LONG*)&s_TlsID,
                    (LONG)TLSIndex,
                    0);

                if (prev != 0) {
                    // Another thread has already set this,
                    // destroy the tls index
                    FlsFree(TLSIndex);
                }
            }
        }
        FlsSetValue(s_TlsID, data);
#endif // _WIN32
    }
    return data;
}

static inline void updateLogTlsData(LogTLSData* data)
{
#ifdef _WIN32
    FlsSetValue(s_TlsID, data);
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

PalVersion PAL_CALL palGetVersion()
{
    return (PalVersion){.major = PAL_VERSION_MAJOR,
                        .minor = PAL_VERSION_MINOR,
                        .build = PAL_VERSION_BUILD};
}

const char* PAL_CALL palGetVersionString()
{
    return PAL_VERSION_STRING;
}

const char* PAL_CALL palFormatResult(PalResult result)
{
    switch (result) {
        case PAL_RESULT_SUCCESS:
            return "Success";

        case PAL_RESULT_NULL_POINTER:
            return "Null pointer";

        case PAL_RESULT_INVALID_ARGUMENT:
            return "Invalid argument";

        case PAL_RESULT_OUT_OF_MEMORY:
            return "Out of memory";

        case PAL_RESULT_PLATFORM_FAILURE:
            return "Platform error";

        case PAL_RESULT_INVALID_ALLOCATOR:
            return "Invalif allocator";

        case PAL_RESULT_ACCESS_DENIED:
            return "Access denied";

        case PAL_RESULT_TIMEOUT:
            return "Timeout expired";

        case PAL_RESULT_INSUFFICIENT_BUFFER:
            return "Insufficient buffer";

        // thread
        case PAL_RESULT_INVALID_THREAD:
            return "Invalid thread";

        case PAL_RESULT_THREAD_FEATURE_NOT_SUPPORTED:
            return "Unsupported thread feature";

        // video
        case PAL_RESULT_VIDEO_NOT_INITIALIZED:
            return "Video system not initialized";

        case PAL_RESULT_INVALID_MONITOR:
            return "Invalid monitor";

        case PAL_RESULT_INVALID_MONITOR_MODE:
            return "Invalid monitor display mode";

        case PAL_RESULT_INVALID_WINDOW:
            return "Invalid window";

        case PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED:
            return "Unsupported video feature";

        case PAL_RESULT_INVALID_KEYCODE:
            return "Invalid keycode";

        case PAL_RESULT_INVALID_SCANCODE:
            return "Invalid scancode";

        case PAL_RESULT_INVALID_MOUSE_BUTTON:
            return "Invalid mouse button";

        case PAL_RESULT_INVALID_ORIENTATION:
            return "Invalid orientation";

        // opengl
        case PAL_RESULT_GL_NOT_INITIALIZED:
            return "Opengl system not initialized";

        case PAL_RESULT_INVALID_GL_WINDOW:
            return "Invalid opengl window";

        case PAL_RESULT_GL_EXTENSION_NOT_SUPPORTED:
            return "Unsupported opengl extension";

        case PAL_RESULT_INVALID_GL_FBCONFIG:
            return "Invalid opengl framebuffer";

        case PAL_RESULT_INVALID_GL_VERSION:
            return "Unsupported opengl version";

        case PAL_RESULT_INVALID_GL_PROFILE:
            return "Unsupported opengl profile";

        case PAL_RESULT_INVALID_GL_CONTEXT:
            return "Invalid opengl context";
    }
    return "Unknown";
}

void* PAL_CALL palAllocate(
    const PalAllocator* allocator,
    Uint64 size,
    Uint64 alignment)
{
    Uint64 align = alignment;
    if (align == 0) {
        align = PAL_DEFAULT_ALIGNMENT;
    }

    if (allocator && allocator->allocate) {
        return allocator->allocate(allocator->userData, size, align);
    }
    return alignedAlloc(size, align);
}

void PAL_CALL palFree(
    const PalAllocator* allocator,
    void* ptr)
{
    if (allocator && allocator->free && ptr) {
        allocator->free(allocator->userData, ptr);

    } else {
        alignedFree(ptr);
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

Uint64 PAL_CALL palGetPerformanceCounter()
{
#ifdef _WIN32
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (Uint64)counter.QuadPart;
#endif // _WIN32
}

Uint64 PAL_CALL palGetPerformanceFrequency()
{
#ifdef _WIN32
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return (Uint64)frequency.QuadPart;
#endif // _WIN32
}