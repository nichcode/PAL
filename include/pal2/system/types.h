
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

#ifndef PAL_SYSTEM_TYPES_H
#define PAL_SYSTEM_TYPES_H

#include "pal2/core/types.h"

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
 * @ingroup pal_system
 */
typedef uint32_t PalCpuArch;

/**
 * @typedef PalCpuFeatures
 * @brief CPU features (instruction sets).
 *
 * Identifies the instruction sets supported by the CPU. This is a bitmask of all
 * supported instruction sets of the CPU. This only includes the instruction sets
 * known to PAL; the CPU might support more.
 *
 * All values of this type follow the format `PAL_CPU_FEATURE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_system
 */
typedef uint64_t PalCpuFeatures;

/**
 * @typedef PalPlatformType
 * @brief Platform (operating system) type.
 *
 * Identifies the operating system of the platform.
 *
 * All values of this type follow the format `PAL_PLATFORM_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_system
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
 * @ingroup pal_system
 */
typedef uint32_t PalPlatformApiType;

#endif // PAL_SYSTEM_TYPES_H