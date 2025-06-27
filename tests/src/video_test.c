
#include "tests.h"
#include "pal/pal_video.h"
#include "pal/pal_events.h"

void onEvent(const PalEvent* event)
{
    if (event->type == PAL_EVENT_WINDOW_RESIZE) {
        palLogInfo("Size (%i, %i)", event->size.width, event->size.height);
    }
}

bool videoTest(void* data)
{
    PalError error = PAL_ERROR_NONE;
    if (!palInitVideoSystem(PAL_NULL)) {
        error = palGetError();
        palLogError(palFormatError(error));
        return PAL_FALSE;
    }

    const PalDisplay* display = palGetPrimaryDisplay();
    PalWindowDesc desc;
    desc.displayIndex = display->index;
    desc.flags = PAL_APPWINDOW | PAL_WINDOW_CENTER;
    desc.width = 640;
    desc.height = 480;
    desc.title = "PAL Window";
    PalWindow* window = palCreateWindow(&desc);

    if (!window) {
        error = palGetError();
        palLogError(palFormatError(error));
        return PAL_FALSE;
    }

    // register your interested events. you can check for return value
    palRegisterEvent(PAL_EVENT_QUIT, PAL_DISPATCH_POLL);
    palRegisterEvent(PAL_EVENT_WINDOW_RESIZE, PAL_DISPATCH_CALLBACK);

    // set event callback for callback dispatch
    palSetEventCallback(onEvent);

    bool running = PAL_TRUE;
    while (running) {
        PalEvent event;
        while (palPollEvent(&event)) {
            // we got an event.
            // Now if you haven't registered an event you won't be notified if it happens
            switch (event.type) {
                case PAL_EVENT_QUIT: {
                    // quit event
                    // optional, check the window
                    PalWindowID id = palGetWindowID(window);
                    if (id == event.windowID) {
                        running = PAL_FALSE;
                    }
                }

                case PAL_EVENT_WINDOW_RESIZE: {
                    // resize event. We handle as a callback for real time events
                    break;
                }
            }
        }
        
        // you can event put event handling into a different function
        // update
        // render
    }

    // cleanup
    palDestroyWindow(window);
    palShutdownVideoSystem();
    return PAL_TRUE;
}