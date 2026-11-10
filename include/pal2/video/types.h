
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

#ifndef PAL_VIDEO_TYPES_H
#define PAL_VIDEO_TYPES_H

#include "pal2/core/types.h"

/**
 * @typedef PalVideoFeatures
 * @brief Video system features.
 *
 * Identifies the features supported by the video system. This is a bitmask of
 * all supported features.
 *
 * All values of this type follow the format `PAL_VIDEO_FEATURE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef uint64_t PalVideoFeatures;

/**
 * @typedef PalVideoDriverType
 * @brief Video driver type.
 *
 * Identifies the type of a video driver.
 *
 * All values of this type follow the format `PAL_VIDEO_DRIVER_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.2
 * @ingroup pal_video
 */
typedef uint32_t PalVideoDriverType;

/**
 * @typedef PalWindowStyle
 * @brief Window style.
 *
 * Identifies the style of a window. Styles are combined as a bitmask.
 *
 * All values of this type follow the format `PAL_WINDOW_STYLE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef uint32_t PalWindowStyle;

/**
 * @typedef PalWindowState
 * @brief Window state.
 *
 * Identifies the state of a window.
 *
 * All values of this type follow the format `PAL_WINDOW_STATE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef uint32_t PalWindowState;

/**
 * @typedef PalFlashFlags
 * @brief Window flash flags.
 *
 * Identifies how a window is flashed. Flags are combined as a bitmask.
 *
 * All values of this type follow the format `PAL_FLASH_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef uint32_t PalFlashFlags;

/**
 * @typedef PalFBConfigBackend
 * @brief Window framebuffer configuration backend.
 *
 * Identifies the backend used to select the framebuffer configuration of a window.
 *
 * All values of this type follow the format `PAL_FBCONFIG_BACKEND_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef uint32_t PalFBConfigBackend;

/**
 * @typedef PalOrientation
 * @brief Monitor orientation.
 *
 * Identifies the orientation of a monitor.
 *
 * All values of this type follow the format `PAL_ORIENTATION_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef uint32_t PalOrientation;

/**
 * @typedef PalCursorType
 * @brief System cursor type.
 *
 * Identifies the kind of a system cursor.
 *
 * All values of this type follow the format `PAL_CURSOR_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef uint32_t PalCursorType;

/**
 * @typedef PalKeycode
 * @brief Keycode (layout aware key) of a keyboard.
 *
 * Identifies a key of the keyboard by the character it produces on the current layout.
 *
 * All values of this type follow the format `PAL_KEYCODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef uint32_t PalKeycode;

/**
 * @typedef PalScancode
 * @brief Scancode (layout independent key) of a keyboard.
 *
 * Identifies a physical key of the keyboard regardless of the layout.
 *
 * All values of this type follow the format `PAL_SCANCODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef uint32_t PalScancode;

/**
 * @typedef PalMouseButton
 * @brief Mouse button.
 *
 * Identifies a button of the mouse.
 *
 * All values of this type follow the format `PAL_MOUSE_BUTTON_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef uint32_t PalMouseButton;

/**
 * @struct PalWindow
 * @brief Opaque handle to a window.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalWindow PalWindow;

/**
 * @struct PalMonitor
 * @brief Opaque handle to a monitor.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalMonitor PalMonitor;

/**
 * @struct PalCursor
 * @brief Opaque handle to a cursor.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalCursor PalCursor;

/**
 * @struct PalIcon
 * @brief Opaque handle to an icon.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalIcon PalIcon;

#endif // PAL_VIDEO_TYPES_H