
/**
 * @file types.h
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

#ifndef PAL_SYSTEM_PLATFORM_H
#define PAL_SYSTEM_PLATFORM_H

#include "types.h"
#include "pal2/core/version.h"

/**
 * @brief Maximum size of the platform name, including the NULL terminator.
 * @ingroup pal_system
 */
#define PAL_PLATFORM_NAME_SIZE 32

/**
 * @defgroup platform_types Platform Types
 * @ingroup pal_system
 * @{
 */
#define PAL_PLATFORM_TYPE_WINDOWS 0 /**< Windows. */
#define PAL_PLATFORM_TYPE_LINUX 1 /**< Linux. */
#define PAL_PLATFORM_TYPE_MACOS 2 /**< MacOS. */
#define PAL_PLATFORM_TYPE_ANDROID 3 /**< Android. */
#define PAL_PLATFORM_TYPE_IOS 4 /**< IOS. */
#define PAL_PLATFORM_TYPE_COUNT 5 /**< Number of platform types. */
/** @} */

/**
 * @defgroup platform_api_types Platform API Types
 * @ingroup pal_system
 * @{
 */
#define PAL_PLATFORM_API_TYPE_WIN32 0 /**< Win32 (Windows). */
#define PAL_PLATFORM_API_TYPE_WAYLAND 1 /**< Wayland (Linux). */
#define PAL_PLATFORM_API_TYPE_X11 2 /**< X11 (Linux). */
#define PAL_PLATFORM_API_TYPE_COCOA 3 /**< Cocoa (MacOS). */
#define PAL_PLATFORM_API_TYPE_ANDRIOD 4 /**< Android native windowing. */
#define PAL_PLATFORM_API_TYPE_UIKIT 5 /**< UIKit (IOS). */
#define PAL_PLATFORM_API_TYPE_HEADLESS 6 /**< No windowing system available. */
#define PAL_PLATFORM_API_TYPE_COUNT 7 /**< Number of platform API types. */
/** @} */

/**
 * @struct PalPlatformInfo
 * @brief Platform information.
 *
 * Filled in by @ref palGetPlatformInfo().
 *
 * @since Added in version 2.0
 * @ingroup pal_system
 */
typedef struct PalPlatformInfo {
    PalPlatformType type; /**< Platform type. */
    PalPlatformApiType apiType; /**< Platform API type. */
    uint32_t totalMemory; /**< Total disk size in GB. */
    uint32_t totalRAM; /**< Total system memory (RAM) in MB. */
    PalVersion version; /**< Platform version. */
    char name[PAL_PLATFORM_NAME_SIZE]; /**< Platform name (NULL-terminated). */
} PalPlatformInfo;

/**
 * @brief Gets the platform information.
 *
 * @param[out] info Output struct to receive the platform information.
 *
 * @thread-safety `info` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_system
 */
PAL_API void PAL_CALL palGetPlatformInfo(PalPlatformInfo* info);

#endif // PAL_SYSTEM_PLATFORM_H