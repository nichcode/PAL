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

PalResult win32CreateBitmap(
    uint32_t width, 
    uint32_t height, 
    const uint8_t* pixels,
    HBITMAP* bitmap)
{
    BITMAPV5HEADER bitInfo = {0};
    bitInfo.bV5Size = sizeof(BITMAPV5HEADER);
    bitInfo.bV5Width = (LONG)width;
    bitInfo.bV5Height = -(LONG)height; // this is topdown by default

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
    HBITMAP _bitmap = g_Win32.createDIBSection(
        hdc, 
        (BITMAPINFO*)&bitInfo, 
        DIB_RGB_COLORS, 
        &dibPixels, 
        nullptr, 
        0);
    // clang-format on

    if (!_bitmap) {
        ReleaseDC(nullptr, hdc);
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, GetLastError());
    }

    ReleaseDC(nullptr, hdc);

    // convert RGBA to BGRA
    uint8_t* tmp = (uint8_t*)dibPixels;
    for (uint32_t i = 0; i < width * height; i++) {
        uint8_t r = pixels[i * 4 + 0];
        uint8_t g = pixels[i * 4 + 1];
        uint8_t b = pixels[i * 4 + 2];
        uint8_t a = pixels[i * 4 + 3];

        // premultiply only if alpha is not 0
        if (a == 0) {
            r = g = b = 0;
        } else {
            r = (uint8_t)((r * a) / 255);
            g = (uint8_t)((g * a) / 255);
            b = (uint8_t)((b * a) / 255);
        }

        tmp[i * 4 + 0] = b;
        tmp[i * 4 + 1] = g;
        tmp[i * 4 + 2] = r;
        tmp[i * 4 + 3] = a;
    }

    *bitmap = _bitmap;
    return PAL_RESULT_SUCCESS;
}

void win32DestroyBitmap(HBITMAP map)
{
    g_Win32.deleteObject(map);
}

#endif // _WIN32