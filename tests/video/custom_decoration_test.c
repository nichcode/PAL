
#ifdef __linux__
#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L // for linux
#include "pal/pal_video.h"
#include "tests.h"

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define WINDOW_TITLE "Custom Decoration Test"
#define WL_MARSHAL_FLAG_DESTROY 1 << 0
#define TITLEBAR_HEIGHT 34
#define BUTTON_SIZE 24
#define BUTTON_POSY 8
#define BUTTON_OFFSET 30

struct wl_display;
struct wl_registry;
struct wl_proxy;
struct wl_interface;
struct wl_seat;
struct wl_compositor;
struct wl_subcompositor;
struct wl_shm;
struct wl_buffer;
struct wl_surface;
struct wl_subsurface;
struct xdg_toplevel;

typedef struct {
    int fd;

    // cache mouse position
    int mouseX;
    int mouseY;

    struct wl_buffer* buffer;
    struct wl_surface* surface;
    struct wl_subsurface* subsurface;
    uint32_t* pixels;
    PalEventDriver* driver; // a pointer to the event driver
    uint64_t size;
} WaylandDecoration;

static struct wl_display* s_Display = nullptr;
static struct wl_registry* s_Registry = nullptr;
static struct wl_compositor* s_Compositor = nullptr;
static struct wl_subcompositor* s_Subcompositor = nullptr;
static struct wl_shm* s_Shm = nullptr;
static struct wl_surface* s_Surface = nullptr;
static struct wl_seat* s_Seat = nullptr;

static WaylandDecoration s_Decoration = {0};

typedef struct wl_display* (*wl_display_connect_fn)(const char*);
typedef void (*wl_display_disconnect_fn)(struct wl_display*);
typedef uint32_t (*wl_proxy_get_version_fn)(struct wl_proxy*);
typedef int (*wl_display_roundtrip_fn)(struct wl_display*);

typedef struct wl_proxy* (*wl_proxy_marshal_flags_fn)(
    struct wl_proxy*,
    uint32_t,
    const struct wl_interface*,
    uint32_t,
    uint32_t,
    ...);

typedef int (*wl_proxy_add_listener_fn)(
    struct wl_proxy*,
    void (**)(void),
    void*);

typedef void (*wl_proxy_destroy_fn)(struct wl_proxy*);

struct wl_interface {
    const char* name;
    int version;
    int method_count;
    const struct wl_message* methods;
    int event_count;
    const struct wl_message* events;
};

struct wl_registry_listener {
    void (*global)(
        void*,
        struct wl_registry*,
        uint32_t,
        const char*,
        uint32_t);

    void (*global_remove)(
        void*,
        struct wl_registry*,
        uint32_t);
};

static void* s_LibWayland;
static wl_display_connect_fn s_wl_display_connect;
static wl_display_disconnect_fn s_wl_display_disconnect;
static wl_display_roundtrip_fn s_wl_display_roundtrip;
static wl_proxy_get_version_fn s_wl_proxy_get_version;
static wl_proxy_marshal_flags_fn s_wl_proxy_marshal_flags;
static wl_proxy_add_listener_fn s_wl_proxy_add_listener;
static wl_proxy_destroy_fn s_wl_proxy_destroy;

static const struct wl_interface* registryInterface;
static const struct wl_interface* seatInterface;
static const struct wl_interface* compositorInterface;
static const struct wl_interface* subCompositorInterface;
static const struct wl_interface* shmInterface;
static const struct wl_interface* shmPoolInterface;
static const struct wl_interface* surfaceInterface;
static const struct wl_interface* bufferInterface;
static const struct wl_interface* subsurfaceInterface;

static inline void* wlRegistryBind(
    struct wl_registry* wl_registry,
    uint32_t name,
    const struct wl_interface* interface,
    uint32_t version)
{
    struct wl_proxy* id;
    id = s_wl_proxy_marshal_flags(
        (struct wl_proxy*)wl_registry,
        0, // WL_REGISTRY_BIND
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
    return s_wl_proxy_add_listener((struct wl_proxy*)wl_registry, (void (**)(void))listener, data);
}

static inline struct wl_registry* wlDisplayGetRegistry(struct wl_display* wl_display)
{
    struct wl_proxy* registry;
    registry = s_wl_proxy_marshal_flags(
        (struct wl_proxy*)wl_display,
        1, // WL_DISPLAY_GET_REGISTRY
        registryInterface,
        s_wl_proxy_get_version((struct wl_proxy*)wl_display),
        0,
        NULL);

    return (struct wl_registry*)registry;
}

static inline struct wl_surface* wlCompositorCreateSurface(struct wl_compositor* wl_compositor)
{
    struct wl_proxy* id;
    id = s_wl_proxy_marshal_flags(
        (struct wl_proxy*)wl_compositor,
        0, // WL_COMPOSITOR_CREATE_SURFACE,
        surfaceInterface,
        s_wl_proxy_get_version((struct wl_proxy*)wl_compositor),
        0,
        NULL);

    return (struct wl_surface*)id;
}

static inline void wlSurfaceCommit(struct wl_surface* wl_surface)
{
    s_wl_proxy_marshal_flags(
        (struct wl_proxy*)wl_surface,
        6, // WL_SURFACE_COMMIT
        NULL,
        s_wl_proxy_get_version((struct wl_proxy*)wl_surface),
        0);
}

static inline void wlSurfaceDestroy(struct wl_surface* wl_surface)
{
    s_wl_proxy_marshal_flags(
        (struct wl_proxy*)wl_surface,
        0, // WL_SURFACE_DESTROY
        NULL,
        s_wl_proxy_get_version((struct wl_proxy*)wl_surface),
        WL_MARSHAL_FLAG_DESTROY);
}

static inline struct wl_shm_pool* wlShmCreatePool(
    struct wl_shm* wl_shm,
    int32_t fd,
    int32_t size)
{
    struct wl_proxy* id;
    id = s_wl_proxy_marshal_flags(
        (struct wl_proxy*)wl_shm,
        0, // WL_SHM_CREATE_POOL
        shmPoolInterface,
        s_wl_proxy_get_version((struct wl_proxy*)wl_shm),
        0,
        NULL,
        fd,
        size);

    return (struct wl_shm_pool*)id;
}

static inline void wlShmPoolDestroy(struct wl_shm_pool* wl_shm_pool)
{
    s_wl_proxy_marshal_flags(
        (struct wl_proxy*)wl_shm_pool,
        1,
        NULL,
        s_wl_proxy_get_version((struct wl_proxy*)wl_shm_pool),
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
    id = s_wl_proxy_marshal_flags(
        (struct wl_proxy*)wl_shm_pool,
        0,
        bufferInterface,
        s_wl_proxy_get_version((struct wl_proxy*)wl_shm_pool),
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
    s_wl_proxy_marshal_flags(
        (struct wl_proxy*)wl_buffer,
        0,
        NULL,
        s_wl_proxy_get_version((struct wl_proxy*)wl_buffer),
        WL_MARSHAL_FLAG_DESTROY);
}

static inline void wlSurfaceAttach(
    struct wl_surface* wl_surface,
    struct wl_buffer* buffer,
    int32_t x,
    int32_t y)
{
    s_wl_proxy_marshal_flags(
        (struct wl_proxy*)wl_surface,
        1,
        NULL,
        s_wl_proxy_get_version((struct wl_proxy*)wl_surface),
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
    s_wl_proxy_marshal_flags(
        (struct wl_proxy*)wl_surface,
        9,
        NULL,
        s_wl_proxy_get_version((struct wl_proxy*)wl_surface),
        0,
        x,
        y,
        width,
        height);
}

static inline void subcompositorDestroy(struct wl_subcompositor* wl_subcompositor)
{
    s_wl_proxy_marshal_flags(
        (struct wl_proxy*)wl_subcompositor,
        0,
        NULL,
        s_wl_proxy_get_version((struct wl_proxy*)wl_subcompositor),
        WL_MARSHAL_FLAG_DESTROY);
}

static inline struct wl_subsurface* subcompositorGetSubsurface(
    struct wl_subcompositor* wl_subcompositor,
    struct wl_surface* surface,
    struct wl_surface* parent)
{
    struct wl_proxy* id;
    id = s_wl_proxy_marshal_flags(
        (struct wl_proxy*)wl_subcompositor,
        1,
        subsurfaceInterface,
        s_wl_proxy_get_version((struct wl_proxy*)wl_subcompositor),
        0,
        NULL,
        surface,
        parent);

    return (struct wl_subsurface*)id;
}

static inline void subsurfaceDestroy(struct wl_subsurface* wl_subsurface)
{
    s_wl_proxy_marshal_flags(
        (struct wl_proxy*)wl_subsurface,
        0,
        NULL,
        s_wl_proxy_get_version((struct wl_proxy*)wl_subsurface),
        WL_MARSHAL_FLAG_DESTROY);
}

static inline void subsurfaceSetPosition(
    struct wl_subsurface* wl_subsurface,
    int32_t x,
    int32_t y)
{
    s_wl_proxy_marshal_flags(
        (struct wl_proxy*)wl_subsurface,
        1,
        NULL,
        s_wl_proxy_get_version((struct wl_proxy*)wl_subsurface),
        0,
        x,
        y);
}

static inline void subsurfaceSetDesync(struct wl_subsurface* wl_subsurface)
{
    s_wl_proxy_marshal_flags(
        (struct wl_proxy*)wl_subsurface,
        5,
        NULL,
        s_wl_proxy_get_version((struct wl_proxy*)wl_subsurface),
        0);
}

static void globalHandle(
    void* data,
    struct wl_registry* registry,
    uint32_t name,
    const char* interface,
    uint32_t version)
{
    if (strcmp(interface, "wl_seat") == 0) {
        s_Seat = wlRegistryBind(registry, name, seatInterface, 5);

    } else if (strcmp(interface, "wl_compositor") == 0) {
        s_Compositor = wlRegistryBind(registry, name, compositorInterface, 4);

    } else if (strcmp(interface, "wl_subcompositor") == 0) {
        s_Subcompositor = wlRegistryBind(registry, name, subCompositorInterface, 1);

    } else if (strcmp(interface, "wl_shm") == 0) {
        s_Shm = wlRegistryBind(registry, name, shmInterface, 1);
    }
}

static void globalRemove(
    void* data,
    struct wl_registry* registry,
    uint32_t name)
{
}

static const struct wl_registry_listener s_RegistryListener = {
    .global = globalHandle,
    .global_remove = globalRemove};

// xdg-shell protocol
static inline void xdgToplevelMove(
    struct xdg_toplevel* xdg_toplevel,
    struct wl_seat* seat,
    uint32_t serial)
{
    s_wl_proxy_marshal_flags(
        (struct wl_proxy*)xdg_toplevel,
        5,
        NULL,
        s_wl_proxy_get_version((struct wl_proxy*)xdg_toplevel),
        0,
        seat,
        serial);
}

static inline void xdgToplevelResize(
    struct xdg_toplevel* xdg_toplevel,
    struct wl_seat* seat,
    uint32_t serial,
    uint32_t edges)
{
    s_wl_proxy_marshal_flags(
        (struct wl_proxy*)xdg_toplevel,
        6,
        NULL,
        s_wl_proxy_get_version((struct wl_proxy*)xdg_toplevel),
        0,
        seat,
        serial,
        edges);
}

static void openDisplayWayland()
{
    const char* session = getenv("XDG_SESSION_TYPE");
    if (session) {
        if (strcmp(session, "x11") == 0) {
            return;
        }
    }

    s_LibWayland = dlopen("libwayland-client.so.0", RTLD_LAZY);
    if (!s_LibWayland) {
        return;
    }

    // clang-format off
    s_wl_display_connect = (wl_display_connect_fn)dlsym(
        s_LibWayland,
        "wl_display_connect");

    s_wl_display_disconnect = (wl_display_disconnect_fn)dlsym(
        s_LibWayland,
        "wl_display_disconnect");

    s_wl_display_roundtrip = (wl_display_roundtrip_fn)dlsym(
        s_LibWayland,
        "wl_display_roundtrip");

    s_wl_proxy_marshal_flags = (wl_proxy_marshal_flags_fn)dlsym(
        s_LibWayland,
        "wl_proxy_marshal_flags");

    s_wl_proxy_get_version = (wl_proxy_get_version_fn)dlsym(
        s_LibWayland,
        "wl_proxy_get_version");

    s_wl_proxy_add_listener = (wl_proxy_add_listener_fn)dlsym(
        s_LibWayland,
        "wl_proxy_add_listener");

    s_wl_proxy_destroy = (wl_proxy_destroy_fn)dlsym(
        s_LibWayland,
        "wl_proxy_destroy");
    // clang-format on

    registryInterface = dlsym(s_LibWayland, "wl_registry_interface");
    seatInterface = dlsym(s_LibWayland, "wl_seat_interface");
    compositorInterface = dlsym(s_LibWayland, "wl_compositor_interface");
    subCompositorInterface = dlsym(s_LibWayland, "wl_subcompositor_interface");
    shmInterface = dlsym(s_LibWayland, "wl_shm_interface");
    shmPoolInterface = dlsym(s_LibWayland, "wl_shm_pool_interface");
    surfaceInterface = dlsym(s_LibWayland, "wl_surface_interface");
    bufferInterface = dlsym(s_LibWayland, "wl_buffer_interface");
    subsurfaceInterface = dlsym(s_LibWayland, "wl_subsurface_interface");

    struct wl_display* display = s_wl_display_connect(nullptr);
    if (display) {
        struct wl_registry* registry = wlDisplayGetRegistry(display);
        wlRegistryAddListener(registry, &s_RegistryListener, nullptr);
        s_wl_display_roundtrip(display);
        s_Display = display;
    }

    if (!s_Compositor || !s_Subcompositor || !s_Shm || !s_Seat) {
        palLog(nullptr, "Failed to get globals");
        return;
    }
}

static void closeDisplayWayland()
{
    if (s_Compositor) {
        s_wl_proxy_destroy((struct wl_proxy*)s_Compositor);
        s_wl_proxy_destroy((struct wl_proxy*)s_Shm);
        s_wl_proxy_destroy((struct wl_proxy*)s_Seat);
        subcompositorDestroy(s_Subcompositor);
    }

    s_wl_display_disconnect((struct wl_display*)s_Display);
    dlclose(s_LibWayland);
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
    int* outFd,
    uint64_t* outSize,
    uint32_t** outPixels)
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

    pool = wlShmCreatePool(s_Shm, fd, size);
    if (!pool) {
        return nullptr;
    }

    buffer = wlShmPoolCreateBuffer(pool, 0, width, height, stride, 1);
    if (!buffer) {
        return nullptr;
    }

    wlShmPoolDestroy(pool);

    *outPixels = (uint32_t*)data;
    *outFd = fd;
    *outSize = size;
    return buffer;
}

void fillRect(
    uint32_t* pixels,
    int stride,
    int x,
    int y,
    int w,
    int h,
    uint32_t color)
{
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            pixels[(y + j) * stride + (x + i)] = color;
        }
    }
}

// a simple 8x8 bitmap font
// exchange with your font
// Each byte = one row of 8 pixels, MSB = leftmost pixel
static const uint8_t s_FontBasic[128][8] = {
    ['A'] = {0x18, 0x24, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42},
    ['B'] = {0x7C, 0x42, 0x42, 0x7C, 0x42, 0x42, 0x42, 0x7C},
    ['C'] = {0x3C, 0x42, 0x40, 0x40, 0x40, 0x40, 0x42, 0x3C},
    ['D'] = {0x78, 0x44, 0x42, 0x42, 0x42, 0x42, 0x44, 0x78},
    ['E'] = {0x7E, 0x40, 0x40, 0x7C, 0x40, 0x40, 0x40, 0x7E},
    ['F'] = {0x7E, 0x40, 0x40, 0x7C, 0x40, 0x40, 0x40, 0x40},
    ['G'] = {0x3C, 0x42, 0x40, 0x40, 0x4E, 0x42, 0x42, 0x3C},
    ['H'] = {0x42, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x42},
    ['I'] = {0x3E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3E},
    ['J'] = {0x1E, 0x04, 0x04, 0x04, 0x04, 0x44, 0x44, 0x38},
    ['K'] = {0x42, 0x44, 0x48, 0x70, 0x48, 0x44, 0x42, 0x42},
    ['L'] = {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7E},
    ['M'] = {0x42, 0x66, 0x5A, 0x5A, 0x42, 0x42, 0x42, 0x42},
    ['N'] = {0x42, 0x62, 0x52, 0x4A, 0x46, 0x42, 0x42, 0x42},
    ['O'] = {0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C},
    ['P'] = {0x7C, 0x42, 0x42, 0x7C, 0x40, 0x40, 0x40, 0x40},
    ['Q'] = {0x3C, 0x42, 0x42, 0x42, 0x42, 0x4A, 0x44, 0x3A},
    ['R'] = {0x7C, 0x42, 0x42, 0x7C, 0x48, 0x44, 0x42, 0x42},
    ['S'] = {0x3C, 0x42, 0x40, 0x3C, 0x02, 0x02, 0x42, 0x3C},
    ['T'] = {0x7F, 0x49, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08},
    ['U'] = {0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C},
    ['V'] = {0x42, 0x42, 0x42, 0x42, 0x42, 0x24, 0x24, 0x18},
    ['W'] = {0x42, 0x42, 0x42, 0x5A, 0x5A, 0x5A, 0x66, 0x42},
    ['X'] = {0x42, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x42},
    ['Y'] = {0x42, 0x42, 0x24, 0x18, 0x08, 0x08, 0x08, 0x08},
    ['Z'] = {0x7E, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x7E},

    ['a'] = {0x00, 0x00, 0x3C, 0x02, 0x3E, 0x42, 0x46, 0x3A},
    ['b'] = {0x40, 0x40, 0x5C, 0x62, 0x42, 0x42, 0x62, 0x5C},
    ['c'] = {0x00, 0x00, 0x3C, 0x42, 0x40, 0x40, 0x42, 0x3C},
    ['d'] = {0x02, 0x02, 0x3A, 0x46, 0x42, 0x42, 0x46, 0x3A},
    ['e'] = {0x00, 0x00, 0x3C, 0x42, 0x7E, 0x40, 0x42, 0x3C},
    ['f'] = {0x0C, 0x12, 0x10, 0x3C, 0x10, 0x10, 0x10, 0x10},
    ['g'] = {0x00, 0x00, 0x3A, 0x46, 0x46, 0x3E, 0x02, 0x3C},
    ['h'] = {0x40, 0x40, 0x5C, 0x62, 0x42, 0x42, 0x42, 0x42},
    ['i'] = {0x08, 0x00, 0x18, 0x08, 0x08, 0x08, 0x08, 0x1C},
    ['j'] = {0x04, 0x00, 0x0C, 0x04, 0x04, 0x04, 0x44, 0x38},
    ['k'] = {0x40, 0x40, 0x44, 0x48, 0x70, 0x48, 0x44, 0x42},
    ['l'] = {0x18, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1C},
    ['m'] = {0x00, 0x00, 0x6C, 0x52, 0x52, 0x42, 0x42, 0x42},
    ['n'] = {0x00, 0x00, 0x5C, 0x62, 0x42, 0x42, 0x42, 0x42},
    ['o'] = {0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x3C},
    ['p'] = {0x00, 0x00, 0x5C, 0x62, 0x42, 0x62, 0x40, 0x40},
    ['q'] = {0x00, 0x00, 0x3A, 0x46, 0x42, 0x46, 0x02, 0x02},
    ['r'] = {0x00, 0x00, 0x5C, 0x62, 0x40, 0x40, 0x40, 0x40},
    ['s'] = {0x00, 0x00, 0x3E, 0x40, 0x3C, 0x02, 0x42, 0x3C},
    ['t'] = {0x10, 0x10, 0x3C, 0x10, 0x10, 0x12, 0x0C, 0x00},
    ['u'] = {0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x46, 0x3A},
    ['v'] = {0x00, 0x00, 0x42, 0x42, 0x42, 0x24, 0x24, 0x18},
    ['w'] = {0x00, 0x00, 0x42, 0x42, 0x42, 0x52, 0x52, 0x2C},
    ['x'] = {0x00, 0x00, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42},
    ['y'] = {0x00, 0x00, 0x42, 0x42, 0x46, 0x3A, 0x02, 0x3C},
    ['z'] = {0x00, 0x00, 0x7E, 0x04, 0x08, 0x10, 0x20, 0x7E},
};

void drawCharacter(
    uint32_t* pixels,
    int stride,
    int x,
    int y,
    char c,
    uint32_t color)
{
    if (c < 0 || c > 127)
        return; // not in out font

    const uint8_t* bitmap = s_FontBasic[(int)c];
    for (int row = 0; row < 8; row++) {
        uint8_t bits = bitmap[row];
        for (int col = 0; col < 8; col++) {
            if (bits & (1 << (7 - col))) {
                int px = x + col;
                int py = y + row;
                if (px < stride && py < TITLEBAR_HEIGHT) {
                    pixels[py * stride + px] = color;
                }
            }
        }
    }
}

void drawText(
    uint32_t* pixels,
    int stride,
    int x,
    int y,
    const char* text,
    uint32_t color)
{
    while (*text) {
        drawCharacter(pixels, stride, x, y, *text, color);
        x += 8; // advance 8 pixels per character
        text++;
    }
}

static PalWindowHandleInfo s_WinHandle;

static void createDecoration()
{
    s_Decoration.surface = wlCompositorCreateSurface(s_Compositor);
    if (!s_Decoration.surface) {
        palLog(nullptr, "Failed to create wayland surface");
        return;
    }

    s_Decoration.subsurface = subcompositorGetSubsurface(
        s_Subcompositor,
        s_Decoration.surface,
        (struct wl_surface*)s_WinHandle.nativeWindow);

    if (!s_Decoration.subsurface) {
        palLog(nullptr, "Failed to create wayland subsurface");
        return;
    }

    // make it soo that the decoration updates independently of the window
    subsurfaceSetDesync(s_Decoration.subsurface);
    subsurfaceSetPosition(s_Decoration.subsurface, 0, 0);

    // create decoration shm buffer
    int width = 640;
    s_Decoration.buffer = createShmBuffer(
        width, // might be different depending on compositor
        TITLEBAR_HEIGHT,
        &s_Decoration.fd,
        &s_Decoration.size,
        &s_Decoration.pixels);

    if (!s_Decoration.buffer) {
        palLog(nullptr, "Failed to create wayland buffer");
        return;
    }

    // write pixels
    // title bar color (dark grey)
    fillRect(s_Decoration.pixels, width, 0, 0, width, TITLEBAR_HEIGHT, 0x002F3030);

    // Close button
    // this is just a rectangle for this simple example
    // to show CSD works with PAL
    fillRect(
        s_Decoration.pixels,
        width,
        width - BUTTON_SIZE,
        BUTTON_POSY,
        BUTTON_SIZE,
        TITLEBAR_HEIGHT / 2, // half of the size of the title bar
        0x00AA3333);

    // Maximize button
    fillRect(
        s_Decoration.pixels,
        width,
        width - (BUTTON_OFFSET + BUTTON_SIZE),
        BUTTON_POSY,
        BUTTON_SIZE,
        TITLEBAR_HEIGHT / 2, // half of the size of the title bar
        0x0033AA33);

    // Minimize button
    fillRect(
        s_Decoration.pixels,
        width,
        width - (BUTTON_OFFSET * 2 + BUTTON_SIZE),
        BUTTON_POSY,
        BUTTON_SIZE,
        TITLEBAR_HEIGHT / 2, // half of the size of the title bar
        0x0033AAAA);

    wlSurfaceAttach(s_Decoration.surface, s_Decoration.buffer, 0, 0);
    wlSurfaceDamageBuffer(s_Decoration.surface, 0, 0, width, TITLEBAR_HEIGHT);
    wlSurfaceCommit(s_Decoration.surface);

    // wayland requires the main surface to be committed as well
    wlSurfaceCommit(s_WinHandle.nativeWindow);

    // draw window title
    // this example does not support unicode characters
    int textWidth = strlen(WINDOW_TITLE) * 8; // 8x8 font
    int x = (width - textWidth) / 2;
    int y = (TITLEBAR_HEIGHT - 8) / 2;

    drawText(s_Decoration.pixels, width, x, y, WINDOW_TITLE, 0x00FFFFFF);

    // set opaque regions for optimazation
    // we wont do this in this example
    // this does not include any hover effects and any shadows
    // and any fancy stuff
}

static void destroyDecoration()
{
    subsurfaceDestroy(s_Decoration.subsurface);
    wlSurfaceDestroy(s_Decoration.surface);
    wlBufferDestroy(s_Decoration.buffer);
    munmap((void*)s_Decoration.pixels, s_Decoration.size);
    close(s_Decoration.fd);
}

static void PAL_CALL onEvent(
    void* userData,
    const PalEvent* event)
{
    if (event->type == PAL_EVENT_TYPE_MOUSE_BUTTONDOWN) {
        uint32_t button, serial;
        palUnpackUint32(event->data, &button, &serial);

        if (button == PAL_MOUSE_BUTTON_LEFT) {
            // check if the mouse is on the title bar
            // and move the window
            // use the cache mouse position to check if the mouse
            // is in the title bar
            int x = s_Decoration.mouseX;
            int y = s_Decoration.mouseY;
            // width should reflect window width
            if (x >= 0 && x < 640 && y >= 0 && y < TITLEBAR_HEIGHT) {
                xdgToplevelMove(s_WinHandle.nativeHandle2, s_Seat, serial);

                // Optionally check for another click and maximize the window
                // maybe set a PalBool or query mouse button state
                // we will skip it for this example
            }

            // we skip maximize and minimize button for simplicity
            // we just deal with the close button
            int buttonX = 640 - BUTTON_SIZE;
            if (x >= buttonX && x < buttonX + BUTTON_SIZE && y >= BUTTON_POSY &&
                y < BUTTON_POSY + BUTTON_SIZE) {
                // inside close button
                // trigger a window close event

                PalEvent event = {0};
                event.data2 = palPackPointer(s_WinHandle.nativeWindow);
                event.type = PAL_EVENT_TYPE_WINDOW_CLOSE;
                palPushEvent(s_Decoration.driver, &event);
            }
        }

    } else if (event->type == PAL_EVENT_TYPE_MOUSE_MOVE) {
        int32_t x, y;
        palUnpackInt32(event->data, &x, &y);
        s_Decoration.mouseX = x;
        s_Decoration.mouseY = y;

    } else if (event->type == PAL_EVENT_TYPE_MONITOR_DPI_CHANGED) {
        palLog(nullptr, "Monitor DPI: %d", event->data);
    }
}

#else
#include "pal/pal_core.h"
#endif // __linux__

PalBool customDecorationTest()
{
#ifdef __linux__
    openDisplayWayland();
    if (!s_Display) {
        // not on wayland
        palLog(nullptr, "Not on wayland platform");
        return PAL_FALSE;
    }

    palLog(nullptr, "Press Escape or click close button to close Test");
    palLog(nullptr, "This only implements close and window movement for simplicity");

    // fill the event driver create info
    PalEventDriverCreateInfo eventDriverCreateInfo = {0};
    eventDriverCreateInfo.allocator = nullptr; // default allocator
    eventDriverCreateInfo.callback = onEvent;  // no callback dispatch
    eventDriverCreateInfo.queue = nullptr;     // default queue
    eventDriverCreateInfo.userData = nullptr;  // null

    // create the event driver
    PalEventDriver* eventDriver = nullptr;
    PalResult result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create event driver");
        return PAL_FALSE;
    }

    palSetEventDispatchMode(eventDriver, PAL_EVENT_TYPE_WINDOW_CLOSE, PAL_DISPATCH_MODE_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_TYPE_WINDOW_DECORATION_MODE, PAL_DISPATCH_MODE_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_TYPE_KEYDOWN, PAL_DISPATCH_MODE_POLL);

    // we use PAL_DISPATCH_MODE_CALLBACK for the mouse button to get
    // real time events which we then use for moving and resizing
    palSetEventDispatchMode(eventDriver, PAL_EVENT_TYPE_MOUSE_BUTTONDOWN, PAL_DISPATCH_MODE_CALLBACK);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_TYPE_MOUSE_MOVE, PAL_DISPATCH_MODE_CALLBACK);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_TYPE_MONITOR_DPI_CHANGED, PAL_DISPATCH_MODE_CALLBACK);

    // initialize the video system. We pass the event driver to recieve video
    // related events the video system does not copy the event driver, it must
    // be valid till the video system is shutdown
    // tell the video system to use out instance rather than creating a new one
    result = palInitVideo(nullptr, eventDriver, (void*)s_Display);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to initialize video");
        return PAL_FALSE;
    }

    PalWindow* window = nullptr;
    PalWindowCreateInfo createInfo = {0};
    createInfo.height = 480;
    createInfo.width = 640;
    createInfo.show = PAL_TRUE;
    createInfo.style = PAL_WINDOW_STYLE_BORDERLESS;
    createInfo.title = WINDOW_TITLE;

    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create window");
        return PAL_FALSE;
    }

    // get native handles
    result = palGetWindowHandleInfo(window, &s_WinHandle);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to get window handle info");
        return PAL_FALSE;
    }

    s_Decoration.driver = eventDriver;
    createDecoration();

    PalBool running = PAL_TRUE;
    while (running) {
        // update the video system to push video events
        palUpdateVideo();

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_TYPE_WINDOW_CLOSE: {
                    running = PAL_FALSE;
                    break;
                }

                case PAL_EVENT_TYPE_KEYDOWN: {
                    PalKeycode keycode = 0;
                    palUnpackUint32(event.data, &keycode, nullptr);
                    if (keycode == PAL_KEYCODE_ESCAPE) {
                        running = PAL_FALSE;
                    }
                    break;
                }

                case PAL_EVENT_TYPE_WINDOW_DECORATION_MODE: {
                    if (event.data == PAL_DECORATION_MODE_CLIENT_SIDE) {
                        palLog(nullptr, "Window Decoration Mode: Client Side");

                    } else {
                        palLog(nullptr, "Window Decoration Mode: Server Side");
                    }
                    break;
                }
            }
        }
    }

    // destroy decoration
    destroyDecoration();

    // destroy the window
    palDestroyWindow(window);

    // shutdown the video system
    palShutdownVideo();

    // destroy the event driver
    palDestroyEventDriver(eventDriver);

    closeDisplayWayland();

    return PAL_TRUE;

#else 
    palLog(nullptr, "Custom decoration not supported");
    return PAL_FALSE;
#endif // __linux__
}
