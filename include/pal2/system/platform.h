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

#ifndef PAL_SYSTEM_PLATFORM_H
#define PAL_SYSTEM_PLATFORM_H

#include "pal2/core/defines.h"
#include "pal2/core/version.h"

#define PAL_PLATFORM_NAME_SIZE 32

/**
 * @defgroup platform_types Platform Types
 * @brief Platform types.
 * 
 * @{
 */
#define PAL_PLATFORM_TYPE_WINDOWS 0
#define PAL_PLATFORM_TYPE_LINUX 1
#define PAL_PLATFORM_TYPE_MACOS 2
#define PAL_PLATFORM_TYPE_ANDROID 3
#define PAL_PLATFORM_TYPE_IOS 4
#define PAL_PLATFORM_TYPE_COUNT 5
/** @} */

/**
 * @defgroup platform_api_types Platform API Types
 * @brief Platform API types.
 * 
 * @{
 */
#define PAL_PLATFORM_API_TYPE_WIN32 0
#define PAL_PLATFORM_API_TYPE_WAYLAND 1
#define PAL_PLATFORM_API_TYPE_X11 2
#define PAL_PLATFORM_API_TYPE_COCOA 3
#define PAL_PLATFORM_API_TYPE_ANDRIOD 4
#define PAL_PLATFORM_API_TYPE_UIKIT 5
#define PAL_PLATFORM_API_TYPE_HEADLESS 6
#define PAL_PLATFORM_API_TYPE_COUNT 7
/** @} */

/**
 * @typedef PalPlatformType
 * @brief Platform types.
 *
 * This is the family name (eg. This does not show if its Windows 7 or 
 * Windows 8 etc).
 * 
 * All values of this type follow the format `PAL_PLATFORM_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalPlatformType;

/**
 * @typedef PalPlatformApiType
 * @brief Platform API types.
 *
 * This is the API the playform uses. Most platforms support only one (eg.
 * Windows).
 * 
 * All values of this type follow the format `PAL_PLATFORM_API_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalPlatformApiType;

/**
 * @struct PalPlatformInfo
 * @brief Contains information about a platform (OS).
 *
 * @since Added in version 2.0
 * 
 * @var PalPlatformInfo::type
 * The platform type (eg. `PAL_PLATFORM_TYPE_WINDOWS`).
 * 
 * @var PalPlatformInfo::apiType
 * The platform API type (eg. `PAL_PLATFORM_API_TYPE_WIN32`).
 * 
 * @var PalPlatformInfo::totalMemory
 * Total Disk space in GB. 
 * On `Windows`: the size is from C drive only. @nl
 * On `Linux`: the size is from root only.
 * 
 * @var PalPlatformInfo::totalRAM
 * Total CPU memory in MB.
 * 
 * @var PalPlatformInfo::version
 * The platform version.
 * 
 * @var PalPlatformInfo::name
 * The full platform name with contains the family name and version.
 * (eg. Windows 11.22000)
 */
typedef struct PalPlatformInfo
{
    PalPlatformType type;
    PalPlatformApiType apiType;
    uint32_t totalMemory;
    uint32_t totalRAM;
    PalVersion version;
    char name[PAL_PLATFORM_NAME_SIZE];
} PalPlatformInfo;

/**
 * @brief Gets the platform (OS) information.
 *
 * @param[out] info The platform information.
 *
 * @Thread-safety `info` must be per thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetPlatformInfo(PalPlatformInfo* info);

#endif // PAL_SYSTEM_PLATFORM_H