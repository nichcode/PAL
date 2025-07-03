
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

    bool running = PAL_TRUE;
    while (running) {
        palUpdateWindows();
    }

    palDestroyWindow(window);
    palShutdownVideo();
    return PAL_TRUE;
}