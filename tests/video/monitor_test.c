
#include "pal/pal_video.h"
#include "tests.h"

PalBool monitorTest()
{
    // initialize the video system
    PalResult result = palInitVideo(nullptr, nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to initialize video");
        return PAL_FALSE;
    }

    // get the number of connected monitors
    uint32_t count = 0;
    result = palEnumerateMonitors(&count, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to get query monitors");
        return PAL_FALSE;
    }

    // if count == 0, we fail
    if (count == 0) {
        palLog(nullptr, "No monitor connected");
        return PAL_FALSE;
    }

    palLog(nullptr, "Monitor Count: %d", count);
    // allocate an array of monitors or use a fixed array
    // Example: PalMonitor* monitors[12];
    PalMonitor** monitors = nullptr;
    monitors = palAllocate(nullptr, sizeof(PalMonitor*) * count, 0);
    if (!monitors) {
        palLog(nullptr, "Failed to allocate memory");
        return PAL_FALSE;
    }

    // get the handle of the connected monitors
    result = palEnumerateMonitors(&count, monitors);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to get query monitors");
        return PAL_FALSE;
    }

    // get monitor info for every monitor and log the information
    PalMonitorInfo info = {0};
    for (int32_t i = 0; i < count; i++) {
        PalMonitor* monitor = monitors[i];
        palGetMonitorInfo(monitor, &info);

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

    return PAL_TRUE;
}
