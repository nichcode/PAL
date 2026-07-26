/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup pal_system System
 * @ingroup pal_system
 * @{
 */

#ifndef _PAL_SYSTEM_H
#define _PAL_SYSTEM_H

#include "pal_core.h"

#define PAL_PLATFORM_NAME_SIZE 32
#define PAL_CPU_VENDOR_NAME_SIZE 16
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
#define PAL_CPU_FEATURE_SSE41 (1ULL << 4) /**< SSE4.1.*/
#define PAL_CPU_FEATURE_SSE42 (1ULL << 5) /**< SSE4.2.*/
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
 * @brief CPU achitecture. This is not a bitmask.
 *
 * All CPU achitectures follow the format `PAL_CPU_ARCH_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalCpuArch;

/**
 * @typedef PalCpuFeatures
 * @brief CPU features (instruction sets).
 *
 * All CPU features sets follow the format `PAL_CPU_FEATURE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint64_t PalCpuFeatures;

/**
 * @typedef PalPlatformType
 * @brief Platform types. This is not a bitmask.
 *
 * This is the family name (eg. This does not show if its Windows 7 or Windows 8
 * etc).
 *
 * All platform types follow the format `PAL_PLATFORM_TYPE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalPlatformType;

/**
 * @typedef PalPlatformApiType
 * @brief Platform API types. This is not a bitmask.
 *
 * This is the API the playform uses. Most platforms support only one (eg.
 * Windows).
 *
 * All platform API types follow the format `PAL_PLATFORM_API_TYPE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalPlatformApiType;

/**
 * @struct PalPlatformInfo
 * @brief Information about a platform (OS).
 *
 * @since 2.0
 */
typedef struct {
    PalPlatformType type;              /**< (eg. `PAL_PLATFORM_TYPE_WINDOWS`).*/
    PalPlatformApiType apiType;        /**< (eg. `PAL_PLATFORM_API_TYPE_WIN32`).*/
    uint32_t totalMemory;              /**< Total Disk space (memory) in GB.*/
    uint32_t totalRAM;                 /**< Total CPU RAM (memory) in MB.*/
    PalVersion version;                /**< Platform version.*/
    char name[PAL_PLATFORM_NAME_SIZE]; /**< (eg. Windows 11.22000).*/
} PalPlatformInfo;

/**
 * @struct PalCPUInfo
 * @brief Information about a CPU.
 *
 * @since 2.0
 */
typedef struct {
    PalCpuFeatures features;               /**< Supported CPU features (instructions).*/
    PalCpuArch architecture;               /**< (eg. `PAL_CPU_ARCH_X86_64`).*/
    uint32_t numCores;                     /**< Number of cores.*/
    uint32_t cacheL1;                      /**< L1 cache in KB.*/
    uint32_t cacheL2;                      /**< L2 cache in KB.*/
    uint32_t cacheL3;                      /**< L3 cache in KB.*/
    uint32_t numLogicalProcessors;         /**< Number of CPUs.*/
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
 * @since 2.0
 */
PAL_API void PAL_CALL palGetPlatformInfo(PalPlatformInfo* info);

/**
 * @brief Get CPU information.
 *
 * @param[in] allocator Optional user provided allocator. Set to `nullptr` to
 * use default.
 * @param[out] info Pointer to a PalCPUInfo to receive the CPU info.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread-safe if the proivded allocator is
 * thread safe and `info` is per thread. The default allocator is thread safe.
 *
 * @since 2.0
 */
PAL_API void PAL_CALL palGetCPUInfo(
    const PalAllocator* allocator,
    PalCPUInfo* info);

/** @} */

#endif // _PAL_SYSTEM_H
