
#include "tests.h"
#include "pal/pal_video.h"

PalResult displayTest()
{
    palLogConsoleInfo("");
    palLogConsoleInfo("===========================================");
    palLogConsoleInfo("Display Test");
    palLogConsoleInfo("===========================================");
    palLogConsoleInfo("");

    PalResult result;
    PalVideoInstance* videoInstance = PAL_NULL;
    PalDisplayInfo displayInfo;
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

    result = palEnumerateDisplays(videoInstance, &displayCount, PAL_NULL);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return PAL_RESULT_FAIL;
    }
    palLogConsoleInfo("Display Count: %i", displayCount);

    PalDisplay* displays[displayCount];
    result = palEnumerateDisplays(videoInstance, &displayCount, displays);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return PAL_RESULT_FAIL;
    }

    // get display info
    for (Uint32 i = 0; i < displayCount; i++) {
        PalDisplay* display = displays[i];
        result = palGetDisplayInfo(display, &displayInfo);
        if (result != PAL_RESULT_OK) {
            PalError error = palGetError();
            palLogConsoleError(palErrorToString(error));
            return PAL_RESULT_FAIL;
        }

        // log display info
        palLogConsoleInfo("Display: %s", displayInfo.name);
        palLogConsoleInfo(" Size: (%i, %i)", displayInfo.width, displayInfo.height);
        palLogConsoleInfo(" DPI Scale: (%.2f, %.2f)", displayInfo.dpiScaleX, displayInfo.dpiScaleY);
        palLogConsoleInfo(" RefreshRate: %i", displayInfo.refreshRate);
    }

    palDestroyVideoInstance(videoInstance);
    return result;
}