
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