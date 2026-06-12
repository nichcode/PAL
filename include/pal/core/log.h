
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup log Log section
 * @ingroup pal_core
 * @{
 */

#ifndef _CORE_LOG_H
#define _CORE_LOG_H

#include "defines.h"

/**
 * @typedef PalLogCallback
 * @brief Function pointer type used for log callbacks.
 *
 * @param userData Optional pointer to user data passed from ::PalLogger. Can be nullptr.
 * @param msg Null-terminated UTF-8 log message.
 *
 * @since 1.0
 * @sa palLog
 */
typedef void(PAL_CALL* PalLogCallback)(
    void* userData,
    const char* msg);

/**
 * @struct PalLogger
 * @brief Logging configuration.
 *
 * Provides a callback and user data for handling log messages.
 *
 * @since 1.0
 */
typedef struct {
    PalLogCallback callback;
    void* userData; /** Optional user-provided data. Can be nullptr.*/
} PalLogger;

/**
 * Log a formatted message.
 *
 * @param logger Logger instance. Set to nullptr to use default logger.
 * @param fmt printf-style format string.
 * @param ... Arguments for the format string.
 *
 * Thread safety: Thread safe, but log output and
 * callbacks may be invoked concurrently. The user must ensure the callback
 * implementation is thread safe.
 *
 * @since 1.0
 * @sa palFormatResult
 */
PAL_API void PAL_CALL palLog(
    const PalLogger* logger,
    const char* fmt,
    ...);

#endif // _CORE_LOG_H

/** @} */