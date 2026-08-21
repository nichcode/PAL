
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_WAYLAND_BACKEND == 1
#define _POSIX_C_SOURCE 200112L
#define _GNU_SOURCE
#include "pal_wayland.h"
#include "pal_wayland_protocols.h"
#include "video/pal_video_egl.h"
#include <dlfcn.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    PalBool pendingScroll;
    int32_t lastX;
    int32_t lastY;
    int32_t dx;
    int32_t dy;
    int32_t WheelX;
    int32_t WheelY;
    PalBool state[PAL_MOUSE_BUTTON_COUNT];
    double tmpScrollX;
    double tmpScrollY;
    double accumScrollX;
    double accumScrollY;
} Mouse;

typedef struct {
    PalBool scancodeState[PAL_SCANCODE_COUNT];
    PalBool keycodeState[PAL_KEYCODE_COUNT];
    int repeatRate;
    int repeatDelay;
    int repeatKey;
    int repeatScancode;
    int scancodes[512];
    int keycodes[256];
    uint64_t timer;
    uint64_t frequency;
} Keyboard;

Wayland s_Wl = {0};
static Mouse s_Mouse = {0};
static Keyboard s_Keyboard = {0};

// ==================================================
// Helpers
// ==================================================

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

struct wl_buffer* createShmBuffer(
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

    pool = wlShmCreatePool(s_Wl.shm, fd, size);
    if (!pool) {
        return nullptr;
    }

    buffer = wlShmPoolCreateBuffer(pool, 0, width, height, stride, format);
    if (!buffer) {
        return nullptr;
    }

    wlShmPoolDestroy(pool);
    munmap(data, size);
    close(fd);
    return buffer;
}

static void createScancodeTable()
{
    // Letters
    s_Keyboard.scancodes[0x01E] = PAL_SCANCODE_A;
    s_Keyboard.scancodes[0x030] = PAL_SCANCODE_B;
    s_Keyboard.scancodes[0x02E] = PAL_SCANCODE_C;
    s_Keyboard.scancodes[0x020] = PAL_SCANCODE_D;
    s_Keyboard.scancodes[0x012] = PAL_SCANCODE_E;
    s_Keyboard.scancodes[0x021] = PAL_SCANCODE_F;
    s_Keyboard.scancodes[0x022] = PAL_SCANCODE_G;
    s_Keyboard.scancodes[0x023] = PAL_SCANCODE_H;
    s_Keyboard.scancodes[0x017] = PAL_SCANCODE_I;
    s_Keyboard.scancodes[0x024] = PAL_SCANCODE_J;
    s_Keyboard.scancodes[0x025] = PAL_SCANCODE_K;
    s_Keyboard.scancodes[0x026] = PAL_SCANCODE_L;
    s_Keyboard.scancodes[0x032] = PAL_SCANCODE_M;
    s_Keyboard.scancodes[0x031] = PAL_SCANCODE_N;
    s_Keyboard.scancodes[0x018] = PAL_SCANCODE_O;
    s_Keyboard.scancodes[0x019] = PAL_SCANCODE_P;
    s_Keyboard.scancodes[0x010] = PAL_SCANCODE_Q;
    s_Keyboard.scancodes[0x013] = PAL_SCANCODE_R;
    s_Keyboard.scancodes[0x01F] = PAL_SCANCODE_S;
    s_Keyboard.scancodes[0x014] = PAL_SCANCODE_T;
    s_Keyboard.scancodes[0x016] = PAL_SCANCODE_U;
    s_Keyboard.scancodes[0x02F] = PAL_SCANCODE_V;
    s_Keyboard.scancodes[0x011] = PAL_SCANCODE_W;
    s_Keyboard.scancodes[0x02D] = PAL_SCANCODE_X;
    s_Keyboard.scancodes[0x015] = PAL_SCANCODE_Y;
    s_Keyboard.scancodes[0x02C] = PAL_SCANCODE_Z;

    // Numbers (top row)
    s_Keyboard.scancodes[0x00B] = PAL_SCANCODE_0;
    s_Keyboard.scancodes[0x002] = PAL_SCANCODE_1;
    s_Keyboard.scancodes[0x003] = PAL_SCANCODE_2;
    s_Keyboard.scancodes[0x004] = PAL_SCANCODE_3;
    s_Keyboard.scancodes[0x005] = PAL_SCANCODE_4;
    s_Keyboard.scancodes[0x006] = PAL_SCANCODE_5;
    s_Keyboard.scancodes[0x007] = PAL_SCANCODE_6;
    s_Keyboard.scancodes[0x008] = PAL_SCANCODE_7;
    s_Keyboard.scancodes[0x009] = PAL_SCANCODE_8;
    s_Keyboard.scancodes[0x00A] = PAL_SCANCODE_9;

    // Function
    s_Keyboard.scancodes[0x03B] = PAL_SCANCODE_F1;
    s_Keyboard.scancodes[0x03C] = PAL_SCANCODE_F2;
    s_Keyboard.scancodes[0x03D] = PAL_SCANCODE_F3;
    s_Keyboard.scancodes[0x03E] = PAL_SCANCODE_F4;
    s_Keyboard.scancodes[0x03F] = PAL_SCANCODE_F5;
    s_Keyboard.scancodes[0x040] = PAL_SCANCODE_F6;
    s_Keyboard.scancodes[0x041] = PAL_SCANCODE_F7;
    s_Keyboard.scancodes[0x042] = PAL_SCANCODE_F8;
    s_Keyboard.scancodes[0x043] = PAL_SCANCODE_F9;
    s_Keyboard.scancodes[0x044] = PAL_SCANCODE_F10;
    s_Keyboard.scancodes[0x057] = PAL_SCANCODE_F11;
    s_Keyboard.scancodes[0x058] = PAL_SCANCODE_F12;

    // Control
    s_Keyboard.scancodes[0x001] = PAL_SCANCODE_ESCAPE;
    s_Keyboard.scancodes[0x01C] = PAL_SCANCODE_ENTER;
    s_Keyboard.scancodes[0x00F] = PAL_SCANCODE_TAB;
    s_Keyboard.scancodes[0x00E] = PAL_SCANCODE_BACKSPACE;
    s_Keyboard.scancodes[0x039] = PAL_SCANCODE_SPACE;
    s_Keyboard.scancodes[0x03A] = PAL_SCANCODE_CAPSLOCK;
    s_Keyboard.scancodes[0x045] = PAL_SCANCODE_NUMLOCK;
    s_Keyboard.scancodes[0x046] = PAL_SCANCODE_SCROLLLOCK;
    s_Keyboard.scancodes[0x02A] = PAL_SCANCODE_LSHIFT;
    s_Keyboard.scancodes[0x036] = PAL_SCANCODE_RSHIFT;
    s_Keyboard.scancodes[0x01D] = PAL_SCANCODE_LCTRL;
    s_Keyboard.scancodes[0x061] = PAL_SCANCODE_RCTRL;
    s_Keyboard.scancodes[0x038] = PAL_SCANCODE_LALT;
    s_Keyboard.scancodes[0x064] = PAL_SCANCODE_RALT;

    // Arrows
    s_Keyboard.scancodes[0x069] = PAL_SCANCODE_LEFT;
    s_Keyboard.scancodes[0x06A] = PAL_SCANCODE_RIGHT;
    s_Keyboard.scancodes[0x067] = PAL_SCANCODE_UP;
    s_Keyboard.scancodes[0x06C] = PAL_SCANCODE_DOWN;

    // Navigation
    s_Keyboard.scancodes[0x06E] = PAL_SCANCODE_INSERT;
    s_Keyboard.scancodes[0x06F] = PAL_SCANCODE_DELETE;
    s_Keyboard.scancodes[0x066] = PAL_SCANCODE_HOME;
    s_Keyboard.scancodes[0x067] = PAL_SCANCODE_END;
    s_Keyboard.scancodes[0x068] = PAL_SCANCODE_PAGEUP;
    s_Keyboard.scancodes[0x06D] = PAL_SCANCODE_PAGEDOWN;

    // Keypad
    s_Keyboard.scancodes[0x052] = PAL_SCANCODE_KP_0;
    s_Keyboard.scancodes[0x04F] = PAL_SCANCODE_KP_1;
    s_Keyboard.scancodes[0x050] = PAL_SCANCODE_KP_2;
    s_Keyboard.scancodes[0x051] = PAL_SCANCODE_KP_3;
    s_Keyboard.scancodes[0x04B] = PAL_SCANCODE_KP_4;
    s_Keyboard.scancodes[0x04C] = PAL_SCANCODE_KP_5;
    s_Keyboard.scancodes[0x04D] = PAL_SCANCODE_KP_6;
    s_Keyboard.scancodes[0x047] = PAL_SCANCODE_KP_7;
    s_Keyboard.scancodes[0x048] = PAL_SCANCODE_KP_8;
    s_Keyboard.scancodes[0x049] = PAL_SCANCODE_KP_9;
    s_Keyboard.scancodes[0x060] = PAL_SCANCODE_KP_ENTER;
    s_Keyboard.scancodes[0x04E] = PAL_SCANCODE_KP_ADD;
    s_Keyboard.scancodes[0x04A] = PAL_SCANCODE_KP_SUBTRACT;
    s_Keyboard.scancodes[0x037] = PAL_SCANCODE_KP_MULTIPLY;
    s_Keyboard.scancodes[0x062] = PAL_SCANCODE_KP_DIVIDE;
    s_Keyboard.scancodes[0x053] = PAL_SCANCODE_KP_DECIMAL;

    // Misc
    s_Keyboard.scancodes[0x063] = PAL_SCANCODE_PRINTSCREEN;
    s_Keyboard.scancodes[0x066] = PAL_SCANCODE_PAUSE;
    s_Keyboard.scancodes[0x07F] = PAL_SCANCODE_MENU;
    s_Keyboard.scancodes[0x028] = PAL_SCANCODE_APOSTROPHE;
    s_Keyboard.scancodes[0x02B] = PAL_SCANCODE_BACKSLASH;
    s_Keyboard.scancodes[0x033] = PAL_SCANCODE_COMMA;
    s_Keyboard.scancodes[0x00D] = PAL_SCANCODE_EQUAL;
    s_Keyboard.scancodes[0x029] = PAL_SCANCODE_GRAVEACCENT;
    s_Keyboard.scancodes[0x00C] = PAL_SCANCODE_SUBTRACT;
    s_Keyboard.scancodes[0x034] = PAL_SCANCODE_PERIOD;
    s_Keyboard.scancodes[0x027] = PAL_SCANCODE_SEMICOLON;
    s_Keyboard.scancodes[0x035] = PAL_SCANCODE_SLASH;
    s_Keyboard.scancodes[0x01A] = PAL_SCANCODE_LBRACKET;
    s_Keyboard.scancodes[0x01B] = PAL_SCANCODE_RBRACKET;
    s_Keyboard.scancodes[0x07D] = PAL_SCANCODE_LSUPER;
    s_Keyboard.scancodes[0x07E] = PAL_SCANCODE_RSUPER;
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

MonitorData* wlGetFreeMonitorData()
{
    for (int i = 0; i < s_Wl.maxMonitorData; ++i) {
        if (!s_Wl.monitorData[i].used) {
            s_Wl.monitorData[i].used = PAL_TRUE;
            return &s_Wl.monitorData[i];
        }
    }

    // resize the data array
    // this will almost not reach here since most setups are 1-4 monitors
    MonitorData* data = nullptr;
    int count = s_Wl.maxMonitorData * 2; // double the size
    int freeIndex = s_Wl.maxMonitorData + 1;
    data = palAllocate(s_Wl.allocator, sizeof(MonitorData) * count, 0);
    if (data) {
        memset(data, 0, sizeof(MonitorData) * count);
        memcpy(data, s_Wl.monitorData, s_Wl.maxMonitorData * sizeof(MonitorData));

        palFree(s_Wl.allocator, s_Wl.monitorData);
        s_Wl.monitorData = data;
        s_Wl.maxMonitorData = count;

        s_Wl.monitorData[freeIndex].used = PAL_TRUE;
        return &s_Wl.monitorData[freeIndex];
    }
    return nullptr;
}

MonitorData* wlFindMonitorData(PalMonitor* monitor)
{
    for (int i = 0; i < s_Wl.maxMonitorData; ++i) {
        if (s_Wl.monitorData[i].used && s_Wl.monitorData[i].monitor == monitor) {
            return &s_Wl.monitorData[i];
        }
    }
    return nullptr;
}

void wlFreeMonitorData(PalMonitor* monitor)
{
    for (int i = 0; i < s_Wl.maxMonitorData; ++i) {
        if (s_Wl.monitorData[i].used && s_Wl.monitorData[i].monitor == monitor) {
            s_Wl.monitorData[i].used = PAL_FALSE;
        }
    }
}

WindowData* wlGetFreeWindowData()
{
    for (int i = 0; i < s_Wl.maxWindowData; ++i) {
        if (!s_Wl.windowData[i].used) {
            s_Wl.windowData[i].used = PAL_TRUE;
            return &s_Wl.windowData[i];
        }
    }

    // resize the data array
    // It is rare for a user to create and manage
    // 32 windows at the same time
    WindowData* data = nullptr;
    int count = s_Wl.maxWindowData * 2; // double the size
    int freeIndex = s_Wl.maxWindowData + 1;
    data = palAllocate(s_Wl.allocator, sizeof(WindowData) * count, 0);
    if (data) {
        memset(data, 0, sizeof(WindowData) * count);
        memcpy(data, s_Wl.windowData, s_Wl.maxWindowData * sizeof(WindowData));

        palFree(s_Wl.allocator, s_Wl.windowData);
        s_Wl.windowData = data;
        s_Wl.maxWindowData = count;

        s_Wl.windowData[freeIndex].used = PAL_TRUE;
        return &s_Wl.windowData[freeIndex];
    }
    return nullptr;
}

WindowData* wlFindWindowData(PalWindow* window)
{
    for (int i = 0; i < s_Wl.maxWindowData; ++i) {
        if (s_Wl.windowData[i].used && s_Wl.windowData[i].window == window) {
            return &s_Wl.windowData[i];
        }
    }
    return nullptr;
}

static inline uint64_t getCurrentTime()
{
    uint64_t now = palGetPerformanceCounter();
    return (now * 1000) / s_Keyboard.frequency;
}

// ==================================================
// Registry
// ==================================================

extern struct wl_interface xdg_wm_base_interface;

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

        s_Wl.features = features;
        s_Wl.checkFeatures = PAL_FALSE;
    }

    if (strcmp(interface, "wl_compositor") == 0) {
        s_Wl.compositor = wlRegistryBind(registry, name, s_Wl.compositorInterface, 4);

    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        s_Wl.xdgBase = wlRegistryBind(registry, name, &xdg_wm_base_interface, 1);

        xdgWmBaseAddListener(s_Wl.xdgBase, &s_WmBaseListener, nullptr);

    } else if (strcmp(interface, "wl_shm") == 0) {
        s_Wl.shm = wlRegistryBind(registry, name, s_Wl.shmInterface, 1);

    } else if (strcmp(interface, "wl_seat") == 0) {
        s_Wl.seat = wlRegistryBind(registry, name, s_Wl.seatInterface, 5);

        wlSeatAddListener(s_Wl.seat, &s_SeatListener, nullptr);

    } else if (strcmp(interface, "zxdg_decoration_manager_v1") == 0) {
        s_Wl.decorationManager =
            wlRegistryBind(registry, name, &zxdg_decoration_manager_v1_interface, 1);

        s_Wl.features |= PAL_VIDEO_FEATURE_DECORATED_WINDOW;

    } else if (strcmp(interface, "wl_output") == 0) {
        // wayland does not let use query monitors directly
        // so we enumerate and store at init and update the
        // cache when a monitor is added or removed
        MonitorData* monitorData = wlGetFreeMonitorData();
        if (!monitorData) {
            return;
        }

        struct wl_output* output = nullptr;
        output = wlRegistryBind(s_Wl.registry, name, s_Wl.outputInterface, 3);
        wlOutputAddListener(output, &s_OutputListener, monitorData);

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
    for (int i = 0; i < s_Wl.maxMonitorData; ++i) {
        if (s_Wl.monitorData[i].used && s_Wl.monitorData[i].wlName == name) {
            MonitorData* data = &s_Wl.monitorData[i];
            data->used = PAL_FALSE;
            s_Wl.proxyDestroy((struct wl_proxy*)data->monitor);
            s_Wl.monitorCount--;
        }
    }
}

// ==================================================
// Output
// ==================================================

static void outputGeometry(
    void* data,
    struct wl_output* output,
    int32_t x,
    int32_t y,
    int32_t tmp,  // we dont need physical size
    int32_t tmp2, // we dont need physical size
    int32_t tmp3, // we dont need subpixel
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

// ==================================================
// Surface
// ==================================================

static void surfaceHandleEnter(
    void* userData,
    struct wl_surface* surface,
    struct wl_output* output)
{
    WindowData* data = userData;
    MonitorData* monitorData = wlFindMonitorData((PalMonitor*)output);
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
    PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
    PalEventType type = PAL_EVENT_TYPE_MONITOR_DPI_CHANGED;
    if (!s_Wl.eventDriver) {
        return;
    }

    PalEventDriver* driver = s_Wl.eventDriver;
    mode = palGetEventDispatchMode(driver, type);
    if (mode == PAL_DISPATCH_MODE_NONE) {
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
    MonitorData* monitorData = wlFindMonitorData((PalMonitor*)output);
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

// ==================================================
// Pointer
// ==================================================

static void pointerHandleEnter(
    void* userData,
    struct wl_pointer* pointer,
    uint32_t serial,
    struct wl_surface* surface,
    wl_fixed_t surface_x,
    wl_fixed_t surface_y)
{
    WindowData* data = wlFindWindowData((PalWindow*)surface);
    if (!data) {
        return;
    }

    if (data->cursor) {
        // our window
        WaylandCursor* cursor = data->cursor;
        wlPointerSetCursor(pointer, serial, cursor->surface, cursor->hotspotX, cursor->hotspotY);
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
        s_Wl.pointerSurface = nullptr;
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

    PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
    PalWindow* window = (PalWindow*)s_Wl.pointerSurface;
    if (s_Wl.eventDriver && window) {
        // we only push a mouse move only if we are on a window
        PalEventDriver* driver = s_Wl.eventDriver;
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
    PalEventType type = PAL_EVENT_TYPE_MOUSE_BUTTONUP;

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
        type = PAL_EVENT_TYPE_MOUSE_BUTTONDOWN;
    }

    s_Mouse.state[_button] = pressed;
    if (s_Wl.eventDriver) {
        PalEventDriver* driver = s_Wl.eventDriver;
        PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
        mode = palGetEventDispatchMode(driver, type);
        if (mode != PAL_DISPATCH_MODE_NONE) {
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
    if (s_Wl.eventDriver) {
        PalEventType type = PAL_EVENT_TYPE_MOUSE_WHEEL;
        PalEventDriver* driver = s_Wl.eventDriver;
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

// ==================================================
// Keyboard
// ==================================================

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
        s_Wl.keyboardSurface = nullptr;
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
    PalEventType type = PAL_EVENT_TYPE_KEYUP;
    PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
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

        type = PAL_EVENT_TYPE_KEYDOWN;

    } else {
        // key release
        if (s_Keyboard.repeatKey == keycode) {
            s_Keyboard.repeatKey = 0;
        }
    }

    if (s_Wl.eventDriver) {
        PalEventDriver* driver = s_Wl.eventDriver;
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

// ==================================================
// Seat
// ==================================================

static void seatHandleCapabilities(
    void* userData,
    struct wl_seat* seat,
    enum wl_seat_capability caps)
{
    if (caps & WL_SEAT_CAPABILITY_KEYBOARD) {
        s_Wl.keyboard = wlSeatGetKeyboard(seat);
        wlKeyboardAddListener(s_Wl.keyboard, &s_KeyboardListener, nullptr);
    }

    if (caps & WL_SEAT_CAPABILITY_POINTER) {
        s_Wl.pointer = wlSeatGetPointer(seat);
        wlPointerAddListener(s_Wl.pointer, &s_PointerListener, nullptr);
    }
}

static void seatHandleName(
    void* userData,
    struct wl_seat* seat,
    const char* name)
{
}

// ==================================================
// Xdg
// ==================================================

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
                wlSurfaceAttach(_surface, buffer, 0, 0);
                wlSurfaceDamageBuffer(_surface, 0, 0, winData->w, winData->h);
                wlSurfaceCommit(_surface);

                // destroy old buffer
                wlBufferDestroy(winData->buffer);
                winData->buffer = buffer;
            }

            // push a window resize event
            if (s_Wl.eventDriver) {
                PalEventType type = PAL_EVENT_TYPE_WINDOW_SIZE;
                PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
                mode = palGetEventDispatchMode(s_Wl.eventDriver, type);
                if (mode != PAL_DISPATCH_MODE_NONE) {
                    PalEvent event = {0};
                    event.type = type;
                    event.data = palPackUint32(winData->w, winData->h);
                    event.data2 = palPackPointer(winData->window);
                    palPushEvent(s_Wl.eventDriver, &event);
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
        if (s_Wl.eventDriver) {
            PalEventType type = PAL_EVENT_TYPE_WINDOW_STATE;
            PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
            mode = palGetEventDispatchMode(s_Wl.eventDriver, type);
            if (mode != PAL_DISPATCH_MODE_NONE) {
                PalEvent event = {0};
                event.type = type;
                event.data = winData->state;
                event.data2 = palPackPointer(winData->window);
                palPushEvent(s_Wl.eventDriver, &event);
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

    if (s_Wl.eventDriver) {
        PalEventDriver* driver = s_Wl.eventDriver;
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
}

static void xdgToplevelHandleClose(
    void* data,
    struct xdg_toplevel* toplevel)
{
    WindowData* winData = (WindowData*)data;
    if (s_Wl.eventDriver) {
        PalEventType type = PAL_EVENT_TYPE_WINDOW_CLOSE;
        PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
        mode = palGetEventDispatchMode(s_Wl.eventDriver, type);
        if (mode != PAL_DISPATCH_MODE_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data2 = palPackPointer(winData->window);
            palPushEvent(s_Wl.eventDriver, &event);
        }
    }
}

// ==================================================
// Zxdg Decoration
// ==================================================

void zxdgDecorationHandleConfigure(
    void* data,
    struct zxdg_toplevel_decoration_v1* dec,
    uint32_t mode)
{
    if (s_Wl.eventDriver) {
        PalEventDriver* driver = s_Wl.eventDriver;
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
}

PalResult wlInitVideo(
    const PalAllocator* allocator,
    PalEventDriver* eventDriver,
    void* preferredInstance)
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
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_POSIX, 
            errno);
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

    s_Wl.maxMonitorData = 16; // initial size
    s_Wl.maxWindowData = 32;  // initial size
    s_Wl.windowData = palAllocate(s_Wl.allocator, sizeof(WindowData) * s_Wl.maxWindowData, 0);
    s_Wl.monitorData = palAllocate(s_Wl.allocator, sizeof(MonitorData) * s_Wl.maxMonitorData, 0);
    if (!s_Wl.monitorData || !s_Wl.windowData) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    memset(s_Wl.windowData, 0, sizeof(WindowData) * s_Wl.maxWindowData);
    memset(s_Wl.monitorData, 0, sizeof(MonitorData) * s_Wl.maxMonitorData);

    // initialize wayland
    s_Wl.checkFeatures = PAL_TRUE;
    s_Wl.monitorCount = 0;
    setupProtocols();

    // check if user supplied their own display
    if (preferredInstance) {
        s_Wl.display = (struct wl_display*)preferredInstance;

    } else {
        s_Wl.display = s_Wl.displayConnect(nullptr);
        if (!s_Wl.display) {
            return PAL_RESULT_CODE_PLATFORM_FAILURE;
        }
    }

    s_Wl.display = (void*)s_Wl.display;
    s_Wl.registry = wlDisplayGetRegistry(s_Wl.display);
    wlRegistryAddListener(s_Wl.registry, &s_RegistryListener, nullptr);
    s_Wl.displayRoundtrip(s_Wl.display);

    // do a roundtrip again to get remaining handles
    s_Wl.displayRoundtrip(s_Wl.display);

    if (!s_Wl.compositor || !s_Wl.xdgBase || !s_Wl.shm) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    // create an input context
    s_Wl.inputContext = s_Wl.xkbContextNew(XKB_CONTEXT_NO_FLAGS);
    if (!s_Wl.inputContext) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    // get the current theme
    s_Wl.cursorTheme = s_Wl.cursorThemeLoad(nullptr, 32, s_Wl.shm);
    if (!s_Wl.cursorTheme) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    // load EGL
    s_VideoEgl.handle = dlopen("libEGL.so", RTLD_LAZY);
    if (s_VideoEgl.handle) {
        eglGetProcAddressFn load = nullptr;
        load = (eglGetProcAddressFn)dlsym(s_VideoEgl.handle, "eglGetProcAddress");

        s_VideoEgl.initialize = (eglInitializeFn)load("eglInitialize");
        s_VideoEgl.terminate = (eglTerminateFn)load("eglTerminate");
        s_VideoEgl.getDisplay = (eglGetDisplayFn)load("eglGetDisplay");
        s_VideoEgl.chooseConfig = (eglChooseConfigFn)load("eglChooseConfig");
        s_VideoEgl.getError = (eglGetErrorFn)load("eglGetError");
        s_VideoEgl.bindAPI = (eglBindAPIFn)load("eglBindAPI");
        s_VideoEgl.getConfigs = (eglGetConfigsFn)load("eglGetConfigs");
        s_VideoEgl.getConfigAttrib = (eglGetConfigAttribFn)load("eglGetConfigAttrib");
    }

    createKeycodeTable();
    createScancodeTable();

    s_Wl.allocator = allocator;
    s_Wl.eventDriver = eventDriver;
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

    if (!s_Wl.display) {
        // opened by PAL
        s_Wl.displayDisconnect(s_Wl.display);

        dlclose(s_Wl.libCursor);
        dlclose(s_Wl.xkbCommon);
        dlclose(s_Wl.libWaylandEgl);
        dlclose(s_Wl.handle);
    }

    palFree(s_Wl.allocator, s_Wl.windowData);
    palFree(s_Wl.allocator, s_Wl.monitorData);
    if (s_VideoEgl.handle) {
        dlclose(s_VideoEgl.handle);
    }

    memset(&s_Wl, 0, sizeof(Wayland));
}

void wlUpdateVideo()
{
    // flush pending requests
    s_Mouse.tmpScrollX = 0;
    s_Mouse.tmpScrollY = 0;

    // push key repeats
    // we only do this if the user wants key repeat events
    if (s_Keyboard.repeatKey != 0 && s_Wl.eventDriver) {
        PalEventDriver* driver = s_Wl.eventDriver;
        PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
        mode = palGetEventDispatchMode(driver, PAL_EVENT_TYPE_KEYREPEAT);
        if (mode != PAL_DISPATCH_MODE_NONE) {
            // get now time and check with the key repeat time
            uint64_t now = getCurrentTime();
            if (now >= s_Keyboard.timer) {
                PalWindow* window = (PalWindow*)s_Wl.keyboardSurface;
                PalKeycode key = s_Keyboard.repeatKey;
                PalScancode scancode = s_Keyboard.repeatScancode;

                PalEvent event = {0};
                event.type = PAL_EVENT_TYPE_KEYREPEAT;
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

PalVideoFeatures wlGetVideoFeatures()
{
    return s_Wl.features;
}

const PalBool* wlGetKeycodeState()
{
    return s_Keyboard.keycodeState;
}

const PalBool* wlGetScancodeState()
{
    return s_Keyboard.scancodeState;
}

const PalBool* wlGetMouseState()
{
    return s_Mouse.state;
}

void wlGetMouseDelta(
    float* dx,
    float* dy)
{
    if (dx) {
        *dx = (float)s_Mouse.dx;
    }

    if (dy) {
        *dy = (float)s_Mouse.dy;
    }
}

void wlGetMouseWheelDelta(
    float* dx,
    float* dy)
{
    if (dx) {
        *dx = (float)s_Mouse.WheelX;
    }

    if (dy) {
        *dy = (float)s_Mouse.WheelY;
    }
}

void* wlGetInstance()
{
    return (void*)s_Wl.display;
}

// ==================================================
// Listeners
// ==================================================

struct wl_registry_listener s_RegistryListener = {
    .global = globalHandle,
    .global_remove = globalRemove};

struct wl_output_listener s_OutputListener =
    {.geometry = outputGeometry, .mode = outputMode, .done = outputDone, .scale = outputScale};

struct wl_output_listener s_DefaultModeListener =
    {.geometry = outputGeometry, .mode = outputMode, .done = outputDone, .scale = outputScale};

struct wl_surface_listener s_SurfaceListener = {
    .enter = surfaceHandleEnter,
    .leave = surfaceHandleLeave};

struct wl_pointer_listener s_PointerListener = {
    .enter = pointerHandleEnter,
    .leave = pointerHandleLeave,
    .motion = pointerHandleMotion,
    .button = pointerHandleButton,
    .axis = pointerHandleAxis,
    .axis_discrete = pointerHandleAxisDiscrete,
    .frame = pointerHandleFrame,
    .axis_source = pointerHandleAxisSource,
    .axis_stop = pointerHandleAxisStop};

struct wl_keyboard_listener s_KeyboardListener = {
    .enter = keyboardHandleEnter,
    .leave = keyboardHandleLeave,
    .keymap = keyboardHandleRemap,
    .key = keyboardHandleKey,
    .repeat_info = keyboardHandleRepeatInfo,
    .modifiers = keyboardHandleModifiers};

struct zxdg_toplevel_decoration_v1_listener s_DecorationListener = {
    .configure = zxdgDecorationHandleConfigure};

struct wl_seat_listener s_SeatListener = {
    .capabilities = seatHandleCapabilities,
    .name = seatHandleName};

struct xdg_wm_base_listener s_WmBaseListener = {.ping = wmBaseHandlePing};

struct xdg_surface_listener s_XdgSurfaceListener = {.configure = xdgSurfaceHandleConfigure};

struct xdg_toplevel_listener s_XdgToplevelListener = {
    .configure = xdgToplevelHandleConfigure,
    .close = xdgToplevelHandleClose,
    .configure_bounds = nullptr,
    .wm_capabilities = nullptr};

#endif // PAL_HAS_WAYLAND_BACKEND