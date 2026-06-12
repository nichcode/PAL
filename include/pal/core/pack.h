
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup pack Packing and unpacking helpers
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
static inline Int64 PAL_CALL palPackUint32(
    Uint32 low,
    Uint32 high)
{
    return (Int64)(((Uint64)high << 32) | (Uint64)low);
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
static inline Int64 PAL_CALL palPackInt32(
    Int32 low,
    Int32 high)
{
    return ((Int64)(Uint32)high << 32) | (Uint32)low;
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
static inline Int64 PAL_CALL palPackPointer(void* ptr)
{
    return (Int64)(UintPtr)ptr;
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
static inline Int64 PAL_CALL palPackFloat(
    float low,
    float high)
{
    Int64 combined = 0;
#if PAL_BIG_ENDIAN
    memcpy(&((Uint32*)&combined)[0], &high, sizeof(float));
    memcpy(&((Uint32*)&combined)[1], &low, sizeof(float));
#else
    memcpy(&((Uint32*)&combined)[0], &low, sizeof(float));
    memcpy(&((Uint32*)&combined)[1], &high, sizeof(float));
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
    Int64 data,
    Uint32* outLow,
    Uint32* outHigh)
{
    if (outLow) {
        *outLow = (Uint32)(data & 0xFFFFFFFF);
    }

    if (outHigh) {
        *outHigh = (Uint32)((Uint64)data >> 32);
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
    Int64 data,
    Int32* outLow,
    Int32* outHigh)
{
    if (outLow) {
        *outLow = (Int32)(data & 0xFFFFFFFF);
    }

    if (outHigh) {
        *outHigh = (Int32)((Uint64)data >> 32);
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
static inline void* PAL_CALL palUnpackPointer(Int64 data)
{
    return (void*)(UintPtr)data;
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
    Int64 data,
    float* low,
    float* high)
{
#if PAL_BIG_ENDIAN
    if (low) {
        memcpy(low, &((Uint32*)&data)[1], sizeof(float));
    }

    if (high) {
        memcpy(high, &((Uint32*)&data)[0], sizeof(float));
    }
#else
    if (low) {
        memcpy(low, &((Uint32*)&data)[0], sizeof(float));
    }

    if (high) {
        memcpy(high, &((Uint32*)&data)[1], sizeof(float));
    }

#endif // PAL_BIG_ENDIAN
}

#endif // _CORE_PACK_H

/** @} */