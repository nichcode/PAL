
/**

Copyright (C) 2025 Nicholas Agbo <agbonicholas04@gmail.com>

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

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L
#include "pal/pal_video.h"

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

// X11 headers
#if PAL_HAS_X11
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/extensions/Xrandr.h>
#endif // PAL_HAS_X11

// Wayland headers
#if PAL_HAS_WAYLAND
#include <wayland-client.h>
#include <wayland-util.h>

#include <fcntl.h>
#include <locale.h>
#include <poll.h>
#include <sys/mman.h>

#include <linux/input-event-codes.h>
#include <xkbcommon/xkbcommon-keysyms.h>
#include <xkbcommon/xkbcommon.h>

#include <wayland-cursor.h>
#endif // PAL_HAS_WAYLAND

// ==================================================
// Typedefs, enums and structs
// ==================================================

#define TO_PAL_HANDLE(type, val) ((type*)(UintPtr)(val))
#define FROM_PAL_HANDLE(type, handle) ((type)(UintPtr)(handle))
#define MAX_SPAN_MONITORS 4

#pragma region EGL Typedefs

typedef void* EGLConfig;
typedef void* EGLSurface;
typedef void* EGLContext;
typedef void* EGLDisplay;
typedef void* EGLNativeDisplayType;

#define EGL_CAST(type, value) ((type)(value))
#define EGL_OPENGL_API 0x30A2
#define EGL_OPENGL_BIT 0x0008
#define EGL_OPENGL_ES_BIT 0x0001
#define EGL_OPENGL_ES_API 0x30A0
#define EGL_NO_CONTEXT EGL_CAST(EGLContext, 0)
#define EGL_NO_DISPLAY EGL_CAST(EGLDisplay, 0)
#define EGL_NO_SURFACE EGL_CAST(EGLSurface, 0)
#define EGL_NATIVE_VISUAL_ID 0x302E

typedef int32_t EGLint;
typedef unsigned int EGLBoolean;
typedef unsigned int EGLenum;

typedef void* (*eglGetProcAddressFn)(const char*);

typedef EGLBoolean (*eglInitializeFn)(
    EGLDisplay,
    EGLint*,
    EGLint*);

typedef EGLBoolean (*eglTerminateFn)(EGLDisplay);

typedef EGLDisplay (*eglGetDisplayFn)(void*);

typedef EGLBoolean (*eglChooseConfigFn)(
    EGLDisplay,
    const EGLint*,
    EGLConfig*,
    EGLint,
    EGLint*);

typedef EGLBoolean (*eglGetConfigAttribFn)(
    EGLDisplay,
    EGLConfig,
    EGLint,
    EGLint*);

typedef EGLint (*eglGetErrorFn)(void);

typedef EGLBoolean (*eglBindAPIFn)(EGLenum);

typedef EGLBoolean (*eglGetConfigsFn)(
    EGLDisplay,
    EGLConfig*,
    EGLint,
    EGLint*);

#pragma endregion

typedef struct {
    void* monitor;
    int dpi;
} SpanMonitor;

typedef struct {
    bool skipConfigure;
    bool skipState;
    bool used;
    bool isAttached;
    bool skipIfAttached;
    bool focused;
    bool pushConfigureEvent;
    bool pushStateEvent;
    int x;
    int y;
    Uint32 w;
    int dpi;
    int monitorCount;
    Uint32 h;
    PalWindowState state;
    PalWindow* window;

    // X11 only
    void* ic;
    unsigned long colormap;

    // Wayland only
    void* xdgSurface;
    void* xdgToplevel;
    void* buffer;
    void* decoration;
    void* cursor;
    void* eglWindow;
    SpanMonitor monitors[MAX_SPAN_MONITORS];
} WindowData;

typedef struct {
    bool used;
    int dpi;
    int x;
    int y;
    Uint32 w;
    Uint32 h;
    Uint32 refreshRate;
    Uint32 wlName;
    PalOrientation orientation;
    PalMonitor* monitor;
    PalMonitorMode mode; // wayland only sends current
    char name[32];
} MonitorData;

typedef struct {
    bool pendingScroll;
    Int32 lastX;
    Int32 lastY;
    Int32 dx;
    Int32 dy;
    Int32 WheelX;
    Int32 WheelY;
    float WheelXf;
    float WheelYf;
    bool state[PAL_MOUSE_BUTTON_MAX];
    double tmpScrollX;
    double tmpScrollY;
    double accumScrollX;
    double accumScrollY;
} Mouse;

typedef struct {
    bool scancodeState[PAL_SCANCODE_MAX];
    bool keycodeState[PAL_KEYCODE_MAX];
    int repeatRate;
    int repeatDelay;
    int repeatKey;
    int repeatScancode;
    int scancodes[512];
    int keycodes[256];
    Uint64 timer;
    Uint64 frequency;
} Keyboard;

typedef struct {
    void* handle;
    eglInitializeFn eglInitialize;
    eglTerminateFn eglTerminate;
    eglGetDisplayFn eglGetDisplay;
    eglChooseConfigFn eglChooseConfig;
    eglGetConfigAttribFn eglGetConfigAttrib;
    eglGetErrorFn eglGetError;
    eglBindAPIFn eglBindAPI;
    eglGetConfigsFn eglGetConfigs;
} EGL;

typedef struct {
    // clang-format off
    void (*shutdownVideo)();
    void (*updateVideo)();
    PalResult (*setFBConfig)(const int, PalFBConfigBackend);
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
    PalResult (*maximizeWindow)(PalWindow*);
    PalResult (*minimizeWindow)(PalWindow*);
    PalResult (*restoreWindow)(PalWindow*);
    PalResult (*showWindow)(PalWindow*);
    PalResult (*hideWindow)(PalWindow*);
    PalResult (*flashWindow)(PalWindow*, const PalFlashInfo*);
    PalResult (*getWindowStyle)(PalWindow*, PalWindowStyle*);
    PalResult (*getWindowMonitor)(PalWindow*, PalMonitor**);
    PalResult (*getWindowTitle)(PalWindow*, Uint64, Uint64*, char*);
    PalResult (*getWindowPos)(PalWindow*, Int32*, Int32*);
    PalResult (*getWindowSize)(PalWindow*, Uint32*, Uint32*);
    PalResult (*getWindowState)(PalWindow*, PalWindowState*);
    bool (*isWindowVisible)(PalWindow*);
    PalWindow* (*getFocusWindow)();
    PalWindowHandleInfo (*getWindowHandleInfo)(PalWindow*);
    PalWindowHandleInfoEx (*getWindowHandleInfoEx)(PalWindow*);
    PalResult (*setWindowOpacity)(PalWindow*, float);
    PalResult (*setWindowStyle)(PalWindow*, PalWindowStyle);
    PalResult (*setWindowTitle)(PalWindow*, const char*);
    PalResult (*setWindowPos)(PalWindow*, Int32, Int32);
    PalResult (*setWindowSize)(PalWindow*, Uint32, Uint32);
    PalResult (*setFocusWindow)(PalWindow*);

    PalResult (*createIcon)(const PalIconCreateInfo*, PalIcon**);
    void (*destroyIcon)(PalIcon*);
    PalResult (*setWindowIcon)(PalWindow*, PalIcon*);

    PalResult (*createCursor)(const PalCursorCreateInfo*, PalCursor**);
    PalResult (*createCursorFrom)(PalCursorType, PalCursor**);
    void (*destroyCursor)(PalCursor*);
    void (*showCursor)(bool);
    PalResult (*clipCursor)(PalWindow*, bool);
    PalResult (*getCursorPos)(PalWindow*, Int32*, Int32*);
    PalResult (*setCursorPos)(PalWindow*, Int32, Int32);
    PalResult (*setWindowCursor)(PalWindow*, PalCursor*);

    PalResult (*attachWindow)(void*, PalWindow**);
    PalResult (*detachWindow)(PalWindow*, void**);
    // clang-format off
} Backend;

typedef struct {
    bool initialized;
    Int32 maxWindowData;
    Int32 maxMonitorData;
    Int32 pixelFormat;
    PalVideoFeatures features;
    PalVideoFeatures64 features64;
    const PalAllocator* allocator;
    PalEventDriver* eventDriver;
    const Backend* backend;
    WindowData* windowData;
    MonitorData* monitorData;
    const char* className;
    void* platformInstance;
    void* display;
} VideoLinux;

static VideoLinux s_Video = {0};
static Mouse s_Mouse = {0};
static Keyboard s_Keyboard = {0};
static EGL s_Egl;

// ==================================================
// X11 Typedefs, enums and structs
// ==================================================

#pragma region X11 Typedefs
#if PAL_HAS_X11

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
    XVisualInfo* visualInfo;

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

static X11 s_X11 = {0};
static X11Atoms s_X11Atoms = {0};

#endif // PAL_HAS_X11
#pragma endregion

// ==================================================
// Wayland Typedefs, enums and structs
// ==================================================

#pragma region Wayland Typedefs
#if PAL_HAS_WAYLAND

typedef struct wl_display* (*wl_display_connect_fn)(const char*);
typedef void (*wl_display_disconnect_fn)(struct wl_display*);
typedef int (*wl_display_roundtrip_fn)(struct wl_display*);
typedef int (*wl_display_dispatch_fn)(struct wl_display*);
typedef void (*wl_proxy_destroy_fn)(struct wl_proxy*);

typedef int (*wl_proxy_add_listener_fn)(
    struct wl_proxy*, 
    void (**)(void), void*);

typedef struct wl_proxy* (*wl_proxy_marshal_constructor_v_fn)(
    struct wl_proxy*, 
    uint32_t, 
    const struct wl_interface*, 
    uint32_t, ...);

typedef struct wl_proxy* (*wl_proxy_marshal_flags_fn)(
    struct wl_proxy*, 
    uint32_t, 
    const struct wl_interface*, 
    uint32_t, 
    uint32_t, ...);

typedef uint32_t (*wl_proxy_get_version_fn)(struct wl_proxy*);

typedef int (*wl_proxy_add_listener_fn)(
    struct wl_proxy*,
    void (**)(void), void*);

typedef int (*wl_display_get_error_fn)(struct wl_display*);
typedef int (*wl_display_dispatch_pending_fn)(struct wl_display*);
typedef int (*wl_display_flush_fn)(struct wl_display*);
typedef int (*wl_display_prepare_read_fn)(struct wl_display*);
typedef int (*wl_display_read_events_fn)(struct wl_display*);
typedef int (*wl_display_get_fd_fn)(struct wl_display*);
typedef void (*wl_display_cancel_read_fn)(struct wl_display*);

// xkb
typedef void (*xkb_keymap_unref_fn)(struct xkb_keymap*);
typedef struct xkb_state* (*xkb_state_new_fn)(struct xkb_keymap*);
typedef void (*xkb_state_unref_fn)(struct xkb_state*);
typedef void (*xkb_context_unref_fn)(struct xkb_context*);
typedef struct xkb_context* (*xkb_context_new_fn)(enum xkb_context_flags);
typedef uint32_t (*xkb_keysym_to_utf32_fn)(xkb_keysym_t);

typedef xkb_keysym_t (*xkb_state_key_get_one_sym_fn)(
    struct xkb_state*, 
    xkb_keycode_t);

typedef struct xkb_keymap* (*xkb_keymap_new_from_string_fn)(
    struct xkb_context*, 
    const char*,
    enum xkb_keymap_format,
    enum xkb_keymap_compile_flags);

typedef enum xkb_state_component (*xkb_state_update_mask_fn)(
    struct xkb_state*,
    xkb_mod_mask_t,
    xkb_mod_mask_t,
    xkb_mod_mask_t,
    xkb_layout_index_t,
    xkb_layout_index_t,
    xkb_layout_index_t);

typedef int (*xkb_keymap_key_repeats_fn)(
    struct xkb_keymap*, 
    xkb_keycode_t);

// wayland cursor
typedef struct wl_cursor_theme* (*wl_cursor_theme_load_fn)(
    const char*, 
    int, 
    struct wl_shm*);

typedef struct wl_cursor* (*wl_cursor_theme_get_cursor_fn)(
    struct wl_cursor_theme*,
    const char*);

typedef struct wl_buffer* (*wl_cursor_image_get_buffer_fn)(
    struct wl_cursor_image*);

// egl_window
struct wl_egl_window;
struct wl_surface;

typedef struct wl_egl_window* (*wl_egl_window_create_fn)(
    struct wl_surface*,
    int, 
    int);

typedef void (*wl_egl_window_destroy_fn)(struct wl_egl_window*);

typedef void (*wl_egl_window_resize_fn)(
    struct wl_egl_window*,
    int, 
    int, 
    int, 
    int);

typedef struct {
    bool checkFeatures;
    int monitorCount;
    
    void* handle;
    void* xkbCommon;
    void* libCursor;
    void* libWaylandEgl;
    struct wl_display* display;
    struct wl_registry* registry;
    struct xdg_wm_base* xdgBase;
    struct wl_compositor* compositor;
    struct wl_shm* shm;
    struct wl_seat* seat;
    struct wl_pointer* pointer;
    struct wl_keyboard* keyboard;
    struct zxdg_decoration_manager_v1* decorationManager;
    struct zwp_pointer_constraints* pointerConstraints;
    struct wl_surface* pointerSurface;
    struct wl_surface* keyboardSurface;
    struct wl_cursor_theme* cursorTheme;

    struct xkb_context* inputContext;
    struct xkb_keymap* keymap;
    struct xkb_state* state;

    const struct wl_interface* outputInterface;
    const struct wl_interface* seatInterface;
    const struct wl_interface* compositorInterface;
    const struct wl_interface* registryInterface;
    const struct wl_interface* surfaceInterface;
    const struct wl_interface* shmInterface;
    const struct wl_interface* bufferInterface;
    const struct wl_interface* shmPoolInterface;
    const struct wl_interface* regionInterface;
    const struct wl_interface* pointerInterface;
    const struct wl_interface* keyboardInterface;

    wl_display_connect_fn displayConnect;
    wl_display_disconnect_fn displayDisconnect;
    wl_display_roundtrip_fn displayRoundtrip;
    wl_display_dispatch_fn displayDispatch;
    wl_proxy_destroy_fn proxyDestroy;
    wl_proxy_add_listener_fn proxyAddListener;
    wl_proxy_marshal_constructor_v_fn proxyMarshalCnstructor;
    wl_proxy_marshal_flags_fn proxyMarshalFlags;
    wl_proxy_get_version_fn proxyGetVersion;
    wl_display_get_error_fn getError;
    wl_display_dispatch_pending_fn dispatchPending;
    wl_display_flush_fn displayFlush;
    wl_display_prepare_read_fn prepareRead;
    wl_display_read_events_fn readEvents;
    wl_display_get_fd_fn displayGetFd;
    wl_display_cancel_read_fn cancelRead;

    xkb_keymap_unref_fn xkbKeymapUnref;
    xkb_state_new_fn xkbStateNew;
    xkb_state_unref_fn xkbStateUnref;
    xkb_context_unref_fn xkbContextUnref;
    xkb_context_new_fn xkbContextNew;
    xkb_state_key_get_one_sym_fn xkbStateKeyGetOneSym;
    xkb_keymap_new_from_string_fn xkbKeymapNewFromString;
    xkb_keysym_to_utf32_fn xkbKeysymToUtf32;
    xkb_state_update_mask_fn xkbStateUpdateMask;
    xkb_keymap_key_repeats_fn xkbKeymapKeyRepeats;

    wl_cursor_theme_load_fn cursorThemeLoad;
    wl_cursor_theme_get_cursor_fn cursorThemeGetCursor;
    wl_cursor_image_get_buffer_fn cursorImageGetBuffer;

    EGLConfig eglFBConfig;
    wl_egl_window_create_fn eglWindowCreate;
    wl_egl_window_destroy_fn eglWindowDestroy;
    wl_egl_window_resize_fn eglWindowResize;
} Wayland;

typedef struct {
    struct wl_buffer* buffer;
    struct wl_surface* surface;
    int hotspotX;
    int hotspotY;
} WaylandCursor;

static Wayland s_Wl = {0};

#endif // PAL_HAS_WAYLAND
#pragma endregion

#pragma region Wayland-Client-Protocol
#if PAL_HAS_WAYLAND

static WindowData* findWindowData(PalWindow* window);
static MonitorData* findMonitorData(PalMonitor* monitor);

static inline Uint64 getTime()
{
    Uint64 now = palGetPerformanceCounter();
    return (now * 1000) / s_Keyboard.frequency;
}

static inline void* wlRegistryBind(
    struct wl_registry *wl_registry, 
    uint32_t name, 
    const struct wl_interface *interface, 
    uint32_t version)
{
	struct wl_proxy *id;
	id = s_Wl.proxyMarshalFlags(
        (struct wl_proxy *)wl_registry,
        WL_REGISTRY_BIND, 
        interface, 
        version, 
        0, 
        name, 
        interface->name, 
        version, 
        NULL);

	return (void *)id;
}

static inline int wlRegistryAddListener(
    struct wl_registry *wl_registry,
    const struct wl_registry_listener *listener, 
    void *data)
{
	return s_Wl.proxyAddListener(
        (struct wl_proxy *) wl_registry,
        (void (**)(void)) listener, data);
}

static inline struct wl_registry* wlDisplayGetRegistry(
    struct wl_display *wl_display)
{
	struct wl_proxy *registry;
	registry = s_Wl.proxyMarshalFlags(
        (struct wl_proxy *) wl_display,
        1, // WL_DISPLAY_GET_REGISTRY
        s_Wl.registryInterface, 
        s_Wl.proxyGetVersion(
            (struct wl_proxy *) wl_display), 
            0, 
            NULL);

	return (struct wl_registry *)registry;
}

static inline int wlOutputAddListener(
    struct wl_output *wl_output,
    const struct wl_output_listener *listener, 
    void *data)
{
	return s_Wl.proxyAddListener(
        (struct wl_proxy *) wl_output,
        (void (**)(void)) listener, data);
}

static inline struct wl_surface* wlCompositorCreateSurface(
    struct wl_compositor *wl_compositor)
{
	struct wl_proxy *id;
	id = s_Wl.proxyMarshalFlags(
        (struct wl_proxy *) wl_compositor,
        0, // WL_COMPOSITOR_CREATE_SURFACE,
        s_Wl.surfaceInterface, 
        s_Wl.proxyGetVersion(
            (struct wl_proxy *) wl_compositor), 
            0, 
            NULL);

	return (struct wl_surface*) id;
}

static inline void wlSurfaceCommit(struct wl_surface *wl_surface)
{
	s_Wl.proxyMarshalFlags(
        (struct wl_proxy *) wl_surface,
        6, // WL_SURFACE_COMMIT
        NULL, 
        s_Wl.proxyGetVersion(
            (struct wl_proxy *) wl_surface), 
            0);
}

static inline void wlSurfaceDestroy(struct wl_surface *wl_surface)
{
	s_Wl.proxyMarshalFlags(
        (struct wl_proxy *) wl_surface,
        0, // WL_SURFACE_DESTROY
        NULL, 
        s_Wl.proxyGetVersion(
            (struct wl_proxy *) wl_surface), 
            WL_MARSHAL_FLAG_DESTROY);
}

static inline struct wl_shm_pool* wlShmCreatePool(
    struct wl_shm *wl_shm, 
    int32_t fd, 
    int32_t size)
{
	struct wl_proxy *id;
	id = s_Wl.proxyMarshalFlags(
        (struct wl_proxy *) wl_shm,
        0, // WL_SHM_CREATE_POOL
        s_Wl.shmPoolInterface, 
        s_Wl.proxyGetVersion(
            (struct wl_proxy *) wl_shm), 
            0, 
            NULL, 
            fd, 
            size);

	return (struct wl_shm_pool *) id;
}

static inline void wlShmPoolDestroy(struct wl_shm_pool *wl_shm_pool)
{
	s_Wl.proxyMarshalFlags(
        (struct wl_proxy *) wl_shm_pool,
        WL_SHM_POOL_DESTROY, 
        NULL, 
        s_Wl.proxyGetVersion(
            (struct wl_proxy *) wl_shm_pool), 
            WL_MARSHAL_FLAG_DESTROY);
}

static inline struct wl_buffer* wlShmPoolCreateBuffer(
    struct wl_shm_pool *wl_shm_pool, 
    int32_t offset, 
    int32_t width, 
    int32_t height, 
    int32_t stride, 
    uint32_t format)
{
	struct wl_proxy *id;
	id = s_Wl.proxyMarshalFlags(
        (struct wl_proxy *) wl_shm_pool,
        WL_SHM_POOL_CREATE_BUFFER, 
        s_Wl.bufferInterface, 
        s_Wl.proxyGetVersion(
            (struct wl_proxy *) wl_shm_pool),
            0, 
            NULL, 
            offset,
            width, 
            height, 
            stride, 
            format);

	return (struct wl_buffer *) id;
}

static inline void wlBufferDestroy(struct wl_buffer *wl_buffer)
{
	s_Wl.proxyMarshalFlags(
        (struct wl_proxy *) wl_buffer,
        WL_BUFFER_DESTROY, 
        NULL, 
        s_Wl.proxyGetVersion(
            (struct wl_proxy *) wl_buffer),
            WL_MARSHAL_FLAG_DESTROY);
}

static inline void wlSurfaceAttach(
    struct wl_surface *wl_surface, 
    struct wl_buffer *buffer, 
    int32_t x, 
    int32_t y)
{
	s_Wl.proxyMarshalFlags(
        (struct wl_proxy *) wl_surface,
        WL_SURFACE_ATTACH, 
        NULL, 
        s_Wl.proxyGetVersion(
            (struct wl_proxy *) wl_surface), 
            0, 
            buffer, 
            x, 
            y);
}

static inline void wlSurfaceDamageBuffer(
    struct wl_surface *wl_surface, 
    int32_t x, 
    int32_t y, 
    int32_t width, 
    int32_t height)
{
	s_Wl.proxyMarshalFlags(
        (struct wl_proxy *) wl_surface,
        WL_SURFACE_DAMAGE_BUFFER, 
        NULL, 
        s_Wl.proxyGetVersion(
            (struct wl_proxy *) wl_surface), 
            0, 
            x, 
            y, 
            width, 
            height);
}

static inline int wlSurfaceAddListener(
    struct wl_surface *wl_surface,
    const struct wl_surface_listener *listener, 
    void *data)
{
	return s_Wl.proxyAddListener(
        (struct wl_proxy *) wl_surface,
        (void (**)(void)) listener, data);
}

static inline int wlSeatAddListener(
    struct wl_seat *wl_seat,
    const struct wl_seat_listener *listener, 
    void *data)
{
	return s_Wl.proxyAddListener(
        (struct wl_proxy *) wl_seat,
        (void (**)(void)) listener, data);
}

static inline struct wl_pointer* wlSeatGetPointer(struct wl_seat *wl_seat)
{
	struct wl_proxy *id;
	id = s_Wl.proxyMarshalFlags(
        (struct wl_proxy *) wl_seat,
        WL_SEAT_GET_POINTER, 
        s_Wl.pointerInterface, 
        s_Wl.proxyGetVersion(
            (struct wl_proxy *) wl_seat), 
            0, 
            NULL);

	return (struct wl_pointer *) id;
}

static inline struct wl_keyboard* wlSeatGetKeyboard(struct wl_seat *wl_seat)
{
	struct wl_proxy *id;
	id = s_Wl.proxyMarshalFlags(
        (struct wl_proxy *) wl_seat,
        WL_SEAT_GET_KEYBOARD, 
        s_Wl.keyboardInterface, 
        s_Wl.proxyGetVersion(
            (struct wl_proxy *) wl_seat), 
            0, 
            NULL);

	return (struct wl_keyboard *) id;
}

static inline int wlPointerAddListener(
    struct wl_pointer *wl_pointer,
    const struct wl_pointer_listener *listener, 
    void *data)
{
	return s_Wl.proxyAddListener(
        (struct wl_proxy *) wl_pointer,
        (void (**)(void)) listener, data);
}

static inline void wlPointerSetCursor(
    struct wl_pointer *wl_pointer, 
    uint32_t serial, 
    struct wl_surface *surface, 
    int32_t hotspot_x, 
    int32_t hotspot_y)
{
	s_Wl.proxyMarshalFlags(
        (struct wl_proxy *) wl_pointer,
        WL_POINTER_SET_CURSOR, 
        NULL, 
        s_Wl.proxyGetVersion(
            (struct wl_proxy *) wl_pointer), 
            0, 
            serial, 
            surface, 
            hotspot_x, 
            hotspot_y);
}

static inline int wlKeyboardAddListener(
    struct wl_keyboard *wl_keyboard,
    const struct wl_keyboard_listener *listener, 
    void *data)
{
	return s_Wl.proxyAddListener(
        (struct wl_proxy *) wl_keyboard,
        (void (**)(void)) listener, data);
}

static inline struct wl_region* wlCompositorCreateRegion(
    struct wl_compositor *wl_compositor)
{
	struct wl_proxy *id;
	id = s_Wl.proxyMarshalFlags(
        (struct wl_proxy *) wl_compositor,
        WL_COMPOSITOR_CREATE_REGION, 
        s_Wl.regionInterface, 
        s_Wl.proxyGetVersion(
            (struct wl_proxy *) wl_compositor), 
            0, 
            NULL);

	return (struct wl_region *) id;
}

static inline void wlRegionAdd(
    struct wl_region *wl_region, 
    int32_t x, 
    int32_t y, 
    int32_t width, 
    int32_t height)
{
	s_Wl.proxyMarshalFlags(
        (struct wl_proxy *) wl_region,
        WL_REGION_ADD, 
        NULL, 
        s_Wl.proxyGetVersion(
            (struct wl_proxy *) wl_region), 
            0, 
            x, 
            y, 
            width, 
            height);
}

static inline void wlSurfaceSetOpaqueRegion(
    struct wl_surface *wl_surface, 
    struct wl_region *region)
{
	s_Wl.proxyMarshalFlags(
        (struct wl_proxy *) wl_surface,
        WL_SURFACE_SET_OPAQUE_REGION, 
        NULL, 
        s_Wl.proxyGetVersion(
            (struct wl_proxy *) wl_surface), 
            0, 
            region);
}

static inline void wlRegionDestroy(struct wl_region *wl_region)
{
	s_Wl.proxyMarshalFlags(
        (struct wl_proxy *) wl_region,
        WL_REGION_DESTROY, 
        NULL, 
        s_Wl.proxyGetVersion(
            (struct wl_proxy *) wl_region), 
            WL_MARSHAL_FLAG_DESTROY);
}

static void surfaceHandleEnter(
    void* userData,
    struct wl_surface* surface,
    struct wl_output* output)
{
    WindowData* data = userData;
    MonitorData* monitorData = findMonitorData((PalMonitor*)output);
    if (!monitorData) {
        return;
    }

    // wayland sends multiple surface enter events
    // if the surface spans multiple monitors
    // we get all and return the highest dpi
    // this assumes a surface can only span 4 monitors
    // at the sametime but this might be wrong
    // FIXME: check if we need more
    if (data->monitorCount < MAX_SPAN_MONITORS) {
        SpanMonitor* span = &data->monitors[data->monitorCount];
        span->monitor = output;
        span->dpi = monitorData->dpi;
        data->monitorCount++;
    }

    if (data->dpi == 0) {
        // this is triggered when the window is created
        // we cache the DPI and skip the event
        data->dpi = monitorData->dpi;        
        return;
    }

    // the code below should be skipped if users are not 
    // interested in DPI changed events
    PalDispatchMode mode = PAL_DISPATCH_NONE;
    PalEventType type = PAL_EVENT_MONITOR_DPI_CHANGED;
    if (!s_Video.eventDriver) {
        return;
    }

    PalEventDriver* driver = s_Video.eventDriver;
    mode = palGetEventDispatchMode(driver, type);
    if (mode == PAL_DISPATCH_NONE) {
        return;
    }

    // get the highest dpi and check if the it has changed
    int maxDPI = 96; // baseline
    for (int i = 0; i < data->monitorCount; i++) {
        if (!data->monitors[i].monitor) {
            // not a valid index. continue
            continue;
        }

        if (data->monitors[i].dpi > maxDPI) {
            // new highest
            maxDPI = data->monitors[i].dpi;
        }
    }

    if (maxDPI != data->dpi) {
        data->dpi = maxDPI;

        PalEvent event = {0};
        event.type = type;
        event.data = maxDPI;
        event.data2 = palPackPointer((void*)data->window);
        palPushEvent(driver, &event);
    }
}

static void surfaceHandleLeave(
    void* userData,
    struct wl_surface* surface,
    struct wl_output* output)
{
    // remove the monitor from our span monitor list
    WindowData* data = userData;
    MonitorData* monitorData = findMonitorData((PalMonitor*)output);
    if (!monitorData) {
        return;
    }

    for (int i = 0; i < data->monitorCount; i++) {
        if (data->monitors[i].monitor == (void*)output) {
            // found our monitor
            // we might want to pack the array but its just 4 monitors
            // so we leave it like that
            data->monitors[i].monitor = nullptr;
            data->monitorCount--;
            break;
        }
    }
}

static struct wl_surface_listener surfaceListener = {
    .enter = surfaceHandleEnter,
    .leave = surfaceHandleLeave
};

static void pointerHandleEnter(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t serial,
    struct wl_surface* surface,
    wl_fixed_t surface_x,
    wl_fixed_t surface_y)
{
    WindowData* data = findWindowData((PalWindow*)surface);
    if (!data) {
        return;
    }

    if (data->cursor) {
        // our window
        WaylandCursor* cursor = data->cursor;
        wlPointerSetCursor(
            pointer, 
            serial, 
            cursor->surface,
            cursor->hotspotX, 
            cursor->hotspotY);
    }

    // cache the surface the pointer is currently on
    s_Wl.pointerSurface = surface;
}

static void pointerHandleLeave(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t serial,
    struct wl_surface* surface)
{
    if (s_Wl.pointerSurface == surface) {
        s_Wl.pointerSurface == nullptr;
    }
}

static void pointerHandleMotion(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t time,
    wl_fixed_t surface_x,
    wl_fixed_t surface_y)
{
    int x = wl_fixed_to_int(surface_x);
    int y = wl_fixed_to_int(surface_y);
    const int dx = x - s_Mouse.lastX;
    const int dy = y - s_Mouse.lastY;

    PalDispatchMode mode = PAL_DISPATCH_NONE;
    PalWindow* window = (PalWindow*)s_Wl.pointerSurface;
    if (s_Video.eventDriver && window) {
        // we only push a mouse move only if we are on a window
        PalEventDriver* driver = s_Video.eventDriver;
        PalEventType type = PAL_EVENT_MOUSE_MOVE;
        mode = palGetEventDispatchMode(driver, type);
        if (mode != PAL_DISPATCH_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data = palPackInt32(x, y);
            event.data2 = palPackPointer(window);
            palPushEvent(driver, &event);
        }

        // push a mouse delta event
        type = PAL_EVENT_MOUSE_DELTA;
        mode = palGetEventDispatchMode(driver, type);
        if (mode != PAL_DISPATCH_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data = palPackInt32(dx, dy);
            event.data2 = palPackPointer(window);
            palPushEvent(driver, &event);
        }
    }

    s_Mouse.lastX = x;
    s_Mouse.lastY = y;
    s_Mouse.dx = dx;
    s_Mouse.dy = dy;
}

static void pointerHandleButton(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t serial,
    uint32_t time,
    uint32_t button,
    uint32_t state)
{
    PalWindow* window = nullptr;
    if (s_Wl.pointerSurface) {
        window = (PalWindow*)s_Wl.pointerSurface;

    } else {
        // cannot recieve events without a focused surface
        return;
    }
    
    bool pressed = state == WL_POINTER_BUTTON_STATE_PRESSED;
    PalMouseButton _button = 0;
    PalEventType type = PAL_EVENT_MOUSE_BUTTONUP;

    if (button == BTN_LEFT) {
        _button = PAL_MOUSE_BUTTON_LEFT;

    } else if (button == BTN_RIGHT) {
        _button = PAL_MOUSE_BUTTON_RIGHT;

    } else if (button == BTN_MIDDLE) {
        _button = PAL_MOUSE_BUTTON_MIDDLE;

    } else if (button == BTN_SIDE) {
        _button = PAL_MOUSE_BUTTON_X1;

    } else if (button == BTN_EXTRA) {
        _button = PAL_MOUSE_BUTTON_X2;
    }

    if (pressed) {
        type = PAL_EVENT_MOUSE_BUTTONDOWN;
    }

    s_Mouse.state[_button] = pressed;
    if (s_Video.eventDriver) {
        PalEventDriver* driver = s_Video.eventDriver;
        PalDispatchMode mode = PAL_DISPATCH_NONE;
        mode = palGetEventDispatchMode(driver, type);
        if (mode != PAL_DISPATCH_NONE) {
            PalEvent event = {0};
            event.type = type;

            // since we are not drawing decorations for users
            // they need the serial in order to draw their decorations
            // we put the serial at the upper 32 so ABI is preserved
            event.data = palPackUint32(_button, serial);
            event.data2 = palPackPointer(window);
            palPushEvent(driver, &event);
        }
    }
}

static void pointerHandleAxis(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t time,
    uint32_t axis,
    wl_fixed_t value)
{
    double delta = wl_fixed_to_double(value);
    if (axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL) {
        s_Mouse.tmpScrollX += delta;
        s_Mouse.accumScrollX += delta;

    } else if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL) {
        s_Mouse.tmpScrollY += delta;
        s_Mouse.accumScrollY += delta;
    }
    
    s_Mouse.pendingScroll = true;
}

static void pointerHandleAxisDiscrete(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t axis,
    int32_t discrete)
{
    if (axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL) {
        s_Mouse.tmpScrollX += discrete;
        s_Mouse.accumScrollX += discrete;

    } else if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL) {
        s_Mouse.tmpScrollY += discrete;
        s_Mouse.accumScrollY += discrete;
    }
    
    s_Mouse.pendingScroll = true;
}

static void pointerHandleFrame(
    void* userData,
    struct wl_pointer* pointer)
{
    if (!s_Mouse.pendingScroll) {
        // no wheel event
        return;
    }

    PalWindow* window = nullptr;
    if (s_Wl.pointerSurface) {
        window = (PalWindow*)s_Wl.pointerSurface;

    } else {
        // cannot recieve events without a focused surface
        return;
    }

    const int dx = (int)s_Mouse.accumScrollX;
    const int dy = (int)s_Mouse.accumScrollY;
    if (s_Video.eventDriver) {
        PalEventType type = PAL_EVENT_MOUSE_WHEEL;
        PalEventDriver* driver = s_Video.eventDriver;
        PalDispatchMode mode = PAL_DISPATCH_NONE;
        mode = palGetEventDispatchMode(driver, type);
        if (mode != PAL_DISPATCH_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data = palPackUint32(dx, dy);
            event.data2 = palPackPointer(window);
            palPushEvent(driver, &event);
        }
    }

    s_Mouse.WheelX = dx;
    s_Mouse.WheelY = dy;
    s_Mouse.accumScrollX -= dx;
    s_Mouse.accumScrollY -= dy;
    s_Mouse.pendingScroll = false;
}

static void pointerHandleAxisSource(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t axis_source)
{

}

static void pointerHandleAxisStop(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t time,
	uint32_t axis)
{
    
}

static void keyboardHandleEnter(
    void* userData,
    struct wl_keyboard* keyboard,
    uint32_t serial,
    struct wl_surface* surface,
    struct wl_array* keys)
{
    // cache the surface the keyboard is currently on
    s_Wl.keyboardSurface = surface;
}

static void keyboardHandleLeave(
    void* userData,
    struct wl_keyboard* keyboard,
    uint32_t serial,
    struct wl_surface* surface)
{
    if (s_Wl.keyboardSurface == surface) {
        s_Wl.keyboardSurface == nullptr;
    }
}

static void keyboardHandleRemap(
    void* userData,
    struct wl_keyboard* keyboard,
    uint32_t format,
    int32_t fd,
    uint32_t size)
{
    if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        close(fd);
        return;
    }

    struct xkb_keymap* keymap = nullptr;
    struct xkb_state* state = nullptr;

    char* keymapStr = mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0);
    if (keymapStr == MAP_FAILED) {
        close(fd);
        return;
    }

    keymap = s_Wl.xkbKeymapNewFromString(
        s_Wl.inputContext,
        keymapStr,
        XKB_KEYMAP_FORMAT_TEXT_V1,
        XKB_KEYMAP_COMPILE_NO_FLAGS);

    if (!keymap) {
        return;
    }

    munmap(keymapStr, size);
    close(fd);

    state = s_Wl.xkbStateNew(keymap);
    if (!state) {
        return;
    }

    // check if we have old keymap and state
    if (s_Wl.state) {
        s_Wl.xkbStateUnref(s_Wl.state);
        s_Wl.xkbKeymapUnref(s_Wl.keymap);
    }

    s_Wl.state = state;
    s_Wl.keymap = keymap;
    s_Keyboard.frequency = palGetPerformanceFrequency();
}

static void keyboardHandleKey(
    void* userData,
    struct wl_keyboard* keyboard,
    uint32_t serial,
    uint32_t time,
    uint32_t key,
    uint32_t state)
{
    PalWindow* window = nullptr;
    if (s_Wl.keyboardSurface) {
        window = (PalWindow*)s_Wl.keyboardSurface;

    } else {
        // cannot recieve events without a focused surface
        return;
    }

    PalScancode scancode = 0;
    PalKeycode keycode = 0;
    bool pressed = (state == WL_KEYBOARD_KEY_STATE_PRESSED);
    PalEventType type = PAL_EVENT_KEYUP;
    PalDispatchMode mode = PAL_DISPATCH_NONE;
    xkb_keysym_t keySym = s_Wl.xkbStateKeyGetOneSym(s_Wl.state, key + 8);

    // special handling
    if (key == 119) {
        scancode = PAL_SCANCODE_PAUSE;
    } else if (key == 107) {
        scancode = PAL_SCANCODE_END;
    } else if (key == 103) {
        scancode = PAL_SCANCODE_UP;
    } else if (key == 102) {
        scancode = PAL_SCANCODE_HOME;
    
    } else {
        scancode = s_Keyboard.scancodes[key];
    }

    // printable and text input keys are from the range
    // 32 (PAL_KEYCODE_SPACE) and 122 (PAL_KEYCODE_Z)
    // The rest are almost the same as their scancode
    // Maybe there will be a layout that makes this wrong
    // but for now this works
    if (keySym >= XKB_KEY_space && keySym <= XKB_KEY_z) {
        // a printable or input key
        keycode = s_Keyboard.keycodes[keySym];

    } else {
        // Since PalKeycode and PalScancode have the same integers
        // we can make a direct cast without a table
        // Examle: PAL_KEYCODE_A(int 0) == PAL_SCANCODE_A(int 0)
        keycode = (PalKeycode)(Uint32)scancode;
    }

    // If we got a keySym but its not mapped into our keycode array
    // we do a direct cast as well
    if (keycode == PAL_KEYCODE_UNKNOWN) {
        keycode = (PalKeycode)(Uint32)scancode;
    }

    s_Keyboard.scancodeState[scancode] = pressed;
    s_Keyboard.keycodeState[keycode] = pressed;

    // check for key repeats
    if (pressed) {
        if (s_Wl.xkbKeymapKeyRepeats(s_Wl.keymap, key + 8)) {
            s_Keyboard.repeatKey = keycode;
            s_Keyboard.repeatScancode = scancode;
            s_Keyboard.timer = getTime() + s_Keyboard.repeatDelay;

        } else {
            s_Keyboard.repeatKey = 0;
        }

        type = PAL_EVENT_KEYDOWN;

    } else {
        // key release
        if (s_Keyboard.repeatKey == keycode) {
            s_Keyboard.repeatKey = 0;
        }
    }

    if (s_Video.eventDriver) {
        PalEventDriver* driver = s_Video.eventDriver;
        mode = palGetEventDispatchMode(driver, type);
        if (mode != PAL_DISPATCH_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data = palPackUint32(keycode, scancode);
            event.data2 = palPackPointer(window);
            palPushEvent(driver, &event);
        }

        // check for char event if enabled
        type = PAL_EVENT_KEYCHAR;
        mode = palGetEventDispatchMode(driver, type);
        if (mode == PAL_DISPATCH_NONE) {
            return;
        }

        Uint32 codepoint = s_Wl.xkbKeysymToUtf32(keySym);
        if (codepoint <= 0) {
            return;
        }

        PalEvent event = {0};
        event.type = type;
        event.data = codepoint;
        event.data2 = palPackPointer(window);
        palPushEvent(driver, &event);
    }
}

static void keyboardHandleRepeatInfo(
    void* userData,
    struct wl_keyboard* keyboard,
    int32_t rate,
    int32_t delay)
{
    if (s_Wl.keyboard == keyboard) {
        s_Keyboard.repeatDelay = delay;
        s_Keyboard.repeatRate = rate;

    } else {
        if (s_Keyboard.repeatDelay == 0) {
            s_Keyboard.repeatDelay = 500;
            s_Keyboard.repeatRate = 30;
        }
    }
}

static void keyboardHandleModifiers(
    void* userData,
    struct wl_keyboard* keyboard,
    uint32_t serial,
    uint32_t mods_depressed,
    uint32_t mods_latched,
    uint32_t mods_locked,
    uint32_t group)
{
    if (s_Wl.state) {
        s_Wl.xkbStateUpdateMask(
            s_Wl.state,
            mods_depressed,
            mods_latched,
            mods_locked,
            group,
            0,
            0);
    }
}

static struct wl_pointer_listener pointerListener = {
    .enter = pointerHandleEnter,
    .leave = pointerHandleLeave,
    .motion = pointerHandleMotion,
    .button = pointerHandleButton,
    .axis = pointerHandleAxis,
    .axis_discrete = pointerHandleAxisDiscrete,
    .frame = pointerHandleFrame,
    .axis_source = pointerHandleAxisSource,
    .axis_stop = pointerHandleAxisStop
};

static struct wl_keyboard_listener keyboardListener = {
    .enter = keyboardHandleEnter,
    .leave = keyboardHandleLeave,
    .keymap = keyboardHandleRemap,
    .key = keyboardHandleKey,
    .repeat_info = keyboardHandleRepeatInfo,
    .modifiers = keyboardHandleModifiers
};

static void seatHandleCapabilities(
    void* userData,
    struct wl_seat* seat,
    enum wl_seat_capability caps)
{
    if (caps & WL_SEAT_CAPABILITY_KEYBOARD) {
        s_Wl.keyboard = wlSeatGetKeyboard(seat);
        wlKeyboardAddListener(s_Wl.keyboard, &keyboardListener, nullptr);
    }

    if (caps & WL_SEAT_CAPABILITY_POINTER) {
        s_Wl.pointer = wlSeatGetPointer(seat);
        wlPointerAddListener(s_Wl.pointer, &pointerListener, nullptr);
    }
}

static void seatHandleName(
    void* userData,
    struct wl_seat* seat,
    const char* name)
{
    
}

static struct wl_seat_listener seatListener = {
    .capabilities = seatHandleCapabilities,
    .name = seatHandleName
};

#endif // PAL_HAS_WAYLAND
#pragma endregion

#pragma region Xdg-Shell-Protocol
#if PAL_HAS_WAYLAND

struct xdg_wm_base;
struct xdg_surface;
struct xdg_toplevel;

// forward declare
static struct wl_buffer* createShmBuffer(
    int width,
    int height,
    const Uint8* pixels,
    bool cursor);

const struct wl_interface xdg_popup_interface;
const struct wl_interface xdg_positioner_interface;
const struct wl_interface xdg_surface_interface;
const struct wl_interface xdg_toplevel_interface;

struct xdg_wm_base_listener {
	void (*ping)(void*, struct xdg_wm_base*, uint32_t);
};

struct xdg_surface_listener {
	void (*configure)(void*, struct xdg_surface*, uint32_t);
};

struct xdg_toplevel_listener {
    // clang-format off
	void (*configure)(void*, struct xdg_toplevel*, int32_t, int32_t, struct wl_array*);
	void (*close)(void*, struct xdg_toplevel*);
	void (*configure_bounds)(void*, struct xdg_toplevel*, int32_t, int32_t);
	void (*wm_capabilities)(void*, struct xdg_toplevel*, struct wl_array*);
    // clang-format on
};

static inline void xdgWmBasePong(
    struct xdg_wm_base* xdg_wm_base,
    uint32_t serial)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_wm_base,
        3, // XDG_WM_BASE_PONG
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)xdg_wm_base),
        0,
        serial);
}

static inline int xdgWmBaseAddListener(
    struct xdg_wm_base* xdg_wm_base,
    const struct xdg_wm_base_listener* listener,
    void* data)
{
    return s_Wl.proxyAddListener(
        (struct wl_proxy*)xdg_wm_base,
        (void (**)(void))listener,
        data);
}

static inline struct xdg_surface* xdgWmBaseGetXdgSurface(
    struct xdg_wm_base* xdg_wm_base,
    struct wl_surface* surface)
{
    struct wl_proxy* id;
    id = s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_wm_base,
        2, // XDG_WM_BASE_GET_XDG_SURFACE,
        &xdg_surface_interface,
        s_Wl.proxyGetVersion((struct wl_proxy*)xdg_wm_base),
        0,
        NULL,
        surface);

    return (struct xdg_surface*)id;
}

static inline struct xdg_toplevel*
xdgSurfaceGetToplevel(struct xdg_surface* xdg_surface)
{
    struct wl_proxy* id;
    id = s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_surface,
        1, // XDG_SURFACE_GET_TOPLEVEL,
        &xdg_toplevel_interface,
        s_Wl.proxyGetVersion((struct wl_proxy*)xdg_surface),
        0,
        NULL);

    return (struct xdg_toplevel*)id;
}

static inline void xdgSurfaceAckConfigure(
    struct xdg_surface* xdg_surface,
    uint32_t serial)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_surface,
        4, // XDG_SURFACE_ACK_CONFIGURE
        NULL,
        s_Wl.proxyGetVersion(
            (struct wl_proxy*)xdg_surface),
            0,
            serial);
}

static void wmBaseHandlePing(
    void* data,
    struct xdg_wm_base* base,
    uint32_t serial)
{
    xdgWmBasePong(base, serial);
}

static void xdgSurfaceHandleConfigure(
    void* data,
    struct xdg_surface* surface,
    uint32_t serial)
{
    WindowData* winData = (WindowData*)data;
    xdgSurfaceAckConfigure(surface, serial);

    // push and resolve any pending events
    if (!winData->skipConfigure) {
        if (winData->pushConfigureEvent) {
            if (winData->eglWindow) {
                s_Wl.eglWindowResize(
                    winData->eglWindow,
                    winData->w,
                    winData->h,
                    0,
                    0);

            } else {
                // create a new buffer with the new size
                struct wl_buffer* buffer = nullptr;
                buffer = createShmBuffer(winData->w, winData->h, nullptr, false);
                if (!buffer) {
                    return;
                }

                struct wl_surface* _surface = nullptr;
                _surface = (struct wl_surface*)winData->window;

                wlSurfaceAttach(_surface, buffer, 0, 0);
                wlSurfaceDamageBuffer(_surface, 0, 0, winData->w, winData->h);
                wlSurfaceCommit(_surface);

                // destroy old buffer
                wlBufferDestroy(winData->buffer);
                winData->buffer = buffer;
            }

            // push a window resize event
            if (s_Video.eventDriver) {
                PalEventType type = PAL_EVENT_WINDOW_SIZE;
                PalDispatchMode mode = PAL_DISPATCH_NONE;
                mode = palGetEventDispatchMode(s_Video.eventDriver, type);
                if (mode != PAL_DISPATCH_NONE) {
                    PalEvent event = {0};
                    event.type = type;
                    event.data = palPackUint32(winData->w, winData->h);
                    event.data2 = palPackPointer(winData->window);
                    palPushEvent(s_Video.eventDriver, &event);
                }
            }

            winData->pushConfigureEvent = false;
        }
    }

    // pending state
    if (!winData->skipState) {
        if (!winData->pushStateEvent) {
            return;
        }

        // push a window state event
        // we dont recreate buffers over here
        // since we already create the buffer with the new size
        if (s_Video.eventDriver) {
            PalEventType type = PAL_EVENT_WINDOW_STATE;
            PalDispatchMode mode = PAL_DISPATCH_NONE;
            mode = palGetEventDispatchMode(s_Video.eventDriver, type);
            if (mode != PAL_DISPATCH_NONE) {
                PalEvent event = {0};
                event.type = type;
                event.data = winData->state;
                event.data2 = palPackPointer(winData->window);
                palPushEvent(s_Video.eventDriver, &event);
            }
        }

        winData->pushStateEvent = false;
    }
}

static void xdgToplevelHandleConfigure(
    void* data,
    struct xdg_toplevel* toplevel,
    int32_t width,
    int32_t height,
    struct wl_array* states)
{
    WindowData* winData = (WindowData*)data;
    uint32_t* state;
    bool activated = false;
    wl_array_for_each(state, states) {
        // we need only maximized
        if (*state == 1) { // XDG_TOPLEVEL_STATE_MAXIMIZED
            if (winData->state != PAL_WINDOW_STATE_MAXIMIZED) {
                winData->state = PAL_WINDOW_STATE_MAXIMIZED;
                winData->pushStateEvent = true;
            }

        } else if (*state == 4) { // XDG_TOPLEVEL_STATE_ACTIVATED
            activated = true;
        }
    }

    if (width > 0 && height > 0) {
        if (width != winData->w || height != winData->h) {
            // size change
            winData->pushConfigureEvent = true;
        }

        winData->w = width;
        winData->h = height;
    }

    if (activated && !winData->focused) {
        // focus gained
        winData->focused = true;

    } else if (!activated && winData->focused) {
        // focus lost
        winData->focused = false;

    } else {
        // discard double focus gained and double focus lost
        return;
    }

    if (s_Video.eventDriver) {
        PalEventDriver* driver = s_Video.eventDriver;
        PalDispatchMode mode = PAL_DISPATCH_NONE;
        PalEventType type = PAL_EVENT_WINDOW_FOCUS;
        mode = palGetEventDispatchMode(driver, type);

        if (mode != PAL_DISPATCH_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data = winData->focused;
            event.data2 = palPackPointer(winData->window);
            palPushEvent(driver, &event);
        }
    }
}

static void xdgToplevelHandleClose(
    void* data,
    struct xdg_toplevel* toplevel)
{
    WindowData* winData = (WindowData*)data;
    if (s_Video.eventDriver) {
        PalEventType type = PAL_EVENT_WINDOW_CLOSE;
        PalDispatchMode mode = PAL_DISPATCH_NONE;
        mode = palGetEventDispatchMode(s_Video.eventDriver, type);
        if (mode != PAL_DISPATCH_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data2 = palPackPointer(winData->window);
            palPushEvent(s_Video.eventDriver, &event);
        }
    }
}

static inline int xdgSurfaceAddListener(
    struct xdg_surface* xdg_surface,
    const struct xdg_surface_listener* listener,
    void* data)
{
    return s_Wl.proxyAddListener(
        (struct wl_proxy*)xdg_surface,
        (void (**)(void))listener,
        data);
}

static inline void xdgSurfaceDestroy(struct xdg_surface* xdg_surface)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_surface,
        0, // XDG_SURFACE_DESTROY
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)xdg_surface),
        WL_MARSHAL_FLAG_DESTROY);
}

static inline void xdgToplevelDestroy(struct xdg_toplevel* xdg_toplevel)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_toplevel,
        0, // XDG_TOPLEVEL_DESTROY
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)xdg_toplevel),
        WL_MARSHAL_FLAG_DESTROY);
}

static inline void xdgToplevelSetTitle(
    struct xdg_toplevel* xdg_toplevel,
    const char* title)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_toplevel,
        2, // XDG_TOPLEVEL_SET_TITLE
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)xdg_toplevel),
        0,
        title);
}

static inline void xdgToplevelSetMaximized(struct xdg_toplevel* xdg_toplevel)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_toplevel,
        9, // XDG_TOPLEVEL_SET_MAXIMIZED
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)xdg_toplevel),
        0);
}

static inline void xdgToplevelSetMinimized(struct xdg_toplevel* xdg_toplevel)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_toplevel,
        13, // XDG_TOPLEVEL_SET_MINIMIZED
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)xdg_toplevel),
        0);
}

static inline int xdgToplevelAddListener(
    struct xdg_toplevel* xdg_toplevel,
    const struct xdg_toplevel_listener* listener,
    void* data)
{
    return s_Wl.proxyAddListener(
        (struct wl_proxy*)xdg_toplevel,
        (void (**)(void))listener,
        data);
}

static inline void xdgToplevelSetMinSize(
    struct xdg_toplevel* xdg_toplevel,
    int32_t width,
    int32_t height)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_toplevel,
        8, // XDG_TOPLEVEL_SET_MIN_SIZE
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)xdg_toplevel),
        0,
        width,
        height);
}

static inline void xdgToplevelSetMaxSize(
    struct xdg_toplevel* xdg_toplevel,
    int32_t width,
    int32_t height)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_toplevel,
        7, // XDG_TOPLEVEL_SET_MAX_SIZE
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)xdg_toplevel),
        0,
        width,
        height);
}

static inline void xdgToplevelSetAppId(
    struct xdg_toplevel* xdg_toplevel,
    const char* app_id)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_toplevel,
        3, // XDG_TOPLEVEL_SET_APP_ID
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)xdg_toplevel),
        0,
        app_id);
}

static inline void xdgToplevelUnsetMaximized(struct xdg_toplevel* xdg_toplevel)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_toplevel,
        10, // XDG_TOPLEVEL_UNSET_MAXIMIZED
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)xdg_toplevel),
        0);
}

static const struct wl_interface* xdg_shell_types[26];

static const struct wl_message xdg_wm_base_requests[] = {
    {"destroy", "", xdg_shell_types + 0},
    {"create_positioner", "n", xdg_shell_types + 4},
    {"get_xdg_surface", "no", xdg_shell_types + 5},
    {"pong", "u", xdg_shell_types + 0},
};

static const struct wl_message xdg_wm_base_events[] = {
    {"ping", "u", xdg_shell_types + 0},
};

const struct wl_interface xdg_wm_base_interface = {
    "xdg_wm_base",
    6,
    4,
    xdg_wm_base_requests,
    1,
    xdg_wm_base_events,
};

static const struct wl_message xdg_positioner_requests[] = {
    {"destroy", "", xdg_shell_types + 0},
    {"set_size", "ii", xdg_shell_types + 0},
    {"set_anchor_rect", "iiii", xdg_shell_types + 0},
    {"set_anchor", "u", xdg_shell_types + 0},
    {"set_gravity", "u", xdg_shell_types + 0},
    {"set_constraint_adjustment", "u", xdg_shell_types + 0},
    {"set_offset", "ii", xdg_shell_types + 0},
    {"set_reactive", "3", xdg_shell_types + 0},
    {"set_parent_size", "3ii", xdg_shell_types + 0},
    {"set_parent_configure", "3u", xdg_shell_types + 0},
};

const struct wl_interface xdg_positioner_interface = {
    "xdg_positioner",
    6,
    10,
    xdg_positioner_requests,
    0,
    NULL,
};

static const struct wl_message xdg_surface_requests[] = {
    {"destroy", "", xdg_shell_types + 0},
    {"get_toplevel", "n", xdg_shell_types + 7},
    {"get_popup", "n?oo", xdg_shell_types + 8},
    {"set_window_geometry", "iiii", xdg_shell_types + 0},
    {"ack_configure", "u", xdg_shell_types + 0},
};

static const struct wl_message xdg_surface_events[] = {
    {"configure", "u", xdg_shell_types + 0},
};

const struct wl_interface xdg_surface_interface = {
    "xdg_surface",
    6,
    5,
    xdg_surface_requests,
    1,
    xdg_surface_events,
};

static const struct wl_message xdg_toplevel_requests[] = {
    {"destroy", "", xdg_shell_types + 0},
    {"set_parent", "?o", xdg_shell_types + 11},
    {"set_title", "s", xdg_shell_types + 0},
    {"set_app_id", "s", xdg_shell_types + 0},
    {"show_window_menu", "ouii", xdg_shell_types + 12},
    {"move", "ou", xdg_shell_types + 16},
    {"resize", "ouu", xdg_shell_types + 18},
    {"set_max_size", "ii", xdg_shell_types + 0},
    {"set_min_size", "ii", xdg_shell_types + 0},
    {"set_maximized", "", xdg_shell_types + 0},
    {"unset_maximized", "", xdg_shell_types + 0},
    {"set_fullscreen", "?o", xdg_shell_types + 21},
    {"unset_fullscreen", "", xdg_shell_types + 0},
    {"set_minimized", "", xdg_shell_types + 0},
};

static const struct wl_message xdg_toplevel_events[] = {
    {"configure", "iia", xdg_shell_types + 0},
    {"close", "", xdg_shell_types + 0},
    {"configure_bounds", "4ii", xdg_shell_types + 0},
    {"wm_capabilities", "5a", xdg_shell_types + 0},
};

const struct wl_interface xdg_toplevel_interface = {
    "xdg_toplevel",
    6,
    14,
    xdg_toplevel_requests,
    4,
    xdg_toplevel_events,
};

static const struct wl_message xdg_popup_requests[] = {
    {"destroy", "", xdg_shell_types + 0},
    {"grab", "ou", xdg_shell_types + 22},
    {"reposition", "3ou", xdg_shell_types + 24},
};

static const struct wl_message xdg_popup_events[] = {
    {"configure", "iiii", xdg_shell_types + 0},
    {"popup_done", "", xdg_shell_types + 0},
    {"repositioned", "3u", xdg_shell_types + 0},
};

const struct wl_interface xdg_popup_interface = {
    "xdg_popup",
    6,
    3,
    xdg_popup_requests,
    3,
    xdg_popup_events,
};

static void setupXdgShellProtocol()
{
    xdg_shell_types[0] = NULL;
    xdg_shell_types[1] = NULL;
    xdg_shell_types[2] = NULL;
    xdg_shell_types[3] = NULL;
    xdg_shell_types[4] = &xdg_positioner_interface;
    xdg_shell_types[5] = &xdg_surface_interface;
    xdg_shell_types[6] = s_Wl.surfaceInterface;
    xdg_shell_types[7] = &xdg_toplevel_interface;
    xdg_shell_types[8] = &xdg_popup_interface;
    xdg_shell_types[9] = &xdg_surface_interface;
    xdg_shell_types[10] = &xdg_positioner_interface;
    xdg_shell_types[11] = &xdg_toplevel_interface;
    xdg_shell_types[12] = s_Wl.seatInterface;
    xdg_shell_types[13] = NULL;
    xdg_shell_types[14] = NULL;
    xdg_shell_types[15] = NULL;
    xdg_shell_types[16] = s_Wl.seatInterface;
    xdg_shell_types[17] = NULL;
    xdg_shell_types[18] = s_Wl.seatInterface;
    xdg_shell_types[19] = NULL;
    xdg_shell_types[20] = NULL;
    xdg_shell_types[21] = s_Wl.outputInterface;
    xdg_shell_types[22] = s_Wl.seatInterface;
    xdg_shell_types[23] = NULL;
    xdg_shell_types[24] = &xdg_positioner_interface;
    xdg_shell_types[25] = NULL;
}

static const struct xdg_wm_base_listener wmBaseListener = {
    .ping = wmBaseHandlePing};

static const struct xdg_surface_listener xdgSurfaceListener = {
    .configure = xdgSurfaceHandleConfigure};

static const struct xdg_toplevel_listener xdgToplevelListener = {
    .configure = xdgToplevelHandleConfigure,
    .close = xdgToplevelHandleClose,
    .configure_bounds = nullptr,
    .wm_capabilities = nullptr};

#endif // PAL_HAS_WAYLAND
#pragma endregion

#pragma region Zxdg-Decoraion_Manager-V1
#if PAL_HAS_WAYLAND

struct xdg_toplevel;
struct zxdg_decoration_manager_v1;
struct zxdg_toplevel_decoration_v1;

struct zxdg_toplevel_decoration_v1_listener {
    void (*configure)(
        void*,
        struct zxdg_toplevel_decoration_v1*,
        uint32_t);
};

const struct wl_interface zxdg_decoration_manager_v1_interface;
const struct wl_interface zxdg_toplevel_decoration_v1_interface;

static inline void zxdgDecorationManagerV1Destroy(
    struct zxdg_decoration_manager_v1* zxdg_decoration_manager_v1)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)zxdg_decoration_manager_v1,
        0, // ZXDG_DECORATION_MANAGER_V1_DESTROY
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)zxdg_decoration_manager_v1),
        WL_MARSHAL_FLAG_DESTROY);
}

static inline struct zxdg_toplevel_decoration_v1* zxdgGetToplevelDecoration(
    struct zxdg_decoration_manager_v1* zxdg_decoration_manager_v1,
    struct xdg_toplevel* toplevel)
{
    struct wl_proxy* id;
    id = s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)zxdg_decoration_manager_v1,
        1, // ZXDG_DECORATION_MANAGER_V1_GET_TOPLEVEL_DECORATION,
        &zxdg_toplevel_decoration_v1_interface,
        s_Wl.proxyGetVersion((struct wl_proxy*)zxdg_decoration_manager_v1),
        0,
        NULL,
        toplevel);

    return (struct zxdg_toplevel_decoration_v1*)id;
}

static inline int zxdgToplevelDecorationV1AddListener(
    struct zxdg_toplevel_decoration_v1* zxdg_toplevel_decoration_v1,
    const struct zxdg_toplevel_decoration_v1_listener* listener,
    void* data)
{
    return s_Wl.proxyAddListener(
        (struct wl_proxy*)zxdg_toplevel_decoration_v1,
        (void (**)(void))listener,
        data);
}

static inline void zxdgToplevelDecorationV1Destroy(
    struct zxdg_toplevel_decoration_v1* zxdg_toplevel_decoration_v1)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)zxdg_toplevel_decoration_v1,
        0, // ZXDG_TOPLEVEL_DECORATION_V1_DESTROY
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)zxdg_toplevel_decoration_v1),
        WL_MARSHAL_FLAG_DESTROY);
}

static inline void zxdgToplevelDecorationV1SetMode(
    struct zxdg_toplevel_decoration_v1* zxdg_toplevel_decoration_v1,
    uint32_t mode)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)zxdg_toplevel_decoration_v1,
        1, // ZXDG_TOPLEVEL_DECORATION_V1_SET_MODE,
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)zxdg_toplevel_decoration_v1),
        0,
        mode);
}

static const struct wl_interface* xdg_decoration_unstable_v1_types[] = {
    NULL,
    &zxdg_toplevel_decoration_v1_interface,
    &xdg_toplevel_interface,
};

static const struct wl_message zxdg_decoration_manager_v1_requests[] = {
    {"destroy", "", xdg_decoration_unstable_v1_types + 0},
    {"get_toplevel_decoration", "no", xdg_decoration_unstable_v1_types + 1},
};

const struct wl_interface zxdg_decoration_manager_v1_interface = {
    "zxdg_decoration_manager_v1",
    1,
    2,
    zxdg_decoration_manager_v1_requests,
    0,
    NULL,
};

static const struct wl_message zxdg_toplevel_decoration_v1_requests[] = {
    {"destroy", "", xdg_decoration_unstable_v1_types + 0},
    {"set_mode", "u", xdg_decoration_unstable_v1_types + 0},
    {"unset_mode", "", xdg_decoration_unstable_v1_types + 0},
};

static const struct wl_message zxdg_toplevel_decoration_v1_events[] = {
    {"configure", "u", xdg_decoration_unstable_v1_types + 0},
};

const struct wl_interface zxdg_toplevel_decoration_v1_interface = {
    "zxdg_toplevel_decoration_v1",
    1,
    3,
    zxdg_toplevel_decoration_v1_requests,
    1,
    zxdg_toplevel_decoration_v1_events,
};

void zxdgDecorationHandleConfigure(
    void* data,
    struct zxdg_toplevel_decoration_v1* dec,
    uint32_t mode)
{
    if (s_Video.eventDriver) {
        PalEventDriver* driver = s_Video.eventDriver;
        PalDispatchMode dispatchMode = PAL_DISPATCH_NONE;
        PalEventType type = PAL_EVENT_WINDOW_DECORATION_MODE;
        dispatchMode = palGetEventDispatchMode(driver, type);

        if (dispatchMode != PAL_DISPATCH_NONE) {
            PalDecorationMode decorMode = PAL_DECORATION_MODE_SERVER_SIDE;
            if (mode == 0 || mode == 1) {
                // client side decoration
                decorMode = PAL_DECORATION_MODE_CLIENT_SIDE;
            }

            PalEvent event = {0};
            event.type = type;
            event.data = decorMode;
            event.data2 = palPackPointer(data);
            palPushEvent(driver, &event);
        }
    }
}

static struct zxdg_toplevel_decoration_v1_listener decorationListener = {
    .configure = zxdgDecorationHandleConfigure
};

#endif // PAL_HAS_WAYLAND
#pragma endregion

// ==================================================
// Internal API
// ==================================================

static int compareModes(
    const void* a,
    const void* b)
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

static WindowData* findWindowData(PalWindow* window)
{
    for (int i = 0; i < s_Video.maxWindowData; ++i) {
        if (s_Video.windowData[i].used &&
            s_Video.windowData[i].window == window) {
            return &s_Video.windowData[i];
        }
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
    return nullptr;
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

static void createScancodeTable()
{
    // Letters
    s_Keyboard.scancodes[0x01E] = PAL_SCANCODE_A;
    s_Keyboard.scancodes[0x030] = PAL_SCANCODE_B;
    s_Keyboard.scancodes[0x02E] = PAL_SCANCODE_C;
    s_Keyboard.scancodes[0x020] = PAL_SCANCODE_D;
    s_Keyboard.scancodes[0x012] = PAL_SCANCODE_E;
    s_Keyboard.scancodes[0x021] = PAL_SCANCODE_F;
    s_Keyboard.scancodes[0x022] = PAL_SCANCODE_G;
    s_Keyboard.scancodes[0x023] = PAL_SCANCODE_H;
    s_Keyboard.scancodes[0x017] = PAL_SCANCODE_I;
    s_Keyboard.scancodes[0x024] = PAL_SCANCODE_J;
    s_Keyboard.scancodes[0x025] = PAL_SCANCODE_K;
    s_Keyboard.scancodes[0x026] = PAL_SCANCODE_L;
    s_Keyboard.scancodes[0x032] = PAL_SCANCODE_M;
    s_Keyboard.scancodes[0x031] = PAL_SCANCODE_N;
    s_Keyboard.scancodes[0x018] = PAL_SCANCODE_O;
    s_Keyboard.scancodes[0x019] = PAL_SCANCODE_P;
    s_Keyboard.scancodes[0x010] = PAL_SCANCODE_Q;
    s_Keyboard.scancodes[0x013] = PAL_SCANCODE_R;
    s_Keyboard.scancodes[0x01F] = PAL_SCANCODE_S;
    s_Keyboard.scancodes[0x014] = PAL_SCANCODE_T;
    s_Keyboard.scancodes[0x016] = PAL_SCANCODE_U;
    s_Keyboard.scancodes[0x02F] = PAL_SCANCODE_V;
    s_Keyboard.scancodes[0x011] = PAL_SCANCODE_W;
    s_Keyboard.scancodes[0x02D] = PAL_SCANCODE_X;
    s_Keyboard.scancodes[0x015] = PAL_SCANCODE_Y;
    s_Keyboard.scancodes[0x02C] = PAL_SCANCODE_Z;

    // Numbers (top row)
    s_Keyboard.scancodes[0x00B] = PAL_SCANCODE_0;
    s_Keyboard.scancodes[0x002] = PAL_SCANCODE_1;
    s_Keyboard.scancodes[0x003] = PAL_SCANCODE_2;
    s_Keyboard.scancodes[0x004] = PAL_SCANCODE_3;
    s_Keyboard.scancodes[0x005] = PAL_SCANCODE_4;
    s_Keyboard.scancodes[0x006] = PAL_SCANCODE_5;
    s_Keyboard.scancodes[0x007] = PAL_SCANCODE_6;
    s_Keyboard.scancodes[0x008] = PAL_SCANCODE_7;
    s_Keyboard.scancodes[0x009] = PAL_SCANCODE_8;
    s_Keyboard.scancodes[0x00A] = PAL_SCANCODE_9;

    // Function
    s_Keyboard.scancodes[0x03B] = PAL_SCANCODE_F1;
    s_Keyboard.scancodes[0x03C] = PAL_SCANCODE_F2;
    s_Keyboard.scancodes[0x03D] = PAL_SCANCODE_F3;
    s_Keyboard.scancodes[0x03E] = PAL_SCANCODE_F4;
    s_Keyboard.scancodes[0x03F] = PAL_SCANCODE_F5;
    s_Keyboard.scancodes[0x040] = PAL_SCANCODE_F6;
    s_Keyboard.scancodes[0x041] = PAL_SCANCODE_F7;
    s_Keyboard.scancodes[0x042] = PAL_SCANCODE_F8;
    s_Keyboard.scancodes[0x043] = PAL_SCANCODE_F9;
    s_Keyboard.scancodes[0x044] = PAL_SCANCODE_F10;
    s_Keyboard.scancodes[0x057] = PAL_SCANCODE_F11;
    s_Keyboard.scancodes[0x058] = PAL_SCANCODE_F12;

    // Control
    s_Keyboard.scancodes[0x001] = PAL_SCANCODE_ESCAPE;
    s_Keyboard.scancodes[0x01C] = PAL_SCANCODE_ENTER;
    s_Keyboard.scancodes[0x00F] = PAL_SCANCODE_TAB;
    s_Keyboard.scancodes[0x00E] = PAL_SCANCODE_BACKSPACE;
    s_Keyboard.scancodes[0x039] = PAL_SCANCODE_SPACE;
    s_Keyboard.scancodes[0x03A] = PAL_SCANCODE_CAPSLOCK;
    s_Keyboard.scancodes[0x045] = PAL_SCANCODE_NUMLOCK;
    s_Keyboard.scancodes[0x046] = PAL_SCANCODE_SCROLLLOCK;
    s_Keyboard.scancodes[0x02A] = PAL_SCANCODE_LSHIFT;
    s_Keyboard.scancodes[0x036] = PAL_SCANCODE_RSHIFT;
    s_Keyboard.scancodes[0x01D] = PAL_SCANCODE_LCTRL;
    s_Keyboard.scancodes[0x061] = PAL_SCANCODE_RCTRL;
    s_Keyboard.scancodes[0x038] = PAL_SCANCODE_LALT;
    s_Keyboard.scancodes[0x064] = PAL_SCANCODE_RALT;

    // Arrows
    s_Keyboard.scancodes[0x069] = PAL_SCANCODE_LEFT;
    s_Keyboard.scancodes[0x06A] = PAL_SCANCODE_RIGHT;
    s_Keyboard.scancodes[0x067] = PAL_SCANCODE_UP;
    s_Keyboard.scancodes[0x06C] = PAL_SCANCODE_DOWN;

    // Navigation
    s_Keyboard.scancodes[0x06E] = PAL_SCANCODE_INSERT;
    s_Keyboard.scancodes[0x06F] = PAL_SCANCODE_DELETE;
    s_Keyboard.scancodes[0x066] = PAL_SCANCODE_HOME;
    s_Keyboard.scancodes[0x067] = PAL_SCANCODE_END;
    s_Keyboard.scancodes[0x068] = PAL_SCANCODE_PAGEUP;
    s_Keyboard.scancodes[0x06D] = PAL_SCANCODE_PAGEDOWN;

    // Keypad
    s_Keyboard.scancodes[0x052] = PAL_SCANCODE_KP_0;
    s_Keyboard.scancodes[0x04F] = PAL_SCANCODE_KP_1;
    s_Keyboard.scancodes[0x050] = PAL_SCANCODE_KP_2;
    s_Keyboard.scancodes[0x051] = PAL_SCANCODE_KP_3;
    s_Keyboard.scancodes[0x04B] = PAL_SCANCODE_KP_4;
    s_Keyboard.scancodes[0x04C] = PAL_SCANCODE_KP_5;
    s_Keyboard.scancodes[0x04D] = PAL_SCANCODE_KP_6;
    s_Keyboard.scancodes[0x047] = PAL_SCANCODE_KP_7;
    s_Keyboard.scancodes[0x048] = PAL_SCANCODE_KP_8;
    s_Keyboard.scancodes[0x049] = PAL_SCANCODE_KP_9;
    s_Keyboard.scancodes[0x060] = PAL_SCANCODE_KP_ENTER;
    s_Keyboard.scancodes[0x04E] = PAL_SCANCODE_KP_ADD;
    s_Keyboard.scancodes[0x04A] = PAL_SCANCODE_KP_SUBTRACT;
    s_Keyboard.scancodes[0x037] = PAL_SCANCODE_KP_MULTIPLY;
    s_Keyboard.scancodes[0x062] = PAL_SCANCODE_KP_DIVIDE;
    s_Keyboard.scancodes[0x053] = PAL_SCANCODE_KP_DECIMAL;

    // Misc
    s_Keyboard.scancodes[0x063] = PAL_SCANCODE_PRINTSCREEN;
    s_Keyboard.scancodes[0x066] = PAL_SCANCODE_PAUSE;
    s_Keyboard.scancodes[0x07F] = PAL_SCANCODE_MENU;
    s_Keyboard.scancodes[0x028] = PAL_SCANCODE_APOSTROPHE;
    s_Keyboard.scancodes[0x02B] = PAL_SCANCODE_BACKSLASH;
    s_Keyboard.scancodes[0x033] = PAL_SCANCODE_COMMA;
    s_Keyboard.scancodes[0x00D] = PAL_SCANCODE_EQUAL;
    s_Keyboard.scancodes[0x029] = PAL_SCANCODE_GRAVEACCENT;
    s_Keyboard.scancodes[0x00C] = PAL_SCANCODE_SUBTRACT;
    s_Keyboard.scancodes[0x034] = PAL_SCANCODE_PERIOD;
    s_Keyboard.scancodes[0x027] = PAL_SCANCODE_SEMICOLON;
    s_Keyboard.scancodes[0x035] = PAL_SCANCODE_SLASH;
    s_Keyboard.scancodes[0x01A] = PAL_SCANCODE_LBRACKET;
    s_Keyboard.scancodes[0x01B] = PAL_SCANCODE_RBRACKET;
    s_Keyboard.scancodes[0x07D] = PAL_SCANCODE_LSUPER;
    s_Keyboard.scancodes[0x07E] = PAL_SCANCODE_RSUPER;
}

// ==================================================
// X11 API
// ==================================================

#pragma region X11 API
#if PAL_HAS_X11

static PalResult glxBackend(const int index)
{
    // user choose GLX FBConfig backend
    if (!s_X11.glxHandle) {
        // Rare
        return PAL_RESULT_PLATFORM_FAILURE;
    }
    // clang-format off

    int count = 0;
    GLXFBConfig* configs = s_X11.glxGetFBConfigs(
        s_X11.display, 
        s_X11.screen, 
        &count);

    GLXFBConfig fbConfig = configs[index];
    if (!fbConfig) {
        return PAL_RESULT_INVALID_GL_FBCONFIG;
    }

    // get a matching visual
    XVisualInfo* visualInfo = s_X11.glxGetVisualFromFBConfig(
        s_X11.display, 
        fbConfig);

    if (!visualInfo) {
        return PAL_RESULT_INVALID_GL_FBCONFIG;
    }

    s_X11.visualInfo = visualInfo;
    return PAL_RESULT_SUCCESS;
}

static PalResult eglXBackend(int index)
{    
    // user choose EGL FBConfig backend
    if (!s_Egl.handle) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    EGLDisplay display = EGL_NO_DISPLAY;
    display = s_Egl.eglGetDisplay((EGLNativeDisplayType)s_X11.display);
    if (display == EGL_NO_DISPLAY) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    EGLint numConfigs = 0;
    if (!s_Egl.eglGetConfigs(display, nullptr, 0, &numConfigs)) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    EGLint configSize = sizeof(EGLConfig) * numConfigs;
    EGLConfig* eglConfigs = palAllocate(s_Video.allocator, configSize, 0);
    if (!eglConfigs) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    s_Egl.eglGetConfigs(display, eglConfigs, numConfigs, &numConfigs);
    EGLConfig config = eglConfigs[index];

    // we get a visual info from the config
    EGLint visualID;
    s_Egl.eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &visualID);
    if (visualID == 0) {
        return PAL_RESULT_INVALID_GL_FBCONFIG;
    }

    int numVisuals = 0;
    XVisualInfo tmp;
    tmp.visualid = visualID;

    // clang-format off
    // get a matching visual info
    XVisualInfo* visualInfo = s_X11.getVisualInfo(
        s_X11.display, 
        VisualIDMask, 
        &tmp, 
        &numVisuals);
    // clang-format on

    if (!visualInfo) {
        return PAL_RESULT_INVALID_GL_FBCONFIG;
    }
    
    s_X11.visualInfo = visualInfo;
    palFree(s_Video.allocator, eglConfigs);
    return PAL_RESULT_SUCCESS;
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
        if (info->width == mode->width && info->height == mode->height &&
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
    X_INTERN(_NET_WM_STATE_HIDDEN);
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
    PalVideoFeatures64 features64 = 0;
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

        if (supportedAtoms[i] == s_X11Atoms._NET_WM_WINDOW_TYPE_SPLASH) {
            features |= PAL_VIDEO_FEATURE_BORDERLESS_WINDOW;
        }

        if (supportedAtoms[i] == s_X11Atoms._NET_WM_NAME) {
            s_X11Atoms.unicodeTitle = true;
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
    features |= PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY;

    // extended features
    // old features
    features64 |= PAL_VIDEO_FEATURE64_MULTI_MONITORS;
    features64 |= PAL_VIDEO_FEATURE64_MONITOR_GET_ORIENTATION;
    features64 |= PAL_VIDEO_FEATURE64_MONITOR_SET_MODE;
    features64 |= PAL_VIDEO_FEATURE64_MONITOR_GET_MODE;
    features64 |= PAL_VIDEO_FEATURE64_WINDOW_SET_SIZE;
    features64 |= PAL_VIDEO_FEATURE64_WINDOW_GET_SIZE;
    features64 |= PAL_VIDEO_FEATURE64_WINDOW_SET_VISIBILITY;
    features64 |= PAL_VIDEO_FEATURE64_WINDOW_GET_VISIBILITY;

    features64 |= PAL_VIDEO_FEATURE64_CLIP_CURSOR;
    features64 |= PAL_VIDEO_FEATURE64_WINDOW_SET_INPUT_FOCUS;
    features64 |= PAL_VIDEO_FEATURE64_WINDOW_GET_INPUT_FOCUS;
    features64 |= PAL_VIDEO_FEATURE64_CURSOR_SET_POS;
    features64 |= PAL_VIDEO_FEATURE64_CURSOR_GET_POS;
    features64 |= PAL_VIDEO_FEATURE64_WINDOW_SET_TITLE;
    features64 |= PAL_VIDEO_FEATURE64_WINDOW_GET_TITLE;
    features64 |= PAL_VIDEO_FEATURE64_WINDOW_FLASH_TRAY;

    features64 |= PAL_VIDEO_FEATURE64_WINDOW_SET_ICON;
    features64 |= PAL_VIDEO_FEATURE64_TOPMOST_WINDOW;
    features64 |= PAL_VIDEO_FEATURE64_DECORATED_WINDOW;
    features64 |= PAL_VIDEO_FEATURE64_MONITOR_GET_PRIMARY;
    features64 |= PAL_VIDEO_FEATURE64_FOREIGN_WINDOWS;
    features64 |= PAL_VIDEO_FEATURE64_WINDOW_SET_CURSOR;

    s_Video.features = features;
    s_Video.features64 = features64;
    s_X11.free(supportedAtoms);
}

static int xErrorHandler(
    Display*,
    XErrorEvent* e)
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

    s_X11.free(atoms);
    return state;
}

static void xCacheMonitors()
{
    resetMonitorData();

    XRRScreenResources* resources = nullptr;
    resources = s_X11.getScreenResources(s_X11.display, s_X11.root);

    for (int i = 0; i < resources->noutput; ++i) {
        RROutput output = resources->outputs[i];
        // clang-format off
        XRROutputInfo* info = s_X11.getOutputInfo(s_X11.display, resources, output);
        // clang-format on

        if (info->connection == RR_Connected && info->crtc != None) {
            // get monitor data and update info
            PalMonitor* monitor = TO_PAL_HANDLE(PalMonitor, output);
            MonitorData* data = nullptr;
            data = getFreeMonitorData();
            if (!data) {
                return;
            }

            data->monitor = monitor;

            // clang-format off
            XRRCrtcInfo* crtc = s_X11.getCrtcInfo(s_X11.display, resources, info->crtc);
            // clang-format on

            // get DPI
            float raw = crtc->width / 1920.0f;
            float steps[] = {1.0f, 1.2f, 1.5f, 1.75, 2.0f};
            float closest = steps[0];
            float minDiff = fabsf(raw - steps[0]);

            for (int i = 1; i < sizeof(steps) / sizeof(steps[0]); i++) {
                float diff = fabsf(raw - steps[i]);
                if (diff < minDiff) {
                    minDiff = diff;
                    closest = steps[i];
                }
            }

            data->dpi = (Uint32)(closest * 96.0f);
            data->w = crtc->width;
            data->h = crtc->height;
            data->x = crtc->x;
            data->y = crtc->y;

            s_X11.freeCrtcInfo(crtc);
            s_X11.monitorCount++;
        }

        s_X11.freeOutputInfo(info);
    }

    s_X11.freeScreenResources(resources);
}

static int xGetWindowMonitorDPI(WindowData* data)
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
        if (winX >= info->x && winX < info->x + info->w && winY >= info->y &&
            winY < info->y + info->h) {
            // found monitor
            return info->dpi;
        }
    }
}

static void xSendWMEvent(
    Window window,
    Atom type,
    long a,
    long b,
    long c,
    long d,
    bool add)
{
    XEvent e = {0};
    e.xclient.type = ClientMessage;
    e.xclient.send_event = True;
    e.xclient.window = window;
    e.xclient.message_type = type;
    e.xclient.format = 32;
    if (add) {
        e.xclient.data.l[0] = 1; // _NET_WM_STATE_ADD
    } else {
        e.xclient.data.l[0] = 0; // _NET_WM_STATE_REMOVE
    }

    e.xclient.data.l[1] = a;
    e.xclient.data.l[2] = b;
    e.xclient.data.l[3] = c;
    e.xclient.data.l[4] = d;

    s_X11.sendEvent(
        s_X11.display,
        s_X11.root,
        False,
        SubstructureNotifyMask | SubstructureRedirectMask,
        &e);
}

static void xCreateKeycodeTable()
{
    // Tis is for only printable and text input keys

    // Letters
    s_Keyboard.keycodes[XK_a] = PAL_KEYCODE_A;
    s_Keyboard.keycodes[XK_b] = PAL_KEYCODE_B;
    s_Keyboard.keycodes[XK_c] = PAL_KEYCODE_C;
    s_Keyboard.keycodes[XK_d] = PAL_KEYCODE_D;
    s_Keyboard.keycodes[XK_e] = PAL_KEYCODE_E;
    s_Keyboard.keycodes[XK_f] = PAL_KEYCODE_F;
    s_Keyboard.keycodes[XK_g] = PAL_KEYCODE_G;
    s_Keyboard.keycodes[XK_h] = PAL_KEYCODE_H;
    s_Keyboard.keycodes[XK_i] = PAL_KEYCODE_I;
    s_Keyboard.keycodes[XK_j] = PAL_KEYCODE_J;
    s_Keyboard.keycodes[XK_k] = PAL_KEYCODE_K;
    s_Keyboard.keycodes[XK_l] = PAL_KEYCODE_L;
    s_Keyboard.keycodes[XK_m] = PAL_KEYCODE_M;
    s_Keyboard.keycodes[XK_n] = PAL_KEYCODE_N;
    s_Keyboard.keycodes[XK_o] = PAL_KEYCODE_O;
    s_Keyboard.keycodes[XK_p] = PAL_KEYCODE_P;
    s_Keyboard.keycodes[XK_q] = PAL_KEYCODE_Q;
    s_Keyboard.keycodes[XK_r] = PAL_KEYCODE_R;
    s_Keyboard.keycodes[XK_s] = PAL_KEYCODE_S;
    s_Keyboard.keycodes[XK_t] = PAL_KEYCODE_T;
    s_Keyboard.keycodes[XK_u] = PAL_KEYCODE_U;
    s_Keyboard.keycodes[XK_v] = PAL_KEYCODE_V;
    s_Keyboard.keycodes[XK_w] = PAL_KEYCODE_W;
    s_Keyboard.keycodes[XK_x] = PAL_KEYCODE_X;
    s_Keyboard.keycodes[XK_y] = PAL_KEYCODE_Y;
    s_Keyboard.keycodes[XK_z] = PAL_KEYCODE_Z;

    // Control
    s_Keyboard.keycodes[XK_space] = PAL_KEYCODE_SPACE;

    // Misc
    s_Keyboard.keycodes[XK_apostrophe] = PAL_KEYCODE_APOSTROPHE;
    s_Keyboard.keycodes[XK_backslash] = PAL_KEYCODE_BACKSLASH;
    s_Keyboard.keycodes[XK_comma] = PAL_KEYCODE_COMMA;
    s_Keyboard.keycodes[XK_equal] = PAL_KEYCODE_EQUAL;
    s_Keyboard.keycodes[XK_grave] = PAL_KEYCODE_GRAVEACCENT;
    s_Keyboard.keycodes[XK_minus] = PAL_KEYCODE_SUBTRACT;
    s_Keyboard.keycodes[XK_period] = PAL_KEYCODE_PERIOD;
    s_Keyboard.keycodes[XK_semicolon] = PAL_KEYCODE_SEMICOLON;
    s_Keyboard.keycodes[XK_slash] = PAL_KEYCODE_SLASH;
    s_Keyboard.keycodes[XK_bracketleft] = PAL_KEYCODE_LBRACKET;
    s_Keyboard.keycodes[XK_bracketright] = PAL_KEYCODE_RBRACKET;
}

static PalResult xInitVideo()
{
    // load X11 library
    s_X11.handle = dlopen("libX11.so", RTLD_LAZY);
    if (!s_X11.handle) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // libXCursor is needed
    s_X11.libCursor = dlopen("libXcursor.so", RTLD_LAZY);
    if (!s_X11.libCursor) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // Xrandr is needed
    s_X11.xrandr = dlopen("libXrandr.so.2", RTLD_LAZY);
    if (!s_X11.xrandr) {
        s_X11.xrandr = dlopen("libXrandr.so", RTLD_LAZY);
    }

    // clang-format off

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

    s_X11.getWMNormalHints = (XGetWMNormalHintsFn)dlsym(
        s_X11.handle, 
        "XGetWMNormalHints");

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

    s_X11.getVisualInfo = (XGetVisualInfoFn)dlsym(
        s_X11.handle, 
        "XGetVisualInfo");

    s_X11.createFontCursor = (XCreateFontCursorFn)dlsym(
        s_X11.handle, 
        "XCreateFontCursor");
    
    s_X11.freePixmap = (XFreePixmapFn)dlsym(
        s_X11.handle, 
        "XFreePixmap");

    s_X11.setWMHints = (XSetWMHintsFn)dlsym(
        s_X11.handle, 
        "XSetWMHints");

    s_X11.grabPointer = (XGrabPointerFn)dlsym(
        s_X11.handle, 
        "XGrabPointer");

    s_X11.createPixmapCursor = (XCreatePixmapCursorFn)dlsym(
        s_X11.handle, 
        "XCreatePixmapCursor");

    s_X11.warpPointer = (XWarpPointerFn)dlsym(
        s_X11.handle, 
        "XWarpPointer");

    s_X11.getWMName = (XGetWMNameFn)dlsym(
        s_X11.handle, 
        "XGetWMName");

    s_X11.queryPointer = (XQueryPointerFn)dlsym(
        s_X11.handle, 
        "XQueryPointer");

    s_X11.ungrabPointer = (XUngrabPointerFn)dlsym(
        s_X11.handle, 
        "XUngrabPointer");

    s_X11.allocWMHints = (XAllocWMHintsFn)dlsym(
        s_X11.handle, 
        "XAllocWMHints");

    s_X11.mapRaised = (XMapRaisedFn)dlsym(
        s_X11.handle, 
        "XMapRaised");

    s_X11.undefineCursor = (XUndefineCursorFn)dlsym(
        s_X11.handle, 
        "XUndefineCursor");

    s_X11.defineCursor = (XDefineCursorFn)dlsym(
        s_X11.handle, 
        "XDefineCursor");

    s_X11.freeCursor = (XFreeCursorFn)dlsym(
        s_X11.handle, 
        "XFreeCursor");

    s_X11.getWMHints = (XGetWMHintsFn)dlsym(
        s_X11.handle, 
        "XGetWMHints");

    s_X11.createPixmap = (XCreatePixmapFn)dlsym(
        s_X11.handle, 
        "XCreatePixmap");

    s_X11.setInputFocus = (XSetInputFocusFn)dlsym(
        s_X11.handle, 
        "XSetInputFocus");

    s_X11.getInputFocus = (XGetInputFocusFn)dlsym(
        s_X11.handle, 
        "XGetInputFocus");

    s_X11.selectInput = (XSelectInputFn)dlsym(
        s_X11.handle, 
        "XSelectInput");

    // libXcursor
    s_X11.cursorImageLoadCursor = (XcursorImageLoadCursorFn)dlsym(
        s_X11.libCursor, 
        "XcursorImageLoadCursor");

    s_X11.cursorImageCreate = (XcursorImageCreateFn)dlsym(
        s_X11.libCursor, 
        "XcursorImageCreate");

    s_X11.cursorImageDestroy = (XcursorImageDestroyFn)dlsym(
        s_X11.libCursor, 
        "XcursorImageDestroy");

    s_X11.lookupKeysym = (XLookupKeysymFn)dlsym(
        s_X11.libCursor, 
        "XLookupKeysym");

    s_X11.setDetectableAutoRepeat = (XkbSetDetectableAutoRepeatFn)dlsym(
        s_X11.handle, 
        "XkbSetDetectableAutoRepeat");

    s_X11.setLocaleModifiers = (XSetLocaleModifiersFn)dlsym(
        s_X11.handle, 
        "XSetLocaleModifiers");

    s_X11.openIM = (XOpenIMFn)dlsym(
        s_X11.handle, 
        "XOpenIM");

    s_X11.closeIM = (XCloseIMFn)dlsym(
        s_X11.handle, 
        "XCloseIM");

    s_X11.createIC = (XCreateICFn)dlsym(
        s_X11.handle, 
        "XCreateIC");

    s_X11.destroyIC = (XDestroyICFn)dlsym(
        s_X11.handle, 
        "XDestroyIC");

    s_X11.utf8LookupString = (Xutf8LookupStringFn)dlsym(
        s_X11.handle, 
        "Xutf8LookupString");

    // X11 server
    if (s_Video.platformInstance) {
        s_X11.display = (Display*)s_Video.platformInstance;

    } else {
        s_X11.display = s_X11.openDisplay(nullptr);
        s_Video.platformInstance = nullptr;
    }

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

    s_X11.dataID = (XContext)s_X11.uniqueContext();
    resetMonitorData();

    s_X11.monitorCount = 0;
    xCacheMonitors();

    // since X11 supports both EGL and GLX
    // we try to load them and resolve the needed functions

    // we load GLX
    s_X11.glxHandle = dlopen("libGL.so.1", RTLD_LAZY);
    if (s_X11.glxHandle) {

        GLXGetProcAddressFn load = nullptr;
        load = (GLXGetProcAddressFn)dlsym(
            s_X11.glxHandle,
            "glXGetProcAddress");

        s_X11.glxGetFBConfigs = (GLXGetFBConfigsFn)load(
            "glXGetFBConfigs");

        s_X11.glxGetFBConfigAttrib = (GLXGetFBConfigAttribFn)load(
            "glXGetFBConfigAttrib");

        s_X11.glxGetVisualFromFBConfig = (GLXGetVisualFromFBConfigFn)load(
            "glXGetVisualFromFBConfig");
    }

    xCreateKeycodeTable();

    // disable auto key repeats
    int supported;
    s_X11.setDetectableAutoRepeat(s_X11.display, True, &supported);
    if (!supported) {
        // FIXME: fallback to manual key repeat detection
    }

    // create an input method
    s_X11.setLocaleModifiers("");
    s_X11.im = s_X11.openIM(s_X11.display, nullptr, nullptr, nullptr);
    if (s_X11.im == None) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    s_Video.display = (void*)s_X11.display;
    return PAL_RESULT_SUCCESS;
}

static void xShutdownVideo()
{
    s_X11.closeIM(s_X11.im);
    if (!s_Video.platformInstance) {
        // opened by PAL
        s_X11.closeDisplay(s_X11.display);
    }

    dlclose(s_X11.handle);
    dlclose(s_X11.xrandr);
    dlclose(s_X11.libCursor);

    if (s_X11.glxHandle) {
        dlclose(s_X11.glxHandle);
    }
    memset(&s_X11, 0, sizeof(X11));
    memset(&s_X11Atoms, 0, sizeof(X11Atoms));
}

PalResult xSetFBConfig(
    const int index,
    PalFBConfigBackend backend)
{
    if (backend == PAL_CONFIG_BACKEND_GLX) {
        return glxBackend(index);

    } else if (backend == PAL_CONFIG_BACKEND_EGL || 
              backend == PAL_CONFIG_BACKEND_PAL_OPENGL) {
        return eglXBackend(index);

    } else {
        return PAL_RESULT_INVALID_FBCONFIG_BACKEND;
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
                break;
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
                    break;
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

                    // attach windows sometimes bypass
                    // skipConfgure an still send an initial move event
                    if (data->isAttached) {
                        if (data->skipIfAttached) {
                            data->skipIfAttached = false;
                            break;
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
                        int monitorDPI = xGetWindowMonitorDPI(data);
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
                break;
            }

            case FocusIn: {
                // window has gained focus
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
                        event.data = true;
                        event.data2 = palPackPointer(window);
                        palPushEvent(driver, &event);
                    }
                }
                break;
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
                break;
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
                            break;
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
                break;
            }

            case RANDR_SCREEN_CHANGE_EVENT: {
                // skip the first event
                if (s_X11.skipScreenEvent) {
                    s_X11.skipScreenEvent = false;
                    break;
                }

                // store old monitor count
                int oldCount = s_X11.monitorCount;
                s_X11.monitorCount = 0;
                xCacheMonitors();

                if (oldCount != s_X11.monitorCount) {
                    // a monitor has been added or removed
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
                }
                break;
            }
  
            case MotionNotify: {
                // mouse moved
                const int x = event.xmotion.x;
                const int y = event.xmotion.y;
                const int dx = x - s_Mouse.lastX;
                const int dy = y - s_Mouse.lastY;

                if (s_Video.eventDriver) {
                    PalEventDriver* driver = s_Video.eventDriver;
                    PalEventType type = PAL_EVENT_MOUSE_MOVE;
                    mode = palGetEventDispatchMode(driver, type);
                    if (mode != PAL_DISPATCH_NONE) {
                        PalEvent event = {0};
                        event.type = type;
                        event.data = palPackInt32(x, y);
                        event.data2 = palPackPointer(window);
                        palPushEvent(driver, &event);
                    }

                    // push a mouse delta event
                    type = PAL_EVENT_MOUSE_DELTA;
                    mode = palGetEventDispatchMode(driver, type);
                    if (mode != PAL_DISPATCH_NONE) {
                        PalEvent event = {0};
                        event.type = type;
                        event.data = palPackInt32(dx, dy);
                        event.data2 = palPackPointer(window);
                        palPushEvent(driver, &event);
                    }
                }

                s_Mouse.lastX = x;
                s_Mouse.lastY = y;
                s_Mouse.dx = dx;
                s_Mouse.dy = dy;
                break;
            }

            case ButtonPress:
            case ButtonRelease: {
                int xButton = event.xbutton.button;
                bool pressed = (event.xbutton.type == ButtonPress);
                PalMouseButton button = 0;
                PalEventType type;

                if (xButton == 1) {
                    button = PAL_MOUSE_BUTTON_LEFT;
                } else if (xButton == 3) {
                    button = PAL_MOUSE_BUTTON_RIGHT;
                } else if (xButton == 2) {
                    button = PAL_MOUSE_BUTTON_MIDDLE;
                }

                s_Mouse.state[button] = pressed;
                if (s_Video.eventDriver && button != 0) {
                    PalEventDriver* driver = s_Video.eventDriver;
                    if (pressed) {
                        type = PAL_EVENT_MOUSE_BUTTONDOWN;
                    } else {
                        type = PAL_EVENT_MOUSE_BUTTONUP;
                    }

                    mode = palGetEventDispatchMode(driver, type);
                    if (mode != PAL_DISPATCH_NONE) {
                        PalEvent event = {0};
                        event.type = type;
                        event.data = palPackUint32(button, 0);
                        event.data2 = palPackPointer(window);
                        palPushEvent(driver, &event);
                    }
                }

                int scrollX = 0;
                int scrollY = 0;
                if (xButton == 4) {
                    // scroll up
                    scrollY = 1;
                } else if (xButton == 5) {
                    // scroll down
                    scrollY = -1;
                } else if (xButton == 6) {
                    // scroll left
                    scrollX = -1;
                } else if (xButton == 7) {
                    // scroll right
                    scrollX = 1;
                }

                s_Mouse.WheelX = scrollX;
                s_Mouse.WheelY = scrollY;
                if (s_Video.eventDriver && (scrollX || scrollY)) {
                    PalEventDriver* driver = s_Video.eventDriver;
                    // clang-format off
                    mode = palGetEventDispatchMode(driver, PAL_EVENT_MOUSE_WHEEL);
                    // clang-format on

                    if (mode != PAL_DISPATCH_NONE) {
                        PalEvent event = {0};
                        event.type = PAL_EVENT_MOUSE_WHEEL;
                        event.data = palPackInt32(scrollX, scrollY);
                        event.data2 = palPackPointer(window);
                        palPushEvent(driver, &event);
                    }
                }
                break;
            }

            case KeyPress:
            case KeyRelease: {
                int xScancode = event.xkey.keycode;
                bool pressed = (event.xbutton.type == KeyPress);
                PalScancode scancode = PAL_SCANCODE_UNKNOWN;
                PalKeycode keycode = PAL_KEYCODE_UNKNOWN;
                PalEventType type;
                KeySym keySym = s_X11.lookupKeysym(&event.xkey, 0);

                // special handling for Pause/break with Home
                if (xScancode == 110) {
                    if (keySym == XK_Pause) {
                        scancode = PAL_SCANCODE_PAUSE;
                    } else {
                        scancode = PAL_SCANCODE_HOME;
                    }

                } else {
                    int index = xScancode - 8;
                    scancode = s_Keyboard.scancodes[index];
                }

                // printable and text input keys are from the range
                // 32 (PAL_KEYCODE_SPACE) and 122 (PAL_KEYCODE_Z)
                // The rest are almost the same as their scancode
                // Maybe there will be a layout that makes this wrong
                // but for now this works
                if (keySym >= XK_space && keySym <= XK_z) {
                    // a printable or input key
                    keycode = s_Keyboard.keycodes[keySym];

                } else {
                    // Since PalKeycode and PalScancode have the same integers
                    // we can make a direct cast without a table
                    // Examle: PAL_KEYCODE_A(int 0) == PAL_SCANCODE_A(int 0)
                    keycode = (PalKeycode)(Uint32)scancode;
                }

                // If we got a keySym but its not mapped into our keycode array
                // we do a direct cast as well
                if (keycode == PAL_KEYCODE_UNKNOWN) {
                    keycode = (PalKeycode)(Uint32)scancode;
                }

                // update our keyboard and mouse state to handle key repeat
                bool repeat = s_Keyboard.keycodeState[keycode];
                s_Keyboard.scancodeState[scancode] = pressed;
                s_Keyboard.keycodeState[keycode] = pressed;

                if (pressed) {
                    if (repeat) {
                        type = PAL_EVENT_KEYREPEAT;
                    } else {
                        type = PAL_EVENT_KEYDOWN;
                    }
                } else {
                    type = PAL_EVENT_KEYUP;
                }

                if (s_Video.eventDriver) {
                    PalEventDriver* driver = s_Video.eventDriver;
                    mode = palGetEventDispatchMode(driver, type);
                    if (mode != PAL_DISPATCH_NONE) {
                        PalEvent event = {0};
                        event.type = type;
                        event.data = palPackUint32(keycode, scancode);
                        event.data2 = palPackPointer(window);
                        palPushEvent(driver, &event);
                    }

                    // check for char event if enabled
                    type = PAL_EVENT_KEYCHAR;
                    mode = palGetEventDispatchMode(driver, type);
                    if (mode == PAL_DISPATCH_NONE) {
                        break;
                    }

                    int status;
                    char buffer[32];
                    KeySym keySym;
                    int len = s_X11.utf8LookupString(
                        data->ic,
                        &event.xkey,
                        buffer,
                        sizeof(buffer),
                        &keySym,
                        &status);

                    Uint32 codepoint = 0;
                    if (status == XLookupChars || status == XLookupBoth) {
                        // decode to Unicode codepoint
                        unsigned char ch = buffer[0];
                        if (ch < 0x80) {
                            // 1 byte (A-Z)
                            codepoint = ch;

                        } else if ((ch >> 5) == 0x6 && len >= 2) {
                            // 2 byte
                            codepoint = ((ch & 0x1F) << 6) | buffer[1] & 0x3F;

                        } else if ((ch >> 4) == 0xE && len >= 3) {
                            // 3 byte
                            // clang-format off
                            codepoint = ((ch & 0x0F) << 12)       | 
                                        ((buffer[1] & 0x3F) << 6) | 
                                        (buffer[2] & 0x3F);
                            // clang-format on

                        } else if ((ch >> 3) == 0x1E && len >= 4) {
                            // 4 byte
                            // clang-format off
                            codepoint = ((ch & 0x07) << 18)        | 
                                        ((buffer[1] & 0x3F) << 12) | 
                                        ((buffer[2] & 0x3F) << 6)  | 
                                        (buffer[3] & 0x3F);
                            // clang-format on
                        }

                        PalEvent event = {0};
                        event.type = type;
                        event.data = codepoint;
                        event.data2 = palPackPointer(window);
                        palPushEvent(driver, &event);
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
    // clang-format off
    XRRScreenResources* resources = s_X11.getScreenResources(s_X11.display, s_X11.root);
    // clang-format on

    for (int i = 0; i < resources->noutput; ++i) {
        RROutput output = resources->outputs[i];
        // clang-format off
        XRROutputInfo* outputInfo = s_X11.getOutputInfo(s_X11.display, resources, output);
        // clang-format on

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
    RROutput primary = s_X11.getOutputPrimary(s_X11.display, s_X11.root);
    if (primary) {
        *outMonitor = TO_PAL_HANDLE(PalMonitor, primary);
        return PAL_RESULT_SUCCESS;
    }

    return PAL_RESULT_PLATFORM_FAILURE;
}

static PalResult xGetMonitorInfo(
    PalMonitor* monitor,
    PalMonitorInfo* info)
{
    // clang-format off
    XRRScreenResources* resources = s_X11.getScreenResources(s_X11.display, s_X11.root);
    // clang-format on

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
    RROutput primary = s_X11.getOutputPrimary(s_X11.display, s_X11.root);

    if (monitor == TO_PAL_HANDLE(PalMonitor, primary)) {
        info->primary = true;
    } else {
        info->primary = false;
    }

    // get monitor pos and size
    // clang-format off
    XRRCrtcInfo* crtc = s_X11.getCrtcInfo(s_X11.display, resources, outputInfo->crtc);
    // clang-format on

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
    float raw = crtc->width / 1920.0f;
    float steps[] = {1.0f, 1.2f, 1.5f, 1.75, 2.0f};
    float closest = steps[0];
    float minDiff = fabsf(raw - steps[0]);

    for (int i = 1; i < sizeof(steps) / sizeof(steps[0]); i++) {
        float diff = fabsf(raw - steps[i]);
        if (diff < minDiff) {
            minDiff = diff;
            closest = steps[i];
        }
    }

    info->dpi = (Uint32)(closest * 96.0f);

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

    // clang-format off
    XRRScreenResources* resources = s_X11.getScreenResources(s_X11.display, s_X11.root);
    // clang-format on

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

                        // clang-format off
                        double tmp = (double)info->hTotal * (double)info->vTotal;
                        // clang-format on

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
    // clang-format off
    XRRScreenResources* resources = s_X11.getScreenResources(s_X11.display, s_X11.root);
    // clang-format on

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
    // clang-format off
    XRRCrtcInfo* crtc = s_X11.getCrtcInfo(s_X11.display, resources, outputInfo->crtc);
    // clang-format on

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
    // clang-format off
    XRRScreenResources* resources = s_X11.getScreenResources(s_X11.display, s_X11.root);
    // clang-format on

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
    // clang-format off
    XRRCrtcInfo* crtc = s_X11.getCrtcInfo(s_X11.display, resources, outputInfo->crtc);
    // clang-format on

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
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

static PalResult xSetMonitorOrientation(
    PalMonitor* monitor,
    PalOrientation orientation)
{
    // clang-format off
    XRRScreenResources* resources = s_X11.getScreenResources(s_X11.display, s_X11.root);
    // clang-format on

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
    // clang-format off
    XRRCrtcInfo* crtc = s_X11.getCrtcInfo(s_X11.display, resources, outputInfo->crtc);
    // clang-format on

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

    if (s_X11.visualInfo) {
        visual = s_X11.visualInfo->visual;
        depth = s_X11.visualInfo->depth;
        bgPixel = 0;
        borderPixel = 0;

        // clang-format off
        
        colormap = s_X11.createColormap(
            s_X11.display, 
            s_X11.root, 
            visual, 
            AllocNone);
        // clang-format on

        if (!colormap) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        data->colormap = colormap;

    } else {
        // use a default visual
        visual = DefaultVisual(s_X11.display, s_X11.screen);
        depth = DefaultDepth(s_X11.display, s_X11.screen);
        bgPixel = WhitePixel(s_X11.display, s_X11.screen);
        borderPixel = BlackPixel(s_X11.display, s_X11.screen);
        colormap = DefaultColormap(s_X11.display, s_X11.screen);
        data->colormap = None;
    }

    // get monitor
    int monitorX = 0;
    int monitorY = 0;
    Uint32 monitorW = 0;
    Uint32 monitorH = 0;
    int dpi = 0;
    if (info->monitor) {
        monitor = info->monitor;

    } else {
        // get primary monitor
        xGetPrimaryMonitor(&monitor);
    }

    if (monitor) {
        // get monitor info
        PalResult result = palGetMonitorInfo(monitor, &monitorInfo);
        if (result != PAL_RESULT_SUCCESS) {
            return result;
        }

        monitorX = monitorInfo.x;
        monitorY = monitorInfo.y;
        monitorW = monitorInfo.width;
        monitorH = monitorInfo.height;
        dpi = monitorInfo.dpi;

    } else {
        // primary monitor is not set
        XRRScreenResources* resources = nullptr;
        resources = s_X11.getScreenResources(s_X11.display, s_X11.root);
        for (int i = 0; i < resources->noutput; ++i) {
            RROutput output = resources->outputs[i];
            XRROutputInfo* outputInfo = s_X11.getOutputInfo(
                s_X11.display,
                resources,
                FROM_PAL_HANDLE(RROutput, monitor));

            // check if its a monitor
            if (outputInfo->connection != RR_Connected ||
                outputInfo->crtc == None) {
                s_X11.freeOutputInfo(outputInfo);
                continue;
            }

            // clang-format off
            XRRCrtcInfo* crtc = s_X11.getCrtcInfo(
                s_X11.display, 
                resources, 
                outputInfo->crtc);
            // clang-format on

            monitorX = crtc->x;
            monitorY = crtc->y;
            monitorW = crtc->width;
            monitorH = crtc->height;

            // get DPI
            float raw = crtc->width / 1920.0f;
            float steps[] = {1.0f, 1.2f, 1.5f, 1.75, 2.0f};
            float closest = steps[0];
            float minDiff = fabsf(raw - steps[0]);

            for (int i = 1; i < sizeof(steps) / sizeof(steps[0]); i++) {
                float diff = fabsf(raw - steps[i]);
                if (diff < minDiff) {
                    minDiff = diff;
                    closest = steps[i];
                }
            }

            dpi = (Uint32)(closest * 96.0f);

            s_X11.freeCrtcInfo(crtc);
            s_X11.freeOutputInfo(outputInfo);
            break;
        }
        s_X11.freeScreenResources(resources);
    }

    Int32 x, y = 0;
    // the position and size must be scaled with the dpi before this call
    if (info->center) {
        x = monitorX + (monitorW - info->width) / 2;
        y = monitorY + (monitorH - info->height) / 2;

    } else {
        // we set 100 for each axix
        x = monitorX + 100;
        y = monitorY + 100;
    }

    // check and set transparency
    if (info->style & PAL_WINDOW_STYLE_TRANSPARENT) {
        if (!(s_Video.features & PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW)) {
            return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
        }

        // we dont need to set any flag
    }

    long mask = StructureNotifyMask | KeyPressMask;
    mask |= KeyReleaseMask;
    mask |= ButtonPressMask;
    mask |= ButtonReleaseMask;
    mask |= PointerMotionMask;
    mask |= FocusChangeMask;
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
            resClass = s_Video.className;
        }

        hints->res_name = (char*)resName;
        hints->res_class = (char*)resClass;
        s_X11.setClassHint(s_X11.display, window, hints);
        s_X11.free(hints);
    }

    if (s_X11Atoms.unicodeTitle && info->title) {
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
        if (info->title) {
            s_X11.storeName(s_X11.display, window, info->title);
        }
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

    // set size hints
    XSizeHints wmHints = {0};
    wmHints.flags = PPosition | PSize;
    wmHints.x = x;
    wmHints.y = y;
    wmHints.width = info->width;
    wmHints.height = info->height;

    // resizable
    if (!(info->style & PAL_WINDOW_STYLE_RESIZABLE)) {
        wmHints.flags |= PMinSize;
        wmHints.flags |= PMaxSize;
        wmHints.min_width = wmHints.max_width = info->width;
        wmHints.min_height = wmHints.max_height = info->height;
    }

    // show window
    s_X11.setWMNormalHints(s_X11.display, window, &wmHints);
    if (info->show) {
        s_X11.mapWindow(s_X11.display, window);
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

        xSendWMEvent(
            window,
            s_X11Atoms._NET_WM_STATE,
            s_X11Atoms._NET_WM_STATE_MAXIMIZED_VERT,
            s_X11Atoms._NET_WM_STATE_MAXIMIZED_HORZ,
            1,
            0,
            true); // _NET_WM_STATE_ADD
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

    s_X11
        .setWMProtocols(s_X11.display, window, &s_X11Atoms.WM_DELETE_WINDOW, 1);

    s_X11.flush(s_X11.display);

    // attach the window data to the window
    data->skipConfigure = true;
    data->skipState = true;
    data->isAttached = false; // true for attached windows
    data->dpi = dpi;          // the current window monitor
    data->window = TO_PAL_HANDLE(PalWindow, window);
    s_X11.saveContext(s_X11.display, window, s_X11.dataID, (XPointer)data);

    // create an input context
    data->ic = s_X11.createIC(
        s_X11.im,
        XNInputStyle,
        XIMPreeditNothing | XIMStatusNothing,
        XNClientWindow,
        window,
        XNFocusWindow,
        window,
        nullptr);

    if (!data->ic) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    *outWindow = TO_PAL_HANDLE(PalWindow, window);
    return PAL_RESULT_SUCCESS;
}

static void xDestroyWindow(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    WindowData* data = nullptr;
    s_X11.findContext(s_X11.display, xWin, s_X11.dataID, (XPointer*)&data);

    // PAL does not destroy an attached window
    if (data->isAttached) {
        return;
    }

    s_X11.destroyIC(data->ic);
    s_X11.destroyWindow(s_X11.display, xWin);

    if (data->colormap != None) {
        s_X11.freeColormap(s_X11.display, data->colormap);
    }

    data->used = false;
}

PalResult xMinimizeWindow(PalWindow* window)
{
    if (!(s_Video.features & PAL_VIDEO_FEATURE_WINDOW_SET_STATE)) {
        return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
    }

    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    s_X11.iconifyWindow(s_X11.display, xWin, s_X11.screen);
    return PAL_RESULT_SUCCESS;
}

PalResult xMaximizeWindow(PalWindow* window)
{
    if (!(s_Video.features & PAL_VIDEO_FEATURE_WINDOW_SET_STATE)) {
        return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
    }

    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    xSendWMEvent(
        xWin,
        s_X11Atoms._NET_WM_STATE,
        s_X11Atoms._NET_WM_STATE_MAXIMIZED_VERT,
        s_X11Atoms._NET_WM_STATE_MAXIMIZED_HORZ,
        1,
        0,
        true); // _NET_WM_STATE_ADD

    return PAL_RESULT_SUCCESS;
}

PalResult xRestoreWindow(PalWindow* window)
{
    if (!(s_Video.features & PAL_VIDEO_FEATURE_WINDOW_SET_STATE)) {
        return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
    }

    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    // since we have no fixed way to restore the window
    // we just restore from minimized and maximized state
    xSendWMEvent(
        xWin,
        s_X11Atoms._NET_WM_STATE,
        s_X11Atoms._NET_WM_STATE_MAXIMIZED_VERT,
        s_X11Atoms._NET_WM_STATE_MAXIMIZED_HORZ,
        1,
        0,
        false); // _NET_WM_STATE_REMOVE

    s_X11.mapRaised(s_X11.display, xWin);

    return PAL_RESULT_SUCCESS;
}

PalResult xShowWindow(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    s_X11.mapWindow(s_X11.display, xWin);
    return PAL_RESULT_SUCCESS;
}

PalResult xHideWindow(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    s_X11.unmapWindow(s_X11.display, xWin);
    return PAL_RESULT_SUCCESS;
}

PalResult xFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info)
{
    if (info->flags & PAL_FLASH_CAPTION) {
        return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
    }

    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    bool add = false;
    if (info->flags & PAL_FLASH_TRAY) {
        add = true;
    }

    // check if modern flashing is supported
    if (s_X11Atoms._NET_WM_STATE_DEMANDS_ATTENTIONS) {
        xSendWMEvent(
            xWin,
            s_X11Atoms._NET_WM_STATE,
            s_X11Atoms._NET_WM_STATE_DEMANDS_ATTENTIONS,
            0,
            0,
            0,
            add); // _NET_WM_STATE_ADD

    } else {
        // legacy mode
        XWMHints* hints = s_X11.getWMHints(s_X11.display, xWin);
        if (!hints) {
            hints = s_X11.allocWMHints();
            if (!hints) {
                return PAL_RESULT_OUT_OF_MEMORY;
            }

            if (add) {
                hints->flags |= XUrgencyHint;
            } else {
                hints->flags &= ~XUrgencyHint;
            }
            s_X11.setWMHints(s_X11.display, xWin, hints);
            s_X11.free(hints);
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult xGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* outStyle)
{
    // Window Manager quirks
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult xGetWindowMonitor(
    PalWindow* window,
    PalMonitor** outMonitor)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult xGetWindowTitle(
    PalWindow* window,
    Uint64 bufferSize,
    Uint64* outSize,
    char* outBuffer)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    if (!outBuffer || bufferSize <= 0) {
        return PAL_RESULT_INSUFFICIENT_BUFFER;
    }

    if (s_X11Atoms.unicodeTitle) {
        Atom type;
        int format;
        unsigned long count, bytesAfter;
        unsigned char* prop = nullptr;
        s_X11.getWindowProperty(
            s_X11.display,
            xWin,
            s_X11Atoms._NET_WM_NAME,
            0,
            (~0L),
            False,
            s_X11Atoms.UTF8_STRING,
            &type,
            &format,
            &count,
            &bytesAfter,
            &prop);

        if (bufferSize >= count) {
            strcpy(outBuffer, (const char*)prop);
        } else {
            // copy up to the limiit of the supplied buffer
            strncpy(outBuffer, (const char*)prop, bufferSize);
        }
        s_X11.free(prop);

    } else {
        XTextProperty text;
        if (!s_X11.getWMName(s_X11.display, xWin, &text)) {
            return PAL_RESULT_INVALID_WINDOW;
        }

        if (bufferSize >= text.nitems) {
            strcpy(outBuffer, (const char*)text.value);
        } else {
            // copy up to the limiit of the supplied buffer
            strncpy(outBuffer, (const char*)text.value, bufferSize);
        }
        s_X11.free(text.value);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult xGetWindowPos(
    PalWindow* window,
    Int32* x,
    Int32* y)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    if (x) {
        *x = attr.x;
    }

    if (y) {
        *y = attr.y;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult xGetWindowSize(
    PalWindow* window,
    Uint32* width,
    Uint32* height)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    if (width) {
        *width = attr.width;
    }

    if (height) {
        *height = attr.height;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult xGetWindowState(
    PalWindow* window,
    PalWindowState* outState)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    Atom type;
    int format;
    unsigned long count, bytesAfter;
    unsigned char* props = nullptr;
    s_X11.getWindowProperty(
        s_X11.display,
        xWin,
        s_X11Atoms._NET_WM_STATE,
        0,
        (~0L),
        False,
        XA_ATOM,
        &type,
        &format,
        &count,
        &bytesAfter,
        &props);

    PalWindowState state = PAL_WINDOW_STATE_RESTORED;
    for (unsigned long i = 0; i < count; ++i) {
        if (props[i] == s_X11Atoms._NET_WM_STATE_MAXIMIZED_HORZ) {
            state = PAL_WINDOW_STATE_MAXIMIZED;
        }

        if (props[i] == s_X11Atoms._NET_WM_STATE_MAXIMIZED_VERT) {
            state = PAL_WINDOW_STATE_MAXIMIZED;
        }

        if (props[i] == s_X11Atoms._NET_WM_STATE_HIDDEN) {
            state = PAL_WINDOW_STATE_MINIMIZED;
        }
    }

    s_X11.free(props);
    *outState = state;
    return PAL_RESULT_SUCCESS;
}

bool xIsWindowVisible(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return false;
    }

    return attr.map_state == IsViewable;
}

PalWindow* xGetFocusWindow()
{
    Window window;
    int tmp;
    s_X11.getInputFocus(s_X11.display, &window, &tmp);
    Window xWin = FROM_PAL_HANDLE(Window, window);

    if (xWin == s_X11.root) {
        return nullptr;
    }
    return TO_PAL_HANDLE(PalWindow, window);
}

PalWindowHandleInfo xGetWindowHandleInfo(PalWindow* window)
{
    PalWindowHandleInfo info;
    info.nativeDisplay = (void*)s_X11.display;
    info.nativeWindow = (void*)window;
    return info;
}

PalWindowHandleInfoEx xGetWindowHandleInfoEx(PalWindow* w)
{
    PalWindowHandleInfoEx info = {0};
    info.nativeDisplay = (void*)s_X11.display;
    info.nativeWindow = (void*)w;
    return info;
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

PalResult xSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style)
{
    // Window Manager quirks
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult xSetWindowTitle(
    PalWindow* window,
    const char* title)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    if (s_X11Atoms.unicodeTitle) {
        s_X11.changeProperty(
            s_X11.display,
            xWin,
            s_X11Atoms._NET_WM_NAME,
            s_X11Atoms.UTF8_STRING,
            8, // unsigned char
            PropModeReplace,
            title,
            strlen(title));

    } else {
        s_X11.storeName(s_X11.display, xWin, title);
    }

    s_X11.flush(s_X11.display);
    return PAL_RESULT_SUCCESS;
}

PalResult xSetWindowPos(
    PalWindow* window,
    Int32 x,
    Int32 y)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    s_X11.moveWindow(s_X11.display, xWin, x, y);
    s_X11.flush(s_X11.display);
    return PAL_RESULT_SUCCESS;
}

PalResult xSetWindowSize(
    PalWindow* window,
    Uint32 width,
    Uint32 height)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    // X11 does not allow users resize programaticaly
    // if the window is not resizable.
    // so we hack it by making the window resizable and resizing
    // then revert back.
    XSizeHints hints;
    long tmp;
    s_X11.getWMNormalHints(s_X11.display, xWin, &hints, &tmp);
    if ((hints.flags & PMinSize) && (hints.flags & PMaxSize)) {
        hints.flags &= ~PMinSize;
        hints.flags &= ~PMaxSize;
        s_X11.resizeWindow(s_X11.display, xWin, width, height);
        s_X11.flush(s_X11.display);

        // revert
        hints.flags |= PMinSize;
        hints.flags |= PMaxSize;
        hints.min_width = hints.max_width = width;
        hints.min_height = hints.max_height = height;

    } else {
        // window is already resizable
        s_X11.resizeWindow(s_X11.display, xWin, width, height);
    }

    s_X11.flush(s_X11.display);
    return PAL_RESULT_SUCCESS;
}

PalResult xSetFocusWindow(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    if (s_X11Atoms._NET_ACTIVE_WINDOW) {
        xSendWMEvent(
            xWin,
            s_X11Atoms._NET_ACTIVE_WINDOW,
            CurrentTime,
            0,
            0,
            0,
            true); // 1

    } else {
        s_X11.setInputFocus(s_X11.display, xWin, RevertToParent, CurrentTime);
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

        // clang-format off
        icon[2 + i] = ((unsigned long)a << 24) | 
                      ((unsigned long)r << 16) |
                      ((unsigned long)g << 8) |
                      ((unsigned long)b);
        // clang-format on
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

PalResult xCreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** outCursor)
{
    XcursorImage* image = s_X11.cursorImageCreate(info->width, info->height);
    image->xhot = info->xHotspot;
    image->yhot = info->yHotspot;

    // convert from RGBA8 to ARGB32
    for (int i = 0; i < info->width * info->height; i++) {
        Uint8 r = info->pixels[i * 4 + 0]; // Red
        Uint8 g = info->pixels[i * 4 + 1]; // Green
        Uint8 b = info->pixels[i * 4 + 2]; // Blue
        Uint8 a = info->pixels[i * 4 + 3]; // Alpha

        // clang-format off
        image->pixels[i] = ((unsigned long)a << 24) | 
                           ((unsigned long)r << 16) |
                           ((unsigned long)g << 8) |
                           ((unsigned long)b);
        // clang-format on
    }

    Cursor cursor = s_X11.cursorImageLoadCursor(s_X11.display, image);
    if (!cursor) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    s_X11.cursorImageDestroy(image);
    *outCursor = TO_PAL_HANDLE(PalCursor, cursor);
    return PAL_RESULT_SUCCESS;
}

PalResult xCreateCursorFrom(
    PalCursorType type,
    PalCursor** outCursor)
{
    int shape;
    Cursor cursor;
    switch (type) {
        case PAL_CURSOR_ARROW: {
            shape = XC_left_ptr;
            break;
        }

        case PAL_CURSOR_HAND: {
            shape = XC_hand2;
            break;
        }

        case PAL_CURSOR_CROSS: {
            shape = XC_cross;
            break;
        }

        case PAL_CURSOR_IBEAM: {
            shape = XC_xterm;
            break;
        }

        case PAL_CURSOR_WAIT: {
            shape = XC_watch;
            break;
        }

        default: {
            return PAL_RESULT_INVALID_ARGUMENT;
        }
    }

    cursor = s_X11.createFontCursor(s_X11.display, shape);
    *outCursor = TO_PAL_HANDLE(PalCursor, cursor);
    return PAL_RESULT_SUCCESS;
}

void xDestroyCursor(PalCursor* cursor)
{
    s_X11.freeCursor(s_X11.display, FROM_PAL_HANDLE(Cursor, cursor));
}

void xShowCursor(bool show)
{
    // x11 does not support Hiding and showing cursor
    return;
}

PalResult xClipCursor(
    PalWindow* window,
    bool clip)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    if (clip) {
        s_X11.grabPointer(
            s_X11.display,
            xWin,
            True,
            0,
            GrabModeAsync,
            GrabModeAsync,
            xWin,
            None,
            CurrentTime);

    } else {
        s_X11.ungrabPointer(s_X11.display, CurrentTime);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult xGetCursorPos(
    PalWindow* window,
    Int32* x,
    Int32* y)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    Window root, rootChild;
    int rootX, rootY, winX, winY;
    unsigned int mask;
    s_X11.queryPointer(
        s_X11.display,
        xWin,
        &root,
        &rootChild,
        &rootX,
        &rootY,
        &winX,
        &winY,
        &mask);

    if (x) {
        *x = winX;
    }

    if (y) {
        *y = winY;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult xSetCursorPos(
    PalWindow* window,
    Int32 x,
    Int32 y)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    s_X11.warpPointer(s_X11.display, None, xWin, 0, 0, 0, 0, x, y);

    s_X11.flush(s_X11.display);
    return PAL_RESULT_SUCCESS;
}

PalResult xSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    Window xCursor = FROM_PAL_HANDLE(Cursor, cursor);
    if (xCursor) {
        s_X11.defineCursor(s_X11.display, xWin, xCursor);

    } else {
        s_X11.undefineCursor(s_X11.display, xWin);
    }

    s_X11.flush(s_X11.display);
    return PAL_RESULT_SUCCESS;
}

PalResult xAttachWindow(
    void* windowHandle,
    PalWindow** outWindow)
{
    Window xWin = FROM_PAL_HANDLE(Window, windowHandle);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    // get a free slot and set the window handle to it
    // we also set a flag to make sure we know this is an attached window
    WindowData* data = getFreeWindowData();
    if (!data) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    PalWindow* window = TO_PAL_HANDLE(PalWindow, xWin);
    // we assume the window was just created, since there is
    // no official way to get the DPI
    data->isAttached = true;
    data->dpi = 96; // if this is not the DPI, a dpi event will be triggered

    // If the window manager has not mapped the window yet,
    // we dont need the initial Size / Move events
    data->skipConfigure = true;
    data->skipState = true;
    data->skipIfAttached = true;
    data->window = window;
    data->w = attr.width;
    data->h = attr.height;
    data->x = attr.x;
    data->y = attr.y;

    // get the current window state
    // we dont check the return code because we know the window is valid
    xGetWindowState(window, &data->state);

    // listen to the events we support
    long mask = StructureNotifyMask | KeyPressMask;
    mask |= KeyReleaseMask;
    mask |= ButtonPressMask;
    mask |= ButtonReleaseMask;
    mask |= PointerMotionMask;
    mask |= FocusChangeMask;
    mask |= PropertyChangeMask;
    s_X11.selectInput(s_X11.display, xWin, mask);

    // listen to window close event
    s_X11.setWMProtocols(s_X11.display, xWin, &s_X11Atoms.WM_DELETE_WINDOW, 1);

    s_X11.saveContext(s_X11.display, xWin, s_X11.dataID, (XPointer)data);
    s_X11.flush(s_X11.display);

    *outWindow = window;
    return PAL_RESULT_SUCCESS;
}

PalResult xDetachWindow(
    PalWindow* window,
    void** outWindowHandle)
{
    // we check is the window is really detachable
    Window xWin = FROM_PAL_HANDLE(Window, window);
    WindowData* data = nullptr;
    s_X11.findContext(s_X11.display, xWin, s_X11.dataID, (XPointer*)&data);
    if (!data) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    if (data->isAttached == false) {
        // window was created by PAL
        return PAL_RESULT_INVALID_WINDOW;
    }

    // detach the window
    data->used = false;
    long mask = 0;
    s_X11.selectInput(s_X11.display, xWin, mask);
    s_X11.setWMProtocols(s_X11.display, xWin, nullptr, 0);

    if (outWindowHandle) {
        *outWindowHandle = (void*)window;
    }

    return PAL_RESULT_SUCCESS;
}

static Backend s_XBackend = {
    .shutdownVideo = xShutdownVideo,
    .updateVideo = xUpdateVideo,
    .setFBConfig = xSetFBConfig,
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
    .maximizeWindow = xMaximizeWindow,
    .minimizeWindow = xMinimizeWindow,
    .restoreWindow = xRestoreWindow,
    .showWindow = xShowWindow,
    .hideWindow = xHideWindow,
    .flashWindow = xFlashWindow,
    .getWindowStyle = xGetWindowStyle,
    .getWindowMonitor = xGetWindowMonitor,
    .getWindowTitle = xGetWindowTitle,
    .getWindowPos = xGetWindowPos,
    .getWindowSize = xGetWindowSize,
    .getWindowState = xGetWindowState,
    .isWindowVisible = xIsWindowVisible,
    .getFocusWindow = xGetFocusWindow,
    .getWindowHandleInfo = xGetWindowHandleInfo,
    .getWindowHandleInfoEx = xGetWindowHandleInfoEx,
    .setWindowOpacity = xSetWindowOpacity,
    .setWindowStyle = xSetWindowStyle,
    .setWindowTitle = xSetWindowTitle,
    .setWindowPos = xSetWindowPos,
    .setWindowSize = xSetWindowSize,
    .setFocusWindow = xSetFocusWindow,

    .createIcon = xCreateIcon,
    .destroyIcon = xDestroyIcon,
    .setWindowIcon = xSetWindowIcon,

    .createCursor = xCreateCursor,
    .createCursorFrom = xCreateCursorFrom,
    .destroyCursor = xDestroyCursor,
    .showCursor = xShowCursor,
    .clipCursor = xClipCursor,
    .getCursorPos = xGetCursorPos,
    .setCursorPos = xSetCursorPos,
    .setWindowCursor = xSetWindowCursor,

    .attachWindow = xAttachWindow,
    .detachWindow = xDetachWindow};

#endif // PAL_HAS_X11
#pragma endregion

// ==================================================
// Wayland API
// ==================================================

#pragma region Wayland API
#if PAL_HAS_WAYLAND

PalResult eglWlBackend(const int index)
{
    // user choose EGL FBConfig backend
    if (!s_Egl.handle) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    EGLDisplay display = EGL_NO_DISPLAY;
    display = s_Egl.eglGetDisplay((EGLNativeDisplayType)s_Wl.display);

    if (display == EGL_NO_DISPLAY) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    EGLint numConfigs = 0;
    if (!s_Egl.eglGetConfigs(display, nullptr, 0, &numConfigs)) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    EGLint configSize = sizeof(EGLConfig) * numConfigs;
    EGLConfig* eglConfigs = palAllocate(s_Video.allocator, configSize, 0);
    if (!eglConfigs) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    s_Egl.eglGetConfigs(display, eglConfigs, numConfigs, &numConfigs);
    s_Wl.eglFBConfig = eglConfigs[index];

    return PAL_RESULT_SUCCESS;
}

static void wlCreateKeycodeTable()
{
    // Tis is for only printable and text input keys

    // Letters
    s_Keyboard.keycodes[XKB_KEY_a] = PAL_KEYCODE_A;
    s_Keyboard.keycodes[XKB_KEY_b] = PAL_KEYCODE_B;
    s_Keyboard.keycodes[XKB_KEY_c] = PAL_KEYCODE_C;
    s_Keyboard.keycodes[XKB_KEY_d] = PAL_KEYCODE_D;
    s_Keyboard.keycodes[XKB_KEY_e] = PAL_KEYCODE_E;
    s_Keyboard.keycodes[XKB_KEY_f] = PAL_KEYCODE_F;
    s_Keyboard.keycodes[XKB_KEY_g] = PAL_KEYCODE_G;
    s_Keyboard.keycodes[XKB_KEY_h] = PAL_KEYCODE_H;
    s_Keyboard.keycodes[XKB_KEY_i] = PAL_KEYCODE_I;
    s_Keyboard.keycodes[XKB_KEY_j] = PAL_KEYCODE_J;
    s_Keyboard.keycodes[XKB_KEY_k] = PAL_KEYCODE_K;
    s_Keyboard.keycodes[XKB_KEY_l] = PAL_KEYCODE_L;
    s_Keyboard.keycodes[XKB_KEY_m] = PAL_KEYCODE_M;
    s_Keyboard.keycodes[XKB_KEY_n] = PAL_KEYCODE_N;
    s_Keyboard.keycodes[XKB_KEY_o] = PAL_KEYCODE_O;
    s_Keyboard.keycodes[XKB_KEY_p] = PAL_KEYCODE_P;
    s_Keyboard.keycodes[XKB_KEY_q] = PAL_KEYCODE_Q;
    s_Keyboard.keycodes[XKB_KEY_r] = PAL_KEYCODE_R;
    s_Keyboard.keycodes[XKB_KEY_s] = PAL_KEYCODE_S;
    s_Keyboard.keycodes[XKB_KEY_t] = PAL_KEYCODE_T;
    s_Keyboard.keycodes[XKB_KEY_u] = PAL_KEYCODE_U;
    s_Keyboard.keycodes[XKB_KEY_v] = PAL_KEYCODE_V;
    s_Keyboard.keycodes[XKB_KEY_w] = PAL_KEYCODE_W;
    s_Keyboard.keycodes[XKB_KEY_x] = PAL_KEYCODE_X;
    s_Keyboard.keycodes[XKB_KEY_y] = PAL_KEYCODE_Y;
    s_Keyboard.keycodes[XKB_KEY_z] = PAL_KEYCODE_Z;

    // Control
    s_Keyboard.keycodes[XKB_KEY_space] = PAL_KEYCODE_SPACE;

    // Misc
    s_Keyboard.keycodes[XKB_KEY_apostrophe] = PAL_KEYCODE_APOSTROPHE;
    s_Keyboard.keycodes[XKB_KEY_backslash] = PAL_KEYCODE_BACKSLASH;
    s_Keyboard.keycodes[XKB_KEY_comma] = PAL_KEYCODE_COMMA;
    s_Keyboard.keycodes[XKB_KEY_equal] = PAL_KEYCODE_EQUAL;
    s_Keyboard.keycodes[XKB_KEY_grave] = PAL_KEYCODE_GRAVEACCENT;
    s_Keyboard.keycodes[XKB_KEY_minus] = PAL_KEYCODE_SUBTRACT;
    s_Keyboard.keycodes[XKB_KEY_period] = PAL_KEYCODE_PERIOD;
    s_Keyboard.keycodes[XKB_KEY_semicolon] = PAL_KEYCODE_SEMICOLON;
    s_Keyboard.keycodes[XKB_KEY_slash] = PAL_KEYCODE_SLASH;
    s_Keyboard.keycodes[XKB_KEY_bracketleft] = PAL_KEYCODE_LBRACKET;
    s_Keyboard.keycodes[XKB_KEY_bracketright] = PAL_KEYCODE_RBRACKET;
}

static int createShmFile(Uint64 size)
{
    char template[] = "/tmp/pal-shm-XXXXXX";
    int fd = mkstemp(template);
    if (fd < 0) {
        return -1;
    }

    unlink(template);
    if (ftruncate(fd, size) < 0) {
        return -1;
    }

    return fd;
}

static struct wl_buffer* createShmBuffer(
    int width,
    int height,
    const Uint8* pixels,
    bool cursor)
{
    int stride = width * 4;
    Uint64 size = stride * height;
    struct wl_buffer* buffer = nullptr;
    struct wl_shm_pool* pool = nullptr;
    void* data = nullptr;

    int fd = createShmFile(size);
    if (fd == -1) {
        return nullptr;
    }

    data = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        return nullptr;
    }

    enum wl_shm_format format = WL_SHM_FORMAT_XRGB8888;
    if (cursor) {
        format = WL_SHM_FORMAT_ARGB8888;
        Uint32* dataPixels = (Uint32*)data;

        // convert from RGBA8 to ARGB32
        for (int i = 0; i < width * height; i++) {
            Uint8 r = pixels[i * 4 + 0]; // Red
            Uint8 g = pixels[i * 4 + 1]; // Green
            Uint8 b = pixels[i * 4 + 2]; // Blue
            Uint8 a = pixels[i * 4 + 3]; // Alpha

            // clang-format off
            dataPixels[i] = ((unsigned long)a << 24) | 
                            ((unsigned long)r << 16) |
                            ((unsigned long)g << 8) |
                            ((unsigned long)b);
            // clang-format on
        }

    } else {
        memset(data, 0xFF, size); // white
    }

    pool = wlShmCreatePool(s_Wl.shm, fd, size);
    if (!pool) {
        return nullptr;
    }

    buffer = wlShmPoolCreateBuffer(pool, 0, width, height, stride, format);

    if (!buffer) {
        return nullptr;
    }

    wlShmPoolDestroy(pool);
    munmap(data, size);
    close(fd);
    return buffer;
}

static void wlOutputGeometry(
    void* data,
    struct wl_output* output,
    int32_t x,
    int32_t y,
    int32_t, // we dont need physical size
    int32_t, // we dont need physical size
    int32_t, // we dont need subpixel
    const char* make,
    const char* model,
    int32_t transform)
{
    MonitorData* monitorData = data;
    monitorData->x = x;
    monitorData->y = y;

    switch (transform) {
        case WL_OUTPUT_TRANSFORM_NORMAL:
        case WL_OUTPUT_TRANSFORM_180: {
            monitorData->orientation = PAL_ORIENTATION_LANDSCAPE;
            break;
        }

        case WL_OUTPUT_TRANSFORM_90:
        case WL_OUTPUT_TRANSFORM_270: {
            monitorData->orientation = PAL_ORIENTATION_PORTRAIT;
            break;
        }

        case WL_OUTPUT_TRANSFORM_FLIPPED:
        case WL_OUTPUT_TRANSFORM_FLIPPED_180: {
            monitorData->orientation = PAL_ORIENTATION_LANDSCAPE_FLIPPED;
            break;
        }

        case WL_OUTPUT_TRANSFORM_FLIPPED_90:
        case WL_OUTPUT_TRANSFORM_FLIPPED_270: {
            monitorData->orientation = PAL_ORIENTATION_PORTRAIT_FLIPPED;
            break;
        }
    }

    snprintf(monitorData->name, 32, "%s %s", make, model);
}

static void wlOutputMode(
    void* data,
    struct wl_output* output,
    uint32_t flags,
    int32_t width,
    int32_t height,
    int32_t refresh)
{
    MonitorData* monitorData = data;
    if (flags & WL_OUTPUT_MODE_CURRENT) {
        monitorData->w = width;
        monitorData->h = height;
        monitorData->refreshRate = (refresh + 500) / 1000;

        // wayland only sends the current mode
        monitorData->mode.bpp = 0;
        monitorData->mode.width = width;
        monitorData->mode.height = height;
        monitorData->mode.refreshRate = (refresh + 500) / 1000;
    }
}

static void wlOutputScale(
    void* data,
    struct wl_output* output,
    int32_t scale)
{
    MonitorData* monitorData = data;
    float dpi = (float)scale * 96.0f;
    monitorData->dpi = (Uint32)dpi;
}

static void wlOutputDone(
    void* data,
    struct wl_output* output)
{
}

static const struct wl_output_listener s_OutputListener = {
    .geometry = wlOutputGeometry,
    .mode = wlOutputMode,
    .done = wlOutputDone,
    .scale = wlOutputScale};

static const struct wl_output_listener s_DefaultModeListener = {
    .geometry = wlOutputGeometry,
    .mode = wlOutputMode,
    .done = wlOutputDone,
    .scale = wlOutputScale};

static void globalHandle(
    void* data,
    struct wl_registry* registry,
    uint32_t name,
    const char* interface,
    uint32_t version)
{
    if (s_Wl.checkFeatures) {
        PalVideoFeatures features = 0;
        PalVideoFeatures64 features64 = 0;
        features |= PAL_VIDEO_FEATURE_HIGH_DPI;
        features |= PAL_VIDEO_FEATURE_MONITOR_GET_ORIENTATION;
        features |= PAL_VIDEO_FEATURE_MULTI_MONITORS;
        features |= PAL_VIDEO_FEATURE_MONITOR_GET_MODE;
        features |= PAL_VIDEO_FEATURE_WINDOW_SET_TITLE;
        features |= PAL_VIDEO_FEATURE_WINDOW_SET_SIZE;
        features |= PAL_VIDEO_FEATURE_WINDOW_SET_STATE;
        features |= PAL_VIDEO_FEATURE_BORDERLESS_WINDOW;

        features64 |= PAL_VIDEO_FEATURE64_HIGH_DPI;
        features64 |= PAL_VIDEO_FEATURE64_MONITOR_GET_ORIENTATION;
        features64 |= PAL_VIDEO_FEATURE64_MULTI_MONITORS;
        features64 |= PAL_VIDEO_FEATURE64_MONITOR_GET_MODE;
        features64 |= PAL_VIDEO_FEATURE64_WINDOW_SET_TITLE;
        features64 |= PAL_VIDEO_FEATURE64_WINDOW_SET_SIZE;
        features64 |= PAL_VIDEO_FEATURE64_WINDOW_SET_STATE;
        features64 |= PAL_VIDEO_FEATURE64_BORDERLESS_WINDOW;

        features64 |= PAL_VIDEO_FEATURE64_WINDOW_SET_CURSOR;

        s_Video.features = features;
        s_Video.features64 = features64;
        s_Wl.checkFeatures = false;
    }

    if (strcmp(interface, "wl_compositor") == 0) {
        s_Wl.compositor =
            wlRegistryBind(registry, name, s_Wl.compositorInterface, 4);

    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        s_Wl.xdgBase =
            wlRegistryBind(registry, name, &xdg_wm_base_interface, 1);

        xdgWmBaseAddListener(s_Wl.xdgBase, &wmBaseListener, nullptr);

    } else if (strcmp(interface, "wl_shm") == 0) {
        s_Wl.shm = wlRegistryBind(registry, name, s_Wl.shmInterface, 1);

    } else if (strcmp(interface, "wl_seat") == 0) {
        s_Wl.seat = wlRegistryBind(registry, name, s_Wl.seatInterface, 5);

        wlSeatAddListener(s_Wl.seat, &seatListener, nullptr);

    } else if (strcmp(interface, "zxdg_decoration_manager_v1") == 0) {
        s_Wl.decorationManager = wlRegistryBind(
            registry,
            name,
            &zxdg_decoration_manager_v1_interface,
            1);

        s_Video.features64 |= PAL_VIDEO_FEATURE64_DECORATED_WINDOW;

    } else if (strcmp(interface, "wl_output") == 0) {
        // wayland does not let use query monitors directly
        // so we enumerate and store at init and update the
        // cache when a monitor is added or removed
        MonitorData* monitorData = getFreeMonitorData();
        if (!monitorData) {
            return;
        }

        struct wl_output* output = nullptr;
        output = wlRegistryBind(s_Wl.registry, name, s_Wl.outputInterface, 3);
        wlOutputAddListener(output, &s_OutputListener, monitorData);

        monitorData->wlName = name;
        monitorData->monitor = (PalMonitor*)output;
        s_Wl.monitorCount++;
    }
}

static void globalRemove(
    void* data,
    struct wl_registry* registry,
    uint32_t name)
{
    for (int i = 0; i < s_Video.maxMonitorData; ++i) {
        if (s_Video.monitorData[i].used &&
            s_Video.monitorData[i].wlName == name) {
            MonitorData* data = &s_Video.monitorData[i];
            data->used = false;
            s_Wl.proxyDestroy((struct wl_proxy*)data->monitor);
            s_Wl.monitorCount--;
        }
    }
}

static const struct wl_registry_listener s_RegistryListener = {
    .global = globalHandle,
    .global_remove = globalRemove};

PalResult wlInitVideo()
{
    // load wayland libray
    s_Wl.handle = dlopen("libwayland-client.so.0", RTLD_LAZY);
    s_Wl.xkbCommon = dlopen("libxkbcommon.so", RTLD_LAZY);
    s_Wl.libCursor = dlopen("libwayland-cursor.so", RTLD_LAZY);
    s_Wl.libWaylandEgl = dlopen("libwayland-egl.so", RTLD_LAZY);

    // clang-format off
    if (!s_Wl.handle || 
        !s_Wl.xkbCommon || 
        !s_Wl.libCursor || 
        !s_Wl.libWaylandEgl) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // get the exported global variables
    s_Wl.outputInterface = dlsym(s_Wl.handle, "wl_output_interface");
    s_Wl.seatInterface = dlsym(s_Wl.handle, "wl_seat_interface");
    s_Wl.compositorInterface = dlsym(s_Wl.handle, "wl_compositor_interface");
    s_Wl.surfaceInterface = dlsym(s_Wl.handle, "wl_surface_interface");
    s_Wl.registryInterface = dlsym(s_Wl.handle, "wl_registry_interface");
    s_Wl.shmInterface = dlsym(s_Wl.handle, "wl_shm_interface");
    s_Wl.bufferInterface = dlsym(s_Wl.handle, "wl_buffer_interface");
    s_Wl.shmPoolInterface = dlsym(s_Wl.handle, "wl_shm_pool_interface");
    s_Wl.regionInterface = dlsym(s_Wl.handle, "wl_region_interface");
    s_Wl.pointerInterface = dlsym(s_Wl.handle, "wl_pointer_interface");
    s_Wl.keyboardInterface = dlsym(s_Wl.handle, "wl_keyboard_interface");

    // load function procs
    s_Wl.displayConnect = (wl_display_connect_fn)dlsym(
        s_Wl.handle, 
        "wl_display_connect");

    s_Wl.displayDisconnect = (wl_display_disconnect_fn)dlsym(
        s_Wl.handle, 
        "wl_display_disconnect");

    s_Wl.displayRoundtrip = (wl_display_roundtrip_fn)dlsym(
        s_Wl.handle, 
        "wl_display_roundtrip");

    s_Wl.displayDispatch = (wl_display_dispatch_fn)dlsym(
        s_Wl.handle, 
        "wl_display_dispatch");

    s_Wl.proxyAddListener = (wl_proxy_add_listener_fn)dlsym(
        s_Wl.handle, 
        "wl_proxy_add_listener");

    s_Wl.proxyMarshalCnstructor = (wl_proxy_marshal_constructor_v_fn)dlsym(
        s_Wl.handle, 
        "wl_proxy_marshal_constructor_versioned");

    s_Wl.proxyDestroy = (wl_proxy_destroy_fn)dlsym(
        s_Wl.handle, 
        "wl_proxy_destroy");

    s_Wl.proxyMarshalFlags = (wl_proxy_marshal_flags_fn)dlsym(
        s_Wl.handle, 
        "wl_proxy_marshal_flags");

    s_Wl.proxyGetVersion = (wl_proxy_get_version_fn)dlsym(
        s_Wl.handle, 
        "wl_proxy_get_version");

    s_Wl.getError = (wl_display_get_error_fn)dlsym(
        s_Wl.handle, 
        "wl_display_get_error");

    s_Wl.dispatchPending = (wl_display_dispatch_pending_fn)dlsym(
        s_Wl.handle, 
        "wl_display_dispatch_pending");

    s_Wl.displayFlush = (wl_display_flush_fn)dlsym(
        s_Wl.handle, 
        "wl_display_flush");

    s_Wl.prepareRead = (wl_display_prepare_read_fn)dlsym(
        s_Wl.handle, 
        "wl_display_prepare_read");

    s_Wl.readEvents = (wl_display_read_events_fn)dlsym(
        s_Wl.handle, 
        "wl_display_read_events");

    s_Wl.displayGetFd = (wl_display_get_fd_fn)dlsym(
        s_Wl.handle, 
        "wl_display_get_fd");

    s_Wl.cancelRead = (wl_display_cancel_read_fn)dlsym(
        s_Wl.handle, 
        "wl_display_cancel_read");

    // load xkbcommon procs
    s_Wl.xkbKeymapUnref = (xkb_keymap_unref_fn)dlsym(
        s_Wl.xkbCommon, 
        "xkb_keymap_unref");

    s_Wl.xkbStateKeyGetOneSym = (xkb_state_key_get_one_sym_fn)dlsym(
        s_Wl.xkbCommon, 
        "xkb_state_key_get_one_sym");

    s_Wl.xkbStateNew = (xkb_state_new_fn)dlsym(
        s_Wl.xkbCommon, 
        "xkb_state_new");

    s_Wl.xkbStateUnref = (xkb_state_unref_fn)dlsym(
        s_Wl.xkbCommon, 
        "xkb_state_unref");

    s_Wl.xkbContextUnref = (xkb_context_unref_fn)dlsym(
        s_Wl.xkbCommon, 
        "xkb_context_unref");

    s_Wl.xkbContextNew = (xkb_context_new_fn)dlsym(
        s_Wl.xkbCommon, 
        "xkb_context_new");

    s_Wl.xkbKeymapNewFromString = (xkb_keymap_new_from_string_fn)dlsym(
        s_Wl.xkbCommon, 
        "xkb_keymap_new_from_string");

    s_Wl.xkbKeysymToUtf32 = (xkb_keysym_to_utf32_fn)dlsym(
        s_Wl.xkbCommon, 
        "xkb_keysym_to_utf32");

    s_Wl.xkbStateUpdateMask = (xkb_state_update_mask_fn)dlsym(
        s_Wl.xkbCommon, 
        "xkb_state_update_mask");

    s_Wl.xkbKeymapKeyRepeats = (xkb_keymap_key_repeats_fn)dlsym(
        s_Wl.xkbCommon, 
        "xkb_keymap_key_repeats");

    // load wayland cursor procs
    s_Wl.cursorImageGetBuffer = (wl_cursor_image_get_buffer_fn)dlsym(
        s_Wl.libCursor, 
        "wl_cursor_image_get_buffer");

    s_Wl.cursorThemeLoad = (wl_cursor_theme_load_fn)dlsym(
        s_Wl.libCursor, 
        "wl_cursor_theme_load");

    s_Wl.cursorThemeGetCursor = (wl_cursor_theme_get_cursor_fn)dlsym(
        s_Wl.libCursor, 
        "wl_cursor_theme_get_cursor");

    // wl_egl procs
    s_Wl.eglWindowCreate = (wl_egl_window_create_fn)dlsym(
        s_Wl.libWaylandEgl, 
        "wl_egl_window_create");

    s_Wl.eglWindowDestroy = (wl_egl_window_destroy_fn)dlsym(
        s_Wl.libWaylandEgl, 
        "wl_egl_window_destroy");

    s_Wl.eglWindowResize = (wl_egl_window_resize_fn)dlsym(
        s_Wl.libWaylandEgl, 
        "wl_egl_window_resize");

    // clang-format on

    // initialize wayland
    s_Wl.checkFeatures = true;
    s_Wl.monitorCount = 0;
    setupXdgShellProtocol();

    // check if user supplied their own display
    if (s_Video.platformInstance) {
        s_Wl.display = (struct wl_display*)s_Video.platformInstance;

    } else {
        s_Wl.display = s_Wl.displayConnect(nullptr);
        s_Video.platformInstance = nullptr;
    }

    if (!s_Wl.display) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    s_Video.display = (void*)s_Wl.display;
    s_Wl.registry = wlDisplayGetRegistry(s_Wl.display);
    wlRegistryAddListener(s_Wl.registry, &s_RegistryListener, nullptr);
    s_Wl.displayRoundtrip(s_Wl.display);

    // do a roundtrip again to get remaining handles
    s_Wl.displayRoundtrip(s_Wl.display);

    if (!s_Wl.compositor || !s_Wl.xdgBase || !s_Wl.shm) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // create an input context
    s_Wl.inputContext = s_Wl.xkbContextNew(XKB_CONTEXT_NO_FLAGS);
    if (!s_Wl.inputContext) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // get the current theme
    s_Wl.cursorTheme = s_Wl.cursorThemeLoad(nullptr, 32, s_Wl.shm);
    if (!s_Wl.cursorTheme) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    wlCreateKeycodeTable();

    s_Video.display = (void*)s_Wl.display;
    return PAL_RESULT_SUCCESS;
}

void wlShutdownVideo()
{
    if (s_Wl.state) {
        s_Wl.xkbStateUnref(s_Wl.state);
        s_Wl.xkbKeymapUnref(s_Wl.keymap);
    }

    s_Wl.xkbContextUnref(s_Wl.inputContext);
    if (s_Wl.compositor) {
        // if compositor was found, all this will be as well
        // since we check all at init
        s_Wl.proxyDestroy((struct wl_proxy *)s_Wl.compositor);
        s_Wl.proxyDestroy((struct wl_proxy *)s_Wl.xdgBase);
        s_Wl.proxyDestroy((struct wl_proxy *)s_Wl.shm);
        s_Wl.proxyDestroy((struct wl_proxy *)s_Wl.seat);
    }

    if (!s_Video.platformInstance) {
        // opened by PAL
        s_Wl.displayDisconnect(s_Wl.display);
    }

    dlclose(s_Wl.libCursor);
    dlclose(s_Wl.xkbCommon);
    dlclose(s_Wl.libWaylandEgl);
    dlclose(s_Wl.handle);

    memset(&s_Wl, 0, sizeof(Wayland));
}

PalResult wlSetFBConfig(
    const int index,
    PalFBConfigBackend backend)
{
    if (backend == PAL_CONFIG_BACKEND_GLES ||
        backend == PAL_CONFIG_BACKEND_PAL_OPENGL) {
        return eglWlBackend(index);

    } else {
        return PAL_RESULT_INVALID_FBCONFIG_BACKEND;
    }    
}

void wlUpdateVideo()
{
    // flush pending requests
    s_Mouse.tmpScrollX = 0;
    s_Mouse.tmpScrollY = 0;

    // push key repeats
    // we only do this if the user wants key repeat events
    if (s_Keyboard.repeatKey != 0 && s_Video.eventDriver) {
        PalEventDriver* driver = s_Video.eventDriver;
        PalDispatchMode mode = PAL_DISPATCH_NONE;
        mode = palGetEventDispatchMode(driver, PAL_EVENT_KEYREPEAT);
        if (mode != PAL_DISPATCH_NONE) {
            // get now time and check with the key repeat time
            Uint64 now = getTime();
            if (now >= s_Keyboard.timer) {
                PalWindow* window = (PalWindow*)s_Wl.keyboardSurface;
                PalKeycode key = s_Keyboard.repeatKey;
                PalScancode scancode = s_Keyboard.repeatScancode;

                PalEvent event = {0};
                event.type = PAL_EVENT_KEYREPEAT;
                event.data = palPackUint32(key, scancode);
                event.data2 = palPackPointer(window);
                palPushEvent(driver, &event);
                s_Keyboard.timer += s_Keyboard.repeatRate;
            }
        }
    }

    while (s_Wl.prepareRead(s_Wl.display) != 0) {
        s_Wl.dispatchPending(s_Wl.display);
    }

    s_Wl.displayFlush(s_Wl.display);
    int fd = s_Wl.displayGetFd(s_Wl.display);
    struct pollfd pfd = {fd, POLLIN, 0};
    if (poll(&pfd, 1, 0) > 0) {
        // there are events ready to be read
        s_Wl.readEvents(s_Wl.display);

    } else {
        s_Wl.cancelRead(s_Wl.display);
    }

    // dispatch events that were read
    s_Wl.dispatchPending(s_Wl.display);
}

PalResult wlEnumerateMonitors(
    Int32* count,
    PalMonitor** outMonitors)
{
    if (outMonitors) {
        int index = 0;
        int maxCount = s_Video.maxMonitorData;
        for (int i = 0; i < maxCount && index < *count; i++) {
            if (s_Video.monitorData[i].used) {
                // found a monitor
                PalMonitor* monitor = s_Video.monitorData[index].monitor;
                outMonitors[index++] = monitor;
            }
        }
    }

    if (!outMonitors) {
        *count = s_Wl.monitorCount;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult wlGetPrimaryMonitor(PalMonitor** outMonitor)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult wlGetMonitorInfo(
    PalMonitor* monitor,
    PalMonitorInfo* info)
{
    MonitorData* monitorData = findMonitorData(monitor);
    if (!monitorData) {
        return PAL_RESULT_INVALID_MONITOR;
    }

    info->dpi = monitorData->dpi;
    info->x = monitorData->x;
    info->y = monitorData->y;
    info->width = monitorData->w;
    info->height = monitorData->h;
    info->refreshRate = monitorData->refreshRate;
    info->orientation = monitorData->orientation;

    info->primary = false; // no way to query
    strcpy(info->name, monitorData->name);

    return PAL_RESULT_SUCCESS;
}

PalResult wlEnumerateMonitorModes(
    PalMonitor* monitor,
    Int32* count,
    PalMonitorMode* modes)
{
    MonitorData* monitorData = findMonitorData(monitor);
    if (!monitorData) {
        return PAL_RESULT_INVALID_MONITOR;
    }

    if (modes && *count > 0) {
        PalMonitorMode* mode = &modes[0];
        mode->bpp = monitorData->mode.bpp;
        mode->width = monitorData->mode.width;
        mode->height = monitorData->mode.height;
        mode->refreshRate = monitorData->mode.refreshRate;
    }

    if (!modes) {
        *count = 1; // wayland only gives the active mode
    }

    return PAL_RESULT_SUCCESS;
}

PalResult wlGetCurrentMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    MonitorData* monitorData = findMonitorData(monitor);
    if (!monitorData) {
        return PAL_RESULT_INVALID_MONITOR;
    }

    // this is the same as the current mode 
    mode->bpp = monitorData->mode.bpp;
    mode->width = monitorData->mode.width;
    mode->height = monitorData->mode.height;
    mode->refreshRate = monitorData->mode.refreshRate;

    return PAL_RESULT_SUCCESS;
}

PalResult wlSetMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult wlValidateMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult wlSetMonitorOrientation(
    PalMonitor* monitor,
    PalOrientation orientation)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult wlCreateWindow(
    const PalWindowCreateInfo* info,
    PalWindow** outWindow)
{
    struct wl_surface* surface = nullptr;
    struct xdg_surface* xdgSurface = nullptr;
    struct xdg_toplevel* xdgToplevel = nullptr;

    if (info->style & PAL_WINDOW_STYLE_TOPMOST) {
        return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
    }

    if (info->style & PAL_WINDOW_STYLE_TRANSPARENT) {
        return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
    }

    if (info->style & PAL_WINDOW_STYLE_TOOL) {
        return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
    }

    if (!(info->style & PAL_WINDOW_STYLE_BORDERLESS)) {
        if (!s_Wl.decorationManager) {
            // user wants decorated window but its not supported
            return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
        }
    }

    WindowData* data = getFreeWindowData();
    if (!data) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(data, 0, sizeof(WindowData));
    data->used = true;
    data->focused = false;

    // create surface
    surface = wlCompositorCreateSurface(s_Wl.compositor);
    if (!surface) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    wlSurfaceAddListener(surface, &surfaceListener, data);
    xdgSurface = xdgWmBaseGetXdgSurface(s_Wl.xdgBase, surface);
    if (!xdgSurface) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    xdgToplevel = xdgSurfaceGetToplevel(xdgSurface);
    if (!xdgSurface) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // set APP id
    const char* appID = getenv("RESOURCE_CLASS");
    if (!appID || strlen(appID) == 0) {
        appID = s_Video.className;
    }

    const char* title = info->title;
    if (!title) {
        title = "";
    }

    xdgToplevelSetTitle(xdgToplevel, title);
    xdgToplevelSetAppId(xdgToplevel, appID);

    xdgToplevelAddListener(xdgToplevel, &xdgToplevelListener, data);
    xdgSurfaceAddListener(xdgSurface, &xdgSurfaceListener, data);

    // decorated window
    if (!(info->style & PAL_WINDOW_STYLE_BORDERLESS)) {
        struct zxdg_toplevel_decoration_v1* decoration = nullptr;
        decoration =
            zxdgGetToplevelDecoration(s_Wl.decorationManager, xdgToplevel);

        zxdgToplevelDecorationV1AddListener(
            decoration, 
            &decorationListener, 
            surface);

        zxdgToplevelDecorationV1SetMode(decoration, 2);
        data->decoration = decoration;
    }

    data->skipState = true;
    data->skipConfigure = true;
    wlSurfaceCommit(surface);
    s_Wl.displayRoundtrip(s_Wl.display);

    if (info->maximized && info->show) {
        // we need the maximized size the compositor will use
        // and use that to create the buffer
        xdgToplevelSetMaximized(xdgToplevel);
        wlSurfaceCommit(surface);
        s_Wl.displayRoundtrip(s_Wl.display);
        data->state = PAL_WINDOW_STATE_MAXIMIZED;

    } else {
        data->w = info->width;
        data->h = info->height;
    }

    data->xdgSurface = xdgSurface;
    data->xdgToplevel = xdgToplevel;
    data->window = (PalWindow*)surface;
    data->buffer = nullptr;
    data->isAttached = false;
    data->state = PAL_WINDOW_STATE_RESTORED;

    // minimize
    // This is just a requeest, the compositor might ignore it
    if (info->minimized) {
        xdgToplevelSetMinimized(xdgToplevel);
        wlSurfaceCommit(surface);
        data->state = PAL_WINDOW_STATE_MINIMIZED;
    }

    if (s_Wl.eglFBConfig) {
        data->eglWindow = s_Wl.eglWindowCreate(surface, data->w, data->h);
        if (!data->eglWindow) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }

    } else {
        // create a white buffer for the surface
        struct wl_buffer* buffer = nullptr;
        buffer = createShmBuffer(data->w, data->h, nullptr, false);
        if (!buffer) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        wlSurfaceAttach(surface, buffer, 0, 0);
        wlSurfaceDamageBuffer(surface, 0, 0, data->w, data->h);
        wlSurfaceCommit(surface);
        data->buffer = buffer;
    }

    struct wl_region* region = wlCompositorCreateRegion(s_Wl.compositor);
    if (region) {
        wlRegionAdd(region, 0, 0, data->w, data->h);
        wlSurfaceSetOpaqueRegion(surface, region);
        wlRegionDestroy(region);
        wlSurfaceCommit(surface);
    }

    s_Wl.displayRoundtrip(s_Wl.display);
    if (!s_Wl.decorationManager && s_Video.eventDriver) {
        PalEventDriver* driver = s_Video.eventDriver;
        PalDispatchMode mode = PAL_DISPATCH_NONE;
        PalEventType type = PAL_EVENT_WINDOW_DECORATION_MODE;
        mode = palGetEventDispatchMode(driver, type);

        if (mode != PAL_DISPATCH_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data = PAL_DECORATION_MODE_CLIENT_SIDE;
            event.data2 = palPackPointer(data->window);
            palPushEvent(driver, &event);
        }
    }

    // Since wayland does not have a way to set unique data
    // to a surface without taking control from users
    // we might implement a simple hash map to do that
    // but at the moment a linear search is fine
    // FIXME: Implement a window hash map

    data->skipState = false;
    data->skipConfigure = false;
    *outWindow = data->window;
    return PAL_RESULT_SUCCESS;
}

void wlDestroyWindow(PalWindow* window)
{
    WindowData* data = findWindowData(window);
    if (!data || (data && data->isAttached)) {
        return;
    }

    if (data->decoration) {
        zxdgToplevelDecorationV1Destroy(data->decoration);
    }

    if (data->eglWindow) {
        s_Wl.eglWindowDestroy(data->eglWindow);
    } else {
        wlBufferDestroy(data->buffer);
    }

    xdgToplevelDestroy(data->xdgToplevel);
    xdgSurfaceDestroy(data->xdgSurface);
    wlSurfaceDestroy((struct wl_surface*)window);
    data->used = false;
}

PalResult wlMinimizeWindow(PalWindow* window)
{
    WindowData* data = findWindowData(window);
    if (!data) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    xdgToplevelSetMinimized(data->xdgToplevel);
    return PAL_RESULT_SUCCESS;
}

PalResult wlMaximizeWindow(PalWindow* window)
{
    WindowData* data = findWindowData(window);
    if (!data) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    xdgToplevelSetMaximized(data->xdgToplevel);
    return PAL_RESULT_SUCCESS;
}

PalResult wlRestoreWindow(PalWindow* window)
{
    WindowData* data = findWindowData(window);
    if (!data) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    // we can only restore from a maximized state
    xdgToplevelUnsetMaximized(data->xdgToplevel);
    return PAL_RESULT_SUCCESS;
}

PalResult wlShowWindow(PalWindow* window)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult wlHideWindow(PalWindow* window)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult wlFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult wlGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* outStyle)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult wlGetWindowMonitor(
    PalWindow* window,
    PalMonitor** outMonitor)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult wlGetWindowTitle(
    PalWindow* window,
    Uint64 bufferSize,
    Uint64* outSize,
    char* outBuffer)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult wlGetWindowPos(
    PalWindow* window,
    Int32* x,
    Int32* y)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult wlGetWindowSize(
    PalWindow* window,
    Uint32* width,
    Uint32* height)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult wlGetWindowState(
    PalWindow* window,
    PalWindowState* outState)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

bool wlIsWindowVisible(PalWindow* window)
{
    return false;
}

PalWindow* wlGetFocusWindow()
{
    // Wayland does not let client query focused window
    return nullptr;
}

PalWindowHandleInfo wlGetWindowHandleInfo(PalWindow* window)
{
    PalWindowHandleInfo info;
    info.nativeDisplay = (void*)s_Wl.display;
    info.nativeWindow = (void*)window;
    return info;
}

PalWindowHandleInfoEx wlGetWindowHandleInfoEx(PalWindow* window)
{
    PalWindowHandleInfoEx info = {0};
    WindowData* data = findWindowData(window);
    if (data) {
        info.nativeDisplay = (void*)s_Wl.display;
        info.nativeWindow = (void*)window;
        info.nativeHandle1 = data->xdgSurface;
        info.nativeHandle2 = data->xdgToplevel;
        info.nativeHandle3 = data->eglWindow;
    }

    return info;
}

PalResult wlSetWindowOpacity(
    PalWindow* window,
    float opacity)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult wlSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult wlSetWindowTitle(
    PalWindow* window,
    const char* title)
{
    WindowData* data = findWindowData(window);
    if (!data) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    xdgToplevelSetTitle(data->xdgToplevel, title);
    s_Wl.displayFlush(s_Wl.display);
    return PAL_RESULT_SUCCESS;
}

PalResult wlSetWindowPos(
    PalWindow* window,
    Int32 x,
    Int32 y)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult wlSetWindowSize(
    PalWindow* window,
    Uint32 width,
    Uint32 height)
{
    WindowData* data = findWindowData(window);
    if (!data) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    xdgToplevelSetMinSize(data->xdgToplevel, width, height);
    xdgToplevelSetMaxSize(data->xdgToplevel, width, height);
    wlSurfaceCommit((struct wl_surface*)window);
    return PAL_RESULT_SUCCESS;
}

PalResult wlSetFocusWindow(PalWindow* window)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult wlCreateIcon(
    const PalIconCreateInfo* info,
    PalIcon** outIcon)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

void wlDestroyIcon(PalIcon* icon)
{
    return;
}

PalResult wlSetWindowIcon(
    PalWindow* window,
    PalIcon* icon)
{
    return PAL_RESULT_THREAD_FEATURE_NOT_SUPPORTED;
}

PalResult wlCreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** outCursor)
{
    WaylandCursor* cursor = nullptr;
    cursor = palAllocate(s_Video.allocator, sizeof(WaylandCursor), 0);
    if (!cursor) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    cursor->surface = wlCompositorCreateSurface(s_Wl.compositor);
    if (!cursor->surface) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    cursor->buffer =
        createShmBuffer(info->width, info->height, info->pixels, true);

    if (!cursor->buffer) {
        return PAL_RESULT_PLATFORM_FAILURE;
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
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    WaylandCursor* cursor = nullptr;
    cursor = palAllocate(s_Video.allocator, sizeof(WaylandCursor), 0);
    if (!cursor) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    cursor->surface = wlCompositorCreateSurface(s_Wl.compositor);
    if (!cursor->surface) {
        return PAL_RESULT_PLATFORM_FAILURE;
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

void wlShowCursor(bool show)
{
    // not supported
    return;
}

PalResult wlClipCursor(
    PalWindow* window,
    bool clip)
{
    if (!(s_Video.features & PAL_VIDEO_FEATURE_CLIP_CURSOR)) {
        return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult wlGetCursorPos(
    PalWindow* window,
    Int32* x,
    Int32* y)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult wlSetCursorPos(
    PalWindow* window,
    Int32 x,
    Int32 y)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult wlSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor)
{
    WindowData* data = findWindowData(window);
    if (!data) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    data->cursor = cursor;
    return PAL_RESULT_SUCCESS;
}

void* wlGetInstance()
{
    return (void*)s_Wl.display;
}

PalResult wlAttachWindow(
    void* windowHandle,
    PalWindow** outWindow)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

PalResult wlDetachWindow(
    PalWindow* window,
    void** outWindowHandle)
{
    return PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED;
}

static Backend s_wlBackend = {
    .shutdownVideo = wlShutdownVideo,
    .updateVideo = wlUpdateVideo,
    .setFBConfig = wlSetFBConfig,
    .enumerateMonitors = wlEnumerateMonitors,
    .getMonitorInfo = wlGetMonitorInfo,
    .getPrimaryMonitor = wlGetPrimaryMonitor,
    .enumerateMonitorModes = wlEnumerateMonitorModes,
    .getCurrentMonitorMode = wlGetCurrentMonitorMode,
    .setMonitorMode = wlSetMonitorMode,
    .validateMonitorMode = wlValidateMonitorMode,
    .setMonitorOrientation = wlSetMonitorOrientation,

    .createWindow = wlCreateWindow,
    .destroyWindow = wlDestroyWindow,
    .maximizeWindow = wlMaximizeWindow,
    .minimizeWindow = wlMinimizeWindow,
    .restoreWindow = wlRestoreWindow,
    .showWindow = wlShowWindow,
    .hideWindow = wlHideWindow,
    .flashWindow = wlFlashWindow,
    .getWindowStyle = wlGetWindowStyle,
    .getWindowMonitor = wlGetWindowMonitor,
    .getWindowTitle = wlGetWindowTitle,
    .getWindowPos = wlGetWindowPos,
    .getWindowSize = wlGetWindowSize,
    .getWindowState = wlGetWindowState,
    .isWindowVisible = wlIsWindowVisible,
    .getFocusWindow = wlGetFocusWindow,
    .getWindowHandleInfo = wlGetWindowHandleInfo,
    .getWindowHandleInfoEx = wlGetWindowHandleInfoEx,
    .setWindowOpacity = wlSetWindowOpacity,
    .setWindowStyle = wlSetWindowStyle,
    .setWindowTitle = wlSetWindowTitle,
    .setWindowPos = wlSetWindowPos,
    .setWindowSize = wlSetWindowSize,
    .setFocusWindow = wlSetFocusWindow,

    .createIcon = wlCreateIcon,
    .destroyIcon = wlDestroyIcon,
    .setWindowIcon = wlSetWindowIcon,

    .createCursor = wlCreateCursor,
    .createCursorFrom = wlCreateCursorFrom,
    .destroyCursor = wlDestroyCursor,
    .showCursor = wlShowCursor,
    .clipCursor = wlClipCursor,
    .getCursorPos = wlGetCursorPos,
    .setCursorPos = wlSetCursorPos,
    .setWindowCursor = wlSetWindowCursor,

    .attachWindow = wlAttachWindow,
    .detachWindow = wlDetachWindow};

#endif // PAL_HAS_WAYLAND
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
    s_Video.maxWindowData = 32;  // initial size

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

    s_Video.className = "PAL";
    if (x11) {
#if PAL_HAS_X11
        PalResult ret = xInitVideo();
        if (ret != PAL_RESULT_SUCCESS) {
            return ret;
        }
        s_Video.backend = &s_XBackend;
#else 
    return PAL_RESULT_PLATFORM_FAILURE;
#endif // PAL_HAS_X11

    } else {
#if PAL_HAS_WAYLAND
        PalResult ret = wlInitVideo();
        if (ret != PAL_RESULT_SUCCESS) {
            return ret;
        }
        s_Video.backend = &s_wlBackend;
#else
    return PAL_RESULT_PLATFORM_FAILURE;
#endif // PAL_HAS_WAYLAND
    }

    createScancodeTable();

    // we load EGL as well
    s_Egl.handle = dlopen("libEGL.so", RTLD_LAZY);
    if (s_Egl.handle) {
        eglGetProcAddressFn load = nullptr;
        load = (eglGetProcAddressFn)dlsym(s_Egl.handle, "eglGetProcAddress");

        s_Egl.eglInitialize = (eglInitializeFn)load("eglInitialize");
        s_Egl.eglTerminate = (eglTerminateFn)load("eglTerminate");
        s_Egl.eglGetDisplay = (eglGetDisplayFn)load("eglGetDisplay");
        s_Egl.eglChooseConfig = (eglChooseConfigFn)load("eglChooseConfig");
        s_Egl.eglGetError = (eglGetErrorFn)load("eglGetError");
        s_Egl.eglBindAPI = (eglBindAPIFn)load("eglBindAPI");
        s_Egl.eglGetConfigs = (eglGetConfigsFn)load("eglGetConfigs");

        // clang-format off
        s_Egl.eglGetConfigAttrib = (eglGetConfigAttribFn)load("eglGetConfigAttrib");
        // clang-format on
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

        if (s_Egl.handle) {
            dlclose(s_Egl.handle);
        }

        s_Video.platformInstance = nullptr;
        s_Video.display = nullptr;
        memset(&s_Keyboard, 0, sizeof(Keyboard));
        memset(&s_Mouse, 0, sizeof(Mouse));
        s_Video.initialized = false;
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

PalVideoFeatures64 PAL_CALL palGetVideoFeaturesEx()
{
    if (!s_Video.initialized) {
        return 0;
    }

    return s_Video.features64;
}

PalResult PAL_CALL palSetFBConfig(
    const int index,
    PalFBConfigBackend backend)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    // X11 and wayland can only used GLX and EGL
    if (backend == PAL_CONFIG_BACKEND_WGL) {
        return PAL_RESULT_INVALID_FBCONFIG_BACKEND;
    }

    return s_Video.backend->setFBConfig(index, backend);
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

    // clang-format off

    PalResult ret = s_Video.backend->enumerateMonitorModes(monitor, count, modes);
    // clang-format on

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

    return s_Video.backend->createWindow(info, outWindow);
}

void PAL_CALL palDestroyWindow(PalWindow* window)
{
    if (s_Video.initialized && window) {
        return s_Video.backend->destroyWindow(window);
    }
}

PalResult PAL_CALL palMinimizeWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->maximizeWindow(window);
}

PalResult PAL_CALL palMaximizeWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->minimizeWindow(window);
}

PalResult PAL_CALL palRestoreWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->restoreWindow(window);
}

PalResult PAL_CALL palShowWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->showWindow(window);
}

PalResult PAL_CALL palHideWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->hideWindow(window);
}

PalResult PAL_CALL palFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window || !info) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->flashWindow(window, info);
}

PalResult PAL_CALL palGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* outStyle)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window || !outStyle) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->getWindowStyle(window, outStyle);
}

PalResult PAL_CALL palGetWindowMonitor(
    PalWindow* window,
    PalMonitor** outMonitor)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window || !outMonitor) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->getWindowMonitor(window, outMonitor);
}

PalResult PAL_CALL palGetWindowTitle(
    PalWindow* window,
    Uint64 bufferSize,
    Uint64* outSize,
    char* outBuffer)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window || !outBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    // clang-format off
    return s_Video.backend->getWindowTitle(
        window, 
        bufferSize, 
        outSize, 
        outBuffer);
    // clang-format on
}

PalResult PAL_CALL palGetWindowPos(
    PalWindow* window,
    Int32* x,
    Int32* y)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->getWindowPos(window, x, y);
}

PalResult PAL_CALL palGetWindowSize(
    PalWindow* window,
    Uint32* width,
    Uint32* height)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->getWindowSize(window, width, height);
}

PalResult PAL_CALL palGetWindowState(
    PalWindow* window,
    PalWindowState* outState)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window || !outState) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->getWindowState(window, outState);
}

const bool* PAL_CALL palGetKeycodeState()
{
    if (!s_Video.initialized) {
        return nullptr;
    }
    return s_Keyboard.keycodeState;
}

const bool* PAL_CALL palGetScancodeState()
{
    if (!s_Video.initialized) {
        return nullptr;
    }
    return s_Keyboard.scancodeState;
}

const bool* PAL_CALL palGetMouseState()
{
    if (!s_Video.initialized) {
        return nullptr;
    }
    return s_Mouse.state;
}

void PAL_CALL palGetMouseDelta(
    Int32* dx,
    Int32* dy)
{
    if (!s_Video.initialized) {
        return;
    }

    if (dx) {
        *dx = s_Mouse.dx;
    }

    if (dy) {
        *dy = s_Mouse.dy;
    }
}

void PAL_CALL palGetMouseWheelDelta(
    Int32* dx,
    Int32* dy)
{
    if (!s_Video.initialized) {
        return;
    }

    if (dx) {
        *dx = s_Mouse.WheelX;
    }

    if (dy) {
        *dy = s_Mouse.WheelY;
    }
}

void PAL_CALL palGetRawMouseWheelDelta(
    float* dx,
    float* dy)
{
    if (!s_Video.initialized) {
        return;
    }

    if (dx) {
        *dx = (float)s_Mouse.tmpScrollX;
    }

    if (dy) {
        *dy = (float)s_Mouse.tmpScrollY;
    }
}

bool PAL_CALL palIsWindowVisible(PalWindow* window)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->isWindowVisible(window);
}

PalWindow* PAL_CALL palGetFocusWindow()
{
    if (!s_Video.initialized) {
        return nullptr;
    }

    return s_Video.backend->getFocusWindow();
}

PalWindowHandleInfo PAL_CALL palGetWindowHandleInfo(PalWindow* window)
{
    if (s_Video.initialized) {
        return s_Video.backend->getWindowHandleInfo(window);
    }
}

PalWindowHandleInfoEx PAL_CALL palGetWindowHandleInfoEx(PalWindow* w)
{
    if (s_Video.initialized) {
        return s_Video.backend->getWindowHandleInfoEx(w);
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

PalResult PAL_CALL palSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->setWindowStyle(window, style);
}

PalResult PAL_CALL palSetWindowTitle(
    PalWindow* window,
    const char* title)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window || !title) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->setWindowTitle(window, title);
}

PalResult PAL_CALL palSetWindowPos(
    PalWindow* window,
    Int32 x,
    Int32 y)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->setWindowPos(window, x, y);
}

PalResult PAL_CALL palSetWindowSize(
    PalWindow* window,
    Uint32 width,
    Uint32 height)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->setWindowSize(window, width, height);
}

PalResult PAL_CALL palSetFocusWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->setFocusWindow(window);
}

// ==================================================
// Icon
// ==================================================

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

// ==================================================
// Cursor
// ==================================================

PalResult PAL_CALL palCreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** outCursor)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!info || !outCursor) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->createCursor(info, outCursor);
}

PalResult PAL_CALL palCreateCursorFrom(
    PalCursorType type,
    PalCursor** outCursor)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!outCursor) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->createCursorFrom(type, outCursor);
}

void PAL_CALL palDestroyCursor(PalCursor* cursor)
{
    if (s_Video.initialized && cursor) {
        s_Video.backend->destroyCursor(cursor);
    }
}

void PAL_CALL palShowCursor(bool show)
{
    if (s_Video.initialized) {
        s_Video.backend->showCursor(show);
    }
}

PalResult PAL_CALL palClipCursor(
    PalWindow* window,
    bool clip)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->clipCursor(window, clip);
}

PalResult PAL_CALL palGetCursorPos(
    PalWindow* window,
    Int32* x,
    Int32* y)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->getCursorPos(window, x, y);
}

PalResult PAL_CALL palSetCursorPos(
    PalWindow* window,
    Int32 x,
    Int32 y)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->setCursorPos(window, x, y);
}

PalResult PAL_CALL palSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->setWindowCursor(window, cursor);
}

void* PAL_CALL palGetInstance()
{
    if (!s_Video.initialized) {
        return nullptr;
    }

    return s_Video.display;
}

PalResult PAL_CALL palAttachWindow(
    void* windowHandle,
    PalWindow** outWindow)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!windowHandle) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->attachWindow(windowHandle, outWindow);
}

PalResult PAL_CALL palDetachWindow(
    PalWindow* window,
    void** outWindowHandle)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_Video.backend->detachWindow(window, outWindowHandle);
}

void PAL_CALL palSetPreferredInstance(void* instance)
{
    if (!s_Video.initialized && instance) {
        s_Video.platformInstance = instance;
    }
}