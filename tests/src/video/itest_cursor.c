
#include "pal2/core/time.h"
#include "pal2/core/memory.h"
#include "pal2/core/log.h"
#include "pal2/core/pack.h"
#include "pal2/event/driver.h"

#include "pal2/video/init.h"
#include "pal2/video/window.h"
#include "pal2/video/keyboard.h"
#include "pal2/video/cursor.h"

#define CURSOR_SIZE 24
#define INTERVAL 1.0
#define CURSOR_COUNT (PAL_CURSOR_TYPE_COUNT + 1)

typedef struct Timer
{
    uint64_t frequency;
    uint64_t startTime;
} Timer;

static inline double getTime(Timer* timer)
{
    uint64_t now = palGetPerformanceCounter();
    return (double)(now - timer->startTime) / (double)timer->frequency;
}

int main(void)
{
    palLog(nullptr, "Running cursor test...");

    Timer timer;
    timer.frequency = palGetPerformanceFrequency();
    timer.startTime = palGetPerformanceCounter();
    double lastTime = getTime(&timer);

    PalEventDriver* eventDriver = nullptr;
    PalWindow* window = nullptr;
    PalCursor* cursors[CURSOR_COUNT] = {0};
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

    // Simple checkerboard RGBA pixel buffer. Every block contains 64 pixels.
    uint8_t pixels[CURSOR_SIZE * CURSOR_SIZE * 4];
    for (int32_t y = 0; y < CURSOR_SIZE; ++y) {
        for (int32_t x = 0; x < CURSOR_SIZE; ++x) {
            int32_t i = (y * CURSOR_SIZE + x) * 4;
            int checker = ((x / 4) ^ (y / 4)) & 1;
            if (checker) {
                pixels[i + 0] = 255;
                pixels[i + 1] = 0;
                pixels[i + 2] = 0;
                pixels[i + 3] = 255;

            } else {
                pixels[i + 0] = 0;
                pixels[i + 1] = 0;
                pixels[i + 2] = 255;
                pixels[i + 3] = 255;
            }
        }
    }

    PalCursorCreateInfo cursorCreateInfo = {0};
    cursorCreateInfo.width = CURSOR_SIZE;
    cursorCreateInfo.height = CURSOR_SIZE;
    cursorCreateInfo.xHotspot = CURSOR_SIZE / 2;
    cursorCreateInfo.yHotspot = CURSOR_SIZE / 2;
    cursorCreateInfo.pixels = pixels;

    result = palCreateCursor(&cursorCreateInfo, &cursors[0]);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create cursor");
        ret = -1;
        goto cleanup;
    }

    PalCursorType cursorTypes[] = {
        PAL_CURSOR_TYPE_ARROW,
        PAL_CURSOR_TYPE_HAND,
        PAL_CURSOR_TYPE_CROSS,
        PAL_CURSOR_TYPE_IBEAM,
        PAL_CURSOR_TYPE_WAIT,
    };

    for (uint32_t i = 0; i < PAL_CURSOR_TYPE_COUNT; i++) {
        result = palCreateCursorFrom(cursorTypes[i], &cursors[i + 1]);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to create cursor");
            ret = -1;
            goto cleanup;
        }
    }

    PalWindowCreateInfo windowCreateInfo = {0};
    windowCreateInfo.monitor = nullptr;
    windowCreateInfo.height = 480;
    windowCreateInfo.width = 640;
    windowCreateInfo.show = PAL_TRUE;
    windowCreateInfo.title = "Cursor Window";

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
    
    uint32_t cursorIndex = 0;
    palSetWindowCursor(window, cursors[cursorIndex]);

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

        double now = getTime(&timer);
        if (now - lastTime >= INTERVAL) {
            cursorIndex = (cursorIndex + 1) % CURSOR_COUNT;
            palSetWindowCursor(window, cursors[cursorIndex]);
            lastTime = now;
        }
    }

cleanup:
    if (window) {
        palDestroyWindow(window);
    }

    for (uint32_t i = 0; i < CURSOR_COUNT; i++) {
        palDestroyCursor(cursors[i]);
    }

    palShutdownVideo();

    if (eventDriver) {
        palDestroyEventDriver(eventDriver);
    }

    return ret;
}