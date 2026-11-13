
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

#ifndef CORE_PLATFORM_H
#define CORE_PLATFORM_H

#include "shared.h"
#include "pal2/core/result.h"
#include "pal2/core/library.h"
#include "pal2/core/log.h"
#include <stdbool.h>

/**
 * Platform contract for the core module.
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
 * Logging
 * ========================================================================= */

/** 
 * A sensible default for native code format buffer size. It can be increased.
 * A heap allocation must be avoided at all cost.
 */
#define FORMAT_BUFFER_SIZE 256

/** Contains Log TLS state. Every platform must use this exact data structure. */
typedef struct LogTLSData
{
    char tmp[PAL_LOG_MSG_SIZE];
    char buffer[PAL_LOG_MSG_SIZE];
    bool isLogging;
} LogTLSData;

/**
 * Creates a TLS for logging. This function returns nothing because it leaves
 * the actual TLS handle to the implementation.
 */
void platformCreateLogTLS(void);

/** Gets the Log TLS data with the TLS handle created from platformCreateLogTLS().
 * It returns nullptr if no data was set on the calling thread.
 */
LogTLSData* platformGetLogTLSData(void);

/** Sets the Log TLS data for the TLS handle created from  platformCreateLogTLS().
 * The platform only stores the pointer. It must not allocate, copy or free the data.
 */
void platformSetLogTLSData(LogTLSData* data);


/* =========================================================================
 * Result
 * ========================================================================= */

/**
 * Gets the native code and the source from the result and gets the platform
 * format string if available.
 *
 * - The buffer size is always `FORMAT_BUFFER_SIZE`, including the null terminator.
 * - `buffer` must always be null terminated. Text that does not fit is truncated.
 * - If there is nothing to format, `buffer` must be an empty string.
 * - This function cannot fail and must not allocate from the heap.
 */
void platformFormatResult(PalResult result, char* buffer);


/* =========================================================================
 * Time
 * ========================================================================= */

/**
 * This function has the same semantics and rules as palGetPerformanceCounter().
 *
 * Additional rules:
 * - Use a monotonic native source; a system clock must not be used.
 */
uint64_t platformGetPerformanceCounter(void);

/**
 * This function has the same semantics and rules as palGetPerformanceFrequency().
 *
 * Additional rules:
 * - The frequency must match the unit of `platformGetPerformanceCounter()`.
 *   For example, a counter in nanoseconds has a frequency of `1000000000`.
 */
uint64_t platformGetPerformanceFrequency(void);


/* =========================================================================
 * Library
 * ========================================================================= */

/** This function has the same semantics and rules as palLoadLibrary(). */
PalLibrary* platformLoadLibrary(const char* path);

/** This function has the same semantics and rules as palGetSymbol(). */
PalLibrarySymbol platformGetSymbol(PalLibrary* library, const char* name);

/** This function has the same semantics and rules as palFreeLibrary(). */
void platformFreeLibrary(PalLibrary* library);

#endif // CORE_PLATFORM_H