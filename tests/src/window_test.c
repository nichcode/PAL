
#include "tests.h"
#include "pal/pal_video.h"

// TODO: remove
#define CLOSE_TIME 1000000

void windowTest() {

    palLogInfo(PAL_NULL, "");
    palLogInfo(PAL_NULL, "===========================================");
    palLogInfo(PAL_NULL, "Window Test");
    palLogInfo(PAL_NULL, "===========================================");
    palLogInfo(PAL_NULL, "");

    PalResult result;
    PalVideo video;
    PalWindow window;
    PalDisplay primaryDisplay;
    PalVideoFeatureFlags videoFeatures;

    PalVideoConfig videoConfig;
    videoConfig.allocator = PAL_NULL;

    result = palCreateVideo(&videoConfig, &video);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return;
    }

    // get video features
    result = palGetVideoFeatures(video, &videoFeatures);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return;
    }

    result = palGetPrimaryDisplay(video, &primaryDisplay);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return;
    }

    // create window
    PalWindowConfig windowConfig;
    windowConfig.display = primaryDisplay; // or NULL for primary display
    windowConfig.width = 640;
    windowConfig.height = 480;
    windowConfig.title = "Pal Window";
    windowConfig.flags = PAL_APPWINDOW | PAL_WINDOW_CENTER;

    if (videoFeatures & PAL_VIDEO_HIGH_DPI) {
        windowConfig.flags |= PAL_WINDOW_HIGH_DPI;
    }

    result = palCreateWindow(video, &windowConfig, &window);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return;
    }

    bool running = PAL_TRUE;
    int timer =  0;
    while (running) {
        palUpdateVideo(video);

        // TODO: fixme(with timers or events)
        timer++;
        if (timer >= CLOSE_TIME) {
            running = PAL_FALSE;
        }
    }

    palDestroyWindow(window);
    palDestroyVideo(video);
}