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

#if PAL_HAS_WAYLAND_BACKEND

#include "wayland_helper.h"
#include "pal2/core/memory.h"
#include "pal2/video/cursor.h"
#include "client_protocol.h"
#include <errno.h>

/* =========================================================================
 * Helper
 * ========================================================================= */

void wlApplyWindowCursor(WindowData* data)
{
    if (!data->pointer) {
        return;
    }

    // revert to default "left_ptr" if user passed nullptr
    WaylandCursor* active = data->cursor ? data->cursor : g_Wl.defaultCursor;

    if (active) {
        wlPointerSetCursor(
            data->pointer,
            data->pointerEnterSerial,
            active->surface,
            active->hotspotX,
            active->hotspotY);
    }

    g_Wl.displayFlush(g_Wl.display);
}


/* =========================================================================
 * API
 * ========================================================================= */

PalResult wlCreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** cursor)
{
    WaylandCursor* _cursor = palAllocate(g_Wl.allocator, sizeof(WaylandCursor), 0);
    if (!_cursor) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    _cursor->surface = wlCompositorCreateSurface(g_Wl.compositor);
    if (!_cursor->surface) {
        palFree(g_Wl.allocator, _cursor);
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    _cursor->buffer = wlCreateShmBuffer((int)info->width, (int)info->height, info->pixels, PAL_TRUE);
    if (!_cursor->buffer) {
        wlSurfaceDestroy(_cursor->surface);
        palFree(g_Wl.allocator, _cursor);
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE,
            PAL_RESULT_SOURCE_POSIX,
            (uint32_t)errno);
    }

    wlSurfaceAttach(_cursor->surface, _cursor->buffer, 0, 0);
    wlSurfaceCommit(_cursor->surface);
    _cursor->hotspotX = info->xHotspot;
    _cursor->hotspotY = info->yHotspot;
    _cursor->isCustom = PAL_TRUE;

    *cursor = (PalCursor*)_cursor;
    return PAL_RESULT_SUCCESS;
}

PalResult wlCreateCursorFrom(
    PalCursorType type,
    PalCursor** cursor)
{
    const char *names[3] = { "left_ptr", nullptr, nullptr };
    switch (type) {
    case PAL_CURSOR_TYPE_HAND:
        names[0] = "pointer";
        names[1] = "hand2";
        names[2] = "hand1";
        break;

    case PAL_CURSOR_TYPE_CROSS:
        names[0] = "crosshair";
        names[1] = "cross";
        break;

    case PAL_CURSOR_TYPE_IBEAM:
        names[0] = "text";
        names[1] = "xterm";
        break;

    case PAL_CURSOR_TYPE_WAIT:
        names[0] = "wait";
        names[1] = "watch";
        break;
    }

    struct wl_cursor* wlCursor = nullptr;
    for (int i = 0; names[i] != NULL; i++) {
        wlCursor = g_Wl.cursorThemeGetCursor(g_Wl.cursorTheme, names[i]);
        if (wlCursor) {
            break;
        }
    }

    if (!wlCursor || wlCursor->image_count == 0) {
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE,
            PAL_RESULT_SOURCE_POSIX,
            (uint32_t)errno);
    }

    WaylandCursor* _cursor = palAllocate(g_Wl.allocator, sizeof(WaylandCursor), 0);
    if (!_cursor) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    _cursor->surface = wlCompositorCreateSurface(g_Wl.compositor);
    if (!_cursor->surface) {
        palFree(g_Wl.allocator, _cursor);
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE,
            PAL_RESULT_SOURCE_POSIX,
            (uint32_t)errno);
    }

    _cursor->buffer = g_Wl.cursorImageGetBuffer(wlCursor->images[0]);
    wlSurfaceAttach(_cursor->surface, _cursor->buffer, 0, 0);
    wlSurfaceCommit(_cursor->surface);
    _cursor->hotspotX = (int)wlCursor->images[0]->hotspot_x;
    _cursor->hotspotY = (int)wlCursor->images[0]->hotspot_y;
    _cursor->isCustom = PAL_FALSE;

    *cursor = (PalCursor*)_cursor;
    return PAL_RESULT_SUCCESS;
}

void wlDestroyCursor(PalCursor* cursor)
{
    WaylandCursor* waylandCursor = (WaylandCursor*)cursor;
    if (waylandCursor->isCustom && waylandCursor->buffer) {
        wlBufferDestroy(waylandCursor->buffer);
    }

    wlSurfaceDestroy(waylandCursor->surface);
    palFree(g_Wl.allocator, waylandCursor);
}

void wlShowCursor(PalBool show)
{
    (void)show;
}

void wlClipCursor(
    PalWindow* window,
    PalBool clip)
{
    (void)window;
    (void)clip;
}

void wlGetCursorPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y)
{
    (void)window;
    (void)x;
    (void)y;
}

void wlSetCursorPos(
    PalWindow* window,
    int32_t x,
    int32_t y)
{
    (void)window;
    (void)x;
    (void)y;
}

void wlSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor)
{
    WindowData* data = wlFindWindowData(window);
    if (data) {
        data->cursor = (WaylandCursor*)cursor;
        wlApplyWindowCursor(data);
    }
}

#endif // PAL_HAS_WAYLAND_BACKEND