/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup pal_system CPU and platform
 * @ingroup pal_system
 * @{
 */

#ifndef _PAL_SYSTEM_H
#define _PAL_SYSTEM_H

#include "core/defines.h"
#include "core/result.h"
#include "core/version.h"
#include "core/memory.h"

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
 */
typedef enum {
    PAL_PLATFORM_API_WIN32,
    PAL_PLATFORM_API_WAYLAND,
    PAL_PLATFORM_API_X11,
    PAL_PLATFORM_API_COCOA,
    PAL_PLATFORM_API_ANDRIOD,
    PAL_PLATFORM_API_UIKIT,
    PAL_PLATFORM_API_HEADLESS
} PalPlatformApiType;

/**
 * @struct PalPlatformInfo
 * @brief Information about a platform (OS).
 *
 * @since 1.0
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
 * Thread safety: Thread-safe if `info` is per thread.
 *
 * @since 1.0
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
 * Thread safety: Thread-safe if the proivded allocator is
 * thread safe and `info` is per thread. The default allocator is thread safe.
 *
 * @since 1.0
 */
PAL_API PalResult PAL_CALL palGetCPUInfo(
    const PalAllocator* allocator,
    PalCPUInfo* info);

/** @} */

#endif // _PAL_SYSTEM_H
