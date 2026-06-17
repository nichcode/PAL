
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef __linux__
#if PAL_HAS_X11_BACKEND == 1

#include "pal_x11.h"
#include "pal_shared.h"

PalResult xCreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** outCursor)
{
    XcursorImage* image = s_X11.cursorImageCreate(info->width, info->height);
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

    Cursor cursor = s_X11.cursorImageLoadCursor(s_X11.display, image);
    if (!cursor) {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    s_X11.cursorImageDestroy(image);
    *outCursor = TO_PAL_HANDLE(PalCursor, cursor);
    return PAL_RESULT_SUCCESS;
}

PalResult xCreateCursorFrom(
    PalCursorType type,
    PalCursor** outCursor)
{
    int shape;
    Cursor cursor;
    switch (type) {
        case PAL_CURSOR_ARROW: {
            shape = XC_left_ptr;
            break;
        }

        case PAL_CURSOR_HAND: {
            shape = XC_hand2;
            break;
        }

        case PAL_CURSOR_CROSS: {
            shape = XC_cross;
            break;
        }

        case PAL_CURSOR_IBEAM: {
            shape = XC_xterm;
            break;
        }

        case PAL_CURSOR_WAIT: {
            shape = XC_watch;
            break;
        }
    }

    cursor = s_X11.createFontCursor(s_X11.display, shape);
    *outCursor = TO_PAL_HANDLE(PalCursor, cursor);
    return PAL_RESULT_SUCCESS;
}

void xDestroyCursor(PalCursor* cursor)
{
    s_X11.freeCursor(s_X11.display, FROM_PAL_HANDLE(Cursor, cursor));
}

void xShowCursor(PalBool show)
{
    // x11 does not support Hiding and showing cursor
    return;
}

PalResult xClipCursor(
    PalWindow* window,
    PalBool clip)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (clip) {
        s_X11.grabPointer(
            s_X11.display,
            xWin,
            True,
            0,
            GrabModeAsync,
            GrabModeAsync,
            xWin,
            None,
            CurrentTime);

    } else {
        s_X11.ungrabPointer(s_X11.display, CurrentTime);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult xGetCursorPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    Window root, rootChild;
    int rootX, rootY, winX, winY;
    unsigned int mask;
    s_X11.queryPointer(s_X11.display, xWin, &root, &rootChild, &rootX, &rootY, &winX, &winY, &mask);

    if (x) {
        *x = winX;
    }

    if (y) {
        *y = winY;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult xSetCursorPos(
    PalWindow* window,
    int32_t x,
    int32_t y)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    s_X11.warpPointer(s_X11.display, None, xWin, 0, 0, 0, 0, x, y);

    s_X11.flush(s_X11.display);
    return PAL_RESULT_SUCCESS;
}

PalResult xSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    Window xCursor = FROM_PAL_HANDLE(Cursor, cursor);
    if (xCursor) {
        s_X11.defineCursor(s_X11.display, xWin, xCursor);

    } else {
        s_X11.undefineCursor(s_X11.display, xWin);
    }

    s_X11.flush(s_X11.display);
    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_X11_BACKEND
#endif // __linux__