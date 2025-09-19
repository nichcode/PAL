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

typedef enum {
    PAL_CPU_ARCH_UNKNOWN,
    PAL_CPU_ARCH_X86,    /** < 32 bit Architecture.*/
    PAL_CPU_ARCH_X86_64, /** < 64 bit Architecture.*/
    PAL_CPU_ARCH_ARM,
    PAL_CPU_ARCH_ARM64
} PalCpuArch;

typedef enum {
    PAL_CPU_FEATURE_SSE = PAL_BIT(0),
    PAL_CPU_FEATURE_SSE2 = PAL_BIT(1),
    PAL_CPU_FEATURE_SSE3 = PAL_BIT(2),
    PAL_CPU_FEATURE_SSSE3 = PAL_BIT(3),
    PAL_CPU_FEATURE_SSE41 = PAL_BIT(4), /** < SSE4.1 instruction set.*/
    PAL_CPU_FEATURE_SSE42 = PAL_BIT(5), /** < SSE4.2 instruction set.*/
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
    Uint32 totalMemory; /** < Total Disk memory*/
    Uint32 totalRAM;    /** < Total CPU memory*/
    PalVersion version; /** < Platform version*/
    char name[PAL_PLATFORM_NAME_SIZE];
} PalPlatformInfo;

typedef struct {
    Uint32 numCores;             /** < Number of cores*/
    Uint32 cache1;               /** < L1 cache in KB*/
    Uint32 cache2;               /** < L2 cache in KB*/
    Uint32 cache3;               /** < L3 cache in KB*/
    Uint32 numLogicalProcessors; /** < Number of threads that can run
                                    simultaneously*/
    PalCpuArch architecture; /** < Build architecture. Based on how the project
                                will be built.*/
    PalCpuFeatures features; /** < Supported instruction sets*/
    char vendor[PAL_CPU_VENDOR_NAME_SIZE];
    char model[PAL_CPU_MODEL_NAME_SIZE];
} PalCPUInfo;

/**
 * @brief Get platform (OS) information.
 *
 * @param[out] info Pointer to receive the platform info.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on
 * failure.
 *
 * @note This function is not thread-safe.
 *
 * @sa PalPlatformInfo
 * @ingroup system
 */
PAL_API PalResult PAL_CALL palGetPlatformInfo(PalPlatformInfo* info);

/**
 * @brief Get CPU information.
 *
 * @param[in] allocator Optional user provided allocator. Set to `nullptr` to
 * use default.
 * @param[out] info Pointer to receive the CPU info.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on
 * failure.
 *
 * @note This function is not thread-safe.
 *
 * @sa PalAllocator, PalCPUInfo
 * @ingroup system
 */
PAL_API PalResult PAL_CALL palGetCPUInfo(
    const PalAllocator* allocator,
    PalCPUInfo* info);

#endif // _PAL_SYSTEM_H