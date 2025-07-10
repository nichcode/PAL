
#include "pal/pal.h"

void OnLoggerLog(void* userData, PalLogLevel level, const char* msg)
{
    palLogConsole(level, msg);
}

int main(int argc, char** argv) 
{
    PalResult result;
    PalVersion version;
    palGetVerion(&version);
    palLogConsoleInfo("PAL v(%i.%i.%i)", version.major, version.minor, version.patch);

    // logger
    PalLogger logger;
    logger.userData = PAL_NULL;
    logger.callback = OnLoggerLog;
    palLog(&logger, PAL_LOG_LEVEL_TRACE, "Hello from float - %f", 10.0f);

    // video
    PalVideo* video;
    result = palCreateVideo(PAL_NULL, &video);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return -1;
    }

    // displays
    Uint32 displayCount = 0;
    result = palEnumerateDisplays(video, &displayCount, PAL_NULL);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return -1;
    }

    palLogConsoleInfo("Display Count: %i", displayCount);
    PalDisplay* displays[displayCount];
    palEnumerateDisplays(video, PAL_NULL, displays);

    // get display info
    PalDisplay* primaryDisplay = displays[0];
    PalDisplayInfo displayInfo;
    
    result = palGetDisplayInfo(primaryDisplay, &displayInfo);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return -1;
    }

    palLogConsoleInfo("Display: %s", displayInfo.name);
    palLogConsoleInfo("  Size: (%i, %i)", displayInfo.width, displayInfo.height);
    palLogConsoleInfo("  DPI Scale: (%.2f, %.2f)", displayInfo.dpiScaleX, displayInfo.dpiScaleY);
    palLogConsoleInfo("  RefreshRate: %i", displayInfo.refreshRate);

    palDestroyVideo(video);
    return 0;
}