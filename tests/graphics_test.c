
#include "pal/pal_graphics.h"
#include "tests.h"

bool graphicsTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Graphics Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    // initialize the video system
    PalResult result = palInitGraphics(nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize graphics: %s", error);
        return false;
    }

    // shutdown the graphics system
    palShutdownGraphics();
    
    return true;
}