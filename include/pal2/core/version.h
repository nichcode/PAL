
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

#ifndef PAL_CORE_VERSION_H
#define PAL_CORE_VERSION_H

#include "defines.h"

/**
 * @struct PalVersion
 * @brief Contains information about the version of PAL.
 * 
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @var PalVersion::major
 * The major version number of the PAL runtime. 
 * This is incremented when breaking changes are made.
 * 
 * @var PalVersion::minor
 * The minor version number of the PAL runtime. 
 * This is incremented when backward-compatible features are added.
 * 
 * @var PalVersion::build
 * The build version number of the PAL runtime. 
 * This is incremented when bugs are fixed without API changes.
 */
typedef struct PalVersion {
    uint32_t major;
    uint32_t minor;
    uint32_t build;
} PalVersion;

/**
 * @brief Retrieves the PAL runtime version number.
 *
 * @param[out] version The output struct to recieve the runtime version. 
 * Must not be `nullptr`.
 *
 * @Thread-safety `version` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * @sa palGetVersionString
 */
PAL_API void PAL_CALL palGetVersion(PalVersion* version);

/**
 * @brief Retrieves the PAL runtime version as a string.
 *
 * @return Null-terminated string containing the PAL runtime version. The
 * returned string is owned by PAL and must not be freed or modified.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * @sa palGetVersion
 */
PAL_API const char* PAL_CALL palGetVersionString(void);

#endif // PAL_CORE_VERSION_H