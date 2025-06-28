

#ifndef _PAL_CORE_H
 #define _PAL_CORE_H

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
typedef int PalTLSID;
typedef Uint32 PalWindowID;

typedef void* (*PalAllocFn)(Uint64 size);
typedef void* (*PalAlignedAllocFn)(Uint64 size, Uint64 alignment);
typedef void (*PalFreeFn)(void* memory);
typedef void (*PalAlignedFreeFn)(void* memory);

typedef enum PalError
{
    PAL_ERROR_NONE = 0,
    PAL_NULL_POINTER,
    PAL_INVALID_ALLOCATOR,
    PAL_VIDEO_NOT_INITIALIZED,
    PAL_EVENT_NOT_INITIALIZED,
    PAL_NULL_VIDEO_DRIVER,
    PAL_OUT_OF_MEMORY,
    PAL_PLATFORM_ERROR,
    PAL_DISPLAY_ERROR,
    PAL_INVALID_ARG,
    PAL_INVALID_EVENT

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
    PalAlignedAllocFn alignedAlloc;
    PalFreeFn free;
    PalAlignedFreeFn alignedFree;
    void* userData;

} PalAllocator;

_PAPI void _PCALL palSetError(PalError error);
_PAPI PalError _PCALL palGetError();

// this is thread safe
_PAPI PalVersion _PCALL palGetVersion();

// this is thread safe
_PAPI const char* _PCALL palFormatError(PalError error);

// this is thread safe
_PAPI const char* _PCALL palGetPlatformString();

_PAPI PalTLSID _PCALL palCreateTLS();
_PAPI bool _PCALL palDestroyTLS(PalTLSID id);
_PAPI void* _PCALL palGetTLS(PalTLSID id);
_PAPI bool _PCALL palSetTLS(PalTLSID id, void* data);

_PAPI void* _PCALL palAllocate(Uint64 size);
_PAPI void* _PCALL palAlignAllocate(Uint64 size, Uint64 alignment);
_PAPI void _PCALL palFree(void* memory);
_PAPI void _PCALL palAlignFree(void* memory);

_PAPI void _PCALL palCopyString(char* dest, const char* src);

_PAPI void _PCALL palSetMemory(void* memory, int value, Uint64 size);
_PAPI void _PCALL palCopyMemory(void* dest, void* src, Uint64 size);
_PAPI void _PCALL palLog(PalLogLevel level, const char* fmt, ...);

#define palLogTrace(...)    palLog(PAL_LOG_LEVEL_TRACE, __VA_ARGS__);
#define palLogInfo(...)     palLog(PAL_LOG_LEVEL_INFO, __VA_ARGS__);
#define palLogWarn(...)     palLog(PAL_LOG_LEVEL_WARN, __VA_ARGS__);
#define palLogError(...)    palLog(PAL_LOG_LEVEL_ERROR, __VA_ARGS__);

static void _PCALL palZeroMemory(void* memory, Uint64 size)
{
    palSetMemory(memory, 0, size);
}

#endif // _PAL_CORE_H