/**
 * @file init.h
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

#ifndef PAL_GRAPHICS_FORMAT_H
#define PAL_GRAPHICS_FORMAT_H

#include "types.h"

/**
 * @defgroup sample_counts Sample Counts
 * @ingroup pal_graphics
 * @{
 */
#define PAL_SAMPLE_COUNT_1 0
#define PAL_SAMPLE_COUNT_2 1
#define PAL_SAMPLE_COUNT_4 2
#define PAL_SAMPLE_COUNT_8 3
#define PAL_SAMPLE_COUNT_16 4
#define PAL_SAMPLE_COUNT_32 5
#define PAL_SAMPLE_COUNT_64 6
#define PAL_SAMPLE_COUNT_COUNT 7 /**< Number of sample counts. */
/** @} */

/**
 * @defgroup formats Formats
 * @ingroup pal_graphics
 * @{
 */
#define PAL_FORMAT_UNDEFINED 0 /**< Invalid format. */
#define PAL_FORMAT_R8_UNORM 1
#define PAL_FORMAT_R8_SNORM 2
#define PAL_FORMAT_R8_UINT 3
#define PAL_FORMAT_R8_SINT 4
#define PAL_FORMAT_R8_SRGB 5
#define PAL_FORMAT_R16_UNORM 6
#define PAL_FORMAT_R16_SNORM 7
#define PAL_FORMAT_R16_UINT 8
#define PAL_FORMAT_R16_SINT 9
#define PAL_FORMAT_R16_SFLOAT 10
#define PAL_FORMAT_R32_UINT 11
#define PAL_FORMAT_R32_SINT 12
#define PAL_FORMAT_R32_SFLOAT 13
#define PAL_FORMAT_R64_UINT 14
#define PAL_FORMAT_R64_SINT 15
#define PAL_FORMAT_R64_SFLOAT 16
#define PAL_FORMAT_R8G8_UNORM 17
#define PAL_FORMAT_R8G8_SNORM 18
#define PAL_FORMAT_R8G8_UINT 19
#define PAL_FORMAT_R8G8_SINT 20
#define PAL_FORMAT_R8G8_SRGB 21
#define PAL_FORMAT_R16G16_UNORM 22
#define PAL_FORMAT_R16G16_SNORM 23
#define PAL_FORMAT_R16G16_UINT 24
#define PAL_FORMAT_R16G16_SINT 25
#define PAL_FORMAT_R16G16_SFLOAT 26
#define PAL_FORMAT_R32G32_UINT 27
#define PAL_FORMAT_R32G32_SINT 28
#define PAL_FORMAT_R32G32_SFLOAT 29
#define PAL_FORMAT_R64G64_UINT 30
#define PAL_FORMAT_R64G64_SINT 31
#define PAL_FORMAT_R64G64_SFLOAT 32
#define PAL_FORMAT_R8G8B8_UNORM 33
#define PAL_FORMAT_R8G8B8_SNORM 34
#define PAL_FORMAT_R8G8B8_UINT 35
#define PAL_FORMAT_R8G8B8_SINT 36
#define PAL_FORMAT_R8G8B8_SRGB 37
#define PAL_FORMAT_R16G16B16_UNORM 38
#define PAL_FORMAT_R16G16B16_SNORM 39
#define PAL_FORMAT_R16G16B16_UINT 40
#define PAL_FORMAT_R16G16B16_SINT 41
#define PAL_FORMAT_R16G16B16_SFLOAT 42
#define PAL_FORMAT_R32G32B32_UINT 43
#define PAL_FORMAT_R32G32B32_SINT 44
#define PAL_FORMAT_R32G32B32_SFLOAT 45
#define PAL_FORMAT_R64G64B64_UINT 46
#define PAL_FORMAT_R64G64B64_SINT 47
#define PAL_FORMAT_R64G64B64_SFLOAT 48
#define PAL_FORMAT_B8G8R8_UNORM 49
#define PAL_FORMAT_B8G8R8_SNORM 50
#define PAL_FORMAT_B8G8R8_UINT 51
#define PAL_FORMAT_B8G8R8_SINT 52
#define PAL_FORMAT_B8G8R8_SRGB 53
#define PAL_FORMAT_R8G8B8A8_UNORM 54
#define PAL_FORMAT_R8G8B8A8_SNORM 55
#define PAL_FORMAT_R8G8B8A8_UINT 56
#define PAL_FORMAT_R8G8B8A8_SINT 57
#define PAL_FORMAT_R8G8B8A8_SRGB 58
#define PAL_FORMAT_R16G16B16A16_UNORM 59
#define PAL_FORMAT_R16G16B16A16_SNORM 60
#define PAL_FORMAT_R16G16B16A16_UINT 61
#define PAL_FORMAT_R16G16B16A16_SINT 62
#define PAL_FORMAT_R16G16B16A16_SFLOAT 63
#define PAL_FORMAT_R32G32B32A32_UINT 64
#define PAL_FORMAT_R32G32B32A32_SINT 65
#define PAL_FORMAT_R32G32B32A32_SFLOAT 66
#define PAL_FORMAT_R64G64B64A64_UINT 67
#define PAL_FORMAT_R64G64B64A64_SINT 68
#define PAL_FORMAT_R64G64B64A64_SFLOAT 69
#define PAL_FORMAT_B8G8R8A8_UNORM 70
#define PAL_FORMAT_B8G8R8A8_SNORM 71
#define PAL_FORMAT_B8G8R8A8_UINT 72
#define PAL_FORMAT_B8G8R8A8_SINT 73
#define PAL_FORMAT_B8G8R8A8_SRGB 74
#define PAL_FORMAT_S8_UINT 75
#define PAL_FORMAT_D16_UNORM 76
#define PAL_FORMAT_D32_SFLOAT 77
#define PAL_FORMAT_D16_UNORM_S8_UINT 78
#define PAL_FORMAT_D32_SFLOAT_S8_UINT 79
#define PAL_FORMAT_D24_UNORM_S8_UINT 80
#define PAL_FORMAT_COUNT 81 /**< Number of formats. */
/** @} */

/**
 * @struct PalFormatInfo
 * @brief Format information.
 *
 * Filled in by @ref palEnumerateFormats().
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalFormatInfo {
    PalImageUsages usages; /**< Bitmask of supported image usages. */
    PalFormat format; /**< Format being described. */
    PalSampleCount sampleCount; /**< Supported sample counts. */
} PalFormatInfo;

/**
 * @brief Enumerates the formats supported by an adapter.
 *
 * This function returns each supported format together with the image usages supported for that format.
 * Use this to select a format based on the required image usages. Use @ref palIsFormatSupported() to check
 * a specific format.
 *
 * This is a two-call function, set `formats` to `nullptr` and `count` to `0` to get the number of
 * supported formats. Allocate the array and call this function again with `formats` set to the allocated
 * array and `count` set to the capacity of the array.
 *
 * If the specified count is less than the number of supported formats, only the formats that fit in the
 * array will be written. The format information is copied into the array.
 *
 * @param[in] adapter Adapter to query its formats.
 * @param[in, out] count Capacity of the format array.
 * @param[out] formats Output array to receive the formats, or `nullptr` to get the number of formats.
 *
 * @thread-safety `formats` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palIsFormatSupported
 */
PAL_API void PAL_CALL palEnumerateFormats(
    PalAdapter* adapter,
    uint32_t* count,
    PalFormatInfo* formats);

/**
 * @brief Checks whether an adapter supports a format.
 *
 * Use this when a specific format is already preferred instead of enumerating every format.
 * If the format is supported, use @ref palQueryFormatImageUsages() to determine which image
 * usages are supported for it.
 *
 * @param[in] adapter Adapter to query.
 * @param[in] format Format to query support for.
 * @return `PAL_TRUE` if the format is supported otherwise `PAL_FALSE`.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 *
 * @sa palQueryFormatImageUsages
 */
PAL_API PalBool PAL_CALL palIsFormatSupported(
    PalAdapter* adapter,
    PalFormat format);

/**
 * @brief Gets the image usages supported for a format.
 *
 * @param[in] adapter Adapter to query.
 * @param[in] format Format to query image usages for.
 * @return Bitmask of image usages supported for the format or `0` if the format is unsupported.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalImageUsages PAL_CALL palQueryFormatImageUsages(
    PalAdapter* adapter,
    PalFormat format);

/**
 * @brief Gets the sample counts supported for a format.
 *
 * @param[in] adapter Adapter to query format on.
 * @param[in] format Format to query sample count for.
 * @return Bitmask of supported sample counts, or `0` if the format is unsupported.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
PAL_API PalSampleCount PAL_CALL palQueryFormatSampleCount(
    PalAdapter* adapter,
    PalFormat format);

#endif // PAL_GRAPHICS_FORMAT_H