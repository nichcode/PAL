
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef _WIN32
#include "pal_video_win32.h"

PalResult win32CreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** outCursor)
{
    // describe the icon pixels
    BITMAPV5HEADER bitInfo = {0};
    bitInfo.bV5Size = sizeof(BITMAPV5HEADER);
    bitInfo.bV5Width = info->width;
    bitInfo.bV5Height = -(int32_t)info->height; // this is topdown by default

    // default parameters
    bitInfo.bV5Planes = 1;
    bitInfo.bV5BitCount = 32; // PAL supports 32 bits
    bitInfo.bV5Compression = BI_BITFIELDS;
    bitInfo.bV5RedMask = 0x00FF0000;
    bitInfo.bV5GreenMask = 0x0000FF00;
    bitInfo.bV5BlueMask = 0x000000FF;
    bitInfo.bV5AlphaMask = 0xFF000000;

    HDC hdc = GetDC(nullptr);
    void* dibPixels = nullptr;

    // create dib section
    // clang-format off
    HBITMAP bitmap = s_Win32.createDIBSection(
        hdc, 
        (BITMAPINFO*)&bitInfo, 
        DIB_RGB_COLORS, 
        &dibPixels, 
        nullptr, 
        0);
    // clang-format on

    if (!bitmap) {
        ReleaseDC(nullptr, hdc);
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_WIN32, 
            GetLastError());
    }
    ReleaseDC(nullptr, hdc);

    // convert RGBA to BGRA
    uint8_t* pixels = (uint8_t*)dibPixels;
    for (uint32_t i = 0; i < info->width * info->height; i++) {
        uint8_t r = info->pixels[i * 4 + 0]; // Red
        uint8_t g = info->pixels[i * 4 + 1]; // Green
        uint8_t b = info->pixels[i * 4 + 2]; // Blue
        uint8_t a = info->pixels[i * 4 + 3]; // Alpha

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

    // create the cursor with the iconinfo
    HCURSOR cursor = CreateIconIndirect(&iconInfo);
    if (!cursor) {
        s_Win32.deleteObject(bitmap);
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_WIN32, 
            GetLastError());
    }

    s_Win32.deleteObject(bitmap);
    *outCursor = (PalCursor*)cursor;
    return PAL_RESULT_SUCCESS;
}

PalResult win32CreateCursorFrom(
    PalCursorType type,
    PalCursor** outCursor)
{
    HCURSOR cursor = nullptr;
    switch (type) {
        case PAL_CURSOR_TYPE_ARROW: {
            cursor = LoadCursorW(nullptr, IDC_ARROW);
            break;
        }

        case PAL_CURSOR_TYPE_HAND: {
            cursor = LoadCursorW(nullptr, IDC_HAND);
            break;
        }

        case PAL_CURSOR_TYPE_CROSS: {
            cursor = LoadCursorW(nullptr, IDC_CROSS);
            break;
        }

        case PAL_CURSOR_TYPE_IBEAM: {
            cursor = LoadCursorW(nullptr, IDC_IBEAM);
            break;
        }

        case PAL_CURSOR_TYPE_WAIT: {
            cursor = LoadCursorW(nullptr, IDC_WAIT);
            break;
        }
    }

    if (!cursor) {
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_WIN32, 
            GetLastError());
    }

    *outCursor = (PalCursor*)cursor;
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
    WindowData* data = (WindowData*)GetPropW((HWND)window, PAL_VIDEO_PROP);
    data->cursor = (HCURSOR)cursor;
}

#endif // _WIN32