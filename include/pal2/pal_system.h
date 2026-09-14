/**
 * @file pal_system.h
 * @brief This is the header file for PAL System API.
 *
 * It defines all the types and functions of the system API.
 *
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @defgroup pal_system System
 */

/** @{ */

#ifndef PAL_SYSTEM_H
#define PAL_SYSTEM_H

#include "pal_core.h"

/** The maximum platform name size.*/
#define PAL_PLATFORM_NAME_SIZE 32

/** The maximum CPU vendor name size.*/
#define PAL_CPU_VENDOR_NAME_SIZE 16

/** The maximum CPU model name size.*/
#define PAL_CPU_MODEL_NAME_SIZE 64

#define PAL_CPU_ARCH_UNKNOWN 0
#define PAL_CPU_ARCH_X86 1
#define PAL_CPU_ARCH_X86_64 2
#define PAL_CPU_ARCH_ARM 3
#define PAL_CPU_ARCH_ARM64 4
#define PAL_CPU_ARCH_COUNT 5

#define PAL_CPU_FEATURE_SSE (1ULL << 0)
#define PAL_CPU_FEATURE_SSE2 (1ULL << 1)
#define PAL_CPU_FEATURE_SSE3 (1ULL << 2)
#define PAL_CPU_FEATURE_SSSE3 (1ULL << 3)
#define PAL_CPU_FEATURE_SSE41 (1ULL << 4)
#define PAL_CPU_FEATURE_SSE42 (1ULL << 5)
#define PAL_CPU_FEATURE_AVX (1ULL << 6)
#define PAL_CPU_FEATURE_AVX2 (1ULL << 7)
#define PAL_CPU_FEATURE_AVX512F (1ULL << 8)
#define PAL_CPU_FEATURE_FMA3 (1ULL << 9)
#define PAL_CPU_FEATURE_BMI1 (1ULL << 10)
#define PAL_CPU_FEATURE_BMI2 (1ULL << 11)

#define PAL_PLATFORM_TYPE_WINDOWS 0
#define PAL_PLATFORM_TYPE_LINUX 1
#define PAL_PLATFORM_TYPE_MACOS 2
#define PAL_PLATFORM_TYPE_ANDROID 3
#define PAL_PLATFORM_TYPE_IOS 4
#define PAL_PLATFORM_TYPE_COUNT 5

#define PAL_PLATFORM_API_TYPE_WIN32 0
#define PAL_PLATFORM_API_TYPE_WAYLAND 1
#define PAL_PLATFORM_API_TYPE_X11 2
#define PAL_PLATFORM_API_TYPE_COCOA 3
#define PAL_PLATFORM_API_TYPE_ANDRIOD 4
#define PAL_PLATFORM_API_TYPE_UIKIT 5
#define PAL_PLATFORM_API_TYPE_HEADLESS 6
#define PAL_PLATFORM_API_TYPE_COUNT 7

/**
 * @typedef PalCpuArch
 * @brief CPU achitecture.
 * 
 * All values of this type follow the format `PAL_CPU_ARCH_*` for API
 * consistency and ease of use. 
 *
 * @since Added in version 2.0
 * 
 * @def PAL_CPU_ARCH_UNKNOWN
 * The CPU achitecture is not known to PAL.
 * 
 * @def PAL_CPU_ARCH_X86
 * The CPU achitecture is x86.
 *
 * @def PAL_CPU_ARCH_X86_64
 * The CPU achitecture is x64.
 *
 * @def PAL_CPU_ARCH_ARM
 * The CPU achitecture is ARM.
 *
 * @def PAL_CPU_ARCH_ARM64
 * The CPU achitecture is ARM64.
 *
 * @def PAL_CPU_ARCH_COUNT
 * The number of CPU architectures. The literal value must not be used.
 */
typedef uint32_t PalCpuArch;

/**
 * @typedef PalCpuFeatures
 * @brief CPU features (instruction sets).
 * 
 * This is a bitmask of all supported instruction sets of the CPU. This
 * only covers the instruction sets known to PAL, therefore the CPU
 * might support more.
 * 
 * All values of this type follow the format `PAL_CPU_FEATURE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * 
 * @def PAL_CPU_FEATURE_SSE
 * The CPU supports SSE instruction set.
 * 
 * @def PAL_CPU_FEATURE_SSE2
 * The CPU supports SSE2 instruction set.
 * 
 * @def PAL_CPU_FEATURE_SSE3
 * The CPU supports SSE3 instruction set.
 * 
 * @def PAL_CPU_FEATURE_SSSE3
 * The CPU supports SSSE3 instruction set.
 * 
 * @def PAL_CPU_FEATURE_SSE41
 * The CPU supports SSE4.1 instruction set.
 * 
 * @def PAL_CPU_FEATURE_SSE42
 * The CPU supports SSE4.2 instruction set.
 * 
 * @def PAL_CPU_FEATURE_AVX
 * The CPU supports AVX instruction set.
 * 
 * @def PAL_CPU_FEATURE_AVX2
 * The CPU supports AVX2 instruction set.
 * 
 * @def PAL_CPU_FEATURE_AVX512F
 * The CPU supports AVX512F instruction set.
 * 
 * @def PAL_CPU_FEATURE_FMA3
 * The CPU supports FMA3 instruction set.
 * 
 * @def PAL_CPU_FEATURE_BMI1
 * The CPU supports BMI1 instruction set.
 * 
 * @def PAL_CPU_FEATURE_BMI2
 * The CPU supports BMI2 instruction set.
 */
typedef uint64_t PalCpuFeatures;

/**
 * @typedef PalPlatformType
 * @brief Platform types.
 *
 * This is the family name (eg. This does not show if its Windows 7 or 
 * Windows 8 etc).
 * 
 * All values of this type follow the format `PAL_PLATFORM_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * 
 * @def PAL_PLATFORM_TYPE_WINDOWS
 * Windows platform.
 * 
 * @def PAL_PLATFORM_TYPE_LINUX
 * Linux platform.
 * 
 * @def PAL_PLATFORM_TYPE_MACOS
 * MacOS platform.
 * 
 * @def PAL_PLATFORM_TYPE_ANDROID
 * Android platform.
 * 
 * @def PAL_PLATFORM_TYPE_IOS
 * IOS platform.
 * 
 * @def PAL_PLATFORM_TYPE_COUNT
 * The number of platform types. The literal value must not be used.
 */
typedef uint32_t PalPlatformType;

/**
 * @typedef PalPlatformApiType
 * @brief Platform API types.
 *
 * This is the API the playform uses. Most platforms support only one (eg.
 * Windows).
 * 
 * All values of this type follow the format `PAL_PLATFORM_API_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * 
 * @def PAL_PLATFORM_API_TYPE_WIN32
 * Win32 platform API type.
 * 
 * @def PAL_PLATFORM_API_TYPE_WAYLAND
 * Wayland platform API type.
 * 
 * @def PAL_PLATFORM_API_TYPE_X11
 * Xlib platform API type.
 * 
 * @def PAL_PLATFORM_API_TYPE_COCOA
 * Cocoa platform API type.
 * 
 * @def PAL_PLATFORM_API_TYPE_ANDRIOD
 * Andriod platform API type.
 * 
 * @def PAL_PLATFORM_API_TYPE_UIKIT
 * UIKIT platform API type.
 * 
 * @def PAL_PLATFORM_API_TYPE_HEADLESS
 * Headless platform API type.
 * 
 * @def PAL_PLATFORM_API_TYPE_COUNT
 * The number of platform API types. The literal value must not be used.
 */
typedef uint32_t PalPlatformApiType;

/**
 * @struct PalPlatformInfo
 * @brief Contains information about a platform (OS).
 *
 * @since Added in version 2.0
 * 
 * @var PalPlatformInfo::type
 * The platform type (eg. `PAL_PLATFORM_TYPE_WINDOWS`).
 * 
 * @var PalPlatformInfo::apiType
 * The platform API type (eg. `PAL_PLATFORM_API_TYPE_WIN32`).
 * 
 * @var PalPlatformInfo::totalMemory
 * Total Disk space in GB. 
 * On `Windows`: the size is from C drive only. @nl
 * On `Linux`: the size is from root only.
 * 
 * @var PalPlatformInfo::totalRAM
 * Total CPU memory in MB.
 * 
 * @var PalPlatformInfo::version
 * The platform version.
 * 
 * @var PalPlatformInfo::name
 * The full platform name with contains the family name and version.
 * (eg. Windows 11.22000)
 */
typedef struct PalPlatformInfo
{
    PalPlatformType type;
    PalPlatformApiType apiType;
    uint32_t totalMemory;
    uint32_t totalRAM;
    PalVersion version;
    char name[PAL_PLATFORM_NAME_SIZE];
} PalPlatformInfo;

/**
 * @struct PalCPUInfo
 * @brief Contains information about a CPU.
 *
 * @since Added in version 2.0
 * 
 * @var PalCPUInfo::features
 * A bitmask of supported CPU features (instructions).
 * 
 * @var PalCPUInfo::architecture
 * The CPU architecture (eg. `PAL_CPU_ARCH_X86_64`).
 * 
 * @var PalCPUInfo::numCores
 * The total number of CPU cores.
 * 
 * @var PalCPUInfo::cacheL1
 * The L1 cache in KB.
 * 
 * @var PalCPUInfo::cacheL2
 * The L2 cache in KB.
 * 
 * @var PalCPUInfo::cacheL3
 * The L3 cache in KB.
 * 
 * @var PalCPUInfo::numLogicalProcessors
 * The total number of CPUs.
 * 
 * @var PalCPUInfo::vendor
 * The CPU vendor name.
 * 
 * @var PalCPUInfo::model
 * The CPU model name.
 */
typedef struct PalCPUInfo
{
    PalCpuFeatures features;
    PalCpuArch architecture;
    uint32_t numCores;
    uint32_t cacheL1;
    uint32_t cacheL2;
    uint32_t cacheL3;
    uint32_t numLogicalProcessors;
    char vendor[PAL_CPU_VENDOR_NAME_SIZE];
    char model[PAL_CPU_MODEL_NAME_SIZE];
} PalCPUInfo;

/**
 * @brief Gets the platform (OS) information.
 *
 * @param[out] info The platform information.
 *
 * @Thread-safety `info` must be per thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetPlatformInfo(PalPlatformInfo* info);

/**
 * @brief Gets the CPU information.
 *
 * @param[in] allocator The allocator to use. Set to `nullptr` to use the
 * thread-safe default allocator.
 * @param[out] info The CPU information.
 *
 * @Thread-safety `info` must be per thread and `allocator` must be 
 * thread-safe.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetCPUInfo(
    const PalAllocator* allocator,
    PalCPUInfo* info);

/** @} */

#endif // PAL_SYSTEM_H
