
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
    PalVideo* video = PAL_NULL;
    PalWindow* window = PAL_NULL;
    PalDisplay* primaryDisplay = PAL_NULL;
    Uint32 displayCount = 0;

    result = palCreateVideo(PAL_NULL, &video);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return -1;
    }

    // get connected displays(monitors)
    result = palEnumerateDisplays(video, &displayCount, PAL_NULL);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return -1;
    }

    // enumerate and get a display. 
    // set display count to 1 to get only primary display.
    // Or pass in PAL_NULL primary display
    PalDisplay* displays[displayCount];
    result = palEnumerateDisplays(video, &displayCount, displays);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return -1;
    }

    // we are selecting the primary display
    primaryDisplay = displays[0];

    // window descriptor
    PalWindowDesc windowDesc;
    windowDesc.display = primaryDisplay;
    windowDesc.flags = PAL_APPWINDOW | PAL_WINDOW_CENTER;
    windowDesc.title = "Pal Window";
    windowDesc.height = 480;
    windowDesc.width = 640;
    
    // create window with descriptor
    result = palCreateWindow(video, &windowDesc, &window);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return -1;
    }

    bool running = PAL_TRUE;
    while (running) {
        palUpdateWindows(video);
    }

    palDestroyWindow(window);
    palDestroyVideo(video);
    return PAL_RESULT_OK;
}