
#include "tests.h"
#include "pal/pal_video.h"

bool displayTest() {

    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Display Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalResult result;
    PalDisplayInfo info;
    Int32 count = 0;

    // initialize the video system
    result = palInitVideo(nullptr, nullptr);
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
    // Example: PalDisplay* displays[12];
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

        // log display info
        palLog(nullptr, "Display Name: %s", info.name);
        palLog(nullptr, " Size: (%d, %d)", info.width, info.height);
        palLog(nullptr, " DPI: %d", info.dpi);
        palLog(nullptr, " RefreshRate: %d", info.refreshRate);

        // check if the display is the primary display
        const char* boolToString;
        if (info.primary) {
            boolToString = "True";
        } else {
            boolToString = "False";
        }
        palLog(nullptr, " Primary: %s", boolToString);

        // convert display orientation to string
        const char* orientationToString;
        switch (info.orientation) {
            case PAL_ORIENTATION_LANDSCAPE:
            orientationToString = "Landscape";
            break;

            case PAL_ORIENTATION_PORTRAIT:
            orientationToString = "Portrait";
            break;

            case PAL_ORIENTATION_LANDSCAPE_FLIPPED:
            orientationToString = "Landscape Flipped";
            break;

            case PAL_ORIENTATION_PORTRAIT_FLIPPED:
            orientationToString = "Portrait Flipped";
            break;
        }
        palLog(nullptr, " Orientation: %s", orientationToString);
    }

    // shutdown the video system
    palShutdownVideo();

    // free displays array
    palFree(nullptr, displays);
    return true;
}