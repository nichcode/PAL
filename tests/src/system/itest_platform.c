#include "pal2/system/platform.h"
#include "pal2/core/log.h"

static const char* platformTypeToString(PalPlatformType type)
{
    switch (type) {
    case PAL_PLATFORM_TYPE_WINDOWS:
        return "Windows";

    case PAL_PLATFORM_TYPE_LINUX:
        return "Linux";

    case PAL_PLATFORM_TYPE_MACOS:
        return "MacOS";

    case PAL_PLATFORM_TYPE_ANDROID:
        return "Android";

    case PAL_PLATFORM_TYPE_IOS:
        return "iOS";
    }

    return "Unknown";
}

static const char* platformAPITypeToString(PalPlatformApiType type)
{
    switch (type) {
    case PAL_PLATFORM_API_TYPE_WIN32:
        return "Win32";

    case PAL_PLATFORM_API_TYPE_WAYLAND:
        return "Wayland";

    case PAL_PLATFORM_API_TYPE_X11:
        return "X11";

    case PAL_PLATFORM_API_TYPE_COCOA:
        return "Cocoa";

    case PAL_PLATFORM_API_TYPE_ANDRIOD:
        return "Android";

    case PAL_PLATFORM_API_TYPE_UIKIT:
        return "UIKit";

    case PAL_PLATFORM_API_TYPE_HEADLESS:
        return "Headless";
    }

    return "Unknown";
}

int main(void)
{
    palLog(nullptr, "Running platform itest...");

    PalPlatformInfo info = {0};
    palGetPlatformInfo(&info);

    palLog(nullptr, "Platform: %s", platformTypeToString(info.type));
    palLog(nullptr, " Name: %s", info.name);
    palLog(nullptr, " API: %s", platformAPITypeToString(info.apiType));
    palLog(nullptr, " Total RAM: %u MB", info.totalRAM);
    palLog(nullptr, " Total Disk: %u GB", info.totalMemory);

    palLog(
        nullptr,
        " Version: (%u.%u.%u)",
        info.version.major,
        info.version.minor,
        info.version.build);

    return 0;
}