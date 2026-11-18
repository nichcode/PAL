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

#ifdef PAL_HAS_WAYLAND_BACKEND

#if PAL_HAS_EVENT_MODULE
#include "pal2/core/pack.h"
#include "pal2/event/driver.h"
#endif // PAL_HAS_EVENT_MODULE

#include "pal2/core/library.h"
#include "pal2/core/time.h"
#include "pal2/video/init.h"
#include "video/platform.h"

#include "listeners.h"
#include "video/video_egl.h"
#include "wayland_helper.h"

#include <dlfcn.h>
#include <errno.h>
#include <poll.h>
#include <string.h>
#include <stdlib.h>

Wayland g_Wl = {0};
WaylandKeyboard g_WlKeyboard = {0};
WaylandMouse g_WlMouse = {0};

extern struct wl_interface xdgWmBaseInterface;
static PalVideoDriver s_WaylandDriver = {0};

/* =========================================================================
 * Helpers
 * ========================================================================= */

static void loadGlobals(void)
{
    // clang-format off

    g_Wl.outputInterface = palGetGlobal(
        g_Wl.libwayland, 
        "wl_output_interface");

    g_Wl.seatInterface = palGetGlobal(
        g_Wl.libwayland, 
        "wl_seat_interface");

    g_Wl.compositorInterface = palGetGlobal(
        g_Wl.libwayland, 
        "wl_compositor_interface");

    g_Wl.surfaceInterface = palGetGlobal(
        g_Wl.libwayland, 
        "wl_surface_interface");

    g_Wl.registryInterface = palGetGlobal(
        g_Wl.libwayland, 
        "wl_registry_interface");

    g_Wl.shmInterface = palGetGlobal(
        g_Wl.libwayland, 
        "wl_shm_interface");

    g_Wl.bufferInterface = palGetGlobal(
        g_Wl.libwayland, 
        "wl_buffer_interface");

    g_Wl.shmPoolInterface = palGetGlobal(
        g_Wl.libwayland, 
        "wl_shm_pool_interface");

    g_Wl.regionInterface = palGetGlobal(
        g_Wl.libwayland, 
        "wl_region_interface");

    g_Wl.pointerInterface = palGetGlobal(
        g_Wl.libwayland, 
        "wl_pointer_interface");

    g_Wl.keyboardInterface = palGetGlobal(
        g_Wl.libwayland, 
        "wl_keyboard_interface");

    // clang-format on
}

static void loadProcs(PalBool loadAll)
{
    // clang-format off

    g_Wl.displayConnect = (wlDisplayConnectFn)palGetSymbol(
        g_Wl.libwayland, 
        "wl_display_connect");

    g_Wl.displayDisconnect = (wlDisplayDisconnectFn)palGetSymbol(
        g_Wl.libwayland, 
        "wl_display_disconnect");

    g_Wl.displayRoundtrip = (wlDisplayRoundtripFn)palGetSymbol(
        g_Wl.libwayland, 
        "wl_display_roundtrip");

    g_Wl.displayDispatch = (wlDisplayDispatchFn)palGetSymbol(
        g_Wl.libwayland, 
        "wl_display_dispatch");

    g_Wl.proxyAddListener = (wlProxyAddListenerFn)palGetSymbol(
        g_Wl.libwayland, 
        "wl_proxy_add_listener");
        
    g_Wl.proxyDestroy = (wlProxyDestroyFn)palGetSymbol(
        g_Wl.libwayland, 
        "wl_proxy_destroy");

    g_Wl.proxyMarshalFlags = (wlProxyMarshalFlagsFn)palGetSymbol(
        g_Wl.libwayland, 
        "wl_proxy_marshal_flags");

    g_Wl.proxyGetVersion = (wlProxyGetVersionFn)palGetSymbol(
        g_Wl.libwayland, 
        "wl_proxy_get_version");

    g_Wl.getError = (wlDisplayGetErrorFn)palGetSymbol(
        g_Wl.libwayland, 
        "wl_display_get_error");

    g_Wl.dispatchPending = (wlDisplayDispatchPendingFn)palGetSymbol(
        g_Wl.libwayland, 
        "wl_display_dispatch_pending");

    g_Wl.displayFlush = (wlDisplayFlushFn)palGetSymbol(
        g_Wl.libwayland, 
        "wl_display_flush");

    g_Wl.prepareRead = (wlDisplayPrepareReadFn)palGetSymbol(
        g_Wl.libwayland, 
        "wl_display_prepare_read");

    g_Wl.readEvents = (wlDisplayReadEventsFn)palGetSymbol(
        g_Wl.libwayland, 
        "wl_display_read_events");

    g_Wl.displayGetFd = (wlDisplayGetFdFn)palGetSymbol(
        g_Wl.libwayland, 
        "wl_display_get_fd");

    g_Wl.cancelRead = (wlDisplayCancelReadFn)palGetSymbol(
        g_Wl.libwayland, 
        "wl_display_cancel_read");

    g_Wl.proxyMarshalConstructor = (wlProxyMarshalConstructorVFn)palGetSymbol(
        g_Wl.libwayland, 
        "wl_proxy_marshal_constructor_versioned");

    if (!loadAll) {
        return;
    }

    // xkbcommon
    g_Wl.xkbKeymapUnref = (xkbKeymapUnrefFn)palGetSymbol(
        g_Wl.xkbcommon, 
        "xkb_keymap_unref");

    g_Wl.xkbStateKeyGetOneSym = (xkbStateKeyGetOneSymFn)palGetSymbol(
        g_Wl.xkbcommon, 
        "xkb_state_key_get_one_sym");

    g_Wl.xkbStateNew = (xkbStateNewFn)palGetSymbol(
        g_Wl.xkbcommon, 
        "xkb_state_new");

    g_Wl.xkbStateUnref = (xkbStateUnrefFn)palGetSymbol(
        g_Wl.xkbcommon, 
        "xkb_state_unref");

    g_Wl.xkbContextUnref = (xkbContextUnrefFn)palGetSymbol(
        g_Wl.xkbcommon, 
        "xkb_context_unref");

    g_Wl.xkbContextNew = (xkbContextNewFn)palGetSymbol(
        g_Wl.xkbcommon, 
        "xkb_context_new");

    g_Wl.xkbKeymapNewFromString = (xkbKeymapNewFromStringFn)palGetSymbol(
        g_Wl.xkbcommon, 
        "xkb_keymap_new_from_string");

    g_Wl.xkbKeysymToUtf32 = (xkbKeysymToUtf32Fn)palGetSymbol(
        g_Wl.xkbcommon, 
        "xkb_keysym_to_utf32");

    g_Wl.xkbStateUpdateMask = (xkbStateUpdateMaskFn)palGetSymbol(
        g_Wl.xkbcommon, 
        "xkb_state_update_mask");

    g_Wl.xkbKeymapKeyRepeats = (xkbKeymapKeyRepeatsFn)palGetSymbol(
        g_Wl.xkbcommon, 
        "xkb_keymap_key_repeats");

    // wayland-cursor
    g_Wl.cursorImageGetBuffer = (wlCursorImageGetBufferFn)palGetSymbol(
        g_Wl.libcursor, 
        "wl_cursor_image_get_buffer");

    g_Wl.cursorThemeLoad = (wlCursorThemeLoadFn)palGetSymbol(
        g_Wl.libcursor, 
        "wl_cursor_theme_load");

    g_Wl.cursorThemeGetCursor = (wlCursorThemeGetCursorFn)palGetSymbol(
        g_Wl.libcursor, 
        "wl_cursor_theme_get_cursor");

    // wl-egl-window
    g_Wl.eglWindowCreate = (wlEglWindowCreateFn)palGetSymbol(
        g_Wl.libegl, 
        "wl_egl_window_create");

    g_Wl.eglWindowDestroy = (wlEglWindowDestroyFn)palGetSymbol(
        g_Wl.libegl, 
        "wl_egl_window_destroy");

    g_Wl.eglWindowResize = (wlEglWindowResizeFn)palGetSymbol(
        g_Wl.libegl, 
        "wl_egl_window_resize");

    // clang-format on
}

static void createScancodeTable(void)
{
    // Letters
    g_WlKeyboard.scancodes[0x01E] = PAL_SCANCODE_A;
    g_WlKeyboard.scancodes[0x030] = PAL_SCANCODE_B;
    g_WlKeyboard.scancodes[0x02E] = PAL_SCANCODE_C;
    g_WlKeyboard.scancodes[0x020] = PAL_SCANCODE_D;
    g_WlKeyboard.scancodes[0x012] = PAL_SCANCODE_E;
    g_WlKeyboard.scancodes[0x021] = PAL_SCANCODE_F;
    g_WlKeyboard.scancodes[0x022] = PAL_SCANCODE_G;
    g_WlKeyboard.scancodes[0x023] = PAL_SCANCODE_H;
    g_WlKeyboard.scancodes[0x017] = PAL_SCANCODE_I;
    g_WlKeyboard.scancodes[0x024] = PAL_SCANCODE_J;
    g_WlKeyboard.scancodes[0x025] = PAL_SCANCODE_K;
    g_WlKeyboard.scancodes[0x026] = PAL_SCANCODE_L;
    g_WlKeyboard.scancodes[0x032] = PAL_SCANCODE_M;
    g_WlKeyboard.scancodes[0x031] = PAL_SCANCODE_N;
    g_WlKeyboard.scancodes[0x018] = PAL_SCANCODE_O;
    g_WlKeyboard.scancodes[0x019] = PAL_SCANCODE_P;
    g_WlKeyboard.scancodes[0x010] = PAL_SCANCODE_Q;
    g_WlKeyboard.scancodes[0x013] = PAL_SCANCODE_R;
    g_WlKeyboard.scancodes[0x01F] = PAL_SCANCODE_S;
    g_WlKeyboard.scancodes[0x014] = PAL_SCANCODE_T;
    g_WlKeyboard.scancodes[0x016] = PAL_SCANCODE_U;
    g_WlKeyboard.scancodes[0x02F] = PAL_SCANCODE_V;
    g_WlKeyboard.scancodes[0x011] = PAL_SCANCODE_W;
    g_WlKeyboard.scancodes[0x02D] = PAL_SCANCODE_X;
    g_WlKeyboard.scancodes[0x015] = PAL_SCANCODE_Y;
    g_WlKeyboard.scancodes[0x02C] = PAL_SCANCODE_Z;

    // Numbers (top row)
    g_WlKeyboard.scancodes[0x00B] = PAL_SCANCODE_0;
    g_WlKeyboard.scancodes[0x002] = PAL_SCANCODE_1;
    g_WlKeyboard.scancodes[0x003] = PAL_SCANCODE_2;
    g_WlKeyboard.scancodes[0x004] = PAL_SCANCODE_3;
    g_WlKeyboard.scancodes[0x005] = PAL_SCANCODE_4;
    g_WlKeyboard.scancodes[0x006] = PAL_SCANCODE_5;
    g_WlKeyboard.scancodes[0x007] = PAL_SCANCODE_6;
    g_WlKeyboard.scancodes[0x008] = PAL_SCANCODE_7;
    g_WlKeyboard.scancodes[0x009] = PAL_SCANCODE_8;
    g_WlKeyboard.scancodes[0x00A] = PAL_SCANCODE_9;

    // Function
    g_WlKeyboard.scancodes[0x03B] = PAL_SCANCODE_F1;
    g_WlKeyboard.scancodes[0x03C] = PAL_SCANCODE_F2;
    g_WlKeyboard.scancodes[0x03D] = PAL_SCANCODE_F3;
    g_WlKeyboard.scancodes[0x03E] = PAL_SCANCODE_F4;
    g_WlKeyboard.scancodes[0x03F] = PAL_SCANCODE_F5;
    g_WlKeyboard.scancodes[0x040] = PAL_SCANCODE_F6;
    g_WlKeyboard.scancodes[0x041] = PAL_SCANCODE_F7;
    g_WlKeyboard.scancodes[0x042] = PAL_SCANCODE_F8;
    g_WlKeyboard.scancodes[0x043] = PAL_SCANCODE_F9;
    g_WlKeyboard.scancodes[0x044] = PAL_SCANCODE_F10;
    g_WlKeyboard.scancodes[0x057] = PAL_SCANCODE_F11;
    g_WlKeyboard.scancodes[0x058] = PAL_SCANCODE_F12;

    // Control
    g_WlKeyboard.scancodes[0x001] = PAL_SCANCODE_ESCAPE;
    g_WlKeyboard.scancodes[0x01C] = PAL_SCANCODE_ENTER;
    g_WlKeyboard.scancodes[0x00F] = PAL_SCANCODE_TAB;
    g_WlKeyboard.scancodes[0x00E] = PAL_SCANCODE_BACKSPACE;
    g_WlKeyboard.scancodes[0x039] = PAL_SCANCODE_SPACE;
    g_WlKeyboard.scancodes[0x03A] = PAL_SCANCODE_CAPSLOCK;
    g_WlKeyboard.scancodes[0x045] = PAL_SCANCODE_NUMLOCK;
    g_WlKeyboard.scancodes[0x046] = PAL_SCANCODE_SCROLLLOCK;
    g_WlKeyboard.scancodes[0x02A] = PAL_SCANCODE_LSHIFT;
    g_WlKeyboard.scancodes[0x036] = PAL_SCANCODE_RSHIFT;
    g_WlKeyboard.scancodes[0x01D] = PAL_SCANCODE_LCTRL;
    g_WlKeyboard.scancodes[0x061] = PAL_SCANCODE_RCTRL;
    g_WlKeyboard.scancodes[0x038] = PAL_SCANCODE_LALT;
    g_WlKeyboard.scancodes[0x064] = PAL_SCANCODE_RALT;

    // Arrows
    g_WlKeyboard.scancodes[0x069] = PAL_SCANCODE_LEFT;
    g_WlKeyboard.scancodes[0x06A] = PAL_SCANCODE_RIGHT;
    g_WlKeyboard.scancodes[0x067] = PAL_SCANCODE_UP;
    g_WlKeyboard.scancodes[0x06C] = PAL_SCANCODE_DOWN;

    // Navigation
    g_WlKeyboard.scancodes[0x06E] = PAL_SCANCODE_INSERT;
    g_WlKeyboard.scancodes[0x06F] = PAL_SCANCODE_DELETE;
    g_WlKeyboard.scancodes[0x066] = PAL_SCANCODE_HOME;
    g_WlKeyboard.scancodes[0x067] = PAL_SCANCODE_END;
    g_WlKeyboard.scancodes[0x068] = PAL_SCANCODE_PAGEUP;
    g_WlKeyboard.scancodes[0x06D] = PAL_SCANCODE_PAGEDOWN;

    // Keypad
    g_WlKeyboard.scancodes[0x052] = PAL_SCANCODE_KP_0;
    g_WlKeyboard.scancodes[0x04F] = PAL_SCANCODE_KP_1;
    g_WlKeyboard.scancodes[0x050] = PAL_SCANCODE_KP_2;
    g_WlKeyboard.scancodes[0x051] = PAL_SCANCODE_KP_3;
    g_WlKeyboard.scancodes[0x04B] = PAL_SCANCODE_KP_4;
    g_WlKeyboard.scancodes[0x04C] = PAL_SCANCODE_KP_5;
    g_WlKeyboard.scancodes[0x04D] = PAL_SCANCODE_KP_6;
    g_WlKeyboard.scancodes[0x047] = PAL_SCANCODE_KP_7;
    g_WlKeyboard.scancodes[0x048] = PAL_SCANCODE_KP_8;
    g_WlKeyboard.scancodes[0x049] = PAL_SCANCODE_KP_9;
    g_WlKeyboard.scancodes[0x060] = PAL_SCANCODE_KP_ENTER;
    g_WlKeyboard.scancodes[0x04E] = PAL_SCANCODE_KP_ADD;
    g_WlKeyboard.scancodes[0x04A] = PAL_SCANCODE_KP_SUBTRACT;
    g_WlKeyboard.scancodes[0x037] = PAL_SCANCODE_KP_MULTIPLY;
    g_WlKeyboard.scancodes[0x062] = PAL_SCANCODE_KP_DIVIDE;
    g_WlKeyboard.scancodes[0x053] = PAL_SCANCODE_KP_DECIMAL;

    // Misc
    g_WlKeyboard.scancodes[0x063] = PAL_SCANCODE_PRINTSCREEN;
    g_WlKeyboard.scancodes[0x066] = PAL_SCANCODE_PAUSE;
    g_WlKeyboard.scancodes[0x07F] = PAL_SCANCODE_MENU;
    g_WlKeyboard.scancodes[0x028] = PAL_SCANCODE_APOSTROPHE;
    g_WlKeyboard.scancodes[0x02B] = PAL_SCANCODE_BACKSLASH;
    g_WlKeyboard.scancodes[0x033] = PAL_SCANCODE_COMMA;
    g_WlKeyboard.scancodes[0x00D] = PAL_SCANCODE_EQUAL;
    g_WlKeyboard.scancodes[0x029] = PAL_SCANCODE_GRAVEACCENT;
    g_WlKeyboard.scancodes[0x00C] = PAL_SCANCODE_SUBTRACT;
    g_WlKeyboard.scancodes[0x034] = PAL_SCANCODE_PERIOD;
    g_WlKeyboard.scancodes[0x027] = PAL_SCANCODE_SEMICOLON;
    g_WlKeyboard.scancodes[0x035] = PAL_SCANCODE_SLASH;
    g_WlKeyboard.scancodes[0x01A] = PAL_SCANCODE_LBRACKET;
    g_WlKeyboard.scancodes[0x01B] = PAL_SCANCODE_RBRACKET;
    g_WlKeyboard.scancodes[0x07D] = PAL_SCANCODE_LSUPER;
    g_WlKeyboard.scancodes[0x07E] = PAL_SCANCODE_RSUPER;
}

static void createKeycodeTable(void)
{
    // Tis is for only printable and text input keys

    // Letters
    g_WlKeyboard.keycodes[XKB_KEY_a] = PAL_KEYCODE_A;
    g_WlKeyboard.keycodes[XKB_KEY_b] = PAL_KEYCODE_B;
    g_WlKeyboard.keycodes[XKB_KEY_c] = PAL_KEYCODE_C;
    g_WlKeyboard.keycodes[XKB_KEY_d] = PAL_KEYCODE_D;
    g_WlKeyboard.keycodes[XKB_KEY_e] = PAL_KEYCODE_E;
    g_WlKeyboard.keycodes[XKB_KEY_f] = PAL_KEYCODE_F;
    g_WlKeyboard.keycodes[XKB_KEY_g] = PAL_KEYCODE_G;
    g_WlKeyboard.keycodes[XKB_KEY_h] = PAL_KEYCODE_H;
    g_WlKeyboard.keycodes[XKB_KEY_i] = PAL_KEYCODE_I;
    g_WlKeyboard.keycodes[XKB_KEY_j] = PAL_KEYCODE_J;
    g_WlKeyboard.keycodes[XKB_KEY_k] = PAL_KEYCODE_K;
    g_WlKeyboard.keycodes[XKB_KEY_l] = PAL_KEYCODE_L;
    g_WlKeyboard.keycodes[XKB_KEY_m] = PAL_KEYCODE_M;
    g_WlKeyboard.keycodes[XKB_KEY_n] = PAL_KEYCODE_N;
    g_WlKeyboard.keycodes[XKB_KEY_o] = PAL_KEYCODE_O;
    g_WlKeyboard.keycodes[XKB_KEY_p] = PAL_KEYCODE_P;
    g_WlKeyboard.keycodes[XKB_KEY_q] = PAL_KEYCODE_Q;
    g_WlKeyboard.keycodes[XKB_KEY_r] = PAL_KEYCODE_R;
    g_WlKeyboard.keycodes[XKB_KEY_s] = PAL_KEYCODE_S;
    g_WlKeyboard.keycodes[XKB_KEY_t] = PAL_KEYCODE_T;
    g_WlKeyboard.keycodes[XKB_KEY_u] = PAL_KEYCODE_U;
    g_WlKeyboard.keycodes[XKB_KEY_v] = PAL_KEYCODE_V;
    g_WlKeyboard.keycodes[XKB_KEY_w] = PAL_KEYCODE_W;
    g_WlKeyboard.keycodes[XKB_KEY_x] = PAL_KEYCODE_X;
    g_WlKeyboard.keycodes[XKB_KEY_y] = PAL_KEYCODE_Y;
    g_WlKeyboard.keycodes[XKB_KEY_z] = PAL_KEYCODE_Z;

    // Control
    g_WlKeyboard.keycodes[XKB_KEY_space] = PAL_KEYCODE_SPACE;

    // Misc
    g_WlKeyboard.keycodes[XKB_KEY_apostrophe] = PAL_KEYCODE_APOSTROPHE;
    g_WlKeyboard.keycodes[XKB_KEY_backslash] = PAL_KEYCODE_BACKSLASH;
    g_WlKeyboard.keycodes[XKB_KEY_comma] = PAL_KEYCODE_COMMA;
    g_WlKeyboard.keycodes[XKB_KEY_equal] = PAL_KEYCODE_EQUAL;
    g_WlKeyboard.keycodes[XKB_KEY_grave] = PAL_KEYCODE_GRAVEACCENT;
    g_WlKeyboard.keycodes[XKB_KEY_minus] = PAL_KEYCODE_SUBTRACT;
    g_WlKeyboard.keycodes[XKB_KEY_period] = PAL_KEYCODE_PERIOD;
    g_WlKeyboard.keycodes[XKB_KEY_semicolon] = PAL_KEYCODE_SEMICOLON;
    g_WlKeyboard.keycodes[XKB_KEY_slash] = PAL_KEYCODE_SLASH;
    g_WlKeyboard.keycodes[XKB_KEY_bracketleft] = PAL_KEYCODE_LBRACKET;
    g_WlKeyboard.keycodes[XKB_KEY_bracketright] = PAL_KEYCODE_RBRACKET;
}

static inline uint64_t getCurrentTime()
{
    uint64_t now = palGetPerformanceCounter();
    return (now * 1000) / g_WlKeyboard.frequency;
}

static void getDriverFeatures(const char* interface)
{
    PalVideoFeatures features = 0;
    features |= PAL_VIDEO_FEATURE_HIGH_DPI;
    features |= PAL_VIDEO_FEATURE_MONITOR_GET_ORIENTATION;
    features |= PAL_VIDEO_FEATURE_MULTI_MONITORS;
    features |= PAL_VIDEO_FEATURE_MONITOR_GET_MODE;
    features |= PAL_VIDEO_FEATURE_WINDOW_SET_TITLE;
    features |= PAL_VIDEO_FEATURE_WINDOW_SET_SIZE;
    features |= PAL_VIDEO_FEATURE_WINDOW_SET_STATE;
    features |= PAL_VIDEO_FEATURE_BORDERLESS_WINDOW;
    features |= PAL_VIDEO_FEATURE_WINDOW_SET_CURSOR;

    if (strcmp(interface, "zxdg_decoration_manager_v1") == 0) {
        features |= PAL_VIDEO_FEATURE_DECORATED_WINDOW;
    }

    s_WaylandDriver.features = features;
}

static void getCursorTheme()
{
    const char* theme = getenv("XCURSOR_THEME");
    if (theme && theme[0] == '\0') {
        theme = nullptr;
    }

    int themeSize = 24;
    const char* tmp = getenv("XCURSOR_SIZE");
    if (tmp && tmp[0] != '\0') {
        char* end = nullptr;
        long parsed = strtol(tmp, &end, 10);
        if (end != tmp && parsed > 0 && parsed <= 256) {
            themeSize = (int)parsed;
        }
    }

    g_Wl.cursorTheme = g_Wl.cursorThemeLoad(theme, themeSize, g_Wl.shm);
}

static void driverGlobalHandle(
    void* data,
    struct wl_registry* registry,
    uint32_t name,
    const char* interface,
    uint32_t version)
{
    (void)data;
    (void)registry;
    (void)name;
    (void)version;

    getDriverFeatures(interface);
}

static void driverGlobalRemove(
    void* data,
    struct wl_registry* registry,
    uint32_t name)
{
    (void)data;
    (void)registry;
    (void)name;
}

static void globalHandle(
    void* data,
    struct wl_registry* registry,
    uint32_t name,
    const char* interface,
    uint32_t version)
{
    (void)data;
    (void)interface;
    (void)version;

    // wlGetVideoDriver() has not been called
    if (s_WaylandDriver.features == 0) {
        getDriverFeatures(interface);
    }

    if (strcmp(interface, "wl_compositor") == 0) {
        g_Wl.compositor = wlRegistryBind(registry, name, g_Wl.compositorInterface, 4);

    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        g_Wl.xdgBase = wlRegistryBind(registry, name, &xdgWmBaseInterface, 1);
        xdgWmBaseAddListener(g_Wl.xdgBase, &g_WmBaseListener, nullptr);

    } else if (strcmp(interface, "wl_shm") == 0) {
        g_Wl.shm = wlRegistryBind(registry, name, g_Wl.shmInterface, 1);

    } else if (strcmp(interface, "wl_seat") == 0) {
        g_Wl.seat = wlRegistryBind(registry, name, g_Wl.seatInterface, 5);
        wlSeatAddListener(g_Wl.seat, &g_SeatListener, nullptr);

    } else if (strcmp(interface, "zxdg_decoration_manager_v1") == 0) {
        g_Wl.decorationManager = wlRegistryBind(registry, name, &zxdg_decoration_manager_v1_interface, 1);

    } else if (strcmp(interface, "wl_output") == 0) {
        // wayland does not let use query monitors directly so we enumerate and store
        // at init and update the cache when a monitor is added or removed.
        MonitorData* monitorData = getFreeMonitorData();
        if (!monitorData) {
            return;
        }

        struct wl_output* output = nullptr;
        output = wlRegistryBind(g_Wl.registry, name, g_Wl.outputInterface, 3);
        wlOutputAddListener(output, &g_OutputListener, monitorData);

        monitorData->wlName = name;
        monitorData->monitor = (PalMonitor*)output;
        g_Wl.monitorCount++;
    }
}

static void globalRemove(
    void* data,
    struct wl_registry* registry,
    uint32_t name)
{
    (void)data;
    (void)registry;

    for (uint32_t i = 0; i < g_Wl.maxMonitorData; ++i) {
        if (g_Wl.monitorData[i].used && g_Wl.monitorData[i].wlName == name) {
            MonitorData* data = &g_Wl.monitorData[i];
            data->used = PAL_FALSE;

            g_Wl.proxyDestroy((struct wl_proxy*)data->monitor);
            g_Wl.monitorCount--;
        }
    }
}

struct wl_registry_listener s_DriverRegistryListener = {
    .global = driverGlobalHandle,
    .global_remove = driverGlobalRemove,
};

struct wl_registry_listener s_RegistryListener = {
    .global = globalHandle,
    .global_remove = globalRemove,
};

/* =========================================================================
 * API
 * ========================================================================= */

void wlShutdownVideo(void);
void wlDestroyCursor(PalCursor* cursor);

PalResult wlCreateCursorFrom(
    PalCursorType type,
    PalCursor** cursor);

PalVideoDriver* wlGetVideoDriver(void)
{
    if (s_WaylandDriver.features != 0) {
        return &s_WaylandDriver;
    }

    // if wlInitVideo() has been called, we dont want to load again
    if (!g_Wl.libwayland) {
        g_Wl.libwayland = palLoadLibrary("libwayland-client.so.0");
        if (g_Wl.libwayland) {
            loadGlobals();
            loadProcs(PAL_FALSE);
            setupProtocols();

            g_Wl.display = g_Wl.displayConnect(nullptr);
            if (g_Wl.display) {
                g_Wl.registry = wlDisplayGetRegistry(g_Wl.display);
                wlRegistryAddListener(g_Wl.registry, &s_DriverRegistryListener, nullptr);

                g_Wl.displayRoundtrip(g_Wl.display);
                g_Wl.displayDisconnect(g_Wl.display);
            }

            palFreeLibrary(g_Wl.libwayland);
            memset(&g_Wl, 0, sizeof(Wayland));
        }
    }

    s_WaylandDriver.type = PAL_VIDEO_DRIVER_TYPE_WAYLAND;
    return &s_WaylandDriver;
}

PalResult wlInitVideo(
    const PalAllocator* allocator,
    struct PalEventDriver* eventDriver,
    const void* preferredInstance,
    void** selectedInstance)
{
    PalResult result = PAL_RESULT_CODE_PLATFORM_FAILURE;
    g_Wl.allocator = allocator;
    g_Wl.eventDriver = eventDriver;

    g_Wl.libwayland = palLoadLibrary("libwayland-client.so.0");
    g_Wl.xkbcommon = palLoadLibrary("libxkbcommon.so");
    g_Wl.libcursor = palLoadLibrary("libwayland-cursor.so");
    g_Wl.libegl = palLoadLibrary("libwayland-egl.so");
    if (!g_Wl.libwayland || !g_Wl.xkbcommon || !g_Wl.libcursor || !g_Wl.libegl) {
        result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_POSIX, (uint32_t)errno);
        goto cleanup;
    }

    loadGlobals();
    loadProcs(PAL_TRUE);
    setupProtocols();
    createScancodeTable();
    createKeycodeTable();

    g_Wl.maxMonitorData = 16;
    g_Wl.maxWindowData = 32;
    g_Wl.windowData = palAllocate(allocator, sizeof(WindowData) * g_Wl.maxWindowData, 0);
    g_Wl.monitorData = palAllocate(allocator, sizeof(MonitorData) * g_Wl.maxMonitorData, 0);

    if (!g_Wl.monitorData || !g_Wl.windowData) {
        result = PAL_RESULT_CODE_OUT_OF_MEMORY;
        goto cleanup;
    }

    memset(g_Wl.windowData, 0, sizeof(WindowData) * g_Wl.maxWindowData);
    memset(g_Wl.monitorData, 0, sizeof(MonitorData) * g_Wl.maxMonitorData);

    g_Wl.monitorCount = 0;
    if (preferredInstance) {
        g_Wl.display = (struct wl_display*)preferredInstance;
        g_Wl.ownsDisplay = PAL_FALSE;
        *selectedInstance = (void*)preferredInstance;

    } else {
        g_Wl.display = g_Wl.displayConnect(nullptr);
        if (!g_Wl.display) {
            result = PAL_RESULT_CODE_PLATFORM_FAILURE;
            goto cleanup;
        }
        g_Wl.ownsDisplay = PAL_TRUE;
        *selectedInstance = (void*)g_Wl.display;
    }

    g_Wl.registry = wlDisplayGetRegistry(g_Wl.display);
    wlRegistryAddListener(g_Wl.registry, &s_RegistryListener, nullptr);
    g_Wl.displayRoundtrip(g_Wl.display);
    g_Wl.displayRoundtrip(g_Wl.display); // do a roundtrip again to get remaining handles

    if (!g_Wl.compositor || !g_Wl.xdgBase || !g_Wl.shm) {
        result = PAL_RESULT_CODE_PLATFORM_FAILURE;
        goto cleanup;
    }

    g_Wl.inputContext = g_Wl.xkbContextNew(XKB_CONTEXT_NO_FLAGS);
    if (!g_Wl.inputContext) {
        result = PAL_RESULT_CODE_PLATFORM_FAILURE;
        goto cleanup;
    }

    getCursorTheme();
    if (!g_Wl.cursorTheme) {
        result = PAL_RESULT_CODE_PLATFORM_FAILURE;
        goto cleanup;
    }

    // create a default arrow cursor for revert
    PalCursor* tmp = nullptr;
    result = wlCreateCursorFrom(PAL_CURSOR_TYPE_ARROW, &tmp);
    if (result != PAL_RESULT_SUCCESS) {
        goto cleanup;
    }
    g_Wl.defaultCursor = (WaylandCursor*)tmp;

    return PAL_RESULT_SUCCESS;

cleanup:
    wlShutdownVideo();
    return result;
}

void wlShutdownVideo(void)
{
    if (g_Wl.state) {
        g_Wl.xkbStateUnref(g_Wl.state);
    }

    if (g_Wl.keymap) {
        g_Wl.xkbKeymapUnref(g_Wl.keymap);
    }

    if (g_Wl.inputContext) {
        g_Wl.xkbContextUnref(g_Wl.inputContext);
    }

    if (g_Wl.monitorData) {
        for (uint32_t i = 0; i < g_Wl.maxMonitorData; ++i) {
            if (g_Wl.monitorData[i].used && g_Wl.monitorData[i].monitor) {
                g_Wl.proxyDestroy((struct wl_proxy*)g_Wl.monitorData[i].monitor);
            }
        }

        palFree(g_Wl.allocator, g_Wl.monitorData);
    }

    if (g_Wl.windowData) {
        palFree(g_Wl.allocator, g_Wl.windowData);
    }

    if (g_Wl.defaultCursor) {
        wlDestroyCursor((PalCursor*)g_Wl.defaultCursor);
    }

    if (g_Wl.pointer) {
        g_Wl.proxyDestroy((struct wl_proxy*)g_Wl.pointer);
    }

    if (g_Wl.keyboard) {
        g_Wl.proxyDestroy((struct wl_proxy*)g_Wl.keyboard);
    }

    if (g_Wl.seat) {
        g_Wl.proxyDestroy((struct wl_proxy*)g_Wl.seat);
    }

    if (g_Wl.decorationManager) {
        g_Wl.proxyDestroy((struct wl_proxy*)g_Wl.decorationManager);
    }

    if (g_Wl.shm) {
        g_Wl.proxyDestroy((struct wl_proxy*)g_Wl.shm);
    }

    if (g_Wl.xdgBase) {
        g_Wl.proxyDestroy((struct wl_proxy*)g_Wl.xdgBase);
    }

    if (g_Wl.compositor) {
        g_Wl.proxyDestroy((struct wl_proxy*)g_Wl.compositor);
    }

    if (g_Wl.registry) {
        g_Wl.proxyDestroy((struct wl_proxy*)g_Wl.registry);
    }

    if (g_Wl.ownsDisplay && g_Wl.display) {
        g_Wl.displayDisconnect(g_Wl.display);
    }

    if (g_Wl.libcursor) {
        palFreeLibrary(g_Wl.libcursor);
    }

    if (g_Wl.xkbcommon) {
        palFreeLibrary(g_Wl.xkbcommon);
    }

    if (g_Wl.libegl) {
        palFreeLibrary(g_Wl.libegl);
    }

    if (g_Wl.libwayland) {
        palFreeLibrary(g_Wl.libwayland);
    }

    unloadVideoEGL();
    memset(&g_Wl, 0, sizeof(Wayland));
    memset(&g_WlKeyboard, 0, sizeof(WaylandKeyboard));
    memset(&g_WlMouse, 0, sizeof(WaylandMouse));
}

void wlUpdateVideo(void)
{
    // flush pending requests
    g_WlMouse.tmpScrollX = 0;
    g_WlMouse.tmpScrollY = 0;

#if PAL_HAS_EVENT_MODULE
    // key repeats. we only do this if the user wants key repeat events
    if (g_WlKeyboard.repeatKey != 0 && g_WlKeyboard.repeatRate > 0 && g_Wl.eventDriver) {
        PalEventDriver* driver = g_Wl.eventDriver;
        PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
        mode = palGetEventDispatchMode(driver, PAL_EVENT_TYPE_KEYREPEAT);
        if (mode != PAL_DISPATCH_MODE_NONE) {
            uint64_t now = getCurrentTime();
            if (now >= g_WlKeyboard.timer) {
                PalWindow* window = (PalWindow*)g_Wl.keyboardSurface;
                PalKeycode key = (PalKeycode)g_WlKeyboard.repeatKey;
                PalScancode scancode = (PalScancode)g_WlKeyboard.repeatScancode;
                PalEvent event = {0};
                event.type = PAL_EVENT_TYPE_KEYREPEAT;
                event.data = palPackUint32(key, scancode);
                event.data2 = palPackPointer(window);
                palPushEvent(driver, &event);
                g_WlKeyboard.timer += 1000UL / (uint64_t)g_WlKeyboard.repeatRate;
            }
        }
    }
#endif // PAL_HAS_EVENT_MODULE

    while (g_Wl.prepareRead(g_Wl.display) != 0) {
        g_Wl.dispatchPending(g_Wl.display);
    }

    g_Wl.displayFlush(g_Wl.display);
    int fd = g_Wl.displayGetFd(g_Wl.display);
    struct pollfd pfd = {fd, POLLIN, 0};

    if (poll(&pfd, 1, 0) > 0) {
        g_Wl.readEvents(g_Wl.display);
    } else {
        g_Wl.cancelRead(g_Wl.display);
    }

    // dispatch events that were read
    g_Wl.dispatchPending(g_Wl.display);
}

#endif // PAL_HAS_WAYLAND_BACKEND