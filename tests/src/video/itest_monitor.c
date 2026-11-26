
#include "pal2/core/memory.h"
#include "pal2/core/log.h"
#include "pal2/video/init.h"
#include "pal2/video/monitor.h"

static void logMonitorMode(PalMonitorMode* mode, uint32_t index)
{
    if (index == UINT32_MAX) {
        palLog(nullptr, " Current Mode:");
        palLog(nullptr, "  Size: (%u, %u)", mode->width, mode->height);
        palLog(nullptr, "  RefreshRate: %u", mode->refreshRate);
        palLog(nullptr, "  Bits Per Pixel: %u", mode->bpp);

    } else {
        palLog(nullptr, "  Mode Index: %u", index);
        palLog(nullptr, "   Size: (%u, %u)", mode->width, mode->height);
        palLog(nullptr, "   RefreshRate: %u", mode->refreshRate);
        palLog(nullptr, "   Bits Per Pixel: %u", mode->bpp);
    }

    palLog(nullptr, "");
}

static void logMonitorInfo(const PalMonitorInfo* info)
{
    palLog(nullptr, "Monitor Name: %s", info->name);
    palLog(nullptr, " Size: (%d, %d)", info->width, info->height);
    palLog(nullptr, " DPI: %d", info->dpi);
    palLog(nullptr, " RefreshRate: %d", info->refreshRate);

    const char* boolToString = "False";
    if (info->primary) {
        boolToString = "True";
    }
    palLog(nullptr, " Primary: %s", boolToString);

    const char* orientationToString = "Landscape";
    switch (info->orientation) {
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

int main(void)
{
    palLog(nullptr, "Running monitor itest...");

    PalMonitor** monitors = nullptr;
    uint32_t count = 0;
    PalResult result = PAL_RESULT_SUCCESS;
    int ret = 0;

    result = palInitVideo(nullptr, nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to initialize video system");
        return -1;
    }

    result = palEnumerateMonitors(&count, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to query monitors");
        ret = -1;
        goto cleanup;
    }

    if (count == 0) {
        palLog(nullptr, "No monitor connected");
        ret = -1;
        goto cleanup;
    }

    palLog(nullptr, "Monitor Count: %u", count);

    monitors = palAllocate(nullptr, sizeof(PalMonitor*) * count, 0);
    if (!monitors) {
        palLog(nullptr, "Failed to allocate memory");
        ret = -1;
        goto cleanup;
    }

    result = palEnumerateMonitors(&count, monitors);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to query monitors");
        ret = -1;
        goto cleanup;
    }

    for (uint32_t i = 0; i < count; i++) {
        PalMonitor* monitor = monitors[i];
        PalMonitorInfo info = {0};
        palGetMonitorInfo(monitor, &info);
        logMonitorInfo(&info);

        uint32_t modeCount = 0;
        palEnumerateMonitorModes(monitor, &modeCount, nullptr);
        palLog(nullptr, " Mode Count: %u", modeCount);

        PalMonitorMode* modes = nullptr;
        modes = palAllocate(nullptr, sizeof(PalMonitorMode) * modeCount, 0);
        if (!modes) {
            palLog(nullptr, "Failed to allocate memory");
            ret = -1;
            goto cleanup;
        }

        palEnumerateMonitorModes(monitor, &modeCount, modes);

        for (uint32_t j = 0; j < modeCount; j++) {
            PalMonitorMode* mode = &modes[j];
            logMonitorMode(mode, j);
        }

        palFree(nullptr, modes);

        PalMonitorMode current;
        palGetCurrentMonitorMode(monitor, &current);
        logMonitorMode(&current, UINT32_MAX);
    }

cleanup:
    if (monitors) {
        palFree(nullptr, monitors);
    }

    palShutdownVideo();
    return ret;
}