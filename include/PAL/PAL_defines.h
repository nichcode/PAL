
#ifndef PAL_DEFINES_H
#define PAL_DEFINES_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define PAL_PLATFORM_WINDOWS 1
#ifndef _WIN64
#error "64-bit is required on Windows!"
#endif
#elif defined(__linux__) || defined(__gnu_linux__)
// Linux OS
#define PAL_PLATFORM_LINUX 1
#if defined(__ANDROID__)
#define PAL_PLATFORM_ANDROID 1
#endif
#elif defined(__unix__)
// Catch anything not caught by the above.
#define PAL_PLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
// Posix
#define PAL_PLATFORM_POSIX 1
#elif __APPLE__
// Apple platforms
#define PAL_PLATFORM_APPLE 1
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#define PAL_PLATFORM_IOS 1
#define PAL_PLATFORM_IOS_SIMULATOR 1
#elif TARGET_OS_IPHONE
#define PAL_PLATFORM_IOS 1
// iOS device
#elif TARGET_OS_MAC
// Other kinds of Mac OS
#else
#error "Unknown Apple platform"
#endif
#else
#error "Unknown platform!"
#endif

#ifdef PAL_PLATFORM_WINDOWS
#define PAL_EXT __declspec(dllexport)
#define PAL_IMP __declspec(dllimport)
#define PAL_BREAK __debugbreak();
#else
#define PAL_EXT __attribute__((visibility("default")))
#define PAL_IMP
#define PAL_BREAK 
#endif // PAL_PLATFORM_WINDOWS

#ifdef PAL_EXPORT
#define PAL_DECLSPEC PAL_EXT
#else 
#define PAL_DECLSPEC PAL_IMP
#endif // PAL_EXPORT

#ifdef __cplusplus
#define PAL_API extern "C" PAL_DECLSPEC
#define PAL_NULL nullptr
#define PAL_TRUE true
#define PAL_FALSE false
#define PAL_BOOL bool
#else
#define PAL_API PAL_DECLSPEC
#define PAL_NULL ((void *)0)
#define PAL_TRUE 1
#define PAL_FALSE 0
#define PAL_BOOL unsigned char

#endif // __cplusplus

// compiler specific
#if defined(__clang__) || defined(__gcc__) || defined(__GNUC__)  // clang or gcc
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


#endif // PAL_DEFINES_H