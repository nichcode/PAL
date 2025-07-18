
#include "pal_pch.h"
#include "pal/pal_video.h"
#include "platform/pal_platform.h"

#ifdef _WIN32

PalResult _PCALL palGetVideoFeatures(
    PalVideo video,
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

    // get os version
    PalVersion osVersion;
    if (!palGetOsVersionWin32(&osVersion)) {
        return PAL_ERROR_MODULE_NOT_FOUND;
    }

    if (palIsOsVersionWin32(&osVersion, &g_VersionsWin32.window8_1)) {
        flags |= PAL_VIDEO_PER_DISPLAY_DPI;
        flags |= PAL_VIDEO_HIGH_DPI;
    }
    
    *features = flags;
    return PAL_SUCCESS;
}

#endif // _WIN32