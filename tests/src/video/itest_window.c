
#include "pal2/core/memory.h"
#include "pal2/core/log.h"
#include "pal2/core/pack.h"
#include "pal2/event/driver.h"

#include "pal2/video/init.h"
#include "pal2/video/window.h"
#include "pal2/video/keyboard.h"
#include "pal2/video/mouse.h"

int main(void)
{
    palLog(nullptr, "Running window test...");

    PalEventDriver* eventDriver = nullptr;
    PalWindow* window = nullptr;
    PalResult result = PAL_RESULT_SUCCESS;
    int ret = 0;

    PalEventDriverCreateInfo eventDriverCreateInfo = {0};

    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create event driver");
        ret = -1;
        goto cleanup;
    }

    result = palInitVideo(nullptr, eventDriver, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to initialize video system");
        palDestroyEventDriver(eventDriver);
        return -1;
    }

    PalVideoFeatures features = palGetVideoFeatures();
    if (!(features & PAL_VIDEO_FEATURE_WINDOW_SET_CURSOR)) {
        goto cleanup;
    }    

    PalWindowCreateInfo windowCreateInfo = {0};
    windowCreateInfo.monitor = nullptr;
    windowCreateInfo.height = 480;
    windowCreateInfo.width = 640;
    windowCreateInfo.show = PAL_TRUE;
    windowCreateInfo.title = "Window";

    if (!(features & PAL_VIDEO_FEATURE_DECORATED_WINDOW)) {
        windowCreateInfo.style = PAL_WINDOW_STYLE_BORDERLESS;
    }

    result = palCreateWindow(&windowCreateInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create window");
        ret = -1;
        goto cleanup;
    }

    for (uint32_t e = 0; e < PAL_EVENT_TYPE_COUNT; e++) {
        palSetEventDispatchMode(eventDriver, e, PAL_DISPATCH_MODE_POLL);
    }

    PalBool running = PAL_TRUE;
    while (running) {
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

                case PAL_EVENT_TYPE_WINDOW_SIZE: {
                    uint32_t width, height;
                    palUnpackUint32(event.data, &width, &height);
                    palLog(nullptr, "Window Resized: (%u, %u)", width, height);
                    break;
                }

                case PAL_EVENT_TYPE_WINDOW_MOVE: {
                    int32_t x, y;
                    palUnpackInt32(event.data, &x, &y);
                    palLog(nullptr, "Window Moved: (%u, %u)", x, y);
                    break;
                }

                case PAL_EVENT_TYPE_WINDOW_VISIBILITY: {
                    if (event.data) {
                        palLog(nullptr, "Window is shown");
                    } else {
                        palLog(nullptr, "Window is hidden");
                    }
                    break;
                }

                case PAL_EVENT_TYPE_WINDOW_STATE: {
                    if (event.data == PAL_WINDOW_STATE_MAXIMIZED) {
                        palLog(nullptr, "Window maximized");
                    } else if (event.data == PAL_WINDOW_STATE_MINIMIZED) {
                        palLog(nullptr, "Window minimized");
                    } else {
                        palLog(nullptr, "Window restored");
                    }
                    break;
                }

                case PAL_EVENT_TYPE_WINDOW_FOCUS: {
                    if (event.data) {
                        palLog(nullptr, "Window has gained focus");
                    } else {
                        palLog(nullptr, "Window has lost focus");
                    }
                    break;
                }

                case PAL_EVENT_TYPE_MONITOR_DPI_CHANGED: {
                    palLog(nullptr, "Monitor DPI: %d", event.data);
                    break;
                }

                case PAL_EVENT_TYPE_MONITOR_LIST_CHANGED: {
                    palLog(nullptr, "Monitor List has been changed");
                    break;
                }

                case PAL_EVENT_TYPE_WINDOW_DECORATION_MODE: {
                    if (event.data == PAL_DECORATION_MODE_CLIENT_SIDE) {
                        palLog(nullptr, "Window Decoration Mode: Client Side");
                    } else {
                        palLog(nullptr, "Window Decoration Mode: Server Side");
                    }
                    break;
                }

                case PAL_EVENT_TYPE_WINDOW_MODAL_BEGIN: {
                    palLog(nullptr, "Window has entered modal mode");
                    break;
                }

                case PAL_EVENT_TYPE_WINDOW_MODAL_END: {
                    palLog(nullptr, "Window has exited modal mode");
                    break;
                }

                case PAL_EVENT_TYPE_KEYCHAR: {
                    uint32_t codepoint = (uint32_t)event.data;
                    palLog(nullptr, "U+%04X", codepoint);
                    break;
                }
            }
        }
    }

cleanup:
    if (window) {
        palDestroyWindow(window);
    }

    palShutdownVideo();

    if (eventDriver) {
        palDestroyEventDriver(eventDriver);
    }

    return ret;
}