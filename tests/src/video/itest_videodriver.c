
#include "pal2/video/init.h"
#include "pal2/core/log.h"
#include "pal2/core/memory.h"
#include <stdbool.h>

void logDriverInfo(PalVideoDriver* driver)
{
    const char* driverType = "Unknown";

    switch (driver->type) {
    case PAL_VIDEO_DRIVER_TYPE_WIN32: {
        driverType = "Win32";
        break;
    }

    case PAL_VIDEO_DRIVER_TYPE_WAYLAND: {
        driverType = "Wayland";
        break;
    }

    case PAL_VIDEO_DRIVER_TYPE_X11: {
        driverType = "X11";
        break;
    }
    }

    palLog(nullptr, "Driver Type: %s", driverType);
    palLog(nullptr, "Driver Id: %u", driver->id);
    
    palLog(nullptr, "Supported Video Features:");
    if (driver->features & PAL_VIDEO_FEATURE_HIGH_DPI) {
        palLog(nullptr, " High DPI windows");
    }

    if (driver->features & PAL_VIDEO_FEATURE_MONITOR_SET_ORIENTATION) {
        palLog(nullptr, " Setting monitor orientation");
    }

    if (driver->features & PAL_VIDEO_FEATURE_MONITOR_GET_ORIENTATION) {
        palLog(nullptr, " Getting monitor orientation");
    }

    if (driver->features & PAL_VIDEO_FEATURE_BORDERLESS_WINDOW) {
        palLog(nullptr, " Borderless windows");
    }

    if (driver->features & PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW) {
        palLog(nullptr, " Transparent windows");
    }

    if (driver->features & PAL_VIDEO_FEATURE_TOOL_WINDOW) {
        palLog(nullptr, " Tool windows");
    }

    if (driver->features & PAL_VIDEO_FEATURE_MONITOR_SET_MODE) {
        palLog(nullptr, " Setting monitor display mode");
    }

    if (driver->features & PAL_VIDEO_FEATURE_MONITOR_GET_MODE) {
        palLog(nullptr, " Getting monitor display mode");
    }

    if (driver->features & PAL_VIDEO_FEATURE_MULTI_MONITORS) {
        palLog(nullptr, " Multi monitors");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_SET_SIZE) {
        palLog(nullptr, " Setting window size");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_GET_SIZE) {
        palLog(nullptr, " Getting window size");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_SET_POS) {
        palLog(nullptr, " Setting window position");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_GET_POS) {
        palLog(nullptr, " Getting window position");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_SET_STATE) {
        palLog(nullptr, " Setting window state");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_GET_STATE) {
        palLog(nullptr, " Getting window state");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY) {
        palLog(nullptr, " Setting window visibility");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_GET_VISIBILITY) {
        palLog(nullptr, " Getting window visibility");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_SET_TITLE) {
        palLog(nullptr, " Setting window title");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_GET_TITLE) {
        palLog(nullptr, " Getting window title");
    }

    if (driver->features & PAL_VIDEO_FEATURE_NO_MINIMIZEBOX) {
        palLog(nullptr, " No minimize box for windows");
    }

    if (driver->features & PAL_VIDEO_FEATURE_NO_MAXIMIZEBOX) {
        palLog(nullptr, " No maximize box for windows");
    }

    if (driver->features & PAL_VIDEO_FEATURE_CLIP_CURSOR) {
        palLog(nullptr, " Clipping cursor (mouse)");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION) {
        palLog(nullptr, " Window titlebar flashing");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY) {
        palLog(nullptr, " Window icon on taskbar flashing");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_FLASH_INTERVAL) {
        palLog(nullptr, " Setting window flash interval");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_SET_INPUT_FOCUS) {
        palLog(nullptr, " Setting input window");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_GET_INPUT_FOCUS) {
        palLog(nullptr, " Getting input window");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_SET_STYLE) {
        palLog(nullptr, " Setting window style");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_GET_STYLE) {
        palLog(nullptr, " Getting window style");
    }

    if (driver->features & PAL_VIDEO_FEATURE_CURSOR_SET_POS) {
        palLog(nullptr, " Setting cursor position");
    }

    if (driver->features & PAL_VIDEO_FEATURE_CURSOR_GET_POS) {
        palLog(nullptr, " Getting cursor position");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_SET_ICON) {
        palLog(nullptr, " Setting window icon");
    }

    if (driver->features & PAL_VIDEO_FEATURE_TOPMOST_WINDOW) {
        palLog(nullptr, " Topmost windows");
    }

    if (driver->features & PAL_VIDEO_FEATURE_DECORATED_WINDOW) {
        palLog(nullptr, " Decorated (Normal) windows");
    }

    if (driver->features & PAL_VIDEO_FEATURE_CURSOR_SET_VISIBILITY) {
        palLog(nullptr, " Setting cursor visibility");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_GET_MONITOR) {
        palLog(nullptr, " Getting window current monitor");
    }

    if (driver->features & PAL_VIDEO_FEATURE_MONITOR_GET_PRIMARY) {
        palLog(nullptr, " Getting primary monitor");
    }

    if (driver->features & PAL_VIDEO_FEATURE_FOREIGN_WINDOWS) {
        palLog(nullptr, " Attaching and detaching foreign windows");
    }

    if (driver->features & PAL_VIDEO_FEATURE_MONITOR_VALIDATE_MODE) {
        palLog(nullptr, " Validate monitor display mode");
    }

    if (driver->features & PAL_VIDEO_FEATURE_WINDOW_SET_CURSOR) {
        palLog(nullptr, " Setting window cursor");
    }

    palLog(nullptr, "");
}

bool defaultVideoDriver(void)
{
    //TODO: initialization should work without a user choosing a driver.
    return true;
}

bool videoDriver(void)
{
    PalVideoDriver* drivers = nullptr;
    uint32_t driverCount = 0;
    palEnumerateVideoDrivers(&driverCount, nullptr);

    drivers = palAllocate(nullptr, sizeof(PalVideoDriver) * driverCount, 0);
    if (!drivers) {
        return false;
    }

    palEnumerateVideoDrivers(&driverCount, drivers);
    for (uint32_t i = 0; i < driverCount; i++) {
        PalVideoDriver* driver = &drivers[i];
        logDriverInfo(driver);
    }

    palFree(nullptr, drivers);

    return true;
}

int main(void)
{
    palLog(nullptr, "Running video driver test...");

    bool ret = defaultVideoDriver();
    if (!ret) {
        return -1;
    }

    ret = videoDriver();
    if (!ret) {
        return -1;
    }

    return 0;
}