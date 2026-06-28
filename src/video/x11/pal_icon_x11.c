
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_X11_BACKEND == 1
#include "pal_x11.h"

PalResult xCreateIcon(
    const PalIconCreateInfo* info,
    PalIcon** outIcon)
{
    uint64_t totalPixels = 2 + (uint64_t)(info->width * info->height);
    uint64_t totalBytes = sizeof(unsigned long) * totalPixels;
    unsigned long* icon = palAllocate(s_X11.allocator, totalBytes, 0);
    if (!icon) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    // store width and height and populate data with icon pixels
    // [width][height][pixels]
    icon[0] = (unsigned long)info->width;
    icon[1] = (unsigned long)info->height;

    // convert from RGBA8 to ARGB32
    for (int i = 0; i < info->width * info->height; i++) {
        uint8_t r = info->pixels[i * 4 + 0]; // Red
        uint8_t g = info->pixels[i * 4 + 1]; // Green
        uint8_t b = info->pixels[i * 4 + 2]; // Blue
        uint8_t a = info->pixels[i * 4 + 3]; // Alpha

        // clang-format off
        icon[2 + i] = ((unsigned long)a << 24) |
                      ((unsigned long)r << 16) |
                      ((unsigned long)g << 8) |
                      ((unsigned long)b);
        // clang-format on
    }

    *outIcon = TO_PAL_HANDLE(PalIcon, icon);
    return PAL_RESULT_SUCCESS;
}

void xDestroyIcon(PalIcon* icon)
{
    if (icon) {
        palFree(s_X11.allocator, icon);
    }
}

PalResult xSetWindowIcon(
    PalWindow* window,
    PalIcon* icon)
{
    WindowData* winData = nullptr;
    Window xWin = FROM_PAL_HANDLE(Window, window);
    s_X11.findContext(s_X11.display, xWin, s_X11.dataID, (XPointer*)&winData);
    if (!winData) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    unsigned long* iconData = FROM_PAL_HANDLE(unsigned long*, icon);
    uint64_t totalPixels = 2 + iconData[0] * iconData[1];
    s_X11.changeProperty(
        s_X11.display,
        xWin,
        s_X11Atoms._NET_WM_ICON,
        XA_CARDINAL,
        32,
        PropModeReplace,
        (unsigned char*)iconData,
        (int)totalPixels);

    s_X11.flush(s_X11.display);
    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_X11_BACKEND