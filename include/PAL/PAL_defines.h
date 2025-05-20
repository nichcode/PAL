
#pragma once

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

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

using f32 = float;
using f64 = double;

using b32 = int;
using b8 = bool;

#define PAL_BIT(x) 1 << x
#define PAL_MAX_SLOTS 16

#ifdef PAL_PLATFORM_WINDOWS
#define PAL_EXPORT __declspec(dllexport)
#define PAL_IMPORT __declspec(dllimport)
#define PAL_BREAK __debugbreak();
#else
#define PAL_EXPORT __attribute__((visibility("default")))
#define PAL_IMPORT
#define PAL_BREAK 
#endif // PAL_PLATFORM_WINDOWS

#ifdef PAL_DLL
#define PAL_LIBRARY PAL_EXPORT
#else 
#define PAL_LIBRARY PAL_IMPORT
#endif // PAL_EXPORT

#ifdef __cplusplus
#define PAL_API extern "C" PAL_LIBRARY
#else
#define PAL_API PAL_LIBRARY
#endif // __cplusplus

// static assertions
#if defined(__clang__) || defined(__gcc__)
#define STATIC_ASSERT _Static_assert
#else
#define STATIC_ASSERT static_assert
#endif

STATIC_ASSERT(sizeof(u8) == 1, "Expected u8 to be 1 byte.");
STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 bytes.");
STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 bytes.");
STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 bytes.");
STATIC_ASSERT(sizeof(i8) == 1, "Expected i8 to be 1 byte.");
STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 bytes.");
STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 bytes.");
STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 bytes.");
STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 bytes.");
STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 bytes.");