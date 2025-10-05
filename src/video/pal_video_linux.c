
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
#include <X11/extensions/Xrandr.h>

#if PAL_HAS_WAYLAND
#include <wayland-client.h>
#endif // PAL_HAS_WAYLAND

// ==================================================
// Typedefs, enums and structs
// ==================================================

#define TO_PAL_HANDLE(type, val) ((type*)(UintPtr)(val))
#define FROM_PAL_HANDLE(type, handle) ((type)(UintPtr)(handle))

typedef struct {
    void (*shutdownVideo)();
    PalResult (*enumerateMonitors)(Int32*, PalMonitor**);
    PalResult (*getPrimaryMonitor)(PalMonitor**);
    PalResult (*getMonitorInfo)(PalMonitor*, PalMonitorInfo*);
    PalResult (*enumerateMonitorModes)(PalMonitor*, Int32*, PalMonitorMode*);
    PalResult (*getCurrentMonitorMode)(PalMonitor*, PalMonitorMode*);
    PalResult (*setMonitorMode)(PalMonitor*, PalMonitorMode*);
} Backend;

typedef struct {
    bool initialized;
    PalVideoFeatures features;
    const PalAllocator* allocator;
    PalEventDriver* eventDriver;
    const Backend* backend;
} VideoLinux;

static VideoLinux s_Video = {0};

typedef Display* (*XOpenDisplayFn)(const char*);
typedef int (*XCloseDisplayFn)(Display*);

typedef int (*XGetWindowAttributesFn)(
    Display*,
    Window,
    XWindowAttributes*);

typedef int (*XRRSetCrtcConfigFn)(
    Display*,
    XRRScreenResources*,
    RRCrtc,
    Time,
    int, 
    int,
    RRMode,
    Rotation,
    RROutput*,
    int);

typedef XRRScreenResources* (*XRRGetScreenResourcesFn)(
    Display*, 
    Window);

typedef RROutput (*XRRGetOutputPrimaryFn)(
    Display*, 
    Window);

typedef XRROutputInfo* (*XRRGetOutputInfoFn)(
    Display*, 
    XRRScreenResources*,
    RROutput);

typedef XRRCrtcInfo* (*XRRGetCrtcInfoFn)(
    Display*, 
    XRRScreenResources*,
    RRCrtc);

typedef void (*XRRFreeScreenResourcesFn)(
    XRRScreenResources*);

typedef void (*XRRFreeOutputInfoFn)(
    XRROutputInfo*);

typedef void (*XRRFreeCrtcInfoFn)(
    XRRCrtcInfo*);

typedef struct {
    int bpp;
    void* handle;
    void* xrandr;
    Display* display;
    Window root;
    XOpenDisplayFn openDisplay;
    XCloseDisplayFn closeDisplay;
    XGetWindowAttributesFn getWindowAttributes;
    XRRSetCrtcConfigFn setCrtcConfig;

    XRRGetScreenResourcesFn getScreenResources;
    XRRGetOutputPrimaryFn getOutputPrimary;
    XRRGetOutputInfoFn getOutputInfo;
    XRRGetCrtcInfoFn getCrtcInfo;
    XRRFreeScreenResourcesFn freeScreenResources;
    XRRFreeOutputInfoFn freeOutputInfo;
    XRRFreeCrtcInfoFn freeCrtcInfo;
} X11;

static X11 s_X11 = {0};

// ==================================================
// Internal API
// ==================================================

static int compareModes(const void* a, const void* b) 
{
    const PalMonitorMode* mode1 = (const PalMonitorMode*)a;
    const PalMonitorMode* mode2 = (const PalMonitorMode*)b;

    // compare fields
    if (mode1->width != mode2->width) {
        return mode1->width - mode2->width;
    }

    if (mode1->height != mode2->height) {
        return mode1->height - mode2->height;
    }

    if (mode1->refreshRate != mode2->refreshRate) {
        return mode1->refreshRate - mode2->refreshRate;
    }

    if (mode1->bpp != mode2->bpp) {
        return mode1->bpp - mode2->bpp;
    }
}

static RRMode xFindMode(
    XRRScreenResources* resources, 
    const PalMonitorMode* mode)
{
    for (int i = 0; i < resources->nmode; ++i) {
        XRRModeInfo* info = &resources->modes[i];

        double tmp = (double)info->hTotal * (double)info->vTotal;
        double rate = (double)info->dotClock / tmp;

        // compare with width, height and refresh rate
        if (info->width == mode->width    &&
            info->height == mode->height  &&
            (Uint32)(rate + 0.5) == mode->refreshRate) {
            return info->id;
        }
    }
    return None;
}

// ==================================================
// X11
// ==================================================

static PalResult xInitVideo() 
{
    // load X11 library
    s_X11.handle = dlopen("libX11.so", RTLD_LAZY);
    if (!s_X11.handle) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    s_X11.xrandr = dlopen("libXrandr.so.2", RTLD_LAZY);
    if (!s_X11.xrandr) {
        s_X11.xrandr = dlopen("libXrandr.so", RTLD_LAZY);
    }

    // load procs
    s_X11.openDisplay = (XOpenDisplayFn)dlsym(
        s_X11.handle, 
        "XOpenDisplay");

    s_X11.closeDisplay = (XCloseDisplayFn)dlsym(
        s_X11.handle, 
        "XCloseDisplay");

    s_X11.getWindowAttributes = (XGetWindowAttributesFn)dlsym(
        s_X11.handle, 
        "XGetWindowAttributes");

    s_X11.setCrtcConfig = (XRRSetCrtcConfigFn)dlsym(
        s_X11.handle, 
        "XRRSetCrtcConfig");

    // X11 server
    s_X11.display = s_X11.openDisplay(nullptr);
    if (!s_X11.display) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    s_X11.root = DefaultRootWindow(s_X11.display);

    // load Xrandr functions
    s_X11.getScreenResources = (XRRGetScreenResourcesFn)dlsym(
        s_X11.xrandr, 
        "XRRGetScreenResources");

    s_X11.getOutputPrimary = (XRRGetOutputPrimaryFn)dlsym(
        s_X11.xrandr, 
        "XRRGetOutputPrimary");

    s_X11.getOutputInfo = (XRRGetOutputInfoFn)dlsym(
        s_X11.xrandr, 
        "XRRGetOutputInfo");
    
    s_X11.getCrtcInfo = (XRRGetCrtcInfoFn)dlsym(
        s_X11.xrandr, 
        "XRRGetCrtcInfo");
    
    s_X11.freeScreenResources = (XRRFreeScreenResourcesFn)dlsym(
        s_X11.xrandr, 
        "XRRFreeScreenResources");
    
    s_X11.freeOutputInfo = (XRRFreeOutputInfoFn)dlsym(
        s_X11.xrandr, 
        "XRRFreeScreenResources");
    
    s_X11.freeCrtcInfo = (XRRFreeCrtcInfoFn)dlsym(
        s_X11.xrandr, 
        "XRRFreeCrtcInfo");

    // set feature flags
    if (s_X11.getScreenResources) {
        s_Video.features |= PAL_VIDEO_FEATURE_MULTI_MONITORS;
    }

    s_Video.features |= PAL_VIDEO_FEATURE_MONITOR_GET_ORIENTATION;
    s_Video.features |= PAL_VIDEO_FEATURE_BORDERLESS_WINDOW;
    s_Video.features |= PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW;
    s_Video.features |= PAL_VIDEO_FEATURE_TOOL_WINDOW; // partial
    s_Video.features |= PAL_VIDEO_FEATURE_MONITOR_SET_MODE;
    s_Video.features |= PAL_VIDEO_FEATURE_MONITOR_GET_MODE;
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

    // get root window bpp
    XWindowAttributes attr;
    s_X11.getWindowAttributes(s_X11.display, s_X11.root, &attr);
    s_X11.bpp = attr.depth;

    return PAL_RESULT_SUCCESS;
}

static void xShutdownVideo() 
{
    s_X11.closeDisplay(s_X11.display);
    dlclose(s_X11.handle);
    dlclose(s_X11.xrandr);
}

static PalResult xEnumerateMonitors(
    Int32* count,
    PalMonitor** outMonitors)
{
    int _count = 0;
    int maxCount = outMonitors ? *count : 0;
    XRRScreenResources* resources = s_X11.getScreenResources(
        s_X11.display,
        s_X11.root);
    
    for (int i = 0; i < resources->noutput; ++i) {
        RROutput output = resources->outputs[i];
        XRROutputInfo* outputInfo = s_X11.getOutputInfo(
            s_X11.display,
            resources,
            output);
        
        if (outputInfo->connection == RR_Connected && 
            outputInfo->crtc != None) {
            // a monitor
            if (outMonitors) {
                if (_count < maxCount) {
                    outMonitors[_count] = TO_PAL_HANDLE(PalMonitor, output);
                }
            }
            _count++;
        }
    }

    if (!outMonitors) {
        *count = _count;
    }
    return PAL_RESULT_SUCCESS;
}

static PalResult xGetPrimaryMonitor(PalMonitor** outMonitor)
{
    // get the primary monitor
    RROutput primary = s_X11.getOutputPrimary(
        s_X11.display,
        s_X11.root);

    if (primary) {
        *outMonitor = TO_PAL_HANDLE(PalMonitor, primary);
        return PAL_RESULT_SUCCESS;
    }

    // primary monitor not set
    XRRScreenResources* resources = s_X11.getScreenResources(
        s_X11.display,
        s_X11.root);

    if (resources->noutput > 0) {
        *outMonitor = TO_PAL_HANDLE(PalMonitor, resources->outputs[0]);
    }
    
    s_X11.freeScreenResources(resources);
    return PAL_RESULT_SUCCESS;
}

static PalResult xGetMonitorInfo(
    PalMonitor* monitor,
    PalMonitorInfo* info)
{
    XRRScreenResources* resources = s_X11.getScreenResources(
        s_X11.display,
        s_X11.root);

    XRROutputInfo* outputInfo = s_X11.getOutputInfo(
        s_X11.display,
        resources,
        FROM_PAL_HANDLE(RROutput, monitor));
    
    strcpy(info->name, outputInfo->name);
    
    // check if its primary monitor
    RROutput primary = s_X11.getOutputPrimary(
        s_X11.display,
        s_X11.root);
    
    if (!primary) {
        if (resources->noutput > 0) {
            primary = resources->outputs[0];
        }
    }

    if (monitor == TO_PAL_HANDLE(PalMonitor, primary)) {
        info->primary = true;
    }

    // get monitor pos and size
    XRRCrtcInfo* crtc = s_X11.getCrtcInfo(
        s_X11.display,
        resources,
        outputInfo->crtc);
    
    info->x = crtc->x;
    info->y = crtc->y;
    info->width = crtc->width;
    info->height = crtc->height;

    // get refresh rate
    double rate = 0;
    for (int i = 0; i < resources->nmode; ++i) {
        // check for our monitor
        if (resources->modes[i].id == crtc->mode) {
            XRRModeInfo* mode = &resources->modes[i];
            double tmp = (double)mode->hTotal * (double)mode->vTotal;
            rate = (double)mode->dotClock / tmp;
            info->refreshRate = rate + 0.5;
            break;
        }
    }

    // orientation
    switch (crtc->rotation) {
        case RR_Rotate_0: {
            info->orientation = PAL_ORIENTATION_LANDSCAPE;
            break;
        }

        case RR_Rotate_90: {
            info->orientation = PAL_ORIENTATION_PORTRAIT;
            break;
        }

        case RR_Rotate_180: {
            info->orientation = PAL_ORIENTATION_LANDSCAPE_FLIPPED;
            break;
        }

        case RR_Rotate_270: {
            info->orientation = PAL_ORIENTATION_PORTRAIT_FLIPPED;
            break;
        }

        default: {
            info->orientation = PAL_ORIENTATION_LANDSCAPE;
        }
    } 

    // get dpi
    double tmp = (double)(crtc->width * 25.4) / (double)outputInfo->mm_width;
    info->dpi = (Uint32)tmp;

    s_X11.freeCrtcInfo(crtc);
    s_X11.freeOutputInfo(outputInfo);
    s_X11.freeScreenResources(resources);

    return PAL_RESULT_SUCCESS;
}

static PalResult xEnumerateMonitorModes(
    PalMonitor* monitor,
    Int32* count,
    PalMonitorMode* modes)
{    
    Int32 modeCount = 0;
    int maxModeCount = modes ? *count : 0;

    XRRScreenResources* resources = s_X11.getScreenResources(
        s_X11.display,
        s_X11.root);
    
    // get the monitor info
    XRROutputInfo* outputInfo = s_X11.getOutputInfo(
        s_X11.display,
        resources,
        FROM_PAL_HANDLE(RROutput, monitor));

    if (!outputInfo) {
        // invalid monitor
        s_X11.freeScreenResources(resources);
        return PAL_RESULT_INVALID_MONITOR;
    }

    if (outputInfo->connection != RR_Connected) {
        // invalid monitor
        s_X11.freeScreenResources(resources);
        return PAL_RESULT_INVALID_MONITOR;
    }

    // get supported display modes
    for (int i = 0; i < outputInfo->nmode; ++i) {
        for (int j = 0; j < resources->nmode; ++j) {
            // get the display mode and check if its for our monitor
            XRRModeInfo* info = &resources->modes[j];
            if (info->id == outputInfo->modes[i]) {
                // check if user supplied a PalMonitorMode array
                if (modes) {
                    if (modeCount < maxModeCount) {
                        PalMonitorMode* mode = &modes[modeCount];
                        mode->width = info->width;
                        mode->height = info->height;
                        mode->bpp = s_X11.bpp;

                        double tmp = (double)info->hTotal * (double)info->vTotal;
                        double rate = (double)info->dotClock / tmp;
                        mode->refreshRate = rate + 0.5;
                    }
                }
                modeCount++;
            }
        }
    }

    if (!modes) {
        *count = modeCount;
    }

    s_X11.freeOutputInfo(outputInfo);
    s_X11.freeScreenResources(resources);
    
    return PAL_RESULT_SUCCESS;
}

static PalResult xGetCurrentMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    XRRScreenResources* resources = s_X11.getScreenResources(
        s_X11.display,
        s_X11.root);
    
    // get the monitor info
    XRROutputInfo* outputInfo = s_X11.getOutputInfo(
        s_X11.display,
        resources,
        FROM_PAL_HANDLE(RROutput, monitor));

    if (!outputInfo) {
        // invalid monitor
        s_X11.freeScreenResources(resources);
        return PAL_RESULT_INVALID_MONITOR;
    }

    if (outputInfo->connection != RR_Connected) {
        // invalid monitor
        s_X11.freeScreenResources(resources);
        return PAL_RESULT_INVALID_MONITOR;
    }

    // get the current display mode
    if (outputInfo->crtc) {
        XRRCrtcInfo* crtc = s_X11.getCrtcInfo(
            s_X11.display,
            resources,
            outputInfo->crtc);
        
        if (crtc) {
            // find the display mode
            XRRModeInfo* info = nullptr;
            for (int i = 0; i < resources->nmode; ++i) {
                if (resources->modes[i].id == crtc->mode) {
                    // found
                    info = &resources->modes[i];
                    break;
                }
            }

            if (mode) {
                mode->width = info->width;
                mode->height = info->height;
                mode->bpp = s_X11.bpp;

                double tmp = (double)info->hTotal * (double)info->vTotal;
                double rate = (double)info->dotClock / tmp;
                mode->refreshRate = rate + 0.5;

                s_X11.freeCrtcInfo(crtc);
            }
        }
    }

    s_X11.freeOutputInfo(outputInfo);
    s_X11.freeScreenResources(resources);
    
    return PAL_RESULT_SUCCESS;
}

static PalResult xSetMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    XRRScreenResources* resources = s_X11.getScreenResources(
        s_X11.display,
        s_X11.root);
    
    // get the monitor info
    XRROutputInfo* outputInfo = s_X11.getOutputInfo(
        s_X11.display,
        resources,
        FROM_PAL_HANDLE(RROutput, monitor));

    if (!outputInfo) {
        // invalid monitor
        s_X11.freeScreenResources(resources);
        return PAL_RESULT_INVALID_MONITOR;
    }

    if (outputInfo->connection != RR_Connected) {
        // invalid monitor
        s_X11.freeScreenResources(resources);
        return PAL_RESULT_INVALID_MONITOR;
    }

    // find the monitor display mode
    RRMode displayMode = xFindMode(resources, mode);
    if (displayMode == None) {
        s_X11.freeOutputInfo(outputInfo);
        s_X11.freeScreenResources(resources);
        return PAL_RESULT_INVALID_MONITOR_MODE;
    }

    // apply the display mode
    XRRCrtcInfo* crtc = s_X11.getCrtcInfo(
        s_X11.display,
        resources,
        outputInfo->crtc);

    RROutput output = FROM_PAL_HANDLE(RROutput, monitor);
    int ret = s_X11.setCrtcConfig(
        s_X11.display,
        resources,
        outputInfo->crtc,
        CurrentTime,
        crtc->x,
        crtc->y,
        displayMode,
        crtc->rotation,
        &output,
        1);
    
    s_X11.freeCrtcInfo(crtc);
    s_X11.freeOutputInfo(outputInfo);
    s_X11.freeScreenResources(resources);

    if (ret != Success) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }
    
    return PAL_RESULT_SUCCESS;
}

static Backend s_XBackend = {
    .shutdownVideo = xShutdownVideo,
    .enumerateMonitors = xEnumerateMonitors,
    .getMonitorInfo = xGetMonitorInfo,
    .getPrimaryMonitor = xGetPrimaryMonitor,
    .enumerateMonitorModes = xEnumerateMonitorModes,
    .getCurrentMonitorMode = xGetCurrentMonitorMode,
    .setMonitorMode = xSetMonitorMode
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
        PalResult ret = xInitVideo();
        if (ret != PAL_RESULT_SUCCESS) {
            return ret;
        }
        s_Video.backend = &s_XBackend;
    }

    s_Video.allocator = allocator;
    s_Video.eventDriver = eventDriver;
    s_Video.initialized = true;
    return PAL_RESULT_SUCCESS;
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

PalResult PAL_CALL palEnumerateMonitors(
    Int32* count,
    PalMonitor** outMonitors)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!count) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (*count == 0 && outMonitors) {
        return PAL_RESULT_INSUFFICIENT_BUFFER;
    }

    return s_Video.backend->enumerateMonitors(count, outMonitors);
}

PalResult PAL_CALL palGetPrimaryMonitor(PalMonitor** outMonitor) 
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!outMonitor) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->getPrimaryMonitor(outMonitor);
}

PalResult PAL_CALL palGetMonitorInfo(
    PalMonitor* monitor,
    PalMonitorInfo* info) 
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!info) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->getMonitorInfo(monitor, info);
}

PalResult PAL_CALL palEnumerateMonitorModes(
    PalMonitor* monitor,
    Int32* count,
    PalMonitorMode* modes)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!monitor || !count) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (*count == 0 && modes) {
        return PAL_RESULT_INSUFFICIENT_BUFFER;
    }

    PalResult ret = s_Video.backend->enumerateMonitorModes(
        monitor,
        count,
        modes
    );

    if (ret == PAL_RESULT_SUCCESS && modes) {
        // sort the modes so that they are lowest to highest
        qsort(modes, *count, sizeof(PalMonitorMode), compareModes);
    }

    return ret;
}

PalResult PAL_CALL palGetCurrentMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!monitor || !mode) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->getCurrentMonitorMode(monitor, mode);
}

PalResult PAL_CALL palSetMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!monitor || !mode) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->setMonitorMode(monitor, mode);
}