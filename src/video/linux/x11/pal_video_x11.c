
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef __linux__
#if PAL_HAS_X11_BACKEND == 1

#include "pal_x11.h"
#include "pal_shared.h"
#include <dlfcn.h>
#include <math.h>

X11 s_X11 = {0};
X11Atoms s_X11Atoms = {0};

static PalResult glxBackend(const int index)
{
    // user choose GLX FBConfig backend
    if (!s_X11.glxHandle) {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    int count = 0;
    GLXFBConfig* configs = s_X11.glxGetFBConfigs(s_X11.display, s_X11.screen, &count);
    GLXFBConfig fbConfig = configs[index];
    if (!fbConfig) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    // get a matching visual
    XVisualInfo* visualInfo = s_X11.glxGetVisualFromFBConfig(s_X11.display, fbConfig);
    if (!visualInfo) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    s_X11.visualInfo = visualInfo;
    return PAL_RESULT_SUCCESS;
}

static PalResult eglXBackend(int index)
{
    // user choose EGL FBConfig backend
    if (!s_Egl.handle) {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    EGLDisplay display = EGL_NO_DISPLAY;
    display = s_Egl.eglGetDisplay((EGLNativeDisplayType)s_X11.display);
    if (display == EGL_NO_DISPLAY) {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    EGLint numConfigs = 0;
    if (!s_Egl.eglGetConfigs(display, nullptr, 0, &numConfigs)) {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    EGLint configSize = sizeof(EGLConfig) * numConfigs;
    EGLConfig* eglConfigs = palAllocate(s_Video.allocator, configSize, 0);
    if (!eglConfigs) {
        return palMakeResult(
            PAL_RESULT_OUT_OF_MEMORY, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    s_Egl.eglGetConfigs(display, eglConfigs, numConfigs, &numConfigs);
    EGLConfig config = eglConfigs[index];

    // we get a visual info from the config
    EGLint visualID;
    s_Egl.eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &visualID);
    if (visualID == 0) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    int numVisuals = 0;
    XVisualInfo tmp;
    tmp.visualid = visualID;

    // get a matching visual info
    XVisualInfo* visualInfo = s_X11.getVisualInfo(s_X11.display, VisualIDMask, &tmp, &numVisuals);
    if (!visualInfo) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    s_X11.visualInfo = visualInfo;
    palFree(s_Video.allocator, eglConfigs);
    return PAL_RESULT_SUCCESS;
}

RRMode findMode(
    XRRScreenResources* resources,
    const PalMonitorMode* mode)
{
    for (int i = 0; i < resources->nmode; ++i) {
        XRRModeInfo* info = &resources->modes[i];

        double tmp = (double)info->hTotal * (double)info->vTotal;
        double rate = (double)info->dotClock / tmp;

        // compare with width, height and refresh rate
        if (info->width == mode->width && info->height == mode->height &&
            (uint32_t)(rate + 0.5) == mode->refreshRate) {
            return info->id;
        }
    }
    return None;
}

static void checkFeatures()
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
            s_X11Atoms.unicodeTitle = PAL_TRUE;
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

    features |= PAL_VIDEO_FEATURE_WINDOW_SET_ICON;
    features |= PAL_VIDEO_FEATURE_TOPMOST_WINDOW;
    features |= PAL_VIDEO_FEATURE_DECORATED_WINDOW;
    features |= PAL_VIDEO_FEATURE_MONITOR_GET_PRIMARY;
    features |= PAL_VIDEO_FEATURE_FOREIGN_WINDOWS;
    features |= PAL_VIDEO_FEATURE_WINDOW_SET_CURSOR;

    s_Video.features = features;
    s_X11.free(supportedAtoms);
}

static PalWindowState queryWindowState(Window xWin)
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

static void cacheMonitors()
{
    resetMonitorData();
    XRRScreenResources* resources = nullptr;
    resources = s_X11.getScreenResources(s_X11.display, s_X11.root);

    for (int i = 0; i < resources->noutput; ++i) {
        RROutput output = resources->outputs[i];
        XRROutputInfo* info = s_X11.getOutputInfo(s_X11.display, resources, output);
        if (info->connection == RR_Connected && info->crtc != None) {
            // get monitor data and update info
            PalMonitor* monitor = TO_PAL_HANDLE(PalMonitor, output);
            MonitorData* data = nullptr;
            data = getFreeMonitorData();
            if (!data) {
                return;
            }

            data->monitor = monitor;
            XRRCrtcInfo* crtc = s_X11.getCrtcInfo(s_X11.display, resources, info->crtc);

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

            data->dpi = (uint32_t)(closest * 96.0f);
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

static int getWindowMonitorDPI(WindowData* data)
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

void sendWMEvent(
    Window window,
    Atom type,
    long a,
    long b,
    long c,
    long d,
    PalBool add)
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

static void createKeycodeTable()
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

PalResult xInitVideo()
{
    // load X11 library
    s_X11.handle = dlopen("libX11.so", RTLD_LAZY);
    if (!s_X11.handle) {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    // libXCursor is needed
    s_X11.libCursor = dlopen("libXcursor.so", RTLD_LAZY);
    if (!s_X11.libCursor) {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
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
    // clang-format on

    // X11 server
    if (s_Video.platformInstance) {
        s_X11.display = (Display*)s_Video.platformInstance;

    } else {
        s_X11.display = s_X11.openDisplay(nullptr);
        s_Video.platformInstance = nullptr;
    }

    if (!s_X11.display) {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    s_X11.root = DefaultRootWindow(s_X11.display);
    s_X11.screen = DefaultScreen(s_X11.display);

    checkFeatures();

    // subscribe for monitor events
    s_X11.selectRRInput(s_X11.display, s_X11.root, RRScreenChangeNotifyMask | RRNotify);

    int eventBase, errorBase = 0;
    s_X11.queryRRExtension(s_X11.display, &eventBase, &errorBase);
    s_X11.rrEventBase = eventBase;
    s_X11.skipScreenEvent = PAL_TRUE;

    s_X11.dataID = (XContext)s_X11.uniqueContext();
    resetMonitorData();

    s_X11.monitorCount = 0;
    cacheMonitors();

    // since X11 supports both EGL and GLX
    // we try to load them and resolve the needed functions

    // we load GLX
    s_X11.glxHandle = dlopen("libGL.so.1", RTLD_LAZY);
    if (s_X11.glxHandle) {
        GLXGetProcAddressFn load = nullptr;
        load = (GLXGetProcAddressFn)dlsym(s_X11.glxHandle, "glXGetProcAddress");
        s_X11.glxGetFBConfigs = (GLXGetFBConfigsFn)load("glXGetFBConfigs");
        s_X11.glxGetFBConfigAttrib = (GLXGetFBConfigAttribFn)load("glXGetFBConfigAttrib");
        s_X11.glxGetVisualFromFBConfig =
            (GLXGetVisualFromFBConfigFn)load("glXGetVisualFromFBConfig");
    }

    createKeycodeTable();

    // disable auto key repeats
    int supported;
    s_X11.setDetectableAutoRepeat(s_X11.display, True, &supported);
    
    // create an input method
    s_X11.setLocaleModifiers("");
    s_X11.im = s_X11.openIM(s_X11.display, nullptr, nullptr, nullptr);
    if (s_X11.im == None) {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    s_Video.display = (void*)s_X11.display;
    return PAL_RESULT_SUCCESS;
}

void xShutdownVideo()
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

    } else if (backend == PAL_CONFIG_BACKEND_EGL || backend == PAL_CONFIG_BACKEND_PAL_OPENGL) {
        return eglXBackend(index);

    } else {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }
}

void xUpdateVideo()
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
                    data->skipConfigure = PAL_FALSE;
                    data->w = event.xconfigure.width;
                    data->h = event.xconfigure.height;
                    data->x = event.xconfigure.x;
                    data->y = event.xconfigure.y;
                    break;
                }

                // real configure event
                if (s_Video.eventDriver) {
                    // check if its a resize event
                    if (data->w != event.xconfigure.width || data->h != event.xconfigure.height) {
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
                            data->skipIfAttached = PAL_FALSE;
                            break;
                        }
                    }

                    // check if its a move event
                    if (data->x != event.xconfigure.x || data->y != event.xconfigure.y) {
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
                        int monitorDPI = getWindowMonitorDPI(data);
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
                        event.data = PAL_TRUE;
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
                        event.data = PAL_FALSE;
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
                    state = queryWindowState(event.xproperty.window);
                    if (state != data->state) {
                        data->state = state;

                        // skip the first state event
                        if (data->skipState) {
                            data->skipState = PAL_FALSE;
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
                    s_X11.skipScreenEvent = PAL_FALSE;
                    break;
                }

                // store old monitor count
                int oldCount = s_X11.monitorCount;
                s_X11.monitorCount = 0;
                cacheMonitors();

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
                PalBool pressed = (event.xbutton.type == ButtonPress);
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
                        event.data = palPackUint32(button, NULL_BUTTON_SERIAL);
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
                    mode = palGetEventDispatchMode(driver, PAL_EVENT_MOUSE_WHEEL);
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
                PalBool pressed = (event.xbutton.type == KeyPress);
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
                    keycode = (PalKeycode)(uint32_t)scancode;
                }

                // If we got a keySym but its not mapped into our keycode array
                // we do a direct cast as well
                if (keycode == PAL_KEYCODE_UNKNOWN) {
                    keycode = (PalKeycode)(uint32_t)scancode;
                }

                // update our keyboard and mouse state to handle key repeat
                PalBool repeat = s_Keyboard.keycodeState[keycode];
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

                    uint32_t codepoint = 0;
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

void* xGetInstance()
{
    return (void*)s_X11.display;
}

#endif // PAL_HAS_X11_BACKEND
#endif // __linux__