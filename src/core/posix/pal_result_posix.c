
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal_platform.h"

#if _PAL_HAS_POSIX
#define _POSIX_C_SOURCE 200112L
#include "core/pal_format.h"
#include "core/pal_result.h"
#include <string.h>

void PAL_CALL palFormatResult(
    PalResult result, 
    uint64_t bufferSize,
    char* buffer)
{
    char tmpBuffer[256];
    uint32_t nativeCode = palGetResultNativeCode(result);
    if (nativeCode != 0) {
        strerror_r(nativeCode, tmpBuffer, 256);
        formatResultMsg(result, buffer, tmpBuffer);
    }
}

#endif // _PAL_HAS_POSIX