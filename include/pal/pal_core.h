
#ifndef _PAL_CORE_H
#define _PAL_CORE_H

#include <stdint.h>

// Set up shared library dependencies
#ifdef _WIN32
#define PAL_CALL __stdcall
#ifdef _PAL_EXPORT
#define PAL_API __declspec(dllexport)
#else 
#define PAL_API __declspec(dllimport)
#endif // PAL_EXPORT
#else
// other platforms
#define PAL_CALL
#ifdef PAL_EXPORT
#define PAL_API extern "C" __attribute__((visibility("default")))
#else 
#define PAL_API
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
typedef intptr_t IntPtr;

typedef uint8_t Uint8;
typedef uint16_t Uint16;
typedef uint32_t Uint32;
typedef uint64_t Uint64;
typedef uintptr_t UintPtr;

typedef void* (PAL_CALL *PalAllocateFn)(
    void* userData, 
    Uint64 size, 
    Uint64 alignment);

typedef void (PAL_CALL *PalFreeFn)(
    void* userData, 
    void* ptr);

typedef void (PAL_CALL *PalLogCallback)(
    void* userData, 
    const char* msg);

typedef enum {
    PAL_RESULT_SUCCESS,
    PAL_RESULT_NULL_POINTER,
    PAL_RESULT_INVALID_PARAMETER,
    PAL_RESULT_OUT_OF_MEMORY,
    PAL_RESULT_PLATFORM_FAILURE,
    PAL_RESULT_INVALID_ALLOCATOR,
    PAL_RESULT_ACCESS_DENIED,
    PAL_RESULT_TIMEOUT,
    PAL_RESULT_INSUFFICIENT_BUFFER,

    PAL_RESULT_INVALID_THREAD,
    PAL_RESULT_THREAD_FEATURE_NOT_SUPPORTED,

    PAL_RESULT_VIDEO_NOT_INITIALIZED,
    PAL_RESULT_INVALID_DISPLAY,
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

PAL_API PalVersion PAL_CALL palGetVersion();
PAL_API const char* PAL_CALL palGetVersionString();

PAL_API const char* PAL_CALL palFormatResult(
    PalResult result);

PAL_API void* PAL_CALL palAllocate(
    const PalAllocator* allocator,
    Uint64 size,
    Uint64 alignment);

PAL_API void PAL_CALL palFree(
    const PalAllocator* allocator, 
    void* ptr);

PAL_API void PAL_CALL palLog(
    const PalLogger* logger, 
    const char* fmt, ...);

PAL_API Uint64 PAL_CALL palGetPerformanceCounter();
PAL_API Uint64 PAL_CALL palGetPerformanceFrequency();

#endif // _PAL_CORE_H