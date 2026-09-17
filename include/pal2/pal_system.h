/**
 * @file pal_system.h
 * @brief This is the header file for PAL System API.
 *
 * It defines all the types and functions of the system module.
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
 * @defgroup pal_system System Module
 * @{
 */

#ifndef PAL_SYSTEM_H
#define PAL_SYSTEM_H

#include "pal_core.h"

#define PAL_PLATFORM_NAME_SIZE 32
#define PAL_CPU_VENDOR_NAME_SIZE 16
#define PAL_CPU_MODEL_NAME_SIZE 64

/**
 * @defgroup cpu_architectures CPU Architectures
 * @brief CPU Architectures.
 * 
 * @{
 */
#define PAL_CPU_ARCH_UNKNOWN 0
#define PAL_CPU_ARCH_X86 1
#define PAL_CPU_ARCH_X86_64 2
#define PAL_CPU_ARCH_ARM 3
#define PAL_CPU_ARCH_ARM64 4
#define PAL_CPU_ARCH_COUNT 5
/** @} */

/**
 * @defgroup cpu_features CPU Features(Instruction Sets)
 * @brief CPU features (instruction sets).
 * 
 * @{
 */
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
/** @} */

/**
 * @defgroup platform_types Platform Types
 * @brief Platform types.
 * 
 * @{
 */
#define PAL_PLATFORM_TYPE_WINDOWS 0
#define PAL_PLATFORM_TYPE_LINUX 1
#define PAL_PLATFORM_TYPE_MACOS 2
#define PAL_PLATFORM_TYPE_ANDROID 3
#define PAL_PLATFORM_TYPE_IOS 4
#define PAL_PLATFORM_TYPE_COUNT 5
/** @} */

/**
 * @defgroup platform_api_types Platform API Types
 * @brief Platform API types.
 * 
 * @{
 */
#define PAL_PLATFORM_API_TYPE_WIN32 0
#define PAL_PLATFORM_API_TYPE_WAYLAND 1
#define PAL_PLATFORM_API_TYPE_X11 2
#define PAL_PLATFORM_API_TYPE_COCOA 3
#define PAL_PLATFORM_API_TYPE_ANDRIOD 4
#define PAL_PLATFORM_API_TYPE_UIKIT 5
#define PAL_PLATFORM_API_TYPE_HEADLESS 6
#define PAL_PLATFORM_API_TYPE_COUNT 7
/** @} */

/**
 * @typedef PalCpuArch
 * @brief CPU achitecture.
 * 
 * All values of this type follow the format `PAL_CPU_ARCH_*` for API
 * consistency and ease of use. 
 *
 * @since Added in version 2.0
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
