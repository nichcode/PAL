
#ifndef _PAL_CORE_H
#define _PAL_CORE_H

#include <stdint.h>

// Set up shared library dependencies
#ifdef _WIN32
#define _PCALL __stdcall
#ifdef _PAL_EXPORT
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

// Set up typedefs for C
#ifndef __cplusplus
#define nullptr ((void *)0)
#define true 1
#define false 0

typedef _Bool bool;
#endif // __cplusplus

#define PAL_BIT(x) 1 << x

typedef int8_t Int8;
typedef int16_t Int16;
typedef int32_t Int32;
typedef int64_t Int64;

typedef uint8_t Uint8;
typedef uint16_t Uint16;
typedef uint32_t Uint32;
typedef uint64_t Uint64;
typedef uintptr_t UintPtr;

typedef void* (*PalAllocateFn)(void*, Uint64, Uint64);
typedef void (*PalFreeFn)(void*, void*);
typedef void (*PalLogCallback)(void*, const char*);

typedef enum {
    PAL_RESULT_SUCCESS,
    PAL_RESULT_NULL_POINTER,
    PAL_RESULT_INVALID_PARAMETER,
    PAL_RESULT_OUT_OF_MEMORY,
    PAL_RESULT_PLATFORM_FAILURE,
    PAL_RESULT_INVALID_ALLOCATOR,
    PAL_RESULT_ACCESS_DENIED,
    PAL_RESULT_TIMEOUT,

    PAL_RESULT_INVALID_THREAD,
    PAL_RESULT_THREAD_FEATURE_NOT_SUPPORTED
} PalResult;

typedef struct {
    Uint16 major;
    Uint16 minor;
    Uint16 build;
} PalVersion;

typedef struct {
    PalAllocateFn allocate;
    PalFreeFn free;
    void* userData;
} PalAllocator;

typedef struct {
    PalLogCallback callback;
    void* userData;
} PalLogger;

_PAPI PalVersion _PCALL palGetVersion();
_PAPI const char* _PCALL palGetVersionString();

_PAPI const char* _PCALL palFormatResult(PalResult result);

_PAPI void* _PCALL palAllocate(
    const PalAllocator* allocator,
    Uint64 size,
    Uint64 alignment);

_PAPI void _PCALL palFree(const PalAllocator* allocator, void* ptr);

_PAPI void _PCALL palLog(const PalLogger* logger, const char* fmt, ...);

_PAPI Uint64 _PCALL palGetPerformanceCounter();
_PAPI Uint64 _PCALL palGetPerformanceFrequency();

#endif // _PAL_CORE_H