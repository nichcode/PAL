
#include "tests.h"
#include "pal/pal_video.h"

PalResult windowTest()
{
    palLogConsoleInfo("");
    palLogConsoleInfo("===========================================");
    palLogConsoleInfo("Window Test");
    palLogConsoleInfo("===========================================");
    palLogConsoleInfo("");

    PalResult result;
    PalVideoInstance* videoInstance = PAL_NULL;
    PalWindow* window = PAL_NULL;
    PalDisplay* primaryDisplay = PAL_NULL;
    Uint32 displayCount = 0;

    PalVideoInstanceDesc desc;
    desc.allocator = PAL_NULL;
    desc.eventinstance = PAL_NULL;

    result = palCreateVideoInstance(&desc, &videoInstance);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return PAL_RESULT_FAIL;
    }

    // get connected displays(monitors)
    result = palEnumerateDisplays(videoInstance, &displayCount, PAL_NULL);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return PAL_RESULT_FAIL;
    }

    // enumerate and get a display. 
    // set display count to 1 to get only primary display.
    // Or pass in PAL_NULL primary display
    PalDisplay* displays[displayCount];
    result = palEnumerateDisplays(videoInstance, &displayCount, displays);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return PAL_RESULT_FAIL;
    }

    // we are selecting the primary display
    // we can use palGetPrimaryDisplay to get it or use index 0 in the display array
    palGetPrimaryDisplay(videoInstance, &primaryDisplay);
    //primaryDisplay = displays[0];

    // window descriptor
    PalWindowDesc windowDesc;
    windowDesc.display = primaryDisplay;
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

    bool running = PAL_TRUE;
    while (running) {
        palUpdateWindows(videoInstance);
    }

    palDestroyWindow(window);
    palDestroyVideoInstance(videoInstance);
    return PAL_RESULT_OK;
}