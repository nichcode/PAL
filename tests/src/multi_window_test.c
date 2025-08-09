
#include "tests.h"
#include "pal/pal_video.h"

void multiWindowTest() {

    palLog("");
    palLog("===========================================");
    palLog("Multi Window Test");
    palLog("===========================================");
    palLog("");

    // create an event driver
    PalResult result;
    PalEventDriverCreateInfo eventDriverCreateInfo;
    PalEventDriver* eventDriver = nullptr;
    PalVideoSystem* video = nullptr;
    PalVideoFeatures features;
    PalVideoSystemCreateInfo videoCreateInfo;

    eventDriverCreateInfo.allocator = nullptr;
    eventDriverCreateInfo.callback = nullptr;
    eventDriverCreateInfo.queue = nullptr; // use default
    eventDriverCreateInfo.userData = nullptr; // not using callback mode
    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        // this can made into a goto to decrease this result checks
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // Set polling dispatch mode for event types
    palSetEventDispatchMode(eventDriver, PAL_EVENT_WINDOW_CLOSE, PAL_DISPATCH_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_WINDOW_RESIZE, PAL_DISPATCH_POLL);

    videoCreateInfo.allocator = nullptr; // for default.
    videoCreateInfo.eventDriver = eventDriver; // for pushing events
    result = palCreateVideoSystem(&videoCreateInfo, &video);
    if (result != PAL_RESULT_SUCCESS) {
        // this can made into a goto to decrease this result checks
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    features = palGetVideoFeatures(video);

    // create window
    PalWindow* window = nullptr;
    PalWindowCreateInfo windowCreateInfo;
    windowCreateInfo.display = nullptr; // use primary display
    windowCreateInfo.width = 640;
    windowCreateInfo.height = 480;
    windowCreateInfo.title = "Pal Test Window";
    windowCreateInfo.flags = PAL_WINDOW_CENTER | PAL_WINDOW_DEFAULT;

    if (features & PAL_VIDEO_FEATURE_HIGH_DPI) {
        windowCreateInfo.flags |= PAL_WINDOW_ALLOW_HIGH_DPI;
    }

    // if (features & PAL_VIDEO_FEATURE_BORDERLESS_WINDOW) {
    //     windowCreateInfo.flags |= PAL_WINDOW_FULLSCREEN;
    // }

    result = palCreateWindow(video, &windowCreateInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // create second window reusing the same PalWindowCreateInfo
    PalWindow* window2 = nullptr;
    windowCreateInfo.title = "Pal Test Window 2";
    result = palCreateWindow(video, &windowCreateInfo, &window2);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    bool running = true;
    while (running) {
        // update video system to get video events
        palUpdateVideo(video);

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_WINDOW_CLOSE: {
                    // Check if it was the window you wanted to close
                    // useful for multi window applications
                    if (event.sourceID == palGetWindowID(window)) {
                        running = false;

                    } else if (event.sourceID == palGetWindowID(window2)) {
                        palLog("Window 2 close button was clicked");

                    }
                    break;
                }

                case PAL_EVENT_WINDOW_RESIZE: {
                    Uint32 width, height;
                    palUnpackUint32(event.data, &width, &height);
                    const char* windowTag = nullptr;

                    if (event.sourceID == palGetWindowID(window)) {
                        windowTag = "Window 1";
                    } else {
                        windowTag = "Window 2";
                    }

                    palLog("%s resized - (%d, %d)", windowTag, width, height);
                }
            }
        }

        // update and render loop
    }

    palDestroyWindow(window);
    palDestroyWindow(window2);
    palDestroyVideoSystem(video);
    palDestroyEventDriver(eventDriver);
}