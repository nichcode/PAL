
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

typedef void(PAL_GL_APIENTRY* PFNGLCLEARPROC)(Uint32 mask); // use GL typedefs if needed

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
    bool driverCreated;
    bool running;
    bool fixedPipeline;
    PalEventDriver* videoEventDriver;
    PalEventDriver* openglEventDriver;
    PalGLContext* context;
    PalGLWindow window;
} SharedState;

static void* PAL_CALL eventDriverWorker(void* arg)
{
    PalResult result;
    SharedState* shared = (SharedState*)arg;
    if (!shared) {
        palLog(nullptr, "Failed to get thread arg");
        return nullptr;
    }

    PalEventDriverCreateInfo createInfo = {0};
    result = palCreateEventDriver(&createInfo, &shared->videoEventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create event driver: %s", error);
        return nullptr;
    }

    // create the opengl driver as well
    result = palCreateEventDriver(&createInfo, &shared->openglEventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create event driver: %s", error);
        return nullptr;
    }

    // set dispatch modes. opengl needs only window resize
    palSetEventDispatchMode(shared->openglEventDriver, PAL_EVENT_WINDOW_SIZE, PAL_DISPATCH_POLL);

    // video needs window close and resize
    palSetEventDispatchMode(shared->videoEventDriver, PAL_EVENT_WINDOW_CLOSE, PAL_DISPATCH_POLL);
    palSetEventDispatchMode(shared->videoEventDriver, PAL_EVENT_WINDOW_SIZE, PAL_DISPATCH_POLL);
    palSetEventDispatchMode(shared->videoEventDriver, PAL_EVENT_KEYDOWN, PAL_DISPATCH_POLL);

    // we are done
    shared->driverCreated = true;
    return nullptr;
}

static void* PAL_CALL rendererWorkder(void* arg)
{
    PalResult result;
    SharedState* shared = (SharedState*)arg;
    if (!shared) {
        palLog(nullptr, "Failed to get thread arg");
        return nullptr;
    }

    // make the context current on the renderer thread
    result = palMakeContextCurrent(&shared->window, shared->context);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to make opengl context current: %s", error);
        return nullptr;
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

    glClearColor = (PFNGLCLEARCOLORPROC)palGLGetProcAddress("glClearColor");
    glClear = (PFNGLCLEARPROC)palGLGetProcAddress("glClear");
    glBegin = (glBeginFn)palGLGetProcAddress("glBegin");
    glEnd = (glEndFn)palGLGetProcAddress("glEnd");
    glVertex2f = (glVertex2fFn)palGLGetProcAddress("glVertex2f");
    glColor3f = (glColor3fFn)palGLGetProcAddress("glColor3f");
    glFlush = (glFlushFn)palGLGetProcAddress("glFlush");

    glGetError = (glGetErrorFn)palGLGetProcAddress("glGetError");
    glViewport = (glViewportFn)palGLGetProcAddress("glViewport");

    // set clear color
    glViewport(0, 0, 640, 480);
    glClearColor(.2f, .2f, .2f, 1.0f);

    // run our while loop over there
    while (shared->running) {
        PalEvent event;
        while (palPollEvent(shared->openglEventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_WINDOW_SIZE: {
                    Uint32 width, height;
                    palUnpackUint32(event.data, &width, &height);
                    palLog(nullptr, "Video driver sent a resize event (%d, %d)", width, height);

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
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to swap buffers from: %s", error);
            return nullptr;
        }
    }

    return nullptr;
}

bool multiThreadOpenGlTest()
{
    palLog(nullptr, "Press Escape or click close button to close Test");
    PalResult result;
    PalThread* eventDriverThread = nullptr;

    SharedState* shared = nullptr;
    shared = palAllocate(nullptr, sizeof(SharedState), 0);
    if (!shared) {
        palLog(nullptr, "Failed to allocate shared state");
        return false;
    }

    // create a thread that creates two event drivers
    PalThreadCreateInfo threadCreateInfo = {0};
    threadCreateInfo.entry = eventDriverWorker;
    threadCreateInfo.arg = (void*)shared;
    result = palCreateThread(&threadCreateInfo, &eventDriverThread);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create thread: %s", error);
        return false;
    }

    // check to see if the event driver thread is done creating the drivers
    // if not we wait for it
    if (!shared->driverCreated) {
        // this will be detached automatically when done
        palJoinThread(eventDriverThread, nullptr);

    } else {
        palDetachThread(eventDriverThread); // we dont need it anymore
    }

    // initialize the video system. We pass the event driver to recieve video
    // related events the video system does not copy the event driver, it must
    // be valid till the video system is shutdown
    result = palInitVideo(nullptr, shared->videoEventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize video: %s", error);
        return false;
    }

    // get the instance or display handle and pass it to the opengl system
    // This must be called before the opengl system is initialized
    palGLSetInstance(palGetInstance());

    // initialize video and opengl systems
    // we need to initialize these on the main thread
    result = palInitGL(nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize opengl: %s", error);
        return false;
    }

    // get all FBConfigs and select one
    Int32 fbCount = 0;
    result = palEnumerateGLFBConfigs(nullptr, &fbCount, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to query GL FBConfigs: %s", error);
        return false;
    }

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

    result = palEnumerateGLFBConfigs(nullptr, &fbCount, fbConfigs);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to query GL FBConfigs: %s", error);
        palFree(nullptr, fbConfigs);
        return false;
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
    desired.stereo = false; // not widely supported
    desired.sRGB = true;
    desired.doubleBuffer = true;

    const PalGLFBConfig* closest = nullptr;
    closest = palGetClosestGLFBConfig(fbConfigs, fbCount, &desired);

    // tell the video system to use our closest FBConfig
    // to create the windows
    result = palSetFBConfig(closest->index, PAL_CONFIG_BACKEND_PAL_OPENGL);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to set FBConfig: %s", error);
        return false;
    }

    // if not using pal_opengl with pal_video
    // we get the backend string from the opengl system and
    // get the backend from it
    // Possible values are `wgl`, `glx`, `gles`, `egl`.
    // PalFBConfigBackend backend;
    // const char* glBackendString = palGLGetBackend();
    // if (strcmp(glBackendString, "wgl") == 0) {
    //     backend = PAL_CONFIG_BACKEND_WGL;

    // } else if (strcmp(glBackendString, "glx") == 0) {
    //     backend = PAL_CONFIG_BACKEND_GLX;

    // } else if (strcmp(glBackendString, "gles") == 0) {
    //     backend = PAL_CONFIG_BACKEND_GLES;

    // } else if (strcmp(glBackendString, "egl") == 0) {
    //     backend = PAL_CONFIG_BACKEND_EGL;
    // }

    // all windows also needs to be created on the main thread
    PalWindow* window = nullptr;
    PalWindowCreateInfo windowCreateInfo = {0};
    windowCreateInfo.width = 640;
    windowCreateInfo.height = 480;
    windowCreateInfo.show = true;
    windowCreateInfo.style = PAL_WINDOW_STYLE_RESIZABLE;
    windowCreateInfo.title = "Multi Thread OpenGL Window";

    // check if we support decorated windows (title bar, close etc)
    PalVideoFeatures64 features = palGetVideoFeaturesEx();
    if (!(features & PAL_VIDEO_FEATURE64_DECORATED_WINDOW)) {
        // if we dont support, we need to create a borderless window
        // and create the decorations ourselves
        windowCreateInfo.style |= PAL_WINDOW_STYLE_BORDERLESS;
    }

    result = palCreateWindow(&windowCreateInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create window: %s", error);
        return false;
    }

    // GL context needs to be created on the main thread
    const PalGLInfo* glInfo = palGetGLInfo();

    // get the native handles of our created window
    PalWindowHandleInfoEx winHandle = {0};
    winHandle = palGetWindowHandleInfoEx(window);

    shared->window.display = winHandle.nativeDisplay;

    // On Wayland the window is the wl_egl_window
    if (winHandle.nativeHandle3) {
        // the window has a valid wl_egl_window
        shared->window.window = winHandle.nativeHandle3;

    } else {
        shared->window.window = winHandle.nativeWindow;
    }

    PalGLContextCreateInfo contextCreateInfo = {0};
    contextCreateInfo.debug = true;
    contextCreateInfo.fbConfig = closest;
    contextCreateInfo.major = glInfo->major;
    contextCreateInfo.minor = glInfo->minor;
    contextCreateInfo.window = &shared->window;

    // we dont want to get into GL pipeline for this example
    // so we request a Compatibility profile if supported
    // NOTE: is its not supported, no triangle would be displayed
    shared->fixedPipeline = false;
    if (glInfo->extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE) {
        contextCreateInfo.profile = PAL_GL_PROFILE_COMPATIBILITY;
        shared->fixedPipeline = true;
    }

    if (!shared->fixedPipeline) {
        palLog(nullptr, "Fixed pipeline not supported");
    }

    result = palCreateGLContext(&contextCreateInfo, &shared->context);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create opengl context: %s", error);
        palFree(nullptr, fbConfigs);
        return false;
    }

    shared->running = true;

    // we create a renderer thread for opengl
    // we dont wait for the renderer thread since it has its own while loop
    // we just passed events between the two event drivers
    // create a thread that creates two event drivers
    PalThread* rendererThread = nullptr;
    threadCreateInfo.entry = rendererWorkder;
    threadCreateInfo.arg = (void*)shared;
    result = palCreateThread(&threadCreateInfo, &rendererThread);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create thread: %s", error);
        return false;
    }

    // we run the video while loop here
    while (shared->running) {
        // update the video system to push video events
        palUpdateVideo();

        PalEvent event;
        while (palPollEvent(shared->videoEventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_WINDOW_CLOSE: {
                    shared->running = false;
                    break;
                }

                case PAL_EVENT_KEYDOWN: {
                    PalKeycode keycode = 0;
                    palUnpackUint32(event.data, &keycode, nullptr);
                    if (keycode == PAL_KEYCODE_ESCAPE) {
                        shared->running = false;
                    }
                    break;
                }

                case PAL_EVENT_WINDOW_SIZE: {
                    // tell the opengl driver about our size change
                    palPushEvent(shared->openglEventDriver, &event);
                    break;
                }
            }
        }
    }

    // we wait for the render thread to finish with
    // the current frame and destroy the context
    palJoinThread(rendererThread, nullptr);

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

    return true;
}
