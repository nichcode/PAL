
#include "opengl_helper.h"

PalBool openglContextTest()
{
    palLog(nullptr, "Press Escape or click close button to close Test");

    PalWindow* window = nullptr;
    PalEventDriver* eventDriver = nullptr;
    PalGLFBConfig fbConfig = {0};
    PalGLWindow glWindow = {0};
    PalGLContext* context = nullptr;

    eventDriver = helperCreateEventDriver(0, PAL_DISPATCH_MODE_POLL, nullptr);
    if (!eventDriver) {
        return PAL_FALSE;
    }

    window = glHelperCreateWindow("GL Context Window", eventDriver, &fbConfig);
    if (!window) {
        return PAL_FALSE;
    }

    const PalGLInfo* glInfo = palGetGLInfo();
    glWindow = glHelperGetGLWindow(window);
    if (!glWindow.window) {
        return PAL_FALSE;
    }

    PalGLContextCreateInfo createInfo = {0};
    createInfo.debug = PAL_FALSE;
    createInfo.fbConfig = &fbConfig;
    createInfo.major = glInfo->major;
    createInfo.minor = glInfo->minor;

    createInfo.noError = PAL_FALSE; // PAL_GL_EXTENSION_NO_ERROR
    createInfo.release = PAL_GL_RELEASE_BEHAVIOR_NONE; // PAL_GL_EXTENSION_FLUSH_CONTROL
    createInfo.reset = PAL_GL_CONTEXT_RESET_NONE; // PAL_GL_EXTENSION_ROBUSTNESS
    createInfo.shareContext = nullptr; // no shared context

    createInfo.window = &glWindow;
    if (glInfo->extensions & PAL_GL_EXTENSION_CREATE_CONTEXT) {
        createInfo.forward = PAL_TRUE;
    }

    if (glInfo->extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE) {
        createInfo.profile = PAL_GL_PROFILE_CORE;
    }

    PalResult result = palCreateGLContext(&createInfo, &context);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create GL context");
        return PAL_FALSE;
    }

    // make the context current and optionally set vsync if supported
    result = palMakeContextCurrent(&glWindow, context);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to make GL context current");
        return PAL_FALSE;
    }

    if (glInfo->extensions & PAL_GL_EXTENSION_SWAP_CONTROL) {
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

    // destroy the GL context and shutdown the GL shutdown
    palDestroyGLContext(context);
    palShutdownGL();
    glHelperDestroyWindow(window, eventDriver);

    return PAL_TRUE;
}
