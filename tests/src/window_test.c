
#include "tests.h"
#include "pal/pal_video.h"

void windowTest() {

    palLog("");
    palLog("===========================================");
    palLog("Window Test");
    palLog("===========================================");
    palLog("");

    PalResult result;
    PalVideoSystem* video = nullptr;
    PalVideoFeatures features;
    PalVideoSystemCreateInfo createInfo;
    createInfo.allocator = nullptr; // for default.

    result = palCreateVideoSystem(&createInfo, &video);
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

    bool running = true;
    while (running) {
        palUpdateVideo(video);
    }

    palDestroyWindow(window);
    palDestroyVideoSystem(video);
}