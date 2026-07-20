
#include "pal/pal_system.h"
#include "tests.h"

static inline const char* platformToString(PalPlatformType type)
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

static inline const char* platformApiToString(PalPlatformApiType type)
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

PalBool platformTest()
{
    PalResult result;
    PalPlatformInfo platformInfo;

    // get the platform info. Users must cache this
    palGetPlatformInfo(&platformInfo);

    palLog(nullptr, "Platform: %s", platformToString(platformInfo.type));
    palLog(nullptr, " Name: %s", platformInfo.name);
    palLog(nullptr, " API: %s", platformApiToString(platformInfo.apiType));
    palLog(nullptr, " Total RAM: %llu MB", platformInfo.totalRAM);
    palLog(nullptr, " Total Memory: %llu GB", platformInfo.totalMemory);

    uint16_t major, minor, build;
    major = platformInfo.version.major;
    minor = platformInfo.version.minor;
    build = platformInfo.version.build;
    palLog(nullptr, " Version: (%d.%d.%d)", major, minor, build);

    return PAL_TRUE;
}
