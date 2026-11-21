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

PalResult wlCreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** cursor)
{
    BITMAPV5HEADER bitInfo = {0};
    bitInfo.bV5Size = sizeof(BITMAPV5HEADER);
    bitInfo.bV5Width = info->width;
    bitInfo.bV5Height = -(int32_t)info->height; // this is topdown by default

    bitInfo.bV5Planes = 1;
    bitInfo.bV5BitCount = 32; // PAL supports 32 bits
    bitInfo.bV5Compression = BI_BITFIELDS;
    bitInfo.bV5RedMask = 0x00FF0000;
    bitInfo.bV5GreenMask = 0x0000FF00;
    bitInfo.bV5BlueMask = 0x000000FF;
    bitInfo.bV5AlphaMask = 0xFF000000;

    HDC hdc = GetDC(nullptr);
    void* dibPixels = nullptr;

    // clang-format off
    HBITMAP bitmap = g_Win32.createDIBSection(
        hdc, 
        (BITMAPINFO*)&bitInfo, 
        DIB_RGB_COLORS, 
        &dibPixels, 
        nullptr, 
        0);
    // clang-format on

    if (!bitmap) {
        ReleaseDC(nullptr, hdc);
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, GetLastError());
    }

    ReleaseDC(nullptr, hdc);

    // convert RGBA to BGRA
    uint8_t* pixels = (uint8_t*)dibPixels;
    for (uint32_t i = 0; i < info->width * info->height; i++) {
        uint8_t r = info->pixels[i * 4 + 0];
        uint8_t g = info->pixels[i * 4 + 1];
        uint8_t b = info->pixels[i * 4 + 2];
        uint8_t a = info->pixels[i * 4 + 3];

        // premultiply only if alpha is not 0
        if (a == 0) {
            r = g = b = 0;
        } else {
            r = (uint8_t)((r * a) / 255);
            g = (uint8_t)((g * a) / 255);
            b = (uint8_t)((b * a) / 255);
        }

        pixels[i * 4 + 0] = b;
        pixels[i * 4 + 1] = g;
        pixels[i * 4 + 2] = r;
        pixels[i * 4 + 3] = a;
    }

    ICONINFO iconInfo = {0};
    iconInfo.fIcon = FALSE;
    iconInfo.hbmColor = bitmap;
    iconInfo.hbmMask = bitmap;
    iconInfo.xHotspot = info->xHotspot;
    iconInfo.xHotspot = info->yHotspot;

    HCURSOR _cursor = CreateIconIndirect(&iconInfo);
    g_Win32.deleteObject(bitmap);

    if (!_cursor) {    
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, GetLastError());
    }

    *cursor = (PalCursor*)_cursor;
    return PAL_RESULT_SUCCESS;
}

PalResult wlCreateCursorFrom(
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

void wlDestroyCursor(PalCursor* cursor)
{
    DestroyCursor((HCURSOR)cursor);
}

void wlShowCursor(PalBool show)
{
    ShowCursor(show);
}

void wlClipCursor(
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

void wlGetCursorPos(
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

void wlSetCursorPos(
    PalWindow* window,
    int32_t x,
    int32_t y)
{
    POINT pos = {x, y};
    ClientToScreen((HWND)window, &pos);
    SetCursorPos(pos.x, pos.y);
}

void wlSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor)
{
    WindowData* data = (WindowData*)GetPropW((HWND)window, VIDEO_PROP);
    if (data) {
        data->cursor = (HCURSOR)cursor;
    }
}

#endif // _WIN32