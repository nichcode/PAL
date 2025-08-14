
#include "tests.h"
#include "pal/pal_video.h"

void windowTest() {

    palLog("");
    palLog("===========================================");
    palLog("Window Test");
    palLog("===========================================");
    palLog("");

    // create an event driver
    PalResult result;
    PalEventDriverCreateInfo eventDriverCreateInfo;
    PalEventDriver* eventDriver = nullptr;
    PalVideoFeatures features;

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

    // Set polling dispatch mode for all event types
    palSetEventDispatchMode(eventDriver, PAL_EVENT_WINDOW_CLOSE, PAL_DISPATCH_POLL);
    
    result = palInitVideo(nullptr, eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        // this can made into a goto to decrease this result checks
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    features = palGetVideoFeatures();

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

    result = palCreateWindow(&windowCreateInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    bool running = true;
    while (running) {
        // update video system to get video events
        palUpdateVideo();

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_WINDOW_CLOSE: {
                    // Check if it was the window you wanted to close
                    // useful for multi window applications
                    if (event.sourceID == palGetWindowID(window)) {
                        running = false;
                    }
                    break;
                }
            }
        }

        // update and render loop
    }

    palDestroyWindow(window);
    palShutdownVideo();
    palDestroyEventDriver(eventDriver);
}