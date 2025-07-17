
#ifndef _PAL_CORE_H
#define _PAL_CORE_H

#include <stdarg.h>

// using from Cpp
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifdef _WIN32
#define _PCALL __stdcall
#ifdef PAL_EXPORT
#define _PAPI __declspec(dllexport)
#else 
#define _PAPI __declspec(dllimport)
#endif // PAL_EXPORT
#else
    // other platforms
#define _PCALL
#ifdef PAL_EXPORT
#define _PAPI extern "C" __attribute__((visibility("default")))
#else 
#define _PAPI
#endif // PAL_EXPORT
#endif // _WIN32

#ifdef __cplusplus
}
#endif // __cplusplus

#ifdef __cplusplus
#define PAL_NULL nullptr
#define PAL_TRUE true
#define PAL_FALSE false
#else
#define PAL_NULL ((void *)0)
#define PAL_TRUE 1
#define PAL_FALSE 0
 typedef _Bool bool;
#endif // __cplusplus

#define PAL_BIT(x) 1 << x
#define PAL_DECLARE_HANDLE(name) typedef struct name name;

typedef unsigned char Uint8;
typedef unsigned short Uint16;
typedef unsigned int Uint32;
typedef unsigned long long Uint64;
typedef long long Int64;

typedef int PalTlsID;
typedef enum PalLogLevel PalLogLevel;

typedef void* (*PalAllocFn)(void*, Uint64);
typedef void (*PalFreeFn)(void*, void*);
typedef void (*PalLogCallbackFn)(void*, PalLogLevel, const char*);

typedef enum PalLogLevel {
    PAL_LOG_LEVEL_TRACE,
    PAL_LOG_LEVEL_INFO,
    PAL_LOG_LEVEL_WARN,
    PAL_LOG_LEVEL_ERROR
} PalLogLevel;

typedef enum PalResult {
    PAL_SUCCESS,
    PAL_ERROR_NULL_POINTER,
    PAL_ERROR_INVALID_ARGUMENT,
    PAL_ERROR_INVALID_ALLOCATOR,
    PAL_ERROR_OUT_OF_MEMORY,
    PAL_ERROR_UNSUPPORTED,
    PAL_ERROR_RESOURCE_IN_USE,
    PAL_ERROR_TIMEOUT,
    PAL_ERROR_INSUFFICIENT_BUFFER
} PalResult;

typedef struct PalVersion {
    int major;
    int minor;
    int patch;
} PalVersion;

typedef struct PalAllocator {
    PalAllocFn alloc;
    PalFreeFn free;
    void* userData;
} PalAllocator;

typedef struct PalLogger {
    PalLogCallbackFn callback;
    void* userData;
} PalLogger;

_PAPI PalVersion _PCALL palGetVersion();
_PAPI const char* _PCALL palGetVersionString();
_PAPI const char* _PCALL palResultToString(PalResult result);

_PAPI void* _PCALL palAllocate(PalAllocator* allocator, Uint64 size);
_PAPI void _PCALL palFree(PalAllocator* allocator, void* memory);

_PAPI void _PCALL palSetMemory(void* memory, int value, Uint64 size);
_PAPI void _PCALL palZeroMemory(void* memory, Uint64 size);
_PAPI void _PCALL palCopyMemory(void* dest, const void* src, Uint64 size);
_PAPI void _PCALL palMoveMemory(void* dest, const void* src, Uint64 size);

_PAPI PalTlsID _PCALL palCreateTls();
_PAPI void _PCALL palDestroyTls(PalTlsID id);
_PAPI void* _PCALL palGetTls(PalTlsID id);
_PAPI void _PCALL palSetTls(PalTlsID id, void* data);

_PAPI bool _PCALL palFormatV(
    const char* fmt, 
    va_list argsList, 
    Uint64* size, 
    char* buffer);

_PAPI bool _PCALL palFormat(
    char* buffer, 
    Uint64* size, 
    const char* fmt, ...);

_PAPI void _PCALL palLog(
    PalLogger* logger, 
    PalLogLevel level, 
    const char* fmt, ...);

_PAPI void _PCALL palLogTrace(
    PalLogger* logger,  
    const char* fmt, ...);

_PAPI void _PCALL palLogInfo(
    PalLogger* logger,  
    const char* fmt, ...);

_PAPI void _PCALL palLogWarn(
    PalLogger* logger,  
    const char* fmt, ...);

_PAPI void _PCALL palLogError(
    PalLogger* logger,  
    const char* fmt, ...);

#endif // _PAL_CORE_H