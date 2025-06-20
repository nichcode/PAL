
#ifndef PAL_DEFINES_H
    #define PAL_DEFINES_H

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

typedef unsigned int Uint32;
typedef unsigned long long Uint64;

typedef enum PAL_Result
{
    PAL_SUCCESS,
    PAL_PLATFORM_ERROR,
    PAL_PLATFORM_WARN,
    PAL_DEVICE_ERROR,
    PAL_DEVICE_WARN,
    PAL_INVALID_ERROR,
    PAL_OUT_OF_MEMORY,

} PAL_Result;

typedef void* (*PalAllocFn)(Uint64 size);
typedef void (*PalFreeFn)(void* memory);
typedef void (*PalDebugMessageFn)(PAL_Result code, const char* msg);

typedef enum PAL_InitFlag
{
    PAL_NONE = 0,
    PAL_DEBUG_BIT = PAL_BIT(1),

} PAL_InitFlag;

typedef struct PalAllocator
{
    PalAllocFn allocate;
    PalFreeFn free;

} PalAllocator;

typedef struct PalInitInfo
{
    Uint32 flags;
    PalAllocator* allocator;

} PalInitInfo;

#endif // PAL_DEFINES_H