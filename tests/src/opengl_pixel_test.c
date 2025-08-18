
#include "tests.h"
#include "pal/pal_opengl.h"
#include "pal/pal_video.h"

void openglPixelTest() {

    palLog("");
    palLog("===========================================");
    palLog("Opengl Pixel Test");
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
    windowCreateInfo.title = "Pal Test Window";
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

    palFree(nullptr, formats);
    palDestroyWindow(window);
    palShutdownVideo();
}