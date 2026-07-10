
#include "pal/pal_opengl.h"
#include "pal/pal_video.h" // for window
#include "tests.h"

static const char* g_BoolsToSting[2] = {"False", "True"};

// opengl typedefs
typedef void(PAL_GL_APIENTRY* PFNGLCLEARCOLORPROC)(
    float red,
    float green,
    float blue,
    float alpha);

typedef void(PAL_GL_APIENTRY* PFNGLCLEARPROC)(uint32_t mask); // use GL typedefs if needed

PalBool openglMultiContextTest()
{
    palLog(nullptr, "Press Escape or click close button to close Test");

    // fill the event driver create info
    PalEventDriverCreateInfo eventDriverCreateInfo = {0};
    eventDriverCreateInfo.allocator = nullptr; // default allocator
    eventDriverCreateInfo.callback = nullptr;  // for callback dispatch
    eventDriverCreateInfo.queue = nullptr;     // default queue
    eventDriverCreateInfo.userData = nullptr;  // null

    // create the event driver
    PalEventDriver* eventDriver = nullptr;
    PalResult result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create event driver");
        return PAL_FALSE;
    }

    // initialize the video system. We pass the event driver to recieve video
    // related events the video system does not copy the event driver, it must
    // be valid till the video system is shutdown
    result = palInitVideo(nullptr, eventDriver, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to initialize video");
        return PAL_FALSE;
    }

    // check if opengl API is supported or fallback to opengl es
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

    // initialize the opengl system. This loads the icd.
    result = palInitGL(openglAPI, videoInstance, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to initialize opengl");
        return PAL_FALSE;
    }
    
    // enumerate supported opengl framebuffer configs
    int32_t fbCount = 0;
    result = palEnumerateGLFBConfigs(&fbCount, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to query GL FBConfigs");
        return PAL_FALSE;
    }

    palLog(nullptr, "GL FBConfig count: %d", fbCount);
    if (fbCount == 0) {
        palLog(nullptr, "No supported FBConfig found");
        return PAL_FALSE;
    }

    PalGLFBConfig* fbConfigs = nullptr;
    fbConfigs = palAllocate(nullptr, sizeof(PalGLFBConfig) * fbCount, 0);
    if (!fbConfigs) {
        palLog(nullptr, "Failed to allocate memory");
        return PAL_FALSE;
    }

    // enumerate supported opengl framebuffer configs
    result = palEnumerateGLFBConfigs(&fbCount, fbConfigs);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to query GL FBConfigs");
        return PAL_FALSE;
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

    desired.stereo = PAL_FALSE; // not widely supported
    desired.sRGB = PAL_TRUE;
    desired.doubleBuffer = PAL_TRUE;

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

    PalWindowCreateInfo createInfo = {0};
    createInfo.monitor = nullptr; // use default monitor
    createInfo.height = 480;
    createInfo.width = 640;
    createInfo.show = PAL_TRUE;
    createInfo.style = PAL_WINDOW_STYLE_RESIZABLE;
    createInfo.title = "Opengl Multi Context Window";

    // check if we support decorated windows (title bar, close etc)
    PalVideoFeatures features = palGetVideoFeatures();
    if (!(features & PAL_VIDEO_FEATURE_DECORATED_WINDOW)) {
        // if we dont support, we need to create a borderless window
        // and create the decorations ourselves
        createInfo.style |= PAL_WINDOW_STYLE_BORDERLESS;
    }

    // set the backend and the fbConfig. We use PAL_FBCONFIG_BACKEND_PAL_OPENGL
    // because we are using both pal_video and pal_opengl
    createInfo.fbConfigBackend = PAL_FBCONFIG_BACKEND_PAL_OPENGL;
    createInfo.fbConfigIndex = closest->index;
    
    // create the window with the create info struct
    PalWindow* window = nullptr;
    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create window");
        return PAL_FALSE;
    }

    // we set window close to poll
    palSetEventDispatchMode(eventDriver, PAL_EVENT_TYPE_WINDOW_CLOSE, PAL_DISPATCH_MODE_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_TYPE_KEYDOWN, PAL_DISPATCH_MODE_POLL);

    // get window handle. You can use any window from any library
    // so long as you can get the window handle and display (if on X11, wayland)
    // If pal video system will not be used, there is no need to initialize it
    PalWindowHandleInfo winHandle = {0};
    palGetWindowHandleInfo(window, &winHandle);

    // PalGLWindow is just a struct to hold native handles
    PalGLWindow glWindow = {0};
    glWindow.instance = winHandle.nativeInstance;

    // On Wayland the window is the wl_egl_window
    if (winHandle.nativeHandle3) {
        // the window has a valid wl_egl_window
        glWindow.window = winHandle.nativeHandle3;

    } else {
        glWindow.window = winHandle.nativeWindow;
    }

    // get opengl info
    const PalGLInfo* info = palGetGLInfo();

    // fill the context create info with the closest FBConfig
    PalGLContextCreateInfo contextCreateInfo = {0};
    contextCreateInfo.debug = PAL_TRUE;        // debug context
    contextCreateInfo.fbConfig = closest;  // we use the closest to what we want
    contextCreateInfo.major = info->major; // context major
    contextCreateInfo.minor = info->minor; // context minor
    contextCreateInfo.noError = PAL_FALSE;     // check PAL_GL_EXTENSION_NO_ERROR

    // check PAL_GL_EXTENSION_FLUSH_CONTROL
    contextCreateInfo.release = PAL_GL_RELEASE_BEHAVIOR_NONE;

    // check PAL_GL_EXTENSION_ROBUSTNESS
    contextCreateInfo.reset = PAL_GL_CONTEXT_RESET_NONE;

    contextCreateInfo.shareContext = nullptr; // no shared context
    contextCreateInfo.window = &glWindow;

    if (info->extensions & PAL_GL_EXTENSION_CREATE_CONTEXT) {
        contextCreateInfo.forward = PAL_TRUE;
    }

    if (info->extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE) {
        contextCreateInfo.profile = PAL_GL_PROFILE_CORE;
    }

    // create the opengl context with the context create info
    PalGLContext* context = nullptr;
    result = palCreateGLContext(&contextCreateInfo, &context);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create opengl context");
        return PAL_FALSE;
    }

    // make the context current and optionally set vsync if supported
    result = palMakeContextCurrent(&glWindow, context);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to make opengl context current");
        return PAL_FALSE;
    }

    if (info->extensions & PAL_GL_EXTENSION_SWAP_CONTROL) {
        // vsync is supported. This is set for the current context
        palSetSwapInterval(1);
    }

    // load function procs
    PFNGLCLEARCOLORPROC glClearColor = nullptr;
    PFNGLCLEARPROC glClear = nullptr;
    glClearColor = (PFNGLCLEARCOLORPROC)palGetGLProcAddress("glClearColor");
    glClear = (PFNGLCLEARPROC)palGetGLProcAddress("glClear");

    // set clear color
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    PalBool running = PAL_TRUE;
    while (running) {
        // update the video system to push video events
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

        // clear the buffer
        glClear(0x00004000); // GL_COLOR_BUFFER_BIT

        // swap buffers
        result = palSwapBuffers(&glWindow, context);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to swap buffers");
            return PAL_FALSE;
        }
    }

    // destroy the opengl context.
    palDestroyGLContext(context);

    // create a new opengl context with the same window
    // the FBConfig of the new context must match the windows
    context = nullptr;
    result = palCreateGLContext(&contextCreateInfo, &context);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create opengl context");
        return PAL_FALSE;
    }

    // make the context current on this thread
    result = palMakeContextCurrent(&glWindow, context);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to make opengl context current");
        return PAL_FALSE;
    }

    glClearColor(.2f, .6f, .6f, 1.0f);
    running = PAL_TRUE;
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
            logResult(result, "Failed to swap buffers");
            return PAL_FALSE;
        }
    }

    // destroy the opengl context
    palDestroyGLContext(context);

    // shutdown the opengl system
    palShutdownGL();

    // destroy the window
    palDestroyWindow(window);

    // shutdown the video system
    palShutdownVideo();

    // destroy the event driver
    palDestroyEventDriver(eventDriver);

    // free the framebuffer configs
    palFree(nullptr, fbConfigs);

    return PAL_TRUE;
}
