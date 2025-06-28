
#include "pal_pch.h"
#include "pal_video_c.h"

bool _PCALL palInitVideo(const PalAllocator* allocator)
{
    if (s_Video.initialized) {
        return PAL_TRUE;
    }

    bool success = PAL_FALSE;
    success = palSetAllocator(&s_Video.allocator, allocator);
    if (!success) {
        return PAL_FALSE;
    }

    success = palRegisterWindowClass();
    if (!success) {
        palSetError(PAL_PLATFORM_ERROR);
        return PAL_FALSE;
    }

    success = palLoadLibraries();
    if (!success) {
        palSetError(PAL_PLATFORM_ERROR);
        return PAL_FALSE;
    }

    // init window hashmap
    s_Video.map = palCreateHashMap(&s_Video.allocator, PAL_MAX_WINDOWS);
    s_Video.nextWindowID = 1;

    s_Video.initialized = PAL_TRUE;
    return success;
}

void _PCALL palShutdownVideo()
{
    if (s_Video.initialized) {
        palUnloadLibraries();
        palDestroyHashMap(&s_Video.map);
    }

    palUnregisterWindowClass();
    s_Video.initialized = PAL_FALSE;
}

bool _PCALL palIsVideoInit()
{
    return s_Video.initialized;
}

int _PCALL palGetDisplayCount()
{
    PAL_CHECK_VIDEO(0);
    return s_Video.displayCount;
}

const PalDisplay* _PCALL palGetPrimaryDisplay()
{
    if (s_Video.displayCount) {
        return &s_Video.displays[0];
    }
    return PAL_NULL;
}

const PalDisplay* _PCALL palGetDisplay(int index)
{
    if (s_Video.displayCount && (index >= 0 && index < s_Video.displayCount)) {
        return &s_Video.displays[index];
    }
    return PAL_NULL;
}

const char* _PCALL palGetWindowTitle(PalWindow* window)
{
    PAL_CHECK_WINDOW(window, PAL_NULL);
    return window->title;
}

void _PCALL palGetWindowPos(PalWindow* window, int* x, int* y)
{
    PAL_CHECK_WINDOW(window, );
    if (x) {
        *x = window->x;
    }

    if (y) {
        *y = window->y;
    }
}

void _PCALL palGetWindowSize(PalWindow* window, Uint32* width, Uint32* height)
{
    PAL_CHECK_WINDOW(window, );
    if (width) {
        *width = window->width;
    }

    if (height) {
        *height = window->height;
    }
}

PalWindowFlags _PCALL palGetWindowFlags(PalWindow* window)
{
    PAL_CHECK_WINDOW(window, 0);
    return window->flags;
}

PalWindowID _PCALL palGetWindowID(PalWindow* window)
{
    PAL_CHECK_WINDOW(window, 0);
    return window->id;
}

bool _PCALL palIsWindowMaximized(PalWindow* window)
{
    PAL_CHECK_WINDOW(window, PAL_FALSE);
    return window->maximized;
}

bool _PCALL palIsWindowMinimized(PalWindow* window)
{
    PAL_CHECK_WINDOW(window, PAL_FALSE);
    return window->minimized;
}

bool _PCALL palIsWindowHidden(PalWindow* window)
{
    PAL_CHECK_WINDOW(window, PAL_FALSE);
    return window->hidden;
}

bool _PCALL palIsWindowFullScreen(PalWindow* window)
{
    PAL_CHECK_WINDOW(window, PAL_FALSE);
    return window->fullscreen;
}