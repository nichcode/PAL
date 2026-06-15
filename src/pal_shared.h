
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_SHARED_H
#define _PAL_SHARED_H

#include "pal/pal_core.h"

#define PAL_RESULT_SOURCE_WINDOWS 100
#define PAL_RESULT_SOURCE_LINUX 101
#define PAL_RESULT_SOURCE_VULKAN 102
#define PAL_RESULT_SOURCE_D3D12 103

static inline PalResult palMakeResult(
    uint16_t code, 
    uint16_t source, 
    uint32_t nativeCode)
{
    return ((uint64_t)nativeCode << 32) | ((uint64_t)source << 16) | (uint64_t)code;
}

#endif // _PAL_SHARED_H