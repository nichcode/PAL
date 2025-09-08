
#include "tests.h"
#include "pal/pal_video.h"

bool inputWindowTest() {

    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Input Window Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalResult result;
    PalWindow* window = nullptr;
    PalWindowCreateInfo createInfo = {};
    bool running = false;

    // event driver
    PalEventDriver* eventDriver = nullptr;
    PalEventDriverCreateInfo eventDriverCreateInfo = {};

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
    // the video system does not copy the event driver, it must be valid till the video system is shutdown
    result = palInitVideo(nullptr, eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to initialize video %s", palFormatResult(result));
        return false;
    }

    // fill the create info struct
    createInfo.display = nullptr; // use primary display
    createInfo.height = 480;
    createInfo.width = 640;
    createInfo.show = true;
    createInfo.style = PAL_WINDOW_STYLE_RESIZABLE;
    createInfo.title = "PAL Input Window";

    // create the window with the create info struct
    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create window %s", palFormatResult(result));
        return false;
    }

    // we set window close to poll
    palSetEventDispatchMode(
        eventDriver, 
        PAL_EVENT_WINDOW_CLOSE, 
        PAL_DISPATCH_POLL
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
            }
        }

        const bool* buttons = palGetMouseState();
        if (buttons[PAL_MOUSE_BUTTON_LEFT]) {
            palLog(nullptr, "Mouse button left pressed");
        }

        if (buttons[PAL_MOUSE_BUTTON_RIGHT]) {
            palLog(nullptr, "Mouse button right pressed");
        }

        const bool* keycodes = palGetKeycodeState();
        if (keycodes[PAL_KEYCODE_LEFT]) {
            palLog(nullptr, "Left keycode pressed");
        }

        if (keycodes[PAL_KEYCODE_RIGHT]) {
            palLog(nullptr, "Right keycode pressed");
        }

        // update
    }

    palDestroyWindow(window);

    // shutdown the video system
    palShutdownVideo();

    // destroy the event driver
    palDestroyEventDriver(eventDriver);

    return true;
}