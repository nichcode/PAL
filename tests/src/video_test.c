
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

    palShutdownVideoSystem();
    return PAL_TRUE;
}