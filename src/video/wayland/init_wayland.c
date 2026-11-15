
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
#include "video/platform.h"
#include "pal2/video/init.h"
#include "pal2/core/time.h"

#include "wayland_helper.h"
#include "listeners.h"
#include "video/video_egl.h"

#include <dlfcn.h>
#include <errno.h>
#include <string.h>
#include <poll.h>

Wayland g_Wayland = {0};
WaylandKeyboard g_WaylandKeyboard = {0};
WaylandMouse g_WaylandMouse = {0};
VideoEGL g_VideoEgl = {0};

extern struct wl_interface xdgWmBaseInterface;
static PalVideoDriver s_WaylandDriver = {0};

/* =========================================================================
 * Helpers
 * ========================================================================= */

static void loadGlobals(void)
{
    WaylandInterfaces* interfaces = &g_Wayland.interfaces;
    PalLibrary* wayland = g_Wayland.procs.handle;

    interfaces->output = palGetGlobal(wayland, "wl_output_interface");
    interfaces->seat = palGetGlobal(wayland, "wl_seat_interface");
    interfaces->compositor = palGetGlobal(wayland, "wl_compositor_interface");
    interfaces->surface = palGetGlobal(wayland, "wl_surface_interface");
    interfaces->registry = palGetGlobal(wayland, "wl_registry_interface");
    interfaces->shm = palGetGlobal(wayland, "wl_shm_interface");
    interfaces->buffer = palGetGlobal(wayland, "wl_buffer_interface");
    interfaces->shmPool = palGetGlobal(wayland, "wl_shm_pool_interface");
    interfaces->region = palGetGlobal(wayland, "wl_region_interface");
    interfaces->pointer = palGetGlobal(wayland, "wl_pointer_interface");
    interfaces->keyboard = palGetGlobal(wayland, "wl_keyboard_interface");
}

static void loadProcs(PalBool loadAll)
{
    WaylandProcs* procs = &g_Wayland.procs;
    XkbProcs* xkbProcs = &g_Wayland.xkbProcs;
    WaylandCursorProcs* cursorProcs = &g_Wayland.cursorProcs;
    WaylandEglProcs* eglProcs = &g_Wayland.eglProcs;

    PalLibrary* wayland = procs->handle;
    PalLibrary* xkb = xkbProcs->handle;
    PalLibrary* cursor = cursorProcs->handle;
    PalLibrary* egl = eglProcs->handle;
    const char* marshal_proc = "wl_proxy_marshal_constructor_versioned";

    procs->displayConnect = (wlDisplayConnectFn)palGetSymbol(wayland, "wl_display_connect");
    procs->displayDisconnect = (wlDisplayDisconnectFn)palGetSymbol(wayland, "wl_display_disconnect");
    procs->displayRoundtrip = (wlDisplayRoundtripFn)palGetSymbol(wayland, "wl_display_roundtrip");
    procs->displayDispatch = (wlDisplayDispatchFn)palGetSymbol(wayland, "wl_display_dispatch");
    procs->proxyAddListener = (wlProxyAddListenerFn)palGetSymbol(wayland, "wl_proxy_add_listener");
    procs->proxyDestroy = (wlProxyDestroyFn)palGetSymbol(wayland, "wl_proxy_destroy");
    procs->proxyMarshalFlags = (wlProxyMarshalFlagsFn)palGetSymbol(wayland, "wl_proxy_marshal_flags");
    procs->proxyGetVersion = (wlProxyGetVersionFn)palGetSymbol(wayland, "wl_proxy_get_version");
    procs->getError = (wlDisplayGetErrorFn)palGetSymbol(wayland, "wl_display_get_error");
    procs->dispatchPending = (wlDisplayDispatchPendingFn)palGetSymbol(wayland, "wl_display_dispatch_pending");
    procs->displayFlush = (wlDisplayFlushFn)palGetSymbol(wayland, "wl_display_flush");
    procs->prepareRead = (wlDisplayPrepareReadFn)palGetSymbol(wayland, "wl_display_prepare_read");
    procs->readEvents = (wlDisplayReadEventsFn)palGetSymbol(wayland, "wl_display_read_events");
    procs->displayGetFd = (wlDisplayGetFdFn)palGetSymbol(wayland, "wl_display_get_fd");
    procs->cancelRead = (wlDisplayCancelReadFn)palGetSymbol(wayland, "wl_display_cancel_read");
    procs->proxyMarshalConstructor = (wlProxyMarshalConstructorVFn)palGetSymbol(wayland, marshal_proc);

    if (!loadAll) {
        return;
    }

    // xkbcommon
    xkbProcs->keymapUnref = (xkbKeymapUnrefFn)palGetSymbol(xkb, "xkb_keymap_unref");
    xkbProcs->stateKeyGetOneSym = (xkbStateKeyGetOneSymFn)palGetSymbol(xkb, "xkb_state_key_get_one_sym");
    xkbProcs->stateNew = (xkbStateNewFn)palGetSymbol(xkb, "xkb_state_new");
    xkbProcs->stateUnref = (xkbStateUnrefFn)palGetSymbol(xkb, "xkb_state_unref");
    xkbProcs->contextUnref = (xkbContextUnrefFn)palGetSymbol(xkb, "xkb_context_unref");
    xkbProcs->contextNew = (xkbContextNewFn)palGetSymbol(xkb, "xkb_context_new");
    xkbProcs->keymapNewFromString = (xkbKeymapNewFromStringFn)palGetSymbol(xkb, "xkb_keymap_new_from_string");
    xkbProcs->keysymToUtf32 = (xkbKeysymToUtf32Fn)palGetSymbol(xkb, "xkb_keysym_to_utf32");
    xkbProcs->stateUpdateMask = (xkbStateUpdateMaskFn)palGetSymbol(xkb, "xkb_state_update_mask");
    xkbProcs->keymapKeyRepeats = (xkbKeymapKeyRepeatsFn)palGetSymbol(xkb, "xkb_keymap_key_repeats");

    // wayland-cursor
    cursorProcs->getImageBuffer = (wlCursorImageGetBufferFn)palGetSymbol(cursor, "wl_cursor_image_get_buffer");
    cursorProcs->loadTheme = (wlCursorThemeLoadFn)palGetSymbol(cursor, "wl_cursor_theme_load");
    cursorProcs->getCursorTheme = (wlCursorThemeGetCursorFn)palGetSymbol(cursor, "wl_cursor_theme_get_cursor");

    // wl-egl-window
    eglProcs->createWindow = (wlEglWindowCreateFn)palGetSymbol(egl, "wl_egl_window_create");
    eglProcs->destroyWindow = (wlEglWindowDestroyFn)palGetSymbol(egl, "wl_egl_window_destroy");
    eglProcs->resizeWindow = (wlEglWindowResizeFn)palGetSymbol(egl, "wl_egl_window_resize");
}

void createScancodeTable(void)
{
    // Letters
    g_WaylandKeyboard.scancodes[0x01E] = PAL_SCANCODE_A;
    g_WaylandKeyboard.scancodes[0x030] = PAL_SCANCODE_B;
    g_WaylandKeyboard.scancodes[0x02E] = PAL_SCANCODE_C;
    g_WaylandKeyboard.scancodes[0x020] = PAL_SCANCODE_D;
    g_WaylandKeyboard.scancodes[0x012] = PAL_SCANCODE_E;
    g_WaylandKeyboard.scancodes[0x021] = PAL_SCANCODE_F;
    g_WaylandKeyboard.scancodes[0x022] = PAL_SCANCODE_G;
    g_WaylandKeyboard.scancodes[0x023] = PAL_SCANCODE_H;
    g_WaylandKeyboard.scancodes[0x017] = PAL_SCANCODE_I;
    g_WaylandKeyboard.scancodes[0x024] = PAL_SCANCODE_J;
    g_WaylandKeyboard.scancodes[0x025] = PAL_SCANCODE_K;
    g_WaylandKeyboard.scancodes[0x026] = PAL_SCANCODE_L;
    g_WaylandKeyboard.scancodes[0x032] = PAL_SCANCODE_M;
    g_WaylandKeyboard.scancodes[0x031] = PAL_SCANCODE_N;
    g_WaylandKeyboard.scancodes[0x018] = PAL_SCANCODE_O;
    g_WaylandKeyboard.scancodes[0x019] = PAL_SCANCODE_P;
    g_WaylandKeyboard.scancodes[0x010] = PAL_SCANCODE_Q;
    g_WaylandKeyboard.scancodes[0x013] = PAL_SCANCODE_R;
    g_WaylandKeyboard.scancodes[0x01F] = PAL_SCANCODE_S;
    g_WaylandKeyboard.scancodes[0x014] = PAL_SCANCODE_T;
    g_WaylandKeyboard.scancodes[0x016] = PAL_SCANCODE_U;
    g_WaylandKeyboard.scancodes[0x02F] = PAL_SCANCODE_V;
    g_WaylandKeyboard.scancodes[0x011] = PAL_SCANCODE_W;
    g_WaylandKeyboard.scancodes[0x02D] = PAL_SCANCODE_X;
    g_WaylandKeyboard.scancodes[0x015] = PAL_SCANCODE_Y;
    g_WaylandKeyboard.scancodes[0x02C] = PAL_SCANCODE_Z;

    // Numbers (top row)
    g_WaylandKeyboard.scancodes[0x00B] = PAL_SCANCODE_0;
    g_WaylandKeyboard.scancodes[0x002] = PAL_SCANCODE_1;
    g_WaylandKeyboard.scancodes[0x003] = PAL_SCANCODE_2;
    g_WaylandKeyboard.scancodes[0x004] = PAL_SCANCODE_3;
    g_WaylandKeyboard.scancodes[0x005] = PAL_SCANCODE_4;
    g_WaylandKeyboard.scancodes[0x006] = PAL_SCANCODE_5;
    g_WaylandKeyboard.scancodes[0x007] = PAL_SCANCODE_6;
    g_WaylandKeyboard.scancodes[0x008] = PAL_SCANCODE_7;
    g_WaylandKeyboard.scancodes[0x009] = PAL_SCANCODE_8;
    g_WaylandKeyboard.scancodes[0x00A] = PAL_SCANCODE_9;

    // Function
    g_WaylandKeyboard.scancodes[0x03B] = PAL_SCANCODE_F1;
    g_WaylandKeyboard.scancodes[0x03C] = PAL_SCANCODE_F2;
    g_WaylandKeyboard.scancodes[0x03D] = PAL_SCANCODE_F3;
    g_WaylandKeyboard.scancodes[0x03E] = PAL_SCANCODE_F4;
    g_WaylandKeyboard.scancodes[0x03F] = PAL_SCANCODE_F5;
    g_WaylandKeyboard.scancodes[0x040] = PAL_SCANCODE_F6;
    g_WaylandKeyboard.scancodes[0x041] = PAL_SCANCODE_F7;
    g_WaylandKeyboard.scancodes[0x042] = PAL_SCANCODE_F8;
    g_WaylandKeyboard.scancodes[0x043] = PAL_SCANCODE_F9;
    g_WaylandKeyboard.scancodes[0x044] = PAL_SCANCODE_F10;
    g_WaylandKeyboard.scancodes[0x057] = PAL_SCANCODE_F11;
    g_WaylandKeyboard.scancodes[0x058] = PAL_SCANCODE_F12;

    // Control
    g_WaylandKeyboard.scancodes[0x001] = PAL_SCANCODE_ESCAPE;
    g_WaylandKeyboard.scancodes[0x01C] = PAL_SCANCODE_ENTER;
    g_WaylandKeyboard.scancodes[0x00F] = PAL_SCANCODE_TAB;
    g_WaylandKeyboard.scancodes[0x00E] = PAL_SCANCODE_BACKSPACE;
    g_WaylandKeyboard.scancodes[0x039] = PAL_SCANCODE_SPACE;
    g_WaylandKeyboard.scancodes[0x03A] = PAL_SCANCODE_CAPSLOCK;
    g_WaylandKeyboard.scancodes[0x045] = PAL_SCANCODE_NUMLOCK;
    g_WaylandKeyboard.scancodes[0x046] = PAL_SCANCODE_SCROLLLOCK;
    g_WaylandKeyboard.scancodes[0x02A] = PAL_SCANCODE_LSHIFT;
    g_WaylandKeyboard.scancodes[0x036] = PAL_SCANCODE_RSHIFT;
    g_WaylandKeyboard.scancodes[0x01D] = PAL_SCANCODE_LCTRL;
    g_WaylandKeyboard.scancodes[0x061] = PAL_SCANCODE_RCTRL;
    g_WaylandKeyboard.scancodes[0x038] = PAL_SCANCODE_LALT;
    g_WaylandKeyboard.scancodes[0x064] = PAL_SCANCODE_RALT;

    // Arrows
    g_WaylandKeyboard.scancodes[0x069] = PAL_SCANCODE_LEFT;
    g_WaylandKeyboard.scancodes[0x06A] = PAL_SCANCODE_RIGHT;
    g_WaylandKeyboard.scancodes[0x067] = PAL_SCANCODE_UP;
    g_WaylandKeyboard.scancodes[0x06C] = PAL_SCANCODE_DOWN;

    // Navigation
    g_WaylandKeyboard.scancodes[0x06E] = PAL_SCANCODE_INSERT;
    g_WaylandKeyboard.scancodes[0x06F] = PAL_SCANCODE_DELETE;
    g_WaylandKeyboard.scancodes[0x066] = PAL_SCANCODE_HOME;
    g_WaylandKeyboard.scancodes[0x067] = PAL_SCANCODE_END;
    g_WaylandKeyboard.scancodes[0x068] = PAL_SCANCODE_PAGEUP;
    g_WaylandKeyboard.scancodes[0x06D] = PAL_SCANCODE_PAGEDOWN;

    // Keypad
    g_WaylandKeyboard.scancodes[0x052] = PAL_SCANCODE_KP_0;
    g_WaylandKeyboard.scancodes[0x04F] = PAL_SCANCODE_KP_1;
    g_WaylandKeyboard.scancodes[0x050] = PAL_SCANCODE_KP_2;
    g_WaylandKeyboard.scancodes[0x051] = PAL_SCANCODE_KP_3;
    g_WaylandKeyboard.scancodes[0x04B] = PAL_SCANCODE_KP_4;
    g_WaylandKeyboard.scancodes[0x04C] = PAL_SCANCODE_KP_5;
    g_WaylandKeyboard.scancodes[0x04D] = PAL_SCANCODE_KP_6;
    g_WaylandKeyboard.scancodes[0x047] = PAL_SCANCODE_KP_7;
    g_WaylandKeyboard.scancodes[0x048] = PAL_SCANCODE_KP_8;
    g_WaylandKeyboard.scancodes[0x049] = PAL_SCANCODE_KP_9;
    g_WaylandKeyboard.scancodes[0x060] = PAL_SCANCODE_KP_ENTER;
    g_WaylandKeyboard.scancodes[0x04E] = PAL_SCANCODE_KP_ADD;
    g_WaylandKeyboard.scancodes[0x04A] = PAL_SCANCODE_KP_SUBTRACT;
    g_WaylandKeyboard.scancodes[0x037] = PAL_SCANCODE_KP_MULTIPLY;
    g_WaylandKeyboard.scancodes[0x062] = PAL_SCANCODE_KP_DIVIDE;
    g_WaylandKeyboard.scancodes[0x053] = PAL_SCANCODE_KP_DECIMAL;

    // Misc
    g_WaylandKeyboard.scancodes[0x063] = PAL_SCANCODE_PRINTSCREEN;
    g_WaylandKeyboard.scancodes[0x066] = PAL_SCANCODE_PAUSE;
    g_WaylandKeyboard.scancodes[0x07F] = PAL_SCANCODE_MENU;
    g_WaylandKeyboard.scancodes[0x028] = PAL_SCANCODE_APOSTROPHE;
    g_WaylandKeyboard.scancodes[0x02B] = PAL_SCANCODE_BACKSLASH;
    g_WaylandKeyboard.scancodes[0x033] = PAL_SCANCODE_COMMA;
    g_WaylandKeyboard.scancodes[0x00D] = PAL_SCANCODE_EQUAL;
    g_WaylandKeyboard.scancodes[0x029] = PAL_SCANCODE_GRAVEACCENT;
    g_WaylandKeyboard.scancodes[0x00C] = PAL_SCANCODE_SUBTRACT;
    g_WaylandKeyboard.scancodes[0x034] = PAL_SCANCODE_PERIOD;
    g_WaylandKeyboard.scancodes[0x027] = PAL_SCANCODE_SEMICOLON;
    g_WaylandKeyboard.scancodes[0x035] = PAL_SCANCODE_SLASH;
    g_WaylandKeyboard.scancodes[0x01A] = PAL_SCANCODE_LBRACKET;
    g_WaylandKeyboard.scancodes[0x01B] = PAL_SCANCODE_RBRACKET;
    g_WaylandKeyboard.scancodes[0x07D] = PAL_SCANCODE_LSUPER;
    g_WaylandKeyboard.scancodes[0x07E] = PAL_SCANCODE_RSUPER;
}

void createKeycodeTable(void)
{
    // Tis is for only printable and text input keys

    // Letters
    g_WaylandKeyboard.keycodes[XKB_KEY_a] = PAL_KEYCODE_A;
    g_WaylandKeyboard.keycodes[XKB_KEY_b] = PAL_KEYCODE_B;
    g_WaylandKeyboard.keycodes[XKB_KEY_c] = PAL_KEYCODE_C;
    g_WaylandKeyboard.keycodes[XKB_KEY_d] = PAL_KEYCODE_D;
    g_WaylandKeyboard.keycodes[XKB_KEY_e] = PAL_KEYCODE_E;
    g_WaylandKeyboard.keycodes[XKB_KEY_f] = PAL_KEYCODE_F;
    g_WaylandKeyboard.keycodes[XKB_KEY_g] = PAL_KEYCODE_G;
    g_WaylandKeyboard.keycodes[XKB_KEY_h] = PAL_KEYCODE_H;
    g_WaylandKeyboard.keycodes[XKB_KEY_i] = PAL_KEYCODE_I;
    g_WaylandKeyboard.keycodes[XKB_KEY_j] = PAL_KEYCODE_J;
    g_WaylandKeyboard.keycodes[XKB_KEY_k] = PAL_KEYCODE_K;
    g_WaylandKeyboard.keycodes[XKB_KEY_l] = PAL_KEYCODE_L;
    g_WaylandKeyboard.keycodes[XKB_KEY_m] = PAL_KEYCODE_M;
    g_WaylandKeyboard.keycodes[XKB_KEY_n] = PAL_KEYCODE_N;
    g_WaylandKeyboard.keycodes[XKB_KEY_o] = PAL_KEYCODE_O;
    g_WaylandKeyboard.keycodes[XKB_KEY_p] = PAL_KEYCODE_P;
    g_WaylandKeyboard.keycodes[XKB_KEY_q] = PAL_KEYCODE_Q;
    g_WaylandKeyboard.keycodes[XKB_KEY_r] = PAL_KEYCODE_R;
    g_WaylandKeyboard.keycodes[XKB_KEY_s] = PAL_KEYCODE_S;
    g_WaylandKeyboard.keycodes[XKB_KEY_t] = PAL_KEYCODE_T;
    g_WaylandKeyboard.keycodes[XKB_KEY_u] = PAL_KEYCODE_U;
    g_WaylandKeyboard.keycodes[XKB_KEY_v] = PAL_KEYCODE_V;
    g_WaylandKeyboard.keycodes[XKB_KEY_w] = PAL_KEYCODE_W;
    g_WaylandKeyboard.keycodes[XKB_KEY_x] = PAL_KEYCODE_X;
    g_WaylandKeyboard.keycodes[XKB_KEY_y] = PAL_KEYCODE_Y;
    g_WaylandKeyboard.keycodes[XKB_KEY_z] = PAL_KEYCODE_Z;

    // Control
    g_WaylandKeyboard.keycodes[XKB_KEY_space] = PAL_KEYCODE_SPACE;

    // Misc
    g_WaylandKeyboard.keycodes[XKB_KEY_apostrophe] = PAL_KEYCODE_APOSTROPHE;
    g_WaylandKeyboard.keycodes[XKB_KEY_backslash] = PAL_KEYCODE_BACKSLASH;
    g_WaylandKeyboard.keycodes[XKB_KEY_comma] = PAL_KEYCODE_COMMA;
    g_WaylandKeyboard.keycodes[XKB_KEY_equal] = PAL_KEYCODE_EQUAL;
    g_WaylandKeyboard.keycodes[XKB_KEY_grave] = PAL_KEYCODE_GRAVEACCENT;
    g_WaylandKeyboard.keycodes[XKB_KEY_minus] = PAL_KEYCODE_SUBTRACT;
    g_WaylandKeyboard.keycodes[XKB_KEY_period] = PAL_KEYCODE_PERIOD;
    g_WaylandKeyboard.keycodes[XKB_KEY_semicolon] = PAL_KEYCODE_SEMICOLON;
    g_WaylandKeyboard.keycodes[XKB_KEY_slash] = PAL_KEYCODE_SLASH;
    g_WaylandKeyboard.keycodes[XKB_KEY_bracketleft] = PAL_KEYCODE_LBRACKET;
    g_WaylandKeyboard.keycodes[XKB_KEY_bracketright] = PAL_KEYCODE_RBRACKET;
}

static inline uint64_t getCurrentTime()
{
    uint64_t now = palGetPerformanceCounter();
    return (now * 1000) / g_WaylandKeyboard.frequency;
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
    (void)interface;
    (void)version;

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

    WaylandInterfaces* interfaces = &g_Wayland.interfaces;

    if (strcmp(interface, "wl_compositor") == 0) {
        g_Wayland.compositor = wlRegistryBind(registry, name, interfaces->compositor, 4);

    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        g_Wayland.xdgBase = wlRegistryBind(registry, name, &xdgWmBaseInterface, 1);
        xdgWmBaseAddListener(g_Wayland.xdgBase, &g_WmBaseListener, nullptr);

    } else if (strcmp(interface, "wl_shm") == 0) {
        g_Wayland.shm = wlRegistryBind(registry, name, interfaces->shm, 1);

    } else if (strcmp(interface, "wl_seat") == 0) {
        g_Wayland.seat = wlRegistryBind(registry, name, interfaces->seat, 5);
        wlSeatAddListener(g_Wayland.seat, &g_SeatListener, nullptr);

    } else if (strcmp(interface, "zxdg_decoration_manager_v1") == 0) {
        g_Wayland.decorationManager = wlRegistryBind(
            registry, 
            name, 
            &zxdg_decoration_manager_v1_interface, 
            1);

    } else if (strcmp(interface, "wl_output") == 0) {
        // wayland does not let use query monitors directly so we enumerate and store
        // at init and update the cache when a monitor is added or removed.
        MonitorData* monitorData = getFreeMonitorData();
        if (!monitorData) {
            return;
        }

        struct wl_output* output = nullptr;
        output = wlRegistryBind(g_Wayland.registry, name, interfaces->output, 3);
        wlOutputAddListener(output, &g_OutputListener, monitorData);

        monitorData->wlName = name;
        monitorData->monitor = (PalMonitor*)output;
        g_Wayland.monitorCount++;
    }
}

static void globalRemove(
    void* data,
    struct wl_registry* registry,
    uint32_t name)
{
    (void)data;
    (void)registry;

    for (uint32_t i = 0; i < g_Wayland.maxMonitorData; ++i) {
        if (g_Wayland.monitorData[i].used && g_Wayland.monitorData[i].wlName == name) {
            MonitorData* data = &g_Wayland.monitorData[i];
            data->used = PAL_FALSE;

            g_Wayland.procs.proxyDestroy((struct wl_proxy*)data->monitor);
            g_Wayland.monitorCount--;
        }
    }
}

struct wl_registry_listener s_DriverRegistryListener = {
    .global = driverGlobalHandle,
    .global_remove = driverGlobalRemove};

struct wl_registry_listener s_RegistryListener = {
    .global = globalHandle,
    .global_remove = globalRemove};


/* =========================================================================
 * API
 * ========================================================================= */

PalVideoDriver* waylandGetVideoDriver(void)
{
    if (s_WaylandDriver.features != 0) {
        return &s_WaylandDriver;
    }

    g_Wayland.procs.handle = palLoadLibrary("libwayland-client.so.0");
    if (g_Wayland.procs.handle) {
        loadGlobals();
        loadProcs(PAL_FALSE);
        setupProtocols();

        g_Wayland.display = g_Wayland.procs.displayConnect(nullptr);
        if (g_Wayland.display) {
            g_Wayland.registry = wlDisplayGetRegistry(g_Wayland.display);
            wlRegistryAddListener(g_Wayland.registry, &s_DriverRegistryListener, nullptr);

            g_Wayland.procs.displayRoundtrip(g_Wayland.display);
            g_Wayland.procs.displayDisconnect(g_Wayland.display);
        }

        palFreeLibrary(g_Wayland.procs.handle);
        memset(&g_Wayland, 0, sizeof(Wayland));
    }

    s_WaylandDriver.type = PAL_VIDEO_DRIVER_TYPE_WAYLAND;
    return &s_WaylandDriver;
}

PalResult waylandInitVideo(
    const PalAllocator* allocator,
    struct PalEventDriver* eventDriver,
    const void* preferredInstance,
    void** selectedInstance)
{
    (void)allocator;
    (void)eventDriver;

    g_Wayland.procs.handle = palLoadLibrary("libwayland-client.so.0");
    g_Wayland.xkbProcs.handle = palLoadLibrary("libxkbcommon.so");
    g_Wayland.cursorProcs.handle = palLoadLibrary("libwayland-cursor.so");
    g_Wayland.eglProcs.handle = palLoadLibrary("libwayland-egl.so");

    // clang-format off
    if (!g_Wayland.procs.handle ||
        !g_Wayland.xkbProcs.handle ||
        !g_Wayland.cursorProcs.handle ||
        !g_Wayland.eglProcs.handle) {
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_POSIX, 
            (uint32_t)errno);
    }
    // clang-format on

    loadGlobals();
    loadProcs(PAL_TRUE);
    setupProtocols();
    createScancodeTable();
    createKeycodeTable();

    PalResult result;
    g_Wayland.maxMonitorData = 16;
    g_Wayland.maxWindowData = 32;

    g_Wayland.windowData = palAllocate(
        allocator, 
        sizeof(WindowData) * g_Wayland.maxWindowData, 
        0);

    g_Wayland.monitorData = palAllocate(
        allocator, 
        sizeof(MonitorData) * g_Wayland.maxMonitorData, 
        0);

    if (!g_Wayland.monitorData || !g_Wayland.windowData) {
        result = PAL_RESULT_CODE_OUT_OF_MEMORY;
        goto cleanup;
    }

    g_Wayland.monitorCount = 0;
    if (preferredInstance) {
        g_Wayland.display = (struct wl_display*)preferredInstance;
        *selectedInstance = (void*)preferredInstance;

    } else {
        g_Wayland.display = g_Wayland.procs.displayConnect(nullptr);
        if (!g_Wayland.display) {
            result = PAL_RESULT_CODE_PLATFORM_FAILURE;
            goto cleanup;
        }

        *selectedInstance = (void*)g_Wayland.display;
    }

    g_Wayland.allocator = allocator;
    g_Wayland.eventDriver = eventDriver;
    return PAL_RESULT_SUCCESS;

    cleanup:
        palFreeLibrary(g_Wayland.procs.handle);
        palFreeLibrary(g_Wayland.xkbProcs.handle);
        palFreeLibrary(g_Wayland.cursorProcs.handle);
        palFreeLibrary(g_Wayland.eglProcs.handle);

        if (g_Wayland.monitorData) {
            palFree(allocator, g_Wayland.monitorData);
        }

        if (g_Wayland.windowData) {
            palFree(allocator, g_Wayland.windowData);
        }

        memset(&g_Wayland, 0, sizeof(Wayland));
        return result;
}

void waylandShutdownVideo(void)
{
    if (g_Wayland.state) {
        g_Wayland.xkbProcs.stateUnref(g_Wayland.state);
        g_Wayland.xkbProcs.keymapUnref(g_Wayland.keymap);
    }

    g_Wayland.xkbProcs.contextUnref(g_Wayland.inputContext);
    if (g_Wayland.compositor) {
        // if compositor was found, all this will be as well since we check all at init
        g_Wayland.procs.proxyDestroy((struct wl_proxy*)g_Wayland.compositor);
        g_Wayland.procs.proxyDestroy((struct wl_proxy*)g_Wayland.xdgBase);
        g_Wayland.procs.proxyDestroy((struct wl_proxy*)g_Wayland.shm);
        g_Wayland.procs.proxyDestroy((struct wl_proxy*)g_Wayland.seat);
    }

    if (!g_Wayland.display) {
        // opened by PAL
        g_Wayland.procs.displayDisconnect(g_Wayland.display);

        palFreeLibrary(g_Wayland.cursorProcs.handle);
        palFreeLibrary(g_Wayland.xkbProcs.handle);
        palFreeLibrary(g_Wayland.eglProcs.handle);
        palFreeLibrary(g_Wayland.procs.handle);
    }

    palFree(g_Wayland.allocator, g_Wayland.windowData);
    palFree(g_Wayland.allocator, g_Wayland.monitorData);
    unloadVideoEGL();

    memset(&g_Wayland, 0, sizeof(Wayland));
}

void waylandUpdateVideo(void)
{
    // flush pending requests
    g_WaylandMouse.tmpScrollX = 0;
    g_WaylandMouse.tmpScrollY = 0;

#if PAL_HAS_EVENT_MODULE
    // key repeats. we only do this if the user wants key repeat events
    if (g_WaylandKeyboard.repeatKey != 0 && g_Wayland.eventDriver) {
        PalEventDriver* driver = g_Wayland.eventDriver;
        PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
        mode = palGetEventDispatchMode(driver, PAL_EVENT_TYPE_KEYREPEAT);
        if (mode != PAL_DISPATCH_MODE_NONE) {
            // get now time and check with the key repeat time
            uint64_t now = getCurrentTime();
            if (now >= g_WaylandKeyboard.timer) {
                PalWindow* window = (PalWindow*)g_Wayland.keyboardSurface;
                PalKeycode key = (PalKeycode)g_WaylandKeyboard.repeatKey;
                PalScancode scancode = (PalScancode)g_WaylandKeyboard.repeatScancode;

                PalEvent event = {0};
                event.type = PAL_EVENT_TYPE_KEYREPEAT;
                event.data = palPackUint32(key, scancode);
                event.data2 = palPackPointer(window);
                palPushEvent(driver, &event);
                g_WaylandKeyboard.timer += (uint64_t)g_WaylandKeyboard.repeatRate;
            }
        }
    }
#endif // PAL_HAS_EVENT_MODULE

    while (g_Wayland.procs.prepareRead(g_Wayland.display) != 0) {
        g_Wayland.procs.dispatchPending(g_Wayland.display);
    }

    g_Wayland.procs.displayFlush(g_Wayland.display);
    int fd = g_Wayland.procs.displayGetFd(g_Wayland.display);
    struct pollfd pfd = {fd, POLLIN, 0};
    if (poll(&pfd, 1, 0) > 0) {
        // there are events ready to be read
        g_Wayland.procs.readEvents(g_Wayland.display);

    } else {
        g_Wayland.procs.cancelRead(g_Wayland.display);
    }

    // dispatch events that were read
    g_Wayland.procs.dispatchPending(g_Wayland.display);
}

#endif // PAL_HAS_WAYLAND_BACKEND