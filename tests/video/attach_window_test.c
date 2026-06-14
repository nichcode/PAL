
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

#define CLASS_NAME L"NATIVE"
static HINSTANCE s_Instance;

#elif defined(__linux__)
#include <X11/Xlib.h>
#include <dlfcn.h>

typedef Window (*XCreateSimpleWindowFn)(
    Display*,
    Window,
    int,
    int,
    unsigned int,
    unsigned int,
    unsigned int,
    unsigned long,
    unsigned long);

typedef int (*XSyncFn)(
    Display*,
    Bool);

typedef int (*XMapRaisedFn)(
    Display*,
    Window);

typedef int (*XDestroyWindowFn)(
    Display*,
    Window);

static void* s_LibX;
static XCreateSimpleWindowFn s_XCreateWindow;
static XSyncFn s_XSync;
static XMapRaisedFn s_XMapRaised;
static XDestroyWindowFn s_XDestroyWindow;

#endif // _WIN32

#define WINDOW_POSX 100
#define WINDOW_POSY 100
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_TITLE "Attach Window Test"

static void* createX11Window()
{
#ifdef __linux__
    // load the procs
    s_LibX = dlopen("libX11.so", RTLD_LAZY);
    if (!s_LibX) {
        return nullptr;
    }

    // clang-format off
    s_XCreateWindow = (XCreateSimpleWindowFn)dlsym(
        s_LibX,
        "XCreateSimpleWindow");

    s_XSync = (XSyncFn)dlsym(
        s_LibX,
        "XSync");

    s_XMapRaised = (XMapRaisedFn)dlsym(
        s_LibX,
        "XMapRaised");

    s_XDestroyWindow = (XDestroyWindowFn)dlsym(
        s_LibX,
        "XDestroyWindow");
    // clang-format on

    if (!s_XCreateWindow || !s_XSync || !s_XMapRaised || !s_XDestroyWindow) {
        return nullptr;
    }

    Display* display = (Display*)palGetInstance();
    if (!display) {
        return nullptr;
    }

    // we create a simple window with no event mask
    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);

    Window window = s_XCreateWindow(
        display,
        root,
        WINDOW_POSX,
        WINDOW_POSX,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        1,
        BlackPixel(display, screen),
        WhitePixel(display, screen));

    if (!window) {
        return nullptr;
    }

    // make sure the window is mapped
    s_XMapRaised(display, window);
    s_XSync(display, False);

    return (void*)(uintptr_t)window;
#endif // __linux__
    return nullptr;
}

static void* createWin32Window()
{
#ifdef _WIN32
    // create a window class
    s_Instance = (HINSTANCE)palGetInstance();
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIconW(NULL, IDI_APPLICATION);
    wc.hInstance = s_Instance;
    wc.lpfnWndProc = DefWindowProcW;
    wc.lpszClassName = CLASS_NAME;
    wc.style = CS_OWNDC;
    if (!RegisterClassExW(&wc)) {
        return nullptr;
    }

    // create a simple window
    HWND window = CreateWindowExW(
        WS_EX_APPWINDOW,
        CLASS_NAME,
        L"",
        WS_OVERLAPPEDWINDOW,
        WINDOW_POSX,
        WINDOW_POSY,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        nullptr,
        nullptr,
        s_Instance,
        nullptr);

    if (!window) {
        return nullptr;
    }

    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);
    return (void*)window;
#endif // _WIN32
    return nullptr;
}

static void destroyWin32Window(void* windowHandle)
{
#ifdef _WIN32
    DestroyWindow((HWND)windowHandle);
    UnregisterClassW(CLASS_NAME, s_Instance);
#endif // _WIN32
}

static void destroyX11Window(void* windowHandle)
{
#ifdef __linux__
    Display* display = palGetInstance();
    s_XDestroyWindow(display, (Window)(uintptr_t)windowHandle);
    dlclose(s_LibX); // we loaded dynamically
#endif
}

static void* createPlatformWindow()
{
#ifdef _WIN32
    return createWin32Window();
#elif defined(__linux__)
    return createX11Window();
#endif // _WIN32
}

static void destroyPlatformWindow(void* windowHandle)
{
#ifdef _WIN32
    destroyWin32Window(windowHandle);
#elif defined(__linux__)
    destroyX11Window(windowHandle);
#endif // _WIN32
}

PalBool attachWindowTest()
{
    palLog(nullptr, "Press A to attach and D to detach window");
    palLog(nullptr, "Press Escape or click close button to close Test");
    
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
    PalVideoFeatures64 features = palGetVideoFeaturesEx();
    if (!(features & PAL_VIDEO_FEATURE64_FOREIGN_WINDOWS)) {
        palLog(nullptr, "Attaching and detaching foreign windows feature not supported");
        palDestroyEventDriver(eventDriver);
        palShutdownVideo();
        return false;
    }

    // we are interested in move and close events
    palSetEventDispatchMode(eventDriver, PAL_EVENT_WINDOW_CLOSE, PAL_DISPATCH_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_WINDOW_MOVE, PAL_DISPATCH_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_KEYDOWN, PAL_DISPATCH_POLL);

    // we listen for key release events to attach and detach the window
    palSetEventDispatchMode(eventDriver, PAL_EVENT_KEYUP, PAL_DISPATCH_POLL);

    // PAL allows users create any kind of window not currently or will not
    // be supported by PAL and just attach the window.
    // After attaching, the window acts as though it was created by PAL
    // Attached window can be detached as well
    // create your desired window with your all the styles you want
    void* platformWindow = createPlatformWindow();
    if (!platformWindow) {
        palLog(nullptr, "Failed to create platform window");
        return false;
    }

    // attach the created window to PAL video system
    // PAL can only attach windows that we created in the same processs
    // and the same instance. Get instance with palGetInstance()
    // destroying the window is the users responsibility
    PalWindow* myWindow = nullptr;
    result = palAttachWindow(platformWindow, &myWindow);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to attach window: %s", error);
        return false;
    }

    // now that the window is attached, we can use PAL video API
    // to manager it
    result = palSetWindowTitle(myWindow, WINDOW_TITLE);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to attach window: %s", error);
        return false;
    }

    PalBool running = true;
    PalBool detached = false;
    int32_t counter = 0;
    while (running) {
        // update the video system to push video events
        palUpdateVideo();

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_WINDOW_CLOSE: {
                    // we check if its really our attached window
                    PalWindow* window = palUnpackPointer(event.data2);
                    if (window == myWindow) {
                        running = false;
                    }
                    break;
                }

                case PAL_EVENT_WINDOW_MOVE: {
                    // this will be triggered for our attached window
                    int32_t x, y; // x == low, y == high
                    palUnpackInt32(event.data, &x, &y);
                    palLog(nullptr, "Window Moved: (%d, %d)", x, y);
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

                case PAL_EVENT_KEYUP: {
                    // we detach the window after keyup
                    // since if the window is detached,
                    // we wont recieve the key up event
                    // keycode == low, scancode == high
                    uint32_t keycode;
                    palUnpackUint32(event.data, &keycode, nullptr);
                    if (keycode == PAL_KEYCODE_D) {
                        palDetachWindow(myWindow, nullptr);
                        palLog(nullptr, "window detached");
                        palLog(nullptr, "not recieving events anymore");
                        detached = true;
                        counter = 0;
                    }
                    break;
                }
            }
        }

        if (detached) {
            counter++;
        }

        if (counter >= 2000000) { // we need a good delay
            palAttachWindow(platformWindow, &myWindow);
            palLog(nullptr, "window attached");
            palLog(nullptr, "recieving events");
            counter = 0; // reset it
            detached = false;
        }
    }

    void* myPlatformWindow = nullptr;
    // myPlatformWindow will be equal our platformWindow
    result = palDetachWindow(myWindow, &myPlatformWindow);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to attach window: %s", error);
        return false;
    }

    // We need to destroy the platform window before we shutdown
    // PAL video since the window was created with PAL video instance
    destroyPlatformWindow(platformWindow);

    // shutdown PAL video
    palShutdownVideo();
    palDestroyEventDriver(eventDriver);

    return true;
}
