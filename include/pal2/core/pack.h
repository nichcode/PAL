
/**
 * @file pack.h
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

#ifndef PAL_CORE_PACK_H
#define PAL_CORE_PACK_H

#include "types.h"
#include <string.h>

/**
 * @brief Combines two 32-bit unsigned integers into a single 64-bit unsigned integer.
 *
 * @param[in] low Low 32-bit unsigned integer.
 * @param[in] high High 32-bit unsigned integer.
 * @return Combined 64-bit unsigned integer.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
 * @sa palUnpackUint32
 */
static inline uint64_t PAL_CALL palPackUint32(
    uint32_t low,
    uint32_t high)
{
    return (uint64_t)(((uint64_t)high << 32) | (uint64_t)low);
}

/**
 * @brief Combines two 32-bit signed integers into a single 64-bit unsigned integer.
 *
 * @param[in] low Low 32-bit signed integer.
 * @param[in] high High 32-bit signed integer.
 * @return Combined 64-bit unsigned integer.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
 * @sa palUnpackInt32
 */
static inline uint64_t PAL_CALL palPackInt32(
    int32_t low,
    int32_t high)
{
    return ((uint64_t)(uint32_t)high << 32) | (uint32_t)low;
}

/**
 * @brief Packs a pointer into a 64-bit unsigned integer.
 *
 * @param[in] ptr Pointer to pack.
 * @return Packed 64-bit unsigned integer.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
 * @sa palUnpackPointer
 */
static inline uint64_t PAL_CALL palPackPointer(void* ptr)
{
    return (uint64_t)(uintptr_t)ptr;
}

/**
 * @brief Combines two floats into a single 64-bit unsigned integer.
 *
 * @param[in] low Low float value.
 * @param[in] high High float value.
 * @return Combined 64-bit unsigned integer.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
 * @sa palUnpackFloat
 */
static inline uint64_t PAL_CALL palPackFloat(
    float low,
    float high)
{
    uint64_t combined = 0;
#if PAL_BIG_ENDIAN
    memcpy(&combined, &high, sizeof(float));
    memcpy((char*)&combined + sizeof(float), &low, sizeof(float));
#else
    memcpy(&combined, &low, sizeof(float));
    memcpy((char*)&combined + sizeof(float), &high, sizeof(float));
#endif // PAL_BIG_ENDIAN

    return combined;
}

/**
 * @brief Retrieves two 32-bit unsigned integers from a 64-bit unsigned integer.
 *
 * @param[in] data 64-bit unsigned integer.
 * @param[out] low Output to receive the low value, or `nullptr` to ignore it.
 * @param[out] high Output to receive the high value, or `nullptr` to ignore it.
 *
 * @thread-safety `low` and `high` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
 * @sa palPackUint32
 */
static inline void PAL_CALL palUnpackUint32(
    uint64_t data,
    uint32_t* low,
    uint32_t* high)
{
    if (low) {
        *low = (uint32_t)(data & 0xFFFFFFFF);
    }

    if (high) {
        *high = (uint32_t)(data >> 32);
    }
}

/**
 * @brief Retrieves two 32-bit signed integers from a 64-bit unsigned integer.
 *
 * @param[in] data 64-bit unsigned integer.
 * @param[out] low Output to receive the low value, or `nullptr` to ignore it.
 * @param[out] high Output to receive the high value, or `nullptr` to ignore it.
 *
 * @thread-safety `low` and `high` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
 * @sa palPackInt32
 */
static inline void PAL_CALL palUnpackInt32(
    uint64_t data,
    int32_t* low,
    int32_t* high)
{
    if (low) {
        *low = (int32_t)(data & 0xFFFFFFFF);
    }

    if (high) {
        *high = (int32_t)(data >> 32);
    }
}

/**
 * @brief Unpacks a pointer from a 64-bit unsigned integer.
 *
 * @param[in] data 64-bit unsigned integer.
 * @return Pointer unpacked from the 64-bit unsigned integer.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
 * @sa palPackPointer
 */
static inline void* PAL_CALL palUnpackPointer(uint64_t data)
{
    return (void*)(uintptr_t)data;
}

/**
 * @brief Retrieves two floats from a 64-bit unsigned integer.
 *
 * @param[in] data 64-bit unsigned integer.
 * @param[out] low Output to receive the low value, or `nullptr` to ignore it.
 * @param[out] high Output to receive the high value, or `nullptr` to ignore it.
 *
 * @thread-safety `low` and `high` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
 * @sa palPackFloat
 */
static inline void PAL_CALL palUnpackFloat(
    uint64_t data,
    float* low,
    float* high)
{
#if PAL_BIG_ENDIAN
    if (low) {
        memcpy(low, (char*)&data + sizeof(float), sizeof(float));
    }

    if (high) {
        memcpy(high, &data, sizeof(float));
    }
#else
    if (low) {
        memcpy(low, &data, sizeof(float));
    }

    if (high) {
        memcpy(high, (char*)&data + sizeof(float), sizeof(float));
    }

#endif // PAL_BIG_ENDIAN
}

#endif // PAL_CORE_PACK_H