
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

    // query video features
    PalVideoFeatureFlags videoFeatures;
    palGetVideoFeatures(video, &videoFeatures);

    palLogInfo(PAL_NULL, "Supported Video Features");

    if (videoFeatures & PAL_VIDEO_PER_DISPLAY_DPI) {
        palLogInfo(PAL_NULL, " Per Display DPI");
    }

    if (videoFeatures & PAL_VIDEO_HIGH_DPI) {
        palLogInfo(PAL_NULL, " High DPI");
    }

    if (videoFeatures & PAL_VIDEO_DISPLAY_ORIENTATION) {
        palLogInfo(PAL_NULL, " Multi Display Orientation");
    }

    if (videoFeatures & PAL_VIDEO_BORDERLESS_WINDOW) {
        palLogInfo(PAL_NULL, " Boderless Window");
    }

    if (videoFeatures & PAL_VIDEO_FULLSCREEN) {
        palLogInfo(PAL_NULL, " Fullscreen");
    }

    if (videoFeatures & PAL_VIDEO_RESIZING) {
        palLogInfo(PAL_NULL, " Dynamic Window Resizing");
    }

    if (videoFeatures & PAL_VIDEO_MULTI_DISPLAYS) {
        palLogInfo(PAL_NULL, " Multi Displays");
    }

    if (videoFeatures & PAL_VIDEO_WINDOW_MINMAX) {
        palLogInfo(PAL_NULL, " Window Minimize and Maximize");
    }

    if (videoFeatures & PAL_VIDEO_WINDOW_POSITIONING) {
        palLogInfo(PAL_NULL, " Dynamic Window Positioning");
    }

    if (videoFeatures & PAL_VIDEO_CLIP_CURSOR) {
        palLogInfo(PAL_NULL, " Clip Cursor");
    }

    if (videoFeatures & PAL_VIDEO_GAMMA_CONTROL) {
        palLogInfo(PAL_NULL, " Gamma Cursor");
    }

    palDestroyVideo(video);
}