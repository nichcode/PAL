
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_RESULT_H
#define _PAL_RESULT_H

#include "pal_format.h"

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

        case PAL_RESULT_CODE_NOT_INITIALIZED:
            return "PAL_RESULT_CODE_NOT_INITIALIZED";

        case PAL_RESULT_CODE_INVALID_OPERATION:
            return "PAL_RESULT_CODE_INVALID_OPERATION";

        case PAL_RESULT_CODE_DEVICE_LOST:
            return "PAL_RESULT_CODE_DEVICE_LOST";

        case PAL_RESULT_CODE_OUT_OF_DATE:
            return "PAL_RESULT_CODE_OUT_OF_DATE";

        case PAL_RESULT_CODE_INVALID_DRIVER:
            return "PAL_RESULT_CODE_INVALID_DRIVER";
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

        case PAL_RESULT_CODE_NOT_INITIALIZED:
            return "Not initialized";

        case PAL_RESULT_CODE_INVALID_OPERATION:
            return "Invalid operation";

        case PAL_RESULT_CODE_DEVICE_LOST:
            return "Device lost";

        case PAL_RESULT_CODE_OUT_OF_DATE:
            return "Out of date";

        case PAL_RESULT_CODE_INVALID_DRIVER:
            return "Invalid driver";
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
            return "DIRECTX12";

        case PAL_RESULT_SOURCE_METAL:
            return "METAL";
    }

    return "NONE";
}

static void formatResultMsg(PalResult result, char* buffer, char* msg)
{
    const char* baseString = resultCodeToString(result);
    const char* sourceString = resultSourceToString(result);
    const char* baseDescription = resultCodeToDescription(result);
    uint32_t nativeCode = palGetResultNativeCode(result);
    
    const char* description = "";
    if (msg) {
        description = msg;
    }

    format(
        buffer, 
        "Source: %s\n PAL Code: %s\n Native Code: 0x%08x\n PAL Description: %s\n Native Description: %s",
        sourceString, 
        baseString,
        nativeCode, 
        baseDescription,
        description);
}

#endif // _PAL_RESULT_H