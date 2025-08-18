
#include "tests.h"
#include "pal/pal_opengl.h"

void openglTest() {

    palLog("");
    palLog("===========================================");
    palLog("Opengl Test");
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

    // get info and log
    PalGLInfo info = palGetGLInfo();
    palLog("GL version - (%d, %d)", info.versionMajor, info.versionMinor);
    palLog("GL version String - %s", info.version);
    palLog("Graphics Card Vendor - %s", info.vendor);
    palLog("Graphics Card - %s", info.graphicsCard);

    palLog("");
    palLog("Supported GL Extensions:");

    if (info.extensions & PAL_GL_EXTENSION_CREATE_CONTEXT) {
        palLog(" GL modern context creation extension supported");
    }

    if (info.extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE) {
        palLog(" GL context profile selection extension supported");
    }

    if (info.extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2) {
        palLog(" GL context profile selection (es2) extension supported");
    }

    if (info.extensions & PAL_GL_EXTENSION_ROBUSTNESS) {
        palLog(" GL robustness extension supported");
    }

    if (info.extensions & PAL_GL_EXTENSION_NO_ERROR) {
        palLog(" GL no error extension supported");
    }

    if (info.extensions & PAL_GL_EXTENSION_PIXEL_FORMAT) {
        palLog(" GL pixel format selection extension supported");
    }

    if (info.extensions & PAL_GL_EXTENSION_MULTISAMPLE) {
        palLog(" GL multisample extension supported");
    }

    if (info.extensions & PAL_GL_EXTENSION_SWAP_CONTROL) {
        palLog(" GL swap control extension supported");
    }

    if (info.extensions & PAL_GL_EXTENSION_COLORSPACE_SRGB) {
        palLog(" GL colospace extension supported");
    }
}