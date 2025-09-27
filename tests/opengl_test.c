
#include "pal/pal_opengl.h"
#include "tests.h"

bool openglTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Opengl Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    // initialize the opengl system. This loads the icd.
    PalResult result = palInitGL(nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize opengl: %s", error);
        return false;
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

    return true;
}