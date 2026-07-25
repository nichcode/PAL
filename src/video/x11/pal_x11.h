
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_X11_H
#define _PAL_X11_H

#if PAL_HAS_X11_BACKEND == 1

#define TO_PAL_HANDLE(type, val) ((type*)(uintptr_t)(val))
#define FROM_PAL_HANDLE(type, handle) ((type)(uintptr_t)(handle))

#include "pal/pal_video.h"
#include "video/pal_video_egl.h"
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/extensions/Xrandr.h>

#define X_INTERN(x) s_X11Atoms.x = s_X11.internAtom(s_X11.display, #x, False)
#define RANDR_SCREEN_CHANGE_EVENT 1040

// optionally, needed to create visual from FBConfig
#define GLX_FBCONFIG_ID 0x8012
typedef struct __GLXFBConfigRec* GLXFBConfig;

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

typedef int (*XGetWMNormalHintsFn)(
    Display*,
    Window,
    XSizeHints*,
    long*);

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

typedef void (*XRRFreeScreenResourcesFn)(XRRScreenResources*);

typedef void (*XRRFreeOutputInfoFn)(XRROutputInfo*);

typedef void (*XRRFreeCrtcInfoFn)(XRRCrtcInfo*);

typedef void (*XRRSelectInputFn)(
    Display*,
    Window,
    int);

typedef int (*XRRQueryExtensionFn)(
    Display*,
    int*,
    int*);

typedef XClassHint* (*XAllocClassHintFn)(void);

typedef int (*XSetClassHintFn)(
    Display*,
    Window,
    XClassHint*);

typedef int (*XFreeFn)(void*);

typedef Cursor (*XCreateFontCursorFn)(
    Display*,
    unsigned int);

typedef int (*XFreePixmapFn)(
    Display*,
    Pixmap);

typedef int (*XSetWMHintsFn)(
    Display*,
    Window,
    XWMHints*);

typedef int (*XGrabPointerFn)(
    Display*,
    Window,
    Bool,
    unsigned int,
    int,
    int,
    Window,
    Cursor,
    Time);

typedef Cursor (*XCreatePixmapCursorFn)(
    Display*,
    Pixmap,
    Pixmap,
    XColor*,
    XColor*,
    unsigned int,
    unsigned int);

typedef int (*XWarpPointerFn)(
    Display*,
    Window,
    Window,
    int,
    int,
    unsigned int,
    unsigned int,
    int,
    int);

typedef Status (*XGetWMNameFn)(
    Display*,
    Window,
    XTextProperty*);

typedef Bool (*XQueryPointerFn)(
    Display*,
    Window,
    Window*,
    Window*,
    int*,
    int*,
    int*,
    int*,
    unsigned int*);

typedef int (*XUngrabPointerFn)(
    Display*,
    Time);

typedef XWMHints* (*XAllocWMHintsFn)(void);

typedef int (*XMapRaisedFn)(
    Display*,
    Window);

typedef int (*XUndefineCursorFn)(
    Display*,
    Window);

typedef int (*XDefineCursorFn)(
    Display*,
    Window,
    Cursor);

typedef int (*XFreeCursorFn)(
    Display*,
    Cursor);

typedef XWMHints* (*XGetWMHintsFn)(
    Display*,
    Window);

typedef Cursor (*XCreatePixmapCursorFn)(
    Display*,
    Pixmap,
    Pixmap,
    XColor*,
    XColor*,
    unsigned int,
    unsigned int);

typedef int (*XSetInputFocusFn)(
    Display*,
    Window,
    int,
    Time);

typedef int (*XGetInputFocusFn)(
    Display*,
    Window*,
    int*);

typedef Pixmap (*XCreatePixmapFn)(
    Display*,
    Drawable,
    unsigned int,
    unsigned int,
    unsigned int);

typedef XVisualInfo* (*XGetVisualInfoFn)(
    Display*,
    long,
    XVisualInfo*,
    int*);

typedef int (*XSelectInputFn)(
    Display*,
    Window,
    long);

typedef Cursor (*XcursorImageLoadCursorFn)(
    Display*,
    const XcursorImage*);

typedef XcursorImage* (*XcursorImageCreateFn)(
    int,
    int);

typedef void (*XcursorImageDestroyFn)(XcursorImage*);

typedef KeySym (*XLookupKeysymFn)(
    XKeyEvent*,
    int);

typedef int (*XkbSetDetectableAutoRepeatFn)(
    Display*,
    int,
    int*);

typedef char* (*XSetLocaleModifiersFn)(const char*);

typedef XIM (*XOpenIMFn)(
    Display*,
    struct _XrmHashBucketRec*,
    char*,
    char*);

typedef int (*XCloseIMFn)(XIM);

typedef XIC (*XCreateICFn)(
    XIM,
    ...) _X_SENTINEL(0);

typedef void (*XDestroyICFn)(XIC);

typedef int (*Xutf8LookupStringFn)(
    XIC,
    XKeyPressedEvent*,
    char*,
    int,
    KeySym*,
    int*);

typedef struct {
    PalBool unicodeTitle;

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
    PalBool skipConfigure;
    PalBool skipState;
    PalBool used;
    PalBool isAttached;
    PalBool skipIfAttached;
    int x;
    int y;
    uint32_t w;
    int dpi;
    uint32_t h;
    PalWindowState state;
    PalWindow* window;
    void* ic;
    unsigned long colormap;
} WindowData;

typedef struct {
    PalBool used;
    int dpi;
    int x;
    int y;
    uint32_t w;
    uint32_t h;
    PalMonitor* monitor;
} MonitorData;

typedef struct {
    PalBool error;
    PalBool skipScreenEvent;
    int bpp;
    int screen;
    int depth;
    int monitorCount;
    int rrEventBase;
    void* handle;
    void* xrandr;
    void* glxHandle;
    void* libCursor;
    XIM im;
    Display* display;
    Window root;
    XContext dataID;

    int32_t maxWindowData;
    int32_t maxMonitorData;
    PalVideoFeatures features;
    const PalAllocator* allocator;
    PalEventDriver* eventDriver;
    WindowData* windowData;
    MonitorData* monitorData;

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
    XGetWMNormalHintsFn getWMNormalHints;
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
    XGetVisualInfoFn getVisualInfo;

    // opengl
    GLXGetFBConfigsFn glxGetFBConfigs;
    GLXGetFBConfigAttribFn glxGetFBConfigAttrib;
    GLXGetVisualFromFBConfigFn glxGetVisualFromFBConfig;

    XCreateFontCursorFn createFontCursor;
    XFreePixmapFn freePixmap;
    XSetWMHintsFn setWMHints;
    XGrabPointerFn grabPointer;
    XCreatePixmapCursorFn createPixmapCursor;
    XWarpPointerFn warpPointer;
    XGetWMNameFn getWMName;
    XQueryPointerFn queryPointer;
    XUngrabPointerFn ungrabPointer;
    XAllocWMHintsFn allocWMHints;
    XMapRaisedFn mapRaised;
    XUndefineCursorFn undefineCursor;
    XDefineCursorFn defineCursor;
    XFreeCursorFn freeCursor;
    XGetWMHintsFn getWMHints;
    XCreatePixmapFn createPixmap;
    XSetInputFocusFn setInputFocus;
    XGetInputFocusFn getInputFocus;
    XcursorImageLoadCursorFn cursorImageLoadCursor;
    XcursorImageCreateFn cursorImageCreate;
    XcursorImageDestroyFn cursorImageDestroy;
    XLookupKeysymFn lookupKeysym;
    XSelectInputFn selectInput;

    XkbSetDetectableAutoRepeatFn setDetectableAutoRepeat;
    XSetLocaleModifiersFn setLocaleModifiers;
    XOpenIMFn openIM;
    XCloseIMFn closeIM;
    XCreateICFn createIC;
    XDestroyICFn destroyIC;
    Xutf8LookupStringFn utf8LookupString;
} X11;

extern X11 s_X11;
extern X11Atoms s_X11Atoms;

RRMode findMode(
    XRRScreenResources* resources,
    const PalMonitorMode* mode);

void sendWMEvent(
    Window window,
    Atom type,
    long a,
    long b,
    long c,
    long d,
    PalBool add);

void xGetPrimaryMonitor(PalMonitor**);
MonitorData* xGetFreeMonitorData();
MonitorData* xFindMonitorData(PalMonitor* monitor);
void xFreeMonitorData(PalMonitor* monitor);
WindowData* xGetFreeWindowData();
WindowData* xFindWindowData(PalWindow* window);

#endif // PAL_HAS_X11_BACKEND
#endif // _PAL_X11_H