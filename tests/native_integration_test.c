
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
#include <string.h>
#include <stdlib.h>

// wayland is optional and might not be supported
// so we stick to typedefs

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

static XInternAtomFn s_XInternAtom;
static XChangePropertyFn s_XChangeProperty;
static XGetWMNameFn s_XGetWMName;
static XStoreNameFn s_XStoreName;
static XGetWindowPropertyFn s_XGetWindowProperty;
static XFlushFn s_XFlush;
static XFreeFn s_XFree;

static Atom s_NET_WM_NAME;
static Atom s_UTF8_STRING;

static bool s_OnWayland = false;
static void* s_X11Lib;

static void* s_WaylandLib;

#endif // _WIN32

static char s_TitleBuffer[32];

void setWindowTitleX11(PalWindowHandleInfoEx* windowInfo)
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

    Display* display = (Display*)windowInfo->nativeDisplay;
    Window window = (Window)(UintPtr)windowInfo->nativeWindow;

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

void getWindowTitleX11(PalWindowHandleInfoEx* windowInfo)
{
#ifdef __linux__
    Display* display = (Display*)windowInfo->nativeDisplay;
    Window window = (Window)(UintPtr)windowInfo->nativeWindow;

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

        strncpy(s_TitleBuffer, (const char*)prop, sizeof(s_TitleBuffer));
        s_XFree(prop);

    } else {
        XTextProperty text;
        s_XGetWMName(display, window, &text);
        strncpy(s_TitleBuffer, (const char*)text.value, sizeof(s_TitleBuffer));
        s_XFree(text.value);
    }

    // free Xlib since we loaded dynamically
    dlclose(s_X11Lib);

#endif // __linux__
}

void setWindowTitleWayland(PalWindowHandleInfoEx* windowInfo)
{
    #ifdef __linux__

    #endif // __linux__
}

void getWindowTitleWayland(PalWindowHandleInfoEx* windowInfo)
{
    #ifdef __linux__

    #endif // __linux__
}

void setWindowTitleWin32(PalWindowHandleInfoEx* windowInfo);
void getWindowTitleWin32(PalWindowHandleInfoEx* windowInfo);

void setWindowTitle(PalWindowHandleInfoEx* windowInfo)
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
        setWindowTitleWayland(windowInfo);
    } else {
        setWindowTitleX11(windowInfo);
    }
#endif // _WIN32
}

void getWindowTitle(PalWindowHandleInfoEx* windowInfo)
{
#ifdef _WIN32
#elif defined(__linux__)
    if (s_OnWayland) {
        getWindowTitleWayland(windowInfo);
    } else {
        getWindowTitleX11(windowInfo);
    }
#endif // _WIN32
}

bool nativeIntegrationTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Native Integration Test");
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

    // initialize the video system. We pass the event driver to recieve video
    // related events the video system does not copy the event driver, it must
    // be valid till the video system is shutdown
    result = palInitVideo(nullptr, eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize video: %s", error);
        return false;
    }

    // check for support
    PalVideoFeatures2 features = palGetVideoFeaturesEx();

    PalWindow* window = nullptr;
    PalWindowCreateInfo createInfo = {0};
    createInfo.monitor = nullptr; // use default monitor
    createInfo.height = 480;
    createInfo.width = 640;
    createInfo.show = true;
    createInfo.style = PAL_WINDOW_STYLE_RESIZABLE;
    createInfo.title = "Native Integration Test";
  
    // check if we support decorated windows (title bar, close etc)
    if (!(features & PAL_VIDEO_FEATURE_DECORATED_WINDOW)) {
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

    // set the window title using native APIs
    PalWindowHandleInfoEx windowInfo = {0};
    windowInfo = palGetWindowHandleInfoEx(window);

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

    return true;

}