
#include "tests.h"
#include "pal/pal_video.h"

bool monitorTest() {

    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Monitor Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalResult result;
    PalMonitorInfo info;
    Int32 count = 0;

    // initialize the video system
    result = palInitVideo(nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to initialize video %s", palFormatResult(result));
        return false;
    }

    // get the number of connected monitors
    result = palEnumerateMonitors(&count, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to get query monitors %s", palFormatResult(result));
        return false;
    }

    // if count == 0, we fail 
    if (count == 0) {
        palLog(nullptr, "No monitor connected");
        return false;
    }

    palLog(nullptr, "Monitor Count: %d", count);
    // allocate an array of monitors or use a fixed array
    // Example: PalMonitor* monitors[12];
    PalMonitor** monitors = palAllocate(nullptr, sizeof(PalMonitor*) * count, 0);
    if (!monitors) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    // get the handle of the connected monitors
    result = palEnumerateMonitors(&count, monitors);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to get query monitors %s", palFormatResult(result));
        return false;
    }

    // get monitor info for every monitor and log the information
    for (Int32 i = 0; i < count; i++) {
        PalMonitor* monitor = monitors[i];
        result = palGetMonitorInfo(monitor, &info);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to get monitor info %s", palFormatResult(result));
            palFree(nullptr, monitors);
            return false;
        }

        // log monitor info
        palLog(nullptr, "Monitor Name: %s", info.name);
        palLog(nullptr, " Size: (%d, %d)", info.width, info.height);
        palLog(nullptr, " DPI: %d", info.dpi);
        palLog(nullptr, " RefreshRate: %d", info.refreshRate);

        // check if the monitor is the primary monitor
        const char* boolToString;
        if (info.primary) {
            boolToString = "True";
        } else {
            boolToString = "False";
        }
        palLog(nullptr, " Primary: %s", boolToString);

        // convert monitor orientation to string
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

    // free monitors array
    palFree(nullptr, monitors);
    return true;
}