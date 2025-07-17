
#include "tests.h"
#include "pal/pal_video.h"

void displayModeTest()
{
    palLogInfo(PAL_NULL, "");
    palLogInfo(PAL_NULL, "===========================================");
    palLogInfo(PAL_NULL, "Display Mode Test");
    palLogInfo(PAL_NULL, "===========================================");
    palLogInfo(PAL_NULL, "");

    PalResult result;
    PalVideo* video = PAL_NULL;
    PalDisplayInfo displayInfo;
    PalDisplayMode displayMode;
    int displayCount = 0;
    int modeCount = 0;

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
    for (int i = 0; i < displayCount; i++) {
        PalDisplay* display = displays[i];
        result = palGetDisplayInfo(display, &displayInfo);
        if (result != PAL_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLogError(PAL_NULL, resultString);
            return;
        }

        // log display name
        palLogInfo(PAL_NULL, "Display Name: %s", displayInfo.name);

        // get display mode info
        result = palEnumerateDisplayModes(display, &modeCount, PAL_NULL);
        if (result != PAL_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLogError(PAL_NULL, resultString);
            return;
        }

        PalDisplayMode modes[modeCount];
        result = palEnumerateDisplayModes(display, &modeCount, modes);
        if (result != PAL_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLogError(PAL_NULL, resultString);
            return;
        }

        for (int i = 0; i < modeCount; i++) {
            // log display mode
            PalDisplayMode* mode = &modes[i];
            palLogInfo(PAL_NULL, " Mode Index: %i", i);
            palLogInfo(PAL_NULL, "  Size: (%i, %i)", mode->width, mode->height);
            palLogInfo(PAL_NULL, "  RefreshRate: %i", mode->refreshRate);
            palLogInfo(PAL_NULL, "  Red Bits: %i", mode->redBits);
            palLogInfo(PAL_NULL, "  Green Bits: %i", mode->greenBits);
            palLogInfo(PAL_NULL, "  Blue Bits: %i", mode->blueBits);
            palLogInfo(PAL_NULL, "  Alpha Bits: %i", mode->alphaBits);
            palLogInfo(PAL_NULL, "");
        }
    }

    palDestroyVideo(video);
}