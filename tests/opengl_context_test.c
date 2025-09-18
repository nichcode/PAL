
#include "pal/pal_opengl.h"
#include "pal/pal_video.h" // for window
#include "tests.h"

static const char* g_BoolsToSting[2] = {"False", "True"};

// oepngl typedefs
typedef void(PAL_GL_APIENTRY* PFNGLCLEARCOLORPROC)(
    float red,
    float green,
    float blue,
    float alpha);

typedef void(PAL_GL_APIENTRY* PFNGLCLEARPROC)(
    Uint32 mask); // use GL typedefs if needed

bool openglContextTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Opengl Context Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    // initialize the opengl system
    PalResult result = palInitGL(nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(
            nullptr,
            "Failed to initialize opengl %s",
            palFormatResult(result));
        return false;
    }

    PalWindow* window = nullptr;
    PalGLContext* context = nullptr;
    PalWindowCreateInfo createInfo = {0};
    PalGLContextCreateInfo contextCreateInfo = {0};
    Int32 fbCount;
    bool running = false;

    // event driver
    PalEventDriver* eventDriver = nullptr;
    PalEventDriverCreateInfo eventDriverCreateInfo;

    // fill the event driver create info
    eventDriverCreateInfo.allocator = nullptr; // default allocator
    eventDriverCreateInfo.callback = nullptr;  // for callback dispatch
    eventDriverCreateInfo.queue = nullptr;     // default queue
    eventDriverCreateInfo.userData = nullptr;  // null

    // create the event driver
    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(
            nullptr,
            "Failed to create event driver %s",
            palFormatResult(result));
        return false;
    }

    // initialize the video system. We pass the event driver to recieve video
    // related events the video system does not copy the event driver, it must
    // be valid till the video system is shutdown
    result = palInitVideo(nullptr, eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(
            nullptr,
            "Failed to initialize video %s",
            palFormatResult(result));
        return false;
    }

    createInfo.monitor = nullptr; // use primary monitor
    createInfo.height = 480;
    createInfo.width = 640;
    createInfo.show = true;
    createInfo.style = PAL_WINDOW_STYLE_RESIZABLE;
    createInfo.title = "Pal Opengl Context Window";

    // create the window with the create info struct
    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create window %s", palFormatResult(result));
        return false;
    }

    // we set window close to poll
    palSetEventDispatchMode(
        eventDriver,
        PAL_EVENT_WINDOW_CLOSE,
        PAL_DISPATCH_POLL);

    // get window handle. You can use any window from any library
    // so long as you can get the window handle and display (if on X11, wayland)
    // If pal video system will not be used, there is no need to initialize it
    PalWindowHandleInfo windowHandleInfo;
    windowHandleInfo = palGetWindowHandleInfo(window);

    // PalGLWindow is just a struct to hold native handles
    PalGLWindow glWindow = {0};
    // needed when using X11 or wayland
    glWindow.display = windowHandleInfo.nativeDisplay;
    glWindow.window = windowHandleInfo.nativeWindow;

    // use the gl window to query supported FBconfigs
    result = palEnumerateGLFBConfigs(&glWindow, &fbCount, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(
            nullptr,
            "Failed to query GL FBConfigs %s",
            palFormatResult(result));
        return false;
    }

    palLog(nullptr, "GL FBConfig count: %d", fbCount);
    if (fbCount == 0) {
        palLog(nullptr, "No supported FBConfig found");
        return false;
    }

    PalGLFBConfig* fbConfigs = nullptr;
    fbConfigs = palAllocate(nullptr, sizeof(PalGLFBConfig) * fbCount, 0);

    if (!fbConfigs) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    result = palEnumerateGLFBConfigs(&glWindow, &fbCount, fbConfigs);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(
            nullptr,
            "Failed to query GL FBConfigs %s",
            palFormatResult(result));
        palFree(nullptr, fbConfigs);
        return false;
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

    desired.stereo = false; // not widely supported
    desired.sRGB = true;
    desired.doubleBuffer = true;

    // get the closest
    const PalGLFBConfig* closest = nullptr;
    closest = palGetClosestGLFBConfig(fbConfigs, fbCount, &desired);

    // log the closest GL FBConfig
    palLog(nullptr, "Closest GL FBConfig:");
    palLog(nullptr, " Index: %d", closest->index);
    palLog(nullptr, " Red Bits: %d", closest->redBits);
    palLog(nullptr, " Green Bits: %d", closest->greenBits);
    palLog(nullptr, " Blue Bits: %d", closest->blueBits);
    palLog(nullptr, " Alpha Bits: %d", closest->alphaBits);
    palLog(nullptr, " Depth Bits: %d", closest->depthBits);
    palLog(nullptr, " Stencil Bits: %d", closest->stencilBits);

    palLog(nullptr, " Samples: %d", closest->samples);
    palLog(nullptr, " DoubleBuffer: %s", g_BoolsToSting[closest->doubleBuffer]);
    palLog(nullptr, " Stereo: %s", g_BoolsToSting[closest->stereo]);
    palLog(nullptr, " sRGB: %s", g_BoolsToSting[closest->sRGB]);
    palLog(nullptr, "");

    // get opengl info
    const PalGLInfo* info = palGetGLInfo();

    // fill the context create info with the closest FBConfig
    contextCreateInfo.debug = true;        // debug context
    contextCreateInfo.fbConfig = closest;  // we use the closest to what we want
    contextCreateInfo.major = info->major; // context major
    contextCreateInfo.minor = info->minor; // context minor
    contextCreateInfo.noError = false;     // check PAL_GL_EXTENSION_NO_ERROR

    // check PAL_GL_EXTENSION_FLUSH_CONTROL
    contextCreateInfo.release = PAL_GL_RELEASE_BEHAVIOR_NONE;

    // check PAL_GL_EXTENSION_ROBUSTNESS
    contextCreateInfo.reset = PAL_GL_CONTEXT_RESET_NONE;

    // if true, check PAL_GL_EXTENSION_ROBUSTNESS
    contextCreateInfo.robustness = false;

    contextCreateInfo.shareContext = nullptr; // no shared context
    contextCreateInfo.window = &glWindow;

    if (info->extensions & PAL_GL_EXTENSION_CREATE_CONTEXT) {
        contextCreateInfo.forward = true;
    }

    if (info->extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE) {
        contextCreateInfo.profile = PAL_GL_PROFILE_CORE;
    }

    // create the opengl context with the context create info
    result = palCreateGLContext(&contextCreateInfo, &context);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(
            nullptr,
            "Failed to create opengl context: %s",
            palFormatResult(result));
        palFree(nullptr, fbConfigs);
        return false;
    }

    // make the context current and optionally set vsync if supported
    result = palMakeContextCurrent(&glWindow, context);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(
            nullptr,
            "Failed to make opengl context current: %s",
            palFormatResult(result));
        palFree(nullptr, fbConfigs);
        return false;
    }

    if (info->extensions & PAL_GL_EXTENSION_SWAP_CONTROL) {
        // vsync is supported. This is set for the current context
        palSetSwapInterval(1);
    }

    // load function procs
    PFNGLCLEARCOLORPROC glClearColor = nullptr;
    PFNGLCLEARPROC glClear = nullptr;

    glClearColor = (PFNGLCLEARCOLORPROC)palGLGetProcAddress("glClearColor");
    glClear = (PFNGLCLEARPROC)palGLGetProcAddress("glClear");

    // set clear color
    glClearColor(.2f, .2f, .2f, .2f);

    running = true;
    while (running) {
        // update the video system to push video events
        palUpdateVideo();

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_WINDOW_CLOSE: {
                    running = false;
                    break;
                }
            }
        }

        // clear the buffer
        glClear(0x00004000); // GL_COLOR_BUFFER_BIT

        // swap buffers
        result = palSwapBuffers(&glWindow, context);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(
                nullptr,
                "Failed to swap buffers: %s",
                palFormatResult(result));
            palFree(nullptr, fbConfigs);
            return false;
        }
    }

    // destroy the window
    palDestroyWindow(window);

    // shutdown the video system
    palShutdownVideo();

    // destroy the event driver
    palDestroyEventDriver(eventDriver);

    // destroy the opengl context
    palDestroyGLContext(context);

    // shutdown the opengl system
    palShutdownGL();

    // free the framebuffer configs
    palFree(nullptr, fbConfigs);

    return true;
}