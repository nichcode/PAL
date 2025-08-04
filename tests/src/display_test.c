
#include "pal/pal_video.h"

void displayTest() {

    palLog("");
    palLog("===========================================");
    palLog("Display Test");
    palLog("===========================================");
    palLog("");

    PalResult result;
    PalVideoSystem* video;
    PalDisplayInfo displayInfo;
    int displayCount = 0;

    PalVideoSystemCreateInfo createInfo;
    createInfo.allocator = nullptr;

    result = palCreateVideoSystem(&createInfo, &video);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog(resultString);
        return;
    }

    result = palEnumerateDisplays(video, &displayCount, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog(resultString);
        return;
    }

    palLog("Display Count: %d", displayCount);

    PalDisplay* displays[displayCount];
    result = palEnumerateDisplays(video, &displayCount, displays);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog(resultString);
        return;
    }

    // get display info
    for (int i = 0; i < displayCount; i++) {
        PalDisplay* display = displays[i];
        result = palGetDisplayInfo(display, &displayInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLog(resultString);
            return;
        }

        // log display info
        palLog("Display Name: %s", displayInfo.name);
        palLog(" Size: (%i, %i)", displayInfo.width, displayInfo.height);
        palLog(" DPI: %i", displayInfo.dpi);
        palLog(" RefreshRate: %i", displayInfo.refreshRate);

        const char* boolToString;
        if (displayInfo.primary) {
            boolToString = "True";
        } else {
            boolToString = "False";
        }
        palLog(" Primary: %s", boolToString);
    }

    palDestroyVideoSystem(video);
}