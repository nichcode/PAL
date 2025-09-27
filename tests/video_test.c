
#include "pal/pal_video.h"
#include "tests.h"

bool videoTest()
{
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
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize video %s", error);
        return false;
    }

    // get supported features
    features = palGetVideoFeatures();
    palLog(nullptr, "Supported Video Features:");
    if (features & PAL_VIDEO_FEATURE_HIGH_DPI) {
        palLog(nullptr, "High DPI windows");
    }

    if (features & PAL_VIDEO_FEATURE_MONITOR_SET_ORIENTATION) {
        palLog(nullptr, "Setting monitor orientation");
    }

    if (features & PAL_VIDEO_FEATURE_MONITOR_GET_ORIENTATION) {
        palLog(nullptr, "Getting monitor orientation");
    }

    if (features & PAL_VIDEO_FEATURE_BORDERLESS_WINDOW) {
        palLog(nullptr, "Borderless windows");
    }

    if (features & PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW) {
        palLog(nullptr, "Transparent windows");
    }

    if (features & PAL_VIDEO_FEATURE_TOOL_WINDOW) {
        palLog(nullptr, "Tool windows");
    }

    if (features & PAL_VIDEO_FEATURE_MONITOR_SET_MODE) {
        palLog(nullptr, "Setting monitor display mode");
    }

    if (features & PAL_VIDEO_FEATURE_MONITOR_GET_MODE) {
        palLog(nullptr, "Getting monitor display mode");
    }

    if (features & PAL_VIDEO_FEATURE_MULTI_MONITORS) {
        palLog(nullptr, "Multi monitors");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_SET_SIZE) {
        palLog(nullptr, "Setting window size");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_GET_SIZE) {
        palLog(nullptr, "Getting window size");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_SET_POS) {
        palLog(nullptr, "Setting window position");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_GET_POS) {
        palLog(nullptr, "Getting window position");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_SET_STATE) {
        palLog(nullptr, "Setting window state");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_GET_STATE) {
        palLog(nullptr, "Getting window state");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY) {
        palLog(nullptr, "Setting window visibility");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_GET_VISIBILITY) {
        palLog(nullptr, "Getting window visibility");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_SET_TITLE) {
        palLog(nullptr, "Setting window title");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_GET_TITLE) {
        palLog(nullptr, "Getting window title");
    }

    if (features & PAL_VIDEO_FEATURE_NO_MINIMIZEBOX) {
        palLog(nullptr, "No minimize box for windows");
    }

    if (features & PAL_VIDEO_FEATURE_NO_MAXIMIZEBOX) {
        palLog(nullptr, "No maximize box for windows");
    }

    if (features & PAL_VIDEO_FEATURE_CLIP_CURSOR) {
        palLog(nullptr, "Clipping cursor (mouse)");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION) {
        palLog(nullptr, "Window titlebar flashing");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY) {
        palLog(nullptr, "Window icon on taskbar flashing");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_FLASH_INTERVAL) {
        palLog(nullptr, "Setting window flash interval");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_SET_INPUT_FOCUS) {
        palLog(nullptr, "Setting input window");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_GET_INPUT_FOCUS) {
        palLog(nullptr, "Getting input window");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_SET_STYLE) {
        palLog(nullptr, "Setting window style");
    }

    if (features & PAL_VIDEO_FEATURE_WINDOW_GET_STYLE) {
        palLog(nullptr, "Getting window style");
    }

    if (features & PAL_VIDEO_FEATURE_CURSOR_SET_POS) {
        palLog(nullptr, "Setting cursor position");
    }

    if (features & PAL_VIDEO_FEATURE_CURSOR_GET_POS) {
        palLog(nullptr, "Getting cursor position");
    }

    // shutdown the video system
    palShutdownVideo();
    return true;
}