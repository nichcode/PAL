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
#include "pal2/video/cursor.h"

#include <X11/Xcursor/Xcursor.h>
#include <X11/cursorfont.h>

PalResult x11CreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** cursor)
{
    XcursorImage* image = g_X11.cursorImageCreate(info->width, info->height);
    image->xhot = info->xHotspot;
    image->yhot = info->yHotspot;

    // convert from RGBA8 to ARGB32
    for (int i = 0; i < info->width * info->height; i++) {
        uint8_t r = info->pixels[i * 4 + 0]; // Red
        uint8_t g = info->pixels[i * 4 + 1]; // Green
        uint8_t b = info->pixels[i * 4 + 2]; // Blue
        uint8_t a = info->pixels[i * 4 + 3]; // Alpha

        // clang-format off
        image->pixels[i] = ((unsigned long)a << 24) |
                           ((unsigned long)r << 16) |
                           ((unsigned long)g << 8) |
                           ((unsigned long)b);
        // clang-format on
    }

    Cursor _cursor = g_X11.cursorImageLoadCursor(g_X11.display, image);
    if (!_cursor) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    g_X11.cursorImageDestroy(image);

    *cursor = TO_PAL_HANDLE(PalCursor, _cursor);
    return PAL_RESULT_SUCCESS;
}

PalResult x11CreateCursorFrom(
    PalCursorType type,
    PalCursor** cursor)
{
    int shape;
    Cursor _cursor;

    switch (type) {
        case PAL_CURSOR_TYPE_ARROW: {
            shape = XC_left_ptr;
            break;
        }

        case PAL_CURSOR_TYPE_HAND: {
            shape = XC_hand2;
            break;
        }

        case PAL_CURSOR_TYPE_CROSS: {
            shape = XC_cross;
            break;
        }

        case PAL_CURSOR_TYPE_IBEAM: {
            shape = XC_xterm;
            break;
        }

        case PAL_CURSOR_TYPE_WAIT: {
            shape = XC_watch;
            break;
        }
    }

    _cursor = g_X11.createFontCursor(g_X11.display, shape);
    
    *cursor = TO_PAL_HANDLE(PalCursor, _cursor);
    return PAL_RESULT_SUCCESS;
}

void x11DestroyCursor(PalCursor* cursor)
{
    g_X11.freeCursor(g_X11.display, FROM_PAL_HANDLE(Cursor, cursor));
}

void x11ShowCursor(PalBool show)
{
    (void)show;
}

void x11ClipCursor(
    PalWindow* window,
    PalBool clip)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);

    if (clip) {
        g_X11.grabPointer(
            g_X11.display,
            xWin,
            True,
            0,
            GrabModeAsync,
            GrabModeAsync,
            xWin,
            None,
            CurrentTime);

    } else {
        g_X11.ungrabPointer(g_X11.display, CurrentTime);
    }
}

void x11GetCursorPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    Window root, child;
    int rootX, rootY, winX, winY;
    unsigned int mask;

    g_X11.queryPointer(g_X11.display, xWin, &root, &child, &rootX, &rootY, &winX, &winY, &mask);
    if (x) {
        *x = winX;
    }

    if (y) {
        *y = winY;
    }
}

void x11SetCursorPos(
    PalWindow* window,
    int32_t x,
    int32_t y)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    g_X11.warpPointer(g_X11.display, None, xWin, 0, 0, 0, 0, x, y);
    g_X11.flush(g_X11.display);
}

void x11SetWindowCursor(
    PalWindow* window,
    PalCursor* cursor)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    Window xCursor = FROM_PAL_HANDLE(Cursor, cursor);

    if (xCursor) {
        g_X11.defineCursor(g_X11.display, xWin, xCursor);

    } else {
        g_X11.undefineCursor(g_X11.display, xWin);
    }

    g_X11.flush(g_X11.display);
}

#endif // PAL_HAS_X11_BACKEND