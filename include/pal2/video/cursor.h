/**
 * @file cursor.h
 * @brief This is the header file for PAL Cursor API.
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

#ifndef CURSOR_H
#define CURSOR_H

#include "window.h"

/**
 * @defgroup cursor_types Cursor Types
 * @brief Cursor Types
 * 
 * @{
 */
#define PAL_CURSOR_TYPE_ARROW 0
#define PAL_CURSOR_TYPE_HAND 1
#define PAL_CURSOR_TYPE_CROSS 2
#define PAL_CURSOR_TYPE_IBEAM 3
#define PAL_CURSOR_TYPE_WAIT 4
#define PAL_CURSOR_TYPE_COUNT 5
/** @} */

/**
 * @typedef PalCursorType
 * @brief System cursor types.
 * 
 * All values of this type follow the format `PAL_CURSOR_TYPE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalCursorType;

/**
 * @struct PalCursor
 * @brief Opaque handle to a cursor.
 *
 * @since Added in version 2.0
 */
typedef struct PalCursor PalCursor;

/**
 * @struct PalCursorCreateInfo
 * @brief Contains creation parameters of a cursor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalCursorCreateInfo::pixels
 * The pixels to use to create the cursor. It must be in `RGBA` format.
 * 
 * @var PalCursorCreateInfo::width
 * The width of the cursor in pixels.
 * 
 * @var PalCursorCreateInfo::height
 * The height of the cursor in pixels.
 * 
 * @var PalCursorCreateInfo::xHotspot
 * The x pixel for detecting clicks.
 * 
 * @var PalCursorCreateInfo::yHotspot
 * The y pixel for detecting clicks.
 */
typedef struct PalCursorCreateInfo
{
    const uint8_t* pixels;
    uint32_t width;
    uint32_t height;
    int32_t xHotspot;
    int32_t yHotspot;
} PalCursorCreateInfo;

/**
 * @brief Creates a cursor.
 *
 * @param[in] info Information about how to create the cursor.
 * @param[out] cursor The output handle to recieve the created cursor.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palDestroyCursor
 */
PAL_API PalResult PAL_CALL palCreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** cursor);

/**
 * @brief Creates a system cursor.
 *
 * @param[in] type The system cursor type.
 * @param[out] cursor The output handle to recieve the created cursor.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palDestroyCursor
 */
PAL_API PalResult PAL_CALL palCreateCursorFrom(
    PalCursorType type,
    PalCursor** cursor);

/**
 * @brief Destroys the cursor.
 *
 * @param[in] cursor The cursor.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palCreateCursor
 */
PAL_API void PAL_CALL palDestroyCursor(PalCursor* cursor);

/**
 * @brief Shows or hide the cursor.
 *
 * `PAL_VIDEO_FEATURE_CURSOR_SET_VISIBILITY` must be supported
 * otherwise undefined behavior.
 *
 * This affects all created cursors since the platform (OS) merges all cursors
 * into a single one on the screen.
 *
 * @param[in] show `PAL_TRUE` to make the cursor visible otherwise `PAL_FALSE`.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palShowCursor(PalBool show);

/**
 * @brief Clips the cursor to the window.
 *
 * `PAL_VIDEO_FEATURE_CLIP_CURSOR` must be supported 
 * otherwise undefined behavior.
 *
 * If the window is destroyed without unclipping the cursor, this cursor might
 * not reset depending on the platform (OS). To be safe, unclip the cursor from
 * the window before destroying the window.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] clip `PAL_TRUE` to clip to window or `PAL_FALSE` to unclip.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palClipCursor(
    PalWindow* window,
    PalBool clip);

/**
 * @brief Gets the position of the cursor relative to the window in
 * pixels.
 *
 * `PAL_VIDEO_FEATURE_CURSOR_GET_POS` must be supported 
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[out] x The output to recieve the x in pixels. Can be `nullptr`.
 * @param[out] y The output to recieve the y in pixels. Can be `nullptr`.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetCursorPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y);

/**
 * @brief Sets the position of the cursor relative to the window in
 * pixels.
 *
 * `PAL_VIDEO_FEATURE_CURSOR_SET_POS` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] x The new x coordinate of the cursor in pixels.
 * @param[in] y The new y coordinate of the cursor in pixels.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palSetCursorPos(
    PalWindow* window,
    int32_t x,
    int32_t y);

/**
 * @brief Sets the cursor for the window.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] cursor The cursor. Set to `nullptr` to revert to default.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor);

#endif // CURSOR_H