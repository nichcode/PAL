
#include "tests.h"
#include "pal2/pal_video.h"

#if PAL_HAS_VIDEO_MODULE

static const char* orientationToString(PalOrientation orientation)
{
    switch (orientation) {
        case PAL_ORIENTATION_LANDSCAPE:
            return "Landscape";

        case PAL_ORIENTATION_PORTRAIT:
            return "Portrait";

        case PAL_ORIENTATION_LANDSCAPE_FLIPPED:
            return "Landscape Flipped";

        case PAL_ORIENTATION_PORTRAIT_FLIPPED:
            return "Portrait Flipped";
    }
    return nullptr;
}

PalBool monitorTest()
{
    PalResult result = palInitVideo(nullptr, nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    PalVideoFeatures features = palGetVideoFeatures();

    uint32_t count = 0;
    result = palEnumerateMonitors(&count, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    if (count == 0) {
        return PAL_FALSE;
    }
    palLog(nullptr, "Monitor Count: %lu", count);

    PalMonitor** monitors = nullptr;
    monitors = palAllocate(nullptr, sizeof(PalMonitor*) * count, 0);
    if (!monitors) {
        return PAL_FALSE;
    }

    result = palEnumerateMonitors(&count, monitors);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

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
        palLog(nullptr, " Orientation: %s", orientationToString(info.orientation));

        uint32_t modeCount = 0;
        palEnumerateMonitorModes(monitor, &modeCount, nullptr);
        palLog(nullptr, "");
        palLog(nullptr, " Mode Count: %d", modeCount);

        PalMonitorMode* modes = nullptr;
        modes = palAllocate(nullptr, sizeof(PalMonitorMode) * modeCount, 0);
        if (!modes) {
            palLog(nullptr, "Failed to allocate memory");
            palFree(nullptr, monitors);
            return PAL_FALSE;
        }

        palEnumerateMonitorModes(monitor, &modeCount, modes);
        for (int32_t i = 0; i < modeCount; i++) {
            // log monitor mode
            PalMonitorMode* mode = &modes[i];
            palLog(nullptr, "  Mode Index: %d", i);
            palLog(nullptr, "   Size: (%d, %d)", mode->width, mode->height);
            palLog(nullptr, "   RefreshRate: %d", mode->refreshRate);
            palLog(nullptr, "   Bits Per Pixel: %d", mode->bpp);
            palLog(nullptr, "");
        }

        palFree(nullptr, modes);

        if (features & PAL_VIDEO_FEATURE_MONITOR_GET_MODE) {
            PalMonitorMode current;
            palGetCurrentMonitorMode(monitor, &current);
            palLog(nullptr, "");
            palLog(nullptr, "  Current Mode:");
            palLog(nullptr, "   Size: (%d, %d)", current.width, current.height);
            palLog(nullptr, "   RefreshRate: %d", current.refreshRate);
            palLog(nullptr, "   Bits Per Pixel: %d", current.bpp);
        }
    }

    palShutdownVideo();
    palFree(nullptr, monitors);
}

PalBool simpleWindowTest()
{
    PalWindow* window = nullptr;
    PalWindowCreateInfo createInfo = {0};

    PalResult result = palInitVideo(nullptr, nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    createInfo.width = 640;
    createInfo.height = 480;
    createInfo.title = "Simple Window";
    createInfo.style = PAL_WINDOW_STYLE_RESIZABLE;
    createInfo.show = PAL_TRUE;
    createInfo.state = PAL_WINDOW_STATE_NORMAL;

    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    palLog(nullptr, "Window Created %p", (void*)window);
    palDestroyWindow(window);
    palShutdownVideo();

    return PAL_TRUE;
}

#endif // PAL_HAS_VIDEO_MODULE

void registerVideoTests()
{
#if PAL_HAS_VIDEO_MODULE
    registerTest(monitorTest, "Monitor Test");
    // registerTest(simpleWindowTest, "Simple Window Test");
#endif // PAL_HAS_VIDEO_MODULE
}