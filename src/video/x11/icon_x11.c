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

#if PAL_HAS_X11_BACKEND

#include "x11_helper.h"
#include "pal2/video/icon.h"
#include <X11/Xatom.h>

PalResult x11CreateIcon(
    const PalIconCreateInfo* info,
    PalIcon** icon)
{
    uint64_t totalPixels = 2 + (uint64_t)(info->width * info->height);
    uint64_t totalBytes = sizeof(unsigned long) * totalPixels;
    unsigned long* _icon = palAllocate(g_X11.allocator, totalBytes, 0);
    if (!_icon) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    // store width and height and populate data with icon pixels
    // [width][height][pixels]
    _icon[0] = (unsigned long)info->width;
    _icon[1] = (unsigned long)info->height;

    // convert from RGBA8 to ARGB32
    for (uint32_t i = 0; i < info->width * info->height; i++) {
        uint8_t r = info->pixels[i * 4 + 0]; // Red
        uint8_t g = info->pixels[i * 4 + 1]; // Green
        uint8_t b = info->pixels[i * 4 + 2]; // Blue
        uint8_t a = info->pixels[i * 4 + 3]; // Alpha

        // clang-format off
        _icon[2 + i] = ((unsigned long)a << 24) |
                      ((unsigned long)r << 16) |
                      ((unsigned long)g << 8) |
                      ((unsigned long)b);
        // clang-format on
    }

    *icon = TO_PAL_HANDLE(PalIcon, _icon);
    return PAL_RESULT_SUCCESS;
}

void x11DestroyIcon(PalIcon* icon)
{
    palFree(g_X11.allocator, icon);
}

void x11SetWindowIcon(
    PalWindow* window,
    PalIcon* icon)
{
    WindowData* winData = nullptr;
    Window xWin = FROM_PAL_HANDLE(Window, window);
    g_X11.findContext(g_X11.display, xWin, g_X11.dataID, (XPointer*)&winData);

    unsigned long* iconData = FROM_PAL_HANDLE(unsigned long*, icon);
    uint64_t totalPixels = 2 + iconData[0] * iconData[1];
    g_X11.changeProperty(
        g_X11.display,
        xWin,
        g_X11Atoms._NET_WM_ICON,
        XA_CARDINAL,
        32,
        PropModeReplace,
        (unsigned char*)iconData,
        (int)totalPixels);

    g_X11.flush(g_X11.display);
}

#endif // PAL_HAS_X11_BACKEND