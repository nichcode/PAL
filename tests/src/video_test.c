
#include "tests.h"
#include "pal/pal_video.h"

bool videoTest(void* data)
{
    PalError error = PAL_ERROR_NONE;
    if (!palInitVideoSystem(PAL_NULL)) {
        error = palGetError();
        palLogError(palFormatError(error));
        return PAL_FALSE;
    }

    int displayCount = palGetDisplayCount();
    const PalDisplay* display = palGetPrimaryDisplay();
    int displayModeCount = display->modeCount;

    palShutdownVideoSystem();
    return PAL_TRUE;
}