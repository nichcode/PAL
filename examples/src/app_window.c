
#include "pal/pal.h"

void onEvent(void* userData, const PalEvent* event) {

    if (event->type == PAL_EVENT_WINDOW_MOVE) {
        PalEventPosInfo posInfo;
        palGetEventPosInfo(event, &posInfo);
        palLogInfo(PAL_NULL, "Window Pos (%i, %i)", posInfo.x, posInfo.y);
    }

    if (event->type == PAL_EVENT_WINDOW_RESIZE) {
        PalEventSizeInfo sizeInfo;
        palGetEventSizeInfo(event, &sizeInfo);
        palLogInfo(PAL_NULL, "Window Size (%u, %u)", sizeInfo.width, sizeInfo.height);
    }
}

int main(int argc, char** argv) {

    PalResult result;
    PalEventDriver eventDriver;
    PalVideo video;
    PalWindow window;
    PalDisplay primaryDisplay;
    PalVideoFeatureFlags videoFeatures;

    // create event driver
    PalEventDriverConfig eventDriverConfig;
    eventDriverConfig.allocator = PAL_NULL;
    eventDriverConfig.callback = onEvent; // for callback mode
    eventDriverConfig.queue = PAL_NULL;
    eventDriverConfig.userData = PAL_NULL;

    result = palCreateEventDriver(&eventDriverConfig, &eventDriver);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return -1;
    }

    PalVideoConfig videoConfig;
    videoConfig.allocator = PAL_NULL;
    videoConfig.eventDriver = eventDriver; // for events

    // create video system
    result = palCreateVideo(&videoConfig, &video);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return -1;
    }

    // enable events 
    // TIP: enable all events and disabled the ones you dont want
    palEnableAllEvents(eventDriver, PAL_EVENT_MODE_POLL);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return -1;
    }

    // we want real time events for PAL_EVENT_WINDOW_MOVE
    palEnableEvent(eventDriver, PAL_EVENT_WINDOW_MOVE, PAL_EVENT_MODE_CALLBACK);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return -1;
    }

    // we want real time events for PAL_EVENT_WINDOW_RESIZE
    palEnableEvent(eventDriver, PAL_EVENT_WINDOW_RESIZE, PAL_EVENT_MODE_CALLBACK);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return -1;
    }

    // get video features
    result = palGetVideoFeatures(video, &videoFeatures);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return -1;
    }

    result = palGetPrimaryDisplay(video, &primaryDisplay);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return -1;
    }

    // create window
    PalWindowConfig windowConfig;
    windowConfig.display = PAL_NULL; // or NULL for primary display
    windowConfig.width = 640;
    windowConfig.height = 480;
    windowConfig.title = "App Window";
    windowConfig.flags = PAL_APPWINDOW | PAL_WINDOW_CENTER;

    if (videoFeatures & PAL_VIDEO_HIGH_DPI) {
        windowConfig.flags |= PAL_WINDOW_HIGH_DPI;
    }

    result = palCreateWindow(video, &windowConfig, &window);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return -1;
    }

    bool running = PAL_TRUE;
    while (running) {
        // update video system to get events
        palUpdateVideo(video);

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                // you must enable events with a mode to get events
                case PAL_EVENT_QUIT: {
                    running = PAL_FALSE;
                    break;
                }

                // this will only be triggered if video features has PAL_VIDEO_HIGH_DPI;
                case PAL_EVENT_DPI_CHANGED: {
                    palLogInfo(PAL_NULL, "New DPI(%i)", event.data);
                    break;
                }
            }

        }

        // update and render
    }

    palDestroyWindow(window);
    palDestroyVideo(video);
    palDestroyEventDriver(eventDriver);
    return 0;
}