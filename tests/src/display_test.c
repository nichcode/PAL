
#include "tests.h"

bool displayTest()
{
    PalError error = PAL_ERROR_NONE;
    if (!palInitVideo(PAL_NULL)) {
        error = palGetError();
        palLogError(palFormatError(error));
        return PAL_FALSE;
    }

    int displayCount = palGetDisplayCount();
    if (!displayCount) { 
        return PAL_FALSE;
    }

    palLogInfo(""); // for space
    palLogInfo("Display(Monitor) Count: %i", displayCount);
    for (int i = 0; i < displayCount; i++) {
        const PalDisplay* display = palGetDisplay(i);
        const PalDisplayMode* firstMode = &display->modes[0];
        const PalDisplayMode* lastMode = &display->modes[display->modeCount];

        palLogInfo("Display: %s", display->name);
        palLogInfo("  Index: %i", display->index);
        palLogInfo("  Modes: %i", display->modeCount);
        palLogInfo("  Size: (%i, %i)", display->width, display->height);
        palLogInfo("  DPI Scale: (%.2f, %.2f)", display->dpiScaleX, display->dpiScaleY);
        palLogInfo("  RefreshRate: %i", display->refreshRate);

        palLogInfo("\tFirst Mode");
        palLogInfo("\t  Size: (%i, %i)", firstMode->width, firstMode->height);
        palLogInfo("\t  RefreshRate: %i", firstMode->refreshRate);
        palLogInfo("\t  Red Bits: %i", firstMode->redBits);
        palLogInfo("\t  Green Bits: %i", firstMode->greenBits);
        palLogInfo("\t  Blue Bits: %i", firstMode->blueBits);
        palLogInfo("\t  Alpha Bits: %i", firstMode->alphaBits);

        palLogInfo(
            "\t  Color Bits: %i", 
            firstMode->alphaBits + 
            firstMode->greenBits +
            firstMode->blueBits +
            firstMode->alphaBits
        );

        palLogInfo("\tLast Mode");
        palLogInfo("\t  Size: (%i, %i)", lastMode->width, lastMode->height);
        palLogInfo("\t  RefreshRate: %i", lastMode->refreshRate);
        palLogInfo("\t  Red Bits: %i", lastMode->redBits);
        palLogInfo("\t  Green Bits: %i", lastMode->greenBits);
        palLogInfo("\t  Blue Bits: %i", lastMode->blueBits);
        palLogInfo("\t  Alpha Bits: %i", lastMode->alphaBits);

        palLogInfo(
            "\t  Color Bits: %i", 
            lastMode->alphaBits + 
            lastMode->greenBits +
            lastMode->blueBits +
            lastMode->alphaBits
        );

    }
    
    palShutdownVideo();
    return PAL_TRUE;
}