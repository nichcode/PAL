
#include "pal/pal_video.h"
#include "tests.h"

PalBool monitorModeTest()
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
    // Example: monitors* monitors[12].
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
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to get monitor info");
            return PAL_FALSE;
        }

        // log monitor name
        palLog(nullptr, "Monitor Name: %s", info.name);

        // get number of monitor modes
        uint32_t modeCount = 0;
        palEnumerateMonitorModes(monitor, &modeCount, nullptr);

        palLog(nullptr, "Monitor Mode Count: %d", modeCount);
        // allocate an array of monitor modes or use a fixed array
        // Example: PalMonitorMode* monitorModes[12];
        PalMonitorMode* modes = nullptr;
        modes = palAllocate(nullptr, sizeof(PalMonitorMode) * modeCount, 0);
        if (!modes) {
            palLog(nullptr, "Failed to allocate memory");
            palFree(nullptr, monitors);
            return PAL_FALSE;
        }

        // get the monitor modes
        palEnumerateMonitorModes(monitor, &modeCount, modes);

        for (int32_t i = 0; i < modeCount; i++) {
            // log monitor mode
            PalMonitorMode* mode = &modes[i];
            palLog(nullptr, " Mode Index: %d", i);
            palLog(nullptr, "  Size: (%d, %d)", mode->width, mode->height);
            palLog(nullptr, "  RefreshRate: %d", mode->refreshRate);
            palLog(nullptr, "  Bits Per Pixel: %d", mode->bpp);
            palLog(nullptr, "");
        }

        palFree(nullptr, modes);

        // get current monitor mode and log
        PalMonitorMode current;
        palGetCurrentMonitorMode(monitor, &current);

        palLog(nullptr, "");
        palLog(nullptr, " Current Mode:");
        palLog(nullptr, "  Size: (%d, %d)", current.width, current.height);
        palLog(nullptr, "  RefreshRate: %d", current.refreshRate);
        palLog(nullptr, "  Bits Per Pixel: %d", current.bpp);
        palLog(nullptr, "");
    }

    // shutdown the video system
    palShutdownVideo();

    // free monitors array
    palFree(nullptr, monitors);

    return PAL_TRUE;
}
