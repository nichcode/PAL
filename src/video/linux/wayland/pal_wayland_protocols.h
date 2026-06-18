
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_WAYLAND_PROTOCOLS_H
#define _PAL_WAYLAND_PROTOCOLS_H
#ifdef __linux__
#if PAL_HAS_WAYLAND_BACKEND == 1

#include "pal_wayland.h"

// ==================================================
// Wayland Client
// ==================================================

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

// ==================================================
// Xdg
// ==================================================

struct xdg_wm_base;
struct xdg_surface;
struct xdg_toplevel;

extern const struct wl_interface xdg_popup_interface;
extern const struct wl_interface xdg_positioner_interface;
extern const struct wl_interface xdg_surface_interface;
extern const struct wl_interface xdg_toplevel_interface;

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
        s_Wl.proxyGetVersion((struct wl_proxy*)xdg_surface),
        0,
        serial);
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

// ==================================================
// Zxdg
// ==================================================

struct zxdg_decoration_manager_v1;
struct zxdg_toplevel_decoration_v1;

struct zxdg_toplevel_decoration_v1_listener {
    void (*configure)(
        void*,
        struct zxdg_toplevel_decoration_v1*,
        uint32_t);
};

extern const struct wl_interface zxdg_decoration_manager_v1_interface;
extern const struct wl_interface zxdg_toplevel_decoration_v1_interface;

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

#endif // PAL_HAS_WAYLAND_BACKEND
#endif // __linux__
#endif // _PAL_WAYLAND_PROTOCOLS_H