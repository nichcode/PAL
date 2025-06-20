
#pragma once

// using from Cpp
#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#ifdef _WIN32
    #define PAL_CALL __stdcall
    #ifdef PAL_EXPORT
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

// types

typedef unsigned int Uint32;
typedef unsigned long long Uint64;

typedef enum PAL_Result
{
    PAL_RESULT_OK

} PAL_Result;