
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

#define PAL_PLATFORM_NAME_SIZE 32
#define PAL_CPU_VENDOR_NAME_SIZE 16
#define PAL_CPU_MODEL_NAME_SIZE 64
#define PAL_BIT(x) 1 << x

typedef int8_t Int8;
typedef int16_t Int16;
typedef int32_t Int32;
typedef int64_t Int64;

typedef uint8_t Uint8;
typedef uint16_t Uint16;
typedef uint32_t Uint32;
typedef uint64_t Uint64;

typedef enum {
    PAL_RESULT_SUCCESS,
    PAL_RESULT_NULL_POINTER,
    PAL_RESULT_INVALID_ARGUMENT,
    PAL_RESULT_OUT_OF_MEMORY,
    PAL_RESULT_PLATFORM_FAILURE
} PalResult;

typedef enum {
    PAL_CPU_ARCH_UNKNOWN,
    PAL_CPU_ARCH_X86,
    PAL_CPU_ARCH_X86_64,
    PAL_CPU_ARCH_ARM,
    PAL_CPU_ARCH_ARM64
} PalCpuArch;

typedef enum {
    PAL_CPU_FEATURE_SSE = PAL_BIT(0),
    PAL_CPU_FEATURE_SSE2 = PAL_BIT(1),
    PAL_CPU_FEATURE_SSE3 = PAL_BIT(2),
    PAL_CPU_FEATURE_SSSE3 = PAL_BIT(3),
    PAL_CPU_FEATURE_SSE41 = PAL_BIT(4),
    PAL_CPU_FEATURE_SSE42 = PAL_BIT(5),
    PAL_CPU_FEATURE_AVX = PAL_BIT(6),
    PAL_CPU_FEATURE_AVX2 = PAL_BIT(7),
    PAL_CPU_FEATURE_AVX512F = PAL_BIT(8),
    PAL_CPU_FEATURE_FMA3 = PAL_BIT(9),
    PAL_CPU_FEATURE_BMI1 = PAL_BIT(10),
    PAL_CPU_FEATURE_BMI2 = PAL_BIT(11),
    PAL_CPU_FEATURE_POPCNT = PAL_BIT(12),
    PAL_CPU_FEATURE_LZCNT = PAL_BIT(13),
} PalCpuFeatures;

typedef enum {
    PAL_PLATFORM_WINDOWS,
    PAL_PLATFORM_LINUX,
    PAL_PLATFORM_MACOS,
    PAL_PLATFORM_ANDROID,
    PAL_PLATFORM_IOS
} PalPlatformType;

typedef enum {
    PAL_PLATFORM_API_WIN32,
    PAL_PLATFORM_API_WAYLAND,
    PAL_PLATFORM_API_X11
} PalPlatformApiType;

typedef struct {
    Uint16 major;
    Uint16 minor;
    Uint16 build;
} PalVersion;

typedef struct {
    Uint32 eax;
    Uint32 ebx;
    Uint32 ecx;
    Uint32 edx;
} PalCpuid;

typedef struct {
    PalPlatformType type;
    PalPlatformApiType apiType;
    PalVersion version;
    char name[PAL_PLATFORM_NAME_SIZE];
} PalPlatformInfo;

typedef struct {
    Uint32 numCores;
    Uint32 cacheKbL1;
    Uint32 cacheKbL2;
    Uint32 cacheKbL3;
    Uint32 numThreads;
    PalCpuArch architecture;
    PalCpuFeatures features;
    char vendor[PAL_CPU_VENDOR_NAME_SIZE];
    char model[PAL_CPU_MODEL_NAME_SIZE];
} PalCpuInfo;

_PAPI PalResult _PAPI palGetPlatformInfo(PalPlatformInfo *info);
_PAPI PalResult _PAPI palGetCpuInfo(PalCpuInfo *info);

_PAPI bool _PAPI palGetCpuid(
    Uint32 leaf, 
    Uint32 subLeaf, 
    PalCpuid *cpuid);

#endif // _PAL_CORE_H