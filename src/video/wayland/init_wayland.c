
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

#include "pal2/core/library.h"
#include "video/platform.h"
#include "pal2/video/init.h"
#ifdef PAL_HAS_WAYLAND_BACKEND

#include "wayland_helper.h"
#include <dlfcn.h>
#include <errno.h>
#include <string.h>

Wayland g_Wayland = {0};
static PalVideoDriver s_WaylandDriver = {0};
extern struct wl_interface xdgWmBaseInterface;

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

static void loadProcs(void)
{
    WaylandProcs* procs = &g_Wayland.procs;
    XkbProcs* xkbProcs = &g_Wayland.xbkProcs;
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

/* =========================================================================
 * API
 * ========================================================================= */

PalVideoDriver* waylandGetVideoDriver(void)
{
    s_WaylandDriver.type = PAL_VIDEO_DRIVER_TYPE_WAYLAND;
    s_WaylandDriver.features = PAL_VIDEO_FEATURE_BORDERLESS_WINDOW;
    
    return &s_WaylandDriver;
}

PalResult waylandInitVideo(
    const PalAllocator* allocator,
    const PalEventDriver* eventDriver,
    const void* preferredInstance,
    void** selectedInstance)
{
    (void)allocator;
    (void)eventDriver;

    g_Wayland.procs.handle = palLoadLibrary("libwayland-client.so.0");
    g_Wayland.xbkProcs.handle = palLoadLibrary("libxkbcommon.so");
    g_Wayland.cursorProcs.handle = palLoadLibrary("libwayland-cursor.so");
    g_Wayland.eglProcs.handle = palLoadLibrary("libwayland-egl.so");

    // clang-format off
    if (!g_Wayland.procs.handle ||
        !g_Wayland.xbkProcs.handle ||
        !g_Wayland.cursorProcs.handle ||
        !g_Wayland.eglProcs.handle) {
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_POSIX, 
            (uint32_t)errno);
    }
    // clang-format on

    loadGlobals();
    loadProcs();
    setupProtocols();

    PalResult result;
    g_Wayland.maxMonitorData = 16;
    g_Wayland.maxWindowData = 32;

    g_Wayland.windowData = palAllocate(
        g_Wayland.allocator, 
        sizeof(WindowData) * g_Wayland.maxWindowData, 
        0);

    g_Wayland.monitorData = palAllocate(
        g_Wayland.allocator, 
        sizeof(MonitorData) * g_Wayland.maxMonitorData, 
        0);

    if (!g_Wayland.monitorData || !g_Wayland.windowData) {
        result = PAL_RESULT_CODE_OUT_OF_MEMORY;
        goto cleanup;
    }

    g_Wayland.checkFeatures = PAL_TRUE;
    g_Wayland.monitorCount = 0;
    if (preferredInstance) {
        g_Wayland.display = (struct wl_display*)preferredInstance;
        *selectedInstance = (void*)preferredInstance;

    } else {
        *selectedInstance = nullptr;
        // g_Wayland.display = g_Wayland.procs.displayConnect(nullptr);
        // if (!g_Wayland.display) {
        //     result = PAL_RESULT_CODE_PLATFORM_FAILURE;
        //     goto cleanup;
        // }
    }

    return PAL_RESULT_SUCCESS;

    cleanup:
        palFreeLibrary(g_Wayland.procs.handle);
        palFreeLibrary(g_Wayland.xbkProcs.handle);
        palFreeLibrary(g_Wayland.cursorProcs.handle);
        palFreeLibrary(g_Wayland.eglProcs.handle);
        
        return result;
}

void waylandShutdownVideo(void)
{
    if (!g_Wayland.display) {
        // opened by PAL
        g_Wayland.procs.displayDisconnect(g_Wayland.display);

        dlclose(g_Wayland.cursorProcs.handle);
        dlclose(g_Wayland.xbkProcs.handle);
        dlclose(g_Wayland.eglProcs.handle);
        dlclose(g_Wayland.procs.handle);
    }

    palFree(g_Wayland.allocator, g_Wayland.windowData);
    palFree(g_Wayland.allocator, g_Wayland.monitorData);

    memset(&g_Wayland, 0, sizeof(Wayland));
}

void waylandUpdateVideo(void)
{

}

const VideoBackend g_WaylandBackend = {
    .getDriver = waylandGetVideoDriver,
    .init = waylandInitVideo,
    .shutdown = waylandUpdateVideo,
    .update = waylandUpdateVideo
};

#endif // PAL_HAS_WAYLAND_BACKEND