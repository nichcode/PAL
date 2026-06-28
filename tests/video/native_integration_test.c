
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
#include <X11/Xutil.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

// X11 typedefs
typedef Atom (*XInternAtomFn)(
    Display*,
    _Xconst char*,
    Bool);

typedef int (*XChangePropertyFn)(
    Display*,
    Window,
    Atom,
    Atom,
    int,
    int,
    _Xconst unsigned char*,
    int);

typedef Status (*XGetWMNameFn)(
    Display*,
    Window,
    XTextProperty*);

typedef int (*XStoreNameFn)(
    Display*,
    Window,
    _Xconst char*);

typedef int (*XGetWindowPropertyFn)(
    Display*,
    Window,
    Atom,
    long,
    long,
    Bool,
    Atom,
    Atom*,
    int*,
    unsigned long*,
    unsigned long*,
    unsigned char**);

typedef int (*XFlushFn)(Display*);
typedef int (*XFreeFn)(void*);

// Wayland typedefs
struct wl_display;
struct wl_interface;
struct xdg_toplevel;

typedef struct wl_proxy* (*wl_proxy_marshal_flags_fn)(
    struct wl_proxy*,
    uint32_t,
    const struct wl_interface*,
    uint32_t,
    uint32_t,
    ...);

typedef uint32_t (*wl_proxy_get_version_fn)(struct wl_proxy*);
typedef int (*wl_display_flush_fn)(struct wl_display*);

static wl_proxy_marshal_flags_fn s_wl_proxy_marshal_flags;
static wl_proxy_get_version_fn s_wl_proxy_get_version;
static wl_display_flush_fn s_wl_display_flush;

static inline void xdgToplevelSetTitle(
    struct xdg_toplevel* xdg_toplevel,
    const char* title)
{
    s_wl_proxy_marshal_flags(
        (struct wl_proxy*)xdg_toplevel,
        2, // XDG_TOPLEVEL_SET_TITLE
        NULL,
        s_wl_proxy_get_version((struct wl_proxy*)xdg_toplevel),
        0,
        title);
}

static XInternAtomFn s_XInternAtom;
static XChangePropertyFn s_XChangeProperty;
static XGetWMNameFn s_XGetWMName;
static XStoreNameFn s_XStoreName;
static XGetWindowPropertyFn s_XGetWindowProperty;
static XFlushFn s_XFlush;
static XFreeFn s_XFree;

static Atom s_NET_WM_NAME;
static Atom s_UTF8_STRING;

static PalBool s_OnWayland = PAL_FALSE;
static void* s_X11Lib;

static void* s_WaylandLib;

#endif // _WIN32

static char s_TitleBuffer[32];

void setWindowTitleX11(PalWindowHandleInfo* windowInfo)
{
#ifdef __linux__
    // load the procs
    s_X11Lib = dlopen("libX11.so", RTLD_LAZY);
    if (!s_X11Lib) {
        return;
    }

    // clang-format off
    s_XInternAtom = (XInternAtomFn)dlsym(
        s_X11Lib,
        "XInternAtom");

    s_XChangeProperty = (XChangePropertyFn)dlsym(
        s_X11Lib,
        "XChangeProperty");

    s_XGetWMName = (XGetWMNameFn)dlsym(
        s_X11Lib,
        "XGetWMName");

    s_XStoreName = (XStoreNameFn)dlsym(
        s_X11Lib,
        "XStoreName");

    s_XGetWindowProperty = (XGetWindowPropertyFn)dlsym(
        s_X11Lib,
        "XGetWindowProperty");

    s_XFlush = (XFlushFn)dlsym(
        s_X11Lib,
        "XFlush");

    s_XFree = (XFreeFn)dlsym(
        s_X11Lib,
        "XFree");
    // clang-format on

    Display* display = (Display*)windowInfo->nativeInstance;
    Window window = (Window)(uintptr_t)windowInfo->nativeWindow;

    s_NET_WM_NAME = s_XInternAtom(display, "_NET_WM_NAME", False);
    s_UTF8_STRING = s_XInternAtom(display, "UTF8_STRING", False);

    const char* title = "Hello from native X11 API";
    if (s_NET_WM_NAME) {
        s_XChangeProperty(
            display,
            window,
            s_NET_WM_NAME,
            s_UTF8_STRING,
            8, // unsigned char
            PropModeReplace,
            title,
            strlen(title));

    } else {
        s_XStoreName(display, window, title);
    }

    s_XFlush(display);
#endif // __linux__
}

void getWindowTitleX11(PalWindowHandleInfo* windowInfo)
{
#ifdef __linux__
    Display* display = (Display*)windowInfo->nativeInstance;
    Window window = (Window)(uintptr_t)windowInfo->nativeWindow;

    if (s_NET_WM_NAME) {
        Atom type;
        int format;
        unsigned long count, bytesAfter;
        unsigned char* prop = nullptr;
        s_XGetWindowProperty(
            display,
            window,
            s_NET_WM_NAME,
            0,
            (~0L),
            False,
            s_UTF8_STRING,
            &type,
            &format,
            &count,
            &bytesAfter,
            &prop);

        strcpy(s_TitleBuffer, (const char*)prop);
        s_XFree(prop);

    } else {
        XTextProperty text;
        s_XGetWMName(display, window, &text);
        strcpy(s_TitleBuffer, (const char*)text.value);
        s_XFree(text.value);
    }

    // free Xlib since we loaded dynamically
    dlclose(s_X11Lib);

#endif // __linux__
}

void setWindowTitleWayland(PalWindowHandleInfo* windowInfo)
{
#ifdef __linux__
    s_WaylandLib = dlopen("libwayland-client.so.0", RTLD_LAZY);
    if (!s_WaylandLib) {
        return;
    }

    s_wl_proxy_marshal_flags =
        (wl_proxy_marshal_flags_fn)dlsym(s_WaylandLib, "wl_proxy_marshal_flags");

    s_wl_proxy_get_version = (wl_proxy_get_version_fn)dlsym(s_WaylandLib, "wl_proxy_get_version");
    s_wl_display_flush = (wl_display_flush_fn)dlsym(s_WaylandLib, "wl_display_flush");

    struct xdg_toplevel* toplevel = nullptr;
    struct wl_display* display = nullptr;
    display = (struct wl_display*)windowInfo->nativeInstance;
    toplevel = (struct xdg_toplevel*)windowInfo->nativeHandle2;

    xdgToplevelSetTitle(toplevel, "Hello from native Wayland API");
    s_wl_display_flush(display);

#endif // __linux__
}

void getWindowTitleWayland(PalWindowHandleInfo* windowInfo)
{
#ifdef __linux__
    // wayland does not support getting window title
    // so we just return the title we set through wayland
    dlclose(s_WaylandLib);
#endif // __linux__
}

void setWindowTitleWin32(PalWindowHandleInfo* windowInfo)
{
#ifdef _WIN32
    const char* title = "Hello from native Win32 API";
    SetWindowTextA((HWND)windowInfo->nativeWindow, title);
#endif // _WIN32
}

void getWindowTitleWin32(PalWindowHandleInfo* windowInfo)
{
#ifdef _WIN32
    GetWindowTextA((HWND)windowInfo->nativeWindow, s_TitleBuffer, sizeof(s_TitleBuffer));
#endif // _WIN32
}

void setWindowTitle(PalWindowHandleInfo* windowInfo)
{
#ifdef _WIN32
    setWindowTitleWin32(windowInfo);
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
        setWindowTitleWayland(windowInfo);
    } else {
        setWindowTitleX11(windowInfo);
    }
#endif // _WIN32
}

void getWindowTitle(PalWindowHandleInfo* windowInfo)
{
#ifdef _WIN32
    getWindowTitleWin32(windowInfo);
#elif defined(__linux__)
    if (s_OnWayland) {
        getWindowTitleWayland(windowInfo);
    } else {
        getWindowTitleX11(windowInfo);
    }
#endif // _WIN32
}

PalBool nativeIntegrationTest()
{
    palLog(nullptr, "Press Escape or click close button to close Test");

    // fill the event driver create info
    PalEventDriverCreateInfo eventDriverCreateInfo = {0};
    eventDriverCreateInfo.allocator = nullptr; // default allocator
    eventDriverCreateInfo.callback = nullptr;  // no callback dispatch
    eventDriverCreateInfo.queue = nullptr;     // default queue
    eventDriverCreateInfo.userData = nullptr;  // null

    // create the event driver
    PalEventDriver* eventDriver = nullptr;
    PalResult result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create event driver");
        return PAL_FALSE;
    }

    // initialize the video system. We pass the event driver to recieve video
    // related events the video system does not copy the event driver, it must
    // be valid till the video system is shutdown
    result = palInitVideo(nullptr, eventDriver, nullptr);
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
    createInfo.title = "Native Integration Test";

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
    palSetEventDispatchMode(eventDriver, PAL_EVENT_TYPE_WINDOW_CLOSE, PAL_DISPATCH_MODE_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_TYPE_KEYDOWN, PAL_DISPATCH_MODE_POLL);

    // set the window title using native APIs
    PalWindowHandleInfo windowInfo = {0};
    result = palGetWindowHandleInfo(window, &windowInfo);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to get window handle info");
        return PAL_FALSE;
    }

    palLog(nullptr, "Window title: %s", createInfo.title);
    palLog(nullptr, "Setting window title with native API");
    setWindowTitle(&windowInfo);

    palLog(nullptr, "Getting window title with PAL API");
    palGetWindowTitle(window, sizeof(s_TitleBuffer), nullptr, s_TitleBuffer);
    palLog(nullptr, "Window title: %s", s_TitleBuffer);

    // set the title with PAL and retreive it with the native API
    palLog(nullptr, "Setting window title with PAL API");
    palSetWindowTitle(window, "Hello from PAL API");

    palLog(nullptr, "Getting window title with native API");
    getWindowTitle(&windowInfo);
    palLog(nullptr, "Window title: %s", s_TitleBuffer);

    // using native API like wl_proxy_set_user_data, XContext and
    // SetWindowLongPtr(GWLP_USERDATA) can be used freely

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
            }
        }
    }

    // destroy the window
    palDestroyWindow(window);

    // shutdown the video system
    palShutdownVideo();

    // destroy the event driver
    palDestroyEventDriver(eventDriver);

    return PAL_TRUE;
}
