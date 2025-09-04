
#include "tests.h"
#include "pal/pal_video.h"

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

    // initialize the video system
    result = palInitVideo(nullptr);
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
    createInfo.title = "PAL Test Window";
    createInfo.flags = PAL_WINDOW_DEFAULT; // PAL_WINDOW_RESIZABLE | PAL_WINDOW_SHOWN

    // remove the minimizebox if supported
    if (features & PAL_VIDEO_FEATURE_NO_MINIMIZEBOX) {
        createInfo.flags |= PAL_WINDOW_NO_MINIMIZEBOX;
    }

    // create the window with the create info struct
    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create window %s", palFormatResult(result));
        return false;
    }

    // destroy the window
    palDestroyWindow(window);

    // shutdown the video system
    palShutdownVideo();

    return true;
}