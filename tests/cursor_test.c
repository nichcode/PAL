
#include "tests.h"
#include "pal/pal_video.h"

bool cursorTest() {

    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Cursor Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalResult result;
    PalWindow* window = nullptr;
    PalCursor* cursor = nullptr;
    PalWindowCreateInfo createInfo = {0};
    PalCursorCreateInfo cursorCreateInfo = {0};
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

    // simple checkerboard RGBA pixel buffer
    // every block contains 64 pixels 
    Uint8 pixels[32 * 32 * 4]; // size is 32 and we have 4 channles
    for (Int32 y = 0; y < 32; ++y) {
        for (Int32 x = 0; x < 32; ++x) {
            Int32 i = (y * 32 + x) * 4;
            int checker = ((x / 8) ^ (y / 8)) & 1;
            if (checker) {
                pixels[i + 0] = 255;  // Red bit
                pixels[i + 1] = 0;  // Green bit
                pixels[i + 2] = 0;  // Blue bit
                pixels[i + 3] = 0;  // Alpha bit

            } else {
                pixels[i + 0] = 0;  // Red bit
                pixels[i + 1] = 0;  // Green bit
                pixels[i + 2] = 255;  // Blue bit
                pixels[i + 3] = 0;  // Alpha bit
            }
        }
    }

    // create cursor
    cursorCreateInfo.width = 32;
    cursorCreateInfo.height = 32;
    cursorCreateInfo.xHotspot = 0;
    cursorCreateInfo.yHotspot = 0;
    cursorCreateInfo.pixels = pixels;

    result = palCreateCursor(&cursorCreateInfo, &cursor);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create window cursor %s", palFormatResult(result));
        return false;
    }

    // fill the create info struct
    createInfo.monitor = nullptr; // use primary monitor
    createInfo.height = 480;
    createInfo.width = 640;
    createInfo.show = true;
    createInfo.style = PAL_WINDOW_STYLE_RESIZABLE;
    createInfo.title = "PAL cursor Window";

    // create the window with the create info struct
    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create window %s", palFormatResult(result));
        return false;
    }

    // set the dispatch mode for window close event to recieve it
    palSetEventDispatchMode(eventDriver, PAL_EVENT_WINDOW_CLOSE, PAL_DISPATCH_POLL); // polling

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