
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

/**
 * @defgroup result_codes Result Codes
 * @brief Result codes extracted from result values.
 * @ingroup pal_core
 */

/**
 * @defgroup result_sources Result Sources
 * @brief Result sources extracted from result values.
 * @ingroup pal_core
 */

/** @brief The PAL function completed successfully.
 * 
 * This is the only value that can be checked directly with standard checks.
 * 
 * (eg. result == PAL_RESULT_SUCCESS).
 * 
 * @ingroup pal_core
*/
#define PAL_RESULT_SUCCESS 0

/** @brief No result code.
 * 
 * @ingroup result_codes
*/
#define PAL_RESULT_CODE_NONE 0

/** @brief Invalid parameter was used.
 * 
 * @ingroup result_codes
*/
#define PAL_RESULT_CODE_INVALID_ARGUMENT 1

/** @brief Memory allocation failed.
 * 
 * @ingroup result_codes
*/
#define PAL_RESULT_CODE_OUT_OF_MEMORY 2

/** @brief A platform specific error which is not known by PAL.
 * 
 * @ingroup result_codes
*/
#define PAL_RESULT_CODE_PLATFORM_FAILURE 3

/** @brief Timeout has occured.
 * 
 * @ingroup result_codes
*/
#define PAL_RESULT_CODE_TIMEOUT 4

/** @brief Invalid handle was used.
 * 
 * @ingroup result_codes
*/
#define PAL_RESULT_CODE_INVALID_HANDLE 5

/** @brief An unsupported feature was used.
 * 
 * @ingroup result_codes
*/
#define PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED 6

/** @brief The requested operation is invalid.
 * 
 * @ingroup result_codes
*/
#define PAL_RESULT_CODE_INVALID_OPERATION 7

/** @brief The device was lost.
 * 
 * @ingroup result_codes
*/
#define PAL_RESULT_CODE_DEVICE_LOST 8

/** @brief The handle is out of date.
 * 
 * @ingroup result_codes
*/
#define PAL_RESULT_CODE_OUT_OF_DATE 9

/** @brief The maximum number of result codes. The literal value must 
 * not be used.
 * 
 * @ingroup result_codes
*/
#define PAL_RESULT_CODE_COUNT 10

/** @brief No result source. The result native code should be ignored.
 * 
 * @ingroup result_sources
*/
#define PAL_RESULT_SOURCE_NONE 0

/** @brief The result native code is from GetLastError().
 * 
 * @ingroup result_sources
*/
#define PAL_RESULT_SOURCE_WIN32 1

/** @brief The result native code is from errno.
 * 
 * @ingroup result_sources
*/
#define PAL_RESULT_SOURCE_POSIX 2

/** @brief The result native code is from eglGetError().
 * 
 * @ingroup result_sources
*/
#define PAL_RESULT_SOURCE_EGL 3

/** @brief The result native code is from VkResult.
 * 
 * @ingroup result_sources
*/
#define PAL_RESULT_SOURCE_VULKAN 4

/** @brief The result native code is from HRESULT.
 * 
 * @ingroup result_sources
*/
#define PAL_RESULT_SOURCE_D3D12 5

/** @brief The result native code is from NSError.
 * 
 * @ingroup result_sources
*/
#define PAL_RESULT_SOURCE_METAL 6

/** @brief The maximum number of result sources. The literal value must
 * not be used.
 * 
 * @ingroup result_sources
*/
#define PAL_RESULT_SOURCE_COUNT 7

/**
 * @typedef PalResult
 * @brief Value returned by most PAL functions.
 *
 * This value constains the PAL result code, the result source and
 * the native code itself.
 * 
 * @since Added in version 2.0
 * @ingroup pal_core
 */
typedef uint64_t PalResult;

/**
 * @typedef PalResultCode
 * @brief Result code from a result value.
 *
 * The result code of a PAL_RESULT_SUCCESS value will always be
 * PAL_RESULT_CODE_NONE.
 *
 * All values of this type follow the format `PAL_RESULT_CODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palGetResultCode
 */
typedef uint16_t PalResultCode;

/**
 * @typedef PalResultSource
 * @brief Result source from a result value.
 * 
 * The result source of a PAL_RESULT_SUCCESS value will always be
 * PAL_RESULT_SOURCE_NONE.

 * All values of this type follow the format `PAL_RESULT_SOURCE_*` for API
 * consistency and ease of use.
 * 
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palGetResultSource
 */
typedef uint16_t PalResultSource;

/**
 * @brief Get the result code from the result value.
 *
 * @param[in] result The result value.
 * @return The result code from the result value.
 *
 * @Thread-safety Thread safe.
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
 * @brief Get the result source from the result value.
 *
 * @param[in] result The result value.
 * @return The result source from the result value.
 *
 * @Thread-safety Thread safe.
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
 * @brief Get the result native code from the result value.
 *
 * @param[in] result The result value.
 * @return The result native code from the result value.
 *
 * @Thread-safety Thread safe.
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
 * @brief Create a result value.
 *
 * If `nativeCode` paramter is not 0 and the result source is
 * PAL_RESULT_SOURCE_NONE, it will be ignored when formatting the result value.
 *
 * @param[in] code The result code.
 * @param[in] source The result source.
 * @param[in] nativeCode The result native code.
 * @return The created result value.
 *
 * @Thread-safety Thread safe.
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

/**
 * @brief Convert a result value to a human-readable string.
 *
 * @param[in] result The result value to format.
 * @param[in] bufferSize The size of the buffer. Must not be `0`.
 * @param[out] buffer The output buffer to write to. The string will be
 *                    truncated and null-terminated if the buffer size
 *                    is insufficient.
 *
 * @Thread-safety `buffer` parameter must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 */
PAL_API void PAL_CALL palFormatResult(
    PalResult result,
    uint64_t bufferSize,
    char* buffer);

#endif // PAL_CORE_RESULT_H