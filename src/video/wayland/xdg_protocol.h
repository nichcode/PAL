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

#ifndef WAYLAND_PROTOCOL_XDG_H
#define WAYLAND_PROTOCOL_XDG_H
#if PAL_HAS_WAYLAND_BACKEND

#include "wayland_helper.h"

struct xdg_wm_base;
struct xdg_surface;
struct xdg_toplevel;

extern const struct wl_interface xdg_popup_interface;
extern const struct wl_interface xdg_positioner_interface;
extern const struct wl_interface xdg_surface_interface;
extern const struct wl_interface xdg_toplevel_interface;

struct xdg_wm_base_listener {
    void (*ping)(
        void*,
        struct xdg_wm_base*,
        uint32_t);
};

struct xdg_surface_listener {
    void (*configure)(
        void*,
        struct xdg_surface*,
        uint32_t);
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
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_wm_base,
        3, // XDG_WM_BASE_PONG
        nullptr,
        g_Wl.proxyGetVersion((struct wl_proxy*)xdg_wm_base),
        0,
        serial);
}

static inline int xdgWmBaseAddListener(
    struct xdg_wm_base* xdg_wm_base,
    const struct xdg_wm_base_listener* listener,
    void* data)
{
    return g_Wl.proxyAddListener((struct wl_proxy*)xdg_wm_base, (void (**)(void))listener, data);
}

static inline struct xdg_surface* xdgWmBaseGetXdgSurface(
    struct xdg_wm_base* xdg_wm_base,
    struct wl_surface* surface)
{
    struct wl_proxy* id;
    id = g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_wm_base,
        2, // XDG_WM_BASE_GET_XDG_SURFACE,
        &xdg_surface_interface,
        g_Wl.proxyGetVersion((struct wl_proxy*)xdg_wm_base),
        0,
        nullptr,
        surface);

    return (struct xdg_surface*)id;
}

static inline struct xdg_toplevel* xdgSurfaceGetToplevel(struct xdg_surface* xdg_surface)
{
    struct wl_proxy* id;
    id = g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_surface,
        1, // XDG_SURFACE_GET_TOPLEVEL,
        &xdg_toplevel_interface,
        g_Wl.proxyGetVersion((struct wl_proxy*)xdg_surface),
        0,
        nullptr);

    return (struct xdg_toplevel*)id;
}

static inline void xdgSurfaceAckConfigure(
    struct xdg_surface* xdg_surface,
    uint32_t serial)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_surface,
        4, // XDG_SURFACE_ACK_CONFIGURE
        nullptr,
        g_Wl.proxyGetVersion((struct wl_proxy*)xdg_surface),
        0,
        serial);
}

static inline int xdgSurfaceAddListener(
    struct xdg_surface* xdg_surface,
    const struct xdg_surface_listener* listener,
    void* data)
{
    return g_Wl.proxyAddListener((struct wl_proxy*)xdg_surface, (void (**)(void))listener, data);
}

static inline void xdgSurfaceDestroy(struct xdg_surface* xdg_surface)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_surface,
        0, // XDG_SURFACE_DESTROY
        nullptr,
        g_Wl.proxyGetVersion((struct wl_proxy*)xdg_surface),
        WL_MARSHAL_FLAG_DESTROY);
}

static inline void xdgToplevelDestroy(struct xdg_toplevel* xdg_toplevel)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_toplevel,
        0, // XDG_TOPLEVEL_DESTROY
        nullptr,
        g_Wl.proxyGetVersion((struct wl_proxy*)xdg_toplevel),
        WL_MARSHAL_FLAG_DESTROY);
}

static inline void xdgToplevelSetTitle(
    struct xdg_toplevel* xdg_toplevel,
    const char* title)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_toplevel,
        2, // XDG_TOPLEVEL_SET_TITLE
        nullptr,
        g_Wl.proxyGetVersion((struct wl_proxy*)xdg_toplevel),
        0,
        title);
}

static inline void xdgToplevelSetMaximized(struct xdg_toplevel* xdg_toplevel)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_toplevel,
        9, // XDG_TOPLEVEL_SET_MAXIMIZED
        nullptr,
        g_Wl.proxyGetVersion((struct wl_proxy*)xdg_toplevel),
        0);
}

static inline void xdgToplevelSetMinimized(struct xdg_toplevel* xdg_toplevel)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_toplevel,
        13, // XDG_TOPLEVEL_SET_MINIMIZED
        nullptr,
        g_Wl.proxyGetVersion((struct wl_proxy*)xdg_toplevel),
        0);
}

static inline int xdgToplevelAddListener(
    struct xdg_toplevel* xdg_toplevel,
    const struct xdg_toplevel_listener* listener,
    void* data)
{
    return g_Wl.proxyAddListener((struct wl_proxy*)xdg_toplevel, (void (**)(void))listener, data);
}

static inline void xdgToplevelSetMinSize(
    struct xdg_toplevel* xdg_toplevel,
    int32_t width,
    int32_t height)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_toplevel,
        8, // XDG_TOPLEVEL_SET_MIN_SIZE
        nullptr,
        g_Wl.proxyGetVersion((struct wl_proxy*)xdg_toplevel),
        0,
        width,
        height);
}

static inline void xdgToplevelSetMaxSize(
    struct xdg_toplevel* xdg_toplevel,
    int32_t width,
    int32_t height)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_toplevel,
        7, // XDG_TOPLEVEL_SET_MAX_SIZE
        nullptr,
        g_Wl.proxyGetVersion((struct wl_proxy*)xdg_toplevel),
        0,
        width,
        height);
}

static inline void xdgToplevelSetAppId(
    struct xdg_toplevel* xdg_toplevel,
    const char* app_id)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_toplevel,
        3, // XDG_TOPLEVEL_SET_APP_ID
        nullptr,
        g_Wl.proxyGetVersion((struct wl_proxy*)xdg_toplevel),
        0,
        app_id);
}

static inline void xdgToplevelUnsetMaximized(struct xdg_toplevel* xdg_toplevel)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)xdg_toplevel,
        10, // XDG_TOPLEVEL_UNSET_MAXIMIZED
        nullptr,
        g_Wl.proxyGetVersion((struct wl_proxy*)xdg_toplevel),
        0);
}

#endif // PAL_HAS_WAYLAND_BACKEND
#endif // WAYLAND_PROTOCOL_XDG_H