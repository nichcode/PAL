
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup pack Packing and unpack helpers section
 * @ingroup pal_core
 * @{
 */

#ifndef _CORE_PACK_H
#define _CORE_PACK_H

#include "defines.h"
#include <string.h>

/**
 * @brief Combine two 32-bit unsigned integers into a single 64-bit signed
 * integer.
 *
 * @return The combined 64-bit signed integer.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palUnpackUint32
 */
static inline int64_t PAL_CALL palPackUint32(
    uint32_t low,
    uint32_t high)
{
    return (int64_t)(((uint64_t)high << 32) | (uint64_t)low);
}

/**
 * @brief Combine two 32-bit signed integers into a single 64-bit signed
 * integer.
 *
 * @return The combined 64-bit signed integer.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palUnpackInt32
 */
static inline int64_t PAL_CALL palPackInt32(
    int32_t low,
    int32_t high)
{
    return ((int64_t)(uint32_t)high << 32) | (uint32_t)low;
}

/**
 * @brief Pack a pointer into a 64-bit signed integer.
 *
 * @return The packed 64-bit signed integer.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palUnpackPointer
 */
static inline int64_t PAL_CALL palPackPointer(void* ptr)
{
    return (int64_t)(uintptr_t)ptr;
}

/**
 * @brief Combine two floats into a single 64-bit signed integer.
 *
 * @return The combined 64-bit signed integer.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.3
 * @sa palUnpackFloat
 */
static inline int64_t PAL_CALL palPackFloat(
    float low,
    float high)
{
    int64_t combined = 0;
#if PAL_BIG_ENDIAN
    memcpy(&((uint32_t*)&combined)[0], &high, sizeof(float));
    memcpy(&((uint32_t*)&combined)[1], &low, sizeof(float));
#else
    memcpy(&((uint32_t*)&combined)[0], &low, sizeof(float));
    memcpy(&((uint32_t*)&combined)[1], &high, sizeof(float));
#endif // PAL_BIG_ENDIAN

    return combined;
}

/**
 * @brief Retrieve two 32-bit unsigned integers from a 64-bit signed integer.
 *
 * @param[out] outLow Low value of the 64-bit signed integer.
 * @param[out] outHigh High value of the 64-bit signed integer.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palPackUint32
 */
static inline void PAL_CALL palUnpackUint32(
    int64_t data,
    uint32_t* outLow,
    uint32_t* outHigh)
{
    if (outLow) {
        *outLow = (uint32_t)(data & 0xFFFFFFFF);
    }

    if (outHigh) {
        *outHigh = (uint32_t)((uint64_t)data >> 32);
    }
}

/**
 * @brief Retrieve two 32-bit signed integers from a 64-bit signed integer.
 *
 * @param[out] outLow Low value of the 64-bit signed integer.
 * @param[out] outHigh High value of the 64-bit signed integer.
 *
 * Thread safety: Thread-safe if `outLow` and `outHigh` are
 * thread local.
 *
 * @since 1.0
 * @sa palPackInt32
 */
static inline void PAL_CALL palUnpackInt32(
    int64_t data,
    int32_t* outLow,
    int32_t* outHigh)
{
    if (outLow) {
        *outLow = (int32_t)(data & 0xFFFFFFFF);
    }

    if (outHigh) {
        *outHigh = (int32_t)((uint64_t)data >> 32);
    }
}

/**
 * @brief Unpack a pointer from a 64-bit signed integer.
 *
 * @return The pointer from the 64-bit signed integer.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palPackPointer
 */
static inline void* PAL_CALL palUnpackPointer(int64_t data)
{
    return (void*)(uintptr_t)data;
}

/**
 * @brief Retrieve two floats from a 64-bit signed integer.
 *
 * @param[out] outLow Low value of the 64-bit signed integer.
 * @param[out] outHigh High value of the 64-bit signed integer.
 *
 * Thread safety: Thread-safe if `outLow` and `outHigh` are
 * thread local.
 *
 * @since 1.3
 * @sa palPackFloat
 */
static inline void PAL_CALL palUnpackFloat(
    int64_t data,
    float* low,
    float* high)
{
#if PAL_BIG_ENDIAN
    if (low) {
        memcpy(low, &((uint32_t*)&data)[1], sizeof(float));
    }

    if (high) {
        memcpy(high, &((uint32_t*)&data)[0], sizeof(float));
    }
#else
    if (low) {
        memcpy(low, &((uint32_t*)&data)[0], sizeof(float));
    }

    if (high) {
        memcpy(high, &((uint32_t*)&data)[1], sizeof(float));
    }

#endif // PAL_BIG_ENDIAN
}

#endif // _CORE_PACK_H

/** @} */