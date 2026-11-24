#include "pal2/opengl/init.h"
#include "pal2/core/log.h"
#include "pal2/video/init.h"

int main(void)
{
    palLog(nullptr, "Running openglinfo itest...");

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

    const PalGLInfo* info = palGetGLInfo();

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

    palShutdownGL();
    palShutdownVideo();

    return 0;
}