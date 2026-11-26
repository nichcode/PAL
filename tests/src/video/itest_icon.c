
#include "pal2/core/memory.h"
#include "pal2/core/log.h"
#include "pal2/core/pack.h"
#include "pal2/event/driver.h"
#include "pal2/video/init.h"

#include "pal2/video/window.h"
#include "pal2/video/keyboard.h"
#include "pal2/video/icon.h"

int main(void)
{
    palLog(nullptr, "Running icon itest...");

    PalEventDriver* eventDriver = nullptr;
    PalWindow* window = nullptr;
    PalIcon* icon = nullptr;
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
    if (!(features & PAL_VIDEO_FEATURE_WINDOW_SET_ICON)) {
        goto cleanup;
    }

    // Simple checkerboard RGBA pixel buffer. Every block contains 64 pixels.
    uint8_t pixels[32 * 32 * 4]; // size is 32 and we have 4 channles
    for (int32_t y = 0; y < 32; ++y) {
        for (int32_t x = 0; x < 32; ++x) {
            int32_t i = (y * 32 + x) * 4;
            int checker = ((x / 8) ^ (y / 8)) & 1;
            if (checker) {
                pixels[i + 0] = 255;
                pixels[i + 1] = 0;
                pixels[i + 2] = 0;
                pixels[i + 3] = 255;

            } else {
                pixels[i + 0] = 0;  
                pixels[i + 1] = 255;
                pixels[i + 2] = 0;
                pixels[i + 3] = 255;
            }
        }
    }

    PalIconCreateInfo iconCreateInfo = {0};
    iconCreateInfo.width = 32;
    iconCreateInfo.height = 32;
    iconCreateInfo.pixels = pixels;

    result = palCreateIcon(&iconCreateInfo, &icon);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create icon");
        ret = -1;
        goto cleanup;
    }

    PalWindowCreateInfo windowCreateInfo = {0};
    windowCreateInfo.monitor = nullptr;
    windowCreateInfo.height = 480;
    windowCreateInfo.width = 640;
    windowCreateInfo.show = PAL_TRUE;
    windowCreateInfo.title = "Icon Window";

    if (!(features & PAL_VIDEO_FEATURE_DECORATED_WINDOW)) {
        windowCreateInfo.style = PAL_WINDOW_STYLE_BORDERLESS;
    }

    result = palCreateWindow(&windowCreateInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create window");
        ret = -1;
        goto cleanup;
    }

    palSetEventDispatchMode(eventDriver, PAL_EVENT_TYPE_WINDOW_CLOSE, PAL_DISPATCH_MODE_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_TYPE_KEYDOWN, PAL_DISPATCH_MODE_POLL);
    palSetWindowIcon(window, icon);

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
            }
        }
    }

cleanup:
    if (window) {
        palDestroyWindow(window);
    }

    if (icon) {
        palDestroyIcon(icon);
    }

    palShutdownVideo();

    if (eventDriver) {
        palDestroyEventDriver(eventDriver);
    }

    return ret;
}