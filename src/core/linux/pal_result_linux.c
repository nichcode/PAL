
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef __linux__
#define _POSIX_C_SOURCE 200112L
#include "core/pal_format.h"
#include "core/pal_result.h"
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
    char tmpBuffer[256];
    uint32_t nativeCode = getResultNativeCode(result);
    if (nativeCode != 0) {
        strerror_r(nativeCode, tmpBuffer, 256);
        formatResultMsg(result, buffer, tmpBuffer);

    } else {
        formatResultMsg(result, buffer, nullptr);
    }
}

#endif // __linux__