
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

#define UNICODE_NAME 1

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
    eventDriverCreateInfo.callback = nullptr; // for callback dispatch
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

    // set the dispatch mode for window close event to recieve it
    palSetEventDispatchMode(eventDriver, PAL_EVENT_WINDOW_CLOSE, PAL_DISPATCH_POLL); // polling

    running = true;
    while (running) {
        // update the video system to push video events
        palUpdateVideo();
        
        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_WINDOW_CLOSE: {
                    // window close buton was clicked
                    // optionally, get the window whose close button was clicked
                    PalWindow* tmp = (PalWindow*)palUnpackPointer(event.data2);
                    if (tmp == window) {
                        // get window title
                        char* title;
                        result = palGetWindowTitle(window, &title);
                        if (result != PAL_RESULT_SUCCESS) {
                            palLog(nullptr, "Failed to get window title %s", palFormatResult(result));
                            return false;
                        }

                        palLog(nullptr, "%s: closed", title);

                        // this is allocated by the video allocate
                        // so free with the same allocator
                        palFree(nullptr, title);
                        running = false;
                        break;
                    }
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