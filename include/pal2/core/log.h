
/**
 * @file log.h
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

#ifndef PAL_CORE_LOG_H
#define PAL_CORE_LOG_H

#include "types.h"

/**
 * @brief Maximum size of the log messagge buffer in bytes including 
 * the NULL terminator.
 * 
 * @ingroup pal_core
 */
#define PAL_LOG_MSG_SIZE 4096

/**
 * @brief Log callback function.
 *
 * `msg` is only valid for the duration of the callback and must not be modified or freed
 * by the callback; the memory is owned by PAL.
 *
 * The callback may be called concurrently from multiple threads, so the implementation
 * must be thread safe if it will be used by multiple threads.
 *
 * The function signature should look like this:
 * @code
 * void PAL_CALL logCallback(void* userData, const char* msg);
 * @endcode
 *
 * @param[in] userData User-defined data passed to the callback or `nullptr`.
 * @param[in] msg Null-terminated UTF-8 string containing the log message.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
 * @sa palLog
 */
typedef void(PAL_CALL* PalLogCallback)(
    void* userData,
    const char* msg);

/**
 * @struct PalLogger
 * @brief Logger.
 *
 * The logger provides a way to intercept log messages made through the PAL log API.
 * The logger will not be copied, so it must remain valid for as long
 * as PAL may use it. The default logger is thread safe and owned by PAL.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 */
typedef struct PalLogger {
    PalLogCallback callback; /**< log callback */
    void* userData; /**< user-defined data passed to callback or `nullptr` */
} PalLogger;

/**
 * @brief Logs a formatted message.
 *
 * This function logs a formatted message to a custom or default logger.
 * Log messages are limited to @ref PAL_LOG_MSG_SIZE; messages exceeding this
 * limit are truncated and the remaining characters are discarded.
 *
 * Logging in a log callback with the default logger is valid, but logging
 * in a log callback with a custom logger triggers recursive logging. PAL
 * guards against this and the whole log message will be discarded.
 *
 * @param[in] logger Logger to use, or `nullptr` for the default.
 * @param[in] fmt printf-style format string.
 * @param[in] ... Arguments for the format string.
 *
 * @Thread-safety Thread safe, but log output and callbacks may be invoked concurrently.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
 * @sa palFormatResult
 */
PAL_API void PAL_CALL palLog(
    const PalLogger* logger,
    const char* fmt,
    ...);

#endif // PAL_CORE_LOG_H