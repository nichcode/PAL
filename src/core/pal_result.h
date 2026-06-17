
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_RESULT_H
#define _PAL_RESULT_H

#include "pal_shared.h"
#include "pal_format.h"

static inline uint16_t getResultCode(PalResult result)
{
    return (uint16_t)(result & 0xFFFFU);
}

static inline uint16_t getResultSource(PalResult result)
{
    return (uint16_t)((result >> 16) & 0xFFFFu);
}

static inline uint32_t getResultNativeCode(PalResult result)
{
    return (uint32_t)(result >> 32);
}

static const char* PAL_CALL resultCodeToString(PalResult result)
{
    uint16_t code = getResultCode(result);
    switch (code) {
        case PAL_RESULT_SUCCESS:
            return "PAL_RESULT_SUCCESS";

        case PAL_RESULT_INVALID_ARGUMENT:
            return "PAL_RESULT_INVALID_ARGUMENT";

        case PAL_RESULT_OUT_OF_MEMORY:
            return "PAL_RESULT_OUT_OF_MEMORY";

        case PAL_RESULT_PLATFORM_FAILURE:
            return "PAL_RESULT_PLATFORM_FAILURE";

        case PAL_RESULT_TIMEOUT:
            return "PAL_RESULT_TIMEOUT";

        case PAL_RESULT_INVALID_HANDLE:
            return "PAL_RESULT_INVALID_HANDLE";

        case PAL_RESULT_FEATURE_NOT_SUPPORTED:
            return "PAL_RESULT_FEATURE_NOT_SUPPORTED";

        case PAL_RESULT_NOT_INITIALIZED:
            return "PAL_RESULT_NOT_INITIALIZED";

        case PAL_RESULT_INVALID_OPERATION:
            return "PAL_RESULT_INVALID_OPERATION";

        case PAL_RESULT_DEVICE_LOST:
            return "PAL_RESULT_DEVICE_LOST";

        case PAL_RESULT_OUT_OF_DATE:
            return "PAL_RESULT_OUT_OF_DATE";
    }

    return nullptr;
}

static const char* PAL_CALL resultCodeToDescription(PalResult result)
{
    uint16_t code = getResultCode(result);
    switch (code) {
        case PAL_RESULT_SUCCESS:
            return "Success";

        case PAL_RESULT_INVALID_ARGUMENT:
            return "Invalid argument";

        case PAL_RESULT_OUT_OF_MEMORY:
            return "Out of memory";

        case PAL_RESULT_PLATFORM_FAILURE:
            return "Platform failure";

        case PAL_RESULT_TIMEOUT:
            return "Timeout";

        case PAL_RESULT_INVALID_HANDLE:
            return "Invalid handle";

        case PAL_RESULT_FEATURE_NOT_SUPPORTED:
            return "Feature not supported";

        case PAL_RESULT_NOT_INITIALIZED:
            return "Not initialized";

        case PAL_RESULT_INVALID_OPERATION:
            return "Invalid operation";

        case PAL_RESULT_DEVICE_LOST:
            return "Device lost";

        case PAL_RESULT_OUT_OF_DATE:
            return "Out of date";
    }

    return nullptr;
}

static const char* PAL_CALL resultSourceToString(PalResult result)
{
    uint16_t source = getResultSource(result);
    switch (source) {
        case PAL_RESULT_SOURCE_WINDOWS:
            return "Windows";

        case PAL_RESULT_SOURCE_LINUX:
            return "Linux";

        case PAL_RESULT_SOURCE_VULKAN:
            return "Vulkan";

        case PAL_RESULT_SOURCE_D3D12:
            return "D3D12";
    }

    return nullptr;
}

static void formatResultMsg(PalResult result, char* buffer, char* msg)
{
    const char* baseString = resultCodeToString(result);
    const char* sourceString = resultSourceToString(result);
    const char* baseDescription = resultCodeToDescription(result);
    uint32_t nativeCode = getResultNativeCode(result);
    
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