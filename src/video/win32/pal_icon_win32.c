
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef _WIN32
#include "pal_video_win32.h"

PalResult win32CreateIcon(
    const PalIconCreateInfo* info,
    PalIcon** outIcon)
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
    iconInfo.fIcon = TRUE;
    iconInfo.hbmMask = bitmap;
    iconInfo.hbmColor = bitmap;

    // create the icon with the icon info
    HICON icon = CreateIconIndirect(&iconInfo);
    if (!icon) {
        s_Win32.deleteObject(bitmap);
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_WIN32, 
            GetLastError());
    }

    s_Win32.deleteObject(bitmap);
    *outIcon = (PalIcon*)icon;
    return PAL_RESULT_SUCCESS;
}

void win32DestroyIcon(PalIcon* icon)
{
    DestroyIcon((HICON)icon);
}

void win32SetWindowIcon(
    PalWindow* window,
    PalIcon* icon)
{
    SendMessageW((HWND)window, WM_SETICON, ICON_BIG, (LPARAM)icon);
    SendMessageW((HWND)window, WM_SETICON, ICON_SMALL, (LPARAM)icon);
}

#endif // _WIN32