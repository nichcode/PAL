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

#include "platform.h"
#include "pal2/video/cursor.h"

PalResult PAL_CALL palCreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** cursor)
{
    if (!g_VideoInitialized) {
        return PAL_RESULT_CODE_INVALID_OPERATION;
    }

    if (!info || !cursor || !info->pixels || info->width == 0 || info->height == 0) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return g_VideoBackend->createCursor(info, cursor);
}

PalResult PAL_CALL palCreateCursorFrom(
    PalCursorType type,
    PalCursor** cursor)
{
    if (!g_VideoInitialized) {
        return PAL_RESULT_CODE_INVALID_OPERATION;
    }

    if (!cursor) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return g_VideoBackend->createCursorFrom(type, cursor);
}

void PAL_CALL palDestroyCursor(PalCursor* cursor)
{
    ASSERT(cursor != nullptr, "cursor parameter is nullptr");

    g_VideoBackend->destroyCursor(cursor);
}

void PAL_CALL palShowCursor(PalBool show)
{
    g_VideoBackend->showCursor(show);
}

void PAL_CALL palClipCursor(
    PalWindow* window,
    PalBool clip)
{
    ASSERT(window != nullptr, "window parameter is nullptr");

    g_VideoBackend->clipCursor(window, clip);
}

void PAL_CALL palGetCursorPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y)
{
    ASSERT(window != nullptr, "window parameter is nullptr");

    g_VideoBackend->getCursorPos(window, x, y);
}

void PAL_CALL palSetCursorPos(
    PalWindow* window,
    int32_t x,
    int32_t y)
{
    ASSERT(window != nullptr, "window parameter is nullptr");

    g_VideoBackend->setCursorPos(window, x, y);
}

void PAL_CALL palSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor)
{
    ASSERT(window != nullptr, "window parameter is nullptr");

    g_VideoBackend->setWindowCursor(window, cursor);
}