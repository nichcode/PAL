
#include "pal2/pal_thread.h"
#include "opengl_helper.h"

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

    // clang-format off
    PalEventType eventTypes[3] = {
        PAL_EVENT_TYPE_WINDOW_CLOSE,
        PAL_EVENT_TYPE_KEYDOWN,
        PAL_EVENT_TYPE_WINDOW_SIZE};
    // clang-format on

    PalEventType glDriverEventTypes[1] = { PAL_EVENT_TYPE_WINDOW_SIZE };
    PalDispatchMode mode = PAL_DISPATCH_MODE_POLL;

    shared->videoEventDriver = helperCreateEventDriver(3, mode, eventTypes);
    shared->openglEventDriver = helperCreateEventDriver(1, mode, glDriverEventTypes);
    if (!shared->videoEventDriver || !shared->openglEventDriver) {
        return nullptr;
    }

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

    PalWindow* window = nullptr;
    PalGLFBConfig fbConfig = {0};
    PalGLWindow glWindow = {0};
    PalGLContext* context = nullptr;

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

    PalEventDriver* eventDriver = shared->videoEventDriver;
    window = glHelperCreateWindow("GL Multi Thread Window", eventDriver, &fbConfig);
    if (!window) {
        return PAL_FALSE;
    }

    glWindow = glHelperGetGLWindow(window);
    if (!glWindow.window) {
        return PAL_FALSE;
    }

    const PalGLInfo* info = palGetGLInfo();

    PalGLContextCreateInfo createInfo = {0};
    createInfo.debug = PAL_TRUE;
    createInfo.fbConfig = &fbConfig;
    createInfo.major = info->major;
    createInfo.minor = info->minor;
    createInfo.window = &glWindow;

    result = palCreateGLContext(&createInfo, &context);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create GL context");
        return PAL_FALSE;
    }

    // we dont want to get into GL pipeline for this example
    // so we request a Compatibility profile if supported
    // NOTE: is its not supported, no triangle would be displayed
    shared->fixedPipeline = PAL_FALSE;
    if (info->extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE) {
        createInfo.profile = PAL_GL_PROFILE_COMPATIBILITY;
        shared->fixedPipeline = PAL_TRUE;
    }

    if (!shared->fixedPipeline) {
        palLog(nullptr, "Fixed pipeline not supported");
    }

    result = palCreateGLContext(&createInfo, &shared->context);
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
    glHelperDestroyWindow(window, shared->videoEventDriver);
    palDestroyEventDriver(shared->openglEventDriver);

    return PAL_TRUE;
}
