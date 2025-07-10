
#include "tests.h"
#include "pal/pal_video.h"

PalResult displayTest()
{
    PalResult result;
    PalVideo* video;
    PalDisplayInfo displayInfo;
    Uint32 displayCount = 0;

    result = palCreateVideo(PAL_NULL, &video);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return -1;
    }

    result = palEnumerateDisplays(video, &displayCount, PAL_NULL);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return -1;
    }
    palLogConsoleInfo("Display Count: %i", displayCount);

    PalDisplay* displays[displayCount];
    result = palEnumerateDisplays(video, PAL_NULL, displays);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return -1;
    }

    // get display info
    for (Uint32 i = 0; i < displayCount; i++) {
        PalDisplay* display = displays[i];
        result = palGetDisplayInfo(display, &displayInfo);
        if (result != PAL_RESULT_OK) {
            PalError error = palGetError();
            palLogConsoleError(palErrorToString(error));
            return -1;
        }

        // log display info

        palLogConsoleInfo("Display: %s", displayInfo.name);
        palLogConsoleInfo("  Size: (%i, %i)", displayInfo.width, displayInfo.height);
        palLogConsoleInfo("  DPI Scale: (%.2f, %.2f)", displayInfo.dpiScaleX, displayInfo.dpiScaleY);
        palLogConsoleInfo("  RefreshRate: %i", displayInfo.refreshRate);
    }

    palDestroyVideo(video);
    return result;
}