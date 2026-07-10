
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_WAYLAND_BACKEND == 1
#include "pal_wayland.h"
#include "pal_wayland_protocols.h"
#include <errno.h>

PalResult wlCreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** outCursor)
{
    WaylandCursor* cursor = nullptr;
    cursor = palAllocate(s_Wl.allocator, sizeof(WaylandCursor), 0);
    if (!cursor) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    cursor->surface = wlCompositorCreateSurface(s_Wl.compositor);
    if (!cursor->surface) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    cursor->buffer = createShmBuffer(info->width, info->height, info->pixels, PAL_TRUE);
    if (!cursor->buffer) {
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_POSIX, errno);
    }

    wlSurfaceAttach(cursor->surface, cursor->buffer, 0, 0);
    wlSurfaceCommit(cursor->surface);
    cursor->hotspotX = info->xHotspot;
    cursor->hotspotY = info->yHotspot;

    *outCursor = (PalCursor*)cursor;
    return PAL_RESULT_SUCCESS;
}

PalResult wlCreateCursorFrom(
    PalCursorType type,
    PalCursor** outCursor)
{
    const char* cursorType = nullptr;
    switch (type) {
        case PAL_CURSOR_TYPE_ARROW: {
            cursorType = "left_ptr";
            break;
        }

        case PAL_CURSOR_TYPE_HAND: {
            cursorType = "hand1";
            break;
        }

        case PAL_CURSOR_TYPE_CROSS: {
            cursorType = "crosshair";
            break;
        }

        case PAL_CURSOR_TYPE_IBEAM: {
            cursorType = "text";
            break;
        }

        case PAL_CURSOR_TYPE_WAIT: {
            cursorType = "wait";
            break;
        }
    }

    struct wl_cursor* wlCursor = nullptr;
    wlCursor = s_Wl.cursorThemeGetCursor(s_Wl.cursorTheme, cursorType);
    if (!wlCursor) {
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_POSIX, errno);
    }

    WaylandCursor* cursor = nullptr;
    cursor = palAllocate(s_Wl.allocator, sizeof(WaylandCursor), 0);
    if (!cursor) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    cursor->surface = wlCompositorCreateSurface(s_Wl.compositor);
    if (!cursor->surface) {
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_POSIX, errno);
    }

    cursor->buffer = s_Wl.cursorImageGetBuffer(wlCursor->images[0]);
    wlSurfaceAttach(cursor->surface, cursor->buffer, 0, 0);
    wlSurfaceCommit(cursor->surface);
    cursor->hotspotX = wlCursor->images[0]->hotspot_x;
    cursor->hotspotY = wlCursor->images[0]->hotspot_y;

    *outCursor = (PalCursor*)cursor;
    return PAL_RESULT_SUCCESS;
}

void wlDestroyCursor(PalCursor* cursor)
{
    WaylandCursor* waylandCursor = (WaylandCursor*)cursor;
    wlBufferDestroy(waylandCursor->buffer);
    wlSurfaceDestroy(waylandCursor->surface);
    palFree(s_Wl.allocator, waylandCursor);
}

void wlSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor)
{
    WindowData* data = wlFindWindowData(window);
    data->cursor = cursor;
}

#endif // PAL_HAS_WAYLAND_BACKEND