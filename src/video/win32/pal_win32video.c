
#include "pal_pch.h"
#include "pal/pal_video.h"

#ifdef _WIN32

typedef LONG (WINAPI *RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

static bool getOsVersion();
static bool isVersion(int major, int minor, int servicePack);

#define WINDOWS_XP          (isVersion(5, 1, 0))
#define WINDOWS_VISTA       (isVersion(6, 0, 0))
#define WINDOWS_7           (isVersion(6, 1, 0))
#define WINDOWS_8           (isVersion(6, 2, 0))
#define WINDOWS_8_1         (isVersion(6, 3, 0))
#define WINDOWS_10          (isVersion(10, 0, 0))
#define WINDOWS_11          (isVersion(10, 0, 22000))

PalResult _PCALL palGetVideoFeatures(
    PalVideo* video,
    PalVideoFeatureFlags* features) {

    if (!features || !video) {
        return PAL_ERROR_NULL_POINTER;
    }

    PalVideoFeatureFlags flags = 0;
    flags |= PAL_VIDEO_DISPLAY_ORIENTATION;
    flags |= PAL_VIDEO_BORDERLESS_WINDOW;
    flags |= PAL_VIDEO_FULLSCREEN;
    flags |= PAL_VIDEO_RESIZING;
    flags |= PAL_VIDEO_MULTI_DISPLAYS;
    flags |= PAL_VIDEO_WINDOW_MINMAX;
    flags |= PAL_VIDEO_WINDOW_POSITIONING;
    flags |= PAL_VIDEO_GAMMA_CONTROL;
    flags |= PAL_VIDEO_CLIP_CURSOR;

    if (WINDOWS_8_1) {
        flags |= PAL_VIDEO_PER_DISPLAY_DPI;
        flags |= PAL_VIDEO_HIGH_DPI;
    }
    
    *features = flags;
    return PAL_SUCCESS;
}

static bool getOsVersion(PalVersion* version) {

    static PalVersion cachedVersion = {};
    static bool init = PAL_FALSE;

    if (init) {
        *version = cachedVersion;
        return PAL_TRUE;
    }

    OSVERSIONINFOEXW ver = { 0 };
    ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);

    HINSTANCE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll) {
        return PAL_FALSE;
    }

    RtlGetVersionPtr getVer = (RtlGetVersionPtr)GetProcAddress(ntdll, "RtlGetVersion");
    if (!getVer) {
        return PAL_FALSE;
    }

    if (getVer((PRTL_OSVERSIONINFOW)&ver)) {
        return PAL_FALSE;
    }

    cachedVersion.major = ver.dwMajorVersion;
    cachedVersion.minor = ver.dwMinorVersion;
    cachedVersion.patch = ver.dwBuildNumber;

    init = PAL_TRUE;
    return PAL_TRUE;
}

static bool isVersion(int major, int minor, int servicePack) {

    // get os version;
    PalVersion version;
    if (!getOsVersion(&version)) {
        return PAL_FALSE;
    }

    if (version.major > major || version.minor > minor) {
        return PAL_TRUE;
    }

    if (version.major < major || version.minor < minor) {
        return PAL_FALSE;
    }

    return version.patch >= servicePack;
}

#endif // _WIN32