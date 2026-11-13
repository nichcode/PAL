
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

#ifndef SYSTEM_PLATFORM_H
#define SYSTEM_PLATFORM_H

#include "shared.h"
#include "pal2/pal_system.h"

/**
 * Platform contract for the system module.
 *
 * Every platform must implement every function declared here. A function
 * that says it has the same semantics and rules as a public `pal*` function
 * has only the platform requirements. A function with no public counterpart
 * is documented in full. These rules apply to all of them:
 *
 * - Arguments are validated by the caller. A platform function can assume
 *   every pointer it receives is valid and must not validate it again.
 *   Any exception will be documented by the function.
 */


/* =========================================================================
 * Platform
 * ========================================================================= */

/**
 * This function has the same semantics and rules as palGetPlatformInfo().
 *
 * Additional rules:
 * - Every field of `info` must be written, defaults must be used if there is
 *   no real information.
 */
void platformGetPlatformInfo(PalPlatformInfo* info);


/* =========================================================================
 * CPU
 * ========================================================================= */

/**
 * This function has the same semantics and rules as palGetCPUInfo().
 *
 * Additional rules:
 * - Every field of `info` must be written, defaults must be used if there is
 *   no real information.
 * - `allocator` is for temporary allocations only, when needed. Every
 *   temporary allocation must be freed before the function returns.
 * - If a temporary allocation fails, defaults must be used for the data.
 *   This function cannot report failure.
 */
void platformGetCPUInfo(
    const PalAllocator* allocator,
    PalCPUInfo* info);

#endif // SYSTEM_PLATFORM_H