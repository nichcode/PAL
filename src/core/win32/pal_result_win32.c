
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

// set unicode
#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include "core/pal_format.h"
#include "core/pal_result.h"
#include <string.h>
#include <windows.h>

void PAL_CALL palFormatResult(
    PalResult result,
    uint64_t bufferSize,
    char* buffer)
{
    char tmpBuffer[256];
    uint32_t nativeCode = palGetResultNativeCode(result);
    if (nativeCode != 0) {
        FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            nativeCode,
            0,
            tmpBuffer,
            256,
            nullptr);

        formatResultMsg(result, buffer, tmpBuffer);
    }
}

#endif // _WIN32