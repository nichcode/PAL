
#include "pal/pal_video.h"

void displayModeTest() {

    palLog("");
    palLog("===========================================");
    palLog("Display Mode Test");
    palLog("User a debugger to see everything");
    palLog("===========================================");
    palLog("");

    PalResult result;
    PalDisplayInfo displayInfo;
    PalDisplayMode displayMode;
    int displayCount = 0;
    int modeCount = 0;

    result = palInitVideo(nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog(resultString);
        return;
    }

    result = palEnumerateDisplays(&displayCount, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog(resultString);
        return;
    }

    palLog("Display Count: %d", displayCount);

    PalDisplay* displays[displayCount];
    result = palEnumerateDisplays(&displayCount, displays);
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

        palLog("Display Name: %s", displayInfo.name);

        // get display mode info
        result = palEnumerateDisplayModes(display, &modeCount, nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLog(resultString);
            return;
        }

        palLog("Mode Count: %d", modeCount);

        PalDisplayMode modes[modeCount];
        result = palEnumerateDisplayModes(display, &modeCount, modes);
        if (result != PAL_RESULT_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLog(resultString);
            return;
        }

        for (int i = 0; i < modeCount; i++) {
            // log display mode
            PalDisplayMode* mode = &modes[i];
            palLog(" Mode Index: %i", i);
            palLog("  Size: (%i, %i)", mode->width, mode->height);
            palLog("  RefreshRate: %i", mode->refreshRate);
            palLog("  Bits Per Pixel: %i", mode->bpp);
            palLog("");
        }

        // get current display mode
        PalDisplayMode currentDisplayMode, resetDisplayMode;
        palGetCurrentDisplayMode(display, &currentDisplayMode);
        if (result != PAL_RESULT_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLog(resultString);
            return;
        }

        palLog("");
        palLog(" Current Display Mode:");
        palLog("  Size: (%i, %i)", currentDisplayMode.width, currentDisplayMode.height);
        palLog("  RefreshRate: %i", currentDisplayMode.refreshRate);
        palLog("  Bits Per Pixel: %i", currentDisplayMode.bpp);
        palLog("");

        resetDisplayMode = currentDisplayMode;

        // validate a display mode
        PalDisplayMode newDisplayMode;
        newDisplayMode.bpp = 32;
        newDisplayMode.height = 720;
        newDisplayMode.width = 1280;
        newDisplayMode.refreshRate = 60;
        palValidateDisplayMode(display, &newDisplayMode);
        if (result != PAL_RESULT_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLog(resultString);
            return;
        }

        palLog("");
        palLog(" Validated Display Mode:");
        palLog("  Size: (%i, %i)", newDisplayMode.width, newDisplayMode.height);
        palLog("  RefreshRate: %i", newDisplayMode.refreshRate);
        palLog("  Bits Per Pixel: %i", newDisplayMode.bpp);
        palLog("");

        // set current display mode to the validated mode
        palLog("Setting Validated Display Mode");
        palSetDisplayMode(display, &newDisplayMode);
        if (result != PAL_RESULT_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLog(resultString);
            return;
        }

        palGetCurrentDisplayMode(display, &currentDisplayMode);
        if (result != PAL_RESULT_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLog(resultString);
            return;
        }

        palLog("");
        palLog(" Current Display Mode:");
        palLog("  Size: (%i, %i)", currentDisplayMode.width, currentDisplayMode.height);
        palLog("  RefreshRate: %i", currentDisplayMode.refreshRate);
        palLog("  Bits Per Pixel: %i", currentDisplayMode.bpp);
        palLog("");

        // reset display mode
        palLog("Display Mode Reset");
        palSetDisplayMode(display, &resetDisplayMode);
        if (result != PAL_RESULT_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLog(resultString);
            return;
        }

        palGetCurrentDisplayMode(display, &currentDisplayMode);
        if (result != PAL_RESULT_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLog(resultString);
            return;
        }

        palLog("");
        palLog(" Current Display Mode:");
        palLog("  Size: (%i, %i)", currentDisplayMode.width, currentDisplayMode.height);
        palLog("  RefreshRate: %i", currentDisplayMode.refreshRate);
        palLog("  Bits Per Pixel: %i", currentDisplayMode.bpp);
        palLog("");

        // changing orientation to portrait
        palLog("Setting orientation to portrait");
        palSetDisplayOrientation(display, PAL_ORIENTATION_PORTRAIT);
        if (result != PAL_RESULT_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLog(resultString);
            return;
        }

        // changing orientation to landscape
        palLog("Setting orientation to landscape");
        palSetDisplayOrientation(display, PAL_ORIENTATION_LANDSCAPE);
        if (result != PAL_RESULT_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLog(resultString);
            return;
        }
    }

    palShutdownVideo();
}