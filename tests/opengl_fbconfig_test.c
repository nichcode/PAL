
#include "pal/pal_opengl.h"
#include "pal/pal_video.h" // for window
#include "tests.h"

static const char* g_BoolsToSting[2] = {"False", "True"};

bool openglFBConfigTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Opengl FBConfig Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    // initialize the video system and create a window
    PalResult result = palInitVideo(nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize video: %s", error);
        return false;
    }

    // get the instance or display handle and pass it to the opengl system
    // This must be called before the opengl system is initialized
    palGLSetInstance(palGetInstance());

    // initialize the opengl system
    result = palInitGL(nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize opengl: %s", error);
        return false;
    }

    // enumerate supported opengl framebuffer configs
    Int32 fbCount = 0;
    result = palEnumerateGLFBConfigs(nullptr, &fbCount, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to query GL FBConfigs: %s", error);
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

    // enumerate supported opengl framebuffer configs
    result = palEnumerateGLFBConfigs(nullptr, &fbCount, fbConfigs);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to query GL FBConfigs: %s", error);
        palFree(nullptr, fbConfigs);
        return false;
    }

    // log configs
    palLog(nullptr, "");
    for (Int32 i = 0; i < fbCount; i++) {
        // log pixel formate
        PalGLFBConfig* config = &fbConfigs[i];
        palLog(nullptr, "FB Config Index: %d", config->index);
        palLog(nullptr, " Red Bits: %d", config->redBits);
        palLog(nullptr, " Green Bits: %d", config->greenBits);
        palLog(nullptr, " Blue Bits: %d", config->blueBits);
        palLog(nullptr, " Alpha Bits: %d", config->alphaBits);
        palLog(nullptr, " Depth Bits: %d", config->depthBits);
        palLog(nullptr, " Stencil Bits: %d", config->stencilBits);

        palLog(nullptr, " Samples: %d", config->samples);

        const char* boolStr = g_BoolsToSting[config->doubleBuffer];
        palLog(nullptr, " DoubleBuffer: %s", boolStr);
        palLog(nullptr, " Stereo: %s", g_BoolsToSting[config->stereo]);
        palLog(nullptr, " sRGB: %s", g_BoolsToSting[config->sRGB]);
        palLog(nullptr, "");
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

    // log both
    palLog(nullptr, "Desired GL FBConfig:");
    palLog(nullptr, " Red Bits: %d", desired.redBits);
    palLog(nullptr, " Green Bits: %d", desired.greenBits);
    palLog(nullptr, " Blue Bits: %d", desired.blueBits);
    palLog(nullptr, " Alpha Bits: %d", desired.alphaBits);
    palLog(nullptr, " Depth Bits: %d", desired.depthBits);
    palLog(nullptr, " Stencil Bits: %d", desired.stencilBits);

    palLog(nullptr, " Samples: %d", desired.samples);
    palLog(nullptr, " DoubleBuffer: %s", g_BoolsToSting[desired.doubleBuffer]);
    palLog(nullptr, " Stereo: %s", g_BoolsToSting[desired.stereo]);
    palLog(nullptr, " sRGB: %s", g_BoolsToSting[desired.sRGB]);
    palLog(nullptr, "");

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

    // shutdown the opengl system
    palShutdownGL();

    palShutdownVideo();

    // free the framebuffer configs
    palFree(nullptr, fbConfigs);

    return true;
}