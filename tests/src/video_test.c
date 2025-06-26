
#include "tests.h"
#include "pal/pal_video.h"

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

    //palIsWindowFullScreen(window);

    // while loop

    // cleanup
    palDestroyWindow(window);
    palShutdownVideoSystem();
    return PAL_TRUE;
}