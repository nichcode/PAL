
/**
 * @file types.h
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

#ifndef PAL_SYSTEM_CPU_H
#define PAL_SYSTEM_CPU_H

#include "types.h"
#include "pal2/core/memory.h"

/**
 * @brief Maximum size of the CPU vendor name, including the NULL terminator.
 * @ingroup pal_system
 */
#define PAL_CPU_VENDOR_NAME_SIZE 16

/**
 * @brief Maximum size of the CPU model name, including the NULL terminator.
 * @ingroup pal_system
 */
#define PAL_CPU_MODEL_NAME_SIZE 64

/**
 * @defgroup cpu_architectures CPU Architectures
 * @brief Defines architectures of a CPU.
 * @ingroup pal_system
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
 * @brief Defines instruction sets of a CPU.
 * @ingroup pal_system
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
#define PAL_CPU_FEATURE_BMI1 (1ULL << 10) /**< Bit manipulation instructions 1 (BMI1). */
#define PAL_CPU_FEATURE_BMI2 (1ULL << 11) /**< Bit manipulation instructions 2 (BMI2). */
/** @} */

/**
 * @struct PalCPUInfo
 * @brief CPU information.
 *
 * Filled in by @ref palGetCPUInfo().
 *
 * @since Added in version 2.0
 * @ingroup pal_system
 */
typedef struct PalCPUInfo {
    PalCpuFeatures features; /**< Supported CPU features (instruction sets). */
    PalCpuArch architecture; /**< CPU architecture. */
    uint32_t numCores; /**< Number of physical cores. */
    uint32_t cacheL1; /**< L1 cache size in KB. */
    uint32_t cacheL2; /**< L2 cache size in KB. */
    uint32_t cacheL3; /**< L3 cache size in KB. */
    uint32_t numLogicalProcessors; /**< Number of logical processors (hardware threads). */
    char vendor[PAL_CPU_VENDOR_NAME_SIZE]; /**< CPU vendor name (NULL-terminated). */
    char model[PAL_CPU_MODEL_NAME_SIZE]; /**< CPU model name (NULL-terminated). */
} PalCPUInfo;

/**
 * @brief Gets the CPU information.
 *
 * `allocator` is not copied. The allocator and any state referenced by it must
 * remain valid for the duration of this function.
 *
 * @param[in] allocator Allocator to use, or `nullptr` for the default.
 * @param[out] info Output struct to receive the CPU information.
 *
 * @thread-safety `info` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_system
 */
PAL_API void PAL_CALL palGetCPUInfo(
    const PalAllocator* allocator,
    PalCPUInfo* info);

#endif // PAL_SYSTEM_CPU_H