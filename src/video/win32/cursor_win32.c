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

#ifdef _WIN32

#include "win32_helper.h"
#include "pal2/core/memory.h"
#include "pal2/video/cursor.h"

PalResult win32CreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** cursor)
{
    HBITMAP bitmap;
    PalResult result = win32CreateBitmap(info->width, info->height, info->pixels, &bitmap);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    ICONINFO iconInfo = {0};
    iconInfo.fIcon = FALSE;
    iconInfo.hbmColor = bitmap;
    iconInfo.hbmMask = bitmap;
    iconInfo.xHotspot = info->xHotspot;
    iconInfo.xHotspot = info->yHotspot;

    HCURSOR _cursor = CreateIconIndirect(&iconInfo);
    win32DestroyBitmap(bitmap);

    if (!_cursor) {    
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, GetLastError());
    }

    *cursor = (PalCursor*)_cursor;
    return PAL_RESULT_SUCCESS;
}

PalResult win32CreateCursorFrom(
    PalCursorType type,
    PalCursor** cursor)
{
    HCURSOR _cursor = nullptr;
    switch (type) {
    case PAL_CURSOR_TYPE_ARROW: {
        _cursor = LoadCursorW(nullptr, IDC_ARROW);
        break;
    }

    case PAL_CURSOR_TYPE_HAND: {
        _cursor = LoadCursorW(nullptr, IDC_HAND);
        break;
    }

    case PAL_CURSOR_TYPE_CROSS: {
        _cursor = LoadCursorW(nullptr, IDC_CROSS);
        break;
    }

    case PAL_CURSOR_TYPE_IBEAM: {
        _cursor = LoadCursorW(nullptr, IDC_IBEAM);
        break;
    }

    case PAL_CURSOR_TYPE_WAIT: {
        cursor = LoadCursorW(nullptr, IDC_WAIT);
        break;
    }
    }

    if (!cursor) {
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, GetLastError());
    }

    *cursor = (PalCursor*)_cursor;
    return PAL_RESULT_SUCCESS;
}

void win32DestroyCursor(PalCursor* cursor)
{
    DestroyCursor((HCURSOR)cursor);
}

void win32ShowCursor(PalBool show)
{
    ShowCursor(show);
}

void win32ClipCursor(
    PalWindow* window,
    PalBool clip)
{
    if (clip) {
        RECT rect;
        GetClientRect((HWND)window, &rect);
        POINT tmp = {rect.left, rect.top};
        POINT tmp2 = {rect.right, rect.bottom};

        ClientToScreen((HWND)window, &tmp);
        ClientToScreen((HWND)window, &tmp2);

        RECT clipRect = {tmp.x, tmp.y, tmp2.x, tmp2.y};
        ClipCursor(&clipRect);

    } else {
        ClipCursor(nullptr);
    }
}

void win32GetCursorPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y)
{
    POINT pos;
    GetCursorPos(&pos);
    ScreenToClient((HWND)window, &pos);
    if (x) {
        *x = pos.x;
    }

    if (y) {
        *y = pos.y;
    }
}

void win32SetCursorPos(
    PalWindow* window,
    int32_t x,
    int32_t y)
{
    POINT pos = {x, y};
    ClientToScreen((HWND)window, &pos);
    SetCursorPos(pos.x, pos.y);
}

void win32SetWindowCursor(
    PalWindow* window,
    PalCursor* cursor)
{
    WindowData* data = (WindowData*)GetPropW((HWND)window, VIDEO_PROP);
    if (data) {
        data->cursor = (HCURSOR)cursor;
    }
}

#endif // _WIN32