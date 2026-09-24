
/**
 * @file log.h
 * @brief This is the header file for PAL Log API.
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

#ifndef LOG_H
#define LOG_H

#include "defines.h"

#define PAL_LOG_MSG_SIZE 4096

/**
 * @brief Function pointer type used for log callbacks.
 * 
 * The function signature should look like this:
 * @code
 * void PAL_CALL logCallback(void* userData, const char* msg);
 * @endcode
 *
 * The message is only valid for the duration of the callback and must not be
 * modified or freed by the callback, the memory is owned by PAL.
 *
 * The callback may be called concurrently from multiple threads. The callback
 * must be thread safe if the same callback is used by multiple threads or a
 * seperate callback must be provided for each thread.
 *
 * @param userData User data passed from the logger. Can be `nullptr`.
 * @param msg Null-terminated UTF-8 log message.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * @sa palLog
 */
typedef void(PAL_CALL* PalLogCallback)(
    void* userData,
    const char* msg);

/**
 * @struct PalLogger
 * @brief Contains information about a logger.
 *
 * This struct provides a way to use custom loggers with PAL. This allows
 * interception of log messages made through the log API. The logger may
 * be called concurrently from multiple threads, therefore it must
 * be thread safe or each thread having its own logger.
 *
 * Uninitialized fields may result in undefined behavior.
 * 
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @var PalLogger::callback
 * The function to forward log messages to. Must not be `nullptr`.
 * 
 * @var PalLogger::userData
 * User data passed to the callback. Can be `nullptr`.
 */
typedef struct PalLogger 
{
    PalLogCallback callback;
    void* userData;
} PalLogger;

/**
 * @brief Logs a formatted message to a custom or default logger.
 * 
 * Log messages have a limit of `PAL_LOG_MSG_SIZE` (4096), any message greater
 * than the limit will be truncated and the remaining discarded.
 * 
 * Logging in a log callback with the default logger is valid. Double logging
 * with the same custom logger will trigger recursive logging. PAL guards
 * against this and will discard the whole message.
 *
 * @param[in] logger Logger instance. `nullptr` to use the default 
 * thread-safe logger.
 * @param[in] fmt printf-style format string.
 * @param[in] ... Arguments for the format string.
 *
 * @Thread-safety Thread safe, but log output and
 * callbacks may be invoked concurrently. The user must ensure the callback
 * implementation is thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * @sa palFormatResult
 */
PAL_API void PAL_CALL palLog(
    const PalLogger* logger,
    const char* fmt,
    ...);

#endif // LOG_H