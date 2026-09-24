
/**
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

#ifndef PAL_CORE_RESULT_H
#define PAL_CORE_RESULT_H

#include "defines.h"

#define PAL_RESULT_SUCCESS 0

/**
 * @defgroup result_codes Result Codes
 * @brief Result codes extracted from result values.
 * @ingroup pal_core
 * 
 * Each function defines its possible result codes and what it means.
 * A function which takes a version struct parameter might set 
 * `PAL_RESULT_CODE_INVALID_ARGUMENT` or `PAL_RESULT_CODE_INVALID_HANDLE`
 * as the result code of the result value if the parameter is not valid.
 * 
 * @{
 */
#define PAL_RESULT_CODE_NONE 0
#define PAL_RESULT_CODE_INVALID_ARGUMENT 1
#define PAL_RESULT_CODE_OUT_OF_MEMORY 2
#define PAL_RESULT_CODE_PLATFORM_FAILURE 3
#define PAL_RESULT_CODE_TIMEOUT 4
#define PAL_RESULT_CODE_INVALID_HANDLE 5
#define PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED 6
#define PAL_RESULT_CODE_INVALID_OPERATION 7
#define PAL_RESULT_CODE_DEVICE_LOST 8
#define PAL_RESULT_CODE_OUT_OF_DATE 9
#define PAL_RESULT_CODE_COUNT 10
/** @} */

/**
 * @defgroup result_sources Result Sources
 * @brief Result sources extracted from result values.
 * @ingroup pal_core
 * 
 * The result source shows where the native code was retrieved.
 * 
 * `PAL_RESULT_SOURCE_NONE` - There is no result source. @nl
 * `PAL_RESULT_SOURCE_WIN32` - The native code is from `GetLastError()`. @nl
 * `PAL_RESULT_SOURCE_POSIX` - The native code is from `errno`. @nl
 * `PAL_RESULT_SOURCE_EGL` - The native code is from `eglGetError()`. @nl
 * `PAL_RESULT_SOURCE_VULKAN` - The native code is from `VkResult`. @nl
 * `PAL_RESULT_SOURCE_D3D12` - The native code is from `HRESULT`. @nl
 * `PAL_RESULT_SOURCE_METAL` - The native code is from `NSError`.
 * 
 * @{
 */
#define PAL_RESULT_SOURCE_NONE 0
#define PAL_RESULT_SOURCE_WIN32 1
#define PAL_RESULT_SOURCE_POSIX 2
#define PAL_RESULT_SOURCE_EGL 3
#define PAL_RESULT_SOURCE_VULKAN 4
#define PAL_RESULT_SOURCE_D3D12 5
#define PAL_RESULT_SOURCE_METAL 6
#define PAL_RESULT_SOURCE_COUNT 7
/** @} */

/**
 * @typedef PalResult
 * @brief Value returned by most PAL functions.
 *
 * This value constains the PAL result code, the result source and
 * the native code itself. If a function completed successfully, it returns
 * `PAL_RESULT_SUCCESS`.
 * 
 * This is the only value that can be checked directly with standard checks.
 * 
 * (eg. result == `PAL_RESULT_SUCCESS`).
 * 
 * @since Added in version 2.0
 * @ingroup pal_core
 */
typedef uint64_t PalResult;

/**
 * @typedef PalResultCode
 * @brief Result codes from a result value.
 *
 * The result code of a `PAL_RESULT_SUCCESS` value will always be
 * `PAL_RESULT_CODE_NONE.`
 *
 * All values of this type follow the format `PAL_RESULT_CODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * @sa palGetResultCode
 */
typedef uint16_t PalResultCode;

/**
 * @typedef PalResultSource
 * @brief Result sources from a result value.
 * 
 * The result source of a `PAL_RESULT_SUCCESS` value will always be
 * `PAL_RESULT_SOURCE_NONE`.

 * All values of this type follow the format `PAL_RESULT_SOURCE_*` for API
 * consistency and ease of use.
 * 
 * @since Added in version 2.0
 * @ingroup pal_core
 * @sa palGetResultSource
 */
typedef uint16_t PalResultSource;

/**
 * @brief Gets the result code from the result value.
 *
 * @param[in] result The result value.
 *
 * @return The result code from the result value.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * @sa palGetResultSource
 * @sa palGetResultNativeCode
 */
static inline PalResultCode PAL_CALL palGetResultCode(PalResult result)
{
    return (uint16_t)(result & 0xFFFFU);
}

/**
 * @brief Gets the result source from the result value.
 *
 * @param[in] result The result value.
 *
 * @return The result source from the result value.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * @sa palGetResultCode
 * @sa palGetResultNativeCode
 */
static inline PalResultSource PAL_CALL palGetResultSource(PalResult result)
{
    return (uint16_t)((result >> 16) & 0xFFFFu);
}

/**
 * @brief Gets the result native code from the result value.
 *
 * @param[in] result The result value.
 *
 * @return The result native code from the result value.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * @sa palGetResultCode
 * @sa palGetResultSource
 */
static inline uint32_t PAL_CALL palGetResultNativeCode(PalResult result)
{
    return (uint32_t)(result >> 32);
}

/**
 * @brief Creates a result value.
 *
 * If `nativeCode` is not `0` and the result source is `PAL_RESULT_SOURCE_NONE`,
 * it will be ignored when formatting the result value.
 *
 * @param[in] code The result code.
 * @param[in] source The result source.
 * @param[in] nativeCode The result native code.
 *
 * @return The created result value.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * @sa palGetResultCode
 * @sa palGetResultSource
 * @sa palGetResultNativeCode
 */
static inline PalResult PAL_CALL palMakeResult(
    PalResultCode code,
    PalResultSource source,
    uint32_t nativeCode)
{
    return ((uint64_t)nativeCode << 32) | ((uint64_t)source << 16) |
           (uint64_t)code;
}

/**
 * @brief Converts a result value to a human-readable string.
 *
 * @param[in] result The result value to format.
 * @param[in] bufferSize The size of the buffer. Must not be `0`.
 * @param[out] buffer The output buffer to write to. The string will be
 * truncated and null-terminated if `bufferSize` is insufficient.
 *
 * @Thread-safety `buffer` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 */
PAL_API void PAL_CALL palFormatResult(
    PalResult result,
    uint64_t bufferSize,
    char* buffer);

#endif // PAL_CORE_RESULT_H