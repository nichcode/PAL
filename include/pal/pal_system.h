/**

Copyright (C) 2025 Nicholas Agbo

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

/**
 * @file pal_system.h
 *
 * Header file for system functions, macros, enum and structs
 *
 * @defgroup system
 *
 */

#ifndef _PAL_SYSTEM_H
#define _PAL_SYSTEM_H

#include "pal_core.h"

#define PAL_PLATFORM_NAME_SIZE 32
#define PAL_CPU_VENDOR_NAME_SIZE 16
#define PAL_CPU_MODEL_NAME_SIZE 64

/**
 * @enum PalCpuArch
 * @brief CPU achitecture This is not a bitmask.
 *
 * This is a build time achitecture. Example: Generating your project with x64
 * will reflect PAL_CPU_ARCH_X86_64.
 *
 * @note All CPU achitectures follow the format `PAL_CPU_ARCH_**` for
 * consistency and API use.
 *
 * @since Added in version 1.0.0.
 * @ingroup system
 */
typedef enum {
    /** Unknown Architecture.*/
    PAL_CPU_ARCH_UNKNOWN,

    /** 32 bit Architecture.*/
    PAL_CPU_ARCH_X86,

    /** 64 bit Architecture.*/
    PAL_CPU_ARCH_X86_64,

    /** Arm Architecture.*/
    PAL_CPU_ARCH_ARM,

    /** Arm64 Architecture.*/
    PAL_CPU_ARCH_ARM64
} PalCpuArch;

/**
 * @enum PalCpuFeatures
 * @brief CPU features (instruction sets).
 *
 * @note All CPU features sets follow the format `PAL_CPU_FEATURE_**` for
 * consistency and API use.
 *
 * @sa PalCPUInfo
 * @sa palGetCPUInfo()
 *
 * @since Added in version 1.0.0.
 * @ingroup system
 */
typedef enum {
    /** SSE (SIMD operations) instruction set.*/
    PAL_CPU_FEATURE_SSE = PAL_BIT(0),

    /** SSE2 (SIMD operations) instruction set.*/
    PAL_CPU_FEATURE_SSE2 = PAL_BIT(1),

    /** SSE3 (SIMD operations) instruction set.*/
    PAL_CPU_FEATURE_SSE3 = PAL_BIT(2),

    /** SSSE3 (SIMD operations) instruction set.*/
    PAL_CPU_FEATURE_SSSE3 = PAL_BIT(3),

    /** SSE4.1 (SIMD operations) instruction set.*/
    PAL_CPU_FEATURE_SSE41 = PAL_BIT(4),

    /** SSE4.2 (SIMD operations) instruction set.*/
    PAL_CPU_FEATURE_SSE42 = PAL_BIT(5),

    /** AVX (SIMD operations) instruction set.*/
    PAL_CPU_FEATURE_AVX = PAL_BIT(6),

    /** AVX2 (SIMD operations) instruction set.*/
    PAL_CPU_FEATURE_AVX2 = PAL_BIT(7),

    /** AVX512F (SIMD operations) instruction set.*/
    PAL_CPU_FEATURE_AVX512F = PAL_BIT(8),

    /** FMA3 instruction set.*/
    PAL_CPU_FEATURE_FMA3 = PAL_BIT(9),

    /** BMI1 instruction set.*/
    PAL_CPU_FEATURE_BMI1 = PAL_BIT(10),

    /** BMI2 instruction set.*/
    PAL_CPU_FEATURE_BMI2 = PAL_BIT(11)
} PalCpuFeatures;

/**
 * @enum PalPlatformType
 * @brief Platform types. This is not a bitmask.
 *
 * This is the family name (eg. This does not show if its Windows 7 or Windows 8
 * etc)
 *
 * @note All platform types follow the format `PAL_PLATFORM_**` for
 * consistency and API use.
 *
 * @since Added in version 1.0.0.
 * @ingroup system
 */
typedef enum {
    /** Windows Platform (OS).*/
    PAL_PLATFORM_WINDOWS,

    /** Linux Platform (OS).*/
    PAL_PLATFORM_LINUX,

    /** MacOS Platform (OS).*/
    PAL_PLATFORM_MACOS,

    /** Android Platform (OS).*/
    PAL_PLATFORM_ANDROID,

    /** IOS Platform (OS).*/
    PAL_PLATFORM_IOS
} PalPlatformType;

/**
 * @enum PalPlatformApiType
 * @brief Platform API types. This is not a bitmask.
 *
 * This is the API the playform uses. Most platforms support only one (eg.
 * Windows)
 *
 * @note All platform API types follow the format `PAL_PLATFORM_API_**` for
 * consistency and API use.
 *
 * @since Added in version 1.0.0.
 * @ingroup system
 */
typedef enum {
    /** Win32 API.*/
    PAL_PLATFORM_API_WIN32,

    /** Wayland API.*/
    PAL_PLATFORM_API_WAYLAND,

    /** X11 API.*/
    PAL_PLATFORM_API_X11
} PalPlatformApiType;

/**
 * @struct PalPlatformInfo
 * @brief Information about a platform (OS).
 *
 * @sa palGetPlatformInfo()
 *
 * @since Added in version 1.0.0.
 * @ingroup system
 */
typedef struct {
    /** Platform family type. See PalPlatformType.*/
    PalPlatformType type;

    /** Platform API type. See PalPlatformApiType.*/
    PalPlatformApiType apiType;

    /** Total Disk space (memory) in GB.*/
    Uint32 totalMemory;

    /** Total CPU RAM (memory) in MB.*/
    Uint32 totalRAM;

    /** < Use this to check a specific platform aside the family.*/
    PalVersion version;

    /** < This is the platform name with the version combined as a UTF-8
     * encoding string. (eg. Windows 11.22000)*/
    char name[PAL_PLATFORM_NAME_SIZE];
} PalPlatformInfo;

/**
 * @struct PalCPUInfo
 * @brief Information about a CPU.
 *
 * @sa palGetCPUInfo()
 *
 * @since Added in version 1.0.0.
 * @ingroup system
 */
typedef struct {
    /** Number of CPU cores.*/
    Uint32 numCores;

    /** L1 cache in KB.*/
    Uint32 cacheL1;

    /** L2 cache in KB.*/
    Uint32 cacheL2;

    /** L3 cache in KB.*/
    Uint32 cacheL3;

    /** Number of threads that can run simultaneously (CPUs).*/
    Uint32 numLogicalProcessors;

    /** Based on how the project will be or built. See PalCpuArch.*/
    PalCpuArch architecture;

    /** Supported CPU instruction sets.*/
    PalCpuFeatures features;

    /** CPU vendor name. (eg. GenuineIntel).*/
    char vendor[PAL_CPU_VENDOR_NAME_SIZE];

    /** CPU modal name.*/
    char model[PAL_CPU_MODEL_NAME_SIZE];
} PalCPUInfo;

/**
 * @brief Get platform (OS) information.
 *
 * @param[out] info Pointer to a PalPlatformInfo to receive the platform info.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on
 * failure. Call palFormatResult() for more information.
 *
 * @note This function is not thread-safe.
 *
 * @sa PalPlatformInfo
 *
 * @since Added in version 1.0.0.
 * @ingroup system
 */
PAL_API PalResult PAL_CALL palGetPlatformInfo(PalPlatformInfo* info);

/**
 * @brief Get CPU information.
 *
 * @param[in] allocator Optional user provided allocator. Set to nullptr to
 * use default.
 * @param[out] info Pointer to a PalCPUInfo to receive the CPU info.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on
 * failure. Call palFormatResult() for more information.
 *
 * @note This function is not thread-safe.
 *
 * @sa PalAllocator
 * @sa PalCPUInfo
 *
 * @since Added in version 1.0.0.
 * @ingroup system
 */
PAL_API PalResult PAL_CALL palGetCPUInfo(
    const PalAllocator* allocator,
    PalCPUInfo* info);

#endif // _PAL_SYSTEM_H