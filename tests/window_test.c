
#include "tests.h"
#include "pal/pal_video.h"

// make the window borderless if supported
#define MAKE_BORDERLESS 0

// make the window transparent if supported
#define MAKE_TRANSPARENT 0
#define OPACITY 0.5 // if transparent window is supported

// make the window a tool window if supported
#define MAKE_TOOL 0

// remove the minimize box if supported
#define NO_MINIMIZEBOX 0

// remove the maximize box if supported
#define NO_MAXIMIZEBOX 0

#define UNICODE_NAME 0
#define DISPATCH_MODE_POLL 1 // use polling dispatch mode

#if DISPATCH_MODE_POLL
static const char* dispatchString = "Poll Mode";
#else
static const char* dispatchString = "Callback Mode";
#endif // DISPATCH_MODE_POLL

// inline helpers
static inline void onWindowResize(const PalEvent* event) {

    Uint32 width, height; // width == low, height == high
    palUnpackUint32(event->data, &width, &height);
    PalWindow* window = palUnpackPointer(event->data2);
    palLog(nullptr, "%s: Window Resized: (%d, %d)", dispatchString, width, height);
}

static inline void onWindowMove(const PalEvent* event) {

    Int32 x, y; // x == low, y == high
    palUnpackInt32(event->data, &x, &y);
    PalWindow* window = palUnpackPointer(event->data2);
    palLog(nullptr, "%s: Window Moved: (%d, %d)", dispatchString, x, y);
}

static inline void onWindowVisibility(const PalEvent* event) {

    PalWindow* window = palUnpackPointer(event->data2);
    if (event->data) {
        palLog(nullptr, "%s: Window is shown", dispatchString);

    } else {
        palLog(nullptr, "%s: Window is hidden", dispatchString);
    }
}

static inline void onWindowFocus(const PalEvent* event) {

    PalWindow* window = palUnpackPointer(event->data2);
    if (event->data) {
        palLog(nullptr, "%s: Window has gained focus", dispatchString);

    } else {
        palLog(nullptr, "%s: Window has lost focus", dispatchString);
    }
}

static inline void onWindowState(const PalEvent* event) {

    PalWindow* window = palUnpackPointer(event->data2);
    if (event->data == PAL_WINDOW_STATE_MAXIMIZED) {
        palLog(nullptr, "%s: Window maximized", dispatchString);

    } else if (event->data == PAL_WINDOW_STATE_MINIMIZED) {
        palLog(nullptr, "%s: Window minimized", dispatchString);

    } else {
        palLog(nullptr, "%s: Window restored", dispatchString);
    }
}

static inline void onWindowModalBegin(const PalEvent* event) {

    // window has entered modal mode (is being resize). only for windows
    PalWindow* window = palUnpackPointer(event->data2);
    palLog(nullptr, "%s: Window has entered modal mode", dispatchString);
}

static inline void onWindowModalEnd(const PalEvent* event) {

    // window has left modal mode. only for windows
    PalWindow* window = palUnpackPointer(event->data2);
    palLog(nullptr, "%s: Window has exited modal mode", dispatchString);
}

static inline void onDisplayDPI(const PalEvent* event) {

    PalWindow* window = palUnpackPointer(event->data2);
    palLog(nullptr, "%s: Display DPI: %d", dispatchString, event->data);
}

static inline void onDisplayList(const PalEvent* event) {

    PalWindow* window = palUnpackPointer(event->data2);
    palLog(nullptr, "%s: Display (monitor) List has been changed", dispatchString);
}

void PAL_CALL onEvent(void* userData, const PalEvent* event) {

    if (event->type == PAL_EVENT_WINDOW_SIZE) {
        onWindowResize(event);

    } else if (event->type == PAL_EVENT_WINDOW_MOVE) {
        onWindowMove(event);

    } else if (event->type == PAL_EVENT_WINDOW_VISIBILITY) {
        onWindowVisibility(event);

    } else if (event->type == PAL_EVENT_WINDOW_FOCUS) {
        onWindowFocus(event);

    } else if (event->type == PAL_EVENT_WINDOW_STATE) {
        onWindowState(event);

    } else if (event->type == PAL_EVENT_WINDOW_MODAL_BEGIN) {
        onWindowModalBegin(event);

    } else if (event->type == PAL_EVENT_WINDOW_MODAL_END) {
        onWindowModalEnd(event);

    } else if (event->type == PAL_EVENT_DISPLAY_DPI_CHANGED) {
        onDisplayDPI(event);

    } else if (event->type == PAL_EVENT_DISPLAYS_CHANGED) {
        onDisplayList(event);
    }
}

bool windowTest() {

    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Window Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalResult result;
    PalWindow* window = nullptr;
    PalWindowCreateInfo createInfo;
    PalVideoFeatures features;
    bool running = false;

    // event driver
    PalEventDriver* eventDriver = nullptr;
    PalEventDriverCreateInfo eventDriverCreateInfo;

    // fill the event driver create info
    eventDriverCreateInfo.allocator = nullptr; // default allocator
    eventDriverCreateInfo.callback = onEvent; // for callback dispatch
    eventDriverCreateInfo.queue = nullptr; // default queue
    eventDriverCreateInfo.userData = nullptr; // null

    // create the event driver
    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create event driver %s", palFormatResult(result));
        return false;
    }

    // initialize the video system. We pass the event driver to recieve video related events
    // the video does not copy this, this must be valid till the video system is shutdown
    result = palInitVideo(nullptr, eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to initialize video %s", palFormatResult(result));
        return false;
    }

    // get video system features 
    features = palGetVideoFeatures();

    // fill the create info struct
    createInfo.display = nullptr; // use primary display
    createInfo.height = 480;
    createInfo.width = 640;
    createInfo.show = true;
    createInfo.style = PAL_WINDOW_STYLE_RESIZABLE;

#if UNICODE_NAME
    createInfo.title = "PAL Test Window Unicode - àà";
#else 
    createInfo.title = "PAL Test Window";
#endif // UNICODE_NAME

#if MAKE_BORDERLESS
    if (features & PAL_VIDEO_FEATURE_BORDERLESS_WINDOW) {
        createInfo.style |= PAL_WINDOW_STYLE_BORDERLESS;
    }
#endif // MAKE_BORDERLESS

#if MAKE_TRANSPARENT
    if (features & PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW) {
        createInfo.style |= PAL_WINDOW_STYLE_TRANSPARENT;
    }
#endif // MAKE_TRANSPARENT

#if MAKE_TOOL
    if (features & PAL_VIDEO_FEATURE_TOOL_WINDOW) {
        createInfo.style |= PAL_WINDOW_STYLE_TOOL;
    }
#endif // MAKE_TOOL

#if NO_MINIMIZEBOX
    if (features & PAL_VIDEO_FEATURE_NO_MINIMIZEBOX) {
        createInfo.style |= PAL_WINDOW_STYLE_NO_MINIMIZEBOX;
    }
#endif // NO_MINIMIZEBOX

#if NO_MAXIMIZEBOX
    if (features & PAL_VIDEO_FEATURE_NO_MAXIMIZEBOX) {
        createInfo.style |= PAL_WINDOW_STYLE_NO_MAXIMIZEBOX;
    }
#endif // NO_MAXIMIZEBOX

    // create the window with the create info struct
    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create window %s", palFormatResult(result));
        return false;
    }

#if MAKE_TRANSPARENT
    if (features & PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW) {
        result = palSetWindowOpacity(window, OPACITY);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to set window opacity %s", palFormatResult(result));
            return false;
        }
    }
#endif // MAKE_TRANSPARENT
    PalDispatchMode dispatchMode = PAL_DISPATCH_NONE;
#if DISPATCH_MODE_POLL
    dispatchMode = PAL_DISPATCH_POLL;
#else
    dispatchMode = PAL_DISPATCH_CALLBACK;
#endif // DISPATCH_MODE_POLL

    // set dispatch mode for all events.
    for (Uint32 e = 0; e < PAL_EVENT_MAX; e++) {
        palSetEventDispatchMode(eventDriver, e, dispatchMode);
    }

    // we set window close to poll
    palSetEventDispatchMode(eventDriver, PAL_EVENT_WINDOW_CLOSE, PAL_DISPATCH_POLL);

    // we set callback mode for modal begin and end. Since we want to capture that instantly
    palSetEventDispatchMode(
        eventDriver, 
        PAL_EVENT_WINDOW_MODAL_BEGIN, 
        PAL_DISPATCH_CALLBACK
    );

    palSetEventDispatchMode(
        eventDriver, 
        PAL_EVENT_WINDOW_MODAL_END, 
        PAL_DISPATCH_CALLBACK
    );

    running = true;
    while (running) {
        // update the video system to push video events
        palUpdateVideo();
        
        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_WINDOW_CLOSE: {
                    return true;
                    break;
                }

                case PAL_EVENT_WINDOW_SIZE: {
                    onWindowResize(&event);
                    break;
                }

                case PAL_EVENT_WINDOW_MOVE: {
                    onWindowMove(&event);
                    break;
                }

                case PAL_EVENT_WINDOW_VISIBILITY: {
                    onWindowVisibility(&event);
                    break;
                }

                case PAL_EVENT_WINDOW_STATE: {
                    onWindowState(&event);
                    break;
                }

                case PAL_EVENT_WINDOW_FOCUS: {
                    onWindowFocus(&event);
                    break;
                }

                case PAL_EVENT_DISPLAY_DPI_CHANGED: {
                    onDisplayDPI(&event);
                    break;
                }

                case PAL_EVENT_DISPLAYS_CHANGED: {
                    onDisplayList(&event);
                    break;
                }
            }
        }

        // update
    }

    // destroy the window
    palDestroyWindow(window);

    // shutdown the video system
    palShutdownVideo();

    // destroy the event driver
    palDestroyEventDriver(eventDriver);

    return true;
}