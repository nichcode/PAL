
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef _WIN32
#include "pal_format.h" 
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
    if (nativeCode != 0) {
        char tmpBuffer[256];
        FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            nativeCode,
            0,
            tmpBuffer,
            256,
            nullptr);

        format(buffer, "%s (0x%08x) - %s: %s", sourceString, nativeCode, baseString, tmpBuffer);

    } else {
        format(buffer, "%s (0x%08x) - %s", sourceString, nativeCode, baseString);
    }
}

#endif // _WIN32