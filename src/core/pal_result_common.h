
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_RESULT_COMMON_H
#define _PAL_RESULT_COMMON_H

#include "pal/pal_core.h"

#define PAL_RESULT_SOURCE_WINDOWS 100
#define PAL_RESULT_SOURCE_LINUX 101
#define PAL_RESULT_SOURCE_MACOS 102
#define PAL_RESULT_SOURCE_VULKAN 103
#define PAL_RESULT_SOURCE_D3D12 104
#define PAL_RESULT_SOURCE_METAL 105

static inline PalResult makeResult(
    uint16_t code, 
    uint16_t source, 
    uint32_t nativeCode)
{
    return ((uint64_t)nativeCode << 32) | ((uint64_t)source << 16) | (uint64_t)code;
}

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

        case PAL_RESULT_SOURCE_MACOS:
            return "MacOS";

        case PAL_RESULT_SOURCE_VULKAN:
            return "Vulkan";

        case PAL_RESULT_SOURCE_D3D12:
            return "D3D12";

        case PAL_RESULT_SOURCE_METAL:
            return "Metal";
    }

    return nullptr;
}

#endif // _PAL_RESULT_COMMON_H