
#include "tests.h"
#include "pal/pal_video.h"

bool videoTest() {

    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Video Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalResult result;
    PalVideoFeatures features;
    
    // initialize the video system
    result = palInitVideo(nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to initialize video %s", palFormatResult(result));
        return false;
    }
    
    // get supported features
    features = palGetVideoFeatures();
    if (features & PAL_VIDEO_FEATURE_HIGH_DPI) {
        palLog(nullptr, "High DPI windows feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_MONITOR_ORIENTATION) {
        palLog(nullptr, "Switching monitor orientation feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_BORDERLESS_WINDOW) {
        palLog(nullptr, "Borderless windows feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW) {
        palLog(nullptr, "Transparent windows feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_TOOL_WINDOW) {
        palLog(nullptr, "Tool windows feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_MONITOR_MODE_SWITCH) {
        palLog(nullptr, "Switching monitor modes feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_MULTI_MONITORS) {
        palLog(nullptr, "Multi monitors feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_RESIZING) {
        palLog(nullptr, "Dynamic window resizing feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_POSITIONING) {
        palLog(nullptr, "Dynamic window positioning feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_MINMAX) {
        palLog(nullptr, "Window maximize and minimize feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_NO_MINIMIZEBOX) {
        palLog(nullptr, "No minimize box for windows feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_NO_MAXIMIZEBOX) {
        palLog(nullptr, "No maximize box for windows feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_MONITOR_GAMMA_CONTROL) {
        palLog(nullptr, "Monitor gamma control feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_CLIP_CURSOR) {
        palLog(nullptr, "Clipping cursor (mouse) feature is supported");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION) {
        palLog(nullptr, "Window titlebar flashing is supported");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY) {
        palLog(nullptr, "Window icon on taskbar flashing is supported");
    }

    // shutdown the video system
    palShutdownVideo();
    return true;
}