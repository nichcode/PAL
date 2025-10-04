
#include "pal/pal_video.h"
#include "tests.h"

bool systemCursorTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "System Cursor Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalResult result;
    PalWindow* window = nullptr;
    PalCursor* cursor = nullptr;
    PalWindowCreateInfo createInfo = {0};
    bool running = false;

    // event driver
    PalEventDriver* eventDriver = nullptr;
    PalEventDriverCreateInfo eventDriverCreateInfo = {0};

    // fill the event driver create info
    eventDriverCreateInfo.allocator = nullptr; // default allocator
    eventDriverCreateInfo.callback = nullptr;  // for callback dispatch
    eventDriverCreateInfo.queue = nullptr;     // default queue
    eventDriverCreateInfo.userData = nullptr;  // null

    // create the event driver
    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create event driver %s", error);
        return false;
    }

    // initialize the video system. We pass the event driver to recieve video
    // related events the video does not copy this, this must be valid till the
    // video system is shutdown
    result = palInitVideo(nullptr, eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize video %s", error);
        return false;
    }

    // create system cursor
    result = palCreateCursorFrom(PAL_CURSOR_CROSS, &cursor);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create window cursor %s", error);
        return false;
    }

    // fill the create info struct
    createInfo.monitor = nullptr; // use primary monitor
    createInfo.height = 480;
    createInfo.width = 640;
    createInfo.show = true;
    createInfo.style = PAL_WINDOW_STYLE_RESIZABLE;
    createInfo.title = "PAL System Cursor Window - Cross";

    // create the window with the create info struct
    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create window %s", error);
        return false;
    }

    // set the dispatch mode for window close event to recieve it
    palSetEventDispatchMode(
        eventDriver,
        PAL_EVENT_WINDOW_CLOSE,
        PAL_DISPATCH_POLL); // polling

    // set the cursor
    palSetWindowCursor(window, cursor);

    running = true;
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

    return true;
}