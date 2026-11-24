#include "pal2/opengl/init.h"
#include "pal2/opengl/context.h"
#include "pal2/core/log.h"
#include "pal2/video/init.h"

#include "pal2/core/pack.h"
#include "pal2/video/window.h"
#include "pal2/video/keyboard.h"
#include "pal2/event/driver.h"

typedef void(PAL_GL_APIENTRY* PFNGLCLEARCOLORPROC)(
    float red,
    float green,
    float blue,
    float alpha);

typedef void(PAL_GL_APIENTRY* PFNGLCLEARPROC)(uint32_t mask);

int main(void)
{
    palLog(nullptr, "Running context itest...");

    PalEventDriver* eventDriver = nullptr;
    PalWindow* window = nullptr;
    PalGLContext* context = nullptr;
    PalGLFBConfig* fbConfigs = nullptr;

    PalResult result = PAL_RESULT_SUCCESS;
    int ret = 0;

    PalEventDriverCreateInfo eventDriverCreateInfo = {0};

    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create event driver");
        return -1;
    }

    result = palInitVideo(nullptr, eventDriver, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to initialize video system");
        palDestroyEventDriver(eventDriver);
        return -1;
    }

    PalGLAPI openglAPI;
    const void* videoInstance = palGetInstance();
    const PalBool* supportedAPIs = palGetSupportedGLAPIs(videoInstance);
    if (supportedAPIs) {
        if (supportedAPIs[PAL_GL_API_OPENGL]) {
            openglAPI = PAL_GL_API_OPENGL;
        } else {
            openglAPI = PAL_GL_API_OPENGL_ES;
        }

    } else {
        palLog(nullptr, "Failed to get supported opengl apis");
        ret = -1;
        goto cleanup;
    }

    result = palInitGL(openglAPI, videoInstance, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to initialize opengl");
        ret = -1;
        goto cleanup;
    }

    palSetEventDispatchMode(eventDriver, PAL_EVENT_TYPE_WINDOW_CLOSE, PAL_DISPATCH_MODE_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_TYPE_KEYDOWN, PAL_DISPATCH_MODE_POLL);

    uint32_t fbCount = 0;
    result = palEnumerateGLFBConfigs(&fbCount, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to query GL FBConfigs");
        ret = -1;
        goto cleanup;
    }

    palLog(nullptr, "GL FBConfig count: %u", fbCount);
    if (fbCount == 0) {
        palLog(nullptr, "No supported FBConfig found");
        ret = -1;
        goto cleanup;
    }

    fbConfigs = palAllocate(nullptr, sizeof(PalGLFBConfig) * fbCount, 0);
    if (!fbConfigs) {
        palLog(nullptr, "Failed to allocate memory");
        ret = -1;
        goto cleanup;
    }

    result = palEnumerateGLFBConfigs(&fbCount, fbConfigs);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to query GL FBConfigs");
        ret = -1;
        goto cleanup;
    }

    PalGLFBConfig desired = {0};
    desired.redBits = 8;
    desired.greenBits = 8;
    desired.blueBits = 8;
    desired.alphaBits = 8;
    desired.alphaBits = 8;
    desired.depthBits = 24;
    desired.stencilBits = 8;
    desired.samples = 2;

    desired.stereo = PAL_FALSE;
    desired.sRGB = PAL_TRUE;
    desired.doubleBuffer = PAL_TRUE;

    const PalGLFBConfig* closest = nullptr;
    closest = palGetClosestGLFBConfig(fbConfigs, fbCount, &desired);

    PalWindowCreateInfo windowCreateInfo = {0};
    windowCreateInfo.monitor = nullptr;
    windowCreateInfo.height = 480;
    windowCreateInfo.width = 640;
    windowCreateInfo.show = PAL_TRUE;
    windowCreateInfo.title = "Context Window";

    windowCreateInfo.fbConfigBackend = PAL_FBCONFIG_BACKEND_PAL_OPENGL;
    windowCreateInfo.fbConfigIndex = closest->index;

    PalVideoFeatures features = palGetVideoFeatures();
    if (!(features & PAL_VIDEO_FEATURE_DECORATED_WINDOW)) {
        windowCreateInfo.style = PAL_WINDOW_STYLE_BORDERLESS;
    }

    result = palCreateWindow(&windowCreateInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create window");
        ret = -1;
        goto cleanup;
    }

    PalWindowHandleInfo winHandle = {0};
    palGetWindowHandleInfo(window, &winHandle);

    PalGLWindow glWindow = {0};
    glWindow.instance = winHandle.nativeInstance;

    // On Wayland the window is the wl_egl_window
    if (winHandle.nativeHandle3) {
        glWindow.window = winHandle.nativeHandle3;
    } else {
        glWindow.window = winHandle.nativeWindow;
    }

    const PalGLInfo* info = palGetGLInfo();

    PalGLContextCreateInfo contextCreateInfo = {0};
    contextCreateInfo.fbConfig = closest;
    contextCreateInfo.major = info->major;
    contextCreateInfo.minor = info->minor;
    contextCreateInfo.window = &glWindow;

    result = palCreateGLContext(&contextCreateInfo, &context);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create opengl context");
        ret = -1;
        goto cleanup;
    }

    result = palMakeContextCurrent(&glWindow, context);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to make opengl context current");
        ret = -1;
        goto cleanup;
    }

    if (info->extensions & PAL_GL_EXTENSION_SWAP_CONTROL) {
        palSetSwapInterval(1);
    }

    PFNGLCLEARCOLORPROC glClearColor = nullptr;
    PFNGLCLEARPROC glClear = nullptr;
    glClearColor = (PFNGLCLEARCOLORPROC)palGetGLProcAddress("glClearColor");
    glClear = (PFNGLCLEARPROC)palGetGLProcAddress("glClear");

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    PalBool running = PAL_TRUE;
    while (running) {
        palUpdateVideo();

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_TYPE_WINDOW_CLOSE: {
                    running = PAL_FALSE;
                    break;
                }

                case PAL_EVENT_TYPE_KEYDOWN: {
                    PalKeycode keycode = 0;
                    palUnpackUint32(event.data, &keycode, nullptr);
                    if (keycode == PAL_KEYCODE_ESCAPE) {
                        running = PAL_FALSE;
                    }
                    break;
                }
            }
        }

        glClear(0x00004000); // GL_COLOR_BUFFER_BIT

        result = palSwapBuffers(&glWindow, context);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to swap buffers");
            ret = -1;
            goto cleanup;
        }
    }

cleanup:
    if (window) {
        palDestroyWindow(window);
    }

    if (context) {
        palDestroyGLContext(context);
    }

    palShutdownGL();
    palShutdownVideo();

    if (eventDriver) {
        palDestroyEventDriver(eventDriver);
    }

    if (fbConfigs) {
        palFree(nullptr, fbConfigs);
    }
    
    return ret;
}