
#include "tests.h"
#include "pal/pal_video.h"

PalResult displayModeTest()
{
    palLogConsoleInfo("");
    palLogConsoleInfo("===========================================");
    palLogConsoleInfo("Display Mode Test");
    palLogConsoleInfo("===========================================");
    palLogConsoleInfo("");

    PalResult result;
    PalVideoInstance* videoInstance = PAL_NULL;
    PalDisplayInfo displayInfo;
    Uint32 displayCount = 0;
    Uint32 displayModeCount = 0;

    PalVideoInstanceDesc desc;
    desc.allocator = PAL_NULL;
    desc.eventinstance = PAL_NULL;

    result = palCreateVideoInstance(&desc, &videoInstance);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return -1;
    }

    result = palEnumerateDisplays(videoInstance, &displayCount, PAL_NULL);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return -1;
    }
    palLogConsoleInfo("Display Count: %i", displayCount);

    PalDisplay* displays[displayCount];
    result = palEnumerateDisplays(videoInstance, &displayCount, displays);
    if (result != PAL_RESULT_OK) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return -1;
    }

    // get display info
    for (Uint32 i = 0; i < displayCount; i++) {
        PalDisplay* display = displays[i];
        result = palGetDisplayInfo(display, &displayInfo);
        if (result != PAL_RESULT_OK) {
            PalError error = palGetError();
            palLogConsoleError(palErrorToString(error));
            return -1;
        }

        // log display info
        palLogConsoleInfo("Display: %s", displayInfo.name);

        // get display mode count
        result = palEnumerateDisplayModes(display, &displayModeCount, PAL_NULL);
            if (result != PAL_RESULT_OK) {
            PalError error = palGetError();
            palLogConsoleError(palErrorToString(error));
            return -1;
        }
        palLogConsoleInfo(" Display Mode Count: %i", displayModeCount);

        PalDisplayMode modes[displayModeCount];
        result = palEnumerateDisplayModes(display, &displayModeCount, modes);
            if (result != PAL_RESULT_OK) {
            PalError error = palGetError();
            palLogConsoleError(palErrorToString(error));
            return -1;
        }

        for (Uint32 i = 0; i < displayModeCount; i++) {
            PalDisplayMode* mode = &modes[i];
            palLogConsoleInfo(" Mode Index: %i", i);
            palLogConsoleInfo("  Size: (%i, %i)", mode->width, mode->height);
            palLogConsoleInfo("  RefreshRate: %i", mode->refreshRate);
            palLogConsoleInfo("  Red Bits: %i", mode->redBits);
            palLogConsoleInfo("  Green Bits: %i", mode->greenBits);
            palLogConsoleInfo("  Blue Bits: %i", mode->blueBits);
            palLogConsoleInfo("  Alpha Bits: %i", mode->alphaBits);
            palLogConsoleInfo(""); // space
        }
    }

    palDestroyVideoInstance(videoInstance);
    return result;
}