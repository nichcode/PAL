/**
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

#include "pal2/core/defines.h"
#include "pal2/core/memory.h"

/**
 * @defgroup cpu_architectures CPU Architectures
 * @brief CPU Architectures.
 * 
 * @ingroup pal_system
 */

/**
 * @defgroup cpu_features CPU Features(Instruction Sets)
 * @brief CPU features (instruction sets).
 * 
 * @ingroup pal_system
 */

/** @brief The maximum CPU vendor name size.
 * 
 * @ingroup pal_system
*/
#define PAL_CPU_VENDOR_NAME_SIZE 16

/** @brief The maximum CPU model name size.
 * 
 * @ingroup pal_system
*/
#define PAL_CPU_MODEL_NAME_SIZE 64

/** @brief The CPU architecture is unknown to PAL.
 * 
 * @ingroup cpu_architectures
*/
#define PAL_CPU_ARCH_UNKNOWN 0

/** @brief The CPU architecture is x86
 * 
 * @ingroup cpu_architectures
*/
#define PAL_CPU_ARCH_X86 1

/** @brief The CPU architecture is x64
 * 
 * @ingroup cpu_architectures
*/
#define PAL_CPU_ARCH_X86_64 2

/** @brief The CPU architecture is ARM
 * 
 * @ingroup cpu_architectures
*/
#define PAL_CPU_ARCH_ARM 3

/** @brief The CPU architecture is ARM64
 * 
 * @ingroup cpu_architectures
*/
#define PAL_CPU_ARCH_ARM64 4

/** @brief The maximum number of CPU architectures. The literal value must
 * not be used.
 * 
 * @ingroup cpu_architectures
*/
#define PAL_CPU_ARCH_COUNT 5

/** @brief The CPU supports SSE instruction set.
 * 
 * @ingroup cpu_features
*/
#define PAL_CPU_FEATURE_SSE (1ULL << 0)

/** @brief The CPU supports SSE2 instruction set.
 * 
 * @ingroup cpu_features
*/
#define PAL_CPU_FEATURE_SSE2 (1ULL << 1)

/** @brief The CPU supports SSE3 instruction set.
 * 
 * @ingroup cpu_features
*/
#define PAL_CPU_FEATURE_SSE3 (1ULL << 2)

/** @brief The CPU supports SSSE3 instruction set.
 * 
 * @ingroup cpu_features
*/
#define PAL_CPU_FEATURE_SSSE3 (1ULL << 3)

/** @brief The CPU supports SSE4.1 instruction set.
 * 
 * @ingroup cpu_features
*/
#define PAL_CPU_FEATURE_SSE41 (1ULL << 4)

/** @brief The CPU supports SSE4.2 instruction set.
 * 
 * @ingroup cpu_features
*/
#define PAL_CPU_FEATURE_SSE42 (1ULL << 5)

/** @brief The CPU supports AVX instruction set.
 * 
 * @ingroup cpu_features
*/
#define PAL_CPU_FEATURE_AVX (1ULL << 6)

/** @brief The CPU supports AVX2 instruction set.
 * 
 * @ingroup cpu_features
*/
#define PAL_CPU_FEATURE_AVX2 (1ULL << 7)

/** @brief The CPU supports AVX512F instruction set.
 * 
 * @ingroup cpu_features
*/
#define PAL_CPU_FEATURE_AVX512F (1ULL << 8)

/** @brief The CPU supports FMA3 instruction set.
 * 
 * @ingroup cpu_features
*/
#define PAL_CPU_FEATURE_FMA3 (1ULL << 9)

/** @brief The CPU supports BMI1 instruction set.
 * 
 * @ingroup cpu_features
*/
#define PAL_CPU_FEATURE_BMI1 (1ULL << 10)

/** @brief The CPU supports BMI2 instruction set.
 * 
 * @ingroup cpu_features
*/
#define PAL_CPU_FEATURE_BMI2 (1ULL << 11)

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
 * @struct PalCPUInfo
 * @brief Information about a CPU.
 *
 * @since Added in version 2.0
 */
typedef struct PalCPUInfo
{
    /** A bitmask of supported CPU features (instructions).*/
    PalCpuFeatures features;

    /** The CPU architecture (eg. PAL_CPU_ARCH_X86_64).*/
    PalCpuArch architecture;

    /** The total number of CPU cores.*/
    uint32_t numCores;

    /** The L1 cache in KB.*/
    uint32_t cacheL1;

    /** The L2 cache in KB.*/
    uint32_t cacheL2;

    /** The L3 cache in KB.*/
    uint32_t cacheL3;

    /** The total number of CPUs.*/
    uint32_t numLogicalProcessors;

    /** The CPU vendor name.*/
    char vendor[PAL_CPU_VENDOR_NAME_SIZE];

    /** The CPU model name.*/
    char model[PAL_CPU_MODEL_NAME_SIZE];
} PalCPUInfo;

/**
 * @brief Get the CPU information.
 *
 * @param[in] allocator The allocator to use. Set to nullptr to use the
 *                      thread-safe default allocator.
 * @param[out] info The output struct to recieve the CPU information.
 *
 * @Thread-safety `info` parameter must be per thread and `allocator` 
 *                parameter must be thread-safe.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetCPUInfo(
    const PalAllocator* allocator,
    PalCPUInfo* info);

#endif // PAL_SYSTEM_CPU_H