
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
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>

#if PAL_HAS_WAYLAND
#include <wayland-client.h>
#endif // PAL_HAS_WAYLAND

// ==================================================
// Typedefs, enums and structs
// ==================================================

#define TO_PAL_HANDLE(type, val) ((type*)(UintPtr)(val))
#define FROM_PAL_HANDLE(type, handle) ((type)(UintPtr)(handle))
#define X_INTERN(x) s_X11Atoms.x = s_X11.internAtom(s_X11.display, #x, False)

#define MAX_MONITOR_STATE 16
#define MAX_WINDOW_STATE 128

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
    bool used;
    int dpi;
    PalMonitor* monitor;
} MonitorState;

typedef struct {
    bool skipState;
    bool skipConfigure;
    bool used;
    int x;
    int y;
    int width;
    int height;
    PalWindowState state;
    PalWindow* window;
} WindowState;

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
} X11Atoms;

typedef struct {
    int bpp;
    int transparentDepth;
    int screen;
    void* handle;
    void* xrandr;
    Display* display;
    Window root;
    Colormap transparentColormap;
    Visual* transparentVisual;
    GC gc;

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

    XRRSetCrtcConfigFn setCrtcConfig;
    XRRGetScreenResourcesFn getScreenResources;
    XRRGetOutputPrimaryFn getOutputPrimary;
    XRRGetOutputInfoFn getOutputInfo;
    XRRGetCrtcInfoFn getCrtcInfo;
    XRRFreeScreenResourcesFn freeScreenResources;
    XRRFreeOutputInfoFn freeOutputInfo;
    XRRFreeCrtcInfoFn freeCrtcInfo;
} X11;

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
} Backend;

typedef struct {
    bool initialized;
    Int32 classNameLen;
    Int32 monitorCount;
    PalVideoFeatures features;
    const PalAllocator* allocator;
    PalEventDriver* eventDriver;
    const Backend* backend;
    WindowState* windowStates;
    MonitorState* monitorStates;
    char className[64];
} VideoLinux;

static X11 s_X11 = {0};
static X11Atoms s_X11Atoms = {0};
static VideoLinux s_Video = {0};
static bool s_XErrorOccurred = false;

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

        if (supportedAtoms[i] == s_X11Atoms._NET_WM_STATE_ABOVE) {
            features |= PAL_VIDEO_FEATURE_TOPMOST_WINDOW;
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
    s_XErrorOccurred = true;
    return 0;
}

static WindowState* getFreeWindowState() 
{
    for (int i = 0; i < MAX_WINDOW_STATE; ++i) {
        if (!s_Video.windowStates[i].used) {
            s_Video.windowStates[i].used = true;
            return &s_Video.windowStates[i];
        }
    }
    // TODO: FIXME
    // maybe expand window states array
    return nullptr;
}

static MonitorState* getFreeMonitorState() 
{
    for (int i = 0; i < MAX_MONITOR_STATE; ++i) {
        if (!s_Video.monitorStates[i].used) {
            s_Video.monitorStates[i].used = true;
            return &s_Video.monitorStates[i];
        }
    }
    // TODO: FIXME
    // maybe expand monitor states array
    return nullptr;
}

static WindowState* findWindowState(PalWindow* window) 
{
    for (int i = 0; i < MAX_WINDOW_STATE; ++i) {
        if (s_Video.windowStates[i].used && 
            s_Video.windowStates[i].window == window) {
            return &s_Video.windowStates[i];
        }
    }
    return nullptr;
}

static MonitorState* findMonitorState(PalMonitor* monitor) 
{
    for (int i = 0; i < MAX_MONITOR_STATE; ++i) {
        if (s_Video.monitorStates[i].used && 
            s_Video.monitorStates[i].monitor == monitor) {
            return &s_Video.monitorStates[i];
        }
    }
    return nullptr;
}

static void freeWindowState(PalWindow* window) 
{
    for (int i = 0; i < MAX_WINDOW_STATE; ++i) {
        if (s_Video.windowStates[i].used && 
            s_Video.windowStates[i].window == window) {
            s_Video.windowStates[i].used = false;
        }
    }
}

static void freeMonitorState(PalMonitor* monitor) 
{
    for (int i = 0; i < MAX_MONITOR_STATE; ++i) {
        if (s_Video.monitorStates[i].used && 
            s_Video.monitorStates[i].monitor == monitor) {
            s_Video.monitorStates[i].used = false;
        }
    }
}

static PalWindowState xQueryWindowState(Window xWindow) 
{
    Atom type;
    int format;
    unsigned long count, bytesAfter;
    Atom* atoms = nullptr;
    PalWindowState state = PAL_WINDOW_STATE_RESTORED;

    s_X11.getWindowProperty(
        s_X11.display,
        xWindow,
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

    // X11 server
    s_X11.display = s_X11.openDisplay(nullptr);
    if (!s_X11.display) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    s_X11.root = DefaultRootWindow(s_X11.display);
    s_X11.screen = DefaultScreen(s_X11.display);

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

    xCheckFeatures();
    // get root window bpp
    XWindowAttributes attr;
    s_X11.getWindowAttributes(s_X11.display, s_X11.root, &attr);
    s_X11.bpp = attr.depth;

    // create a transparent colormap
    XVisualInfo info;
    s_X11.matchVisualInfo(s_X11.display, s_X11.screen, 32, TrueColor, &info);
    s_X11.transparentDepth = info.depth;
    s_X11.transparentVisual = info.visual;

    // create color map
    s_X11.transparentColormap = s_X11.createColormap(
        s_X11.display, 
        s_X11.root,
        info.visual, 
        AllocNone);

    // set class
    const char* instance = "pal_window";
    const char* class = "PAL";
    snprintf(
        s_Video.className, 
        sizeof(s_Video.className), 
        "%s%c%s%c",
        instance,
        '\0',
        class,
        '\0');

    s_Video.classNameLen = strlen(instance) + strlen(class) + 2;

    return PAL_RESULT_SUCCESS;
}

static void xShutdownVideo() 
{
    if (s_X11.transparentColormap) {
        s_X11.freeColormap(s_X11.display, s_X11.transparentColormap);
    }
    
    s_X11.closeDisplay(s_X11.display);
    dlclose(s_X11.handle);
    dlclose(s_X11.xrandr);
}

static void xUpdateVideo()
{
    XEvent event;
    PalDispatchMode mode = PAL_DISPATCH_NONE;
    while (s_X11.pending(s_X11.display)) {
        s_X11.nextEvent(s_X11.display, &event);

        switch (event.type) {
            case ClientMessage: {
                // check for window close
                Atom windowClose = event.xclient.data.l[0];
                if (windowClose == s_X11Atoms.WM_DELETE_WINDOW) {
                    if (s_Video.eventDriver) {
                        PalEventDriver* driver = s_Video.eventDriver;
                        PalEventType type = PAL_EVENT_WINDOW_CLOSE;
                        mode = palGetEventDispatchMode(driver, type);

                        PalWindow* window = TO_PAL_HANDLE(
                            PalWindow, 
                            event.xclient.window);

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
                PalWindow* window = TO_PAL_HANDLE(
                    PalWindow, 
                    event.xconfigure.window);

                WindowState* state = findWindowState(window);
                // skip the first configure event
                if (state->skipConfigure) {
                    state->skipConfigure = false;
                    state->width = event.xconfigure.width;
                    state->height = event.xconfigure.height;
                    state->x = event.xconfigure.x;
                    state->y = event.xconfigure.y;
                    return;
                }

                // real configure event
                if (s_Video.eventDriver) {
                    // check if its a resize event
                    if (state->width != event.xconfigure.width || 
                        state->height != event.xconfigure.height) {
                        state->width = event.xconfigure.width;
                        state->height = event.xconfigure.height;

                        // push a resize event
                        PalEventDriver* driver = s_Video.eventDriver;
                        PalEventType type = PAL_EVENT_WINDOW_SIZE;
                        mode = palGetEventDispatchMode(driver, type);

                        if (mode != PAL_DISPATCH_NONE) {
                            PalEvent event = {0};
                            event.type = type;
                            event.data = palPackUint32(state->width, state->height);
                            event.data2 = palPackPointer(window);
                            palPushEvent(driver, &event);
                        }

                        // we check window state and push event some window 
                        // managers do not update the state PropertyNotify
                        // so we update that if it was not called
                        PalWindowState winState;
                        winState = xQueryWindowState(event.xconfigure.window);
                        if (winState != state->state) {
                            // state has changed even if PropertyNotify was called
                            state->state = winState;

                            // push event
                            PalEventDriver* driver = s_Video.eventDriver;
                            PalEventType type = PAL_EVENT_WINDOW_STATE;
                            mode = palGetEventDispatchMode(driver, type);

                            if (mode != PAL_DISPATCH_NONE) {
                                PalEvent event = {0};
                                event.type = type;
                                event.data = state->state;
                                event.data2 = palPackPointer(window);
                                palPushEvent(driver, &event);
                            }
                        }
                    }

                    // check if its a move event
                    if (state->x != event.xconfigure.x || 
                        state->y != event.xconfigure.y) {
                        state->x = event.xconfigure.x;
                        state->y = event.xconfigure.y;

                        // push a move event
                        PalEventDriver* driver = s_Video.eventDriver;
                        PalEventType type = PAL_EVENT_WINDOW_MOVE;
                        mode = palGetEventDispatchMode(driver, type);

                        if (mode != PAL_DISPATCH_NONE) {
                            PalEvent event = {0};
                            event.type = type;
                            event.data = palPackInt32(state->x, state->y);
                            event.data2 = palPackPointer(window);
                            palPushEvent(driver, &event);
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

                    PalWindow* window = TO_PAL_HANDLE(
                            PalWindow, 
                            event.xfocus.window);

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

                    PalWindow* window = TO_PAL_HANDLE(
                            PalWindow, 
                            event.xfocus.window);

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
                PalWindow* window = TO_PAL_HANDLE(
                    PalWindow, 
                    event.xproperty.window);

                // check window state (maximize, minimize)
                if (event.xproperty.atom == s_X11Atoms._NET_WM_STATE) {
                    WindowState* state = findWindowState(window);

                    PalWindowState winState;
                    winState = xQueryWindowState(event.xproperty.window);
                    if (winState != state->state) {
                        state->state = winState;

                        // skip the first state event
                        if (state->skipState) {
                            state->skipState = false;
                            return;
                        }

                        // push event
                        PalEventDriver* driver = s_Video.eventDriver;
                        PalEventType type = PAL_EVENT_WINDOW_STATE;
                        mode = palGetEventDispatchMode(driver, type);

                        if (mode != PAL_DISPATCH_NONE) {
                            PalEvent event = {0};
                            event.type = type;
                            event.data = state->state;
                            event.data2 = palPackPointer(window);
                            palPushEvent(driver, &event);
                        }
                    }
                }
                break;
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

    Visual* visual = DefaultVisual(s_X11.display, s_X11.screen);
    int depth = DefaultDepth(s_X11.display, s_X11.screen);
    unsigned long bgPixel = WhitePixel(s_X11.display, s_X11.screen);
    unsigned long borderPixel = BlackPixel(s_X11.display, s_X11.screen);
    Colormap colormap = DefaultColormap(s_X11.display, s_X11.screen);

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

        bgPixel = 0;
        borderPixel = 0;
        visual = s_X11.transparentVisual;
        depth = s_X11.transparentDepth;
        colormap = s_X11.transparentColormap;
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

    // set window pid
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
    
    // set class name
    s_X11.changeProperty(
        s_X11.display,
        window,
        s_X11Atoms._WM_CLASS,
        XA_STRING,
        8,
        PropModeReplace,
        (unsigned char*)&s_Video.className,
        s_Video.classNameLen);

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
        if (!(s_Video.features & PAL_VIDEO_FEATURE_TOPMOST_WINDOW)) {
            s_X11.destroyWindow(s_X11.display, window);
            return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
        }

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

    // maximize
    if (info->maximized && info->show) {
        if (!(s_Video.features & PAL_VIDEO_FEATURE_WINDOW_SET_STATE)) {
            s_X11.destroyWindow(s_X11.display, window);
            return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
        }

        // wait till the window is mapped
        for (;;) {
            XEvent event;
            s_X11.nextEvent(s_X11.display, &event);
            if (event.type == MapNotify && event.xmap.window == window) {
                break;
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
    if (info->minimized && info->show) {
        if (!(s_Video.features & PAL_VIDEO_FEATURE_WINDOW_SET_STATE)) {
            s_X11.destroyWindow(s_X11.display, window);
            return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
        }

        // wait till the window is mapped
        for (;;) {
            XEvent event;
            s_X11.nextEvent(s_X11.display, &event);
            if (event.type == MapNotify && event.xmap.window == window) {
                break;
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

    WindowState* state = getFreeWindowState();
    state->skipConfigure = true;
    state->skipState = true;
    state->window = TO_PAL_HANDLE(PalWindow, window);

    *outWindow = state->window;
    return PAL_RESULT_SUCCESS;
}

static void xDestroyWindow(PalWindow* window)
{
    s_X11.destroyWindow(
        s_X11.display, 
        FROM_PAL_HANDLE(Window, window));
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
    if (s_XErrorOccurred) {
        // technically, this is the only error that can occur
        return PAL_RESULT_INVALID_WINDOW;
    }

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
    .setWindowOpacity = xSetWindowOpacity
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

    // allocate an array for window states
    s_Video.windowStates = palAllocate(
        s_Video.allocator, 
        sizeof(WindowState) * MAX_WINDOW_STATE, 
        0);

    // allocate an array for monitor states
    s_Video.monitorStates = palAllocate(
        s_Video.allocator, 
        sizeof(MonitorState) * MAX_MONITOR_STATE, 
        0);

    if (!s_Video.windowStates || !s_Video.monitorStates) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(s_Video.windowStates, 0, sizeof(WindowState) * MAX_WINDOW_STATE);
    memset(s_Video.monitorStates, 0, sizeof(MonitorState) * MAX_MONITOR_STATE);

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

        // get monitor count. X11 does not provide an event for this
        int count = 0;
        xEnumerateMonitors(&count, nullptr);
        s_Video.monitorCount = count;
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
        palFree(s_Video.allocator, s_Video.windowStates);
        palFree(s_Video.allocator, s_Video.monitorStates);
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
        freeWindowState(window);
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