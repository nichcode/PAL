
#ifndef _PAL_CORE_H
#define _PAL_CORE_H

#include "pal_defines.h"

#include <stdarg.h>

typedef int PalTLSID;

typedef void* (*PalAllocFn)(void*, Uint64);
typedef void (*PalFreeFn)(void*, void*);

typedef enum PalLogLevel PalLogLevel;
typedef void (*PalLogCallbackFn)(void*, PalLogLevel, const char*);

typedef enum PalError
{
    PAL_ERROR_NONE,
    PAL_ERROR_NULL_POINTER,
    PAL_ERROR_INVALID_ARGUMENT,
    PAL_ERROR_INVALID_ALLOCATOR,
    PAL_ERROR_ALLOCATION_FAILED,
} PalError;

typedef enum PalLogLevel
{
    PAL_LOG_LEVEL_TRACE,
    PAL_LOG_LEVEL_INFO,
    PAL_LOG_LEVEL_WARN,
    PAL_LOG_LEVEL_ERROR
} PalLogLevel;

typedef struct PalVersion
{
    int major;
    int minor;
    int patch;
} PalVersion;

typedef struct PalAllocator
{
    PalAllocFn alloc;
    PalFreeFn free;
    void* userData;

} PalAllocator;

typedef struct PalLogger
{
    PalLogCallbackFn callback;
    void* userData;
} PalLogger;

_PAPI void _PCALL palGetVerion(PalVersion* version);
_PAPI Uint32 _PCALL palGetVerionInt();

_PAPI void* _PCALL palAlloc(PalAllocator* allocator, Uint64 size);
_PAPI void _PCALL palFree(PalAllocator* allocator, void* ptr);

_PAPI PalTLSID _PCALL palCreateTLS();
_PAPI void _PCALL palDestroyTLS(PalTLSID id);
_PAPI void* _PCALL palGetTLS(PalTLSID id);
_PAPI void _PCALL palSetTLS(PalTLSID id, void* data);

_PAPI void _PCALL palSetError(PalError error);
_PAPI PalError _PCALL palGetError();

_PAPI void _PCALL palFormatArgs(const char* fmt, va_list argsList, char* buffer);
_PAPI void _PCALL palFormat(char* buffer, const char* fmt, ...);

_PAPI void _PCALL palLog(PalLogger* logger, PalLogLevel level, const char* fmt, ...);
_PAPI void _PCALL palLogTrace(PalLogger* logger, const char* fmt, ...);
_PAPI void _PCALL palLogInfo(PalLogger* logger, const char* fmt, ...);
_PAPI void _PCALL palLogWarn(PalLogger* logger, const char* fmt, ...);
_PAPI void _PCALL palLogError(PalLogger* logger, const char* fmt, ...);

_PAPI void _PCALL palLogConsole(PalLogLevel level, const char* fmt, ...);
_PAPI void _PCALL palLogConsoleTrace(const char* fmt, ...);
_PAPI void _PCALL palLogConsoleInfo(const char* fmt, ...);
_PAPI void _PCALL palLogConsoleWarn(const char* fmt, ...);
_PAPI void _PCALL palLogConsoleError(const char* fmt, ...);

#endif // _PAL_CORE_H