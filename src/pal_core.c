
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
#define PAL_MAX_EVENTS 128

static DWORD s_TlsID = 0;
static const char* s_NewlineString = "\n";

typedef struct LogTLSData {
    char buffer[PAL_LOG_SIZE];
    wchar_t wideBuffer[PAL_LOG_SIZE];
} LogTLSData;

typedef struct QueueData {
    PalEvent data[PAL_MAX_EVENTS];
    Int32 head;
    Int32 tail;
} QueueData;

typedef struct PalEventDriver {
    PalDispatchMode modes[PAL_MAX_EVENTS];
    PalEventQueue* queue;
    PalAllocator* allocator;
    PalEventCallback callback;
    void* userData;
    bool freeQueue;
} PalEventDriver;

static LogTLSData* getLogTlsData();

static void palDefaultPush(
    void* queue,
    PalEvent* event);

static bool palDefaultPoll(
    void* queue, 
    PalEvent* outEvent);

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

        case PAL_RESULT_INSUFFICIENT_BUFFER:
        return "Buffer size is too small";

        case PAL_RESULT_INVALID_DISPLAY:
        return "Invalid display";

        case PAL_RESULT_INVALID_OPERATION:
        return "Invalid operation";


        case PAL_RESULT_INVALID_DISPLAY_MODE:
        return "Invalid display mode";

        case PAL_RESULT_INVALID_ORIENTATION:
        return "Invalid display orientation";

        case PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED:
        return "Video feature not supported";

        case PAL_RESULT_VIDEO_NOT_INITIALIZED:
        return "Video system not initialized.";

        case PAL_RESULT_INVALID_WINDOW:
        return "Invalid window";

        
        case PAL_RESULT_INPUT_NOT_INITIALIZED:
        return "Input system not initialized.";

        case PAL_RESULT_INVALID_INPUT_DEVICE:
        return "Invalid input device";

        case PAL_RESULT_INPUT_DEVICE_NOT_REGISTERED:
        return "Input device not registered";
        

        case PAL_RESULT_GL_ICD_NOT_LOADED:
        return "GL ICD not loaded";

        case PAL_RESULT_GL_EXTENSION_NOT_SUPPORTED:
        return "GL extension not supported";
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

    Int32 len = vsnprintf(0, 0, fmt, listCopy);

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

PalClock _PCALL palGetSystemClock() {

    Uint32 freq = palGetPerformanceFrequency();
    Uint32 start = palGetPerformanceCounter();
    return (PalClock) {
        .frequency = freq,
        .startTime = start
    };
}

double _PCALL palGetTime(PalClock* clock) {

    if (!clock) {
        return 0.0;
    }

#ifdef _WIN32
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (double)(now.QuadPart - clock->startTime) / (double)clock->frequency;
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

PalResult _PCALL palCreateEventDriver(
    PalEventDriverCreateInfo* info, 
    PalEventDriver** outEventDriver) {
    
    if (!info || !outEventDriver) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (info->allocator) {
        if (!info->allocator->allocate && !info->allocator->free) {
            return PAL_RESULT_INVALID_ALLOCATOR;
        }
    }

    PalEventDriver* driver = palAllocate(
        info->allocator,
        sizeof(PalEventDriver),
        0
    );
    if (!driver) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(driver, 0, sizeof(PalEventDriver));
    if (info->allocator) {
        driver->allocator = info->allocator;
    }

    if (info->queue) {
        driver->queue = info->queue;
        driver->freeQueue = false;

    } else {
        driver->queue = palAllocate(info->allocator, sizeof(PalEventQueue), 0);
        if (!driver->queue) {
            palFree(info->allocator, driver);
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        driver->queue->userData = palAllocate(info->allocator, sizeof(QueueData), 0);
        if (!driver->queue->userData) {
            palFree(info->allocator, driver->queue);
            palFree(info->allocator, driver);
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        memset(driver->queue->userData, 0, sizeof(QueueData));
        driver->queue->poll = palDefaultPoll;
        driver->queue->push = palDefaultPush;
        driver->freeQueue = true;
    }

    driver->callback = info->callback;
    *outEventDriver = driver;
    return PAL_RESULT_SUCCESS;
}

void _PCALL palDestroyEventDriver(PalEventDriver* eventDriver) {

    if (!eventDriver) {
        return;
    }

    if (eventDriver->freeQueue) {
        palFree(eventDriver->allocator, eventDriver->queue->userData);
        palFree(eventDriver->allocator, eventDriver->queue);
    }
    palFree(eventDriver->allocator, eventDriver);
}

_PAPI void _PCALL palSetEventDispatchMode(
    PalEventDriver* eventDriver, 
    PalEventType type, 
    PalDispatchMode mode) {

    if (!eventDriver) {
        return;
    }
    eventDriver->modes[type] = mode;
}

void _PCALL palSetAllEventDispatchMode(
    PalEventDriver* eventDriver, 
    PalDispatchMode mode) {
    
    if (!eventDriver) {
        return;
    }
    
    for (Int32 i = 0; i < PAL_EVENT_MAX; i++) {
        eventDriver->modes[i] = mode;
    }
}

PalDispatchMode _PCALL palGetEventDispatchMode(
    PalEventDriver* eventDriver, 
    PalEventType type) {

    if (!eventDriver) {
        return PAL_DISPATCH_NONE;
    }
    return eventDriver->modes[type];
}

void _PCALL palPushEvent(
    PalEventDriver* eventDriver,
    PalEvent* event) {

    if (!eventDriver || !event) {
        return;
    }

    // get the event mode
    PalDispatchMode mode = eventDriver->modes[event->type];
    if (mode == PAL_DISPATCH_CALLBACK) {
        if (eventDriver->callback) {
            eventDriver->callback(eventDriver->userData, event);
        }
        return;
    }

    if (mode == PAL_DISPATCH_POLL) {
        eventDriver->queue->push(eventDriver->queue, event);
    }
}

bool _PCALL palPollEvent(
    PalEventDriver* eventDriver,
    PalEvent* outEvent) {

    if (!eventDriver || !outEvent) {
        return false;
    }

    return eventDriver->queue->poll(
        eventDriver->queue,
        outEvent
    );
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

static void palDefaultPush(
    void* queue,
    PalEvent* event) {

    PalEventQueue* eventQueue = queue;
    QueueData* data = eventQueue->userData;
    data->data[data->tail++ % PAL_MAX_EVENTS] = *event;
}

static bool palDefaultPoll(
    void* queue, 
    PalEvent* outEvent) {

    PalEventQueue* eventQueue = queue;
    QueueData* data = eventQueue->userData;
    if (data->head == data->tail) {
        return false;
    }

    *outEvent = data->data[data->head++ % PAL_MAX_EVENTS];    
    return true;
}