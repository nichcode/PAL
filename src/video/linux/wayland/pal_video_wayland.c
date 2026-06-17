
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef __linux__
#if PAL_HAS_WAYLAND_BACKEND == 1

#include "pal_wayland_helper.h"
#include "pal_shared.h"
#include <dlfcn.h>

Wayland s_Wl = {0};

PalResult eglWlBackend(const int index)
{
    // user choose EGL FBConfig backend
    if (!s_Egl.handle) {
        palSetLastPlatformError(errno);
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    EGLDisplay display = EGL_NO_DISPLAY;
    display = s_Egl.eglGetDisplay((EGLNativeDisplayType)s_Wl.display);

    if (display == EGL_NO_DISPLAY) {
        palSetLastPlatformError(errno);
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    EGLint numConfigs = 0;
    if (!s_Egl.eglGetConfigs(display, nullptr, 0, &numConfigs)) {
        palSetLastPlatformError(errno);
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    EGLint configSize = sizeof(EGLConfig) * numConfigs;
    EGLConfig* eglConfigs = palAllocate(s_Video.allocator, configSize, 0);
    if (!eglConfigs) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    s_Egl.eglGetConfigs(display, eglConfigs, numConfigs, &numConfigs);
    s_Wl.eglFBConfig = eglConfigs[index];

    return PAL_RESULT_SUCCESS;
}

static void createKeycodeTable()
{
    // Tis is for only printable and text input keys

    // Letters
    s_Keyboard.keycodes[XKB_KEY_a] = PAL_KEYCODE_A;
    s_Keyboard.keycodes[XKB_KEY_b] = PAL_KEYCODE_B;
    s_Keyboard.keycodes[XKB_KEY_c] = PAL_KEYCODE_C;
    s_Keyboard.keycodes[XKB_KEY_d] = PAL_KEYCODE_D;
    s_Keyboard.keycodes[XKB_KEY_e] = PAL_KEYCODE_E;
    s_Keyboard.keycodes[XKB_KEY_f] = PAL_KEYCODE_F;
    s_Keyboard.keycodes[XKB_KEY_g] = PAL_KEYCODE_G;
    s_Keyboard.keycodes[XKB_KEY_h] = PAL_KEYCODE_H;
    s_Keyboard.keycodes[XKB_KEY_i] = PAL_KEYCODE_I;
    s_Keyboard.keycodes[XKB_KEY_j] = PAL_KEYCODE_J;
    s_Keyboard.keycodes[XKB_KEY_k] = PAL_KEYCODE_K;
    s_Keyboard.keycodes[XKB_KEY_l] = PAL_KEYCODE_L;
    s_Keyboard.keycodes[XKB_KEY_m] = PAL_KEYCODE_M;
    s_Keyboard.keycodes[XKB_KEY_n] = PAL_KEYCODE_N;
    s_Keyboard.keycodes[XKB_KEY_o] = PAL_KEYCODE_O;
    s_Keyboard.keycodes[XKB_KEY_p] = PAL_KEYCODE_P;
    s_Keyboard.keycodes[XKB_KEY_q] = PAL_KEYCODE_Q;
    s_Keyboard.keycodes[XKB_KEY_r] = PAL_KEYCODE_R;
    s_Keyboard.keycodes[XKB_KEY_s] = PAL_KEYCODE_S;
    s_Keyboard.keycodes[XKB_KEY_t] = PAL_KEYCODE_T;
    s_Keyboard.keycodes[XKB_KEY_u] = PAL_KEYCODE_U;
    s_Keyboard.keycodes[XKB_KEY_v] = PAL_KEYCODE_V;
    s_Keyboard.keycodes[XKB_KEY_w] = PAL_KEYCODE_W;
    s_Keyboard.keycodes[XKB_KEY_x] = PAL_KEYCODE_X;
    s_Keyboard.keycodes[XKB_KEY_y] = PAL_KEYCODE_Y;
    s_Keyboard.keycodes[XKB_KEY_z] = PAL_KEYCODE_Z;

    // Control
    s_Keyboard.keycodes[XKB_KEY_space] = PAL_KEYCODE_SPACE;

    // Misc
    s_Keyboard.keycodes[XKB_KEY_apostrophe] = PAL_KEYCODE_APOSTROPHE;
    s_Keyboard.keycodes[XKB_KEY_backslash] = PAL_KEYCODE_BACKSLASH;
    s_Keyboard.keycodes[XKB_KEY_comma] = PAL_KEYCODE_COMMA;
    s_Keyboard.keycodes[XKB_KEY_equal] = PAL_KEYCODE_EQUAL;
    s_Keyboard.keycodes[XKB_KEY_grave] = PAL_KEYCODE_GRAVEACCENT;
    s_Keyboard.keycodes[XKB_KEY_minus] = PAL_KEYCODE_SUBTRACT;
    s_Keyboard.keycodes[XKB_KEY_period] = PAL_KEYCODE_PERIOD;
    s_Keyboard.keycodes[XKB_KEY_semicolon] = PAL_KEYCODE_SEMICOLON;
    s_Keyboard.keycodes[XKB_KEY_slash] = PAL_KEYCODE_SLASH;
    s_Keyboard.keycodes[XKB_KEY_bracketleft] = PAL_KEYCODE_LBRACKET;
    s_Keyboard.keycodes[XKB_KEY_bracketright] = PAL_KEYCODE_RBRACKET;
}

PalResult wlInitVideo()
{
    // load wayland libray
    s_Wl.handle = dlopen("libwayland-client.so.0", RTLD_LAZY);
    s_Wl.xkbCommon = dlopen("libxkbcommon.so", RTLD_LAZY);
    s_Wl.libCursor = dlopen("libwayland-cursor.so", RTLD_LAZY);
    s_Wl.libWaylandEgl = dlopen("libwayland-egl.so", RTLD_LAZY);

    // clang-format off
    if (!s_Wl.handle ||
        !s_Wl.xkbCommon ||
        !s_Wl.libCursor ||
        !s_Wl.libWaylandEgl) {
        palSetLastPlatformError(errno);
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // get the exported global variables
    s_Wl.outputInterface = dlsym(s_Wl.handle, "wl_output_interface");
    s_Wl.seatInterface = dlsym(s_Wl.handle, "wl_seat_interface");
    s_Wl.compositorInterface = dlsym(s_Wl.handle, "wl_compositor_interface");
    s_Wl.surfaceInterface = dlsym(s_Wl.handle, "wl_surface_interface");
    s_Wl.registryInterface = dlsym(s_Wl.handle, "wl_registry_interface");
    s_Wl.shmInterface = dlsym(s_Wl.handle, "wl_shm_interface");
    s_Wl.bufferInterface = dlsym(s_Wl.handle, "wl_buffer_interface");
    s_Wl.shmPoolInterface = dlsym(s_Wl.handle, "wl_shm_pool_interface");
    s_Wl.regionInterface = dlsym(s_Wl.handle, "wl_region_interface");
    s_Wl.pointerInterface = dlsym(s_Wl.handle, "wl_pointer_interface");
    s_Wl.keyboardInterface = dlsym(s_Wl.handle, "wl_keyboard_interface");

    // load function procs
    s_Wl.displayConnect = (wl_display_connect_fn)dlsym(
        s_Wl.handle,
        "wl_display_connect");

    s_Wl.displayDisconnect = (wl_display_disconnect_fn)dlsym(
        s_Wl.handle,
        "wl_display_disconnect");

    s_Wl.displayRoundtrip = (wl_display_roundtrip_fn)dlsym(
        s_Wl.handle,
        "wl_display_roundtrip");

    s_Wl.displayDispatch = (wl_display_dispatch_fn)dlsym(
        s_Wl.handle,
        "wl_display_dispatch");

    s_Wl.proxyAddListener = (wl_proxy_add_listener_fn)dlsym(
        s_Wl.handle,
        "wl_proxy_add_listener");

    s_Wl.proxyMarshalCnstructor = (wl_proxy_marshal_constructor_v_fn)dlsym(
        s_Wl.handle,
        "wl_proxy_marshal_constructor_versioned");

    s_Wl.proxyDestroy = (wl_proxy_destroy_fn)dlsym(
        s_Wl.handle,
        "wl_proxy_destroy");

    s_Wl.proxyMarshalFlags = (wl_proxy_marshal_flags_fn)dlsym(
        s_Wl.handle,
        "wl_proxy_marshal_flags");

    s_Wl.proxyGetVersion = (wl_proxy_get_version_fn)dlsym(
        s_Wl.handle,
        "wl_proxy_get_version");

    s_Wl.getError = (wl_display_get_error_fn)dlsym(
        s_Wl.handle,
        "wl_display_get_error");

    s_Wl.dispatchPending = (wl_display_dispatch_pending_fn)dlsym(
        s_Wl.handle,
        "wl_display_dispatch_pending");

    s_Wl.displayFlush = (wl_display_flush_fn)dlsym(
        s_Wl.handle,
        "wl_display_flush");

    s_Wl.prepareRead = (wl_display_prepare_read_fn)dlsym(
        s_Wl.handle,
        "wl_display_prepare_read");

    s_Wl.readEvents = (wl_display_read_events_fn)dlsym(
        s_Wl.handle,
        "wl_display_read_events");

    s_Wl.displayGetFd = (wl_display_get_fd_fn)dlsym(
        s_Wl.handle,
        "wl_display_get_fd");

    s_Wl.cancelRead = (wl_display_cancel_read_fn)dlsym(
        s_Wl.handle,
        "wl_display_cancel_read");

    // load xkbcommon procs
    s_Wl.xkbKeymapUnref = (xkb_keymap_unref_fn)dlsym(
        s_Wl.xkbCommon,
        "xkb_keymap_unref");

    s_Wl.xkbStateKeyGetOneSym = (xkb_state_key_get_one_sym_fn)dlsym(
        s_Wl.xkbCommon,
        "xkb_state_key_get_one_sym");

    s_Wl.xkbStateNew = (xkb_state_new_fn)dlsym(
        s_Wl.xkbCommon,
        "xkb_state_new");

    s_Wl.xkbStateUnref = (xkb_state_unref_fn)dlsym(
        s_Wl.xkbCommon,
        "xkb_state_unref");

    s_Wl.xkbContextUnref = (xkb_context_unref_fn)dlsym(
        s_Wl.xkbCommon,
        "xkb_context_unref");

    s_Wl.xkbContextNew = (xkb_context_new_fn)dlsym(
        s_Wl.xkbCommon,
        "xkb_context_new");

    s_Wl.xkbKeymapNewFromString = (xkb_keymap_new_from_string_fn)dlsym(
        s_Wl.xkbCommon,
        "xkb_keymap_new_from_string");

    s_Wl.xkbKeysymToUtf32 = (xkb_keysym_to_utf32_fn)dlsym(
        s_Wl.xkbCommon,
        "xkb_keysym_to_utf32");

    s_Wl.xkbStateUpdateMask = (xkb_state_update_mask_fn)dlsym(
        s_Wl.xkbCommon,
        "xkb_state_update_mask");

    s_Wl.xkbKeymapKeyRepeats = (xkb_keymap_key_repeats_fn)dlsym(
        s_Wl.xkbCommon,
        "xkb_keymap_key_repeats");

    // load wayland cursor procs
    s_Wl.cursorImageGetBuffer = (wl_cursor_image_get_buffer_fn)dlsym(
        s_Wl.libCursor,
        "wl_cursor_image_get_buffer");

    s_Wl.cursorThemeLoad = (wl_cursor_theme_load_fn)dlsym(
        s_Wl.libCursor,
        "wl_cursor_theme_load");

    s_Wl.cursorThemeGetCursor = (wl_cursor_theme_get_cursor_fn)dlsym(
        s_Wl.libCursor,
        "wl_cursor_theme_get_cursor");

    // wl_egl procs
    s_Wl.eglWindowCreate = (wl_egl_window_create_fn)dlsym(
        s_Wl.libWaylandEgl,
        "wl_egl_window_create");

    s_Wl.eglWindowDestroy = (wl_egl_window_destroy_fn)dlsym(
        s_Wl.libWaylandEgl,
        "wl_egl_window_destroy");

    s_Wl.eglWindowResize = (wl_egl_window_resize_fn)dlsym(
        s_Wl.libWaylandEgl,
        "wl_egl_window_resize");
    // clang-format on

    // initialize wayland
    s_Wl.checkFeatures = PAL_TRUE;
    s_Wl.monitorCount = 0;
    setupXdgShellProtocol();

    // check if user supplied their own display
    if (s_Video.platformInstance) {
        s_Wl.display = (struct wl_display*)s_Video.platformInstance;

    } else {
        s_Wl.display = s_Wl.displayConnect(nullptr);
        s_Video.platformInstance = nullptr;
    }

    if (!s_Wl.display) {
        palSetLastPlatformError(errno);
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    s_Video.display = (void*)s_Wl.display;
    s_Wl.registry = displayGetRegistry(s_Wl.display);
    registryAddListener(s_Wl.registry, &s_RegistryListener, nullptr);
    s_Wl.displayRoundtrip(s_Wl.display);

    // do a roundtrip again to get remaining handles
    s_Wl.displayRoundtrip(s_Wl.display);

    if (!s_Wl.compositor || !s_Wl.xdgBase || !s_Wl.shm) {
        palSetLastPlatformError(errno);
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // create an input context
    s_Wl.inputContext = s_Wl.xkbContextNew(XKB_CONTEXT_NO_FLAGS);
    if (!s_Wl.inputContext) {
        palSetLastPlatformError(errno);
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // get the current theme
    s_Wl.cursorTheme = s_Wl.cursorThemeLoad(nullptr, 32, s_Wl.shm);
    if (!s_Wl.cursorTheme) {
        palSetLastPlatformError(errno);
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    createKeycodeTable();

    s_Video.display = (void*)s_Wl.display;
    return PAL_RESULT_SUCCESS;
}

void wlShutdownVideo()
{
    if (s_Wl.state) {
        s_Wl.xkbStateUnref(s_Wl.state);
        s_Wl.xkbKeymapUnref(s_Wl.keymap);
    }

    s_Wl.xkbContextUnref(s_Wl.inputContext);
    if (s_Wl.compositor) {
        // if compositor was found, all this will be as well
        // since we check all at init
        s_Wl.proxyDestroy((struct wl_proxy*)s_Wl.compositor);
        s_Wl.proxyDestroy((struct wl_proxy*)s_Wl.xdgBase);
        s_Wl.proxyDestroy((struct wl_proxy*)s_Wl.shm);
        s_Wl.proxyDestroy((struct wl_proxy*)s_Wl.seat);
    }

    if (!s_Video.platformInstance) {
        // opened by PAL
        s_Wl.displayDisconnect(s_Wl.display);
    }

    dlclose(s_Wl.libCursor);
    dlclose(s_Wl.xkbCommon);
    dlclose(s_Wl.libWaylandEgl);
    dlclose(s_Wl.handle);

    memset(&s_Wl, 0, sizeof(Wayland));
}

PalResult wlSetFBConfig(
    const int index,
    PalFBConfigBackend backend)
{
    if (backend == PAL_CONFIG_BACKEND_GLES || backend == PAL_CONFIG_BACKEND_PAL_OPENGL) {
        return eglWlBackend(index);

    } else {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }
}

void wlUpdateVideo()
{
    // flush pending requests
    s_Mouse.tmpScrollX = 0;
    s_Mouse.tmpScrollY = 0;

    // push key repeats
    // we only do this if the user wants key repeat events
    if (s_Keyboard.repeatKey != 0 && s_Video.eventDriver) {
        PalEventDriver* driver = s_Video.eventDriver;
        PalDispatchMode mode = PAL_DISPATCH_NONE;
        mode = palGetEventDispatchMode(driver, PAL_EVENT_KEYREPEAT);
        if (mode != PAL_DISPATCH_NONE) {
            // get now time and check with the key repeat time
            uint64_t now = getCurrentTime();
            if (now >= s_Keyboard.timer) {
                PalWindow* window = (PalWindow*)s_Wl.keyboardSurface;
                PalKeycode key = s_Keyboard.repeatKey;
                PalScancode scancode = s_Keyboard.repeatScancode;

                PalEvent event = {0};
                event.type = PAL_EVENT_KEYREPEAT;
                event.data = palPackUint32(key, scancode);
                event.data2 = palPackPointer(window);
                palPushEvent(driver, &event);
                s_Keyboard.timer += s_Keyboard.repeatRate;
            }
        }
    }

    while (s_Wl.prepareRead(s_Wl.display) != 0) {
        s_Wl.dispatchPending(s_Wl.display);
    }

    s_Wl.displayFlush(s_Wl.display);
    int fd = s_Wl.displayGetFd(s_Wl.display);
    struct pollfd pfd = {fd, POLLIN, 0};
    if (poll(&pfd, 1, 0) > 0) {
        // there are events ready to be read
        s_Wl.readEvents(s_Wl.display);

    } else {
        s_Wl.cancelRead(s_Wl.display);
    }

    // dispatch events that were read
    s_Wl.dispatchPending(s_Wl.display);
}

void* wlGetInstance()
{
    return (void*)s_Wl.display;
}

#endif // PAL_HAS_WAYLAND_BACKEND
#endif // __linux__