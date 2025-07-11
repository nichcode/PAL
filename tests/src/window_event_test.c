
#include "tests.h"
#include "pal/pal_event.h"
#include "pal/pal_video.h"

PalResult windowEventTest()
{
    palLogConsoleInfo("");
    palLogConsoleInfo("===========================================");
    palLogConsoleInfo("Window Event Test");
    palLogConsoleInfo("===========================================");
    palLogConsoleInfo("");

    PalResult result;
    PalVideoInstance* videoInstance = PAL_NULL;
    PalEventInstance* eventInstance = PAL_NULL;
    PalWindow* window = PAL_NULL;

    // create an event instance
    PalEventInstanceDesc eventInstanceDesc;
    eventInstanceDesc.allocator = PAL_NULL;
    eventInstanceDesc.callback = PAL_NULL; // for callback event mode
    eventInstanceDesc.queue = PAL_NULL; // use internal event queue

    result = palCreateEventInstance(&eventInstanceDesc, &eventInstance);
    if (result == PAL_RESULT_FAIL) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return PAL_RESULT_FAIL;
    }

    PalVideoInstanceDesc videoInstanceDesc;
    videoInstanceDesc.allocator = PAL_NULL;
    videoInstanceDesc.eventInstance = eventInstance;

    result = palCreateVideoInstance(&videoInstanceDesc, &videoInstance);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return PAL_RESULT_FAIL;
    }

    // window descriptor
    PalWindowDesc windowDesc;
    windowDesc.display = PAL_NULL; // use primary display
    windowDesc.flags = PAL_APPWINDOW | PAL_WINDOW_CENTER;
    windowDesc.title = "Pal Window";
    windowDesc.height = 480;
    windowDesc.width = 640;
    
    // create window with descriptor
    result = palCreateWindow(videoInstance, &windowDesc, &window);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return PAL_RESULT_FAIL;
    }

    // register for window close event
    palEnableEvent(eventInstance, PAL_EVENT_QUIT, PAL_MODE_POLL);

    bool running = PAL_TRUE;
    while (running) {
        // update all windows to get events
        palUpdateWindows(videoInstance);

        PalEvent event;
        while (palPollEvent(eventInstance, &event) == PAL_RESULT_OK) {
            if (event.type == PAL_EVENT_QUIT) {
                // optional, check the window id
                Uint32 id = 0;
                palGetWindowID(window, &id);
                if (event.id == id) {
                    running = PAL_FALSE;
                }
            }
        }
    }

    palDestroyWindow(window);
    palDestroyEventInstance(eventInstance);
    palDestroyVideoInstance(videoInstance);
    return PAL_RESULT_OK;
}