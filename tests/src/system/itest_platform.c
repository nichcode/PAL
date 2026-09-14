
#include "pal2/pal_system.h"

static const char* platformTypeToString(PalPlatformType type)
{
    switch (type) {
    case PAL_PLATFORM_TYPE_WINDOWS:
        return "Windows";

    case PAL_PLATFORM_TYPE_LINUX:
        return "Linux";

    case PAL_PLATFORM_TYPE_MACOS:
        return "MacOs";

    case PAL_PLATFORM_TYPE_ANDROID:
        return "Android";

    case PAL_PLATFORM_TYPE_IOS:
        return "Ios";
    }

    return nullptr;
}

static const char* platformAPITypeToString(PalPlatformApiType type)
{
    switch (type) {
    case PAL_PLATFORM_API_TYPE_WIN32:
        return "Win32";

    case PAL_PLATFORM_API_TYPE_X11:
        return "X11";

    case PAL_PLATFORM_API_TYPE_WAYLAND:
        return "Wayland";
    }

    return nullptr;
}

int main(void)
{
    palLog(nullptr, "Running platform itest...");

    PalPlatformInfo info;
    palGetPlatformInfo(&info);

    palLog(nullptr, "Platform: %s", platformTypeToString(info.type));
    palLog(nullptr, " Name: %s", info.name);
    palLog(nullptr, " API: %s", platformAPITypeToString(info.apiType));
    palLog(nullptr, " Total RAM: %llu MB", info.totalRAM);
    palLog(nullptr, " Total Memory: %llu GB", info.totalMemory);

    uint32_t major, minor, build;
    major = info.version.major;
    minor = info.version.minor;
    build = info.version.build;

    palLog(nullptr, " Version: (%d.%d.%d)", major, minor, build);

    return 0;
}