
/**
 * @file result.h
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

#ifndef PAL_CORE_RESULT_H
#define PAL_CORE_RESULT_H

#include "base.h"

/**
 * @typedef PalResult
 * @brief Value returned by most PAL functions.
 *
 * Contains the result code and, optionally, the result source and the native code.
 * The result source identifies the origin of the native code.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 */
typedef uint64_t PalResult;

/**
 * @typedef PalResultCode
 * @brief PAL result code.
 *
 * All values of this type follow the format `PAL_RESULT_CODE_*` for API consistency
 * and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 */
typedef uint16_t PalResultCode;

/**
 * @typedef PalResultSource
 * @brief PAL result source.
 *
 * All values of this type follow the format `PAL_RESULT_SOURCE_*` for API consistency
 * and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 */
typedef uint16_t PalResultSource;

/**
 * @brief Function completed successfully result value.
 * @ingroup pal_core
 */
#define PAL_RESULT_SUCCESS 0

/**
 * @defgroup result_codes Result Codes
 * @ingroup pal_core
 * @{
 */
#define PAL_RESULT_CODE_NONE 0 /**< Non-error result code. */
#define PAL_RESULT_CODE_INVALID_ARGUMENT 1 /**< Invalid argument was passed. */
#define PAL_RESULT_CODE_OUT_OF_MEMORY 2 /**< Memory allocation failed. */
#define PAL_RESULT_CODE_PLATFORM_FAILURE 3 /**< Platform-specific error not known to PAL. */
#define PAL_RESULT_CODE_TIMEOUT 4 /**< Timeout occurred. */
#define PAL_RESULT_CODE_INVALID_HANDLE 5 /**< Invalid handle was passed. */
#define PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED 6 /**< Unsupported feature was used. */
#define PAL_RESULT_CODE_INVALID_OPERATION 7 /**< Invalid operation was performed. */
#define PAL_RESULT_CODE_DEVICE_LOST 8 /**< Device was lost. */
#define PAL_RESULT_CODE_OUT_OF_DATE 9 /**< Handle is out of date. */
#define PAL_RESULT_CODE_COUNT 10 /**< Number of result codes. */
/** @} */

/**
 * @defgroup result_sources Result Sources
 * @ingroup pal_core
 * @{
 */
#define PAL_RESULT_SOURCE_NONE 0 /**< No result source. */
#define PAL_RESULT_SOURCE_WIN32 1 /**< Win32 `GetLastError()` code. */
#define PAL_RESULT_SOURCE_POSIX 2 /**< POSIX `errno` code. */
#define PAL_RESULT_SOURCE_EGL 3 /**< EGL `eglGetError()` code. */
#define PAL_RESULT_SOURCE_VULKAN 4 /**< Vulkan `VkResult` code. */
#define PAL_RESULT_SOURCE_D3D12 5 /**< Direct3D 12 `HRESULT` code. */
#define PAL_RESULT_SOURCE_METAL 6 /**< Metal `NSError` code. */
#define PAL_RESULT_SOURCE_COUNT 7 /**< Number of result sources. */
/** @} */

/**
 * @brief Converts a result value to a human-readable string.
 *
 * This function converts a result value into a null-terminated UTF-8 encoded string.
 * The converted string will be truncated if `buffer` is too small.
 *
 * @param[in] result Result value.
 * @param[in] bufferSize Size of `buffer` in bytes.
 * @param[out] buffer Output buffer to write to.
 *
 * @thread-safety `buffer` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 */
PAL_API void PAL_CALL palFormatResult(
    PalResult result,
    uint64_t bufferSize,
    char* buffer);

/**
 * @brief Gets the result code from the specified result value.
 *
 * @param[in] result Result value.
 * @return Result code.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
 * @sa palGetResultSource
 * @sa palGetResultNativeCode
 */
static inline PalResultCode PAL_CALL palGetResultCode(PalResult result)
{
    return (uint16_t)(result & 0xFFFFU);
}

/**
 * @brief Gets the result source from the specified result value.
 *
 * @param[in] result Result value.
 * @return Result source.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
 * @sa palGetResultCode
 * @sa palGetResultNativeCode
 */
static inline PalResultSource PAL_CALL palGetResultSource(PalResult result)
{
    return (uint16_t)((result >> 16) & 0xFFFFu);
}

/**
 * @brief Gets the result native code from the specified result value.
 *
 * @param[in] result Result value.
 * @return Result native code.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
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
 * This function creates a result value with the specified result code, result
 * source and native code. Creating a result value with the
 * result code, result source and native code set to @ref PAL_RESULT_CODE_NONE,
 * @ref PAL_RESULT_SOURCE_NONE and `0` respectively creates a result value
 * which is equal to @ref PAL_RESULT_SUCCESS.
 *
 * @param[in] code Result code.
 * @param[in] source Result source.
 * @param[in] nativeCode Native code.
 * @return Created result value.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
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

#endif // PAL_CORE_RESULT_H