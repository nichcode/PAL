
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
        palLog(
            nullptr,
            "Failed to initialize opengl %s",
            palFormatResult(result));
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
    palLog(nullptr, "Supported GL Extensions:");

    if (info->extensions & PAL_GL_EXTENSION_CREATE_CONTEXT) {
        palLog(nullptr, " GL modern context creation extension supported");
    }

    if (info->extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE) {
        palLog(nullptr, " GL context profile extension supported");
    }

    if (info->extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2) {
        palLog(nullptr, " GL context profile(es2) extension supported");
    }

    if (info->extensions & PAL_GL_EXTENSION_ROBUSTNESS) {
        palLog(nullptr, " GL robustness extension supported");
    }

    if (info->extensions & PAL_GL_EXTENSION_NO_ERROR) {
        palLog(nullptr, " GL no error extension supported");
    }

    if (info->extensions & PAL_GL_EXTENSION_PIXEL_FORMAT) {
        palLog(nullptr, " GL pixel format selection extension supported");
    }

    if (info->extensions & PAL_GL_EXTENSION_MULTISAMPLE) {
        palLog(nullptr, " GL multisample extension supported");
    }

    if (info->extensions & PAL_GL_EXTENSION_SWAP_CONTROL) {
        palLog(nullptr, " GL swap control extension supported");
    }

    if (info->extensions & PAL_GL_EXTENSION_FLUSH_CONTROL) {
        palLog(nullptr, " GL flush control extension supported");
    }

    if (info->extensions & PAL_GL_EXTENSION_COLORSPACE_SRGB) {
        palLog(nullptr, " GL colorspace extension supported");
    }

    // shutdown the opengl system
    palShutdownGL();

    return true;
}