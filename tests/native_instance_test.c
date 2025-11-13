
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
typedef struct wl_display* (*wl_display_connect_fn)(const char*);
typedef void (*wl_display_disconnect_fn)(struct wl_display*);

static void* s_LibWayland;
static wl_display_connect_fn s_wl_display_connect;
static wl_display_disconnect_fn s_wl_display_disconnect;

static bool s_OnWayland = false;

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

    return s_XOpenDisplay(nullptr);
#endif // __linux__
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

    return s_wl_display_connect(nullptr);
#endif // __linux__
}

void closeDisplayWayland(void* instance)
{
#ifdef __linux__
    s_wl_display_disconnect((struct wl_display*)instance);
    dlclose(s_LibWayland);
#endif // __linux__
}

void* openInstance()
{
#ifdef _WIN32
#elif defined(__linux__)
    // get the active session
    const char* session = getenv("XDG_SESSION_TYPE");
    if (session) {
        if (strcmp(session, "wayland") == 0) {
            s_OnWayland = true;
        } else {
            s_OnWayland = false;
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
#elif defined(__linux__)
    if (s_OnWayland) {
        closeDisplayWayland(instance);
    } else {
        closeDisplayX11(instance);
    }
#endif // _WIN32
}

bool nativeInstanceTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Native Instance Test");
    palLog(nullptr, "Press Escape or click close button to close Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalResult result;

    // event driver
    PalEventDriver* eventDriver = nullptr;
    PalEventDriverCreateInfo eventDriverCreateInfo = {0};

    // create the event driver
    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create event driver: %s", error);
        return false;
    }

    // open our own display or instance
    void* instance = openInstance();
    if (!instance) {
        palLog(nullptr, "Failed to open instance");
        return false;
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
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize video: %s", error);
        return false;
    }

    PalWindow* window = nullptr;
    PalWindowCreateInfo createInfo = {0};
    createInfo.monitor = nullptr; // use default monitor
    createInfo.height = 480;
    createInfo.width = 640;
    createInfo.show = true;
    createInfo.style = PAL_WINDOW_STYLE_RESIZABLE;
    createInfo.title = "Native Instance Test";
  
    // check if we support decorated windows (title bar, close etc)
    PalVideoFeatures64 features = palGetVideoFeaturesEx();
    if (!(features & PAL_VIDEO_FEATURE64_DECORATED_WINDOW)) {
        // if we dont support, we need to create a borderless window
        // and create the decorations ourselves
        createInfo.style |= PAL_WINDOW_STYLE_BORDERLESS;
    }

    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create window: %s", error);
        return false;
    }

    // we set window close to poll
    palSetEventDispatchMode(
        eventDriver,
        PAL_EVENT_WINDOW_CLOSE,
        PAL_DISPATCH_POLL);

    palSetEventDispatchMode(
        eventDriver,
        PAL_EVENT_KEYDOWN,
        PAL_DISPATCH_POLL);

    bool running = true;
    while (running) {
        // update the video system to push video events
        palUpdateVideo();

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_WINDOW_CLOSE: {
                    running = false;
                    break;
                }

                case PAL_EVENT_KEYDOWN: {
                    PalKeycode keycode = 0;
                    palUnpackUint32(event.data, &keycode, nullptr);
                    if (keycode == PAL_KEYCODE_ESCAPE) {
                        running = false;
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

    return true;
}