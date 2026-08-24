
#include "opengl_helper.h"

PFNGLCLEARCOLORPROC s_GlClearColor = nullptr;
PFNGLCLEARPROC s_GlClear = nullptr;

static void updateGLContext(
    PalBool* running, 
    PalEventDriver* eventDriver,
    PalGLWindow* glWindow, 
    PalGLContext* context)
{
    while (*running) {
        // update the video system to push video events
        palUpdateVideo();

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_TYPE_WINDOW_CLOSE: {
                    *running = PAL_FALSE;
                    break;
                }

                case PAL_EVENT_TYPE_KEYDOWN: {
                    PalKeycode keycode = 0;
                    palUnpackUint32(event.data, &keycode, nullptr);
                    if (keycode == PAL_KEYCODE_ESCAPE) {
                        *running = PAL_FALSE;
                    }
                    break;
                }
            }
        }

        // clear the buffer
        s_GlClear(0x00004000); // GL_COLOR_BUFFER_BIT

        // swap buffers
        PalResult result = palSwapBuffers(glWindow, context);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to swap buffers");
            return;
        }
    }
}

PalBool openglMultiContextTest()
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

    window = glHelperCreateWindow("GL Multi Context Window", eventDriver, &fbConfig);
    if (!window) {
        return PAL_FALSE;
    }

    glWindow = glHelperGetGLWindow(window);
    if (!glWindow.window) {
        return PAL_FALSE;
    }

    context = glHelperCreateContext(&glWindow, &fbConfig);
    if (!context) {
        return PAL_FALSE;
    }

    // load function procs
    s_GlClearColor = (PFNGLCLEARCOLORPROC)palGetGLProcAddress("glClearColor");
    s_GlClear = (PFNGLCLEARPROC)palGetGLProcAddress("glClear");
    s_GlClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    PalBool running = PAL_TRUE;
    updateGLContext(&running, eventDriver, &glWindow, context);

    // destroy the opengl context.
    palDestroyGLContext(context);
    context = nullptr;

    // create a new opengl context with the same window the FBConfig of the new context must match
    context = glHelperCreateContext(&glWindow, &fbConfig);
    if (!context) {
        return PAL_FALSE;
    }

    running = PAL_TRUE;
    s_GlClearColor(.2f, .6f, .6f, 1.0f);
    updateGLContext(&running, eventDriver, &glWindow, context);

    glHelperDestroyContext(context);
    glHelperDestroyWindow(window, eventDriver);
    
    return PAL_TRUE;
}
