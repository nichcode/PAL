
#include "pal/pal_video.h"

void onEvent(void* userData, const PalEvent* event) {

    if (event->type == PAL_EVENT_WINDOW_RESIZE) {
        PalResizeEventInfo info = palGetResizeEventInfo(event);
        palLog("Size - (%d, %d)", info.width, info.height);
    }

    if (event->type == PAL_EVENT_WINDOW_MOVE) {
        PalMoveEventInfo info = palGetMoveEventInfo(event);
        palLog("Position - (%d, %d)", info.x, info.y);
    }
}

int main(int argc, char**) {

    const char* versionString = palGetVersionString();
    palLog("PAL version - %s", versionString);

    PalResult result;
    PalEventDriver* eventDriver = nullptr;
    PalWindow* window = nullptr;
    bool running = false;

    // event driver create info
    PalEventDriverCreateInfo eventDriverCreateInfo;
    eventDriverCreateInfo.allocator = nullptr; // default allocator
    eventDriverCreateInfo.callback = onEvent; // callback to recieve callback events (PAL_DISPATCH_CALLBACK)
    eventDriverCreateInfo.queue = nullptr; // optional user event queue to recieve queue eevents (PAL_DISPATCH_POLL)
    eventDriverCreateInfo.userData = nullptr; // for event callback (onEvent)

    // we want events, so we create an event driver to recieve events
    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        goto Failed;
    }

    // initialize the video system
    result = palInitVideo(nullptr, eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        goto Failed;
    }

    // get video features the OS supportes
    PalVideoFeatures videoFeatures = palGetVideoFeatures();

    // window create info
    PalWindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.display = nullptr; // use primary display
    windowCreateInfo.width = 640;
    windowCreateInfo.height = 480;
    windowCreateInfo.title = "Window";
    windowCreateInfo.flags = PAL_WINDOW_CENTER | PAL_WINDOW_DEFAULT;

    if (videoFeatures & PAL_VIDEO_FEATURE_HIGH_DPI) {
        // high DPI windows. If the dpi is called and this flag is set,
        // the window will automatically resize and move corresponding to the DPI
        // when no set, PAL_EVENT_DPI_CHANGED will be triggered if the feature is supported but the window stays the same
        windowCreateInfo.flags |= PAL_WINDOW_ALLOW_HIGH_DPI;
    }

    // create the window with the create info
    result = palCreateWindow(&windowCreateInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        goto Failed;
    }

    // set the dispatch modes for the event you are interested in
    // for this example, we will set window move and resize for callback mode
    // and the rest poll mode
    palSetEventDispatchMode(eventDriver, PAL_EVENT_WINDOW_CLOSE, PAL_DISPATCH_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_DPI_CHANGED, PAL_DISPATCH_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_DISPLAYS_CHANGED, PAL_DISPATCH_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_WINDOW_MOVE, PAL_DISPATCH_CALLBACK);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_WINDOW_RESIZE, PAL_DISPATCH_CALLBACK);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_MOUSE_MOVE, PAL_DISPATCH_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_MOUSE_WHEEL, PAL_DISPATCH_POLL);

    running = true;
    while (running) {
        // update video system to get video events
        palUpdateVideo();

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_WINDOW_CLOSE: {
                    running = false;
                    break;
                }

                case PAL_EVENT_DPI_CHANGED: {
                    palLog("DPI - %d", event.data);
                    break;
                }

                case PAL_EVENT_DISPLAYS_CHANGED: {
                    palLog("Display (monitor) List has been changed");
                    break;
                }

                case PAL_EVENT_MOUSE_MOVE: {
                    PalMoveEventInfo info = palGetMoveEventInfo(&event);
                    palLog("Mouse Position - (%i, %i)", info.x, info.y);
                    break;
                }

                case PAL_EVENT_MOUSE_WHEEL: {
                    PalWheelEventInfo info = palGetWheelEventInfo(&event);
                    palLog("Mouse wheel - (%i, %i)", info.scrollX, info.scrollY);
                    break;
                }
            }
        }

    }


    // cleanup
    palDestroyWindow(window);
    palShutdownVideo();
    palDestroyEventDriver(eventDriver);

    return 0;

    // a goto for failure. No need to duplicate that result check
    Failed:
        const char* resultString = palResultToString(result);
        palLog("PAL Error - %s", resultString);
        return -1;
}