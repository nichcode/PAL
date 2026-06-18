
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef __linux__
#if PAL_HAS_WAYLAND_BACKEND == 1

#include "pal_wayland.h"
#include "pal_wayland_protocols.h"
#include "pal_shared.h"

PalResult wlCreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** outCursor)
{
    WaylandCursor* cursor = nullptr;
    cursor = palAllocate(s_Video.allocator, sizeof(WaylandCursor), 0);
    if (!cursor) {
        return palMakeResult(
            PAL_RESULT_OUT_OF_MEMORY, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    cursor->surface = wlCompositorCreateSurface(s_Wl.compositor);
    if (!cursor->surface) {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    cursor->buffer = createShmBuffer(info->width, info->height, info->pixels, PAL_TRUE);
    if (!cursor->buffer) {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
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
        case PAL_CURSOR_ARROW: {
            cursorType = "left_ptr";
            break;
        }

        case PAL_CURSOR_HAND: {
            cursorType = "hand1";
            break;
        }

        case PAL_CURSOR_CROSS: {
            cursorType = "crosshair";
            break;
        }

        case PAL_CURSOR_IBEAM: {
            cursorType = "text";
            break;
        }

        case PAL_CURSOR_WAIT: {
            cursorType = "wait";
            break;
        }
    }

    struct wl_cursor* wlCursor = nullptr;
    wlCursor = s_Wl.cursorThemeGetCursor(s_Wl.cursorTheme, cursorType);
    if (!wlCursor) {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    WaylandCursor* cursor = nullptr;
    cursor = palAllocate(s_Video.allocator, sizeof(WaylandCursor), 0);
    if (!cursor) {
        return palMakeResult(
            PAL_RESULT_OUT_OF_MEMORY, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    cursor->surface = wlCompositorCreateSurface(s_Wl.compositor);
    if (!cursor->surface) {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
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
    palFree(s_Video.allocator, waylandCursor);
}

void wlShowCursor(PalBool show)
{
    // not supported
    return;
}

PalResult wlClipCursor(
    PalWindow* window,
    PalBool clip)
{
    if (!(s_Video.features & PAL_VIDEO_FEATURE_CLIP_CURSOR)) {
        return palMakeResult(
            PAL_RESULT_FEATURE_NOT_SUPPORTED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult wlGetCursorPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalResult wlSetCursorPos(
    PalWindow* window,
    int32_t x,
    int32_t y)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalResult wlSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor)
{
    WindowData* data = findWindowData(window);
    if (!data) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    data->cursor = cursor;
    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_WAYLAND_BACKEND
#endif // __linux__