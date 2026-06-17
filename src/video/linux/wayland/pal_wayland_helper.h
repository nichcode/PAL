
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_WAYLAND_HELPER_H
#define _PAL_WAYLAND_HELPER_H
#ifdef __linux__
#if PAL_HAS_WAYLAND_BACKEND == 1
#include "pal_wayland.h"
#include <unistd.h>
#include <stdlib.h>

static inline void* registryBind(
    struct wl_registry* wl_registry,
    uint32_t name,
    const struct wl_interface* interface,
    uint32_t version)
{
    struct wl_proxy* id;
    id = s_Wl.proxyMarshalFlags(
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

static inline int registryAddListener(
    struct wl_registry* wl_registry,
    const struct wl_registry_listener* listener,
    void* data)
{
    return s_Wl.proxyAddListener((struct wl_proxy*)wl_registry, (void (**)(void))listener, data);
}

static inline struct wl_registry* displayGetRegistry(struct wl_display* wl_display)
{
    struct wl_proxy* registry;
    registry = s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_display,
        1, // WL_DISPLAY_GET_REGISTRY
        s_Wl.registryInterface,
        s_Wl.proxyGetVersion((struct wl_proxy*)wl_display),
        0,
        NULL);

    return (struct wl_registry*)registry;
}

static inline int outputAddListener(
    struct wl_output* wl_output,
    const struct wl_output_listener* listener,
    void* data)
{
    return s_Wl.proxyAddListener((struct wl_proxy*)wl_output, (void (**)(void))listener, data);
}

static inline struct wl_surface* compositorCreateSurface(struct wl_compositor* wl_compositor)
{
    struct wl_proxy* id;
    id = s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_compositor,
        0, // WL_COMPOSITOR_CREATE_SURFACE,
        s_Wl.surfaceInterface,
        s_Wl.proxyGetVersion((struct wl_proxy*)wl_compositor),
        0,
        NULL);

    return (struct wl_surface*)id;
}

static inline void surfaceCommit(struct wl_surface* wl_surface)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_surface,
        6, // WL_SURFACE_COMMIT
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)wl_surface),
        0);
}

static inline void surfaceDestroy(struct wl_surface* wl_surface)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_surface,
        0, // WL_SURFACE_DESTROY
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)wl_surface),
        WL_MARSHAL_FLAG_DESTROY);
}

static inline struct wl_shm_pool* shmCreatePool(
    struct wl_shm* wl_shm,
    int32_t fd,
    int32_t size)
{
    struct wl_proxy* id;
    id = s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_shm,
        0, // WL_SHM_CREATE_POOL
        s_Wl.shmPoolInterface,
        s_Wl.proxyGetVersion((struct wl_proxy*)wl_shm),
        0,
        NULL,
        fd,
        size);

    return (struct wl_shm_pool*)id;
}

static inline void shmPoolDestroy(struct wl_shm_pool* wl_shm_pool)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_shm_pool,
        WL_SHM_POOL_DESTROY,
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)wl_shm_pool),
        WL_MARSHAL_FLAG_DESTROY);
}

static inline struct wl_buffer* shmPoolCreateBuffer(
    struct wl_shm_pool* wl_shm_pool,
    int32_t offset,
    int32_t width,
    int32_t height,
    int32_t stride,
    uint32_t format)
{
    struct wl_proxy* id;
    id = s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_shm_pool,
        WL_SHM_POOL_CREATE_BUFFER,
        s_Wl.bufferInterface,
        s_Wl.proxyGetVersion((struct wl_proxy*)wl_shm_pool),
        0,
        NULL,
        offset,
        width,
        height,
        stride,
        format);

    return (struct wl_buffer*)id;
}

static inline void bufferDestroy(struct wl_buffer* wl_buffer)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_buffer,
        WL_BUFFER_DESTROY,
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)wl_buffer),
        WL_MARSHAL_FLAG_DESTROY);
}

static inline void surfaceAttach(
    struct wl_surface* wl_surface,
    struct wl_buffer* buffer,
    int32_t x,
    int32_t y)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_surface,
        WL_SURFACE_ATTACH,
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)wl_surface),
        0,
        buffer,
        x,
        y);
}

static inline void surfaceDamageBuffer(
    struct wl_surface* wl_surface,
    int32_t x,
    int32_t y,
    int32_t width,
    int32_t height)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_surface,
        WL_SURFACE_DAMAGE_BUFFER,
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)wl_surface),
        0,
        x,
        y,
        width,
        height);
}

static inline int surfaceAddListener(
    struct wl_surface* wl_surface,
    const struct wl_surface_listener* listener,
    void* data)
{
    return s_Wl.proxyAddListener((struct wl_proxy*)wl_surface, (void (**)(void))listener, data);
}

static inline int seatAddListener(
    struct wl_seat* wl_seat,
    const struct wl_seat_listener* listener,
    void* data)
{
    return s_Wl.proxyAddListener((struct wl_proxy*)wl_seat, (void (**)(void))listener, data);
}

static inline struct wl_pointer* seatGetPointer(struct wl_seat* wl_seat)
{
    struct wl_proxy* id;
    id = s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_seat,
        WL_SEAT_GET_POINTER,
        s_Wl.pointerInterface,
        s_Wl.proxyGetVersion((struct wl_proxy*)wl_seat),
        0,
        NULL);

    return (struct wl_pointer*)id;
}

static inline struct wl_keyboard* seatGetKeyboard(struct wl_seat* wl_seat)
{
    struct wl_proxy* id;
    id = s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_seat,
        WL_SEAT_GET_KEYBOARD,
        s_Wl.keyboardInterface,
        s_Wl.proxyGetVersion((struct wl_proxy*)wl_seat),
        0,
        NULL);

    return (struct wl_keyboard*)id;
}

static inline int pointerAddListener(
    struct wl_pointer* wl_pointer,
    const struct wl_pointer_listener* listener,
    void* data)
{
    return s_Wl.proxyAddListener((struct wl_proxy*)wl_pointer, (void (**)(void))listener, data);
}

static inline void pointerSetCursor(
    struct wl_pointer* wl_pointer,
    uint32_t serial,
    struct wl_surface* surface,
    int32_t hotspot_x,
    int32_t hotspot_y)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_pointer,
        WL_POINTER_SET_CURSOR,
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)wl_pointer),
        0,
        serial,
        surface,
        hotspot_x,
        hotspot_y);
}

static inline int keyboardAddListener(
    struct wl_keyboard* wl_keyboard,
    const struct wl_keyboard_listener* listener,
    void* data)
{
    return s_Wl.proxyAddListener((struct wl_proxy*)wl_keyboard, (void (**)(void))listener, data);
}

static inline struct wl_region* compositorCreateRegion(struct wl_compositor* wl_compositor)
{
    struct wl_proxy* id;
    id = s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_compositor,
        WL_COMPOSITOR_CREATE_REGION,
        s_Wl.regionInterface,
        s_Wl.proxyGetVersion((struct wl_proxy*)wl_compositor),
        0,
        NULL);

    return (struct wl_region*)id;
}

static inline void regionAdd(
    struct wl_region* wl_region,
    int32_t x,
    int32_t y,
    int32_t width,
    int32_t height)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_region,
        WL_REGION_ADD,
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)wl_region),
        0,
        x,
        y,
        width,
        height);
}

static inline void surfaceSetOpaqueRegion(
    struct wl_surface* wl_surface,
    struct wl_region* region)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_surface,
        WL_SURFACE_SET_OPAQUE_REGION,
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)wl_surface),
        0,
        region);
}

static inline void regionDestroy(struct wl_region* wl_region)
{
    s_Wl.proxyMarshalFlags(
        (struct wl_proxy*)wl_region,
        WL_REGION_DESTROY,
        NULL,
        s_Wl.proxyGetVersion((struct wl_proxy*)wl_region),
        WL_MARSHAL_FLAG_DESTROY);
}

static void surfaceHandleEnter(
    void* userData,
    struct wl_surface* surface,
    struct wl_output* output)
{
    WindowData* data = userData;
    MonitorData* monitorData = findMonitorData((PalMonitor*)output);
    if (!monitorData) {
        return;
    }

    // wayland sends multiple surface enter events
    // if the surface spans multiple monitors
    // we get all and return the highest dpi
    // this assumes a surface can only span 4 monitors
    // at the sametime but this might be wrong

    // wayland will trigger this event if the window gains focus
    // so we check if the output is the same
    SpanMonitor* span = nullptr;
    if (data->monitorCount > 0) {
        for (int i = 0; i < data->monitorCount; i++) {
            if (data->monitors[i].monitor == (void*)output) {
                // the monitor already exist in our array
                // so we just update it
                span = &data->monitors[i];
                span->dpi = monitorData->dpi;
                break;
            }
        }
    }

    if (span == nullptr) {
        // new entry
        span = &data->monitors[data->monitorCount];
        span->monitor = output;
        span->dpi = monitorData->dpi;
        data->monitorCount++;
    }

    if (data->dpi == 0) {
        // this is triggered when the window is created
        // we cache the DPI and skip the event
        data->dpi = monitorData->dpi;
        return;
    }

    // the code below should be skipped if users are not
    // interested in DPI changed events
    PalDispatchMode mode = PAL_DISPATCH_NONE;
    PalEventType type = PAL_EVENT_MONITOR_DPI_CHANGED;
    if (!s_Video.eventDriver) {
        return;
    }

    PalEventDriver* driver = s_Video.eventDriver;
    mode = palGetEventDispatchMode(driver, type);
    if (mode == PAL_DISPATCH_NONE) {
        return;
    }

    // get the highest dpi and check if the it has changed
    int maxDPI = 96; // baseline
    for (int i = 0; i < data->monitorCount; i++) {
        if (!data->monitors[i].monitor) {
            // not a valid index. continue
            continue;
        }

        if (data->monitors[i].dpi > maxDPI) {
            // new highest
            maxDPI = data->monitors[i].dpi;
        }
    }

    if (maxDPI != data->dpi) {
        data->dpi = maxDPI;

        PalEvent event = {0};
        event.type = type;
        event.data = maxDPI;
        event.data2 = palPackPointer((void*)data->window);
        palPushEvent(driver, &event);
    }
}

static void surfaceHandleLeave(
    void* userData,
    struct wl_surface* surface,
    struct wl_output* output)
{
    // remove the monitor from our span monitor list
    WindowData* data = userData;
    MonitorData* monitorData = findMonitorData((PalMonitor*)output);
    if (!monitorData) {
        return;
    }

    for (int i = 0; i < data->monitorCount; i++) {
        if (data->monitors[i].monitor == (void*)output) {
            // found our monitor
            // we might want to pack the array but its just 4 monitors
            // so we leave it like that
            data->monitors[i].monitor = nullptr;
            data->monitorCount--;
            break;
        }
    }
}

static struct wl_surface_listener surfaceListener = {
    .enter = surfaceHandleEnter,
    .leave = surfaceHandleLeave};

static void pointerHandleEnter(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t serial,
    struct wl_surface* surface,
    wl_fixed_t surface_x,
    wl_fixed_t surface_y)
{
    WindowData* data = findWindowData((PalWindow*)surface);
    if (!data) {
        return;
    }

    if (data->cursor) {
        // our window
        WaylandCursor* cursor = data->cursor;
        pointerSetCursor(pointer, serial, cursor->surface, cursor->hotspotX, cursor->hotspotY);
    }

    // cache the surface the pointer is currently on
    s_Wl.pointerSurface = surface;
}

static void pointerHandleLeave(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t serial,
    struct wl_surface* surface)
{
    if (s_Wl.pointerSurface == surface) {
        s_Wl.pointerSurface == nullptr;
    }
}

static void pointerHandleMotion(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t time,
    wl_fixed_t surface_x,
    wl_fixed_t surface_y)
{
    int x = wl_fixed_to_int(surface_x);
    int y = wl_fixed_to_int(surface_y);
    const int dx = x - s_Mouse.lastX;
    const int dy = y - s_Mouse.lastY;

    PalDispatchMode mode = PAL_DISPATCH_NONE;
    PalWindow* window = (PalWindow*)s_Wl.pointerSurface;
    if (s_Video.eventDriver && window) {
        // we only push a mouse move only if we are on a window
        PalEventDriver* driver = s_Video.eventDriver;
        PalEventType type = PAL_EVENT_MOUSE_MOVE;
        mode = palGetEventDispatchMode(driver, type);
        if (mode != PAL_DISPATCH_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data = palPackInt32(x, y);
            event.data2 = palPackPointer(window);
            palPushEvent(driver, &event);
        }

        // push a mouse delta event
        type = PAL_EVENT_MOUSE_DELTA;
        mode = palGetEventDispatchMode(driver, type);
        if (mode != PAL_DISPATCH_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data = palPackInt32(dx, dy);
            event.data2 = palPackPointer(window);
            palPushEvent(driver, &event);
        }
    }

    s_Mouse.lastX = x;
    s_Mouse.lastY = y;
    s_Mouse.dx = dx;
    s_Mouse.dy = dy;
}

static void pointerHandleButton(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t serial,
    uint32_t time,
    uint32_t button,
    uint32_t state)
{
    PalWindow* window = nullptr;
    if (s_Wl.pointerSurface) {
        window = (PalWindow*)s_Wl.pointerSurface;

    } else {
        // cannot recieve events without a focused surface
        return;
    }

    PalBool pressed = state == WL_POINTER_BUTTON_STATE_PRESSED;
    PalMouseButton _button = 0;
    PalEventType type = PAL_EVENT_MOUSE_BUTTONUP;

    if (button == BTN_LEFT) {
        _button = PAL_MOUSE_BUTTON_LEFT;

    } else if (button == BTN_RIGHT) {
        _button = PAL_MOUSE_BUTTON_RIGHT;

    } else if (button == BTN_MIDDLE) {
        _button = PAL_MOUSE_BUTTON_MIDDLE;

    } else if (button == BTN_SIDE) {
        _button = PAL_MOUSE_BUTTON_X1;

    } else if (button == BTN_EXTRA) {
        _button = PAL_MOUSE_BUTTON_X2;
    }

    if (pressed) {
        type = PAL_EVENT_MOUSE_BUTTONDOWN;
    }

    s_Mouse.state[_button] = pressed;
    if (s_Video.eventDriver) {
        PalEventDriver* driver = s_Video.eventDriver;
        PalDispatchMode mode = PAL_DISPATCH_NONE;
        mode = palGetEventDispatchMode(driver, type);
        if (mode != PAL_DISPATCH_NONE) {
            PalEvent event = {0};
            event.type = type;

            // since we are not drawing decorations for users
            // they need the serial in order to draw their decorations
            // we put the serial at the upper 32 so ABI is preserved
            event.data = palPackUint32(_button, serial);
            event.data2 = palPackPointer(window);
            palPushEvent(driver, &event);
        }
    }
}

static void pointerHandleAxis(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t time,
    uint32_t axis,
    wl_fixed_t value)
{
    double delta = wl_fixed_to_double(value);
    if (axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL) {
        s_Mouse.tmpScrollX += delta;
        s_Mouse.accumScrollX += delta;

    } else if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL) {
        s_Mouse.tmpScrollY += delta;
        s_Mouse.accumScrollY += delta;
    }

    s_Mouse.pendingScroll = PAL_TRUE;
}

static void pointerHandleAxisDiscrete(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t axis,
    int32_t discrete)
{
    if (axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL) {
        s_Mouse.tmpScrollX += discrete;
        s_Mouse.accumScrollX += discrete;

    } else if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL) {
        s_Mouse.tmpScrollY += discrete;
        s_Mouse.accumScrollY += discrete;
    }

    s_Mouse.pendingScroll = PAL_TRUE;
}

static void pointerHandleFrame(
    void* userData,
    struct wl_pointer* pointer)
{
    if (!s_Mouse.pendingScroll) {
        // no wheel event
        return;
    }

    PalWindow* window = nullptr;
    if (s_Wl.pointerSurface) {
        window = (PalWindow*)s_Wl.pointerSurface;

    } else {
        // cannot recieve events without a focused surface
        return;
    }

    const int dx = (int)s_Mouse.accumScrollX;
    const int dy = (int)s_Mouse.accumScrollY;
    if (s_Video.eventDriver) {
        PalEventType type = PAL_EVENT_MOUSE_WHEEL;
        PalEventDriver* driver = s_Video.eventDriver;
        PalDispatchMode mode = PAL_DISPATCH_NONE;
        mode = palGetEventDispatchMode(driver, type);
        if (mode != PAL_DISPATCH_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data = palPackUint32(dx, dy);
            event.data2 = palPackPointer(window);
            palPushEvent(driver, &event);
        }
    }

    s_Mouse.WheelX = dx;
    s_Mouse.WheelY = dy;
    s_Mouse.accumScrollX -= dx;
    s_Mouse.accumScrollY -= dy;
    s_Mouse.pendingScroll = PAL_FALSE;
}

static void pointerHandleAxisSource(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t axis_source)
{
}

static void pointerHandleAxisStop(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t time,
    uint32_t axis)
{
}

static void keyboardHandleEnter(
    void* userData,
    struct wl_keyboard* keyboard,
    uint32_t serial,
    struct wl_surface* surface,
    struct wl_array* keys)
{
    // cache the surface the keyboard is currently on
    s_Wl.keyboardSurface = surface;
}

static void keyboardHandleLeave(
    void* userData,
    struct wl_keyboard* keyboard,
    uint32_t serial,
    struct wl_surface* surface)
{
    if (s_Wl.keyboardSurface == surface) {
        s_Wl.keyboardSurface == nullptr;
    }
}

static void keyboardHandleRemap(
    void* userData,
    struct wl_keyboard* keyboard,
    uint32_t format,
    int32_t fd,
    uint32_t size)
{
    if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        close(fd);
        return;
    }

    struct xkb_keymap* keymap = nullptr;
    struct xkb_state* state = nullptr;

    char* keymapStr = mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0);
    if (keymapStr == MAP_FAILED) {
        close(fd);
        return;
    }

    keymap = s_Wl.xkbKeymapNewFromString(
        s_Wl.inputContext,
        keymapStr,
        XKB_KEYMAP_FORMAT_TEXT_V1,
        XKB_KEYMAP_COMPILE_NO_FLAGS);

    if (!keymap) {
        return;
    }

    munmap(keymapStr, size);
    close(fd);

    state = s_Wl.xkbStateNew(keymap);
    if (!state) {
        return;
    }

    // check if we have old keymap and state
    if (s_Wl.state) {
        s_Wl.xkbStateUnref(s_Wl.state);
        s_Wl.xkbKeymapUnref(s_Wl.keymap);
    }

    s_Wl.state = state;
    s_Wl.keymap = keymap;
    s_Keyboard.frequency = palGetPerformanceFrequency();
}

static void keyboardHandleKey(
    void* userData,
    struct wl_keyboard* keyboard,
    uint32_t serial,
    uint32_t time,
    uint32_t key,
    uint32_t state)
{
    PalWindow* window = nullptr;
    if (s_Wl.keyboardSurface) {
        window = (PalWindow*)s_Wl.keyboardSurface;

    } else {
        // cannot recieve events without a focused surface
        return;
    }

    PalScancode scancode = 0;
    PalKeycode keycode = 0;
    PalBool pressed = (state == WL_KEYBOARD_KEY_STATE_PRESSED);
    PalEventType type = PAL_EVENT_KEYUP;
    PalDispatchMode mode = PAL_DISPATCH_NONE;
    xkb_keysym_t keySym = s_Wl.xkbStateKeyGetOneSym(s_Wl.state, key + 8);

    // special handling
    if (key == 119) {
        scancode = PAL_SCANCODE_PAUSE;
    } else if (key == 107) {
        scancode = PAL_SCANCODE_END;
    } else if (key == 103) {
        scancode = PAL_SCANCODE_UP;
    } else if (key == 102) {
        scancode = PAL_SCANCODE_HOME;

    } else {
        scancode = s_Keyboard.scancodes[key];
    }

    // printable and text input keys are from the range
    // 32 (PAL_KEYCODE_SPACE) and 122 (PAL_KEYCODE_Z)
    // The rest are almost the same as their scancode
    // Maybe there will be a layout that makes this wrong
    // but for now this works
    if (keySym >= XKB_KEY_space && keySym <= XKB_KEY_z) {
        // a printable or input key
        keycode = s_Keyboard.keycodes[keySym];

    } else {
        // Since PalKeycode and PalScancode have the same integers
        // we can make a direct cast without a table
        // Examle: PAL_KEYCODE_A(int 0) == PAL_SCANCODE_A(int 0)
        keycode = (PalKeycode)(uint32_t)scancode;
    }

    // If we got a keySym but its not mapped into our keycode array
    // we do a direct cast as well
    if (keycode == PAL_KEYCODE_UNKNOWN) {
        keycode = (PalKeycode)(uint32_t)scancode;
    }

    s_Keyboard.scancodeState[scancode] = pressed;
    s_Keyboard.keycodeState[keycode] = pressed;

    // check for key repeats
    if (pressed) {
        if (s_Wl.xkbKeymapKeyRepeats(s_Wl.keymap, key + 8)) {
            s_Keyboard.repeatKey = keycode;
            s_Keyboard.repeatScancode = scancode;
            s_Keyboard.timer = getCurrentTime() + s_Keyboard.repeatDelay;

        } else {
            s_Keyboard.repeatKey = 0;
        }

        type = PAL_EVENT_KEYDOWN;

    } else {
        // key release
        if (s_Keyboard.repeatKey == keycode) {
            s_Keyboard.repeatKey = 0;
        }
    }

    if (s_Video.eventDriver) {
        PalEventDriver* driver = s_Video.eventDriver;
        mode = palGetEventDispatchMode(driver, type);
        if (mode != PAL_DISPATCH_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data = palPackUint32(keycode, scancode);
            event.data2 = palPackPointer(window);
            palPushEvent(driver, &event);
        }

        // check for char event if enabled
        type = PAL_EVENT_KEYCHAR;
        mode = palGetEventDispatchMode(driver, type);
        if (mode == PAL_DISPATCH_NONE) {
            return;
        }

        uint32_t codepoint = s_Wl.xkbKeysymToUtf32(keySym);
        if (codepoint <= 0) {
            return;
        }

        PalEvent event = {0};
        event.type = type;
        event.data = codepoint;
        event.data2 = palPackPointer(window);
        palPushEvent(driver, &event);
    }
}

static void keyboardHandleRepeatInfo(
    void* userData,
    struct wl_keyboard* keyboard,
    int32_t rate,
    int32_t delay)
{
    if (s_Wl.keyboard == keyboard) {
        s_Keyboard.repeatDelay = delay;
        s_Keyboard.repeatRate = rate;

    } else {
        if (s_Keyboard.repeatDelay == 0) {
            s_Keyboard.repeatDelay = 500;
            s_Keyboard.repeatRate = 30;
        }
    }
}

static void keyboardHandleModifiers(
    void* userData,
    struct wl_keyboard* keyboard,
    uint32_t serial,
    uint32_t mods_depressed,
    uint32_t mods_latched,
    uint32_t mods_locked,
    uint32_t group)
{
    if (s_Wl.state) {
        s_Wl.xkbStateUpdateMask(s_Wl.state, mods_depressed, mods_latched, mods_locked, group, 0, 0);
    }
}

static struct wl_pointer_listener pointerListener = {
    .enter = pointerHandleEnter,
    .leave = pointerHandleLeave,
    .motion = pointerHandleMotion,
    .button = pointerHandleButton,
    .axis = pointerHandleAxis,
    .axis_discrete = pointerHandleAxisDiscrete,
    .frame = pointerHandleFrame,
    .axis_source = pointerHandleAxisSource,
    .axis_stop = pointerHandleAxisStop};

static struct wl_keyboard_listener keyboardListener = {
    .enter = keyboardHandleEnter,
    .leave = keyboardHandleLeave,
    .keymap = keyboardHandleRemap,
    .key = keyboardHandleKey,
    .repeat_info = keyboardHandleRepeatInfo,
    .modifiers = keyboardHandleModifiers};

static void seatHandleCapabilities(
    void* userData,
    struct wl_seat* seat,
    enum wl_seat_capability caps)
{
    if (caps & WL_SEAT_CAPABILITY_KEYBOARD) {
        s_Wl.keyboard = seatGetKeyboard(seat);
        keyboardAddListener(s_Wl.keyboard, &keyboardListener, nullptr);
    }

    if (caps & WL_SEAT_CAPABILITY_POINTER) {
        s_Wl.pointer = seatGetPointer(seat);
        pointerAddListener(s_Wl.pointer, &pointerListener, nullptr);
    }
}

static void seatHandleName(
    void* userData,
    struct wl_seat* seat,
    const char* name)
{
}

static struct wl_seat_listener seatListener = {
    .capabilities = seatHandleCapabilities,
    .name = seatHandleName};

// Protocols
struct xdg_wm_base;
struct xdg_surface;
struct xdg_toplevel;

// forward declare
static struct wl_buffer* createShmBuffer(
    int width,
    int height,
    const uint8_t* pixels,
    PalBool cursor);

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
    return s_Wl.proxyAddListener((struct wl_proxy*)xdg_wm_base, (void (**)(void))listener, data);
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

static inline struct xdg_toplevel* xdgSurfaceGetToplevel(struct xdg_surface* xdg_surface)
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

static void wmBaseHandlePing(
    void* data,
    struct xdg_wm_base* base,
    uint32_t serial)
{
    xdgWmBasePong(base, serial);
}

static void xdgSurfaceHandleConfigure(
    void* data,
    struct xdg_surface* surface,
    uint32_t serial)
{
    WindowData* winData = (WindowData*)data;
    xdgSurfaceAckConfigure(surface, serial);

    // push and resolve any pending events
    if (!winData->skipConfigure) {
        if (winData->pushConfigureEvent) {
            if (winData->eglWindow) {
                s_Wl.eglWindowResize(winData->eglWindow, winData->w, winData->h, 0, 0);

            } else {
                // create a new buffer with the new size
                struct wl_buffer* buffer = nullptr;
                buffer = createShmBuffer(winData->w, winData->h, nullptr, PAL_FALSE);
                if (!buffer) {
                    return;
                }

                struct wl_surface* _surface = nullptr;
                _surface = (struct wl_surface*)winData->window;

                surfaceAttach(_surface, buffer, 0, 0);
                surfaceDamageBuffer(_surface, 0, 0, winData->w, winData->h);
                surfaceCommit(_surface);

                // destroy old buffer
                bufferDestroy(winData->buffer);
                winData->buffer = buffer;
            }

            // push a window resize event
            if (s_Video.eventDriver) {
                PalEventType type = PAL_EVENT_WINDOW_SIZE;
                PalDispatchMode mode = PAL_DISPATCH_NONE;
                mode = palGetEventDispatchMode(s_Video.eventDriver, type);
                if (mode != PAL_DISPATCH_NONE) {
                    PalEvent event = {0};
                    event.type = type;
                    event.data = palPackUint32(winData->w, winData->h);
                    event.data2 = palPackPointer(winData->window);
                    palPushEvent(s_Video.eventDriver, &event);
                }
            }

            winData->pushConfigureEvent = PAL_FALSE;
        }
    }

    // pending state
    if (!winData->skipState) {
        if (!winData->pushStateEvent) {
            return;
        }

        // push a window state event
        // we dont recreate buffers over here
        // since we already create the buffer with the new size
        if (s_Video.eventDriver) {
            PalEventType type = PAL_EVENT_WINDOW_STATE;
            PalDispatchMode mode = PAL_DISPATCH_NONE;
            mode = palGetEventDispatchMode(s_Video.eventDriver, type);
            if (mode != PAL_DISPATCH_NONE) {
                PalEvent event = {0};
                event.type = type;
                event.data = winData->state;
                event.data2 = palPackPointer(winData->window);
                palPushEvent(s_Video.eventDriver, &event);
            }
        }

        winData->pushStateEvent = PAL_FALSE;
    }
}

static void xdgToplevelHandleConfigure(
    void* data,
    struct xdg_toplevel* toplevel,
    int32_t width,
    int32_t height,
    struct wl_array* states)
{
    WindowData* winData = (WindowData*)data;
    uint32_t* state;
    PalBool activated = PAL_FALSE;
    wl_array_for_each(state, states)
    {
        // we need only maximized
        if (*state == 1) { // XDG_TOPLEVEL_STATE_MAXIMIZED
            if (winData->state != PAL_WINDOW_STATE_MAXIMIZED) {
                winData->state = PAL_WINDOW_STATE_MAXIMIZED;
                winData->pushStateEvent = PAL_TRUE;
            }

        } else if (*state == 4) { // XDG_TOPLEVEL_STATE_ACTIVATED
            activated = PAL_TRUE;
        }
    }

    if (width > 0 && height > 0) {
        if (width != winData->w || height != winData->h) {
            // size change
            winData->pushConfigureEvent = PAL_TRUE;
        }

        winData->w = width;
        winData->h = height;
    }

    if (activated && !winData->focused) {
        // focus gained
        winData->focused = PAL_TRUE;

    } else if (!activated && winData->focused) {
        // focus lost
        winData->focused = PAL_FALSE;

    } else {
        // discard double focus gained and double focus lost
        return;
    }

    if (s_Video.eventDriver) {
        PalEventDriver* driver = s_Video.eventDriver;
        PalDispatchMode mode = PAL_DISPATCH_NONE;
        PalEventType type = PAL_EVENT_WINDOW_FOCUS;
        mode = palGetEventDispatchMode(driver, type);

        if (mode != PAL_DISPATCH_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data = winData->focused;
            event.data2 = palPackPointer(winData->window);
            palPushEvent(driver, &event);
        }
    }
}

static void xdgToplevelHandleClose(
    void* data,
    struct xdg_toplevel* toplevel)
{
    WindowData* winData = (WindowData*)data;
    if (s_Video.eventDriver) {
        PalEventType type = PAL_EVENT_WINDOW_CLOSE;
        PalDispatchMode mode = PAL_DISPATCH_NONE;
        mode = palGetEventDispatchMode(s_Video.eventDriver, type);
        if (mode != PAL_DISPATCH_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data2 = palPackPointer(winData->window);
            palPushEvent(s_Video.eventDriver, &event);
        }
    }
}

static inline int xdgSurfaceAddListener(
    struct xdg_surface* xdg_surface,
    const struct xdg_surface_listener* listener,
    void* data)
{
    return s_Wl.proxyAddListener((struct wl_proxy*)xdg_surface, (void (**)(void))listener, data);
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
    return s_Wl.proxyAddListener((struct wl_proxy*)xdg_toplevel, (void (**)(void))listener, data);
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

static const struct xdg_wm_base_listener wmBaseListener = {.ping = wmBaseHandlePing};

struct xdg_toplevel;
struct zxdg_decoration_manager_v1;
struct zxdg_toplevel_decoration_v1;

struct zxdg_toplevel_decoration_v1_listener {
    void (*configure)(
        void*,
        struct zxdg_toplevel_decoration_v1*,
        uint32_t);
};

const struct wl_interface zxdg_decoration_manager_v1_interface;
const struct wl_interface zxdg_toplevel_decoration_v1_interface;

static inline void
zxdgDecorationManagerV1Destroy(struct zxdg_decoration_manager_v1* zxdg_decoration_manager_v1)
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

static inline void
zxdgToplevelDecorationV1Destroy(struct zxdg_toplevel_decoration_v1* zxdg_toplevel_decoration_v1)
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

static void setupXdgShellProtocol()
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

static void zxdgDecorationHandleConfigure(
    void* data,
    struct zxdg_toplevel_decoration_v1* dec,
    uint32_t mode)
{
    if (s_Video.eventDriver) {
        PalEventDriver* driver = s_Video.eventDriver;
        PalDispatchMode dispatchMode = PAL_DISPATCH_NONE;
        PalEventType type = PAL_EVENT_WINDOW_DECORATION_MODE;
        dispatchMode = palGetEventDispatchMode(driver, type);

        if (dispatchMode != PAL_DISPATCH_NONE) {
            PalDecorationMode decorMode = PAL_DECORATION_MODE_SERVER_SIDE;
            if (mode == 0 || mode == 1) {
                // client side decoration
                decorMode = PAL_DECORATION_MODE_CLIENT_SIDE;
            }

            PalEvent event = {0};
            event.type = type;
            event.data = decorMode;
            event.data2 = palPackPointer(data);
            palPushEvent(driver, &event);
        }
    }
}

static int createShmFile(uint64_t size)
{
    char template[] = "/tmp/pal-shm-XXXXXX";
    int fd = mkstemp(template);
    if (fd < 0) {
        return -1;
    }

    unlink(template);
    if (ftruncate(fd, size) < 0) {
        return -1;
    }

    return fd;
}

static struct wl_buffer* createShmBuffer(
    int width,
    int height,
    const uint8_t* pixels,
    PalBool cursor)
{
    int stride = width * 4;
    uint64_t size = stride * height;
    struct wl_buffer* buffer = nullptr;
    struct wl_shm_pool* pool = nullptr;
    void* data = nullptr;

    int fd = createShmFile(size);
    if (fd == -1) {
        return nullptr;
    }

    data = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        return nullptr;
    }

    enum wl_shm_format format = WL_SHM_FORMAT_XRGB8888;
    if (cursor) {
        format = WL_SHM_FORMAT_ARGB8888;
        uint32_t* dataPixels = (uint32_t*)data;

        // convert from RGBA8 to ARGB32
        for (int i = 0; i < width * height; i++) {
            uint8_t r = pixels[i * 4 + 0]; // Red
            uint8_t g = pixels[i * 4 + 1]; // Green
            uint8_t b = pixels[i * 4 + 2]; // Blue
            uint8_t a = pixels[i * 4 + 3]; // Alpha

            // clang-format off
            dataPixels[i] = ((unsigned long)a << 24) |
                            ((unsigned long)r << 16) |
                            ((unsigned long)g << 8) |
                            ((unsigned long)b);
            // clang-format on
        }

    } else {
        memset(data, 0xFF, size); // white
    }

    pool = shmCreatePool(s_Wl.shm, fd, size);
    if (!pool) {
        return nullptr;
    }

    buffer = shmPoolCreateBuffer(pool, 0, width, height, stride, format);
    if (!buffer) {
        return nullptr;
    }

    shmPoolDestroy(pool);
    munmap(data, size);
    close(fd);
    return buffer;
}

static void outputGeometry(
    void* data,
    struct wl_output* output,
    int32_t x,
    int32_t y,
    int32_t, // we dont need physical size
    int32_t, // we dont need physical size
    int32_t, // we dont need subpixel
    const char* make,
    const char* model,
    int32_t transform)
{
    MonitorData* monitorData = data;
    monitorData->x = x;
    monitorData->y = y;

    switch (transform) {
        case WL_OUTPUT_TRANSFORM_NORMAL:
        case WL_OUTPUT_TRANSFORM_180: {
            monitorData->orientation = PAL_ORIENTATION_LANDSCAPE;
            break;
        }

        case WL_OUTPUT_TRANSFORM_90:
        case WL_OUTPUT_TRANSFORM_270: {
            monitorData->orientation = PAL_ORIENTATION_PORTRAIT;
            break;
        }

        case WL_OUTPUT_TRANSFORM_FLIPPED:
        case WL_OUTPUT_TRANSFORM_FLIPPED_180: {
            monitorData->orientation = PAL_ORIENTATION_LANDSCAPE_FLIPPED;
            break;
        }

        case WL_OUTPUT_TRANSFORM_FLIPPED_90:
        case WL_OUTPUT_TRANSFORM_FLIPPED_270: {
            monitorData->orientation = PAL_ORIENTATION_PORTRAIT_FLIPPED;
            break;
        }
    }

    snprintf(monitorData->name, 32, "%s %s", make, model);
}

static void outputMode(
    void* data,
    struct wl_output* output,
    uint32_t flags,
    int32_t width,
    int32_t height,
    int32_t refresh)
{
    MonitorData* monitorData = data;
    if (flags & WL_OUTPUT_MODE_CURRENT) {
        monitorData->w = width;
        monitorData->h = height;
        monitorData->refreshRate = (refresh + 500) / 1000;

        // wayland only sends the current mode
        monitorData->mode.bpp = 0;
        monitorData->mode.width = width;
        monitorData->mode.height = height;
        monitorData->mode.refreshRate = (refresh + 500) / 1000;
    }
}

static void outputScale(
    void* data,
    struct wl_output* output,
    int32_t scale)
{
    MonitorData* monitorData = data;
    float dpi = (float)scale * 96.0f;
    monitorData->dpi = (uint32_t)dpi;
}

static void outputDone(
    void* data,
    struct wl_output* output)
{
}

static const struct wl_output_listener s_OutputListener = {
    .geometry = outputGeometry,
    .mode = outputMode,
    .done = outputDone,
    .scale = outputScale};

static const struct wl_output_listener s_DefaultModeListener = {
    .geometry = outputGeometry,
    .mode = outputMode,
    .done = outputDone,
    .scale = outputScale};

static void globalHandle(
    void* data,
    struct wl_registry* registry,
    uint32_t name,
    const char* interface,
    uint32_t version)
{
    if (s_Wl.checkFeatures) {
        PalVideoFeatures features = 0;
        features |= PAL_VIDEO_FEATURE_HIGH_DPI;
        features |= PAL_VIDEO_FEATURE_MONITOR_GET_ORIENTATION;
        features |= PAL_VIDEO_FEATURE_MULTI_MONITORS;
        features |= PAL_VIDEO_FEATURE_MONITOR_GET_MODE;
        features |= PAL_VIDEO_FEATURE_WINDOW_SET_TITLE;
        features |= PAL_VIDEO_FEATURE_WINDOW_SET_SIZE;
        features |= PAL_VIDEO_FEATURE_WINDOW_SET_STATE;
        features |= PAL_VIDEO_FEATURE_BORDERLESS_WINDOW;
        features |= PAL_VIDEO_FEATURE_WINDOW_SET_CURSOR;

        s_Video.features = features;
        s_Wl.checkFeatures = PAL_FALSE;
    }

    if (strcmp(interface, "wl_compositor") == 0) {
        s_Wl.compositor = registryBind(registry, name, s_Wl.compositorInterface, 4);

    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        s_Wl.xdgBase = registryBind(registry, name, &xdg_wm_base_interface, 1);

        xdgWmBaseAddListener(s_Wl.xdgBase, &wmBaseListener, nullptr);

    } else if (strcmp(interface, "wl_shm") == 0) {
        s_Wl.shm = registryBind(registry, name, s_Wl.shmInterface, 1);

    } else if (strcmp(interface, "wl_seat") == 0) {
        s_Wl.seat = registryBind(registry, name, s_Wl.seatInterface, 5);

        seatAddListener(s_Wl.seat, &seatListener, nullptr);

    } else if (strcmp(interface, "zxdg_decoration_manager_v1") == 0) {
        s_Wl.decorationManager =
            registryBind(registry, name, &zxdg_decoration_manager_v1_interface, 1);

        s_Video.features |= PAL_VIDEO_FEATURE_DECORATED_WINDOW;

    } else if (strcmp(interface, "wl_output") == 0) {
        // wayland does not let use query monitors directly
        // so we enumerate and store at init and update the
        // cache when a monitor is added or removed
        MonitorData* monitorData = getFreeMonitorData();
        if (!monitorData) {
            return;
        }

        struct wl_output* output = nullptr;
        output = registryBind(s_Wl.registry, name, s_Wl.outputInterface, 3);
        outputAddListener(output, &s_OutputListener, monitorData);

        monitorData->wlName = name;
        monitorData->monitor = (PalMonitor*)output;
        s_Wl.monitorCount++;
    }
}

static void globalRemove(
    void* data,
    struct wl_registry* registry,
    uint32_t name)
{
    for (int i = 0; i < s_Video.maxMonitorData; ++i) {
        if (s_Video.monitorData[i].used && s_Video.monitorData[i].wlName == name) {
            MonitorData* data = &s_Video.monitorData[i];
            data->used = PAL_FALSE;
            s_Wl.proxyDestroy((struct wl_proxy*)data->monitor);
            s_Wl.monitorCount--;
        }
    }
}

static const struct wl_registry_listener s_RegistryListener = {
    .global = globalHandle,
    .global_remove = globalRemove};

#endif // PAL_HAS_WAYLAND_BACKEND
#endif // __linux__
#endif // _PAL_WAYLAND_HELPER_H 