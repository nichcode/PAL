/**
 * @file pal_system.h
 * @brief Header file for the PAL System API.
 * 
 * Defines all the types, constants and functions of the system module.
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

#define PAL_CPU_VENDOR_NAME_SIZE 16 /**< Maximum size of the CPU vendor name, including the null terminator. */
#define PAL_CPU_MODEL_NAME_SIZE 64 /**< Maximum size of the CPU model name, including the null terminator. */
#define PAL_PLATFORM_NAME_SIZE 32 /**< Maximum size of the platform name, including the null terminator. */

/**
 * @defgroup cpu_architectures CPU Architectures
 * @{
 */
#define PAL_CPU_ARCH_UNKNOWN 0 /**< Unknown or unsupported architecture. */
#define PAL_CPU_ARCH_X86 1 /**< 32-bit x86. */
#define PAL_CPU_ARCH_X86_64 2 /**< 64-bit x86 (x64 / AMD64). */
#define PAL_CPU_ARCH_ARM 3 /**< 32-bit ARM. */
#define PAL_CPU_ARCH_ARM64 4 /**< 64-bit ARM (AArch64). */
#define PAL_CPU_ARCH_COUNT 5 /**< Number of CPU architectures. */
/** @} */

/**
 * @defgroup cpu_features CPU Features (Instruction Sets)
 * @{
 */
#define PAL_CPU_FEATURE_SSE (1ULL << 0) /**< SSE. */
#define PAL_CPU_FEATURE_SSE2 (1ULL << 1) /**< SSE2. */
#define PAL_CPU_FEATURE_SSE3 (1ULL << 2) /**< SSE3. */
#define PAL_CPU_FEATURE_SSSE3 (1ULL << 3) /**< Supplemental SSE3 (SSSE3). */
#define PAL_CPU_FEATURE_SSE41 (1ULL << 4) /**< SSE4.1. */
#define PAL_CPU_FEATURE_SSE42 (1ULL << 5) /**< SSE4.2. */
#define PAL_CPU_FEATURE_AVX (1ULL << 6) /**< AVX. */
#define PAL_CPU_FEATURE_AVX2 (1ULL << 7) /**< AVX2. */
#define PAL_CPU_FEATURE_AVX512F (1ULL << 8) /**< AVX-512 Foundation (AVX-512F). */
#define PAL_CPU_FEATURE_FMA3 (1ULL << 9) /**< Fused multiply-add (FMA3). */
#define PAL_CPU_FEATURE_BMI1 (1ULL << 10) /**< Bit Manipulation Instructions 1 (BMI1). */
#define PAL_CPU_FEATURE_BMI2 (1ULL << 11) /**< Bit Manipulation Instructions 2 (BMI2). */
/** @} */

/**
 * @defgroup platform_types Platform Types
 * @{
 */
#define PAL_PLATFORM_TYPE_WINDOWS 0 /**< Windows. */
#define PAL_PLATFORM_TYPE_LINUX 1 /**< Linux. */
#define PAL_PLATFORM_TYPE_MACOS 2 /**< MacOS. */
#define PAL_PLATFORM_TYPE_ANDROID 3 /**< Android. */
#define PAL_PLATFORM_TYPE_IOS 4 /**< iOS. */
#define PAL_PLATFORM_TYPE_COUNT 5 /**< Number of platform types. */
/** @} */

/**
 * @defgroup platform_api_types Platform API Types
 * @{
 */
#define PAL_PLATFORM_API_TYPE_WIN32 0 /**< Win32 (Windows). */
#define PAL_PLATFORM_API_TYPE_WAYLAND 1 /**< Wayland (Linux). */
#define PAL_PLATFORM_API_TYPE_X11 2 /**< X11 (Linux). */
#define PAL_PLATFORM_API_TYPE_COCOA 3 /**< Cocoa (macOS). */
#define PAL_PLATFORM_API_TYPE_ANDRIOD 4 /**< Android native windowing. */
#define PAL_PLATFORM_API_TYPE_UIKIT 5 /**< UIKit (iOS). */
#define PAL_PLATFORM_API_TYPE_HEADLESS 6 /**< No windowing system available. */
#define PAL_PLATFORM_API_TYPE_COUNT 7 /**< Number of platform API types. */
/** @} */

/**
 * @typedef PalCpuArch
 * @brief CPU architecture.
 *
 * Identifies the instruction set architecture of the CPU.
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
 * only includes the instruction sets known to PAL; the CPU might support more.
 *
 * All values of this type follow the format `PAL_CPU_FEATURE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint64_t PalCpuFeatures;

/**
 * @typedef PalPlatformType
 * @brief Platform (operating system) type.
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
 * Identifies the windowing/system API used on the platform.
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
    PalCpuFeatures features; /**< Supported CPU features (instruction sets). */
    PalCpuArch architecture; /**< CPU architecture. */
    uint32_t numCores; /**< Number of physical CPU cores. */
    uint32_t cacheL1; /**< L1 cache size in KB. */
    uint32_t cacheL2; /**< L2 cache size in KB. */
    uint32_t cacheL3; /**< L3 cache size in KB. */
    uint32_t numLogicalProcessors; /**< Number of logical processors (hardware threads). */
    char vendor[PAL_CPU_VENDOR_NAME_SIZE]; /**< CPU vendor name (null-terminated). */
    char model[PAL_CPU_MODEL_NAME_SIZE]; /**< CPU model name (null-terminated). */
} PalCPUInfo;

/**
 * @struct PalPlatformInfo
 * @brief Platform information.
 *
 * @since Added in version 2.0
 * @ingroup pal_system
 */
typedef struct PalPlatformInfo {
    PalPlatformType type; /**< Platform type. */
    PalPlatformApiType apiType; /**< Platform API type. */
    uint32_t totalMemory; /**< Total disk size in GB. */
    uint32_t totalRAM; /**< Total system memory (RAM) in MB. */
    PalVersion version; /**< Platform (OS) version. */
    char name[PAL_PLATFORM_NAME_SIZE]; /**< Platform name (null-terminated). */
} PalPlatformInfo;

/**
 * @brief Gets the CPU information.
 *
 * `allocator` is not copied. The allocator and any state referenced by it must
 * remain valid for the duration of this function.
 *
 * @param[in] allocator Allocator to use, or `nullptr` for the default.
 * @param[out] info Output struct to receive the CPU information. Must not be `nullptr`.
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
 * @param[out] info Output struct to receive the platform information. Must not be `nullptr`.
 *
 * @Thread-safety `info` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_system
 */
PAL_API void PAL_CALL palGetPlatformInfo(PalPlatformInfo* info);

#endif // PAL_SYSTEM_H