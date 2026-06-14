
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef __linux__
#define _POSIX_C_SOURCE 200112L
#include "pal/pal_core.h"
#include "core/pal_result_common.h"
#include <string.h>

uint16_t PAL_CALL palGetResultCode(PalResult result)
{
    getResultCode(result);
}

void PAL_CALL palFormatResult(
    PalResult result, 
    uint64_t bufferSize,
    char* buffer)
{
    const char* baseString = resultCodeToString(result);
    const char* sourceString = resultSourceToString(result);
    uint32_t nativeCode = getResultNativeCode(result);
    strerror_r(nativeCode, buffer, bufferSize);
}

#endif // __linux__