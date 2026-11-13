
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
 * Creates a TLS for logging. This function returns nothing because
 * it leaves the actual TLS handle to the implementation.
 */
void platformCreateLogTLS(void);

/** Gets the Log TLS data with the TLS handle created from 
 * platformCreateLogTLS().*/
LogTLSData* platformGetLogTLSData(void);

/** Sets the Log TLS data for the TLS handle created from 
 * platformCreateLogTLS(). */
void platformSetLogTLSData(LogTLSData* data);


/**
 * Gets the native code and the source from the result
 * and gets the platform format string if available.
 * The buffer size is must be `FORMAT_BUFFER_SIZE` including the
 * Null-terminator.
 */
void platformFormatResult(PalResult result, char* buffer);


/** Gets the performance counter value */
uint64_t platformGetPerformanceCounter(void);

/** Get the performance counter frequency in counts per second. */
uint64_t platformGetPerformanceFrequency(void);


/** Loads a dynamic library. If the library depends on other libraries,
 * they must be loaded. If `path` is nullptr, behavior is
 * implementation defined. */
PalLibrary* platformLoadLibrary(const char* path);

/** Gets a symbol from a loaded dynamic library. */
PalLibrarySymbol platformGetSymbol(PalLibrary* library, const char* name);

/** Frees a loaded dynamic library. */
void platformFreeLibrary(PalLibrary* library);

#endif // CORE_PLATFORM_H