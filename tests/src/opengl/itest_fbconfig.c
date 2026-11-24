#include "pal2/opengl/init.h"
#include "pal2/opengl/fbconfig.h"
#include "pal2/core/log.h"
#include "pal2/video/init.h"

static const char* s_BoolsToSting[2] = {"False", "True",};

static void logConfig(const PalGLFBConfig* config)
{
    palLog(nullptr, "FB Config Index: %d", config->index);
    palLog(nullptr, " Red Bits: %d", config->redBits);
    palLog(nullptr, " Green Bits: %d", config->greenBits);
    palLog(nullptr, " Blue Bits: %d", config->blueBits);
    palLog(nullptr, " Alpha Bits: %d", config->alphaBits);
    palLog(nullptr, " Depth Bits: %d", config->depthBits);
    palLog(nullptr, " Stencil Bits: %d", config->stencilBits);

    palLog(nullptr, " Samples: %d", config->samples);

    const char* boolStr = s_BoolsToSting[config->doubleBuffer];
    palLog(nullptr, " DoubleBuffer: %s", boolStr);
    palLog(nullptr, " Stereo: %s", s_BoolsToSting[config->stereo]);
    palLog(nullptr, " sRGB: %s", s_BoolsToSting[config->sRGB]);
    palLog(nullptr, "");
}

int main(void)
{
    palLog(nullptr, "Running fbconfig itest...");

    PalResult result = palInitVideo(nullptr, nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to initialize video");
        return -1;
    }

    PalGLAPI openglAPI;
    const void* videoInstance = palGetInstance();
    const PalBool* supportedAPIs = palGetSupportedGLAPIs(videoInstance);
    if (supportedAPIs) {
        if (supportedAPIs[PAL_GL_API_OPENGL]) {
            openglAPI = PAL_GL_API_OPENGL;
        } else {
            openglAPI = PAL_GL_API_OPENGL_ES;
        }

    } else {
        palLog(nullptr, "Failed to get supported opengl apis");
        return -1;
    }

    result = palInitGL(openglAPI, videoInstance, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to initialize opengl");
        return -1;
    }

    uint32_t fbCount = 0;
    result = palEnumerateGLFBConfigs(&fbCount, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to query GL FBConfigs");
        return -1;
    }

    palLog(nullptr, "GL FBConfig count: %u", fbCount);
    if (fbCount == 0) {
        palLog(nullptr, "No supported FBConfig found");
        return -1;
    }

    PalGLFBConfig* fbConfigs = nullptr;
    fbConfigs = palAllocate(nullptr, sizeof(PalGLFBConfig) * fbCount, 0);
    if (!fbConfigs) {
        palLog(nullptr, "Failed to allocate memory");
        return -1;
    }

    result = palEnumerateGLFBConfigs(&fbCount, fbConfigs);
    if (result != PAL_RESULT_SUCCESS) {
        palFree(nullptr, fbConfigs);
        palLog(nullptr, "Failed to query GL FBConfigs");
        return -1;
    }

    palLog(nullptr, "");
    for (uint32_t i = 0; i < fbCount; i++) {
        PalGLFBConfig* config = &fbConfigs[i];
        logConfig(config);
    }

    PalGLFBConfig desired = {0};
    desired.redBits = 8;
    desired.greenBits = 8;
    desired.blueBits = 8;
    desired.alphaBits = 8;
    desired.alphaBits = 8;
    desired.depthBits = 24;
    desired.stencilBits = 8;
    desired.samples = 2;

    desired.stereo = PAL_FALSE;
    desired.sRGB = PAL_TRUE;
    desired.doubleBuffer = PAL_TRUE;

    const PalGLFBConfig* closest = nullptr;
    closest = palGetClosestGLFBConfig(fbConfigs, fbCount, &desired);

    palLog(nullptr, "Desired GL FBConfig:");
    logConfig(&desired);

    palLog(nullptr, "Closest GL FBConfig:");
    logConfig(closest);

    palShutdownGL();
    palShutdownVideo();

    palFree(nullptr, fbConfigs);

    return 0;
}