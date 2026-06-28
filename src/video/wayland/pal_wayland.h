
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_WAYLAND_H
#define _PAL_WAYLAND_H

#if PAL_HAS_WAYLAND_BACKEND == 1

#define MAX_SPAN_MONITORS 4

#include "pal/pal_video.h"
#include "video/pal_video_egl.h"
#include <wayland-client.h>
#include <wayland-util.h>
#include <wayland-cursor.h>
#include <fcntl.h>
#include <locale.h>
#include <poll.h>
#include <sys/mman.h>
#include <linux/input-event-codes.h>
#include <xkbcommon/xkbcommon-keysyms.h>
#include <xkbcommon/xkbcommon.h>

typedef struct wl_display* (*wl_display_connect_fn)(const char*);
typedef void (*wl_display_disconnect_fn)(struct wl_display*);
typedef int (*wl_display_roundtrip_fn)(struct wl_display*);
typedef int (*wl_display_dispatch_fn)(struct wl_display*);
typedef void (*wl_proxy_destroy_fn)(struct wl_proxy*);

typedef int (*wl_proxy_add_listener_fn)(
    struct wl_proxy*,
    void (**)(void),
    void*);

typedef struct wl_proxy* (*wl_proxy_marshal_constructor_v_fn)(
    struct wl_proxy*,
    uint32_t,
    const struct wl_interface*,
    uint32_t,
    ...);

typedef struct wl_proxy* (*wl_proxy_marshal_flags_fn)(
    struct wl_proxy*,
    uint32_t,
    const struct wl_interface*,
    uint32_t,
    uint32_t,
    ...);

typedef uint32_t (*wl_proxy_get_version_fn)(struct wl_proxy*);

typedef int (*wl_proxy_add_listener_fn)(
    struct wl_proxy*,
    void (**)(void),
    void*);

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

typedef struct wl_buffer* (*wl_cursor_image_get_buffer_fn)(struct wl_cursor_image*);

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
    struct wl_buffer* buffer;
    struct wl_surface* surface;
    int hotspotX;
    int hotspotY;
} WaylandCursor;

typedef struct {
    void* monitor;
    int dpi;
} SpanMonitor;

typedef struct {
    PalBool used;
    int dpi;
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

typedef struct {
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
    int dpi;
    int monitorCount;
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

typedef struct {
    PalBool checkFeatures;
    int monitorCount;
    int32_t maxWindowData;
    int32_t maxMonitorData;
    PalVideoFeatures features;
    const PalAllocator* allocator;
    PalEventDriver* eventDriver;
    WindowData* windowData;
    MonitorData* monitorData;

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

    wl_egl_window_create_fn eglWindowCreate;
    wl_egl_window_destroy_fn eglWindowDestroy;
    wl_egl_window_resize_fn eglWindowResize;
} Wayland;

extern Wayland s_Wl;
extern struct wl_registry_listener s_RegistryListener;
extern struct wl_output_listener s_OutputListener;
extern struct wl_output_listener s_DefaultModeListener;

extern struct wl_surface_listener s_SurfaceListener;
extern struct wl_pointer_listener s_PointerListener;
extern struct wl_keyboard_listener s_KeyboardListener;
extern struct zxdg_toplevel_decoration_v1_listener s_DecorationListener;

extern struct wl_seat_listener s_SeatListener;
extern struct xdg_wm_base_listener s_WmBaseListener;
extern struct xdg_surface_listener s_XdgSurfaceListener;
extern struct xdg_toplevel_listener s_XdgToplevelListener;

void setupProtocols();

struct wl_buffer* createShmBuffer(
    int width,
    int height,
    const uint8_t* pixels,
    PalBool cursor);

MonitorData* wlGetFreeMonitorData();
MonitorData* wlFindMonitorData(PalMonitor* monitor);
void wlFreeMonitorData(PalMonitor* monitor);
WindowData* wlGetFreeWindowData();
WindowData* wlFindWindowData(PalWindow* window);

#endif // PAL_HAS_WAYLAND_BACKEND
#endif // _PAL_WAYLAND_H