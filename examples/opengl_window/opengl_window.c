
#include "opengl_helper.h"

int main(int argc, char** argv) {

    const char* versionString = palGetVersionString();
    palLog("PAL version - %s", versionString);

    PalWindow* window = nullptr;
    PalEventDriver* eventDriver = nullptr;
    PalGLContext* context = nullptr;
    if (!initOpengl(&eventDriver, &window)) {
        return -1;
    }

    Int32 count = 0;
    PalResult result;
    void* windowHandle = palGetWindowHandle(window);

    result = palEnumerateGLPixelFormats(windowHandle, &count, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return -1;
    }

    palLog("GL Pixel Format count - %d", count);

    PalGLPixelFormat* formats = palAllocate(
        nullptr,
        sizeof(PalGLPixelFormat) * count,
        0
    );

    PalGLPixelFormat desiredFormat = {};
    desiredFormat.redBits = 8;
    desiredFormat.greenBits = 8;
    desiredFormat.blueBits = 8;
    desiredFormat.alphaBits = 8;
    desiredFormat.alphaBits = 8;
    desiredFormat.depthBits = 24;
    desiredFormat.stencilBits = 8;
    desiredFormat.samples = 4;

    desiredFormat.stereo = false;
    desiredFormat.sRGB = true;
    desiredFormat.doubleBuffer = true;

    // get the closest pixel format
    PalGLPixelFormat* closest = palGetClosestGLPixelFormat(
        formats,
        count,
        &desiredFormat
    );

    // pass nullptr as the format to use default
    // always get the format because the default pixel format is very simple and minimal
    if (!createContext(window, closest, &context)) {
        return -1;
    }

    Uint32 vao, vbo, ibo, program;
    createTriangle(&vao, &vbo, &ibo, &program);

    if (!vao) {
        palLog("Failed to create triangle");
        return -1;
    }

    // set the clear color
    glClearColor(.2f, .2f, .2f, 1.0f);

    bool running = true;
    while (running) {
        palUpdateVideo();

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_WINDOW_CLOSE: {
                    running = false;
                    break;
                }

                case PAL_EVENT_WINDOW_RESIZE: {
                    Uint32 width, height;
                    palUnpackUint32(event.data, &width, &height);
                    glViewport(0, 0, width, height);
                    break;
                }
            }
        }

        // draw the triangle
        glClear(GL_COLOR_BUFFER_BIT);
        drawTriangle(vao, program);

        result = palSwapBuffers(context);
        if (result != PAL_RESULT_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLog("PAL error - %s", resultString);
            return -1;
        }
    }

    destroyTriangle(&vao, &vbo, &ibo, &program);
    shutdownOpengl(eventDriver, window, context);
    palFree(nullptr, formats);
    return 0;
}