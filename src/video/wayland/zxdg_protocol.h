
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

#ifndef WAYLAND_PROTOCOL_ZXDG_H
#define WAYLAND_PROTOCOL_ZXDG_H
#if PAL_HAS_WAYLAND_BACKEND

#include "wayland_helper.h"

#define procs g_Wayland.procs

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

static inline void
zxdgDecorationManagerV1Destroy(struct zxdg_decoration_manager_v1* zxdg_decoration_manager_v1)
{
    procs.proxyMarshalFlags(
        (struct wl_proxy*)zxdg_decoration_manager_v1,
        0, // ZXDG_DECORATION_MANAGER_V1_DESTROY
        NULL,
        procs.proxyGetVersion((struct wl_proxy*)zxdg_decoration_manager_v1),
        WL_MARSHAL_FLAG_DESTROY);
}

static inline struct zxdg_toplevel_decoration_v1* zxdgGetToplevelDecoration(
    struct zxdg_decoration_manager_v1* zxdg_decoration_manager_v1,
    struct xdg_toplevel* toplevel)
{
    struct wl_proxy* id;
    id = procs.proxyMarshalFlags(
        (struct wl_proxy*)zxdg_decoration_manager_v1,
        1, // ZXDG_DECORATION_MANAGER_V1_GET_TOPLEVEL_DECORATION,
        &zxdg_toplevel_decoration_v1_interface,
        procs.proxyGetVersion((struct wl_proxy*)zxdg_decoration_manager_v1),
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
    return procs.proxyAddListener(
        (struct wl_proxy*)zxdg_toplevel_decoration_v1,
        (void (**)(void))listener,
        data);
}

static inline void
zxdgToplevelDecorationV1Destroy(struct zxdg_toplevel_decoration_v1* zxdg_toplevel_decoration_v1)
{
    procs.proxyMarshalFlags(
        (struct wl_proxy*)zxdg_toplevel_decoration_v1,
        0, // ZXDG_TOPLEVEL_DECORATION_V1_DESTROY
        NULL,
        procs.proxyGetVersion((struct wl_proxy*)zxdg_toplevel_decoration_v1),
        WL_MARSHAL_FLAG_DESTROY);
}

static inline void zxdgToplevelDecorationV1SetMode(
    struct zxdg_toplevel_decoration_v1* zxdg_toplevel_decoration_v1,
    uint32_t mode)
{
    procs.proxyMarshalFlags(
        (struct wl_proxy*)zxdg_toplevel_decoration_v1,
        1, // ZXDG_TOPLEVEL_DECORATION_V1_SET_MODE,
        NULL,
        procs.proxyGetVersion((struct wl_proxy*)zxdg_toplevel_decoration_v1),
        0,
        mode);
}

#undef procs

#endif // PAL_HAS_WAYLAND_BACKEND
#endif // WAYLAND_PROTOCOL_ZXDG_H