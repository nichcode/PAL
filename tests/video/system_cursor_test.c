
#include "pal/pal_video.h"
#include "tests.h"

PalBool systemCursorTest()
{
    palLog(nullptr, "Press Escape or click close button to close Test");

    // fill the event driver create info
    PalEventDriverCreateInfo eventDriverCreateInfo = {0};
    eventDriverCreateInfo.allocator = nullptr; // default allocator
    eventDriverCreateInfo.callback = nullptr;  // no callback dispatch
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
    if (!(features & PAL_VIDEO_FEATURE_WINDOW_SET_CURSOR)) {
        palLog(nullptr, "Seting cursors feature not supported");
        palDestroyEventDriver(eventDriver);
        palShutdownVideo();
        return PAL_FALSE;
    }

    // create system cursor
    PalCursor* cursor = nullptr;
    result = palCreateCursorFrom(PAL_CURSOR_TYPE_CROSS, &cursor);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create window cursor");
        return PAL_FALSE;
    }

    // fill the create info struct
    PalWindowCreateInfo createInfo = {0};
    createInfo.monitor = nullptr; // use default monitor
    createInfo.height = 480;
    createInfo.width = 640;
    createInfo.show = PAL_TRUE;
    createInfo.style = PAL_WINDOW_STYLE_RESIZABLE;
    createInfo.title = "System Cursor Window - Cross";

    // check if we support decorated windows (title bar, close etc)
    if (!(features & PAL_VIDEO_FEATURE_DECORATED_WINDOW)) {
        // if we dont support, we need to create a borderless window
        // and create the decorations ourselves
        createInfo.style |= PAL_WINDOW_STYLE_BORDERLESS;
    }

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

    // set the cursor
    palSetWindowCursor(window, cursor);

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

    // destroy cursor
    palDestroyCursor(cursor);

    // shutdown the video system
    palShutdownVideo();

    // destroy the event driver
    palDestroyEventDriver(eventDriver);

    return PAL_TRUE;
}
