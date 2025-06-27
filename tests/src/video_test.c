
#include "tests.h"
#include "pal/pal_video.h"
#include "pal/pal_events.h"

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
    palRegisterEvent(PAL_EVENT_QUIT, PAL_EVENT_DISPATCH_POLL);

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