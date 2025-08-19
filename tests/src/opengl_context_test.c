
#include "tests.h"
#include "pal/pal_opengl.h"
#include "pal/pal_video.h"

void openglContextTest() {

    palLog("");
    palLog("===========================================");
    palLog("Opengl Context Test");
    palLog("===========================================");
    palLog("");

    PalResult result;
    result = palLoadGLICD();
    if (result != PAL_RESULT_SUCCESS) {
        // this can made into a goto to decrease this result checks
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // we need a window
    // you can create the window using SDL, GLFW, PAL or th OS
    result = palInitVideo(nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        // this can made into a goto to decrease this result checks
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    PalWindow* window = nullptr;
    PalWindowCreateInfo windowCreateInfo;
    windowCreateInfo.display = nullptr; // use primary display
    windowCreateInfo.width = 640;
    windowCreateInfo.height = 480;
    windowCreateInfo.title = "Pal GL Context Test";
    windowCreateInfo.flags = PAL_WINDOW_CENTER | PAL_WINDOW_DEFAULT;

    result = palCreateWindow(&windowCreateInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // get the native window handle
    void* nativeWindow = palGetWindowHandle(window);

    Int32 pixelCount = 0;
    result = palEnumerateGLPixelFormats(nativeWindow, &pixelCount, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    palLog("Pixel Format count - %d", pixelCount);

    PalGLPixelFormat* formats = palAllocate(
        nullptr,
        sizeof(PalGLPixelFormat) * pixelCount,
        0
    );

    if (!formats) {
        return;
    }

    result = palEnumerateGLPixelFormats(nativeWindow, &pixelCount, formats);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    const char* boolsToSting[2] = {
        "false",
        "true"
    };

    palLog("");
    for (int i = 0; i < pixelCount; i++) {
        // log pixel formate
        PalGLPixelFormat* format = &formats[i];
        palLog("Pixel Format Index: %i", format->index);
        palLog(" Red Bits: %d", format->redBits);
        palLog(" Green Bits: %d", format->greenBits);
        palLog(" Blue Bits: %d", format->blueBits);
        palLog(" Alpha Bits: %d", format->alphaBits);
        palLog(" Depth Bits: %d", format->depthBits);
        palLog(" Stencil Bits: %d", format->stencilBits);

        palLog(" Samples: %d", format->samples);
        palLog(" DoubleBuffer: %s", boolsToSting[format->doubleBuffer]);
        palLog(" Stereo: %s", boolsToSting[format->stereo]);
        palLog(" sRGB: %s", boolsToSting[format->sRGB]);
        palLog("");
    }

    PalGLPixelFormat desired = {};
    desired.redBits = 8;
    desired.greenBits = 8;
    desired.blueBits = 8;
    desired.alphaBits = 8;

    desired.alphaBits = 8;
    desired.depthBits = 24;
    desired.stencilBits = 8;

    desired.stereo = false;
    desired.sRGB = false;
    desired.doubleBuffer = true;

    // get the closest pixel format
    PalGLPixelFormat* closest = palGetClosestGLPixelFormat(
        formats,
        pixelCount,
        &desired
    );

    // log the closest and desired pixel format
    palLog("");
    palLog("Desired Pixel Format");
    palLog(" Pixel Format Index: %i", desired.index);
    palLog(" Red Bits: %d", desired.redBits);
    palLog(" Green Bits: %d", desired.greenBits);
    palLog(" Blue Bits: %d", desired.blueBits);
    palLog(" Alpha Bits: %d", desired.alphaBits);
    palLog(" Depth Bits: %d", desired.depthBits);
    palLog(" Stencil Bits: %d", desired.stencilBits);

    palLog(" Samples: %d", desired.samples);
    palLog(" DoubleBuffer: %s", boolsToSting[desired.doubleBuffer]);
    palLog(" Stereo: %s", boolsToSting[desired.stereo]);
    palLog(" sRGB: %s", boolsToSting[desired.sRGB]);
    palLog("");

    if (!closest) {
        palLog("Failed to get closest pixel format");
        return;
    }

    palLog("");
    palLog("Closest Pixel Format");
    palLog(" Pixel Format Index: %i", closest->index);
    palLog(" Red Bits: %d", closest->redBits);
    palLog(" Green Bits: %d", closest->greenBits);
    palLog(" Blue Bits: %d", closest->blueBits);
    palLog(" Alpha Bits: %d", closest->alphaBits);
    palLog(" Depth Bits: %d", closest->depthBits);
    palLog(" Stencil Bits: %d", closest->stencilBits);

    palLog(" Samples: %d", closest->samples);
    palLog(" DoubleBuffer: %s", boolsToSting[closest->doubleBuffer]);
    palLog(" Stereo: %s", boolsToSting[closest->stereo]);
    palLog(" sRGB: %s", boolsToSting[closest->sRGB]);
    palLog("");

    // create opengl context
    PalGLContext* context = nullptr;
    PalGLInfo info = palGetGLInfo();

    PalGLContextCreateInfo contextCreateInfo;
    contextCreateInfo.allocator = nullptr;
    contextCreateInfo.format = closest;
    contextCreateInfo.major = info.versionMajor;
    contextCreateInfo.minor = info.versionMinor;
    contextCreateInfo.profile = PAL_GL_PROFILE_CORE;
    contextCreateInfo.windowHandle = nativeWindow;

    palCreateGLContext(&contextCreateInfo, &context);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // make the context current
    palMakeCurrent(context);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    bool running = true;
    while (running) {
        palUpdateVideo();

        palSwapBuffers(context);
        if (result != PAL_RESULT_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLog("PAL error - %s", resultString);
            return;
        }
    }

    palDestroyGLContext(context);
    palFree(nullptr, formats);
    palDestroyWindow(window);
    palShutdownVideo();
}