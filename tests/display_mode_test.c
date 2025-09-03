
#include "tests.h"
#include "pal/pal_video.h"

bool displayModeTest() {

    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Display Mode Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalResult result;
    PalDisplayInfo info;
    Int32 count = 0;
    Int32 modeCount = 0;

    // initialize the video system
    result = palInitVideo(nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to initialize video %s", palFormatResult(result));
        return false;
    }

    // get the number of connected displays (monitors
    result = palEnumerateDisplays(&count, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to get query displays %s", palFormatResult(result));
        return false;
    }

    // if count == 0, we fail 
    if (count == 0) {
        palLog(nullptr, "No display (monitor) connected");
        return false;
    }

    palLog(nullptr, "Display Count: %d", count);
    // allocate an array of displays or use a fixed array
    // Example: displays* displays[12].
    PalDisplay** displays = palAllocate(nullptr, sizeof(PalDisplay*) * count, 0);
    if (!displays) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    // get the handle of the connected displays (monitors)
    result = palEnumerateDisplays(&count, displays);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to get query displays %s", palFormatResult(result));
        return false;
    }

    // get display info for every display and log the information
    for (Int32 i = 0; i < count; i++) {
        PalDisplay* display = displays[i];
        result = palGetDisplayInfo(display, &info);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to get display info %s", palFormatResult(result));
            palFree(nullptr, displays);
            return false;
        }

        // log display name
        palLog(nullptr, "Display Name: %s", info.name);
       
    
        // get number of display modes
        result = palEnumerateDisplayModes(display, &modeCount, nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to get query display modes %s", palFormatResult(result));
            return false;
        }

        palLog(nullptr, "Display Mode Count: %d", modeCount);
        // allocate an array of display modes or use a fixed array
        // Example: PalDisplayMode* displayModes[12];
        PalDisplayMode* modes = palAllocate(nullptr, sizeof(PalDisplayMode) * modeCount, 0);
        if (!modes) {
            palLog(nullptr, "Failed to allocate memory");
            palFree(nullptr, displays);
            return false;
        }

        // get the display modes
        result = palEnumerateDisplayModes(display, &modeCount, modes);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to get query display modes %s", palFormatResult(result));
            return false;
        }

        for (Int32 i = 0; i < modeCount; i++) {
            // log display mode
            PalDisplayMode* mode = &modes[i];
            palLog(nullptr, " Mode Index: %d", i);
            palLog(nullptr, "  Size: (%d, %d)", mode->width, mode->height);
            palLog(nullptr, "  RefreshRate: %d", mode->refreshRate);
            palLog(nullptr, "  Bits Per Pixel: %d", mode->bpp);
            palLog(nullptr, "");
        }

        palFree(nullptr, modes);

        // get current display mode and log
        PalDisplayMode current;
        result = palGetCurrentDisplayMode(display, &current);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to get current display mode %s", palFormatResult(result));
            return false;
        }

        palLog(nullptr, "");
        palLog(nullptr, " Current Mode:");
        palLog(nullptr, "  Size: (%d, %d)", current.width, current.height);
        palLog(nullptr, "  RefreshRate: %d", current.refreshRate);
        palLog(nullptr, "  Bits Per Pixel: %d", current.bpp);
    }

    // shutdown the video system
    palShutdownVideo();

    // free displays array
    palFree(nullptr, displays);
    return true;
}