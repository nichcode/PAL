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

#if PAL_HAS_WAYLAND_BACKEND

#if PAL_HAS_EVENT_MODULE
#include "pal2/core/pack.h"
#include "pal2/event/driver.h"
#endif // PAL_HAS_EVENT_MODULE

#include "listeners.h"
#include "pal2/core/time.h"
#include "video/platform.h"
#include <linux/input-event-codes.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

static inline uint64_t getCurrentTime(void)
{
    uint64_t now = palGetPerformanceCounter();
    return (now * 1000) / g_WlKeyboard.frequency;
}

/* =========================================================================
 * Output
 * ========================================================================= */

static void outputGeometry(
    void* data,
    struct wl_output* output,
    int32_t x,
    int32_t y,
    int32_t tmp,
    int32_t tmp2,
    int32_t tmp3,
    const char* make,
    const char* model,
    int32_t transform)
{
    (void)output;
    (void)tmp;
    (void)tmp2;
    (void)tmp3;

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

    snprintf(monitorData->name, PAL_MONITOR_NAME_SIZE, "%s %s", make, model);
}

static void outputMode(
    void* data,
    struct wl_output* output,
    uint32_t flags,
    int32_t width,
    int32_t height,
    int32_t refresh)
{
    (void)output;

    MonitorData* monitorData = data;
    if (flags & WL_OUTPUT_MODE_CURRENT) {
        monitorData->w = (uint32_t)width;
        monitorData->h = (uint32_t)height;
        monitorData->refreshRate = (uint32_t)((refresh + 500) / 1000);

        // wayland only sends the current mode
        monitorData->mode.bpp = 0;
        monitorData->mode.width = (uint32_t)width;
        monitorData->mode.height = (uint32_t)height;
        monitorData->mode.refreshRate = (uint32_t)((refresh + 500) / 1000);
    }
}

static void outputScale(
    void* data,
    struct wl_output* output,
    int32_t scale)
{
    (void)output;

    MonitorData* monitorData = data;
    float dpi = (float)scale * 96.0f;
    monitorData->dpi = (uint32_t)dpi;
}

static void outputDone(
    void* data,
    struct wl_output* output)
{
    (void)data;
    (void)output;
}

/* =========================================================================
 * Surface
 * ========================================================================= */

static void surfaceHandleEnter(
    void* userData,
    struct wl_surface* surface,
    struct wl_output* output)
{
    (void)surface;

    WindowData* data = userData;
    MonitorData* monitorData = wlFindMonitorData((PalMonitor*)output);
    if (!monitorData) {
        return;
    }

    SpanMonitor* existing = nullptr;
    SpanMonitor* emptySlot = nullptr;

    for (uint32_t i = 0; i < MAX_SPAN_MONITORS; i++) {
        if (data->monitors[i].monitor == (void*)output) {
            existing = &data->monitors[i];
            break;
        }
        if (!emptySlot && data->monitors[i].monitor == nullptr) {
            emptySlot = &data->monitors[i];
        }
    }

    if (existing) {
        existing->dpi = monitorData->dpi;
    } else if (emptySlot) {
        emptySlot->monitor = output;
        emptySlot->dpi = monitorData->dpi;
        data->monitorCount++;
    } else {
        // Already tracking MAX_SPAN_MONITORS
        return;
    }

    if (data->dpi == 0) {
        data->dpi = monitorData->dpi;
        return;
    }

    uint32_t maxDPI = 96;
    for (uint32_t i = 0; i < MAX_SPAN_MONITORS; i++) {
        if (data->monitors[i].monitor && data->monitors[i].dpi > maxDPI) {
            maxDPI = data->monitors[i].dpi;
        }
    }

    if (maxDPI != data->dpi) {
        data->dpi = maxDPI;

#if PAL_HAS_EVENT_MODULE
        if (!g_Wl.eventDriver) {
            return;
        }

        PalEventDriver* driver = g_Wl.eventDriver;
        PalEventType type = PAL_EVENT_TYPE_MONITOR_DPI_CHANGED;
        PalDispatchMode mode = palGetEventDispatchMode(driver, type);
        if (mode == PAL_DISPATCH_MODE_NONE) {
            return;
        }

        PalEvent event = {0};
        event.type = type;
        event.data = (uint64_t)maxDPI;
        event.data2 = palPackPointer((void*)data->window);
        palPushEvent(driver, &event);
#endif // PAL_HAS_EVENT_MODULE
    }
}

static void surfaceHandleLeave(
    void* userData,
    struct wl_surface* surface,
    struct wl_output* output)
{
    (void)surface;

    WindowData* data = userData;
    MonitorData* monitorData = wlFindMonitorData((PalMonitor*)output);
    if (!monitorData) {
        return;
    }

    for (uint32_t i = 0; i < MAX_SPAN_MONITORS; i++) {
        if (data->monitors[i].monitor == (void*)output) {
            data->monitors[i].monitor = nullptr;
            data->monitors[i].dpi = 0;
            data->monitorCount--;
            break;
        }
    }

    // Keep cached DPI intact if the surface is hidden/unmapped (0 active outputs)
    if (data->monitorCount == 0) {
        return;
    }

    uint32_t maxDPI = 96;
    for (uint32_t i = 0; i < MAX_SPAN_MONITORS; i++) {
        if (data->monitors[i].monitor && data->monitors[i].dpi > maxDPI) {
            maxDPI = data->monitors[i].dpi;
        }
    }

    if (maxDPI != data->dpi) {
        data->dpi = maxDPI;

#if PAL_HAS_EVENT_MODULE
        if (!g_Wl.eventDriver) {
            return;
        }

        PalEventDriver* driver = g_Wl.eventDriver;
        PalEventType type = PAL_EVENT_TYPE_MONITOR_DPI_CHANGED;
        PalDispatchMode mode = palGetEventDispatchMode(driver, type);
        if (mode == PAL_DISPATCH_MODE_NONE) {
            return;
        }

        PalEvent event = {0};
        event.type = type;
        event.data = (uint64_t)maxDPI;
        event.data2 = palPackPointer((void*)data->window);
        palPushEvent(driver, &event);
#endif // PAL_HAS_EVENT_MODULE
    }
}

/* =========================================================================
 * Pointer
 * ========================================================================= */

static void pointerHandleEnter(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t serial,
    struct wl_surface* surface,
    wl_fixed_t surface_x,
    wl_fixed_t surface_y)
{
    (void)userData;
    (void)surface_x;
    (void)surface_y;

    WindowData* data = wlFindWindowData((PalWindow*)surface);
    if (!data) {
        return;
    }

    data->pointer = pointer;
    data->pointerEnterSerial = serial;
    wlApplyWindowCursor(data);
    g_Wl.pointerSurface = surface;
}

static void pointerHandleLeave(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t serial,
    struct wl_surface* surface)
{
    (void)userData;
    (void)pointer;
    (void)serial;

    WindowData* data = wlFindWindowData((PalWindow*)surface);
    if (!data) {
        return;
    }

    if (g_Wl.pointerSurface == surface) {
        g_Wl.pointerSurface = nullptr;
        data->pointer = nullptr;
        data->pointerEnterSerial = 0;
    }
}

static void pointerHandleMotion(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t time,
    wl_fixed_t surface_x,
    wl_fixed_t surface_y)
{
    (void)userData;
    (void)pointer;
    (void)time;

    int x = wl_fixed_to_int(surface_x);
    int y = wl_fixed_to_int(surface_y);
    const int dx = x - g_WlMouse.lastX;
    const int dy = y - g_WlMouse.lastY;

    g_WlMouse.lastX = x;
    g_WlMouse.lastY = y;
    g_Mouse.dx = dx;
    g_Mouse.dy = dy;

#if PAL_HAS_EVENT_MODULE
    PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
    PalWindow* window = (PalWindow*)g_Wl.pointerSurface;
    if (g_Wl.eventDriver && window) {
        // we only push a mouse move only if we are on a window
        PalEventDriver* driver = g_Wl.eventDriver;
        PalEventType type = PAL_EVENT_TYPE_MOUSE_MOVE;
        mode = palGetEventDispatchMode(driver, type);
        if (mode != PAL_DISPATCH_MODE_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data = palPackInt32(x, y);
            event.data2 = palPackPointer(window);
            palPushEvent(driver, &event);
        }

        // push a mouse delta event
        type = PAL_EVENT_TYPE_MOUSE_DELTA;
        mode = palGetEventDispatchMode(driver, type);
        if (mode != PAL_DISPATCH_MODE_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data = palPackFloat((float)dx, (float)dy);
            event.data2 = palPackPointer(window);
            palPushEvent(driver, &event);
        }
    }

#endif // PAL_HAS_EVENT_MODULE
}

static void pointerHandleButton(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t serial,
    uint32_t time,
    uint32_t button,
    uint32_t state)
{
    (void)userData;
    (void)pointer;
    (void)time;

    PalWindow* window = nullptr;
    if (g_Wl.pointerSurface) {
        window = (PalWindow*)g_Wl.pointerSurface;

    } else {
        // cannot recieve events without a focused surface
        return;
    }

    PalBool pressed = state == WL_POINTER_BUTTON_STATE_PRESSED;
    PalMouseButton palButton = PAL_MOUSE_BUTTON_UNKNOWN;
    PalEventType type = PAL_EVENT_TYPE_MOUSE_BUTTONUP;

    if (button == BTN_LEFT) {
        palButton = PAL_MOUSE_BUTTON_LEFT;

    } else if (button == BTN_RIGHT) {
        palButton = PAL_MOUSE_BUTTON_RIGHT;

    } else if (button == BTN_MIDDLE) {
        palButton = PAL_MOUSE_BUTTON_MIDDLE;

    } else if (button == BTN_SIDE) {
        palButton = PAL_MOUSE_BUTTON_X1;

    } else if (button == BTN_EXTRA) {
        palButton = PAL_MOUSE_BUTTON_X2;
    }

    if (pressed) {
        type = PAL_EVENT_TYPE_MOUSE_BUTTONDOWN;
    }

    if (palButton != PAL_MOUSE_BUTTON_UNKNOWN) {
        g_Mouse.state[palButton] = pressed;
    }

#if PAL_HAS_EVENT_MODULE
    if (g_Wl.eventDriver && palButton != PAL_MOUSE_BUTTON_UNKNOWN) {
        PalEventDriver* driver = g_Wl.eventDriver;
        PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
        mode = palGetEventDispatchMode(driver, type);
        if (mode != PAL_DISPATCH_MODE_NONE) {
            PalEvent event = {0};
            event.type = type;

            // since we are not drawing decorations for users they need the serial
            // in order to draw their decorations we put the serial at the upper 32 so
            // ABI is preserved
            event.data = palPackUint32(palButton, serial);
            event.data2 = palPackPointer(window);
            palPushEvent(driver, &event);
        }
    }
#endif // PAL_HAS_EVENT_MODULE
}

static void pointerHandleAxis(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t time,
    uint32_t axis,
    wl_fixed_t value)
{
    (void)userData;
    (void)pointer;
    (void)time;
    (void)value;

    double delta = wl_fixed_to_double(value);
    if (axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL) {
        g_WlMouse.tmpScrollX += delta;
        g_WlMouse.accumScrollX += delta;

    } else if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL) {
        g_WlMouse.tmpScrollY += delta;
        g_WlMouse.accumScrollY += delta;
    }

    g_WlMouse.pendingScroll = PAL_TRUE;
}

static void pointerHandleAxisDiscrete(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t axis,
    int32_t discrete)
{
    (void)userData;
    (void)pointer;

    if (axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL) {
        g_WlMouse.tmpScrollX += discrete;
        g_WlMouse.accumScrollX += discrete;

    } else if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL) {
        g_WlMouse.tmpScrollY += discrete;
        g_WlMouse.accumScrollY += discrete;
    }

    g_WlMouse.pendingScroll = PAL_TRUE;
}

static void pointerHandleFrame(
    void* userData,
    struct wl_pointer* pointer)
{
    (void)userData;
    (void)pointer;

    if (!g_WlMouse.pendingScroll) {
        // no wheel event
        return;
    }

    PalWindow* window = nullptr;
    if (g_Wl.pointerSurface) {
        window = (PalWindow*)g_Wl.pointerSurface;

    } else {
        // cannot recieve events without a focused surface
        return;
    }

    const int dx = (int)g_WlMouse.accumScrollX;
    const int dy = (int)g_WlMouse.accumScrollY;

    g_Mouse.wheelX = dx;
    g_Mouse.wheelY = dy;
    g_WlMouse.accumScrollX -= dx;
    g_WlMouse.accumScrollY -= dy;
    g_WlMouse.pendingScroll = PAL_FALSE;

#if PAL_HAS_EVENT_MODULE
    if (g_Wl.eventDriver) {
        PalEventType type = PAL_EVENT_TYPE_MOUSE_WHEEL;
        PalEventDriver* driver = g_Wl.eventDriver;
        PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
        mode = palGetEventDispatchMode(driver, type);
        if (mode != PAL_DISPATCH_MODE_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data = palPackFloat((float)dx, (float)dy);
            event.data2 = palPackPointer(window);
            palPushEvent(driver, &event);
        }
    }
#endif // PAL_HAS_EVENT_MODULE
}

static void pointerHandleAxisSource(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t axis_source)
{
    (void)userData;
    (void)pointer;
    (void)axis_source;
}

static void pointerHandleAxisStop(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t time,
    uint32_t axis)
{
    (void)userData;
    (void)pointer;
    (void)time;
    (void)axis;
}

/* =========================================================================
 * Keyboard
 * ========================================================================= */

static void keyboardHandleEnter(
    void* userData,
    struct wl_keyboard* keyboard,
    uint32_t serial,
    struct wl_surface* surface,
    struct wl_array* keys)
{
    (void)userData;
    (void)keyboard;
    (void)serial;
    (void)keys;

    g_Wl.keyboardSurface = surface;
}

static void keyboardHandleLeave(
    void* userData,
    struct wl_keyboard* keyboard,
    uint32_t serial,
    struct wl_surface* surface)
{
    (void)userData;
    (void)keyboard;
    (void)serial;
    (void)surface;

    if (g_Wl.keyboardSurface == surface) {
        g_Wl.keyboardSurface = nullptr;
    }
}

static void keyboardHandleRemap(
    void* userData,
    struct wl_keyboard* keyboard,
    uint32_t format,
    int32_t fd,
    uint32_t size)
{
    (void)userData;
    (void)keyboard;
    (void)format;

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

    keymap = g_Wl.xkbKeymapNewFromString(
        g_Wl.inputContext,
        keymapStr,
        XKB_KEYMAP_FORMAT_TEXT_V1,
        XKB_KEYMAP_COMPILE_NO_FLAGS);

    munmap(keymapStr, size);
    close(fd);

    if (!keymap) {
        return;
    }

    state = g_Wl.xkbStateNew(keymap);
    if (!state) {
        g_Wl.xkbKeymapUnref(keymap);
        return;
    }

    // check if we have old keymap and state
    if (g_Wl.state) {
        g_Wl.xkbStateUnref(g_Wl.state);
        g_Wl.xkbKeymapUnref(g_Wl.keymap);
    }

    g_Wl.state = state;
    g_Wl.keymap = keymap;
    g_WlKeyboard.frequency = palGetPerformanceFrequency();
}

static void keyboardHandleKey(
    void* userData,
    struct wl_keyboard* keyboard,
    uint32_t serial,
    uint32_t time,
    uint32_t key,
    uint32_t state)
{
    (void)userData;
    (void)keyboard;
    (void)serial;
    (void)time;

    if (!g_Wl.state || !g_Wl.keymap) {
        return;
    }

    PalWindow* window = nullptr;
    if (g_Wl.keyboardSurface) {
        window = (PalWindow*)g_Wl.keyboardSurface;

    } else {
        // cannot recieve events without a focused surface
        return;
    }

    PalScancode scancode = 0;
    PalKeycode keycode = 0;
    PalBool pressed = (state == WL_KEYBOARD_KEY_STATE_PRESSED);
    PalEventType type = PAL_EVENT_TYPE_KEYUP;
    PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
    xkb_keysym_t keySym = g_Wl.xkbStateKeyGetOneSym(g_Wl.state, key + 8);

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
        scancode = (key < 512) ? (PalScancode)g_WlKeyboard.scancodes[key] : PAL_SCANCODE_UNKNOWN;
    }

    // printable and text input keys are from the range 32 (PAL_KEYCODE_SPACE) and
    // 122 (PAL_KEYCODE_Z) The rest are almost the same as their scancode.
    // Maybe there will be a layout that makes this wrong but for now this works.
    if (keySym >= XKB_KEY_space && keySym <= XKB_KEY_z) {
        // a printable or input key
        keycode = (PalKeycode)g_WlKeyboard.keycodes[keySym];

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

    g_Keyboard.scancodeState[scancode] = pressed;
    g_Keyboard.keycodeState[keycode] = pressed;

    // check for key repeats
    if (pressed) {
        if (g_Wl.xkbKeymapKeyRepeats(g_Wl.keymap, key + 8)) {
            g_WlKeyboard.repeatKey = (int)keycode;
            g_WlKeyboard.repeatScancode = (int)scancode;
            g_WlKeyboard.timer = getCurrentTime() + (uint64_t)g_WlKeyboard.repeatDelay;

        } else {
            g_WlKeyboard.repeatKey = 0;
        }

        type = PAL_EVENT_TYPE_KEYDOWN;

    } else {
        // key release
        if (g_WlKeyboard.repeatKey == (int)keycode) {
            g_WlKeyboard.repeatKey = 0;
        }
    }

#if PAL_HAS_EVENT_MODULE
    if (g_Wl.eventDriver) {
        PalEventDriver* driver = g_Wl.eventDriver;
        mode = palGetEventDispatchMode(driver, type);
        if (mode != PAL_DISPATCH_MODE_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data = palPackUint32(keycode, scancode);
            event.data2 = palPackPointer(window);
            palPushEvent(driver, &event);
        }

        // check for char event if enabled
        type = PAL_EVENT_TYPE_KEYCHAR;
        mode = palGetEventDispatchMode(driver, type);
        if (mode == PAL_DISPATCH_MODE_NONE) {
            return;
        }

        uint32_t codepoint = g_Wl.xkbKeysymToUtf32(keySym);
        if (codepoint <= 0) {
            return;
        }

        PalEvent charEvent = {0};
        charEvent.type = type;
        charEvent.data = codepoint;
        charEvent.data2 = palPackPointer(window);
        palPushEvent(driver, &charEvent);
    }
#endif // PAL_HAS_EVENT_MODULE
}

static void keyboardHandleRepeatInfo(
    void* userData,
    struct wl_keyboard* keyboard,
    int32_t rate,
    int32_t delay)
{
    (void)userData;

    if (g_Wl.keyboard == keyboard) {
        g_WlKeyboard.repeatDelay = delay;
        g_WlKeyboard.repeatRate = rate;

    } else {
        if (g_WlKeyboard.repeatDelay == 0) {
            g_WlKeyboard.repeatDelay = 500;
            g_WlKeyboard.repeatRate = 30;
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
    (void)userData;
    (void)keyboard;
    (void)serial;
    (void)mods_depressed;
    (void)mods_latched;
    (void)mods_locked;

    if (g_Wl.state) {
        g_Wl.xkbStateUpdateMask(g_Wl.state, mods_depressed, mods_latched, mods_locked, group, 0, 0);
    }
}

/* =========================================================================
 * Seat
 * ========================================================================= */

static void seatHandleCapabilities(
    void* userData,
    struct wl_seat* seat,
    enum wl_seat_capability caps)
{
    (void)userData;

    if (caps & WL_SEAT_CAPABILITY_KEYBOARD) {
        g_Wl.keyboard = wlSeatGetKeyboard(seat);
        wlKeyboardAddListener(g_Wl.keyboard, &g_KeyboardListener, nullptr);
    }

    if (caps & WL_SEAT_CAPABILITY_POINTER) {
        g_Wl.pointer = wlSeatGetPointer(seat);
        wlPointerAddListener(g_Wl.pointer, &g_PointerListener, nullptr);
    }
}

static void seatHandleName(
    void* userData,
    struct wl_seat* seat,
    const char* name)
{
    (void)userData;
    (void)seat;
    (void)name;
}

/* =========================================================================
 * Xdg
 * ========================================================================= */

static void wmBaseHandlePing(
    void* data,
    struct xdg_wm_base* base,
    uint32_t serial)
{
    (void)data;

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
                g_Wl.eglWindowResize(winData->eglWindow, (int)winData->w, (int)winData->h, 0, 0);

            } else {
                // create a new buffer with the new size
                struct wl_buffer* buffer = nullptr;
                buffer = wlCreateShmBuffer((int)winData->w, (int)winData->h, nullptr, PAL_FALSE);

                if (!buffer) {
                    return;
                }

                struct wl_surface* wlSurface = nullptr;
                wlSurface = (struct wl_surface*)winData->window;
                wlSurfaceAttach(wlSurface, buffer, 0, 0);
                wlSurfaceDamageBuffer(wlSurface, 0, 0, (int)winData->w, (int)winData->h);
                wlSurfaceCommit(wlSurface);

                winData->buffer = buffer;
            }

#if PAL_HAS_EVENT_MODULE
            // push a window resize event
            if (g_Wl.eventDriver) {
                PalEventType type = PAL_EVENT_TYPE_WINDOW_SIZE;
                PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
                mode = palGetEventDispatchMode(g_Wl.eventDriver, type);
                if (mode != PAL_DISPATCH_MODE_NONE) {
                    PalEvent event = {0};
                    event.type = type;
                    event.data = palPackUint32(winData->w, winData->h);
                    event.data2 = palPackPointer(winData->window);
                    palPushEvent(g_Wl.eventDriver, &event);
                }
            }
#endif // PAL_HAS_EVENT_MODULE

            winData->pushConfigureEvent = PAL_FALSE;
        }
    }

    // pending state
    if (!winData->skipState) {
        if (!winData->pushStateEvent) {
            return;
        }

#if PAL_HAS_EVENT_MODULE
        // push a window state event we dont recreate buffers over here
        // since we already create the buffer with the new size
        if (g_Wl.eventDriver) {
            PalEventType type = PAL_EVENT_TYPE_WINDOW_STATE;
            PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
            mode = palGetEventDispatchMode(g_Wl.eventDriver, type);
            if (mode != PAL_DISPATCH_MODE_NONE) {
                PalEvent event = {0};
                event.type = type;
                event.data = winData->state;
                event.data2 = palPackPointer(winData->window);
                palPushEvent(g_Wl.eventDriver, &event);
            }
        }
#endif // PAL_HAS_EVENT_MODULE

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
    (void)toplevel;

    WindowData* winData = (WindowData*)data;
    uint32_t* state;
    PalBool activated = PAL_FALSE;
    PalBool maximized = PAL_FALSE;

    wl_array_for_each(state, states)
    {
        if (*state == 1) { // XDG_TOPLEVEL_STATE_MAXIMIZED
            maximized = PAL_TRUE;
        } else if (*state == 4) { // XDG_TOPLEVEL_STATE_ACTIVATED
            activated = PAL_TRUE;
        }
    }

    if (maximized && winData->state != PAL_WINDOW_STATE_MAXIMIZED) {
        winData->state = PAL_WINDOW_STATE_MAXIMIZED;
        winData->pushStateEvent = PAL_TRUE;

    } else if (!maximized && winData->state == PAL_WINDOW_STATE_MAXIMIZED) {
        winData->state = PAL_WINDOW_STATE_NORMAL;
        winData->pushStateEvent = PAL_TRUE;
    }

    if (winData->resizable && width > 0 && height > 0) {
        if ((uint32_t)width != winData->w || (uint32_t)height != winData->h) {
            winData->pushConfigureEvent = PAL_TRUE;
        }

        winData->w = (uint32_t)width;
        winData->h = (uint32_t)height;
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

#if PAL_HAS_EVENT_MODULE
    if (g_Wl.eventDriver) {
        PalEventDriver* driver = g_Wl.eventDriver;
        PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
        PalEventType type = PAL_EVENT_TYPE_WINDOW_FOCUS;
        mode = palGetEventDispatchMode(driver, type);

        if (mode != PAL_DISPATCH_MODE_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data = winData->focused;
            event.data2 = palPackPointer(winData->window);
            palPushEvent(driver, &event);
        }
    }
#endif // PAL_HAS_EVENT_MODULE
}

static void xdgToplevelHandleClose(
    void* data,
    struct xdg_toplevel* toplevel)
{
    (void)toplevel;

    WindowData* winData = (WindowData*)data;

#if PAL_HAS_EVENT_MODULE
    if (g_Wl.eventDriver) {
        PalEventType type = PAL_EVENT_TYPE_WINDOW_CLOSE;
        PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
        mode = palGetEventDispatchMode(g_Wl.eventDriver, type);
        if (mode != PAL_DISPATCH_MODE_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data2 = palPackPointer(winData->window);
            palPushEvent(g_Wl.eventDriver, &event);
        }
    }
#endif // PAL_HAS_EVENT_MODULE
}

/* =========================================================================
 * Zxdg Decoration
 * ========================================================================= */

void zxdgDecorationHandleConfigure(
    void* data,
    struct zxdg_toplevel_decoration_v1* dec,
    uint32_t mode)
{
    (void)dec;

#if PAL_HAS_EVENT_MODULE
    if (g_Wl.eventDriver) {
        PalEventDriver* driver = g_Wl.eventDriver;
        PalDispatchMode dispatchMode = PAL_DISPATCH_MODE_NONE;
        PalEventType type = PAL_EVENT_TYPE_WINDOW_DECORATION_MODE;
        dispatchMode = palGetEventDispatchMode(driver, type);

        if (dispatchMode != PAL_DISPATCH_MODE_NONE) {
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
#endif // PAL_HAS_EVENT_MODULE
}

struct wl_output_listener g_OutputListener = {
    .geometry = outputGeometry,
    .mode = outputMode,
    .done = outputDone,
    .scale = outputScale,
};

struct wl_output_listener g_DefaultModeListener = {
    .geometry = outputGeometry,
    .mode = outputMode,
    .done = outputDone,
    .scale = outputScale,
};

struct wl_surface_listener g_SurfaceListener = {
    .enter = surfaceHandleEnter,
    .leave = surfaceHandleLeave,
};

struct wl_pointer_listener g_PointerListener = {
    .enter = pointerHandleEnter,
    .leave = pointerHandleLeave,
    .motion = pointerHandleMotion,
    .button = pointerHandleButton,
    .axis = pointerHandleAxis,
    .axis_discrete = pointerHandleAxisDiscrete,
    .frame = pointerHandleFrame,
    .axis_source = pointerHandleAxisSource,
    .axis_stop = pointerHandleAxisStop,
};

struct wl_keyboard_listener g_KeyboardListener = {
    .enter = keyboardHandleEnter,
    .leave = keyboardHandleLeave,
    .keymap = keyboardHandleRemap,
    .key = keyboardHandleKey,
    .repeat_info = keyboardHandleRepeatInfo,
    .modifiers = keyboardHandleModifiers,
};

struct zxdg_toplevel_decoration_v1_listener g_DecorationListener = {
    .configure = zxdgDecorationHandleConfigure,
};

struct wl_seat_listener g_SeatListener = {
    .capabilities = seatHandleCapabilities,
    .name = seatHandleName,
};

struct xdg_wm_base_listener g_WmBaseListener = {
    .ping = wmBaseHandlePing,
};

struct xdg_surface_listener g_XdgSurfaceListener = {
    .configure = xdgSurfaceHandleConfigure,
};

struct xdg_toplevel_listener g_XdgToplevelListener = {
    .configure = xdgToplevelHandleConfigure,
    .close = xdgToplevelHandleClose,
    .configure_bounds = nullptr,
    .wm_capabilities = nullptr,
};

#endif // PAL_HAS_WAYLAND_BACKEND