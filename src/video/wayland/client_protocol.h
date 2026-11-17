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

#ifndef WAYLAND_PROTOCOL_CLIENT_H
#define WAYLAND_PROTOCOL_CLIENT_H
#if PAL_HAS_WAYLAND_BACKEND

#include "wayland_helper.h"

static inline void* wlRegistryBind(
    struct wl_registry* wl_registry,
    uint32_t name,
    const struct wl_interface* interface,
    uint32_t version)
{
    struct wl_proxy* id;
    id = g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_registry,
        WL_REGISTRY_BIND,
        interface,
        version,
        0,
        name,
        interface->name,
        version,
        NULL);

    return (void*)id;
}

static inline int wlRegistryAddListener(
    struct wl_registry* wl_registry,
    const struct wl_registry_listener* listener,
    void* data)
{
    return g_Wl.proxyAddListener((struct wl_proxy*)wl_registry, (void (**)(void))listener, data);
}

static inline struct wl_registry* wlDisplayGetRegistry(struct wl_display* wl_display)
{
    struct wl_proxy* registry;
    registry = g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_display,
        WL_DISPLAY_GET_REGISTRY,
        g_Wl.registryInterface,
        g_Wl.proxyGetVersion((struct wl_proxy*)wl_display),
        0,
        NULL);

    return (struct wl_registry*)registry;
}

static inline int wlOutputAddListener(
    struct wl_output* wl_output,
    const struct wl_output_listener* listener,
    void* data)
{
    return g_Wl.proxyAddListener((struct wl_proxy*)wl_output, (void (**)(void))listener, data);
}

static inline struct wl_surface* wlCompositorCreateSurface(struct wl_compositor* wl_compositor)
{
    struct wl_proxy* id;
    id = g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_compositor,
        WL_COMPOSITOR_CREATE_SURFACE,
        g_Wl.surfaceInterface,
        g_Wl.proxyGetVersion((struct wl_proxy*)wl_compositor),
        0,
        NULL);

    return (struct wl_surface*)id;
}

static inline void wlSurfaceCommit(struct wl_surface* wl_surface)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_surface,
        WL_SURFACE_COMMIT,
        NULL,
        g_Wl.proxyGetVersion((struct wl_proxy*)wl_surface),
        0);
}

static inline void wlSurfaceDestroy(struct wl_surface* wl_surface)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_surface,
        WL_SURFACE_DESTROY,
        NULL,
        g_Wl.proxyGetVersion((struct wl_proxy*)wl_surface),
        WL_MARSHAL_FLAG_DESTROY);
}

static inline struct wl_shm_pool* wlShmCreatePool(
    struct wl_shm* wl_shm,
    int32_t fd,
    int32_t size)
{
    struct wl_proxy* id;
    id = g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_shm,
        WL_SHM_CREATE_POOL,
        g_Wl.shmPoolInterface,
        g_Wl.proxyGetVersion((struct wl_proxy*)wl_shm),
        0,
        NULL,
        fd,
        size);

    return (struct wl_shm_pool*)id;
}

static inline void wlShmPoolDestroy(struct wl_shm_pool* wl_shm_pool)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_shm_pool,
        WL_SHM_POOL_DESTROY,
        NULL,
        g_Wl.proxyGetVersion((struct wl_proxy*)wl_shm_pool),
        WL_MARSHAL_FLAG_DESTROY);
}

static inline struct wl_buffer* wlShmPoolCreateBuffer(
    struct wl_shm_pool* wl_shm_pool,
    int32_t offset,
    int32_t width,
    int32_t height,
    int32_t stride,
    uint32_t format)
{
    struct wl_proxy* id;
    id = g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_shm_pool,
        WL_SHM_POOL_CREATE_BUFFER,
        g_Wl.bufferInterface,
        g_Wl.proxyGetVersion((struct wl_proxy*)wl_shm_pool),
        0,
        NULL,
        offset,
        width,
        height,
        stride,
        format);

    return (struct wl_buffer*)id;
}

static inline void wlBufferDestroy(struct wl_buffer* wl_buffer)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_buffer,
        WL_BUFFER_DESTROY,
        NULL,
        g_Wl.proxyGetVersion((struct wl_proxy*)wl_buffer),
        WL_MARSHAL_FLAG_DESTROY);
}

static inline void wlSurfaceAttach(
    struct wl_surface* wl_surface,
    struct wl_buffer* buffer,
    int32_t x,
    int32_t y)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_surface,
        WL_SURFACE_ATTACH,
        NULL,
        g_Wl.proxyGetVersion((struct wl_proxy*)wl_surface),
        0,
        buffer,
        x,
        y);
}

static inline void wlSurfaceDamageBuffer(
    struct wl_surface* wl_surface,
    int32_t x,
    int32_t y,
    int32_t width,
    int32_t height)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_surface,
        WL_SURFACE_DAMAGE_BUFFER,
        NULL,
        g_Wl.proxyGetVersion((struct wl_proxy*)wl_surface),
        0,
        x,
        y,
        width,
        height);
}

static inline int wlSurfaceAddListener(
    struct wl_surface* wl_surface,
    const struct wl_surface_listener* listener,
    void* data)
{
    return g_Wl.proxyAddListener((struct wl_proxy*)wl_surface, (void (**)(void))listener, data);
}

static inline int wlSeatAddListener(
    struct wl_seat* wl_seat,
    const struct wl_seat_listener* listener,
    void* data)
{
    return g_Wl.proxyAddListener((struct wl_proxy*)wl_seat, (void (**)(void))listener, data);
}

static inline struct wl_pointer* wlSeatGetPointer(struct wl_seat* wl_seat)
{
    struct wl_proxy* id;
    id = g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_seat,
        WL_SEAT_GET_POINTER,
        g_Wl.pointerInterface,
        g_Wl.proxyGetVersion((struct wl_proxy*)wl_seat),
        0,
        NULL);

    return (struct wl_pointer*)id;
}

static inline struct wl_keyboard* wlSeatGetKeyboard(struct wl_seat* wl_seat)
{
    struct wl_proxy* id;
    id = g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_seat,
        WL_SEAT_GET_KEYBOARD,
        g_Wl.keyboardInterface,
        g_Wl.proxyGetVersion((struct wl_proxy*)wl_seat),
        0,
        NULL);

    return (struct wl_keyboard*)id;
}

static inline int wlPointerAddListener(
    struct wl_pointer* wl_pointer,
    const struct wl_pointer_listener* listener,
    void* data)
{
    return g_Wl.proxyAddListener((struct wl_proxy*)wl_pointer, (void (**)(void))listener, data);
}

static inline void wlPointerSetCursor(
    struct wl_pointer* wl_pointer,
    uint32_t serial,
    struct wl_surface* surface,
    int32_t hotspot_x,
    int32_t hotspot_y)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_pointer,
        WL_POINTER_SET_CURSOR,
        NULL,
        g_Wl.proxyGetVersion((struct wl_proxy*)wl_pointer),
        0,
        serial,
        surface,
        hotspot_x,
        hotspot_y);
}

static inline int wlKeyboardAddListener(
    struct wl_keyboard* wl_keyboard,
    const struct wl_keyboard_listener* listener,
    void* data)
{
    return g_Wl.proxyAddListener((struct wl_proxy*)wl_keyboard, (void (**)(void))listener, data);
}

static inline struct wl_region* wlCompositorCreateRegion(struct wl_compositor* wl_compositor)
{
    struct wl_proxy* id;
    id = g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_compositor,
        WL_COMPOSITOR_CREATE_REGION,
        g_Wl.regionInterface,
        g_Wl.proxyGetVersion((struct wl_proxy*)wl_compositor),
        0,
        NULL);

    return (struct wl_region*)id;
}

static inline void wlRegionAdd(
    struct wl_region* wl_region,
    int32_t x,
    int32_t y,
    int32_t width,
    int32_t height)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_region,
        WL_REGION_ADD,
        NULL,
        g_Wl.proxyGetVersion((struct wl_proxy*)wl_region),
        0,
        x,
        y,
        width,
        height);
}

static inline void wlSurfaceSetOpaqueRegion(
    struct wl_surface* wl_surface,
    struct wl_region* region)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_surface,
        WL_SURFACE_SET_OPAQUE_REGION,
        NULL,
        g_Wl.proxyGetVersion((struct wl_proxy*)wl_surface),
        0,
        region);
}

static inline void wlRegionDestroy(struct wl_region* wl_region)
{
    g_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_region,
        WL_REGION_DESTROY,
        NULL,
        g_Wl.proxyGetVersion((struct wl_proxy*)wl_region),
        WL_MARSHAL_FLAG_DESTROY);
}

#endif // PAL_HAS_WAYLAND_BACKEND
#endif // WAYLAND_PROTOCOL_CLIENT_H