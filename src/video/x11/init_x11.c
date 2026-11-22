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

#ifdef PAL_HAS_X11_BACKEND

#define NULL_BUTTON_SERIAL 0xffffffffU

#if PAL_HAS_EVENT_MODULE
#include "pal2/core/pack.h"
#include "pal2/event/driver.h"
#endif // PAL_HAS_EVENT_MODULE

#include "pal2/core/memory.h"
#include "pal2/video/init.h"
#include "video/platform.h"
#include "video/video_egl.h"
#include "video_glx.h"
#include "x11_helper.h"

#include <X11/Xatom.h>
#include <errno.h>
#include <math.h>
#include <string.h>

X11 g_X11 = {0};
X11Atoms g_X11Atoms = {0};
X11Keyboard g_X11Keyboard = {0};
X11Mouse g_X11Mouse = {0};
static PalVideoDriver s_X11Driver = {0};

/* =========================================================================
 * Helper
 * ========================================================================= */

static void loadProcs(PalBool loadAll)
{
    // clang-format off

    g_X11.openDisplay = (XOpenDisplayFn)palGetSymbol(
        g_X11.libx11,
        "XOpenDisplay");

    g_X11.closeDisplay = (XCloseDisplayFn)palGetSymbol(
        g_X11.libx11,
        "XCloseDisplay");

    g_X11.internAtom = (XInternAtomFn)palGetSymbol(
        g_X11.libx11,
        "XInternAtom");

    g_X11.getWindowProperty = (XGetWindowPropertyFn)palGetSymbol(
        g_X11.libx11,
        "XGetWindowProperty");

    g_X11.deleteProperty = (XDeletePropertyFn)palGetSymbol(
        g_X11.libx11,
        "XDeleteProperty");

    g_X11.getSelectionOwner = (XGetSelectionOwnerFn)palGetSymbol(
        g_X11.libx11,
        "XGetSelectionOwner");

    g_X11.free = (XFreeFn)palGetSymbol(
        g_X11.libx11,
        "XFree");

    if (!loadAll) {
        return;
    }

    g_X11.getWindowAttributes = (XGetWindowAttributesFn)palGetSymbol(
        g_X11.libx11,
        "XGetWindowAttributes");

    g_X11.translateCoordinates = (XTranslateCoordinatesFn)palGetSymbol(
        g_X11.libx11,
        "XTranslateCoordinates");

    g_X11.setCrtcConfig = (XRRSetCrtcConfigFn)palGetSymbol(
        g_X11.libx11,
        "XRRSetCrtcConfig");

    g_X11.freeColormap = (XFreeColormapFn)palGetSymbol(
        g_X11.libx11,
        "XFreeColormap");

    g_X11.storeName = (XStoreNameFn)palGetSymbol(
        g_X11.libx11,
        "XStoreName");

    g_X11.changeProperty = (XChangePropertyFn)palGetSymbol(
        g_X11.libx11,
        "XChangeProperty");

    g_X11.flush = (XFlushFn)palGetSymbol(
        g_X11.libx11,
        "XFlush");

    g_X11.createColormap = (XCreateColormapFn)palGetSymbol(
        g_X11.libx11,
        "XCreateColormap");

    g_X11.mapWindow = (XMapWindowFn)palGetSymbol(
        g_X11.libx11,
        "XMapWindow");

    g_X11.unmapWindow = (XUnmapWindowFn)palGetSymbol(
        g_X11.libx11,
        "XUnmapWindow");

    g_X11.createWindow = (XCreateWindowFn)palGetSymbol(
        g_X11.libx11,
        "XCreateWindow");

    g_X11.destroyWindow = (XDestroyWindowFn)palGetSymbol(
        g_X11.libx11,
        "XDestroyWindow");

    g_X11.matchVisualInfo = (XMatchVisualInfoFn)palGetSymbol(
        g_X11.libx11,
        "XMatchVisualInfo");

    g_X11.pending = (XPendingFn)palGetSymbol(
        g_X11.libx11,
        "XPending");

    g_X11.setWMProtocols = (XSetWMProtocolsFn)palGetSymbol(
        g_X11.libx11,
        "XSetWMProtocols");

    g_X11.nextEvent = (XNextEventFn)palGetSymbol(
        g_X11.libx11,
        "XNextEvent");

    g_X11.setWMNormalHints = (XSetWMNormalHintsFn)palGetSymbol(
        g_X11.libx11,
        "XSetWMNormalHints");

    g_X11.getWMNormalHints = (XGetWMNormalHintsFn)palGetSymbol(
        g_X11.libx11,
        "XGetWMNormalHints");

    g_X11.sendEvent = (XSendEventFn)palGetSymbol(
        g_X11.libx11,
        "XSendEvent");

    g_X11.moveWindow = (XMoveWindowFn)palGetSymbol(
        g_X11.libx11,
        "XMoveWindow");

    g_X11.resizeWindow = (XResizeWindowFn)palGetSymbol(
        g_X11.libx11,
        "XResizeWindow");

    g_X11.iconifyWindow = (XIconifyWindowFn)palGetSymbol(
        g_X11.libx11,
        "XIconifyWindow");

    g_X11.setErrorHandler = (XSetErrorHandlerFn)palGetSymbol(
        g_X11.libx11,
        "XSetErrorHandler");

    g_X11.sync = (XSyncFn)palGetSymbol(
        g_X11.libx11,
        "XSync");

    g_X11.saveContext = (XSaveContextFn)palGetSymbol(
        g_X11.libx11,
        "XSaveContext");

    g_X11.findContext = (XFindContextFn)palGetSymbol(
        g_X11.libx11,
        "XFindContext");

    g_X11.uniqueContext = (XrmUniqueQuarkFn)palGetSymbol(
        g_X11.libx11,
        "XrmUniqueQuark");

    // load Xrandr functions
    g_X11.getScreenResources = (XRRGetScreenResourcesFn)palGetSymbol(
        g_X11.libxrandr,
        "XRRGetScreenResources");

    g_X11.getOutputPrimary = (XRRGetOutputPrimaryFn)palGetSymbol(
        g_X11.libxrandr,
        "XRRGetOutputPrimary");

    g_X11.getOutputInfo = (XRRGetOutputInfoFn)palGetSymbol(
        g_X11.libxrandr,
        "XRRGetOutputInfo");

    g_X11.getCrtcInfo = (XRRGetCrtcInfoFn)palGetSymbol(
        g_X11.libxrandr,
        "XRRGetCrtcInfo");

    g_X11.freeScreenResources = (XRRFreeScreenResourcesFn)palGetSymbol(
        g_X11.libxrandr,
        "XRRFreeScreenResources");

    g_X11.freeOutputInfo = (XRRFreeOutputInfoFn)palGetSymbol(
        g_X11.libxrandr,
        "XRRFreeOutputInfo");

    g_X11.freeCrtcInfo = (XRRFreeCrtcInfoFn)palGetSymbol(
        g_X11.libxrandr,
        "XRRFreeCrtcInfo");

    g_X11.selectRRInput = (XRRSelectInputFn)palGetSymbol(
        g_X11.libxrandr,
        "XRRSelectInput");

    g_X11.queryRRExtension = (XRRQueryExtensionFn)palGetSymbol(
        g_X11.libxrandr,
        "XRRQueryExtension");

    g_X11.allocClassHint = (XAllocClassHintFn)palGetSymbol(
        g_X11.libx11,
        "XAllocClassHint");

    g_X11.setClassHint = (XSetClassHintFn)palGetSymbol(
        g_X11.libx11,
        "XSetClassHint");

    g_X11.getVisualInfo = (XGetVisualInfoFn)palGetSymbol(
        g_X11.libx11,
        "XGetVisualInfo");

    g_X11.createFontCursor = (XCreateFontCursorFn)palGetSymbol(
        g_X11.libx11,
        "XCreateFontCursor");

    g_X11.freePixmap = (XFreePixmapFn)palGetSymbol(
        g_X11.libx11,
        "XFreePixmap");

    g_X11.setWMHints = (XSetWMHintsFn)palGetSymbol(
        g_X11.libx11,
        "XSetWMHints");

    g_X11.grabPointer = (XGrabPointerFn)palGetSymbol(
        g_X11.libx11,
        "XGrabPointer");

    g_X11.createPixmapCursor = (XCreatePixmapCursorFn)palGetSymbol(
        g_X11.libx11,
        "XCreatePixmapCursor");

    g_X11.warpPointer = (XWarpPointerFn)palGetSymbol(
        g_X11.libx11,
        "XWarpPointer");

    g_X11.getWMName = (XGetWMNameFn)palGetSymbol(
        g_X11.libx11,
        "XGetWMName");

    g_X11.queryPointer = (XQueryPointerFn)palGetSymbol(
        g_X11.libx11,
        "XQueryPointer");

    g_X11.ungrabPointer = (XUngrabPointerFn)palGetSymbol(
        g_X11.libx11,
        "XUngrabPointer");

    g_X11.allocWMHints = (XAllocWMHintsFn)palGetSymbol(
        g_X11.libx11,
        "XAllocWMHints");

    g_X11.mapRaised = (XMapRaisedFn)palGetSymbol(
        g_X11.libx11,
        "XMapRaised");

    g_X11.undefineCursor = (XUndefineCursorFn)palGetSymbol(
        g_X11.libx11,
        "XUndefineCursor");

    g_X11.defineCursor = (XDefineCursorFn)palGetSymbol(
        g_X11.libx11,
        "XDefineCursor");

    g_X11.freeCursor = (XFreeCursorFn)palGetSymbol(
        g_X11.libx11,
        "XFreeCursor");

    g_X11.getWMHints = (XGetWMHintsFn)palGetSymbol(
        g_X11.libx11,
        "XGetWMHints");

    g_X11.createPixmap = (XCreatePixmapFn)palGetSymbol(
        g_X11.libx11,
        "XCreatePixmap");

    g_X11.setInputFocus = (XSetInputFocusFn)palGetSymbol(
        g_X11.libx11,
        "XSetInputFocus");

    g_X11.getInputFocus = (XGetInputFocusFn)palGetSymbol(
        g_X11.libx11,
        "XGetInputFocus");

    g_X11.selectInput = (XSelectInputFn)palGetSymbol(
        g_X11.libx11,
        "XSelectInput");

    g_X11.lookupKeysym = (XLookupKeysymFn)palGetSymbol(
        g_X11.libx11,
        "XLookupKeysym");

    // libXcursor
    g_X11.cursorImageLoadCursor = (XcursorImageLoadCursorFn)palGetSymbol(
        g_X11.libcursor,
        "XcursorImageLoadCursor");

    g_X11.cursorImageCreate = (XcursorImageCreateFn)palGetSymbol(
        g_X11.libcursor,
        "XcursorImageCreate");

    g_X11.cursorImageDestroy = (XcursorImageDestroyFn)palGetSymbol(
        g_X11.libcursor,
        "XcursorImageDestroy");

    g_X11.setDetectableAutoRepeat = (XkbSetDetectableAutoRepeatFn)palGetSymbol(
        g_X11.libx11,
        "XkbSetDetectableAutoRepeat");

    g_X11.setLocaleModifiers = (XSetLocaleModifiersFn)palGetSymbol(
        g_X11.libx11,
        "XSetLocaleModifiers");

    g_X11.openIM = (XOpenIMFn)palGetSymbol(
        g_X11.libx11,
        "XOpenIM");

    g_X11.closeIM = (XCloseIMFn)palGetSymbol(
        g_X11.libx11,
        "XCloseIM");

    g_X11.createIC = (XCreateICFn)palGetSymbol(
        g_X11.libx11,
        "XCreateIC");

    g_X11.destroyIC = (XDestroyICFn)palGetSymbol(
        g_X11.libx11,
        "XDestroyIC");

    g_X11.utf8LookupString = (Xutf8LookupStringFn)palGetSymbol(
        g_X11.libx11,
        "Xutf8LookupString");

    // clang-format on
}

static void checkFeatures(void)
{
    // cache these atoms
    X11_INTERN_ATOM(WM_DELETE_WINDOW);
    X11_INTERN_ATOM(_NET_SUPPORTED);
    X11_INTERN_ATOM(_NET_WM_STATE);
    X11_INTERN_ATOM(_NET_WM_STATE_ABOVE);
    X11_INTERN_ATOM(_NET_WM_STATE_MAXIMIZED_VERT);
    X11_INTERN_ATOM(_NET_WM_STATE_MAXIMIZED_HORZ);
    X11_INTERN_ATOM(_NET_WM_STATE_HIDDEN);
    X11_INTERN_ATOM(_NET_WM_NAME);
    X11_INTERN_ATOM(UTF8_STRING);
    X11_INTERN_ATOM(_NET_WM_WINDOW_TYPE_UTILITY);
    X11_INTERN_ATOM(_NET_WM_DESKTOP);
    X11_INTERN_ATOM(_NET_WM_STATE_DEMANDS_ATTENTION);
    X11_INTERN_ATOM(_NET_WM_WINDOW_OPACITY);
    X11_INTERN_ATOM(_NET_WM_WINDOW_TYPE);
    X11_INTERN_ATOM(_NET_WM_WINDOW_TYPE_SPLASH);
    X11_INTERN_ATOM(_NET_WM_PID);
    X11_INTERN_ATOM(_WM_CLASS);
    X11_INTERN_ATOM(_NET_ACTIVE_WINDOW);
    X11_INTERN_ATOM(_NET_WM_ICON);

    // check for support from the window manager
    Atom type;
    int format;
    unsigned long count, bytesAfters;
    Atom* supportedAtoms = nullptr;
    g_X11.getWindowProperty(
        g_X11.display,
        g_X11.root,
        g_X11Atoms._NET_SUPPORTED,
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
    if (supportedAtoms) {
        for (unsigned long i = 0; i < count; ++i) {
            if (supportedAtoms[i] == g_X11Atoms._NET_WM_STATE_MAXIMIZED_VERT) {
                features |= PAL_VIDEO_FEATURE_WINDOW_SET_STATE;
                features |= PAL_VIDEO_FEATURE_WINDOW_GET_STATE;
            }

            if (supportedAtoms[i] == g_X11Atoms._NET_WM_STATE_MAXIMIZED_HORZ) {
                features |= PAL_VIDEO_FEATURE_WINDOW_SET_STATE;
                features |= PAL_VIDEO_FEATURE_WINDOW_GET_STATE;
            }

            if (supportedAtoms[i] == g_X11Atoms._NET_WM_STATE_HIDDEN) {
                features |= PAL_VIDEO_FEATURE_WINDOW_SET_STATE;
                features |= PAL_VIDEO_FEATURE_WINDOW_GET_STATE;
            }

            if (supportedAtoms[i] == g_X11Atoms._NET_WM_WINDOW_TYPE_SPLASH) {
                features |= PAL_VIDEO_FEATURE_BORDERLESS_WINDOW;
            }

            if (supportedAtoms[i] == g_X11Atoms._NET_WM_NAME) {
                g_X11Atoms.unicodeTitle = PAL_TRUE;
            }

            if (supportedAtoms[i] == g_X11Atoms._NET_WM_WINDOW_TYPE_UTILITY) {
                features |= PAL_VIDEO_FEATURE_TOOL_WINDOW;
            }
        }

        g_X11.free(supportedAtoms);
    }

    // check for transparent windows
    Atom compositor = g_X11.internAtom(g_X11.display, "_NET_WM_CM_S0", True);
    if (compositor != None) {
        Window owner = g_X11.getSelectionOwner(g_X11.display, compositor);
        if (owner != None) {
            features |= PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW;
        }
    }

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

    s_X11Driver.features = features;
}

static void createScancodeTable(void)
{
    // Letters
    g_X11Keyboard.scancodes[0x01E] = PAL_SCANCODE_A;
    g_X11Keyboard.scancodes[0x030] = PAL_SCANCODE_B;
    g_X11Keyboard.scancodes[0x02E] = PAL_SCANCODE_C;
    g_X11Keyboard.scancodes[0x020] = PAL_SCANCODE_D;
    g_X11Keyboard.scancodes[0x012] = PAL_SCANCODE_E;
    g_X11Keyboard.scancodes[0x021] = PAL_SCANCODE_F;
    g_X11Keyboard.scancodes[0x022] = PAL_SCANCODE_G;
    g_X11Keyboard.scancodes[0x023] = PAL_SCANCODE_H;
    g_X11Keyboard.scancodes[0x017] = PAL_SCANCODE_I;
    g_X11Keyboard.scancodes[0x024] = PAL_SCANCODE_J;
    g_X11Keyboard.scancodes[0x025] = PAL_SCANCODE_K;
    g_X11Keyboard.scancodes[0x026] = PAL_SCANCODE_L;
    g_X11Keyboard.scancodes[0x032] = PAL_SCANCODE_M;
    g_X11Keyboard.scancodes[0x031] = PAL_SCANCODE_N;
    g_X11Keyboard.scancodes[0x018] = PAL_SCANCODE_O;
    g_X11Keyboard.scancodes[0x019] = PAL_SCANCODE_P;
    g_X11Keyboard.scancodes[0x010] = PAL_SCANCODE_Q;
    g_X11Keyboard.scancodes[0x013] = PAL_SCANCODE_R;
    g_X11Keyboard.scancodes[0x01F] = PAL_SCANCODE_S;
    g_X11Keyboard.scancodes[0x014] = PAL_SCANCODE_T;
    g_X11Keyboard.scancodes[0x016] = PAL_SCANCODE_U;
    g_X11Keyboard.scancodes[0x02F] = PAL_SCANCODE_V;
    g_X11Keyboard.scancodes[0x011] = PAL_SCANCODE_W;
    g_X11Keyboard.scancodes[0x02D] = PAL_SCANCODE_X;
    g_X11Keyboard.scancodes[0x015] = PAL_SCANCODE_Y;
    g_X11Keyboard.scancodes[0x02C] = PAL_SCANCODE_Z;

    // Numbers (top row)
    g_X11Keyboard.scancodes[0x00B] = PAL_SCANCODE_0;
    g_X11Keyboard.scancodes[0x002] = PAL_SCANCODE_1;
    g_X11Keyboard.scancodes[0x003] = PAL_SCANCODE_2;
    g_X11Keyboard.scancodes[0x004] = PAL_SCANCODE_3;
    g_X11Keyboard.scancodes[0x005] = PAL_SCANCODE_4;
    g_X11Keyboard.scancodes[0x006] = PAL_SCANCODE_5;
    g_X11Keyboard.scancodes[0x007] = PAL_SCANCODE_6;
    g_X11Keyboard.scancodes[0x008] = PAL_SCANCODE_7;
    g_X11Keyboard.scancodes[0x009] = PAL_SCANCODE_8;
    g_X11Keyboard.scancodes[0x00A] = PAL_SCANCODE_9;

    // Function
    g_X11Keyboard.scancodes[0x03B] = PAL_SCANCODE_F1;
    g_X11Keyboard.scancodes[0x03C] = PAL_SCANCODE_F2;
    g_X11Keyboard.scancodes[0x03D] = PAL_SCANCODE_F3;
    g_X11Keyboard.scancodes[0x03E] = PAL_SCANCODE_F4;
    g_X11Keyboard.scancodes[0x03F] = PAL_SCANCODE_F5;
    g_X11Keyboard.scancodes[0x040] = PAL_SCANCODE_F6;
    g_X11Keyboard.scancodes[0x041] = PAL_SCANCODE_F7;
    g_X11Keyboard.scancodes[0x042] = PAL_SCANCODE_F8;
    g_X11Keyboard.scancodes[0x043] = PAL_SCANCODE_F9;
    g_X11Keyboard.scancodes[0x044] = PAL_SCANCODE_F10;
    g_X11Keyboard.scancodes[0x057] = PAL_SCANCODE_F11;
    g_X11Keyboard.scancodes[0x058] = PAL_SCANCODE_F12;

    // Control
    g_X11Keyboard.scancodes[0x001] = PAL_SCANCODE_ESCAPE;
    g_X11Keyboard.scancodes[0x01C] = PAL_SCANCODE_ENTER;
    g_X11Keyboard.scancodes[0x00F] = PAL_SCANCODE_TAB;
    g_X11Keyboard.scancodes[0x00E] = PAL_SCANCODE_BACKSPACE;
    g_X11Keyboard.scancodes[0x039] = PAL_SCANCODE_SPACE;
    g_X11Keyboard.scancodes[0x03A] = PAL_SCANCODE_CAPSLOCK;
    g_X11Keyboard.scancodes[0x045] = PAL_SCANCODE_NUMLOCK;
    g_X11Keyboard.scancodes[0x046] = PAL_SCANCODE_SCROLLLOCK;
    g_X11Keyboard.scancodes[0x02A] = PAL_SCANCODE_LSHIFT;
    g_X11Keyboard.scancodes[0x036] = PAL_SCANCODE_RSHIFT;
    g_X11Keyboard.scancodes[0x01D] = PAL_SCANCODE_LCTRL;
    g_X11Keyboard.scancodes[0x061] = PAL_SCANCODE_RCTRL;
    g_X11Keyboard.scancodes[0x038] = PAL_SCANCODE_LALT;
    g_X11Keyboard.scancodes[0x064] = PAL_SCANCODE_RALT;

    // Arrows
    g_X11Keyboard.scancodes[0x069] = PAL_SCANCODE_LEFT;
    g_X11Keyboard.scancodes[0x06A] = PAL_SCANCODE_RIGHT;
    g_X11Keyboard.scancodes[0x067] = PAL_SCANCODE_UP;
    g_X11Keyboard.scancodes[0x06C] = PAL_SCANCODE_DOWN;

    // Navigation
    g_X11Keyboard.scancodes[0x06E] = PAL_SCANCODE_INSERT;
    g_X11Keyboard.scancodes[0x06F] = PAL_SCANCODE_DELETE;
    g_X11Keyboard.scancodes[0x066] = PAL_SCANCODE_HOME;
    g_X11Keyboard.scancodes[0x067] = PAL_SCANCODE_END;
    g_X11Keyboard.scancodes[0x068] = PAL_SCANCODE_PAGEUP;
    g_X11Keyboard.scancodes[0x06D] = PAL_SCANCODE_PAGEDOWN;

    // Keypad
    g_X11Keyboard.scancodes[0x052] = PAL_SCANCODE_KP_0;
    g_X11Keyboard.scancodes[0x04F] = PAL_SCANCODE_KP_1;
    g_X11Keyboard.scancodes[0x050] = PAL_SCANCODE_KP_2;
    g_X11Keyboard.scancodes[0x051] = PAL_SCANCODE_KP_3;
    g_X11Keyboard.scancodes[0x04B] = PAL_SCANCODE_KP_4;
    g_X11Keyboard.scancodes[0x04C] = PAL_SCANCODE_KP_5;
    g_X11Keyboard.scancodes[0x04D] = PAL_SCANCODE_KP_6;
    g_X11Keyboard.scancodes[0x047] = PAL_SCANCODE_KP_7;
    g_X11Keyboard.scancodes[0x048] = PAL_SCANCODE_KP_8;
    g_X11Keyboard.scancodes[0x049] = PAL_SCANCODE_KP_9;
    g_X11Keyboard.scancodes[0x060] = PAL_SCANCODE_KP_ENTER;
    g_X11Keyboard.scancodes[0x04E] = PAL_SCANCODE_KP_ADD;
    g_X11Keyboard.scancodes[0x04A] = PAL_SCANCODE_KP_SUBTRACT;
    g_X11Keyboard.scancodes[0x037] = PAL_SCANCODE_KP_MULTIPLY;
    g_X11Keyboard.scancodes[0x062] = PAL_SCANCODE_KP_DIVIDE;
    g_X11Keyboard.scancodes[0x053] = PAL_SCANCODE_KP_DECIMAL;

    // Misc
    g_X11Keyboard.scancodes[0x063] = PAL_SCANCODE_PRINTSCREEN;
    g_X11Keyboard.scancodes[0x066] = PAL_SCANCODE_PAUSE;
    g_X11Keyboard.scancodes[0x07F] = PAL_SCANCODE_MENU;
    g_X11Keyboard.scancodes[0x028] = PAL_SCANCODE_APOSTROPHE;
    g_X11Keyboard.scancodes[0x02B] = PAL_SCANCODE_BACKSLASH;
    g_X11Keyboard.scancodes[0x033] = PAL_SCANCODE_COMMA;
    g_X11Keyboard.scancodes[0x00D] = PAL_SCANCODE_EQUAL;
    g_X11Keyboard.scancodes[0x029] = PAL_SCANCODE_GRAVEACCENT;
    g_X11Keyboard.scancodes[0x00C] = PAL_SCANCODE_SUBTRACT;
    g_X11Keyboard.scancodes[0x034] = PAL_SCANCODE_PERIOD;
    g_X11Keyboard.scancodes[0x027] = PAL_SCANCODE_SEMICOLON;
    g_X11Keyboard.scancodes[0x035] = PAL_SCANCODE_SLASH;
    g_X11Keyboard.scancodes[0x01A] = PAL_SCANCODE_LBRACKET;
    g_X11Keyboard.scancodes[0x01B] = PAL_SCANCODE_RBRACKET;
    g_X11Keyboard.scancodes[0x07D] = PAL_SCANCODE_LSUPER;
    g_X11Keyboard.scancodes[0x07E] = PAL_SCANCODE_RSUPER;
}

static void createKeycodeTable(void)
{
    // Tis is for only printable and text input keys

    // Letters
    g_X11Keyboard.keycodes[XK_a] = PAL_KEYCODE_A;
    g_X11Keyboard.keycodes[XK_b] = PAL_KEYCODE_B;
    g_X11Keyboard.keycodes[XK_c] = PAL_KEYCODE_C;
    g_X11Keyboard.keycodes[XK_d] = PAL_KEYCODE_D;
    g_X11Keyboard.keycodes[XK_e] = PAL_KEYCODE_E;
    g_X11Keyboard.keycodes[XK_f] = PAL_KEYCODE_F;
    g_X11Keyboard.keycodes[XK_g] = PAL_KEYCODE_G;
    g_X11Keyboard.keycodes[XK_h] = PAL_KEYCODE_H;
    g_X11Keyboard.keycodes[XK_i] = PAL_KEYCODE_I;
    g_X11Keyboard.keycodes[XK_j] = PAL_KEYCODE_J;
    g_X11Keyboard.keycodes[XK_k] = PAL_KEYCODE_K;
    g_X11Keyboard.keycodes[XK_l] = PAL_KEYCODE_L;
    g_X11Keyboard.keycodes[XK_m] = PAL_KEYCODE_M;
    g_X11Keyboard.keycodes[XK_n] = PAL_KEYCODE_N;
    g_X11Keyboard.keycodes[XK_o] = PAL_KEYCODE_O;
    g_X11Keyboard.keycodes[XK_p] = PAL_KEYCODE_P;
    g_X11Keyboard.keycodes[XK_q] = PAL_KEYCODE_Q;
    g_X11Keyboard.keycodes[XK_r] = PAL_KEYCODE_R;
    g_X11Keyboard.keycodes[XK_s] = PAL_KEYCODE_S;
    g_X11Keyboard.keycodes[XK_t] = PAL_KEYCODE_T;
    g_X11Keyboard.keycodes[XK_u] = PAL_KEYCODE_U;
    g_X11Keyboard.keycodes[XK_v] = PAL_KEYCODE_V;
    g_X11Keyboard.keycodes[XK_w] = PAL_KEYCODE_W;
    g_X11Keyboard.keycodes[XK_x] = PAL_KEYCODE_X;
    g_X11Keyboard.keycodes[XK_y] = PAL_KEYCODE_Y;
    g_X11Keyboard.keycodes[XK_z] = PAL_KEYCODE_Z;

    // Control
    g_X11Keyboard.keycodes[XK_space] = PAL_KEYCODE_SPACE;

    // Misc
    g_X11Keyboard.keycodes[XK_apostrophe] = PAL_KEYCODE_APOSTROPHE;
    g_X11Keyboard.keycodes[XK_backslash] = PAL_KEYCODE_BACKSLASH;
    g_X11Keyboard.keycodes[XK_comma] = PAL_KEYCODE_COMMA;
    g_X11Keyboard.keycodes[XK_equal] = PAL_KEYCODE_EQUAL;
    g_X11Keyboard.keycodes[XK_grave] = PAL_KEYCODE_GRAVEACCENT;
    g_X11Keyboard.keycodes[XK_minus] = PAL_KEYCODE_SUBTRACT;
    g_X11Keyboard.keycodes[XK_period] = PAL_KEYCODE_PERIOD;
    g_X11Keyboard.keycodes[XK_semicolon] = PAL_KEYCODE_SEMICOLON;
    g_X11Keyboard.keycodes[XK_slash] = PAL_KEYCODE_SLASH;
    g_X11Keyboard.keycodes[XK_bracketleft] = PAL_KEYCODE_LBRACKET;
    g_X11Keyboard.keycodes[XK_bracketright] = PAL_KEYCODE_RBRACKET;
}

static void resetMonitorData(void)
{
    memset(g_X11.monitorData, 0, g_X11.maxMonitorData * sizeof(MonitorData));
}

static void cacheMonitors(void)
{
    resetMonitorData();

    XRRScreenResources* resources = nullptr;
    resources = g_X11.getScreenResources(g_X11.display, g_X11.root);
    if (!resources) {
        return;
    }

    for (int i = 0; i < resources->noutput; ++i) {
        RROutput output = resources->outputs[i];
        XRROutputInfo* info = g_X11.getOutputInfo(g_X11.display, resources, output);
        if (info->connection == RR_Connected && info->crtc != None) {
            PalMonitor* monitor = TO_PAL_HANDLE(PalMonitor, output);
            MonitorData* data = nullptr;
            data = x11GetFreeMonitorData();
            if (!data) {
                g_X11.freeOutputInfo(info);
                return;
            }

            data->monitor = monitor;
            XRRCrtcInfo* crtc = g_X11.getCrtcInfo(g_X11.display, resources, info->crtc);

            // get DPI
            float raw = (float)crtc->width / 1920.0f;
            float steps[] = {1.0f, 1.2f, 1.5f, 1.75, 2.0f};
            float closest = steps[0];
            float minDiff = fabsf(raw - steps[0]);

            for (uint32_t i = 1; i < sizeof(steps) / sizeof(steps[0]); i++) {
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

            g_X11.freeCrtcInfo(crtc);
            g_X11.monitorCount++;
        }

        g_X11.freeOutputInfo(info);
    }

    g_X11.freeScreenResources(resources);
}

static PalWindowState queryWindowState(Window xWin)
{
    Atom type;
    int format;
    unsigned long count, bytesAfter;
    Atom* atoms = nullptr;
    PalWindowState state = PAL_WINDOW_STATE_RESTORED;

    g_X11.getWindowProperty(
        g_X11.display,
        xWin,
        g_X11Atoms._NET_WM_STATE,
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
        if (atoms[i] == g_X11Atoms._NET_WM_STATE_MAXIMIZED_HORZ) {
            state = PAL_WINDOW_STATE_MAXIMIZED;
        }

        if (atoms[i] == g_X11Atoms._NET_WM_STATE_MAXIMIZED_VERT) {
            state = PAL_WINDOW_STATE_MAXIMIZED;
        }

        if (atoms[i] == g_X11Atoms._NET_WM_STATE_HIDDEN) {
            state = PAL_WINDOW_STATE_MINIMIZED;
        }
    }

    g_X11.free(atoms);
    return state;
}

static uint32_t getWindowMonitorDPI(WindowData* data)
{
    int winX = data->x + (int)data->w / 2;
    int winY = data->y + (int)data->h / 2;

    for (uint32_t i = 0; i < g_X11.maxMonitorData; i++) {
        if (!g_X11.monitorData[i].used) {
            continue;
        }

        MonitorData* info = &g_X11.monitorData[i];
        if (winX >= info->x && winX < info->x + (int)info->w &&
            winY >= info->y && winY < info->y + (int)info->h) {
            return info->dpi;
        }
    }

    return 96;
}


/* =========================================================================
 * API
 * ========================================================================= */

void x11ShutdownVideo(void);

PalVideoDriver* x11GetVideoDriver(void)
{
    if (s_X11Driver.features != 0) {
        return &s_X11Driver;
    }

    if (!g_X11.libx11) {
        g_X11.libx11 = palLoadLibrary("libX11.so");
        if (g_X11.libx11) {
            loadProcs(PAL_FALSE);

            g_X11.display = g_X11.openDisplay(nullptr);
            if (g_X11.display) {
                g_X11.root = DefaultRootWindow(g_X11.display);
                g_X11.screen = DefaultScreen(g_X11.display);
                checkFeatures();
                g_X11.closeDisplay(g_X11.display);
            }

            palFreeLibrary(g_X11.libx11);
            memset(&g_X11, 0, sizeof(X11));
        }
    }

    s_X11Driver.type = PAL_VIDEO_DRIVER_TYPE_X11;
    return &s_X11Driver;
}

PalResult x11InitVideo(
    const PalAllocator* allocator,
    struct PalEventDriver* eventDriver,
    const void* preferredInstance,
    void** selectedInstance)
{
    PalResult result = PAL_RESULT_CODE_PLATFORM_FAILURE;
    g_X11.allocator = allocator;
    g_X11.eventDriver = eventDriver;

    g_X11.libx11 = palLoadLibrary("libX11.so");
    g_X11.libcursor = palLoadLibrary("libXcursor.so");
    if (!g_X11.libx11 || !g_X11.libcursor) {
        result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_POSIX, (uint32_t)errno);
        goto cleanup;
    }

    g_X11.libxrandr = palLoadLibrary("libXrandr.so.2");
    if (!g_X11.libxrandr) {
        g_X11.libxrandr = palLoadLibrary("libXrandr.so");
    }

    if (!g_X11.libxrandr) {
        result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_POSIX, (uint32_t)errno);
        goto cleanup;
    }

    loadProcs(PAL_TRUE);
    createScancodeTable();
    createKeycodeTable();

    g_X11.maxMonitorData = 16;
    g_X11.maxWindowData = 32;
    g_X11.windowData = palAllocate(allocator, sizeof(WindowData) * g_X11.maxWindowData, 0);
    g_X11.monitorData = palAllocate(allocator, sizeof(MonitorData) * g_X11.maxMonitorData, 0);

    if (!g_X11.monitorData || !g_X11.windowData) {
        result = PAL_RESULT_CODE_OUT_OF_MEMORY;
        goto cleanup;
    }

    memset(g_X11.windowData, 0, sizeof(WindowData) * g_X11.maxWindowData);
    memset(g_X11.monitorData, 0, sizeof(MonitorData) * g_X11.maxMonitorData);

    g_X11.monitorCount = 0;
    if (preferredInstance) {
        g_X11.display = (Display*)preferredInstance;
        g_X11.ownsDisplay = PAL_FALSE;
        *selectedInstance = (void*)preferredInstance;

    } else {
        g_X11.display = g_X11.openDisplay(nullptr);
        if (!g_X11.display) {
            result = PAL_RESULT_CODE_PLATFORM_FAILURE;
            goto cleanup;
        }
        g_X11.ownsDisplay = PAL_TRUE;
        *selectedInstance = (void*)g_X11.display;
    }

    g_X11.root = DefaultRootWindow(g_X11.display);
    g_X11.screen = DefaultScreen(g_X11.display);
    checkFeatures();

    g_X11.selectRRInput(g_X11.display, g_X11.root, RRScreenChangeNotifyMask | RRNotify);
    int eventBase, errorBase = 0;
    g_X11.queryRRExtension(g_X11.display, &eventBase, &errorBase);
    g_X11.rrEventBase = eventBase;
    g_X11.skipScreenEvent = PAL_TRUE;

    g_X11.dataID = (XContext)g_X11.uniqueContext();

    g_X11.monitorCount = 0;
    resetMonitorData();
    cacheMonitors();

    loadVideoGLX();
    loadVideoEGL();

    int supported;
    g_X11.setDetectableAutoRepeat(g_X11.display, True, &supported);

    g_X11.setLocaleModifiers("");
    g_X11.im = g_X11.openIM(g_X11.display, nullptr, nullptr, nullptr);
    if (g_X11.im == None) {
        result = PAL_RESULT_CODE_PLATFORM_FAILURE;
        goto cleanup;
    }

    return PAL_RESULT_SUCCESS;

cleanup:
    x11ShutdownVideo();
    return result;
}

void x11ShutdownVideo(void)
{
    if (g_X11.im) {
        g_X11.closeIM(g_X11.im);
    }

    if (g_X11.ownsDisplay && g_X11.display) {
        g_X11.closeDisplay(g_X11.display);
    }

    if (g_X11.windowData) {
        palFree(g_X11.allocator, g_X11.windowData);
    }

    if (g_X11.monitorData) {
        palFree(g_X11.allocator, g_X11.monitorData);
    }

    if (g_X11.libcursor) {
        palFreeLibrary(g_X11.libcursor);
    }

    if (g_X11.libxrandr) {
        palFreeLibrary(g_X11.libxrandr);
    }

    if (g_X11.libx11) {
        palFreeLibrary(g_X11.libx11);
    }

    unloadVideoGLX();
    unloadVideoEGL();

    memset(&g_X11, 0, sizeof(X11));
    memset(&g_X11Atoms, 0, sizeof(X11Atoms));
    memset(&g_X11Keyboard, 0, sizeof(X11Keyboard));
    memset(&g_X11Mouse, 0, sizeof(X11Mouse));
}

void x11UpdateVideo(void)
{
    XEvent event;

#if PAL_HAS_EVENT_MODULE
    PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
#endif // PAL_HAS_EVENT_MODULE

    while (g_X11.pending(g_X11.display)) {
        g_X11.nextEvent(g_X11.display, &event);

        Window xWin = event.xany.window;
        PalWindow* window = TO_PAL_HANDLE(PalWindow, xWin);
        WindowData* data = nullptr;
        g_X11.findContext(g_X11.display, xWin, g_X11.dataID, (XPointer*)&data);

        if (event.type == g_X11.rrEventBase + RRScreenChangeNotify) {
            event.type = RANDR_SCREEN_CHANGE_EVENT; // for switch flow
        }

        switch (event.type) {
            case ClientMessage: {
#if PAL_HAS_EVENT_MODULE
                Atom windowClose = (Atom)event.xclient.data.l[0];
                if (windowClose == g_X11Atoms.WM_DELETE_WINDOW) {
                    if (g_X11.eventDriver) {
                        PalEventDriver* driver = g_X11.eventDriver;
                        PalEventType type = PAL_EVENT_TYPE_WINDOW_CLOSE;
                        mode = palGetEventDispatchMode(driver, type);
                        if (mode != PAL_DISPATCH_MODE_NONE) {
                            PalEvent event = {0};
                            event.type = type;
                            event.data2 = palPackPointer(window);
                            palPushEvent(driver, &event);
                        }
                    }
                }
#endif // PAL_HAS_EVENT_MODULE

                break;
            }

            case ConfigureNotify: {
                if (!data) {
                    break;
                }

                // skip the first configure event
                if (data->skipConfigure) {
                    data->skipConfigure = PAL_FALSE;
                    data->w = (uint32_t)event.xconfigure.width;
                    data->h = (uint32_t)event.xconfigure.height;
                    data->x = event.xconfigure.x;
                    data->y = event.xconfigure.y;
                    break;
                }

                uint32_t tmpWidth = (uint32_t)event.xconfigure.width;
                uint32_t tmpHeight = (uint32_t)event.xconfigure.height;

                if (data->w != tmpWidth || data->h != tmpHeight) {
                    data->w = tmpWidth;
                    data->h = tmpHeight;

#if PAL_HAS_EVENT_MODULE
                    if (g_X11.eventDriver) {
                        PalEventDriver* driver = g_X11.eventDriver;
                        PalEventType type = PAL_EVENT_TYPE_WINDOW_SIZE;
                        mode = palGetEventDispatchMode(driver, type);

                        if (mode != PAL_DISPATCH_MODE_NONE) {
                            PalEvent outEvent = {0};
                            outEvent.type = type;
                            outEvent.data = palPackUint32(data->w, data->h);
                            outEvent.data2 = palPackPointer(window);
                            palPushEvent(driver, &outEvent);
                        }
                    }
#endif // PAL_HAS_EVENT_MODULE
                }

                if (data->isAttached && data->skipIfAttached) {
                    data->skipIfAttached = PAL_FALSE;
                    break;
                }

                if (data->x != event.xconfigure.x || data->y != event.xconfigure.y) {
                    data->x = event.xconfigure.x;
                    data->y = event.xconfigure.y;

#if PAL_HAS_EVENT_MODULE
                    if (g_X11.eventDriver) {
                        PalEventDriver* driver = g_X11.eventDriver;
                        PalEventType type = PAL_EVENT_TYPE_WINDOW_MOVE;
                        mode = palGetEventDispatchMode(driver, type);

                        if (mode != PAL_DISPATCH_MODE_NONE) {
                            PalEvent outEvent = {0};
                            outEvent.type = type;
                            outEvent.data = palPackInt32(data->x, data->y);
                            outEvent.data2 = palPackPointer(window);
                            palPushEvent(driver, &outEvent);
                        }
                    }
#endif // PAL_HAS_EVENT_MODULE

                    uint32_t monitorDPI = getWindowMonitorDPI(data);
                    if (monitorDPI != data->dpi) {
                        data->dpi = monitorDPI;

#if PAL_HAS_EVENT_MODULE
                        if (g_X11.eventDriver) {
                            PalEventDriver* driver = g_X11.eventDriver;
                            PalEventType type = PAL_EVENT_TYPE_MONITOR_DPI_CHANGED;
                            mode = palGetEventDispatchMode(driver, type);

                            if (mode != PAL_DISPATCH_MODE_NONE) {
                                PalEvent outEvent = {0};
                                outEvent.type = type;
                                outEvent.data = monitorDPI;
                                outEvent.data2 = palPackPointer(window);
                                palPushEvent(driver, &outEvent);
                            }
                        }
#endif // PAL_HAS_EVENT_MODULE
                    }
                }

                break;
            }

            case FocusIn: {
#if PAL_HAS_EVENT_MODULE
                if (g_X11.eventDriver) {
                    int eventmode = event.xfocus.mode;
                    if (eventmode == NotifyGrab || eventmode == NotifyUngrab) {
                        // ignore dragging and popup focus events
                        break;
                    }

                    PalEventDriver* driver = g_X11.eventDriver;
                    PalEventType type = PAL_EVENT_TYPE_WINDOW_FOCUS;
                    mode = palGetEventDispatchMode(driver, type);
                    if (mode != PAL_DISPATCH_MODE_NONE) {
                        PalEvent event = {0};
                        event.type = type;
                        event.data = PAL_TRUE;
                        event.data2 = palPackPointer(window);
                        palPushEvent(driver, &event);
                    }
                }
#endif // PAL_HAS_EVENT_MODULE

                break;
            }

            case FocusOut: {
    #if PAL_HAS_EVENT_MODULE
                if (g_X11.eventDriver) {
                    int eventmode = event.xfocus.mode;
                    if (eventmode == NotifyGrab || eventmode == NotifyUngrab) {
                        // ignore dragging and popup focus events
                        break;
                    }

                    PalEventDriver* driver = g_X11.eventDriver;
                    PalEventType type = PAL_EVENT_TYPE_WINDOW_FOCUS;
                    mode = palGetEventDispatchMode(driver, type);
                    if (mode != PAL_DISPATCH_MODE_NONE) {
                        PalEvent event = {0};
                        event.type = type;
                        event.data = PAL_FALSE;
                        event.data2 = palPackPointer(window);
                        palPushEvent(driver, &event);
                    }
                }
#endif // PAL_HAS_EVENT_MODULE

                break;
            }

            case PropertyNotify: {
                if (!data) {
                    break;
                }

                // check window state (maximize, minimize)
                if (event.xproperty.atom == g_X11Atoms._NET_WM_STATE) {
                    PalWindowState state;
                    state = queryWindowState(event.xproperty.window);
                    if (state != data->state) {
                        data->state = state;

                        // skip the first state event
                        if (data->skipState) {
                            data->skipState = PAL_FALSE;
                            break;
                        }

#if PAL_HAS_EVENT_MODULE
                    if (g_X11.eventDriver) {
                        PalEventDriver* driver = g_X11.eventDriver;
                        PalEventType type = PAL_EVENT_TYPE_WINDOW_STATE;
                        mode = palGetEventDispatchMode(driver, type);

                        if (mode != PAL_DISPATCH_MODE_NONE) {
                            PalEvent event = {0};
                            event.type = type;
                            event.data = data->state;
                            event.data2 = palPackPointer(window);
                            palPushEvent(driver, &event);
                        }
                    }
#endif // PAL_HAS_EVENT_MODULE
                    }
                }

                break;
            }

            case RANDR_SCREEN_CHANGE_EVENT: {
                // skip the first event
                if (g_X11.skipScreenEvent) {
                    g_X11.skipScreenEvent = PAL_FALSE;
                    break;
                }

                // store old monitor count
                uint32_t oldCount = g_X11.monitorCount;
                g_X11.monitorCount = 0;
                cacheMonitors();

                if (oldCount != g_X11.monitorCount) {
                    // a monitor has been added or removed
#if PAL_HAS_EVENT_MODULE
                    if (g_X11.eventDriver) {
                        PalEventDriver* driver = g_X11.eventDriver;
                        PalEventType type = PAL_EVENT_TYPE_MONITOR_LIST_CHANGED;
                        mode = palGetEventDispatchMode(driver, type);
                        if (mode != PAL_DISPATCH_MODE_NONE) {
                            PalEvent event = {0};
                            event.type = type;
                            event.data2 = palPackPointer(window);
                            palPushEvent(driver, &event);
                        }
                    }
#endif // PAL_HAS_EVENT_MODULE
                }

                break;
            }

            case MotionNotify: {
                // mouse moved
                const int x = event.xmotion.x;
                const int y = event.xmotion.y;
                const int dx = x - g_X11Mouse.lastX;
                const int dy = y - g_X11Mouse.lastY;

#if PAL_HAS_EVENT_MODULE
                if (g_X11.eventDriver) {
                    PalEventDriver* driver = g_X11.eventDriver;
                    PalEventType type = PAL_EVENT_TYPE_MOUSE_MOVE;
                    mode = palGetEventDispatchMode(driver, type);
                    if (mode != PAL_DISPATCH_MODE_NONE) {
                        PalEvent event = {0};
                        event.type = type;
                        event.data = palPackInt32(x, y);
                        event.data2 = palPackPointer(window);
                        palPushEvent(driver, &event);
                    }

                    // push a mouse delta event
                    type = PAL_EVENT_TYPE_MOUSE_DELTA;
                    mode = palGetEventDispatchMode(driver, type);
                    if (mode != PAL_DISPATCH_MODE_NONE) {
                        PalEvent event = {0};
                        event.type = type;
                        event.data = palPackFloat((float)dx, (float)dy);
                        event.data2 = palPackPointer(window);
                        palPushEvent(driver, &event);
                    }
                }
#endif // PAL_HAS_EVENT_MODULE

                g_X11Mouse.lastX = x;
                g_X11Mouse.lastY = y;
                g_Mouse.dx = dx;
                g_Mouse.dy = dy;
                break;
            }

            case ButtonPress:
            case ButtonRelease: {
                int xButton = (int)event.xbutton.button;
                PalBool pressed = (event.xbutton.type == ButtonPress);
                PalMouseButton button = 0;
                PalBool validButton = PAL_FALSE;

                if (xButton == 1) {
                    button = PAL_MOUSE_BUTTON_LEFT;
                    validButton = PAL_TRUE;
                } else if (xButton == 3) {
                    button = PAL_MOUSE_BUTTON_RIGHT;
                    validButton = PAL_TRUE;
                } else if (xButton == 2) {
                    button = PAL_MOUSE_BUTTON_MIDDLE;
                    validButton = PAL_TRUE;
                } else if (xButton == 8) {
                    button = PAL_MOUSE_BUTTON_X1;
                    validButton = PAL_TRUE;
                } else if (xButton == 9) {
                    button = PAL_MOUSE_BUTTON_X2;
                    validButton = PAL_TRUE;
                }

                if (validButton) {
                    g_Mouse.state[button] = pressed;

#if PAL_HAS_EVENT_MODULE
                    if (g_X11.eventDriver) {
                        PalEventDriver* driver = g_X11.eventDriver;
                        PalEventType type = pressed ? PAL_EVENT_TYPE_MOUSE_BUTTONDOWN : PAL_EVENT_TYPE_MOUSE_BUTTONUP;

                        mode = palGetEventDispatchMode(driver, type);
                        if (mode != PAL_DISPATCH_MODE_NONE) {
                            PalEvent outEvent = {0};
                            outEvent.type = type;
                            outEvent.data = palPackUint32(button, NULL_BUTTON_SERIAL);
                            outEvent.data2 = palPackPointer(window);
                            palPushEvent(driver, &outEvent);
                        }
                    }
#endif // PAL_HAS_EVENT_MODULE
                }

                int scrollX = 0;
                int scrollY = 0;
                if (xButton == 4) {
                    scrollY = 1;
                } else if (xButton == 5) {
                    scrollY = -1;
                } else if (xButton == 6) {
                    scrollX = -1;
                } else if (xButton == 7) {
                    scrollX = 1;
                }

                if (scrollX || scrollY) {
                    g_Mouse.wheelX = scrollX;
                    g_Mouse.wheelY = scrollY;

#if PAL_HAS_EVENT_MODULE
                    if (g_X11.eventDriver) {
                        PalEventDriver* driver = g_X11.eventDriver;
                        mode = palGetEventDispatchMode(driver, PAL_EVENT_TYPE_MOUSE_WHEEL);
                        if (mode != PAL_DISPATCH_MODE_NONE) {
                            PalEvent outEvent = {0};
                            outEvent.type = PAL_EVENT_TYPE_MOUSE_WHEEL;
                            outEvent.data = palPackFloat((float)scrollX, (float)scrollY);
                            outEvent.data2 = palPackPointer(window);
                            palPushEvent(driver, &outEvent);
                        }
                    }
#endif // PAL_HAS_EVENT_MODULE
                }

                break;
            }

            case KeyPress:
            case KeyRelease: {
                int xScancode = (int)event.xkey.keycode;
                PalBool pressed = (event.xkey.type == KeyPress);
                PalScancode scancode = PAL_SCANCODE_UNKNOWN;
                PalKeycode keycode = PAL_KEYCODE_UNKNOWN;
                PalEventType type;
                KeySym keySym = g_X11.lookupKeysym(&event.xkey, 0);

                if (xScancode == 110) {
                    if (keySym == XK_Pause) {
                        scancode = PAL_SCANCODE_PAUSE;
                    } else {
                        scancode = PAL_SCANCODE_HOME;
                    }
                } else {
                    int index = xScancode - 8;
                    if (index >= 0 && index < 512) {
                        scancode = (PalScancode)g_X11Keyboard.scancodes[index];
                    }
                }

                if (keySym >= XK_space && keySym <= XK_z) {
                    keycode = (PalKeycode)g_X11Keyboard.keycodes[keySym];
                } else {
                    keycode = (PalKeycode)(uint32_t)scancode;
                }

                if (keycode == PAL_KEYCODE_UNKNOWN) {
                    keycode = (PalKeycode)(uint32_t)scancode;
                }

                PalBool repeat = g_Keyboard.keycodeState[keycode];
                g_Keyboard.scancodeState[scancode] = pressed;
                g_Keyboard.keycodeState[keycode] = pressed;

                if (pressed) {
                    type = repeat ? PAL_EVENT_TYPE_KEYREPEAT : PAL_EVENT_TYPE_KEYDOWN;
                } else {
                    type = PAL_EVENT_TYPE_KEYUP;
                }

#if PAL_HAS_EVENT_MODULE
                if (g_X11.eventDriver) {
                    PalEventDriver* driver = g_X11.eventDriver;
                    mode = palGetEventDispatchMode(driver, type);
                    if (mode != PAL_DISPATCH_MODE_NONE) {
                        PalEvent outEvent = {0};
                        outEvent.type = type;
                        outEvent.data = palPackUint32(keycode, scancode);
                        outEvent.data2 = palPackPointer(window);
                        palPushEvent(driver, &outEvent);
                    }

                    if (!pressed || !data || !data->ic) {
                        break;
                    }

                    type = PAL_EVENT_TYPE_KEYCHAR;
                    mode = palGetEventDispatchMode(driver, type);
                    if (mode == PAL_DISPATCH_MODE_NONE) {
                        break;
                    }

                    int status = 0;
                    char buffer[32];
                    KeySym lookupSym = 0;
                    int len = g_X11.utf8LookupString(
                        data->ic,
                        &event.xkey,
                        buffer,
                        sizeof(buffer),
                        &lookupSym,
                        &status);

                    uint32_t codepoint = 0;
                    if (status == XLookupChars || status == XLookupBoth) {
                        unsigned char ch = (unsigned char)buffer[0];
                        if (ch < 0x80) {
                            codepoint = ch;
                        } else if ((ch >> 5) == 0x6 && len >= 2) {
                            codepoint = (((uint32_t)ch & 0x1Fu) << 6) | ((uint32_t)buffer[1] & 0x3Fu);

                        } else if ((ch >> 4) == 0xE && len >= 3) {
                            // clang-format off
                            codepoint = (((uint32_t)ch & 0x0Fu) << 12)       |
                                        (((uint32_t)buffer[1] & 0x3Fu) << 6) |
                                        ((uint32_t)buffer[2] & 0x3Fu);
                            // clang-format on

                        } else if ((ch >> 3) == 0x1E && len >= 4) {
                            // clang-format off
                            codepoint = (((uint32_t)ch & 0x07u) << 18)        |
                                        (((uint32_t)buffer[1] & 0x3Fu) << 12) |
                                        (((uint32_t)buffer[2] & 0x3Fu) << 6)  |
                                        ((uint32_t)buffer[3] & 0x3Fu);
                            // clang-format on
                        }

                        if (codepoint > 0) {
                            PalEvent outEvent = {0};
                            outEvent.type = type;
                            outEvent.data = codepoint;
                            outEvent.data2 = palPackPointer(window);
                            palPushEvent(driver, &outEvent);
                        }
                    }
                }
#endif // PAL_HAS_EVENT_MODULE

                break;
            }
        }
    }

    g_X11.flush(g_X11.display);
}

#endif // PAL_HAS_X11_BACKEND