
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

#include "shared.h"
#include "core_platform.h"

static const char* resultCodeToString(PalResult result)
{
    PalResultCode code = palGetResultCode(result);
    switch (code) {
    case PAL_RESULT_CODE_INVALID_ARGUMENT:
        return "PAL_RESULT_CODE_INVALID_ARGUMENT";

    case PAL_RESULT_CODE_OUT_OF_MEMORY:
        return "PAL_RESULT_CODE_OUT_OF_MEMORY";

    case PAL_RESULT_CODE_PLATFORM_FAILURE:
        return "PAL_RESULT_CODE_PLATFORM_FAILURE";

    case PAL_RESULT_CODE_TIMEOUT:
        return "PAL_RESULT_CODE_TIMEOUT";

    case PAL_RESULT_CODE_INVALID_HANDLE:
        return "PAL_RESULT_CODE_INVALID_HANDLE";

    case PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED:
        return "PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED";

    case PAL_RESULT_CODE_INVALID_OPERATION:
        return "PAL_RESULT_CODE_INVALID_OPERATION";

    case PAL_RESULT_CODE_DEVICE_LOST:
        return "PAL_RESULT_CODE_DEVICE_LOST";

    case PAL_RESULT_CODE_OUT_OF_DATE:
        return "PAL_RESULT_CODE_OUT_OF_DATE";
    }

    return "";
}

static const char* resultCodeToDescription(PalResult result)
{
    PalResultCode code = palGetResultCode(result);
    switch (code) {
    case PAL_RESULT_CODE_INVALID_ARGUMENT:
        return "Invalid argument";

    case PAL_RESULT_CODE_OUT_OF_MEMORY:
        return "Out of memory";

    case PAL_RESULT_CODE_PLATFORM_FAILURE:
        return "Platform failure";

    case PAL_RESULT_CODE_TIMEOUT:
        return "Timeout";

    case PAL_RESULT_CODE_INVALID_HANDLE:
        return "Invalid handle";

    case PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED:
        return "Feature not supported";

    case PAL_RESULT_CODE_INVALID_OPERATION:
        return "Invalid operation";

    case PAL_RESULT_CODE_DEVICE_LOST:
        return "Device lost";

    case PAL_RESULT_CODE_OUT_OF_DATE:
        return "Out of date";
    }

    return nullptr;
}

static const char* resultSourceToString(PalResult result)
{
    PalResultSource source = palGetResultSource(result);
    switch (source) {
    case PAL_RESULT_SOURCE_WIN32:
        return "WIN32";

    case PAL_RESULT_SOURCE_POSIX:
        return "POSIX";

    case PAL_RESULT_SOURCE_EGL:
        return "EGL";

    case PAL_RESULT_SOURCE_VULKAN:
        return "VULKAN";

    case PAL_RESULT_SOURCE_D3D12:
        return "D3D12";

    case PAL_RESULT_SOURCE_METAL:
        return "METAL";
    }

    return "NONE";
}

void PAL_CALL palFormatResult(
    PalResult result,
    uint64_t bufferSize,
    char* buffer)
{
    const char* baseString = resultCodeToString(result);
    const char* sourceString = resultSourceToString(result);
    const char* baseDescription = resultCodeToDescription(result);
    uint32_t nativeCode = palGetResultNativeCode(result);

    const char* description = "\n";
    char tmp[FORMAT_BUFFER_SIZE];
    memset(tmp, 0, FORMAT_BUFFER_SIZE);

    char formatted[FORMAT_BUFFER_SIZE * 2];
    memset(formatted, 0, FORMAT_BUFFER_SIZE * 2);

    corePlatformFormatResult(result, tmp);
    if (tmp[0] != 0) {
        description = tmp;
    }

    formatMsg(
        formatted,
        FORMAT_BUFFER_SIZE,
        "Source: %s\n PAL Code: %s\n Native Code: 0x%08X\n PAL Description: "
        "%s\n Native "
        "Description: %s",
        sourceString,
        baseString,
        nativeCode,
        baseDescription,
        description);

    (void)strncpy(buffer, formatted, bufferSize);
}