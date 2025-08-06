
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
    PalVideoFeatures features;
    PalVideoSystemCreateInfo createInfo;
    createInfo.allocator = nullptr; // for default.

    result = palCreateVideoSystem(&createInfo, &video);
    if (result != PAL_RESULT_SUCCESS) {
        // this can made into a goto to decrease this result checks
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    features = palGetVideoFeatures(video);

    if (features & PAL_VIDEO_FEATURE_HIGH_DPI) {
        palLog("High DPI windows feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_DISPLAY_ORIENTATION) {
        palLog("Switching display orientation feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_BORDERLESS_WINDOW) {
        palLog("Borderless windows feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_DISPLAY_MODE_SWITCH) {
        palLog("Switching display modes feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_MULTI_DISPLAYS) {
        palLog("Multi monitors feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_RESIZING) {
        palLog("Dynamic window resizing feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_POSITIONING) {
        palLog("Dynamic window positioning feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_MINMAX) {
        palLog("Window maximize and minimize feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_DISPLAY_GAMMA_CONTROL) {
        palLog("Display gamma control feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_CLIP_CURSOR) {
        palLog("Clipping cursor (mouse) feature is supported");
    }

    palDestroyVideoSystem(video);
}