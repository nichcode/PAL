
#include "pal/pal_video.h"
#include "tests.h"

PalBool iconTest()
{
    palLog(nullptr, "Press Escape or click close button to close Test");

    // fill the event driver create info
    PalEventDriverCreateInfo eventDriverCreateInfo = {0};
    eventDriverCreateInfo.allocator = nullptr; // default allocator
    eventDriverCreateInfo.callback = nullptr;  // for callback dispatch
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
    // related events the video does not copy this, this must be valid till the
    // video system is shutdown
    result = palInitVideo(nullptr, eventDriver, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to initialize video");
        return PAL_FALSE;
    }

    // check for support
    PalVideoFeatures features = palGetVideoFeatures();
    if (!(features & PAL_VIDEO_FEATURE_WINDOW_SET_ICON)) {
        palLog(nullptr, "Setting icons feature not supported");
        palDestroyEventDriver(eventDriver);
        palShutdownVideo();
        return PAL_FALSE;
    }

    // simple checkerboard RGBA pixel buffer
    // every block contains 64 pixels
    uint8_t pixels[32 * 32 * 4]; // size is 32 and we have 4 channles
    for (int32_t y = 0; y < 32; ++y) {
        for (int32_t x = 0; x < 32; ++x) {
            int32_t i = (y * 32 + x) * 4;
            int checker = ((x / 8) ^ (y / 8)) & 1;
            if (checker) {
                pixels[i + 0] = 255; // Red bit
                pixels[i + 1] = 0;   // Green bit
                pixels[i + 2] = 0;   // Blue bit
                pixels[i + 3] = 255; // Alpha bit

            } else {
                pixels[i + 0] = 0;   // Red bit
                pixels[i + 1] = 255; // Green bit
                pixels[i + 2] = 0;   // Blue bit
                pixels[i + 3] = 255; // Alpha bit
            }
        }
    }

    // create icon
    PalIconCreateInfo iconCreateInfo = {0};
    iconCreateInfo.width = 32;
    iconCreateInfo.height = 32;
    iconCreateInfo.pixels = pixels; // can be loaded from an .ico file

    PalIcon* icon = nullptr;
    result = palCreateIcon(&iconCreateInfo, &icon);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create window icon");
        return PAL_FALSE;
    }

    // fill the create info struct
    PalWindowCreateInfo createInfo = {0};
    createInfo.monitor = nullptr; // use default monitor
    createInfo.height = 480;
    createInfo.width = 640;
    createInfo.show = PAL_TRUE;
    createInfo.style = PAL_WINDOW_STYLE_RESIZABLE;
    createInfo.title = "Icon Window";

    // create the window with the create info struct
    PalWindow* window = nullptr;
    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create window");
        return PAL_FALSE;
    }

    // set the dispatch mode for window close event to recieve it
    palSetEventDispatchMode(eventDriver, PAL_EVENT_TYPE_WINDOW_CLOSE, PAL_DISPATCH_MODE_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_TYPE_KEYDOWN, PAL_DISPATCH_MODE_POLL);

    // set the icon
    result = palSetWindowIcon(window, icon);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to set window icon");
        return PAL_FALSE;
    }

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

    // destroy icon
    palDestroyIcon(icon);

    // shutdown the video system
    palShutdownVideo();

    // destroy the event driver
    palDestroyEventDriver(eventDriver);

    return PAL_TRUE;
}
