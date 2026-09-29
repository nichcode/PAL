/**
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

#ifndef PAL_SYSTEM_H
#define PAL_SYSTEM_H

#include "pal_core.h"

/**
 * @defgroup pal_system System Module
 * @{
 */

#define PAL_CPU_VENDOR_NAME_SIZE 16 /**< maximum CPU vendor name size */
#define PAL_CPU_MODEL_NAME_SIZE 64 /**< maximum CPU model name size */
#define PAL_PLATFORM_NAME_SIZE 32 /**< maximum platform name size */

/**
 * @defgroup cpu_architectures CPU Architectures
 * @{
 */
#define PAL_CPU_ARCH_UNKNOWN 0
#define PAL_CPU_ARCH_X86 1
#define PAL_CPU_ARCH_X86_64 2
#define PAL_CPU_ARCH_ARM 3
#define PAL_CPU_ARCH_ARM64 4
#define PAL_CPU_ARCH_COUNT 5 /**< number of CPU architectures */
/** @} */

/**
 * @defgroup cpu_features CPU Features (Instruction Sets)
 * @{
 */
#define PAL_CPU_FEATURE_SSE (1ULL << 0)
#define PAL_CPU_FEATURE_SSE2 (1ULL << 1)
#define PAL_CPU_FEATURE_SSE3 (1ULL << 2)
#define PAL_CPU_FEATURE_SSSE3 (1ULL << 3)
#define PAL_CPU_FEATURE_SSE41 (1ULL << 4) /**< SSE4.1 */
#define PAL_CPU_FEATURE_SSE42 (1ULL << 5) /**< SSE4.2 */
#define PAL_CPU_FEATURE_AVX (1ULL << 6)
#define PAL_CPU_FEATURE_AVX2 (1ULL << 7)
#define PAL_CPU_FEATURE_AVX512F (1ULL << 8)
#define PAL_CPU_FEATURE_FMA3 (1ULL << 9)
#define PAL_CPU_FEATURE_BMI1 (1ULL << 10)
#define PAL_CPU_FEATURE_BMI2 (1ULL << 11)
/** @} */

/**
 * @defgroup platform_types Platform Types
 * @{
 */
#define PAL_PLATFORM_TYPE_WINDOWS 0
#define PAL_PLATFORM_TYPE_LINUX 1
#define PAL_PLATFORM_TYPE_MACOS 2
#define PAL_PLATFORM_TYPE_ANDROID 3
#define PAL_PLATFORM_TYPE_IOS 4
#define PAL_PLATFORM_TYPE_COUNT 5 /**< number of platform types */
/** @} */

/**
 * @defgroup platform_api_types Platform API Types
 * @{
 */
#define PAL_PLATFORM_API_TYPE_WIN32 0
#define PAL_PLATFORM_API_TYPE_WAYLAND 1
#define PAL_PLATFORM_API_TYPE_X11 2
#define PAL_PLATFORM_API_TYPE_COCOA 3
#define PAL_PLATFORM_API_TYPE_ANDRIOD 4
#define PAL_PLATFORM_API_TYPE_UIKIT 5
#define PAL_PLATFORM_API_TYPE_HEADLESS 6
#define PAL_PLATFORM_API_TYPE_COUNT 7 /**< number of platform API types */
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
 * only includes the instruction sets known to PAL, the CPU might support more.
 * 
 * All values of this type follow the format `PAL_CPU_FEATURE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint64_t PalCpuFeatures;

/**
 * @typedef PalPlatformType
 * @brief Platform type.
 *
 * All values of this type follow the format `PAL_PLATFORM_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalPlatformType;

/**
 * @typedef PalPlatformApiType
 * @brief Platform API type.
 * 
 * All values of this type follow the format `PAL_PLATFORM_API_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalPlatformApiType;
/** @} */

/**
 * @struct PalCPUInfo
 * @brief CPU information.
 *
 * @since Added in version 2.0
 * @ingroup pal_system
 */
typedef struct PalCPUInfo {
    PalCpuFeatures features; /**< supported CPU features (instruction sets) */
    PalCpuArch architecture; /**< CPU architecture */
    uint32_t numCores; /**< number of CPU cores */
    uint32_t cacheL1; /**< l1 cache in KB */
    uint32_t cacheL2; /**< l2 cache in KB */
    uint32_t cacheL3; /**< l3 cache in KB */
    uint32_t numLogicalProcessors; /**< number of CPUs */
    char vendor[PAL_CPU_VENDOR_NAME_SIZE]; /**< CPU vendor name */
    char model[PAL_CPU_MODEL_NAME_SIZE]; /**< CPU model name */
} PalCPUInfo;

/**
 * @struct PalPlatformInfo
 * @brief Platform information.
 *
 * @since Added in version 2.0
 * @ingroup pal_system
 */
typedef struct PalPlatformInfo {
    PalPlatformType type; /**< platform type */
    PalPlatformApiType apiType; /**< platform API type */
    uint32_t totalMemory; /**< platform disk size in GB*/
    uint32_t totalRAM; /**< platform memory (RAM) in MB */
    PalVersion version; /**< platform version */
    char name[PAL_PLATFORM_NAME_SIZE]; /**< platform name */
} PalPlatformInfo;

/**
 * @brief Gets the CPU information.
 * 
 * `allocator` is not copied. The allocator and any state referenced by it must
 * remain valid for the duration of this function.
 *
 * @param[in] allocator Allocator to use or `nullptr` for default.
 * @param[out] info Output struct to recieve the CPU information.
 *
 * @Thread-safety `info` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_system
 */
PAL_API void PAL_CALL palGetCPUInfo(
    const PalAllocator* allocator,
    PalCPUInfo* info);

/**
 * @brief Gets the platform information.
 *
 * @param[out] info Output struct to recieve the platform information.
 *
 * @Thread-safety `info` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_system
 */
PAL_API void PAL_CALL palGetPlatformInfo(PalPlatformInfo* info);

#endif // PAL_SYSTEM_H
