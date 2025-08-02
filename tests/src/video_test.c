
#include "tests.h"
#include "pal/pal_video.h"

void videoTest() {

    palLog("");
    palLog("===========================================");
    palLog("Video Test");
    palLog("===========================================");
    palLog("");

    PalResult result;
    PalVideoSystem* video = nullptr;
    PalVideoSystemCreateInfo createInfo;
    createInfo.allocator = nullptr; // for default.

    result = palCreateVideoSystem(&createInfo, &video);
    if (result != PAL_RESULT_SUCCESS) {
        // this can made into a goto to decrease this result checks
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    palDestroyVideoSystem(video);
}