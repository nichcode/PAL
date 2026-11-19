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

#ifndef X11_HELPER_H
#define X11_HELPER_H
#if PAL_HAS_X11_BACKEND

#include "pal2/core/library.h"
#include "pal2/video/init.h"
#include "pal2/video/window.h"
#include "pal2/video/monitor.h"

#include <X11/extensions/Xrandr.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/Xresource.h>

#define TO_PAL_HANDLE(type, val) ((type*)(uintptr_t)(val))
#define FROM_PAL_HANDLE(type, handle) ((type)(uintptr_t)(handle))

#define X11_INTERN_ATOM(x) g_X11Atoms.x = g_X11.internAtom(g_X11.display, #x, False)
#define RANDR_SCREEN_CHANGE_EVENT 1040

typedef Display* (*XOpenDisplayFn)(const char*);
typedef int (*XCloseDisplayFn)(Display*);

typedef int (*XGetWindowAttributesFn)(
    Display*,
    Window,
    XWindowAttributes*);

typedef Bool (*XTranslateCoordinatesFn)(
    Display*,
    Window,
    Window,
    int,
    int,
    int*,
    int*,
    Window*);

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

typedef int (*XDeletePropertyFn)(
    Display*,
    Window,
    Atom);

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
    ...);

typedef void (*XDestroyICFn)(XIC);

typedef int (*Xutf8LookupStringFn)(
    XIC,
    XKeyPressedEvent*,
    char*,
    int,
    KeySym*,
    int*);

typedef struct X11Atoms {
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
    Atom _NET_WM_STATE_DEMANDS_ATTENTION;
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

typedef struct WindowData {
    PalBool skipConfigure;
    PalBool skipState;
    PalBool used;
    PalBool isAttached;
    PalBool skipIfAttached;
    int x;
    int y;
    uint32_t w;
    uint32_t dpi;
    uint32_t h;
    PalWindowState state;
    PalWindow* window;
    void* ic;
    unsigned long colormap;
} WindowData;

typedef struct MonitorData {
    PalBool used;
    uint32_t dpi;
    int x;
    int y;
    uint32_t w;
    uint32_t h;
    PalMonitor* monitor;
} MonitorData;

typedef struct X11 {
    PalLibrary* libx11;
    PalLibrary* libxrandr;
    PalLibrary* libcursor;

    PalBool ownsDisplay;
    PalBool error;
    PalBool skipScreenEvent;
    uint32_t bpp;
    int screen;
    int depth;
    uint32_t monitorCount;
    int rrEventBase;
    XIM im;
    Display* display;
    Window root;
    XContext dataID;

    uint32_t maxWindowData;
    uint32_t maxMonitorData;
    PalVideoFeatures features;
    const PalAllocator* allocator;
    struct PalEventDriver* eventDriver;
    WindowData* windowData;
    MonitorData* monitorData;

    XOpenDisplayFn openDisplay;
    XCloseDisplayFn closeDisplay;
    XGetWindowAttributesFn getWindowAttributes;
    XTranslateCoordinatesFn translateCoordinates;
    XGetWindowPropertyFn getWindowProperty;
    XDeletePropertyFn deleteProperty;
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

typedef struct X11Keyboard {
    int scancodes[512];
    int keycodes[256];
} X11Keyboard;

typedef struct X11Mouse {
    int32_t lastX;
    int32_t lastY;
} X11Mouse;

extern X11 g_X11;
extern X11Atoms g_X11Atoms;
extern X11Keyboard g_X11Keyboard;
extern X11Mouse g_X11Mouse;

RRMode x11FindMode(
    XRRScreenResources* resources,
    const PalMonitorMode* mode);

void x11SendWMEvent(
    Window window,
    Atom type,
    long a,
    long b,
    long c,
    long d,
    PalBool add);

MonitorData* x11GetFreeMonitorData(void);
MonitorData* x11FindMonitorData(PalMonitor* monitor);
void x11FreeMonitorData(PalMonitor* monitor);
WindowData* x11GetFreeWindowData(void);
WindowData* x11FindWindowData(PalWindow* window);

#endif // PAL_HAS_X11_BACKEND
#endif // X11_HELPER_H