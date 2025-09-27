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
 * @defgroup pal_system System
 * System PAL functionality such as CPU info and Platform info.
 *
 * @{
 */

#ifndef _PAL_SYSTEM_H
#define _PAL_SYSTEM_H

#include "pal_core.h"

#define PAL_PLATFORM_NAME_SIZE 32
#define PAL_CPU_VENDOR_NAME_SIZE 16
#define PAL_CPU_MODEL_NAME_SIZE 64

/**
 * @enum PalCpuArch
 * @brief CPU achitecture. This is not a bitmask.
 *
 * This is a build time achitecture. Example: Generating your project with x64
 * will reflect PAL_CPU_ARCH_X86_64.
 *
 * All CPU achitectures follow the format `PAL_CPU_ARCH_**` for
 * consistency and API use.
 *
 * @since 1.0
 * @ingroup pal_system
 */
typedef enum {
    PAL_CPU_ARCH_UNKNOWN,
    PAL_CPU_ARCH_X86,
    PAL_CPU_ARCH_X86_64,
    PAL_CPU_ARCH_ARM,
    PAL_CPU_ARCH_ARM64
} PalCpuArch;

/**
 * @enum PalCpuFeatures
 * @brief CPU features (instruction sets).
 *
 * All CPU features sets follow the format `PAL_CPU_FEATURE_**` for
 * consistency and API use.
 *
 * @since 1.0
 * @ingroup pal_system
 */
typedef enum {
    PAL_CPU_FEATURE_SSE = PAL_BIT(0),
    PAL_CPU_FEATURE_SSE2 = PAL_BIT(1),
    PAL_CPU_FEATURE_SSE3 = PAL_BIT(2),
    PAL_CPU_FEATURE_SSSE3 = PAL_BIT(3),
    PAL_CPU_FEATURE_SSE41 = PAL_BIT(4), /**< SSE4.1.*/
    PAL_CPU_FEATURE_SSE42 = PAL_BIT(5), /**< SSE4.1.*/
    PAL_CPU_FEATURE_AVX = PAL_BIT(6),
    PAL_CPU_FEATURE_AVX2 = PAL_BIT(7),
    PAL_CPU_FEATURE_AVX512F = PAL_BIT(8),
    PAL_CPU_FEATURE_FMA3 = PAL_BIT(9),
    PAL_CPU_FEATURE_BMI1 = PAL_BIT(10),
    PAL_CPU_FEATURE_BMI2 = PAL_BIT(11)
} PalCpuFeatures;

/**
 * @enum PalPlatformType
 * @brief Platform types. This is not a bitmask.
 *
 * This is the family name (eg. This does not show if its Windows 7 or Windows 8
 * etc).
 *
 * All platform types follow the format `PAL_PLATFORM_**` for
 * consistency and API use.
 *
 * @since 1.0
 * @ingroup pal_system
 */
typedef enum {
    PAL_PLATFORM_WINDOWS,
    PAL_PLATFORM_LINUX,
    PAL_PLATFORM_MACOS,
    PAL_PLATFORM_ANDROID,
    PAL_PLATFORM_IOS
} PalPlatformType;

/**
 * @enum PalPlatformApiType
 * @brief Platform API types. This is not a bitmask.
 *
 * This is the API the playform uses. Most platforms support only one (eg.
 * Windows).
 *
 * All platform API types follow the format `PAL_PLATFORM_API_**` for
 * consistency and API use.
 *
 * @since 1.0
 * @ingroup pal_system
 */
typedef enum {
    PAL_PLATFORM_API_WIN32,
    PAL_PLATFORM_API_WAYLAND,
    PAL_PLATFORM_API_X11
} PalPlatformApiType;

/**
 * @struct PalPlatformInfo
 * @brief Information about a platform (OS).
 *
 * @since 1.0
 * @ingroup pal_system
 */
typedef struct {
    PalPlatformType type;
    PalPlatformApiType apiType;
    Uint32 totalMemory; /**< Total Disk space (memory) in GB.*/
    Uint32 totalRAM;    /**< Total CPU RAM (memory) in MB.*/
    PalVersion version;
    char name[PAL_PLATFORM_NAME_SIZE]; /**< (eg. Windows 11.22000).*/
} PalPlatformInfo;

/**
 * @struct PalCPUInfo
 * @brief Information about a CPU.
 *
 * @since 1.0
 * @ingroup pal_system
 */
typedef struct {
    Uint32 numCores;
    Uint32 cacheL1;              /**< L1 cache in KB.*/
    Uint32 cacheL2;              /**< L2 cache in KB.*/
    Uint32 cacheL3;              /**< L3 cache in KB.*/
    Uint32 numLogicalProcessors; /**< Number of CPUs.*/
    PalCpuArch architecture;
    PalCpuFeatures features;
    char vendor[PAL_CPU_VENDOR_NAME_SIZE]; /**< CPU vendor name.*/
    char model[PAL_CPU_MODEL_NAME_SIZE];   /**< CPU modal name.*/
} PalCPUInfo;

/**
 * @brief Get platform (OS) information.
 *
 * @param[out] info Pointer to a PalPlatformInfo to receive the platform info.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function is thread-safe if `info` is thread local.
 *
 * @since 1.0
 * @ingroup pal_system
 */
PAL_API PalResult PAL_CALL palGetPlatformInfo(PalPlatformInfo* info);

/**
 * @brief Get CPU information.
 *
 * @param[in] allocator Optional user provided allocator. Set to nullptr to
 * use default.
 * @param[out] info Pointer to a PalCPUInfo to receive the CPU info.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: This function is thread-safe if the proivded allocator is
 * thread safe and `info` is thread local. The default allocator is thread safe.
 *
 * @since 1.0
 * @ingroup pal_system
 */
PAL_API PalResult PAL_CALL palGetCPUInfo(
    const PalAllocator* allocator,
    PalCPUInfo* info);

/** @} */ // end of pal_system group

#endif // _PAL_SYSTEM_H