
#ifndef _PAL_SYSTEM_H
#define _PAL_SYSTEM_H

#include "pal_core.h"

#define PAL_PLATFORM_NAME_SIZE 32
#define PAL_CPU_VENDOR_NAME_SIZE 16
#define PAL_CPU_MODEL_NAME_SIZE 64

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
    PAL_CPU_FEATURE_BMI2 = PAL_BIT(11)
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
    PalPlatformType type;
    PalPlatformApiType apiType;
    Uint32 totalMemory;
    Uint32 totalRAM;
    PalVersion version;
    char name[PAL_PLATFORM_NAME_SIZE];
} PalPlatformInfo;

typedef struct {
    Uint32 numCores;
    Uint32 cache1;
    Uint32 cache2;
    Uint32 cache3;
    Uint32 numLogicalProcessors;
    PalCpuArch architecture;
    PalCpuFeatures features;
    char vendor[PAL_CPU_VENDOR_NAME_SIZE];
    char model[PAL_CPU_MODEL_NAME_SIZE];
} PalCPUInfo;

PAL_API PalResult PAL_API palGetPlatformInfo(
    PalPlatformInfo *info);

PAL_API PalResult PAL_API palGetCPUInfo(
    const PalAllocator* allocator, 
    PalCPUInfo *info);

#endif // _PAL_SYSTEM_H