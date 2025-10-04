
/**

Copyright (C) 2025 Nicholas Agbo

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

// ==================================================
// Includes
// ==================================================

#include "pal/pal_video.h"

#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

// X11 headers
#include <X11/Xlib.h>

#if PAL_HAS_WAYLAND
#include <wayland-client.h>
#endif // PAL_HAS_WAYLAND

// ==================================================
// Typedefs, enums and structs
// ==================================================

typedef struct {
    void (*shutdownVideo)();
} Backend;

typedef struct {
    bool initialized;
    PalVideoFeatures features;
    const PalAllocator* allocator;
    PalEventDriver* eventDriver;
    const Backend* backend;

    // we combine both since its not taking that much memory
    void* xDisplay;
    void* xRootWindow;

    void* wlDisplay;
    void* wlCompositor;
} VideoLinux;

static VideoLinux s_Video = {0};

// ==================================================
// X11
// ==================================================

typedef Display* (*XOpenDisplayFn)(const char*);
typedef int (*XCloseDisplayFn)(Display*);

typedef struct {
    void* handle;
    XOpenDisplayFn openDisplay;
    XCloseDisplayFn closeDisplay;
} X11;

static X11 s_X11 = {0};

static void xShutdownVideo() {
    s_X11.closeDisplay((Display*)s_Video.xDisplay);
}

// ==================================================
// Internal API
// ==================================================

// X11
static Backend s_XBackend = {
    .shutdownVideo = xShutdownVideo
};

// TODO: will be iplmented layer
static Backend s_WlBackend = {
    .shutdownVideo = nullptr
};

// ==================================================
// Public API
// ==================================================

PalResult PAL_CALL palInitVideo(
    const PalAllocator* allocator,
    PalEventDriver* eventDriver) 
{
    if (s_Video.initialized) {
        return PAL_RESULT_SUCCESS;
    }

    if (allocator && (!allocator->allocate || !allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    // get backend type
    bool x11 = true;
    const char* session = getenv("XDG_SESSION_TYPE");
    if (session) {
        if (strcmp(session, "wayland") == 0) {
            x11 = false;
        }
    }

    if (x11) {
        // load X11 library
        s_X11.handle = dlopen("libX11.so", RTLD_LAZY);
        if (!s_X11.handle) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        // load procs
        s_X11.openDisplay = (XOpenDisplayFn)dlsym(s_X11.handle, "XOpenDisplay");
        s_X11.closeDisplay = (XCloseDisplayFn)dlsym(s_X11.handle, "XCloseDisplay");

        // X11 server
        Display* display = s_X11.openDisplay(nullptr);
        if (!display) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        Window rootWindow = DefaultRootWindow(display);

        // set feature flags
        s_Video.features |= PAL_VIDEO_FEATURE_BORDERLESS_WINDOW;
        s_Video.features |= PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW;
        s_Video.features |= PAL_VIDEO_FEATURE_TOOL_WINDOW; // partial
        s_Video.features |= PAL_VIDEO_FEATURE_MONITOR_SET_MODE;
        s_Video.features |= PAL_VIDEO_FEATURE_MONITOR_GET_MODE;
        s_Video.features |= PAL_VIDEO_FEATURE_MULTI_MONITORS;
        s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_SET_SIZE;
        s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_GET_SIZE;
        s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_SET_POS; // partial
        s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_GET_POS; // partial
        s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_SET_STATE; // partial
        s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_GET_STATE; // partial
        s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY;
        s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_GET_VISIBILITY;
        s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_SET_TITLE;
        s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_GET_TITLE;

        s_Video.features |= PAL_VIDEO_FEATURE_CLIP_CURSOR;
        s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY; // partial
        s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_SET_INPUT_FOCUS;
        s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_GET_INPUT_FOCUS;
        s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_SET_STYLE; // partial
        s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_GET_STYLE; // partial
        s_Video.features |= PAL_VIDEO_FEATURE_CURSOR_SET_POS;
        s_Video.features |= PAL_VIDEO_FEATURE_CURSOR_GET_POS;

        s_Video.allocator = allocator;
        s_Video.eventDriver = eventDriver;
        s_Video.backend = &s_XBackend;
        s_Video.initialized = true;
        s_Video.xDisplay = display;
        s_Video.xRootWindow = (void*)(UintPtr)rootWindow;

        return PAL_RESULT_SUCCESS;

    } else {
        // wayland
        return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
    }
}

void PAL_CALL palShutdownVideo()
{
    if (s_Video.initialized) {
        s_Video.backend->shutdownVideo();
    }
}

void PAL_CALL palUpdateVideo();

PalVideoFeatures PAL_CALL palGetVideoFeatures() 
{
    if (!s_Video.initialized) {
        return 0;
    }

    return s_Video.features;
}