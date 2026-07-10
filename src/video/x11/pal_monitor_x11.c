
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_X11_BACKEND == 1
#include "pal_x11.h"
#include <math.h>

PalResult xEnumerateMonitors(
    uint32_t* count,
    PalMonitor** outMonitors)
{
    int _count = 0;
    int maxCount = outMonitors ? *count : 0;
    XRRScreenResources* resources = s_X11.getScreenResources(s_X11.display, s_X11.root);
    for (int i = 0; i < resources->noutput; ++i) {
        RROutput output = resources->outputs[i];
        XRROutputInfo* outputInfo = s_X11.getOutputInfo(s_X11.display, resources, output);
        if (outputInfo->connection == RR_Connected && outputInfo->crtc != None) {
            // a monitor
            if (outMonitors) {
                if (_count < maxCount) {
                    outMonitors[_count] = TO_PAL_HANDLE(PalMonitor, output);
                }
            }
            _count++;
        }
    }

    if (!outMonitors) {
        *count = _count;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult xGetPrimaryMonitor(PalMonitor** outMonitor)
{
    RROutput primary = s_X11.getOutputPrimary(s_X11.display, s_X11.root);
    if (primary) {
        *outMonitor = TO_PAL_HANDLE(PalMonitor, primary);
        return PAL_RESULT_SUCCESS;
    }
    return PAL_RESULT_CODE_PLATFORM_FAILURE;
}

PalResult xGetMonitorInfo(
    PalMonitor* monitor,
    PalMonitorInfo* info)
{
    XRRScreenResources* resources = s_X11.getScreenResources(s_X11.display, s_X11.root);
    RROutput output = FROM_PAL_HANDLE(RROutput, monitor);
    XRROutputInfo* outputInfo = s_X11.getOutputInfo(s_X11.display, resources, output);
    if (!outputInfo) {
        // invalid monitor
        s_X11.freeScreenResources(resources);
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    if (outputInfo->connection != RR_Connected) {
        // invalid monitor
        s_X11.freeScreenResources(resources);
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    // check if its primary monitor
    strcpy(info->name, outputInfo->name);
    RROutput primary = s_X11.getOutputPrimary(s_X11.display, s_X11.root);
    if (monitor == TO_PAL_HANDLE(PalMonitor, primary)) {
        info->primary = PAL_TRUE;
    } else {
        info->primary = PAL_FALSE;
    }

    // get monitor pos and size
    XRRCrtcInfo* crtc = s_X11.getCrtcInfo(s_X11.display, resources, outputInfo->crtc);
    info->x = crtc->x;
    info->y = crtc->y;
    info->width = crtc->width;
    info->height = crtc->height;

    // get refresh rate
    double rate = 0;
    for (int i = 0; i < resources->nmode; ++i) {
        // check for our monitor
        if (resources->modes[i].id == crtc->mode) {
            XRRModeInfo* mode = &resources->modes[i];
            double tmp = (double)mode->hTotal * (double)mode->vTotal;
            rate = (double)mode->dotClock / tmp;
            info->refreshRate = rate + 0.5;
            break;
        }
    }

    // orientation
    switch (crtc->rotation) {
        case RR_Rotate_0: {
            info->orientation = PAL_ORIENTATION_LANDSCAPE;
            break;
        }

        case RR_Rotate_90: {
            info->orientation = PAL_ORIENTATION_PORTRAIT;
            break;
        }

        case RR_Rotate_180: {
            info->orientation = PAL_ORIENTATION_LANDSCAPE_FLIPPED;
            break;
        }

        case RR_Rotate_270: {
            info->orientation = PAL_ORIENTATION_PORTRAIT_FLIPPED;
            break;
        }

        default: {
            info->orientation = PAL_ORIENTATION_LANDSCAPE;
        }
    }

    // get dpi
    float raw = crtc->width / 1920.0f;
    float steps[] = {1.0f, 1.2f, 1.5f, 1.75, 2.0f};
    float closest = steps[0];
    float minDiff = fabsf(raw - steps[0]);

    for (int i = 1; i < sizeof(steps) / sizeof(steps[0]); i++) {
        float diff = fabsf(raw - steps[i]);
        if (diff < minDiff) {
            minDiff = diff;
            closest = steps[i];
        }
    }

    info->dpi = (uint32_t)(closest * 96.0f);
    s_X11.freeCrtcInfo(crtc);
    s_X11.freeOutputInfo(outputInfo);
    s_X11.freeScreenResources(resources);

    return PAL_RESULT_SUCCESS;
}

PalResult xEnumerateMonitorModes(
    PalMonitor* monitor,
    uint32_t* count,
    PalMonitorMode* modes)
{
    uint32_t modeCount = 0;
    int maxModeCount = modes ? *count : 0;
    XRRScreenResources* resources = s_X11.getScreenResources(s_X11.display, s_X11.root);

    // get the monitor info
    RROutput output = FROM_PAL_HANDLE(RROutput, monitor);
    XRROutputInfo* outputInfo = s_X11.getOutputInfo(s_X11.display, resources, output);
    if (!outputInfo) {
        // invalid monitor
        s_X11.freeScreenResources(resources);
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    if (outputInfo->connection != RR_Connected) {
        // invalid monitor
        s_X11.freeScreenResources(resources);
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    // get supported display modes
    for (int i = 0; i < outputInfo->nmode; ++i) {
        for (int j = 0; j < resources->nmode; ++j) {
            // get the display mode and check if its for our monitor
            XRRModeInfo* info = &resources->modes[j];
            if (info->id == outputInfo->modes[i]) {
                // check if user supplied a PalMonitorMode array
                if (modes) {
                    if (modeCount < maxModeCount) {
                        PalMonitorMode* mode = &modes[modeCount];
                        mode->width = info->width;
                        mode->height = info->height;
                        mode->bpp = s_X11.bpp;

                        double tmp = (double)info->hTotal * (double)info->vTotal;
                        double rate = (double)info->dotClock / tmp;
                        mode->refreshRate = rate + 0.5;
                    }
                }
                modeCount++;
            }
        }
    }

    if (!modes) {
        *count = modeCount;
    }

    s_X11.freeOutputInfo(outputInfo);
    s_X11.freeScreenResources(resources);
    return PAL_RESULT_SUCCESS;
}

PalResult xGetCurrentMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    XRRScreenResources* resources = s_X11.getScreenResources(s_X11.display, s_X11.root);

    // get the monitor info
    RROutput output = FROM_PAL_HANDLE(RROutput, monitor);
    XRROutputInfo* outputInfo =s_X11.getOutputInfo(s_X11.display, resources, output);

    if (!outputInfo) {
        // invalid monitor
        s_X11.freeScreenResources(resources);
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    if (outputInfo->connection != RR_Connected) {
        // invalid monitor
        s_X11.freeScreenResources(resources);
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    // get the current display mode
    XRRCrtcInfo* crtc = s_X11.getCrtcInfo(s_X11.display, resources, outputInfo->crtc);
    // find the display mode
    XRRModeInfo* info = nullptr;
    for (int i = 0; i < resources->nmode; ++i) {
        if (resources->modes[i].id == crtc->mode) {
            // found
            info = &resources->modes[i];
            break;
        }
    }

    if (mode) {
        mode->width = info->width;
        mode->height = info->height;
        mode->bpp = s_X11.bpp;

        double tmp = (double)info->hTotal * (double)info->vTotal;
        double rate = (double)info->dotClock / tmp;
        mode->refreshRate = rate + 0.5;
    }

    s_X11.freeCrtcInfo(crtc);
    s_X11.freeOutputInfo(outputInfo);
    s_X11.freeScreenResources(resources);

    return PAL_RESULT_SUCCESS;
}

PalResult xSetMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    XRRScreenResources* resources = s_X11.getScreenResources(s_X11.display, s_X11.root);

    // get the monitor info
    RROutput output = FROM_PAL_HANDLE(RROutput, monitor);
    XRROutputInfo* outputInfo = s_X11.getOutputInfo(s_X11.display, resources, output);

    if (!outputInfo) {
        // invalid monitor
        s_X11.freeScreenResources(resources);
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    if (outputInfo->connection != RR_Connected) {
        // invalid monitor
        s_X11.freeScreenResources(resources);
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    // find the monitor display mode
    RRMode displayMode = findMode(resources, mode);
    if (displayMode == None) {
        s_X11.freeOutputInfo(outputInfo);
        s_X11.freeScreenResources(resources);
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    // apply the display mode
    XRRCrtcInfo* crtc = s_X11.getCrtcInfo(s_X11.display, resources, outputInfo->crtc);
    int ret = s_X11.setCrtcConfig(
        s_X11.display,
        resources,
        outputInfo->crtc,
        CurrentTime,
        crtc->x,
        crtc->y,
        displayMode,
        crtc->rotation,
        &output,
        1);

    s_X11.freeCrtcInfo(crtc);
    s_X11.freeOutputInfo(outputInfo);
    s_X11.freeScreenResources(resources);
    if (ret != Success) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult xValidateMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult xSetMonitorOrientation(
    PalMonitor* monitor,
    PalOrientation orientation)
{
    XRRScreenResources* resources = s_X11.getScreenResources(s_X11.display, s_X11.root);

    // get the monitor info
    RROutput output = FROM_PAL_HANDLE(RROutput, monitor);
    XRROutputInfo* outputInfo = s_X11.getOutputInfo(s_X11.display, resources, output);
    if (!outputInfo) {
        // invalid monitor
        s_X11.freeScreenResources(resources);
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    if (outputInfo->connection != RR_Connected) {
        // invalid monitor
        s_X11.freeScreenResources(resources);
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    // get the current display mode
    XRRCrtcInfo* crtc = s_X11.getCrtcInfo(s_X11.display, resources, outputInfo->crtc);

    // check if the new orientation is supported
    Rotation rotation = 0;
    switch (orientation) {
        case PAL_ORIENTATION_LANDSCAPE: {
            rotation = RR_Rotate_0;
            break;
        }

        case PAL_ORIENTATION_PORTRAIT: {
            rotation = RR_Rotate_90;
            break;
        }

        case PAL_ORIENTATION_LANDSCAPE_FLIPPED: {
            rotation = RR_Rotate_180;
            break;
        }

        case PAL_ORIENTATION_PORTRAIT_FLIPPED: {
            rotation = RR_Rotate_270;
            break;
        }
    }

    if (!(crtc->rotations & rotation)) {
        return PAL_RESULT_CODE_INVALID_OPERATION;
    }

    int ret = s_X11.setCrtcConfig(
        s_X11.display,
        resources,
        outputInfo->crtc,
        CurrentTime,
        crtc->x,
        crtc->y,
        crtc->mode,
        rotation,
        &output,
        1);

    s_X11.freeCrtcInfo(crtc);
    s_X11.freeOutputInfo(outputInfo);
    s_X11.freeScreenResources(resources);
    if (ret != Success) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_X11_BACKEND