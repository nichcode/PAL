
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

#include "pal/pal_core.h"

#include <stdio.h>
#include <stdarg.h>

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

#define PAL_ALIGNMENT 16
#define PAL_VMAJOR 1
#define PAL_VMINOR 0
#define PAL_VPATCH 0
#define PAL_VSTRING "1.0.0"
#define PAL_LOG_SIZE 1024

static DWORD s_TlsID = 0;
static const char* s_NewlineString = "\n";

typedef struct {
    char buffer[PAL_LOG_SIZE];
    wchar_t wideBuffer[PAL_LOG_SIZE];
} LogTLSData;

static LogTLSData* getLogTlsData();

// ==================================================
// Public API
// ==================================================

const char* _PCALL palResultToString(PalResult result) {
        
    switch (result) {
        case PAL_RESULT_SUCCESS:
        return "Success";

        case PAL_RESULT_NULL_POINTER:
        return "Invalid pointer";

        case PAL_RESULT_INVALID_ARGUMENT:
        return "Invalid argument";

        case PAL_RESULT_OUT_OF_MEMORY:
        return "Out of memory";

        case PAL_RESULT_INVALID_ALLOCATOR:
        return "Invalid allocator";

        case PAL_RESULT_ACCESS_DENIED:
        return "Access denied";

        case PAL_RESULT_INVALID_DISPLAY:
        return "Invalid display";
   }

    return nullptr;
}

PalVersion _PCALL palGetVersion() { 

    return (PalVersion){
        .major = PAL_VMAJOR,
        .minor = PAL_VMINOR,
        .patch = PAL_VPATCH
    };
}

const char* _PCALL palGetVersionString() {

    return PAL_VSTRING;
}

void* _PCALL palAllocate(
    PalAllocator* allocator, 
    Uint64 size,
    Uint64 PAL_alignment) {

    Uint64 align = PAL_alignment;
    if (align == 0) {
        align = PAL_ALIGNMENT;
    }
    
    if (allocator && allocator->allocate) {
        return allocator->allocate(allocator->userData, size, align);
    }

#ifdef _WIN32
    return _aligned_malloc(size, align);
#endif // _WIN32
}

void _PCALL palFree(
    PalAllocator* allocator, 
    void* ptr) {

    if (allocator && allocator->free && ptr) {
        allocator->free(allocator->userData, ptr);
        return;
    }

#ifdef _WIN32
    _aligned_free(ptr);
#endif // _WIN32
}

void _PCALL palLog(const char* fmt, ...) {

    if (!fmt) {
        return;
    }

    LogTLSData* data = getLogTlsData();

    // get arguments
    va_list argPtr;
    va_start(argPtr, fmt);
    va_list listCopy;
#ifdef _MSC_VER
    listCopy = argPtr;
#else
    __builtin_va_copy(listCopy, argPtr);
#endif // _MSC_VER

    int len = vsnprintf(0, 0, fmt, listCopy);

    vsnprintf(data->buffer, len + 1, fmt, listCopy);
    data->buffer[len] = 0;
    va_end(argPtr);

    // log to console
#ifdef _WIN32
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    len = MultiByteToWideChar(CP_UTF8, 0, data->buffer, -1, nullptr, 0);
    if (!len) {
        return;
    }

    MultiByteToWideChar(CP_UTF8, 0, data->buffer, -1, data->wideBuffer, len);
    if (console) {
        WriteConsoleW(console, data->wideBuffer, (DWORD)len - 1, NULL, 0);
        WriteConsoleW(console, s_NewlineString, 1, NULL, 0);
    } else {
        OutputDebugStringW(data->wideBuffer);
        OutputDebugStringA(s_NewlineString);
    }
#endif // _WIN32
}

PalTimer _PCALL palGetSystemTimer() {

    Uint32 freq = palGetPerformanceFrequency();
    Uint32 startTime = palGetPerformanceCounter();
    return (PalTimer) {
        .frequency = freq,
        .startTime = startTime
    };
}

double _PCALL palGetTime(PalTimer* timer) {

    if (!timer) {
        return 0.0;
    }

#ifdef _WIN32
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (double)(now.QuadPart - timer->startTime) / (double)timer->frequency;
#endif // _WIN32
}

Uint64 _PCALL palGetPerformanceCounter() {

#ifdef _WIN32
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (Uint64)counter.QuadPart;
#endif // _WIN32
}

Uint64 _PCALL palGetPerformanceFrequency() {

#ifdef _WIN32
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return (Uint64)frequency.QuadPart;
#endif // _WIN32
}


// ==================================================
// Internal API
// ==================================================

static LogTLSData* getLogTlsData() {

#ifdef _WIN32
    LogTLSData* data = TlsGetValue(s_TlsID);
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

    memset(data, 0, sizeof(LogTLSData));
    return data;
}