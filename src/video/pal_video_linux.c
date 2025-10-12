
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
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>

// X11 headers
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>

// Wayland headers
#if PAL_HAS_WAYLAND
#include <wayland-client.h>
#endif // PAL_HAS_WAYLAND

// ==================================================
// Typedefs, enums and structs
// ==================================================

#define TO_PAL_HANDLE(type, val) ((type*)(UintPtr)(val))
#define FROM_PAL_HANDLE(type, handle) ((type)(UintPtr)(handle))

typedef struct {
    bool skipConfigure;
    bool skipState;
    bool used;
    int x;
    int y;
    Uint32 w;
    int dpi;
    Uint32 h;
    PalWindowState state;
} WindowData;

typedef struct {
    bool used;
    int dpi;
    int x;
    int y;
    Uint32 w;
    Uint32 h;
    PalMonitor* monitor;
} MonitorData;

// ==================================================
// X11 Typedefs, enums and structs
// ==================================================

#pragma region X11 Typedefs
#define X_INTERN(x) s_X11Atoms.x = s_X11.internAtom(s_X11.display, #x, False)

#define RANDR_SCREEN_CHANGE_EVENT 1040
#define RANDR_NOTIFY_EVENT 1041

// optionally, needed to create visual from FBConfig
#define GLX_FBCONFIG_ID 0x8012
typedef struct __GLXFBConfigRec *GLXFBConfig;

typedef GLXFBConfig* (*GLXGetFBConfigsFn)(
    Display*,
    int,
    int*);

typedef int (*GLXGetFBConfigAttribFn)(
    Display*,
    GLXFBConfig,
    int,
    int*);

typedef XVisualInfo* (*GLXGetVisualFromFBConfigFn)(
    Display*,
    GLXFBConfig);

typedef XVisualInfo* (*GLXGetProcAddressFn)(const unsigned char*);

typedef Display* (*XOpenDisplayFn)(const char*);
typedef int (*XCloseDisplayFn)(Display*);

typedef int (*XGetWindowAttributesFn)(
    Display*,
    Window,
    XWindowAttributes*);

typedef int (*XGetWindowPropertyFn)(
    Display*,
    Window,
    Atom,
    long,		
    long,		
    Bool,		
    Atom,		
    Atom*,		
    int*,		
    unsigned long*,	
    unsigned long*,	
    unsigned char**);

typedef Atom (*XInternAtomFn)(
    Display*,	
    _Xconst char*,
    Bool);

typedef Window (*XGetSelectionOwnerFn)(
    Display*,
    Atom);

typedef Window (*XCreateWindowFn)(
    Display*,
    Window,
    int,
    int,
    unsigned int,
    unsigned int,
    unsigned int,
    int,
    unsigned int,
    Visual*,
    unsigned long,
    XSetWindowAttributes*);

typedef int (*XChangePropertyFn)(
    Display*,
    Window,
    Atom,
    Atom,
    int,
    int,
    _Xconst unsigned char*,
    int);

typedef int (*XFlushFn)(Display*);

typedef Colormap (*XCreateColormapFn)(
    Display*,
    Window,
    Visual*,
    int);

typedef int (*XFreeColormapFn)(
    Display*,
    Colormap);

typedef int (*XDestroyWindowFn)(
    Display*,
    Window);

typedef int (*XStoreNameFn)(
    Display*,
    Window,
    _Xconst char*);

typedef int (*XMapWindowFn)(
    Display*,
    Window);

typedef int (*XUnmapWindowFn)(
    Display*,
    Window);

typedef int (*XMatchVisualInfoFn)(
    Display*,
    int,
    int,
    int,
    XVisualInfo*);

typedef int (*XPendingFn)(Display*);

typedef int (*XSetWMProtocolsFn)(
    Display*,
    Window,
    Atom*,
    int);

typedef int (*XNextEventFn)(
    Display*,
    XEvent*);

typedef int (*XSetWMNormalHintsFn)(
    Display*,
    Window,
    XSizeHints*);

typedef int (*XSendEventFn)(
    Display*,
    Window,
    Bool,
    long,
    XEvent*);

typedef int (*XMoveWindowFn)(
    Display*,
    Window,
    int,
    int);

typedef int (*XResizeWindowFn)(
    Display*,
    Window,
    unsigned int,
    unsigned int);

typedef int (*XIconifyWindowFn)(
    Display*,
    Window,
    int);

typedef XErrorHandler (*XSetErrorHandlerFn)(XErrorHandler);

typedef int (*XSyncFn)(
    Display*,
    Bool);

typedef int (*XSaveContextFn)(
    Display*,
    XID,
    XContext,
    _Xconst char*);

typedef int (*XFindContextFn)(
    Display*,
    XID,
    XContext,
    XPointer*);

typedef XrmQuark (*XrmUniqueQuarkFn)(void);

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

typedef void (*XRRSelectInputFn)(
    Display*, 
    Window, 
    int);

typedef int (*XRRQueryExtensionFn)(
    Display*,
	int*,
	int*);

typedef XClassHint *(*XAllocClassHintFn)(void);

typedef int (*XSetClassHintFn)(
    Display*,
    Window,
    XClassHint*);

typedef int (*XFreeFn)(void*);

typedef struct 
{
    bool unicodeTitle;

    Atom WM_DELETE_WINDOW;
    Atom _NET_SUPPORTED;
    Atom _NET_WM_STATE;
    Atom _NET_WM_STATE_ABOVE;
    Atom _NET_WM_STATE_HIDDEN;
    Atom _NET_WM_STATE_MAXIMIZED_VERT;
    Atom _NET_WM_STATE_MAXIMIZED_HORZ;
    Atom _NET_WM_WINDOW_TYPE_UTILITY;
    Atom _NET_WM_DESKTOP;
    Atom _NET_WM_STATE_DEMANDS_ATTENTIONS;
    Atom _NET_WM_WINDOW_OPACITY;

    Atom _NET_WM_NAME;
    Atom UTF8_STRING;
    Atom _NET_WM_WINDOW_TYPE;
    Atom _NET_WM_WINDOW_TYPE_SPLASH;
    Atom _NET_WM_PID;
    Atom _WM_CLASS;
    Atom _NET_ACTIVE_WINDOW;
    Atom _NET_WM_ICON;
} X11Atoms;

typedef struct {
    bool error;
    bool skipScreenEvent;
    bool skipNotifyEvent;
    int bpp;
    int screen;
    int rrEventBase;
    void* handle;
    void* xrandr;
    void* opengl;
    Display* display;
    Window root;
    GC gc;
    XContext dataID;
    const char* className;

    XOpenDisplayFn openDisplay;
    XCloseDisplayFn closeDisplay;
    XGetWindowAttributesFn getWindowAttributes;
    XGetWindowPropertyFn getWindowProperty;
    XInternAtomFn internAtom;
    XGetSelectionOwnerFn getSelectionOwner;
    XFreeColormapFn freeColormap;
    XStoreNameFn storeName;
    XChangePropertyFn changeProperty;
    XFlushFn flush;
    XCreateColormapFn createColormap;
    XMapWindowFn mapWindow;
    XUnmapWindowFn unmapWindow;

    XCreateWindowFn createWindow;
    XDestroyWindowFn destroyWindow;
    XMatchVisualInfoFn matchVisualInfo;
    XPendingFn pending;
    XSetWMProtocolsFn setWMProtocols;
    XNextEventFn nextEvent;
    XSetWMNormalHintsFn setWMNormalHints;
    XSendEventFn sendEvent;
    XMoveWindowFn moveWindow;
    XResizeWindowFn resizeWindow;
    XIconifyWindowFn iconifyWindow;

    XSetErrorHandlerFn setErrorHandler;
    XSyncFn sync;
    XSaveContextFn saveContext;
    XFindContextFn findContext;
    XrmUniqueQuarkFn uniqueContext;

    XRRSetCrtcConfigFn setCrtcConfig;
    XRRGetScreenResourcesFn getScreenResources;
    XRRGetOutputPrimaryFn getOutputPrimary;
    XRRGetOutputInfoFn getOutputInfo;
    XRRGetCrtcInfoFn getCrtcInfo;
    XRRFreeScreenResourcesFn freeScreenResources;
    XRRFreeOutputInfoFn freeOutputInfo;
    XRRFreeCrtcInfoFn freeCrtcInfo;
    XRRSelectInputFn selectRRInput;
    XRRQueryExtensionFn queryRRExtension;

    XAllocClassHintFn allocClassHint;
    XSetClassHintFn setClassHint;
    XFreeFn free;

    // opengl
    GLXGetFBConfigsFn glxGetFBConfigs;
    GLXGetFBConfigAttribFn glxGetFBConfigAttrib;
    GLXGetVisualFromFBConfigFn glxGetVisualFromFBConfig;
} X11;

static X11 s_X11 = {0};
static X11Atoms s_X11Atoms = {0};

#pragma endregion

typedef struct {
    void (*shutdownVideo)();
    void (*updateVideo)();
    PalResult (*enumerateMonitors)(Int32*, PalMonitor**);
    PalResult (*getPrimaryMonitor)(PalMonitor**);
    PalResult (*getMonitorInfo)(PalMonitor*, PalMonitorInfo*);
    PalResult (*enumerateMonitorModes)(PalMonitor*, Int32*, PalMonitorMode*);
    PalResult (*getCurrentMonitorMode)(PalMonitor*, PalMonitorMode*);
    PalResult (*setMonitorMode)(PalMonitor*, PalMonitorMode*);
    PalResult (*validateMonitorMode)(PalMonitor*, PalMonitorMode*);
    PalResult (*setMonitorOrientation)(PalMonitor*, PalOrientation);

    PalResult (*createWindow)(const PalWindowCreateInfo*, PalWindow**);
    void (*destroyWindow)(PalWindow*);
    PalResult (*setWindowOpacity)(PalWindow*, float);

    PalResult (*createIcon)(const PalIconCreateInfo*, PalIcon**);
    void (*destroyIcon)(PalIcon*);
    PalResult (*setWindowIcon)(PalWindow*, PalIcon*);
} Backend;

typedef struct {
    bool initialized;
    Int32 maxWindowData;
    Int32 maxMonitorData;
    Int32 pixelFormat;
    PalVideoFeatures features;
    const PalAllocator* allocator;
    PalEventDriver* eventDriver;
    const Backend* backend;
    WindowData* windowData;
    MonitorData* monitorData;
} VideoLinux;

static VideoLinux s_Video = {0};

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

static WindowData* getFreeWindowData() 
{
    for (int i = 0; i < s_Video.maxWindowData; ++i) {
        if (!s_Video.windowData[i].used) {
            s_Video.windowData[i].used = true;
            return &s_Video.windowData[i];
        }
    }

    // resize the data array
    // It is rare for a user to create and manage
    // 32 windows at the same time
    WindowData* data = nullptr;
    int count = s_Video.maxWindowData * 2; // double the size
    int freeIndex = s_Video.maxWindowData + 1;
    data = palAllocate(s_Video.allocator, sizeof(WindowData) * count, 0);
    if (data) {
        memcpy(
            data, 
            s_Video.windowData, 
            s_Video.maxWindowData * sizeof(WindowData));

        palFree(s_Video.allocator, s_Video.windowData);
        s_Video.windowData = data;
        s_Video.maxWindowData = count;

        s_Video.windowData[freeIndex].used = true;
        return &s_Video.windowData[freeIndex];
    }
    return nullptr;
}

static void resetMonitorData() 
{
    memset(
        s_Video.monitorData, 
        0, 
        s_Video.maxMonitorData * sizeof(MonitorData));
}

static MonitorData* getFreeMonitorData() 
{
    for (int i = 0; i < s_Video.maxMonitorData; ++i) {
        if (!s_Video.monitorData[i].used) {
            s_Video.monitorData[i].used = true;
            return &s_Video.monitorData[i];
        }
    }

    // resize the data array
    // this will almost not reach here since most setups are 1-4 monitors
    MonitorData* data = nullptr;
    int count = s_Video.maxMonitorData * 2; // double the size
    int freeIndex = s_Video.maxMonitorData + 1;
    data = palAllocate(s_Video.allocator, sizeof(MonitorData) * count, 0);
    if (data) {
        memcpy(
            data, 
            s_Video.monitorData, 
            s_Video.maxMonitorData * sizeof(MonitorData));

        palFree(s_Video.allocator, s_Video.monitorData);
        s_Video.monitorData = data;
        s_Video.maxWindowData = count;

        s_Video.monitorData[freeIndex].used = true;
        return &s_Video.monitorData[freeIndex];
    }
    return nullptr;
}

static MonitorData* findMonitorData(PalMonitor* monitor) 
{
    for (int i = 0; i < s_Video.maxMonitorData; ++i) {
        if (s_Video.monitorData[i].used && 
            s_Video.monitorData[i].monitor == monitor) {
            return &s_Video.monitorData[i];
        }
    }
}

static void freeMonitorData(PalMonitor* monitor) 
{
    for (int i = 0; i < s_Video.maxMonitorData; ++i) {
        if (s_Video.monitorData[i].used && 
            s_Video.monitorData[i].monitor == monitor) {
            s_Video.monitorData[i].used = false;
        }
    }
}

// ==================================================
// X11 API
// ==================================================

#pragma region X11 API

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

static void xCheckFeatures()
{
    // cache this atoms
    X_INTERN(WM_DELETE_WINDOW);
    X_INTERN(_NET_SUPPORTED);
    X_INTERN(_NET_WM_STATE);
    X_INTERN(_NET_WM_STATE_ABOVE);
    X_INTERN(_NET_WM_STATE_MAXIMIZED_VERT);
    X_INTERN(_NET_WM_STATE_MAXIMIZED_HORZ);
    X_INTERN(_NET_WM_NAME);
    X_INTERN(UTF8_STRING);
    X_INTERN(_NET_WM_WINDOW_TYPE_UTILITY);
    X_INTERN(_NET_WM_DESKTOP);
    X_INTERN(_NET_WM_STATE_DEMANDS_ATTENTIONS);
    X_INTERN(_NET_WM_WINDOW_OPACITY);
    X_INTERN(_NET_WM_WINDOW_TYPE);
    X_INTERN(_NET_WM_WINDOW_TYPE_SPLASH);
    X_INTERN(_NET_WM_PID);
    X_INTERN(_WM_CLASS);
    X_INTERN(_NET_ACTIVE_WINDOW);
    X_INTERN(_NET_WM_ICON);

    // check for support from the window manager
    Atom type;
    int format;
    unsigned long count, bytesAfters;
    Atom* supportedAtoms = nullptr;
    s_X11.getWindowProperty(
        s_X11.display,
        s_X11.root,
        s_X11Atoms._NET_SUPPORTED,
        0,
        (~0L),
        False,
        XA_ATOM,
        &type,
        &format,
        &count,
        &bytesAfters,
        (unsigned char**)&supportedAtoms);

    PalVideoFeatures features = 0;
    for (unsigned long i = 0; i < count; ++i) {
        if (supportedAtoms[i] == s_X11Atoms._NET_WM_STATE_MAXIMIZED_VERT) {
            features |= PAL_VIDEO_FEATURE_WINDOW_SET_STATE;
            features |= PAL_VIDEO_FEATURE_WINDOW_GET_STATE;
        }

        if (supportedAtoms[i] == s_X11Atoms._NET_WM_STATE_MAXIMIZED_HORZ) {
            features |= PAL_VIDEO_FEATURE_WINDOW_SET_STATE;
            features |= PAL_VIDEO_FEATURE_WINDOW_GET_STATE;
        }

        if (supportedAtoms[i] == s_X11Atoms._NET_WM_STATE_HIDDEN) {
            features |= PAL_VIDEO_FEATURE_WINDOW_SET_STATE;
            features |= PAL_VIDEO_FEATURE_WINDOW_GET_STATE;
        }

        if (supportedAtoms[i] == s_X11Atoms._NET_WM_WINDOW_TYPE) {
            features |= PAL_VIDEO_FEATURE_WINDOW_SET_STYLE;
            features |= PAL_VIDEO_FEATURE_WINDOW_GET_STYLE;
        }

        if (supportedAtoms[i] == s_X11Atoms._NET_WM_WINDOW_TYPE_SPLASH) {
            features |= PAL_VIDEO_FEATURE_BORDERLESS_WINDOW;
        }

        if (supportedAtoms[i] == s_X11Atoms._NET_WM_NAME) {
            s_X11Atoms.unicodeTitle = true;
        }

        if (supportedAtoms[i] == s_X11Atoms._NET_WM_STATE_DEMANDS_ATTENTIONS) {
            features |= PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY;
        }

        if (supportedAtoms[i] == s_X11Atoms._NET_WM_WINDOW_TYPE_UTILITY) {
            features |= PAL_VIDEO_FEATURE_TOOL_WINDOW;
        }
    }

    // check for transparent windows
    Atom compositor = s_X11.internAtom(s_X11.display, "_NET_WM_CM_S0", True);
    if (compositor != None) {
        Window owner = s_X11.getSelectionOwner(s_X11.display, compositor);
        if (owner != None) {
            features |= PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW;
        }
    }

    // general features
    features |= PAL_VIDEO_FEATURE_MULTI_MONITORS;
    features |= PAL_VIDEO_FEATURE_MONITOR_GET_ORIENTATION;
    features |= PAL_VIDEO_FEATURE_MONITOR_SET_MODE;
    features |= PAL_VIDEO_FEATURE_MONITOR_GET_MODE;
    features |= PAL_VIDEO_FEATURE_WINDOW_SET_SIZE;
    features |= PAL_VIDEO_FEATURE_WINDOW_GET_SIZE;
    features |= PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY;
    features |= PAL_VIDEO_FEATURE_WINDOW_GET_VISIBILITY;

    features |= PAL_VIDEO_FEATURE_CLIP_CURSOR;
    features |= PAL_VIDEO_FEATURE_WINDOW_SET_INPUT_FOCUS;
    features |= PAL_VIDEO_FEATURE_WINDOW_GET_INPUT_FOCUS;
    features |= PAL_VIDEO_FEATURE_CURSOR_SET_POS;
    features |= PAL_VIDEO_FEATURE_CURSOR_GET_POS;
    features |= PAL_VIDEO_FEATURE_WINDOW_SET_TITLE;
    features |= PAL_VIDEO_FEATURE_WINDOW_GET_TITLE;

    s_Video.features = features;
}

static int xErrorHandler(Display*, XErrorEvent* e) 
{
    // this is use for simple success and failure
    s_X11.error = true;
    return 0;
}

static PalWindowState xQueryWindowState(Window xWin) 
{
    Atom type;
    int format;
    unsigned long count, bytesAfter;
    Atom* atoms = nullptr;
    PalWindowState state = PAL_WINDOW_STATE_RESTORED;

    s_X11.getWindowProperty(
        s_X11.display,
        xWin,
        s_X11Atoms._NET_WM_STATE,
        0,
        1024,
        False,
        XA_ATOM,
        &type,
        &format,
        &count,
        &bytesAfter,
        (unsigned char**)&atoms);

    for (unsigned int i = 0; i < count; i++) {
        if (atoms[i] == s_X11Atoms._NET_WM_STATE_MAXIMIZED_HORZ) {
            state = PAL_WINDOW_STATE_MAXIMIZED;
        }

        if (atoms[i] == s_X11Atoms._NET_WM_STATE_MAXIMIZED_VERT) {
            state = PAL_WINDOW_STATE_MAXIMIZED;
        }

        if (atoms[i] == s_X11Atoms._NET_WM_STATE_HIDDEN) {
            state = PAL_WINDOW_STATE_MINIMIZED;
        }
    }

    return state;
}

static void xCacheMonitors(bool enumerate)
{
    XRRScreenResources* resources = nullptr;
    resources = s_X11.getScreenResources(
        s_X11.display,
        s_X11.root);

    for (int i = 0; i < resources->noutput; ++i) {
        RROutput output = resources->outputs[i];
        XRROutputInfo* info = s_X11.getOutputInfo(
            s_X11.display,
            resources,
            output);

        if (info->connection == RR_Connected && 
            info->crtc != None) {
            // get monitor data and update info
            PalMonitor* monitor = TO_PAL_HANDLE(PalMonitor, output);
            MonitorData* data = nullptr;

            if (enumerate) {
                data = getFreeMonitorData();
                if (!data) {
                    return;
                }

                data->monitor = monitor;

            } else {
                data = findMonitorData(monitor);
            }

            XRRCrtcInfo* crtc = s_X11.getCrtcInfo(
                s_X11.display,
                resources,
                info->crtc);

            double dpi = (double)(crtc->width * 25.4) / (double)info->mm_width;
            data->dpi = (int)dpi;
            data->w = crtc->width;
            data->h = crtc->height;
            data->x = crtc->x;
            data->y = crtc->y;

            s_X11.freeCrtcInfo(crtc);
        }

        s_X11.freeOutputInfo(info);
    }

    s_X11.freeScreenResources(resources);
}

static int xGetWindowMonitorDPI(
    WindowData* data, 
    bool enumerate)
{
    int winX = data->x + data->w / 2;
    int winY = data->y + data->w / 2;
    // get the DPI from our cached monitor
    for (int i = 0; i < s_Video.maxMonitorData; i++) {
        if (!s_Video.monitorData->used) {
            continue;
        }

        // we found a monitor, check the monitor bounds with the window
        MonitorData* info = &s_Video.monitorData[i];
        if (winX >= info->x && 
            winX < info->x + info->w && 
            winY >= info->y && 
            winY < info->y + info->h) {
            // found monitor
            return info->dpi;
        }
    }
}

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

    s_X11.getWindowProperty = (XGetWindowPropertyFn)dlsym(
        s_X11.handle, 
        "XGetWindowProperty");

    s_X11.internAtom = (XInternAtomFn)dlsym(
        s_X11.handle, 
        "XInternAtom");

    s_X11.getSelectionOwner = (XGetSelectionOwnerFn)dlsym(
        s_X11.handle, 
        "XGetSelectionOwner");

    s_X11.freeColormap = (XFreeColormapFn)dlsym(
        s_X11.handle, 
        "XFreeColormap");

    s_X11.storeName = (XStoreNameFn)dlsym(
        s_X11.handle, 
        "XStoreName");

    s_X11.changeProperty = (XChangePropertyFn)dlsym(
        s_X11.handle, 
        "XChangeProperty");

    s_X11.flush = (XFlushFn)dlsym(
        s_X11.handle, 
        "XFlush");

    s_X11.createColormap = (XCreateColormapFn)dlsym(
        s_X11.handle, 
        "XCreateColormap");

    s_X11.mapWindow = (XMapWindowFn)dlsym(
        s_X11.handle, 
        "XMapWindow");

    s_X11.unmapWindow = (XUnmapWindowFn)dlsym(
        s_X11.handle, 
        "XUnmapWindow");

    s_X11.createWindow = (XCreateWindowFn)dlsym(
        s_X11.handle, 
        "XCreateWindow");
    
    s_X11.destroyWindow = (XDestroyWindowFn)dlsym(
        s_X11.handle, 
        "XDestroyWindow");

    s_X11.matchVisualInfo = (XMatchVisualInfoFn)dlsym(
        s_X11.handle, 
        "XMatchVisualInfo");

    s_X11.pending = (XPendingFn)dlsym(
        s_X11.handle, 
        "XPending");

    s_X11.setWMProtocols = (XSetWMProtocolsFn)dlsym(
        s_X11.handle, 
        "XSetWMProtocols");

    s_X11.nextEvent = (XNextEventFn)dlsym(
        s_X11.handle, 
        "XNextEvent");

    s_X11.setWMNormalHints = (XSetWMNormalHintsFn)dlsym(
        s_X11.handle, 
        "XSetWMNormalHints");

    s_X11.sendEvent = (XSendEventFn)dlsym(
        s_X11.handle, 
        "XSendEvent");

    s_X11.moveWindow = (XMoveWindowFn)dlsym(
        s_X11.handle, 
        "XMoveWindow");

    s_X11.resizeWindow = (XResizeWindowFn)dlsym(
        s_X11.handle, 
        "XResizeWindow");

    s_X11.iconifyWindow = (XIconifyWindowFn)dlsym(
        s_X11.handle, 
        "XIconifyWindow");

    s_X11.setErrorHandler = (XSetErrorHandlerFn)dlsym(
        s_X11.handle, 
        "XSetErrorHandler");

    s_X11.sync = (XSyncFn)dlsym(
        s_X11.handle, 
        "XSync");

    s_X11.saveContext = (XSaveContextFn)dlsym(
        s_X11.handle, 
        "XSaveContext");

    s_X11.findContext = (XFindContextFn)dlsym(
        s_X11.handle, 
        "XFindContext");

    s_X11.uniqueContext = (XrmUniqueQuarkFn)dlsym(
        s_X11.handle, 
        "XrmUniqueQuark");

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

    s_X11.selectRRInput = (XRRSelectInputFn)dlsym(
        s_X11.xrandr, 
        "XRRSelectInput");

    s_X11.queryRRExtension = (XRRQueryExtensionFn)dlsym(
        s_X11.xrandr, 
        "XRRQueryExtension");

    s_X11.allocClassHint = (XAllocClassHintFn)dlsym(
        s_X11.handle, 
        "XAllocClassHint");

    s_X11.setClassHint = (XSetClassHintFn)dlsym(
        s_X11.handle, 
        "XSetClassHint");

    s_X11.free = (XFreeFn)dlsym(
        s_X11.handle, 
        "XFree");

    // X11 server
    s_X11.display = s_X11.openDisplay(nullptr);
    if (!s_X11.display) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    s_X11.root = DefaultRootWindow(s_X11.display);
    s_X11.screen = DefaultScreen(s_X11.display);

    xCheckFeatures();
   
    // subscribe for monitor events
    s_X11.selectRRInput(
        s_X11.display, 
        s_X11.root, 
        RRScreenChangeNotifyMask | RRNotify);

    int eventBase, errorBase = 0;
    s_X11.queryRRExtension(s_X11.display, &eventBase, &errorBase);
    s_X11.rrEventBase = eventBase;
    s_X11.skipScreenEvent = true;
    s_X11.skipNotifyEvent = true;

    s_X11.dataID = (XContext)s_X11.uniqueContext();
    s_X11.className = "PAL";
    resetMonitorData();
    xCacheMonitors(true);

    // load opengl functions
    s_X11.opengl = dlopen("libGL.so.1", RTLD_LAZY | RTLD_LOCAL);
    if (s_X11.opengl) {

        GLXGetProcAddressFn load = nullptr;
        load = (GLXGetProcAddressFn)dlsym(
            s_X11.opengl,
            "glXGetProcAddress");

        s_X11.glxGetFBConfigs = (GLXGetFBConfigsFn)load(
            "glXGetFBConfigs");

        s_X11.glxGetFBConfigAttrib = (GLXGetFBConfigAttribFn)load(
            "glXGetFBConfigAttrib");

        s_X11.glxGetVisualFromFBConfig = (GLXGetVisualFromFBConfigFn)load(
            "glXGetVisualFromFBConfig");

    }

    return PAL_RESULT_SUCCESS;
}

static void xShutdownVideo() 
{
    s_X11.closeDisplay(s_X11.display);
    dlclose(s_X11.handle);
    dlclose(s_X11.xrandr);
    if (s_X11.opengl) {
        dlclose(s_X11.opengl);
    }
}

static void xUpdateVideo()
{
    XEvent event;
    PalDispatchMode mode = PAL_DISPATCH_NONE;
    while (s_X11.pending(s_X11.display)) {
        s_X11.nextEvent(s_X11.display, &event);

        Window xWin = event.xany.window;
        PalWindow* window = TO_PAL_HANDLE(PalWindow, xWin);
        WindowData* data = nullptr;
        s_X11.findContext(s_X11.display, xWin, s_X11.dataID, (XPointer*)&data);

        if (event.type == s_X11.rrEventBase + RRScreenChangeNotify) {
            event.type = RANDR_SCREEN_CHANGE_EVENT; // for switch flow
        } else if (event.type == s_X11.rrEventBase + RRNotify) {
            event.type = RANDR_NOTIFY_EVENT; // for switch flow
        }

        switch (event.type) {
            case ClientMessage: {
                // check for window close
                Atom windowClose = event.xclient.data.l[0];
                if (windowClose == s_X11Atoms.WM_DELETE_WINDOW) {
                    if (s_Video.eventDriver) {
                        PalEventDriver* driver = s_Video.eventDriver;
                        PalEventType type = PAL_EVENT_WINDOW_CLOSE;
                        mode = palGetEventDispatchMode(driver, type);
                        if (mode != PAL_DISPATCH_NONE) {
                            PalEvent event = {0};
                            event.type = type;
                            event.data2 = palPackPointer(window);
                            palPushEvent(driver, &event);
                        }
                    }
                }
                return;
            }

            case ConfigureNotify: {
                // window resize or move

                // skip the first configure event
                if (data->skipConfigure) {
                    data->skipConfigure = false;
                    data->w = event.xconfigure.width;
                    data->h = event.xconfigure.height;
                    data->x = event.xconfigure.x;
                    data->y = event.xconfigure.y;
                    return;
                }

                // real configure event
                if (s_Video.eventDriver) {
                    // check if its a resize event
                    if (data->w != event.xconfigure.width || 
                        data->h != event.xconfigure.height) {
                        data->w = event.xconfigure.width;
                        data->h = event.xconfigure.height;

                        // push a resize event
                        PalEventDriver* driver = s_Video.eventDriver;
                        PalEventType type = PAL_EVENT_WINDOW_SIZE;
                        mode = palGetEventDispatchMode(driver, type);

                        if (mode != PAL_DISPATCH_NONE) {
                            PalEvent event = {0};
                            event.type = type;
                            event.data = palPackUint32(data->w, data->h);
                            event.data2 = palPackPointer(window);
                            palPushEvent(driver, &event);
                        }
                    }

                    // check if its a move event
                    if (data->x != event.xconfigure.x || 
                        data->y != event.xconfigure.y) {
                        data->x = event.xconfigure.x;
                        data->y = event.xconfigure.y;

                        // push a move event
                        PalEventDriver* driver = s_Video.eventDriver;
                        PalEventType type = PAL_EVENT_WINDOW_MOVE;
                        mode = palGetEventDispatchMode(driver, type);

                        if (mode != PAL_DISPATCH_NONE) {
                            PalEvent event = {0};
                            event.type = type;
                            event.data = palPackInt32(data->x, data->y);
                            event.data2 = palPackPointer(window);
                            palPushEvent(driver, &event);
                        }

                        /** a window has to be moved 
                        before its can change monitors
                        we get the monitor the moved 
                        window is on and check if the dpi is different
                        from the one it was created on */
                        int monitorDPI = xGetWindowMonitorDPI(data, false);
                        if (monitorDPI != data->dpi) {
                            // window is on a different monitor
                            data->dpi = monitorDPI;

                            // push a DPI event
                            type = PAL_EVENT_MONITOR_DPI_CHANGED;
                            mode = palGetEventDispatchMode(driver, type);

                            if (mode != PAL_DISPATCH_NONE) {
                                PalEvent event = {0};
                                event.type = type;
                                event.data = monitorDPI;
                                event.data2 = palPackPointer(window);
                                palPushEvent(driver, &event);
                            }
                        }
                    }
                }
                return;
            }

            case FocusIn: {
                // window has gained focus
                if (s_Video.eventDriver) {
                    int mode = event.xfocus.mode;
                    if (mode == NotifyGrab || mode == NotifyUngrab) {
                        // ignore dragging and popup focus events
                        return;
                    }

                    PalEventDriver* driver = s_Video.eventDriver;
                    PalEventType type = PAL_EVENT_WINDOW_FOCUS;
                    mode = palGetEventDispatchMode(driver, type);
                    if (mode != PAL_DISPATCH_NONE) {
                        PalEvent event = {0};
                        event.type = type;
                        event.data = true;
                        event.data2 = palPackPointer(window);
                        palPushEvent(driver, &event);
                    }
                }
                return;
            }

            case FocusOut: {
                // window has lost focus
                if (s_Video.eventDriver) {
                    int mode = event.xfocus.mode;
                    if (mode == NotifyGrab || mode == NotifyUngrab) {
                        // ignore dragging and popup focus events
                        break;
                    }

                    PalEventDriver* driver = s_Video.eventDriver;
                    PalEventType type = PAL_EVENT_WINDOW_FOCUS;
                    mode = palGetEventDispatchMode(driver, type);
                    if (mode != PAL_DISPATCH_NONE) {
                        PalEvent event = {0};
                        event.type = type;
                        event.data = false;
                        event.data2 = palPackPointer(window);
                        palPushEvent(driver, &event);
                    }
                }
                return;
            }

            case PropertyNotify: {
                // check window state (maximize, minimize)
                if (event.xproperty.atom == s_X11Atoms._NET_WM_STATE) {
                    PalWindowState state;
                    state = xQueryWindowState(event.xproperty.window);
                    if (state != data->state) {
                        data->state = state;

                        // skip the first state event
                        if (data->skipState) {
                            data->skipState = false;
                            return;
                        }

                        // push event
                        PalEventDriver* driver = s_Video.eventDriver;
                        PalEventType type = PAL_EVENT_WINDOW_STATE;
                        mode = palGetEventDispatchMode(driver, type);

                        if (mode != PAL_DISPATCH_NONE) {
                            PalEvent event = {0};
                            event.type = type;
                            event.data = data->state;
                            event.data2 = palPackPointer(window);
                            palPushEvent(driver, &event);
                        }
                    }
                }
                return;
            }

            case RANDR_SCREEN_CHANGE_EVENT: {
                // skip the first event 
                if (s_X11.skipScreenEvent) {
                    s_X11.skipScreenEvent = false;
                    return;
                }

                // something change on pre existing monitor
                // cache the information
                xCacheMonitors(false);
                return;
            }

            case RANDR_NOTIFY_EVENT: {
                // skip the first event 
                if (s_X11.skipNotifyEvent) {
                    s_X11.skipNotifyEvent = false;
                    return;
                }

                XRRNotifyEvent* e = (XRRNotifyEvent*)&event;
                switch (e->subtype) {
                    case RRNotify_OutputChange: {
                        // push a event monitor list changed event
                        if (s_Video.eventDriver) {
                            PalEventDriver* driver = s_Video.eventDriver;
                            PalEventType type = PAL_EVENT_MONITOR_LIST_CHANGED;
                            mode = palGetEventDispatchMode(driver, type);
                            if (mode != PAL_DISPATCH_NONE) {
                                PalEvent event = {0};
                                event.type = type;
                                event.data2 = palPackPointer(window);
                                palPushEvent(driver, &event);
                            }
                        }

                        /** enumerate monitors and cache them 
                        these will be used to detect DPI changed
                        since X11 does not have a DPI changed function */
                        resetMonitorData();
                        xCacheMonitors(true);
                        return;
                    }
                }
            }
        }
    }

    s_X11.flush(s_X11.display);
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
    RROutput primary = s_X11.getOutputPrimary(
        s_X11.display,
        s_X11.root);

    if (!primary) {
        // primary monitor is not set, set the first one
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
                primary = resources->outputs[i];
                break;
            }
        }
    }

    // if we still did not get one, we fail
    if (primary) {
        *outMonitor = TO_PAL_HANDLE(PalMonitor, primary);
        return PAL_RESULT_SUCCESS;

    } else {
        return PAL_RESULT_PLATFORM_FAILURE;
    }
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
    
    strcpy(info->name, outputInfo->name);
    
    // check if its primary monitor
    RROutput primary = s_X11.getOutputPrimary(
        s_X11.display,
        s_X11.root);

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
    XRRCrtcInfo* crtc = s_X11.getCrtcInfo(
        s_X11.display,
        resources,
        outputInfo->crtc);
    
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
    }
    
    s_X11.freeCrtcInfo(crtc);
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

static PalResult xValidateMonitorMode(
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

    s_X11.freeOutputInfo(outputInfo);
    s_X11.freeScreenResources(resources);

    return PAL_RESULT_SUCCESS;
}

static PalResult xSetMonitorOrientation(
    PalMonitor* monitor,
    PalOrientation orientation)
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
    XRRCrtcInfo* crtc = s_X11.getCrtcInfo(
        s_X11.display,
        resources,
        outputInfo->crtc);

    // check if the new orientation is supported
    Rotation rotation = 0;
    switch (orientation) {
        case PAL_ORIENTATION_LANDSCAPE: {
            rotation = RR_Rotate_0;
            break;
        }

        case PAL_ORIENTATION_PORTRAIT: {
            rotation = RR_Rotate_90;
            break;
        }

        case PAL_ORIENTATION_LANDSCAPE_FLIPPED: {
            rotation = RR_Rotate_180;
            break;
        }

        case PAL_ORIENTATION_PORTRAIT_FLIPPED: {
            rotation = RR_Rotate_270;
            break;
        }

        default: {
            return PAL_RESULT_INVALID_ORIENTATION;
        }
    }

    if (!(crtc->rotations & rotation)) {
        return PAL_RESULT_INVALID_ORIENTATION;
    }

    RROutput output = FROM_PAL_HANDLE(RROutput, monitor);
    int ret = s_X11.setCrtcConfig(
        s_X11.display,
        resources,
        outputInfo->crtc,
        CurrentTime,
        crtc->x,
        crtc->y,
        crtc->mode,
        rotation,
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

static PalResult xCreateWindow(
    const PalWindowCreateInfo* info,
    PalWindow** outWindow)
{
    Window window = None;
    PalMonitor* monitor = nullptr;
    PalMonitorInfo monitorInfo;

    WindowData* data = getFreeWindowData();
    if (!data) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    Visual* visual = nullptr;
    int depth = 0;
    Colormap colormap = None;
    unsigned long bgPixel = 0;
    unsigned long borderPixel = 0;

    // check to see if the user has set a pixel format
    // with palSetPixelFormat()
    if (s_Video.pixelFormat) {
        // this is the pixel format driver index
        // we query info about it and create the visual with it
        if (s_X11.opengl) {
            int count = 0;
            GLXFBConfig* configs = s_X11.glxGetFBConfigs(
                s_X11.display,
                s_X11.screen,
                &count);

            GLXFBConfig fbConfig = configs[s_Video.pixelFormat];
            if (!fbConfig) {
                return PAL_RESULT_INVALID_GL_FBCONFIG;
            }

            // get a matching visual and use that to create the window
            XVisualInfo* visualInfo = s_X11.glxGetVisualFromFBConfig(
                s_X11.display,
                fbConfig);
            
            if (!visualInfo) {
                return PAL_RESULT_INVALID_GL_FBCONFIG;
            }

            visual = visualInfo->visual;
            depth = visualInfo->depth;
            bgPixel = 0;
            borderPixel = 0;
            colormap = s_X11.createColormap(
                s_X11.display, 
                s_X11.root, 
                visual, 
                AllocNone);
        }

    } else {
        // use a default visual
        visual = DefaultVisual(s_X11.display, s_X11.screen);
        depth = DefaultDepth(s_X11.display, s_X11.screen);
        bgPixel = WhitePixel(s_X11.display, s_X11.screen);
        borderPixel = BlackPixel(s_X11.display, s_X11.screen);
        colormap = DefaultColormap(s_X11.display, s_X11.screen);
    }

    // get monitor
    if (info->monitor) {
        monitor = info->monitor;

    } else {
        // get primary monitor
        xGetPrimaryMonitor(&monitor);
        if (!monitor) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    // get monitor info
    PalResult result = palGetMonitorInfo(monitor, &monitorInfo);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    Int32 x, y = 0;
    // the position and size must be scaled with the dpi before this call
    if (info->center) {
        x = monitorInfo.x + (monitorInfo.width - info->width) / 2;
        y = monitorInfo.y + (monitorInfo.height - info->height) / 2;

    } else {
        // we set 100 for each axix
        x = monitorInfo.x + 100;
        y = monitorInfo.y + 100;
    }

    // check and set transparency
    if (info->style & PAL_WINDOW_STYLE_TRANSPARENT) {
        if (!(s_Video.features & PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW)) {
            return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
        }

        // we dont need to set any flag
    }

    long mask = ExposureMask | StructureNotifyMask | KeyPressMask;
    mask |= KeyReleaseMask;
    mask |= ButtonPressMask;
    mask |= ButtonReleaseMask;
    mask |= PointerMotionMask;
    mask |= FocusChangeMask;
    mask |= EnterWindowMask;
    mask |= LeaveWindowMask;
    mask |= PropertyChangeMask;

    XSetWindowAttributes attrs = {0};
    attrs.colormap = colormap;
    attrs.event_mask = mask;
    attrs.background_pixel = bgPixel;
    attrs.border_pixel = borderPixel;
    attrs.override_redirect = False;

    // create window
    window = s_X11.createWindow(
        s_X11.display,
        s_X11.root,
        x,
        y,
        info->width,
        info->height,
        0, // border width
        depth,
        InputOutput, // class
        visual,
        CWEventMask | CWColormap | CWBorderPixel | CWBackPixel,
        &attrs);

    if (window == None) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // set pid property
    pid_t pid = getpid();
    s_X11.changeProperty(
        s_X11.display,
        window,
        s_X11Atoms._NET_WM_PID,
        XA_CARDINAL,
        32,
        PropModeReplace,
        (unsigned char*)&pid,
        1);

    // set class property
    XClassHint* hints = s_X11.allocClassHint();
    if (hints) {
        const char* resName = getenv("RESOURCE_NAME");
        const char* resClass = getenv("RESOURCE_CLASS");

        if (!resName || strlen(resName) == 0) {
            resName = info->title;
        }

        if (!resClass || strlen(resClass) == 0) {
            resClass = s_X11.className;
        }

        hints->res_name = (char*)resName;
        hints->res_class = (char*)resClass;
        s_X11.setClassHint(s_X11.display, window, hints);
        s_X11.free(hints);
    }
    
    if (s_X11Atoms.unicodeTitle) {
        s_X11.changeProperty(
            s_X11.display,
            window,
            s_X11Atoms._NET_WM_NAME,
            s_X11Atoms.UTF8_STRING,
            8, // unsigned char
            PropModeReplace,
            info->title,
            strlen(info->title));

    } else {
        s_X11.storeName(s_X11.display, window, info->title);
    }

    // borderless
    if (info->style & PAL_WINDOW_STYLE_BORDERLESS) {
        if (!(s_Video.features & PAL_VIDEO_FEATURE_BORDERLESS_WINDOW)) {
            s_X11.destroyWindow(s_X11.display, window);
            return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
        }

        s_X11.changeProperty(
            s_X11.display,
            window,
            s_X11Atoms._NET_WM_WINDOW_TYPE,
            XA_ATOM,
            32,
            PropModeReplace,
            (unsigned char*)&s_X11Atoms._NET_WM_WINDOW_TYPE_SPLASH,
            1);

    }

    // tool window
    if (info->style & PAL_WINDOW_STYLE_TOOL) {
        if (!(s_Video.features & PAL_VIDEO_FEATURE_TOOL_WINDOW)) {
            s_X11.destroyWindow(s_X11.display, window);
            return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
        }

        s_X11.changeProperty(
            s_X11.display,
            window,
            s_X11Atoms._NET_WM_WINDOW_TYPE,
            XA_ATOM,
            32,
            PropModeReplace,
            (unsigned char*)&s_X11Atoms._NET_WM_WINDOW_TYPE_UTILITY,
            1);

    }

    // topmost
    if (info->style & PAL_WINDOW_STYLE_TOPMOST) {
        if (s_X11Atoms._NET_WM_STATE_ABOVE) {
            s_X11.changeProperty(
                s_X11.display,
                window,
                s_X11Atoms._NET_WM_STATE,
                XA_ATOM,
                32,
                PropModeAppend,
                (unsigned char*)&s_X11Atoms._NET_WM_STATE_ABOVE,
                1);
        }
    }

    // resizable
    if (!(info->style & PAL_WINDOW_STYLE_RESIZABLE)) {
        XSizeHints hints = {0};
        hints.flags = PMinSize | PMaxSize;
        hints.min_width = hints.max_width = info->width;
        hints.min_height = hints.max_height = info->height;
        s_X11.setWMNormalHints(s_X11.display, window, &hints);
    }

    // show window
    if (info->show) {
        s_X11.mapWindow(s_X11.display, window);
        // move the window if window manager didnt
        s_X11.moveWindow(s_X11.display, window, x, y);
        s_X11.flush(s_X11.display);
    }

    // check if the window has been mapped
    // we use this to minimize or maximize
    XWindowAttributes attr;
    s_X11.getWindowAttributes(s_X11.display, window, &attr);
    bool windowMapped = attr.map_state = IsViewable;

    // maximize
    if (info->maximized) {
        if (!(s_Video.features & PAL_VIDEO_FEATURE_WINDOW_SET_STATE)) {
            s_X11.destroyWindow(s_X11.display, window);
            return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
        }

        // if the window is not mapped, we wait till its mapped
        if (!windowMapped) {
            // wait till the window is mapped
            for (;;) {
                XEvent event;
                s_X11.nextEvent(s_X11.display, &event);
                if (event.type == MapNotify && event.xmap.window == window) {
                    windowMapped = true;
                    break;
                }
            }
        }

        Atom data[2];
        data[0] = s_X11Atoms._NET_WM_STATE_MAXIMIZED_VERT;
        data[1] = s_X11Atoms._NET_WM_STATE_MAXIMIZED_HORZ;

        XEvent e = {0};
        e.xclient.type = ClientMessage;
        e.xclient.send_event = True;
        e.xclient.window = window;
        e.xclient.message_type = s_X11Atoms._NET_WM_STATE;
        e.xclient.format = 32;
        e.xclient.data.l[0] = 1; // _NET_WM_STATE_ADD
        e.xclient.data.l[1] = data[0];
        e.xclient.data.l[2] = data[1];
        e.xclient.data.l[3] = 1;
        e.xclient.data.l[4] = 0;

        s_X11.sendEvent(
            s_X11.display,
            s_X11.root,
            False,
            SubstructureNotifyMask | SubstructureRedirectMask,
            &e);
    }

    // minimize
    if (info->minimized) {
        if (!(s_Video.features & PAL_VIDEO_FEATURE_WINDOW_SET_STATE)) {
            s_X11.destroyWindow(s_X11.display, window);
            return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
        }

        // if the window is not mapped, we wait till its mapped
        if (!windowMapped) {
            // wait till the window is mapped
            for (;;) {
                XEvent event;
                s_X11.nextEvent(s_X11.display, &event);
                if (event.type == MapNotify && event.xmap.window == window) {
                    windowMapped = true;
                    break;
                }
            }
        }
        s_X11.iconifyWindow(s_X11.display, window, s_X11.screen);
    }

    s_X11.setWMProtocols(
        s_X11.display, 
        window, 
        &s_X11Atoms.WM_DELETE_WINDOW, 
        True);

    s_X11.flush(s_X11.display);

    // attach the window data to the window
    data->skipConfigure = true;
    data->skipState = true;
    data->dpi = monitorInfo.dpi; // the current window monitor
    s_X11.saveContext(s_X11.display, window, s_X11.dataID, (XPointer)data);

    *outWindow = TO_PAL_HANDLE(PalWindow, window);
    return PAL_RESULT_SUCCESS;
}

static void xDestroyWindow(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    WindowData* data = nullptr;
    s_X11.findContext(s_X11.display, xWin, s_X11.dataID, (XPointer*)&data);
    s_X11.destroyWindow(s_X11.display, xWin);
    data->used = false;
}

static PalResult xSetWindowOpacity(
    PalWindow* window,
    float opacity)
{
    XErrorHandler old = s_X11.setErrorHandler(xErrorHandler);
    unsigned long value = (unsigned long)(opacity * 0xFFFFFFFFUL + 0.5f);

    s_X11.changeProperty(
        s_X11.display,
        FROM_PAL_HANDLE(Window, window),
        s_X11Atoms._NET_WM_WINDOW_OPACITY,
        XA_CARDINAL,
        32,
        PropModeReplace,
        (unsigned char*)&value,
        1);
    
    s_X11.sync(s_X11.display, False);
    s_X11.setErrorHandler(old);
    if (s_X11.error) {
        // technically, this is the only error that can occur
        return PAL_RESULT_INVALID_WINDOW;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult xCreateIcon(
    const PalIconCreateInfo* info,
    PalIcon** outIcon)
{
    Uint64 totalPixels = 2 + (Uint64)(info->width * info->height);
    Uint64 totalBytes = sizeof(unsigned long) * totalPixels;

    unsigned long* icon = palAllocate(s_Video.allocator, totalBytes, 0);
    if (!icon) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    // store width and height and populate data with icon pixels
    // [width][height][pixels]
    icon[0] = (unsigned long)info->width;
    icon[1] = (unsigned long)info->height;

    // convert from RGBA8 to ARGB32
    for (int i = 0; i < info->width * info->height; i++) {
        Uint8 r = info->pixels[i * 4 + 0]; // Red
        Uint8 g = info->pixels[i * 4 + 1]; // Green
        Uint8 b = info->pixels[i * 4 + 2]; // Blue
        Uint8 a = info->pixels[i * 4 + 3]; // Alpha

        icon[2 + i] = ((unsigned long)a << 24) | 
                      ((unsigned long)r << 16) |
                      ((unsigned long)g << 8) |
                      ((unsigned long)b);
    }

    *outIcon = TO_PAL_HANDLE(PalIcon, icon);
    return PAL_RESULT_SUCCESS;
}

void xDestroyIcon(PalIcon* icon)
{
    if (icon) {
        palFree(s_Video.allocator, icon);
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
        return PAL_RESULT_INVALID_WINDOW;
    }

    unsigned long* iconData = FROM_PAL_HANDLE(unsigned long*, icon);
    Uint64 totalPixels = 2 + iconData[0] * iconData[1];  
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

static Backend s_XBackend = {
    .shutdownVideo = xShutdownVideo,
    .updateVideo = xUpdateVideo,
    .enumerateMonitors = xEnumerateMonitors,
    .getMonitorInfo = xGetMonitorInfo,
    .getPrimaryMonitor = xGetPrimaryMonitor,
    .enumerateMonitorModes = xEnumerateMonitorModes,
    .getCurrentMonitorMode = xGetCurrentMonitorMode,
    .setMonitorMode = xSetMonitorMode,
    .validateMonitorMode = xValidateMonitorMode,
    .setMonitorOrientation = xSetMonitorOrientation,

    .createWindow = xCreateWindow,
    .destroyWindow = xDestroyWindow,
    .setWindowOpacity = xSetWindowOpacity,

    .createIcon = xCreateIcon,
    .destroyIcon = xDestroyIcon,
    .setWindowIcon = xSetWindowIcon
};

#pragma endregion

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

    s_Video.maxMonitorData = 16; // initial size
    s_Video.maxWindowData = 32;// initial size

    s_Video.windowData = palAllocate(
        s_Video.allocator,
        sizeof(WindowData) * s_Video.maxWindowData,
        0);

    s_Video.monitorData = palAllocate(
        s_Video.allocator,
        sizeof(MonitorData) * s_Video.maxMonitorData,
        0);

    if (!s_Video.monitorData || !s_Video.windowData) {
        return PAL_RESULT_OUT_OF_MEMORY;
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
        palFree(s_Video.allocator, s_Video.windowData);
        palFree(s_Video.allocator, s_Video.monitorData);
    }
}

void PAL_CALL palUpdateVideo() 
{
    if (s_Video.initialized) {
        s_Video.backend->updateVideo();
    }
}

PalVideoFeatures PAL_CALL palGetVideoFeatures() 
{
    if (!s_Video.initialized) {
        return 0;
    }

    return s_Video.features;
}

PalResult PAL_CALL palSetPixelFormat(const int pixelFormatIndex)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (pixelFormatIndex) {
        s_Video.pixelFormat = pixelFormatIndex;
        return PAL_RESULT_SUCCESS;
    }
    return PAL_RESULT_INVALID_GL_FBCONFIG;
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

PalResult PAL_CALL palValidateMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)     
{
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!monitor || !mode) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->validateMonitorMode(monitor, mode);
}

PalResult PAL_CALL palSetMonitorOrientation(
    PalMonitor* monitor,
    PalOrientation orientation)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!monitor) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->setMonitorOrientation(monitor, orientation);
}

// ==================================================
// Window
// ==================================================

PalResult PAL_CALL palCreateWindow(
    const PalWindowCreateInfo* info,
    PalWindow** outWindow)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!info || !outWindow) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (info->style & PAL_WINDOW_STYLE_NO_MINIMIZEBOX) {
        return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
    }

    if (info->style & PAL_WINDOW_STYLE_NO_MAXIMIZEBOX) {
        return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
    }

    return s_Video.backend->createWindow(info,outWindow);
}

void PAL_CALL palDestroyWindow(PalWindow* window)
{
    if (s_Video.initialized && window) {
        return s_Video.backend->destroyWindow(window);
    }
}

PalResult PAL_CALL palSetWindowOpacity(
    PalWindow* window,
    float opacity)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!(s_Video.features & PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW)) {
        return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
    }

    if (opacity < 0.0f) {
        opacity = 0.0f;
    }

    if (opacity > 1.0f) {
        opacity = 1.0f;
    }

    return s_Video.backend->setWindowOpacity(window, opacity);
}

PalResult PAL_CALL palCreateIcon(
    const PalIconCreateInfo* info,
    PalIcon** outIcon)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!info || !outIcon) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->createIcon(info, outIcon);
}

void PAL_CALL palDestroyIcon(PalIcon* icon)
{
    if (s_Video.initialized && icon) {
        s_Video.backend->destroyIcon(icon);
    }
}

PalResult PAL_CALL palSetWindowIcon(
    PalWindow* window,
    PalIcon* icon)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->setWindowIcon(window, icon);
}