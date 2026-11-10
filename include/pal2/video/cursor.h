
/**
 * @file cursor.h
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

#ifndef PAL_VIDEO_CURSOR_H
#define PAL_VIDEO_CURSOR_H

#include "types.h"

/**
 * @defgroup cursor_types Cursor Types
 * @ingroup pal_video
 * @{
 */
#define PAL_CURSOR_TYPE_ARROW 0 /**< Arrow cursor. */
#define PAL_CURSOR_TYPE_HAND 1 /**< Hand cursor. */
#define PAL_CURSOR_TYPE_CROSS 2 /**< Crosshair cursor. */
#define PAL_CURSOR_TYPE_IBEAM 3 /**< Text I-beam cursor. */
#define PAL_CURSOR_TYPE_WAIT 4 /**< Busy or wait cursor. */
#define PAL_CURSOR_TYPE_COUNT 5 /**< Number of cursor types. */
/** @} */

/**
 * @struct PalCursorCreateInfo
 * @brief Cursor creation parameters.
 *
 * This struct is used only during @ref palCreateCursor() and may be
 * discarded after the function returns.
 *
 * Coordinates are relative to the upper-left corner of the cursor.
 * X-coordinate increases to the right and Y-coordinate increases down.
 *
 * `pixels` must be little-endian 32-bit, RGBA 8-bits per channel.
 * The pixels are copied, so they can be freed after the cursor is created.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalCursorCreateInfo {
    const uint8_t* pixels; /**< Cursor pixels. */
    uint32_t width; /**< Cursor width in pixels. */
    uint32_t height; /**< Cursor height in pixels. */
    int32_t xHotspot; /**< Cursor x hotspot in pixels. */
    int32_t yHotspot; /**< Cursor y hotspot in pixels. */
} PalCursorCreateInfo;

/**
 * @brief Creates a cursor.
 *
 * The created cursor must be destroyed using @ref palDestroyCursor().
 *
 * This function creates a cursor from the provided pixels.
 * The created cursor should be set to the window with @ref palSetWindowCursor().
 * The provided pixels are copied when the cursor is created, so the data may be freed
 * afterwards. @ref PAL_VIDEO_FEATURE_WINDOW_SET_CURSOR feature must be supported.
 *
 * @param[in] info Cursor creation parameters.
 * @param[out] cursor Output handle to receive the created cursor.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @Result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @Result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @Result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palDestroyCursor
 * @sa palSetWindowCursor
 */
PAL_API PalResult PAL_CALL palCreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** cursor);

/**
 * @brief Creates a system cursor.
 *
 * The created cursor must be destroyed using @ref palDestroyCursor().
 *
 * This function creates a cursor from the platform predefined types.
 * The created cursor should be set to the window with @ref palSetWindowCursor().
 * The cursor appearance may vary based on the platform.
 * @ref PAL_VIDEO_FEATURE_WINDOW_SET_CURSOR feature must be supported.
 *
 * @param[in] type System cursor type.
 * @param[out] cursor Output handle to receive the created cursor.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @Result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @Result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @Result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palDestroyCursor
 * @sa palSetWindowCursor
 */
PAL_API PalResult PAL_CALL palCreateCursorFrom(
    PalCursorType type,
    PalCursor** cursor);

/**
 * @brief Destroys a cursor.
 *
 * Any window still using the cursor must be reverted to the platform default by
 * calling @ref palSetWindowCursor() with `nullptr` before this function.
 *
 * @param[in] cursor Cursor to destroy.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palCreateCursor
 * @sa palCreateCursorFrom
 */
PAL_API void PAL_CALL palDestroyCursor(PalCursor* cursor);

/**
 * @brief Shows or hides the cursor.
 *
 * This function shows or hides the cursor. This affects all cursors
 * of the screen. @ref PAL_VIDEO_FEATURE_CURSOR_SET_VISIBILITY feature must be supported
 * or this function results in undefined behavior.
 *
 * @param[in] show `PAL_TRUE` to make the cursor visible, otherwise `PAL_FALSE`.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palShowCursor(PalBool show);

/**
 * @brief Confines the cursor to a window.
 *
 * This function confines the cursor to the bounds of the specified window.
 * The cursor must be unclipped before the window is destroyed; otherwise the behavior is undefined.
 * @ref PAL_VIDEO_FEATURE_CLIP_CURSOR feature must be supported or this function
 * results in undefined behavior.
 *
 * @param[in] window Window to confine the cursor to.
 * @param[in] clip `PAL_TRUE` to clip the cursor to the window, or `PAL_FALSE` to unclip it.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palClipCursor(
    PalWindow* window,
    PalBool clip);

/**
 * @brief Gets the position of the cursor relative to the window.
 *
 * The cursor position is relative to the upper-left corner of the window
 * in screen coordinates. @ref PAL_VIDEO_FEATURE_CURSOR_GET_POS feature must be
 * supported or this function results in undefined behavior.
 *
 * @param[in] window Window the cursor position is relative to.
 * @param[out] x Output to receive the x cursor position, or `nullptr` to ignore it.
 * @param[out] y Output to receive the y cursor position, or `nullptr` to ignore it.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palGetCursorPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y);

/**
 * @brief Sets the position of the cursor relative to the window.
 *
 * The cursor position is relative to the upper-left corner of the window
 * in screen coordinates. @ref PAL_VIDEO_FEATURE_CURSOR_SET_POS feature must be
 * supported or this function results in undefined behavior.
 *
 * @param[in] window Window the cursor position is relative to.
 * @param[in] x X coordinate of the cursor.
 * @param[in] y Y coordinate of the cursor.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palSetCursorPos(
    PalWindow* window,
    int32_t x,
    int32_t y);

/**
 * @brief Sets the cursor for a window.
 *
 * A single cursor can be set to multiple windows at the same time. `cursor` will not be copied;
 * it must remain valid until the window is destroyed or reverted. Destroying the cursor and not
 * setting it to `nullptr` will result in undefined behavior if windows are referencing it.
 * @ref PAL_VIDEO_FEATURE_WINDOW_SET_CURSOR feature must be supported or this function results
 * in undefined behavior.
 *
 * @param[in] window Window to set cursor on.
 * @param[in] cursor Cursor, or `nullptr` to revert to the platform default.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor);

#endif // PAL_VIDEO_CURSOR_H