
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_WAYLAND_BACKEND == 1
#include "pal_wayland_protocols.h"

// ==================================================
// Xdg Shell
// ==================================================

static const struct wl_interface* xdg_shell_types[26];

static const struct wl_message xdg_wm_base_requests[] = {
    {"destroy", "", xdg_shell_types + 0},
    {"create_positioner", "n", xdg_shell_types + 4},
    {"get_xdg_surface", "no", xdg_shell_types + 5},
    {"pong", "u", xdg_shell_types + 0},
};

static const struct wl_message xdg_wm_base_events[] = {
    {"ping", "u", xdg_shell_types + 0},
};

const struct wl_interface xdg_wm_base_interface = {
    "xdg_wm_base",
    6,
    4,
    xdg_wm_base_requests,
    1,
    xdg_wm_base_events,
};

static const struct wl_message xdg_positioner_requests[] = {
    {"destroy", "", xdg_shell_types + 0},
    {"set_size", "ii", xdg_shell_types + 0},
    {"set_anchor_rect", "iiii", xdg_shell_types + 0},
    {"set_anchor", "u", xdg_shell_types + 0},
    {"set_gravity", "u", xdg_shell_types + 0},
    {"set_constraint_adjustment", "u", xdg_shell_types + 0},
    {"set_offset", "ii", xdg_shell_types + 0},
    {"set_reactive", "3", xdg_shell_types + 0},
    {"set_parent_size", "3ii", xdg_shell_types + 0},
    {"set_parent_configure", "3u", xdg_shell_types + 0},
};

const struct wl_interface xdg_positioner_interface = {
    "xdg_positioner",
    6,
    10,
    xdg_positioner_requests,
    0,
    NULL,
};

static const struct wl_message xdg_surface_requests[] = {
    {"destroy", "", xdg_shell_types + 0},
    {"get_toplevel", "n", xdg_shell_types + 7},
    {"get_popup", "n?oo", xdg_shell_types + 8},
    {"set_window_geometry", "iiii", xdg_shell_types + 0},
    {"ack_configure", "u", xdg_shell_types + 0},
};

static const struct wl_message xdg_surface_events[] = {
    {"configure", "u", xdg_shell_types + 0},
};

const struct wl_interface xdg_surface_interface = {
    "xdg_surface",
    6,
    5,
    xdg_surface_requests,
    1,
    xdg_surface_events,
};

static const struct wl_message xdg_toplevel_requests[] = {
    {"destroy", "", xdg_shell_types + 0},
    {"set_parent", "?o", xdg_shell_types + 11},
    {"set_title", "s", xdg_shell_types + 0},
    {"set_app_id", "s", xdg_shell_types + 0},
    {"show_window_menu", "ouii", xdg_shell_types + 12},
    {"move", "ou", xdg_shell_types + 16},
    {"resize", "ouu", xdg_shell_types + 18},
    {"set_max_size", "ii", xdg_shell_types + 0},
    {"set_min_size", "ii", xdg_shell_types + 0},
    {"set_maximized", "", xdg_shell_types + 0},
    {"unset_maximized", "", xdg_shell_types + 0},
    {"set_fullscreen", "?o", xdg_shell_types + 21},
    {"unset_fullscreen", "", xdg_shell_types + 0},
    {"set_minimized", "", xdg_shell_types + 0},
};

static const struct wl_message xdg_toplevel_events[] = {
    {"configure", "iia", xdg_shell_types + 0},
    {"close", "", xdg_shell_types + 0},
    {"configure_bounds", "4ii", xdg_shell_types + 0},
    {"wm_capabilities", "5a", xdg_shell_types + 0},
};

const struct wl_interface xdg_toplevel_interface = {
    "xdg_toplevel",
    6,
    14,
    xdg_toplevel_requests,
    4,
    xdg_toplevel_events,
};

static const struct wl_message xdg_popup_requests[] = {
    {"destroy", "", xdg_shell_types + 0},
    {"grab", "ou", xdg_shell_types + 22},
    {"reposition", "3ou", xdg_shell_types + 24},
};

static const struct wl_message xdg_popup_events[] = {
    {"configure", "iiii", xdg_shell_types + 0},
    {"popup_done", "", xdg_shell_types + 0},
    {"repositioned", "3u", xdg_shell_types + 0},
};

const struct wl_interface xdg_popup_interface = {
    "xdg_popup",
    6,
    3,
    xdg_popup_requests,
    3,
    xdg_popup_events,
};

// ==================================================
// Zxdg Decoration
// ==================================================

static const struct wl_interface* xdg_decoration_unstable_v1_types[] = {
    NULL,
    &zxdg_toplevel_decoration_v1_interface,
    &xdg_toplevel_interface,
};

static const struct wl_message zxdg_decoration_manager_v1_requests[] = {
    {"destroy", "", xdg_decoration_unstable_v1_types + 0},
    {"get_toplevel_decoration", "no", xdg_decoration_unstable_v1_types + 1},
};

const struct wl_interface zxdg_decoration_manager_v1_interface = {
    "zxdg_decoration_manager_v1",
    1,
    2,
    zxdg_decoration_manager_v1_requests,
    0,
    NULL,
};

static const struct wl_message zxdg_toplevel_decoration_v1_requests[] = {
    {"destroy", "", xdg_decoration_unstable_v1_types + 0},
    {"set_mode", "u", xdg_decoration_unstable_v1_types + 0},
    {"unset_mode", "", xdg_decoration_unstable_v1_types + 0},
};

static const struct wl_message zxdg_toplevel_decoration_v1_events[] = {
    {"configure", "u", xdg_decoration_unstable_v1_types + 0},
};

const struct wl_interface zxdg_toplevel_decoration_v1_interface = {
    "zxdg_toplevel_decoration_v1",
    1,
    3,
    zxdg_toplevel_decoration_v1_requests,
    1,
    zxdg_toplevel_decoration_v1_events,
};

// ==================================================
// Protocols
// ==================================================

void setupProtocols()
{
    xdg_shell_types[0] = NULL;
    xdg_shell_types[1] = NULL;
    xdg_shell_types[2] = NULL;
    xdg_shell_types[3] = NULL;
    xdg_shell_types[4] = &xdg_positioner_interface;
    xdg_shell_types[5] = &xdg_surface_interface;
    xdg_shell_types[6] = s_Wl.surfaceInterface;
    xdg_shell_types[7] = &xdg_toplevel_interface;
    xdg_shell_types[8] = &xdg_popup_interface;
    xdg_shell_types[9] = &xdg_surface_interface;
    xdg_shell_types[10] = &xdg_positioner_interface;
    xdg_shell_types[11] = &xdg_toplevel_interface;
    xdg_shell_types[12] = s_Wl.seatInterface;
    xdg_shell_types[13] = NULL;
    xdg_shell_types[14] = NULL;
    xdg_shell_types[15] = NULL;
    xdg_shell_types[16] = s_Wl.seatInterface;
    xdg_shell_types[17] = NULL;
    xdg_shell_types[18] = s_Wl.seatInterface;
    xdg_shell_types[19] = NULL;
    xdg_shell_types[20] = NULL;
    xdg_shell_types[21] = s_Wl.outputInterface;
    xdg_shell_types[22] = s_Wl.seatInterface;
    xdg_shell_types[23] = NULL;
    xdg_shell_types[24] = &xdg_positioner_interface;
    xdg_shell_types[25] = NULL;
}

#endif // PAL_HAS_WAYLAND_BACKEND