
#include "tests.h"
#include "pal/pal_video.h"

void displayTest()
{
    palLogInfo(PAL_NULL, "");
    palLogInfo(PAL_NULL, "===========================================");
    palLogInfo(PAL_NULL, "Display Test");
    palLogInfo(PAL_NULL, "===========================================");
    palLogInfo(PAL_NULL, "");

    PalResult result;
    PalVideo* video = PAL_NULL;
    PalDisplayInfo displayInfo;
    int displayCount = 0;

    PalVideoConfig videoConfig;
    videoConfig.allocator = PAL_NULL;

    result = palCreateVideo(&videoConfig, &video);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return;
    }

    result = palEnumerateDisplays(video, &displayCount, PAL_NULL);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return;
    }

    palLogInfo(PAL_NULL, "Display Count: %i", displayCount);

    PalDisplay* displays[displayCount];
    result = palEnumerateDisplays(video, &displayCount, displays);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return;
    }

    // get display info
    for (Uint32 i = 0; i < displayCount; i++) {
        PalDisplay* display = displays[i];
        result = palGetDisplayInfo(display, &displayInfo);
        if (result != PAL_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLogError(PAL_NULL, resultString);
            return;
        }

        // log display info
        palLogInfo(PAL_NULL, "Display Name: %s", displayInfo.name);
        palLogInfo(PAL_NULL, " Size: (%i, %i)", displayInfo.width, displayInfo.height);
        palLogInfo(PAL_NULL, " DPI Scale: (%.2f, %.2f)", displayInfo.dpiScaleX, displayInfo.dpiScaleY);
        palLogInfo(PAL_NULL, " RefreshRate: %i", displayInfo.refreshRate);
    }

    palDestroyVideo(video);
}