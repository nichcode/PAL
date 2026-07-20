
#include "pal/pal_opengl.h"
#include "pal/pal_thread.h"
#include "pal/pal_video.h"
#include "tests.h"

// opengl typedefs
typedef void(PAL_GL_APIENTRY* PFNGLCLEARCOLORPROC)(
    float red,
    float green,
    float blue,
    float alpha);

typedef void(PAL_GL_APIENTRY* PFNGLCLEARPROC)(uint32_t mask); // use GL typedefs if needed

typedef void (*glFlushFn)();
typedef void (*glBeginFn)(unsigned int);
typedef void (*glEndFn)();
typedef unsigned int (*glGetErrorFn)();

typedef void (*glVertex2fFn)(
    float,
    float);

typedef void (*glColor3fFn)(
    float,
    float,
    float);

typedef void (*glViewportFn)(
    int,
    int,
    int,
    int);

typedef struct {
    PalBool driverCreated;
    PalBool running;
    PalBool fixedPipeline;
    PalEventDriver* videoEventDriver;
    PalEventDriver* openglEventDriver;
    PalGLContext* context;
    PalGLWindow window;
} SharedState;

static void* PAL_CALL eventDriverWorker(void* arg)
{
    SharedState* shared = (SharedState*)arg;
    if (!shared) {
        palLog(nullptr, "Failed to get thread arg");
        return nullptr;
    }

    PalEventDriverCreateInfo createInfo = {0};
    PalResult result = palCreateEventDriver(&createInfo, &shared->videoEventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create event driver");
        return nullptr;
    }

    // create the opengl driver as well
    result = palCreateEventDriver(&createInfo, &shared->openglEventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create event driver");
        return nullptr;
    }

    // set dispatch modes. opengl needs only window resize
    palSetEventDispatchMode(
        shared->openglEventDriver,
        PAL_EVENT_TYPE_WINDOW_SIZE,
        PAL_DISPATCH_MODE_POLL);

    // video needs window close and resize
    palSetEventDispatchMode(
        shared->videoEventDriver,
        PAL_EVENT_TYPE_WINDOW_CLOSE,
        PAL_DISPATCH_MODE_POLL);

    palSetEventDispatchMode(
        shared->videoEventDriver,
        PAL_EVENT_TYPE_WINDOW_SIZE,
        PAL_DISPATCH_MODE_POLL);

    palSetEventDispatchMode(
        shared->videoEventDriver,
        PAL_EVENT_TYPE_KEYDOWN,
        PAL_DISPATCH_MODE_POLL);

    // we are done
    shared->driverCreated = PAL_TRUE;
    return nullptr;
}

static void* PAL_CALL rendererWorkder(void* arg)
{
    SharedState* shared = (SharedState*)arg;
    if (!shared) {
        palLog(nullptr, "Failed to get thread arg");
        return nullptr;
    }

    // make the context current on the renderer thread
    PalResult result = palMakeContextCurrent(&shared->window, shared->context);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to make opengl context current");
        return PAL_FALSE;
    }

    // load function procs
    PFNGLCLEARCOLORPROC glClearColor = nullptr;
    PFNGLCLEARPROC glClear = nullptr;
    glBeginFn glBegin;
    glEndFn glEnd;
    glVertex2fFn glVertex2f;
    glColor3fFn glColor3f;
    glFlushFn glFlush;
    glGetErrorFn glGetError;
    glViewportFn glViewport;

    glClearColor = (PFNGLCLEARCOLORPROC)palGetGLProcAddress("glClearColor");
    glClear = (PFNGLCLEARPROC)palGetGLProcAddress("glClear");
    glBegin = (glBeginFn)palGetGLProcAddress("glBegin");
    glEnd = (glEndFn)palGetGLProcAddress("glEnd");
    glVertex2f = (glVertex2fFn)palGetGLProcAddress("glVertex2f");
    glColor3f = (glColor3fFn)palGetGLProcAddress("glColor3f");
    glFlush = (glFlushFn)palGetGLProcAddress("glFlush");

    glGetError = (glGetErrorFn)palGetGLProcAddress("glGetError");
    glViewport = (glViewportFn)palGetGLProcAddress("glViewport");

    // set clear color
    glViewport(0, 0, 640, 480);
    glClearColor(.2f, .2f, .2f, 1.0f);

    // run our while loop over there
    while (shared->running) {
        PalEvent event;
        while (palPollEvent(shared->openglEventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_TYPE_WINDOW_SIZE: {
                    uint32_t width, height;
                    palUnpackUint32(event.data, &width, &height);
                    palLog(
                        nullptr,
                        "Video event driver sent a resize event (%d, %d)",
                        width,
                        height);

                    glViewport(0, 0, width, height);
                    // we can optionally send back a user event
                    // to let the video driver know we have recieved the event
                    break;
                }
            }
        }

        // clear the buffer
        glClear(0x00004000); // GL_COLOR_BUFFER_BIT

        // draw a triangle using the fixed pipeline
        if (shared->fixedPipeline) {
            glBegin(0x0004); // GL_TRIANGLES
            glColor3f(1.0, 0.0, 0.0);
            glVertex2f(-0.5f, -0.5f);

            glColor3f(0.0, 1.0, 0.0);
            glVertex2f(0.5f, -0.5f);

            glColor3f(0.0, 0.0, 1.0);
            glVertex2f(0.0f, 0.5f);

            glEnd();
            glFlush();
        }

        // swap buffers
        result = palSwapBuffers(&shared->window, shared->context);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to swap buffers");
            return nullptr;
        }
    }

    return nullptr;
}

PalBool multiThreadOpenGlTest()
{
    palLog(nullptr, "Press Escape or click close button to close Test");

    SharedState* shared = nullptr;
    shared = palAllocate(nullptr, sizeof(SharedState), 0);
    if (!shared) {
        palLog(nullptr, "Failed to allocate shared state");
        return PAL_FALSE;
    }
    memset(shared, 0, sizeof(SharedState));

    // create a thread that creates two event drivers
    PalThreadCreateInfo threadCreateInfo = {0};
    threadCreateInfo.entry = eventDriverWorker;
    threadCreateInfo.arg = (void*)shared;

    PalThread* eventDriverThread = nullptr;
    PalResult result = palCreateThread(&threadCreateInfo, &eventDriverThread);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create thread");
        return PAL_FALSE;
    }

    // check to see if the event driver thread is done creating the drivers
    // if not we wait for it
    if (!shared->driverCreated) {
        // this will be detached automatically when done
        result = palJoinThread(eventDriverThread, nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to join thread");
            return PAL_FALSE;
        }

    } else {
        palDetachThread(eventDriverThread); // we dont need it anymore
    }

    // initialize the video system. We pass the event driver to recieve video
    // related events the video system does not copy the event driver, it must
    // be valid till the video system is shutdown
    result = palInitVideo(nullptr, shared->videoEventDriver, nullptr);
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

    // get all FBConfigs and select one
    int32_t fbCount = 0;
    result = palEnumerateGLFBConfigs(&fbCount, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to query GL FBConfigs");
        return PAL_FALSE;
    }

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

    result = palEnumerateGLFBConfigs(&fbCount, fbConfigs);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to query GL FBConfigs");
        return PAL_FALSE;
    }

    // we get our desired FBConfig
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

    const PalGLFBConfig* closest = nullptr;
    closest = palGetClosestGLFBConfig(fbConfigs, fbCount, &desired);

    // all windows also needs to be created on the main thread
    PalWindowCreateInfo windowCreateInfo = {0};
    windowCreateInfo.width = 640;
    windowCreateInfo.height = 480;
    windowCreateInfo.show = PAL_TRUE;
    windowCreateInfo.style = PAL_WINDOW_STYLE_RESIZABLE;
    windowCreateInfo.title = "Multi Thread OpenGL Window";

    // check if we support decorated windows (title bar, close etc)
    PalVideoFeatures features = palGetVideoFeatures();
    if (!(features & PAL_VIDEO_FEATURE_DECORATED_WINDOW)) {
        // if we dont support, we need to create a borderless window
        // and create the decorations ourselves
        windowCreateInfo.style |= PAL_WINDOW_STYLE_BORDERLESS;
    }

    // set the backend and the fbConfig. We use PAL_FBCONFIG_BACKEND_PAL_OPENGL
    // because we are using both pal_video and pal_opengl
    windowCreateInfo.fbConfigBackend = PAL_FBCONFIG_BACKEND_PAL_OPENGL;
    windowCreateInfo.fbConfigIndex = closest->index;

    PalWindow* window = nullptr;
    result = palCreateWindow(&windowCreateInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create window");
        return PAL_FALSE;
    }

    // GL context needs to be created on the main thread
    const PalGLInfo* glInfo = palGetGLInfo();

    // get the native handles of our created window
    PalWindowHandleInfo winHandle = {0};
    palGetWindowHandleInfo(window, &winHandle);

    shared->window.instance = winHandle.nativeInstance;
    // On Wayland the window is the wl_egl_window
    if (winHandle.nativeHandle3) {
        // the window has a valid wl_egl_window
        shared->window.window = winHandle.nativeHandle3;

    } else {
        shared->window.window = winHandle.nativeWindow;
    }

    PalGLContextCreateInfo contextCreateInfo = {0};
    contextCreateInfo.debug = PAL_TRUE;
    contextCreateInfo.fbConfig = closest;
    contextCreateInfo.major = glInfo->major;
    contextCreateInfo.minor = glInfo->minor;
    contextCreateInfo.window = &shared->window;

    // we dont want to get into GL pipeline for this example
    // so we request a Compatibility profile if supported
    // NOTE: is its not supported, no triangle would be displayed
    shared->fixedPipeline = PAL_FALSE;
    if (glInfo->extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE) {
        contextCreateInfo.profile = PAL_GL_PROFILE_COMPATIBILITY;
        shared->fixedPipeline = PAL_TRUE;
    }

    if (!shared->fixedPipeline) {
        palLog(nullptr, "Fixed pipeline not supported");
    }

    result = palCreateGLContext(&contextCreateInfo, &shared->context);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create opengl context");
        return PAL_FALSE;
    }

    shared->running = PAL_TRUE;

    // we create a renderer thread for opengl
    // we dont wait for the renderer thread since it has its own while loop
    // we just passed events between the two event drivers
    // create a thread that creates two event drivers
    PalThread* rendererThread = nullptr;
    threadCreateInfo.entry = rendererWorkder;
    threadCreateInfo.arg = (void*)shared;
    result = palCreateThread(&threadCreateInfo, &rendererThread);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create thread");
        return PAL_FALSE;
    }

    // we run the video while loop here
    while (shared->running) {
        // update the video system to push video events
        palUpdateVideo();

        PalEvent event;
        while (palPollEvent(shared->videoEventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_TYPE_WINDOW_CLOSE: {
                    shared->running = PAL_FALSE;
                    break;
                }

                case PAL_EVENT_TYPE_KEYDOWN: {
                    PalKeycode keycode = 0;
                    palUnpackUint32(event.data, &keycode, nullptr);
                    if (keycode == PAL_KEYCODE_ESCAPE) {
                        shared->running = PAL_FALSE;
                    }
                    break;
                }

                case PAL_EVENT_TYPE_WINDOW_SIZE: {
                    // tell the opengl driver about our size change
                    palPushEvent(shared->openglEventDriver, &event);
                    break;
                }
            }
        }
    }

    // we wait for the render thread to finish with
    // the current frame and destroy the context
    result = palJoinThread(rendererThread, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to join thread");
        return PAL_FALSE;
    }

    palDestroyGLContext(shared->context);
    palShutdownGL();

    // shutdown video and opengl systems
    // we need to shutdown these on the main thread
    palDestroyWindow(window);
    palShutdownVideo();

    // The event drivers cn be destroyed on a seperate thread
    // but we destroy them here for simplicity
    palDestroyEventDriver(shared->videoEventDriver);
    palDestroyEventDriver(shared->openglEventDriver);
    palFree(nullptr, fbConfigs);

    return PAL_TRUE;
}
