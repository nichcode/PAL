
/**
 * PAL - Prime Abstraction Layer (PAL)
 * A cross platform abstraction layer over graphics and windowing APIs
 * -------------------------------------------------------------------
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

#ifndef CORE_PLATFORM_H
#define CORE_PLATFORM_H

#include "pal2/pal_core.h"
#include <stdbool.h>

/** 
 * A sensible default for native code format buffer size. It can be increased.
 * A heap allocation must be avoided at all cost.
 */
#define FORMAT_BUFFER_SIZE 256

/** Contains Log TLS state. Every platform must use this exact data structure*/
typedef struct LogTLSData
{
    char tmp[PAL_LOG_MSG_SIZE];
    char buffer[PAL_LOG_MSG_SIZE];
    bool isLogging;
} LogTLSData;

/**
 * Create a TLS for logging. This function returns nothing because
 * it leaves the actual TLS handle to the implementation.
 */
void platformCreateLogTLS(void);
LogTLSData* platformGetLogTLSData(void);
void platformSetLogTLSData(LogTLSData* data);

void platformFormatResult(PalResult result, char* buffer);

uint64_t platformGetPerformanceCounter(void);
uint64_t platformGetPerformanceFrequency(void);

PalLibrary* platformLoadLibrary(const char* path);
PalLibrarySymbol platformGetSymbol(PalLibrary* library, const char* name);
void platformFreeLibrary(PalLibrary* library);


#endif // CORE_PLATFORM_H