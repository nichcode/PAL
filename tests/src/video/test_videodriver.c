
#include "pal2/video/init.h"
#include "pal2/core/log.h"
#include "pal2/core/memory.h"
#include <stdbool.h>

bool defaultVideoDriver(void)
{
    PalResult result = palInitVideo(nullptr, nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to initialize video system");
        return false;
    }

    palShutdownVideo();
    return true;
}

bool videoDriver(void)
{
    PalVideoDriver* drivers = nullptr;
    uint32_t driverCount = 0;
    palEnumerateVideoDrivers(&driverCount, nullptr);

    drivers = palAllocate(nullptr, sizeof(PalVideoDriver) * driverCount, 0);
    if (!drivers) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    PalVideoDriver selectedDriver = {0};
    PalVideoDriver usedDriver = {0};

    palEnumerateVideoDrivers(&driverCount, drivers);
    PalVideoDriver* driver = &drivers[driverCount -1]; // select the last one
    selectedDriver = *driver;

    palSetVideoDriver(&selectedDriver);
    PalResult result = palInitVideo(nullptr, nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to initialize video system");
        return false;
    }

    palGetVideoDriver(&usedDriver);
    palShutdownVideo();
    palFree(nullptr, drivers);
    
    if (usedDriver.features == selectedDriver.features && 
        usedDriver.id == selectedDriver.id &&
        usedDriver.type == selectedDriver.type
    ) {
        return true;
    }
    
    palLog(nullptr, "Video driver used was not the requested");
    return false;
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