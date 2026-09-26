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

/**
 * @defgroup platform_types Platform Types
 * @brief Platform types
 * 
 * @ingroup pal_system
 */

/**
 * @defgroup platform_api_types Platform API Types
 * @brief Platform API types.
 * 
 * @ingroup pal_system
 */

/** @brief The maximum platform name size.
 * 
 * @ingroup pal_system
*/
#define PAL_PLATFORM_NAME_SIZE 32

/** @brief The platform family type is Windows.
 * 
 * @ingroup platform_types
*/
#define PAL_PLATFORM_TYPE_WINDOWS 0

/** @brief The platform family type is Linux.
 * 
 * @ingroup platform_types
*/
#define PAL_PLATFORM_TYPE_LINUX 1

/** @brief The platform family type is MacOS.
 * 
 * @ingroup platform_types
*/
#define PAL_PLATFORM_TYPE_MACOS 2

/** @brief The platform family type is Android.
 * 
 * @ingroup platform_types
*/
#define PAL_PLATFORM_TYPE_ANDROID 3

/** @brief The platform family type is IOS.
 * 
 * @ingroup platform_types
*/
#define PAL_PLATFORM_TYPE_IOS 4

/** @brief The maximum number of platform types. The literal value must
 * not be used.
 * 
 * @ingroup platform_types
*/
#define PAL_PLATFORM_TYPE_COUNT 5

/** @brief The platform API type is Win32.
 * 
 * @ingroup platform_api_types
*/
#define PAL_PLATFORM_API_TYPE_WIN32 0

/** @brief The platform API type is Wayland.
 * 
 * @ingroup platform_api_types
*/
#define PAL_PLATFORM_API_TYPE_WAYLAND 1

/** @brief The platform API type is Xlib.
 * 
 * @ingroup platform_api_types
*/
#define PAL_PLATFORM_API_TYPE_X11 2

/** @brief The platform API type is Cocoa.
 * 
 * @ingroup platform_api_types
*/
#define PAL_PLATFORM_API_TYPE_COCOA 3

/** @brief The platform API type is Android.
 * 
 * @ingroup platform_api_types
*/
#define PAL_PLATFORM_API_TYPE_ANDRIOD 4

/** @brief The platform API type is UIKIT.
 * 
 * @ingroup platform_api_types
*/
#define PAL_PLATFORM_API_TYPE_UIKIT 5

/** @brief The platform API type is headless (no API).
 * 
 * @ingroup platform_api_types
*/
#define PAL_PLATFORM_API_TYPE_HEADLESS 6

/** @brief The maximum number of platform API types. The literal value must
 * not be used.
 * 
 * @ingroup platform_api_types
*/
#define PAL_PLATFORM_API_TYPE_COUNT 7

/**
 * @typedef PalPlatformType
 * @brief Platform type.
 *
 * This is the family name (eg. This does not show if its Windows 7 or 
 * Windows 8).
 * 
 * All values of this type follow the format `PAL_PLATFORM_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalPlatformType;

/**
 * @typedef PalPlatformApiType
 * @brief Platform API type.
 *
 * This is the API the platform uses.
 * 
 * All values of this type follow the format `PAL_PLATFORM_API_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalPlatformApiType;

/**
 * @struct PalPlatformInfo
 * @brief Information about a platform (OS).
 *
 * @since Added in version 2.0
 */
typedef struct PalPlatformInfo {
    /** The platform type (eg. PAL_PLATFORM_TYPE_WINDOWS).*/
    PalPlatformType type;

    /** The platform API type (eg. PAL_PLATFORM_API_TYPE_WIN32).*/
    PalPlatformApiType apiType;

    /** Total Disk space in GB.
     * On `Windows`: the size is from C drive only.
     * On `Linux`: the size is from root only.
     */
    uint32_t totalMemory;

    /** Total CPU memory in MB.*/
    uint32_t totalRAM;

    /** The platform version.*/
    PalVersion version;

    /** The full platform name with contains the family name and version.
     * (eg. Windows 11.22000)
     */
    char name[PAL_PLATFORM_NAME_SIZE];
} PalPlatformInfo;

/**
 * @brief Get the platform (OS) information.
 *
 * @param[out] info The output struct to recieve the platform information.
 *
 * @Thread-safety `info` parameter must be per thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetPlatformInfo(PalPlatformInfo* info);

#endif // PAL_SYSTEM_PLATFORM_H