
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

typedef enum PalError
{
    PAL_ERROR_NONE = 0,
    PAL_ERROR_NULL_POINTER,
    PAL_ERROR_SYSTEM_NOT_INITIALIZED,
    PAL_ERROR_INVALID_ALLOCATOR,
    PAL_ERROR_OUT_OF_MEMORY

} PalError;

typedef void* (*PalAllocFn)(Uint64 size);
typedef void* (*PalAlignAllocFn)(Uint64 size, Uint64 alignment);
typedef void (*PalFreeFn)(void* memory);
typedef void (*PalAlignFreeFn)(void* memory);

typedef struct PAlAllocator
{
    PalAllocFn alloc;
    PalAlignAllocFn alignAlloc;
    PalFreeFn free;
    PalFreeFn alignFree;
    void* userData;

} PAlAllocator;

_PAPI void _PCALL palSetError(PalError error);
_PAPI PalError _PCALL palGetError();

#endif // _PAL_CORE_H