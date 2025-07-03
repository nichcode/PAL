
#ifndef _PAL_CORE_H
 #define _PAL_CORE_H

#include "pal_defines.h"

// ==================================================
// Version Info
// ==================================================

typedef struct PalVersion
{
    int major;
    int minor;
    int patch;

} PalVersion;

_PAPI PalVersion _PCALL palGetVersion();

// ==================================================
// Memory and Allocator
// ==================================================

typedef void* (*PalAllocFn)(Uint64);
typedef void (*PalFreeFn)(void*);

typedef struct PalAllocator
{
    PalAllocFn alloc;
    PalFreeFn free;
    void* userData;

} PalAllocator;

_PAPI void* _PCALL palAlloc(Uint64 size);
_PAPI void _PCALL palFree(void* memory);

_PAPI void _PCALL palSetMemory(void* memory, int value, Uint64 size);
_PAPI void _PCALL palCopyMemory(void* dest, void* src, Uint64 size);

#define palZeroMemory(memory, size)    palSetMemory(memory, 0, size);

// ==================================================
// Thread Local Storage
// ==================================================

typedef int PalTLSID;

_PAPI PalTLSID _PCALL palCreateTLS();
_PAPI bool _PCALL palDestroyTLS(PalTLSID id);
_PAPI void* _PCALL palGetTLS(PalTLSID id);
_PAPI bool _PCALL palSetTLS(PalTLSID id, void* data);

// ==================================================
// Error
// ==================================================

typedef enum PalError
{
    PAL_ERROR_NONE,
    PAL_NULL_POINTER,
    PAL_INVALID_ALLOCATOR,
    PAL_VIDEO_NOT_INITIALIZED,
    PAL_OUT_OF_MEMORY,
    PAL_DISPLAY_ERROR,

} PalError;

_PAPI void _PCALL palSetError(PalError error);
_PAPI PalError _PCALL palGetError();
_PAPI const char* _PCALL palFormatError(PalError error);

// ==================================================
// Log
// ==================================================

typedef enum PalLogLevel
{
    PAL_LOG_LEVEL_TRACE,
    PAL_LOG_LEVEL_INFO,
    PAL_LOG_LEVEL_WARN,
    PAL_LOG_LEVEL_ERROR

} PalLogLevel;

typedef void (*PalLogCallbackFn)(PalLogLevel, const char*);

_PAPI void _PCALL palLog(PalLogLevel level, const char* fmt, ...);
_PAPI void _PCALL palLogConsole(PalLogLevel level, const char* fmt, ...);

_PAPI void _PCALL palSetLogCallback(PalLogCallbackFn callback);
_PAPI PalLogCallbackFn _PCALL palGetLogCallback();
_PAPI const char* _PCALL palGetLastLog();

#define palLogTrace(...)            palLog(PAL_LOG_LEVEL_TRACE, __VA_ARGS__)
#define palLogInfo(...)             palLog(PAL_LOG_LEVEL_INFO, __VA_ARGS__)
#define palLogWarn(...)             palLog(PAL_LOG_LEVEL_WARN, __VA_ARGS__)
#define palLogError(...)            palLog(PAL_LOG_LEVEL_ERROR, __VA_ARGS__)

#define palLogConsoleTrace(...)     palLogConsole(PAL_LOG_LEVEL_TRACE, __VA_ARGS__)
#define palLogConsoleInfo(...)      palLogConsole(PAL_LOG_LEVEL_INFO, __VA_ARGS__)
#define palLogConsoleWarn(...)      palLogConsole(PAL_LOG_LEVEL_WARN, __VA_ARGS__)
#define palLogConsoleError(...)     palLogConsole(PAL_LOG_LEVEL_ERROR, __VA_ARGS__)

#endif // _PAL_CORE_H