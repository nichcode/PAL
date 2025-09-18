
#include "pal/pal_video.h"
#include "tests.h"

bool monitorModeTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Monitor Mode Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalMonitorInfo info;
    Int32 count = 0;
    Int32 modeCount = 0;

    // initialize the video system
    PalResult result = palInitVideo(nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(
            nullptr,
            "Failed to initialize video %s",
            palFormatResult(result));
        return false;
    }

    // get the number of connected monitors
    result = palEnumerateMonitors(&count, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(
            nullptr,
            "Failed to get query monitors %s",
            palFormatResult(result));
        return false;
    }

    // if count == 0, we fail
    if (count == 0) {
        palLog(nullptr, "No monitor connected");
        return false;
    }

    palLog(nullptr, "Monitor Count: %d", count);
    // allocate an array of monitors or use a fixed array
    // Example: monitors* monitors[12].
    PalMonitor** monitors = nullptr;
    monitors = palAllocate(nullptr, sizeof(PalMonitor*) * count, 0);
    if (!monitors) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    // get the handle of the connected monitors
    result = palEnumerateMonitors(&count, monitors);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(
            nullptr,
            "Failed to get query monitors %s",
            palFormatResult(result));
        return false;
    }

    // get monitor info for every monitor and log the information
    for (Int32 i = 0; i < count; i++) {
        PalMonitor* monitor = monitors[i];
        result = palGetMonitorInfo(monitor, &info);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(
                nullptr,
                "Failed to get display info %s",
                palFormatResult(result));
            palFree(nullptr, monitors);
            return false;
        }

        // log monitor name
        palLog(nullptr, "Monitor Name: %s", info.name);

        // get number of monitor modes
        result = palEnumerateMonitorModes(monitor, &modeCount, nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(
                nullptr,
                "Failed to get query monitor modes %s",
                palFormatResult(result));
            return false;
        }

        palLog(nullptr, "Monitor Mode Count: %d", modeCount);
        // allocate an array of monitor modes or use a fixed array
        // Example: PalMonitorMode* monitorModes[12];
        PalMonitorMode* modes = nullptr;
        modes = palAllocate(nullptr, sizeof(PalMonitorMode) * modeCount, 0);
        if (!modes) {
            palLog(nullptr, "Failed to allocate memory");
            palFree(nullptr, monitors);
            return false;
        }

        // get the monitor modes
        result = palEnumerateMonitorModes(monitor, &modeCount, modes);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(
                nullptr,
                "Failed to get query monitor modes %s",
                palFormatResult(result));
            return false;
        }

        for (Int32 i = 0; i < modeCount; i++) {
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
        result = palGetCurrentMonitorMode(monitor, &current);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(
                nullptr,
                "Failed to get current monitor mode %s",
                palFormatResult(result));
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

    // free monitors array
    palFree(nullptr, monitors);
    return true;
}