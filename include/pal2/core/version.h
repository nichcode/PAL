
/**
 * @file version.h
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

#ifndef PAL_CORE_VERSION_H
#define PAL_CORE_VERSION_H

#include "types.h"

/**
 * @struct PalVersion
 * @brief PAL runtime version.
 *
 * `major` is incremented when breaking changes are made.
 * `minor` is incremented when backward-compatible features are added.
 * `build` is incremented when bugs are fixed without adding features or making breaking changes.
 *
 * Filled in by @ref palGetVersion().
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 */
typedef struct PalVersion {
    uint32_t major; /**< major version */
    uint32_t minor; /**< minor version */
    uint32_t build; /**< build (patch) version */
} PalVersion;

/**
 * @brief Retrieves the PAL runtime version.
 *
 * @param[out] version Output struct to receive the PAL runtime version.
 *
 * @Thread-safety `version` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
 * @sa palGetVersionString
 */
PAL_API void PAL_CALL palGetVersion(PalVersion* version);

/**
 * @brief Retrieves the PAL runtime version as a string.
 *
 * This function converts the PAL runtime version into a null-terminated UTF-8
 * encoded string. The returned string is owned by PAL and must not be modified
 * or freed. The returned string is formatted as `major.minor.build`.
 *
 * @return Null-terminated string containing the PAL runtime version.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 *
 * @sa palGetVersion
 */
PAL_API const char* PAL_CALL palGetVersionString(void);

#endif // PAL_CORE_VERSION_H