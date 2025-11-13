
#include "pal/pal_video.h"
#include "tests.h"

bool charEventTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Character Event Test");
    palLog(nullptr, "Press Escape or click close button to close Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalResult result;
    PalWindow* window = nullptr;
    PalWindowCreateInfo createInfo = {0};

    // event driver
    PalEventDriver* eventDriver = nullptr;
    PalEventDriverCreateInfo eventDriverCreateInfo = {0};

    // create the event driver
    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create event driver: %s", error);
        return false;
    }

    // initialize the video system. We pass the event driver to recieve video
    // related events the video system does not copy the event driver, it must
    // be valid till the video system is shutdown
    result = palInitVideo(nullptr, eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize video: %s", error);
        return false;
    }

    // fill the create info struct
    createInfo.monitor = nullptr; // use default monitor
    createInfo.height = 480;
    createInfo.width = 640;
    createInfo.show = true;
    createInfo.title = "Character Window";

    // check if we support decorated windows (title bar, close etc)
    PalVideoFeatures64 features = palGetVideoFeaturesEx();
    if (!(features & PAL_VIDEO_FEATURE64_DECORATED_WINDOW)) {
        // if we dont support, we need to create a borderless window
        // and create the decorations ourselves
        createInfo.style |= PAL_WINDOW_STYLE_BORDERLESS;
    }

    // create the window with the create info struct
    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create window: %s", error);
        return false;
    }

    // we only neeed PAL_EVENT_KEYCHAR and PAL_EVENT_WINDOW_CLOSE
    palSetEventDispatchMode(
        eventDriver,
        PAL_EVENT_WINDOW_CLOSE,
        PAL_DISPATCH_POLL);

    palSetEventDispatchMode(
        eventDriver,
        PAL_EVENT_KEYDOWN,
        PAL_DISPATCH_POLL);

    palSetEventDispatchMode(eventDriver, PAL_EVENT_KEYCHAR, PAL_DISPATCH_POLL);

    bool running = true;
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

                case PAL_EVENT_KEYCHAR: {
                    Uint32 codepoint = (Uint32)event.data;
                    PalWindow* window = palUnpackPointer(event.data2);

                    // we log the codepoint
                    // and also log if its A or a charracter
                    palLog(nullptr, "U+%04X", codepoint);

                    if (codepoint == 0x0041) {
                        palLog(nullptr, "A Character");
                    } else if (codepoint == 0x0061) {
                        // small a
                        palLog(nullptr, "a Character");
                    }
                    break;
                }

                case PAL_EVENT_KEYDOWN: {
                    PalKeycode keycode = 0;
                    palUnpackUint32(event.data, &keycode, nullptr);
                    if (keycode == PAL_KEYCODE_ESCAPE) {
                        running = false;
                    }
                    break;
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