
#include "tests.h"
#include "pal/pal_video.h"

void videoTest() {

    palLogInfo(PAL_NULL, "");
    palLogInfo(PAL_NULL, "===========================================");
    palLogInfo(PAL_NULL, "Video Test");
    palLogInfo(PAL_NULL, "===========================================");
    palLogInfo(PAL_NULL, "");

    PalResult result;
    PalVideo* video = PAL_NULL;
    PalVideoConfig config;
    config.allocator = PAL_NULL;

    result = palCreateVideo(&config, &video);
    if (result != PAL_SUCCESS) {
        // opiional log error to console
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return;
    }

    palDestroyVideo(video);
}