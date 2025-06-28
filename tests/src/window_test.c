
#include "tests.h"

bool windowTest()
{
    PalError error = PAL_ERROR_NONE;
    if (!palInitVideo(PAL_NULL)) {
        error = palGetError();
        palLogError(palFormatError(error));
        return PAL_FALSE;
    }

    int displayCount = palGetDisplayCount();
    if (!displayCount) { 
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

    if (!palInitEvent(PAL_NULL)) {
        error = palGetError();
        palLogError(palFormatError(error));
        return PAL_FALSE;
    }

    // event system must be initialized
    if (!palRegisterEvent(PAL_EVENT_QUIT, PAL_DISPATCH_POLL)) {
        error = palGetError();
        palLogError(palFormatError(error));
        return PAL_FALSE;
    }

    bool running = PAL_TRUE;
    while (running) {
        PalEvent event;
        while (palPollEvent(&event)) {
            switch (event.type) {
                case PAL_EVENT_QUIT: {
                    running = PAL_FALSE;
                }
            }
        }

        // update
    }

    palDestroyWindow(window);

    palShutdownVideo();
    palShutdownEvent();
    return PAL_TRUE;
}