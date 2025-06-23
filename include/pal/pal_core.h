
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

// compiler specific
#if defined(__clang__) || defined(__gcc__) || defined(__GNUC__)
 #define PAL_INLINE __attribute__((always_inline))
 #define PAL_NO_INLINE __attribute__((noinline))
 #define PAL_DEPRECATED(message) __attribute__((deprecated(message)))
#elif defined(_MSC_VER) // msvc
 #define PAL_INLINE __forceinline
 #define PAL_NO_INLINE __declspec(noinline)
 #define PAL_DEPRECATED(message) __declspec(deprecated(message))
#else // other compilers
 #define PAL_INLINE __forceinline
 #define PAL_NO_INLINE __declspec(noinline)
 #error "Unsupported compiler - don't know how to define deprecations!"
#endif 

#define PAL_BIT(x) 1 << x

typedef unsigned char Uint8;
typedef unsigned short Uint16;
typedef unsigned int Uint32;
typedef unsigned long long Uint64;

// Tiny Local storage thread id
typedef int PalTLSID;

typedef enum PalError
{
    PAL_ERROR_NONE = 0,
    PAL_ERROR_NULL_POINTER,
    PAL_ERROR_SYSTEM_NOT_INITIALIZED,
    PAL_ERROR_INVALID_ALLOCATOR,
    PAL_ERROR_OUT_OF_MEMORY

} PalError;

typedef enum PalLogLevel
{
    PAL_LOG_TRACE,
    PAL_LOG_INFO,
    PAL_LOG_WARN,
    PAL_LOG_ERROR

} PalLogLevel;

typedef void* (*PalAllocFn)(Uint64 size);
typedef void* (*PalAlignedAllocFn)(Uint64 size, Uint64 alignment);
typedef void (*PalFreeFn)(void* memory);
typedef void (*PalAlignedFreeFn)(void* memory);

typedef void (*PalLogCallback)(PalError code, PalLogLevel level, const char* msg);

typedef struct PAlAllocator
{
    PalAllocFn alloc;
    PalAlignedAllocFn alignedAlloc;
    PalFreeFn free;
    PalAlignedFreeFn alignedFree;
    void* userData;

} PAlAllocator;

_PAPI void _PCALL palSetError(PalError error);
_PAPI PalError _PCALL palGetError();

_PAPI PalTLSID _PCALL palCreateTLS();
_PAPI bool _PCALL palDestroyTLS(PalTLSID id);
_PAPI void* _PCALL palGetTLS(PalTLSID id);
_PAPI bool _PCALL palSetTLS(PalTLSID id, void* data, void (*destructor)(void*));

_PAPI void _PCALL palSetLogCallback(PalLogCallback callback);
_PAPI void _PCALL palLog(PalLogLevel level, const char* fmt, ...);
_PAPI bool _PCALL palFormatArgs(const char* fmt, va_list argsList, char* buffer);
_PAPI bool _PCALL palFormat(char* buffer, const char* fmt, ...);

#define palLogTrace(...)    palLog(PAL_LOG_TRACE, __VA_ARGS__);
#define palLogInfo(...)     palLog(PAL_LOG_INFO, __VA_ARGS__);
#define palLogWarn(...)     palLog(PAL_LOG_WARN, __VA_ARGS__);
#define palLogError(...)    palLog(PAL_LOG_ERROR, __VA_ARGS__);

#endif // _PAL_CORE_H