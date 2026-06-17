
#include "pal/pal_video.h"
#include "tests.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

// set unicode
#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include <windows.h>

#elif defined(__linux__)
#include <X11/Xlib.h>
#include <dlfcn.h>
#include <stdlib.h>

typedef Display* (*XOpenDisplayFn)(const char*);
typedef int (*XCloseDisplayFn)(Display*);

static void* s_LibX;
static XOpenDisplayFn s_XOpenDisplay;
static XCloseDisplayFn s_XCloseDisplay;

struct wl_display;
struct wl_registry;
struct wl_proxy;
struct wl_interface;

typedef struct wl_display* (*wl_display_connect_fn)(const char*);
typedef void (*wl_display_disconnect_fn)(struct wl_display*);
typedef uint32_t (*wl_proxy_get_version_fn)(struct wl_proxy*);
typedef int (*wl_display_roundtrip_fn)(struct wl_display*);
typedef void (*wl_proxy_destroy_fn)(struct wl_proxy*);

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
static struct wl_registry* s_Registry;

static const struct wl_interface* registryInterface;

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

static inline int registryAddListener(
    struct wl_registry* wl_registry,
    const struct wl_registry_listener* listener,
    void* data)
{
    return s_wl_proxy_add_listener((struct wl_proxy*)wl_registry, (void (**)(void))listener, data);
}

static inline struct wl_registry* displayGetRegistry(struct wl_display* wl_display)
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

static PalBool s_Logged = PAL_FALSE;
static void globalHandle(
    void* data,
    struct wl_registry* registry,
    uint32_t name,
    const char* interface,
    uint32_t version)
{
    if (!s_Logged) {
        palLog(nullptr, "Registry global handle working");
        s_Logged = PAL_TRUE;
    }
}

static void globalRemove(
    void* data,
    struct wl_registry* registry,
    uint32_t name)
{
    if (s_Logged) {
        palLog(nullptr, "Registry global remove working");
        s_Logged = PAL_FALSE;
    }
}

static const struct wl_registry_listener s_RegistryListener = {
    .global = globalHandle,
    .global_remove = globalRemove};

static PalBool s_OnWayland = PAL_FALSE;

#endif // _WIN32

void* openDisplayX11()
{
#ifdef __linux__
    // load the procs
    s_LibX = dlopen("libX11.so", RTLD_LAZY);
    if (!s_LibX) {
        return nullptr;
    }

    // clang-format off
    s_XOpenDisplay = (XOpenDisplayFn)dlsym(
        s_LibX,
        "XOpenDisplay");

    s_XCloseDisplay = (XCloseDisplayFn)dlsym(
        s_LibX,
        "XCloseDisplay");
    // clang-format on

    return s_XOpenDisplay(nullptr);
#endif // __linux__
    return nullptr;
}

void closeDisplayX11(void* instance)
{
#ifdef __linux__
    s_XCloseDisplay((Display*)instance);
    dlclose(s_LibX);
#endif // __linux__
}

void* openDisplayWayland()
{
#ifdef __linux__
    // load the procs
    s_LibWayland = dlopen("libwayland-client.so.0", RTLD_LAZY);
    if (!s_LibWayland) {
        return nullptr;
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
    struct wl_display* display = s_wl_display_connect(nullptr);
    if (display) {
        s_Registry = displayGetRegistry(display);
        registryAddListener(s_Registry, &s_RegistryListener, nullptr);
        s_wl_display_roundtrip(display);
    }

    return display;
#endif // __linux__
    return nullptr;
}

void closeDisplayWayland(void* instance)
{
#ifdef __linux__
    s_wl_proxy_destroy((struct wl_proxy*)s_Registry);
    s_wl_display_disconnect((struct wl_display*)instance);
    dlclose(s_LibWayland);
#endif // __linux__
}

void* openDisplayWin32()
{
#ifdef _WIN32
    return GetModuleHandleW(nullptr);
#endif // _WIN32
}

void closeDisplayWin32(void* instance)
{
    // this does nothing
}

void* openInstance()
{
#ifdef _WIN32
    return openDisplayWin32();
#elif defined(__linux__)
    // get the active session
    const char* session = getenv("XDG_SESSION_TYPE");
    if (session) {
        if (strcmp(session, "wayland") == 0) {
            s_OnWayland = PAL_TRUE;
        } else {
            s_OnWayland = PAL_FALSE;
        }
    }

    if (s_OnWayland) {
        return openDisplayWayland();
    } else {
        return openDisplayX11();
    }
#endif // _WIN32
}

void closeInstance(void* instance)
{
#ifdef _WIN32
    closeDisplayWin32(instance);
#elif defined(__linux__)
    if (s_OnWayland) {
        closeDisplayWayland(instance);
    } else {
        closeDisplayX11(instance);
    }
#endif // _WIN32
}

PalBool nativeInstanceTest()
{
    palLog(nullptr, "Press Escape or click close button to close Test");
    
    PalResult result;

    // event driver
    PalEventDriver* eventDriver = nullptr;
    PalEventDriverCreateInfo eventDriverCreateInfo = {0};

    // create the event driver
    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create event driver");
        return PAL_FALSE;
    }

    // open our own display or instance
    void* instance = openInstance();
    if (!instance) {
        palLog(nullptr, "Failed to open instance");
        return PAL_FALSE;
    }

    // tell the video system to use out instance rather
    // than creating a new one
    // this can be set to the opengl system as well
    palSetPreferredInstance(instance);

    // initialize the video system. We pass the event driver to recieve video
    // related events the video system does not copy the event driver, it must
    // be valid till the video system is shutdown
    result = palInitVideo(nullptr, eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to initialize video");
        return PAL_FALSE;
    }

    PalWindow* window = nullptr;
    PalWindowCreateInfo createInfo = {0};
    createInfo.monitor = nullptr; // use default monitor
    createInfo.height = 480;
    createInfo.width = 640;
    createInfo.show = PAL_TRUE;
    createInfo.style = PAL_WINDOW_STYLE_RESIZABLE;
    createInfo.title = "Native Instance Test";

    // check if we support decorated windows (title bar, close etc)
    PalVideoFeatures features = palGetVideoFeatures();
    if (!(features & PAL_VIDEO_FEATURE_DECORATED_WINDOW)) {
        // if we dont support, we need to create a borderless window
        // and create the decorations ourselves
        createInfo.style |= PAL_WINDOW_STYLE_BORDERLESS;
    }

    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create window");
        return PAL_FALSE;
    }

    // we set window close to poll
    palSetEventDispatchMode(eventDriver, PAL_EVENT_WINDOW_CLOSE, PAL_DISPATCH_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_KEYDOWN, PAL_DISPATCH_POLL);

    PalBool running = PAL_TRUE;
    while (running) {
        // update the video system to push video events
        palUpdateVideo();

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_WINDOW_CLOSE: {
                    running = PAL_FALSE;
                    break;
                }

                case PAL_EVENT_KEYDOWN: {
                    PalKeycode keycode = 0;
                    palUnpackUint32(event.data, &keycode, nullptr);
                    if (keycode == PAL_KEYCODE_ESCAPE) {
                        running = PAL_FALSE;
                    }
                    break;
                }
            }
        }
    }

    // destroy the window
    palDestroyWindow(window);

    // shutdown the video system
    palShutdownVideo();

    // destroy the event driver
    palDestroyEventDriver(eventDriver);

    // the video system does not destroy or close the handle provided
    closeInstance(instance);

    return PAL_TRUE;
}
