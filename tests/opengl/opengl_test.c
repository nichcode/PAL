
#include "pal/pal_opengl.h"
#include "pal/pal_video.h"
#include "tests.h"

PalBool openglTest()
{
    // initialize the video system
    PalResult result = palInitVideo(nullptr, nullptr, nullptr);
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

    // get the icd info. max version, graphics driver etc
    const PalGLInfo* info = palGetGLInfo();

    // log the info
    palLog(nullptr, "GL version: (%d.%d)", info->major, info->minor);
    palLog(nullptr, "GL version String: %s", info->version);
    palLog(nullptr, "Graphics Card Vendor: %s", info->vendor);
    palLog(nullptr, "Graphics Card: %s", info->graphicsCard);

    palLog(nullptr, "");
    palLog(nullptr, "Supported Opengl Extensions:");

    if (info->extensions & PAL_GL_EXTENSION_CREATE_CONTEXT) {
        palLog(nullptr, " GL modern context creation");
    }

    if (info->extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE) {
        palLog(nullptr, " GL context profile");
    }

    if (info->extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2) {
        palLog(nullptr, " GL context profile(es2)");
    }

    if (info->extensions & PAL_GL_EXTENSION_ROBUSTNESS) {
        palLog(nullptr, " GL robustness");
    }

    if (info->extensions & PAL_GL_EXTENSION_NO_ERROR) {
        palLog(nullptr, " GL no error");
    }

    if (info->extensions & PAL_GL_EXTENSION_PIXEL_FORMAT) {
        palLog(nullptr, " GL pixel format selection");
    }

    if (info->extensions & PAL_GL_EXTENSION_MULTISAMPLE) {
        palLog(nullptr, " GL multisample");
    }

    if (info->extensions & PAL_GL_EXTENSION_SWAP_CONTROL) {
        palLog(nullptr, " GL swap control");
    }

    if (info->extensions & PAL_GL_EXTENSION_FLUSH_CONTROL) {
        palLog(nullptr, " GL flush control");
    }

    if (info->extensions & PAL_GL_EXTENSION_COLORSPACE_SRGB) {
        palLog(nullptr, " GL colorspace");
    }

    // shutdown the opengl system
    palShutdownGL();

    // shutdown the video system
    palShutdownVideo();

    return PAL_TRUE;
}
