
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

#ifndef WAYLAND_LISTENERS_H
#define WAYLAND_LISTENERS_H
#if PAL_HAS_WAYLAND_BACKEND

#include "wayland_helper.h"
#include "client_protocol.h"
#include "xdg_protocol.h"
#include "zxdg_protocol.h"

extern struct wl_output_listener g_OutputListener;
extern struct wl_output_listener g_DefaultModeListener;
extern struct wl_surface_listener g_SurfaceListener;
extern struct wl_pointer_listener g_PointerListener;
extern struct wl_keyboard_listener g_KeyboardListener;
extern struct zxdg_toplevel_decoration_v1_listener g_DecorationListener;
extern struct wl_seat_listener g_SeatListener;
extern struct xdg_wm_base_listener g_WmBaseListener;
extern struct xdg_surface_listener g_XdgSurfaceListener;
extern struct xdg_toplevel_listener g_XdgToplevelListener;

#endif // PAL_HAS_WAYLAND_BACKEND
#endif // WAYLAND_LISTENERS_H