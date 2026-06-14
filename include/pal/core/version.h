
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup version Version section
 * @ingroup pal_core
 * @{
 */

#ifndef _CORE_VERSION_H
#define _CORE_VERSION_H

#include "defines.h"

/**
 * @struct PalVersion
 * @brief Describes the version of PAL.
 *
 * @since 1.0
 */
typedef struct {
    uint32_t major; /**< Major version (breaking changes).*/
    uint32_t minor; /**< Minor version (adding features).*/
    uint32_t build; /**< Build version (bug fixes).*/
} PalVersion;

/**
 * Retrieve the PAL version number.
 *
 * @return PAL version (major, minor, build).
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palGetVersionString
 */
PAL_API PalVersion PAL_CALL palGetVersion();

/**
 * Retrieve the PAL version string.
 *
 * @return Null-terminated string containing the PAL version.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palGetVersion
 */
PAL_API const char* PAL_CALL palGetVersionString();

#endif // _CORE_VERSION_H

/** @} */