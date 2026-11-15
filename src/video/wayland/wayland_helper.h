
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

#ifndef WAYLAND_HELPER_H
#define WAYLAND_HELPER_H
#if PAL_HAS_WAYLAND_BACKEND

#include "pal2/core/library.h"
#include "pal2/video/init.h"
#include "pal2/video/window.h"
#include "pal2/video/monitor.h"

#include <wayland-client.h>
#include <wayland-cursor.h>
#include <wayland-util.h>
#include <xkbcommon/xkbcommon.h>

#define MAX_SPAN_MONITORS 4

/* =========================================================================
 * wayland
 * ========================================================================= */

typedef struct wl_display* (*wlDisplayConnectFn)(const char*);
typedef void (*wlDisplayDisconnectFn)(struct wl_display*);
typedef int (*wlDisplayRoundtripFn)(struct wl_display*);
typedef int (*wlDisplayDispatchFn)(struct wl_display*);
typedef void (*wlProxyDestroyFn)(struct wl_proxy*);
typedef int (*wlProxyAddListenerFn)(struct wl_proxy*, void (**)(void), void*);

typedef struct wl_proxy* (*wlProxyMarshalConstructorVFn)(
    struct wl_proxy*,
    uint32_t,
    const struct wl_interface*,
    uint32_t,
    ...);

typedef struct wl_proxy* (*wlProxyMarshalFlagsFn)(
    struct wl_proxy*,
    uint32_t,
    const struct wl_interface*,
    uint32_t,
    uint32_t,
    ...);

typedef uint32_t (*wlProxyGetVersionFn)(struct wl_proxy*);
typedef int (*wlDisplayGetErrorFn)(struct wl_display*);
typedef int (*wlDisplayDispatchPendingFn)(struct wl_display*);
typedef int (*wlDisplayFlushFn)(struct wl_display*);
typedef int (*wlDisplayPrepareReadFn)(struct wl_display*);
typedef int (*wlDisplayReadEventsFn)(struct wl_display*);
typedef int (*wlDisplayGetFdFn)(struct wl_display*);
typedef void (*wlDisplayCancelReadFn)(struct wl_display*);

/* =========================================================================
 * xkb
 * ========================================================================= */

typedef void (*xkbKeymapUnrefFn)(struct xkb_keymap*);
typedef struct xkb_state* (*xkbStateNewFn)(struct xkb_keymap*);
typedef void (*xkbStateUnrefFn)(struct xkb_state*);
typedef void (*xkbContextUnrefFn)(struct xkb_context*);
typedef struct xkb_context* (*xkbContextNewFn)(enum xkb_context_flags);
typedef uint32_t (*xkbKeysymToUtf32Fn)(xkb_keysym_t);
typedef xkb_keysym_t (*xkbStateKeyGetOneSymFn)(struct xkb_state*, xkb_keycode_t);

typedef struct xkb_keymap* (*xkbKeymapNewFromStringFn)(
    struct xkb_context*,
    const char*,
    enum xkb_keymap_format,
    enum xkb_keymap_compile_flags);

typedef enum xkb_state_component (*xkbStateUpdateMaskFn)(
    struct xkb_state*,
    xkb_mod_mask_t,
    xkb_mod_mask_t,
    xkb_mod_mask_t,
    xkb_layout_index_t,
    xkb_layout_index_t,
    xkb_layout_index_t);

typedef int (*xkbKeymapKeyRepeatsFn)(struct xkb_keymap*, xkb_keycode_t);

/* =========================================================================
 * wayland-cursor
 * ========================================================================= */

typedef struct wl_cursor_theme* (*wlCursorThemeLoadFn)(const char*, int, struct wl_shm*);
typedef struct wl_cursor* (*wlCursorThemeGetCursorFn)(struct wl_cursor_theme*, const char*);
typedef struct wl_buffer* (*wlCursorImageGetBufferFn)(struct wl_cursor_image*);

/* =========================================================================
 * wgl-window
 * ========================================================================= */

struct wl_egl_window;
struct wl_surface;
typedef struct wl_egl_window* (*wlEglWindowCreateFn)(struct wl_surface*, int, int);
typedef void (*wlEglWindowDestroyFn)(struct wl_egl_window*);
typedef void (*wlEglWindowResizeFn)(struct wl_egl_window*, int, int, int, int);

typedef struct {
    struct wl_buffer* buffer;
    struct wl_surface* surface;
    int hotspotX;
    int hotspotY;
} WaylandCursor;

typedef struct {
    void* monitor;
    uint32_t dpi;
} SpanMonitor;

typedef struct MonitorData {
    PalBool used;
    uint32_t dpi;
    int x;
    int y;
    uint32_t w;
    uint32_t h;
    uint32_t refreshRate;
    uint32_t wlName;
    PalOrientation orientation;
    PalMonitor* monitor;
    PalMonitorMode mode; // wayland only sends current
    char name[32];
} MonitorData;

typedef struct WindowData {
    PalBool skipConfigure;
    PalBool skipState;
    PalBool used;
    PalBool isAttached;
    PalBool skipIfAttached;
    PalBool focused;
    PalBool pushConfigureEvent;
    PalBool pushStateEvent;
    int x;
    int y;
    uint32_t w;
    uint32_t h;
    uint32_t dpi;
    uint32_t monitorCount;
    PalWindowState state;
    PalWindow* window;

    struct xdg_surface* xdgSurface;
    struct xdg_toplevel* xdgToplevel;
    struct wl_buffer* buffer;
    struct zxdg_toplevel_decoration_v1* decoration;
    void* cursor;
    struct wl_egl_window* eglWindow;
    SpanMonitor monitors[MAX_SPAN_MONITORS];
} WindowData;

typedef struct Wayland {
    uint32_t monitorCount;
    uint32_t maxWindowData;
    uint32_t maxMonitorData;

    PalLibrary* libwayland;
    PalLibrary* xkbcommon;
    PalLibrary* libegl;
    PalLibrary* libcursor;

    WindowData* windowData;
    MonitorData* monitorData;
    const PalAllocator* allocator;
    struct PalEventDriver* eventDriver;

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

    wlDisplayConnectFn displayConnect;
    wlDisplayDisconnectFn displayDisconnect;
    wlDisplayRoundtripFn displayRoundtrip;
    wlDisplayDispatchFn displayDispatch;
    wlProxyDestroyFn proxyDestroy;
    wlProxyAddListenerFn proxyAddListener;
    wlProxyMarshalConstructorVFn proxyMarshalConstructor;
    wlProxyMarshalFlagsFn proxyMarshalFlags;
    wlProxyGetVersionFn proxyGetVersion;
    wlDisplayGetErrorFn getError;
    wlDisplayDispatchPendingFn dispatchPending;
    wlDisplayFlushFn displayFlush;
    wlDisplayPrepareReadFn prepareRead;
    wlDisplayReadEventsFn readEvents;
    wlDisplayGetFdFn displayGetFd;
    wlDisplayCancelReadFn cancelRead;

    xkbKeymapUnrefFn xkbKeymapUnref;
    xkbStateNewFn xkbStateNew;
    xkbStateUnrefFn xkbStateUnref;
    xkbContextUnrefFn xkbContextUnref;
    xkbContextNewFn xkbContextNew;
    xkbStateKeyGetOneSymFn xkbStateKeyGetOneSym;
    xkbKeymapNewFromStringFn xkbKeymapNewFromString;
    xkbKeysymToUtf32Fn xkbKeysymToUtf32;
    xkbStateUpdateMaskFn xkbStateUpdateMask;
    xkbKeymapKeyRepeatsFn xkbKeymapKeyRepeats;

    wlEglWindowCreateFn eglWindowCreate;
    wlEglWindowDestroyFn eglWindowDestroy;
    wlEglWindowResizeFn eglWindowResize;

    wlCursorThemeLoadFn cursorThemeLoad;
    wlCursorThemeGetCursorFn cursorThemeGetCursor;
    wlCursorImageGetBufferFn cursorImageGetBuffer;

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
} Wayland;

typedef struct WaylandKeyboard {
    int repeatRate;
    int repeatDelay;
    int repeatKey;
    int repeatScancode;
    int scancodes[512];
    int keycodes[256];
    uint64_t timer;
    uint64_t frequency;
} WaylandKeyboard;

typedef struct WaylandMouse {
    PalBool pendingScroll;
    int32_t lastX;
    int32_t lastY;
    double tmpScrollX;
    double tmpScrollY;
    double accumScrollX;
    double accumScrollY;
} WaylandMouse;

extern Wayland g_Wayland;
extern WaylandKeyboard g_WaylandKeyboard;
extern WaylandMouse g_WaylandMouse;

struct wl_buffer* createShmBuffer(
    int width,
    int height,
    const uint8_t* pixels,
    PalBool cursor);

MonitorData* getFreeMonitorData();
MonitorData* findMonitorData(PalMonitor* monitor);
void freeMonitorData(PalMonitor* monitor);
WindowData* getFreeWindowData();
WindowData* findWindowData(PalWindow* window);

/** Defined in protocols.c */
void setupProtocols();

#endif // PAL_HAS_WAYLAND_BACKEND
#endif // WAYLAND_HELPER_H