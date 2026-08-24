
#include "opengl_helper.h"

PalWindow* glHelperCreateWindow(
    const char* title,
    PalEventDriver* eventDriver,
    PalGLFBConfig* outFbConfig)
{
    PalResult result = palInitVideo(nullptr, eventDriver, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to initialize video");
        return nullptr;
    }

    // initialize GL
    PalGLAPI openglAPI;
    void* videoInstance = palGetInstance();
    const PalBool* supportedAPIs = palGetSupportedGLAPIs(videoInstance);
    if (supportedAPIs) {
        if (supportedAPIs[PAL_GL_API_OPENGL]) {
            openglAPI = PAL_GL_API_OPENGL;

        } else {
            openglAPI = PAL_GL_API_OPENGL_ES;
        }

    } else {
        palLog(nullptr, "Failed to get supported opengl apis");
        return PAL_FALSE;
    }

    result = palInitGL(openglAPI, videoInstance, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to initialize opengl");
        return PAL_FALSE;
    }

    // find a fbconfig
    uint32_t fbCount = 0;
    result = palEnumerateGLFBConfigs(&fbCount, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to query GL FBConfigs");
        return nullptr;
    }

    palLog(nullptr, "GL FBConfig count: %d", fbCount);
    if (fbCount == 0) {
        palLog(nullptr, "No supported FBConfig found");
        return nullptr;
    }

    PalGLFBConfig* fbConfigs = nullptr;
    fbConfigs = palAllocate(nullptr, sizeof(PalGLFBConfig) * fbCount, 0);
    if (!fbConfigs) {
        palLog(nullptr, "Failed to allocate memory");
        return nullptr;
    }

    // enumerate supported opengl framebuffer configs
    result = palEnumerateGLFBConfigs(&fbCount, fbConfigs);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to query GL FBConfigs");
        return nullptr;
    }

    // we desire a FB config and see what is closest the driver will give us
    PalGLFBConfig desired = {0};
    desired.redBits = 8;
    desired.greenBits = 8;
    desired.blueBits = 8;
    desired.alphaBits = 8;
    desired.alphaBits = 8;
    desired.depthBits = 24;
    desired.stencilBits = 8;
    desired.samples = 2;

    desired.sRGB = PAL_TRUE;
    desired.doubleBuffer = PAL_TRUE;

    // get the closest and create window with that
    const PalGLFBConfig* closest = nullptr;
    closest = palGetClosestGLFBConfig(fbConfigs, fbCount, &desired);

    PalWindowCreateInfo createInfo = {0};
    createInfo.height = WINDOW_HEIGHT;
    createInfo.width = WINDOW_WIDTH;
    createInfo.show = PAL_TRUE;
    createInfo.title = title;

    createInfo.fbConfigBackend = PAL_FBCONFIG_BACKEND_PAL_OPENGL;
    createInfo.fbConfigIndex = closest->index;

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

    *outFbConfig = *closest;
    palFree(nullptr, fbConfigs);
    return window;
}

void glHelperDestroyWindow(
    PalWindow* window, 
    PalEventDriver* eventDriver)
{
    palDestroyWindow(window);
    palShutdownVideo();
    palDestroyEventDriver(eventDriver);
}

PalGLWindow glHelperGetGLWindow(PalWindow* window)
{
    PalWindowHandleInfo winHandle = {0};
    palGetWindowHandleInfo(window, &winHandle);

    PalGLWindow glWindow = {0};
    glWindow.instance = winHandle.nativeInstance;

    // On Wayland the window is the wl_egl_window
    if (winHandle.nativeHandle3) {
        // the window has a valid wl_egl_window
        glWindow.window = winHandle.nativeHandle3;

    } else {
        glWindow.window = winHandle.nativeWindow;
    }

    return glWindow;
}

PalGLContext* glHelperCreateContext(
    PalGLWindow* glWindow, 
    PalGLFBConfig* fbConfig)
{
    PalGLContext* context = nullptr;
    const PalGLInfo* info = palGetGLInfo();

    PalGLContextCreateInfo createInfo = {0};
    createInfo.debug = PAL_TRUE;
    createInfo.fbConfig = fbConfig;
    createInfo.major = info->major;
    createInfo.minor = info->minor;
    createInfo.window = glWindow;

    if (info->extensions & PAL_GL_EXTENSION_CREATE_CONTEXT) {
        createInfo.forward = PAL_TRUE;
    }

    if (info->extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE) {
        createInfo.profile = PAL_GL_PROFILE_CORE;
    }

    PalResult result = palCreateGLContext(&createInfo, &context);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create GL context");
        return nullptr;
    }

    // make the context current and optionally set vsync if supported
    result = palMakeContextCurrent(glWindow, context);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to make GL context current");
        return nullptr;
    }

    if (info->extensions & PAL_GL_EXTENSION_SWAP_CONTROL) {
        palSetSwapInterval(1);
    }

    return context;
}

void glHelperDestroyContext(PalGLContext* context)
{
    palDestroyGLContext(context);
    palShutdownGL();
}