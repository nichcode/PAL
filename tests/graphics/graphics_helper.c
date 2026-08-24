
#include "graphics_helper.h"
#include "pal2/pal_system.h"

PalWindow* gfxHelperCreateWindow(
    const char* title,
    PalEventDriver* eventDriver)
{
    PalResult result = palInitVideo(nullptr, eventDriver, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to initialize video");
        return nullptr;
    }

    PalWindowCreateInfo createInfo = {0};
    createInfo.height = WINDOW_HEIGHT;
    createInfo.width = WINDOW_WIDTH;
    createInfo.show = PAL_TRUE;
    createInfo.title = title;

    PalVideoFeatures features = palGetVideoFeatures();
    if (!(features & PAL_VIDEO_FEATURE_DECORATED_WINDOW)) {
        createInfo.style |= PAL_WINDOW_STYLE_BORDERLESS;
    }

    PalWindow* window = nullptr;
    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create window");
        return nullptr;
    }

    return window;
}

void gfxHelperDestroyWindow(
    PalWindow* window, 
    PalEventDriver* eventDriver)
{
    palDestroyWindow(window);
    palShutdownVideo();
    palDestroyEventDriver(eventDriver);
}

PalSurface* gfxHelperCreateSurface(
    PalDevice* device, 
    PalWindow* window)
{
    PalResult result;
    PalSurface* surface = nullptr;

    PalWindowHandleInfo winHandle = {0};
    PalPlatformInfo platformInfo = {0};
    palGetWindowHandleInfo(window, &winHandle);
    palGetPlatformInfo(&platformInfo);

    PalWindowInstanceType instanceType = PAL_WINDOW_INSTANCE_TYPE_XCB;
    if (platformInfo.apiType == PAL_PLATFORM_API_TYPE_WAYLAND) {
        instanceType = PAL_WINDOW_INSTANCE_TYPE_WAYLAND;

    } else if (platformInfo.apiType == PAL_PLATFORM_API_TYPE_X11) {
        instanceType = PAL_WINDOW_INSTANCE_TYPE_X11;

    } else if (platformInfo.apiType == PAL_PLATFORM_API_TYPE_WIN32) {
        instanceType = PAL_WINDOW_INSTANCE_TYPE_WIN32;
    }

    // create surface
    result = palCreateSurface(
        device,
        winHandle.nativeWindow,
        winHandle.nativeInstance,
        instanceType,
        &surface);

    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create surface");
        return nullptr;
    }

    return surface;
}