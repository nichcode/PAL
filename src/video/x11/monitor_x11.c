/**
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#if PAL_HAS_X11_BACKEND

#include "pal2/video/monitor.h"
#include "x11_helper.h"
#include <string.h>
#include <math.h>

/* =========================================================================
 * Helper
 * ========================================================================= */

typedef struct MonitorContext {
    XRRScreenResources* resources;
    XRROutputInfo* outputInfo;
    XRRCrtcInfo* crtc;
    RROutput output;
} MonitorContext;

static PalBool acquireMonitorContext(
    PalMonitor* monitor,
    PalBool needCrtc,
    MonitorContext* outCtx)
{
    memset(outCtx, 0, sizeof(*outCtx));
    if (!monitor) {
        return PAL_FALSE;
    }

    outCtx->output = FROM_PAL_HANDLE(RROutput, monitor);
    outCtx->resources = g_X11.getScreenResources(g_X11.display, g_X11.root);
    if (!outCtx->resources) {
        return PAL_FALSE;
    }

    outCtx->outputInfo = g_X11.getOutputInfo(g_X11.display, outCtx->resources, outCtx->output);
    if (!outCtx->outputInfo || outCtx->outputInfo->connection != RR_Connected) {
        releaseMonitorContext(outCtx);
        return PAL_FALSE;
    }

    if (needCrtc) {
        if (outCtx->outputInfo->crtc == None) {
            releaseMonitorContext(outCtx);
            return PAL_FALSE;
        }
        outCtx->crtc = g_X11.getCrtcInfo(g_X11.display, outCtx->resources, outCtx->outputInfo->crtc);
        if (!outCtx->crtc) {
            releaseMonitorContext(outCtx);
            return PAL_FALSE;
        }
    }

    return PAL_TRUE;
}

static void releaseMonitorContext(MonitorContext* ctx)
{
    if (ctx->crtc) {
        g_X11.freeCrtcInfo(ctx->crtc);
    }

    if (ctx->outputInfo) {
        g_X11.freeOutputInfo(ctx->outputInfo);
    }

    if (ctx->resources) {
        g_X11.freeScreenResources(ctx->resources);
    }
}

static const XRRModeInfo* findModeById(
    const XRRScreenResources* resources,
    RRMode id)
{
    for (int i = 0; i < resources->nmode; ++i) {
        if (resources->modes[i].id == id) {
            return &resources->modes[i];
        }
    }

    return NULL;
}

static uint32_t calcRefreshRate(const XRRModeInfo* mode)
{
    double total = (double)mode->hTotal * (double)mode->vTotal;
    if (total <= 0.0) {
        return 0;
    }
    return (uint32_t)(((double)mode->dotClock / total) + 0.5);
}

static void fillMonitorMode(
    const XRRModeInfo* src,
    PalMonitorMode* dst)
{
    dst->width = src->width;
    dst->height = src->height;
    dst->bpp = g_X11.bpp;
    dst->refreshRate = calcRefreshRate(src);
}

static uint32_t estimateDpi(int width)
{
    static const float steps[] = {1.0f, 1.2f, 1.5f, 1.75f, 2.0f};
    float raw = width / 1920.0f;
    float closest = steps[0];
    float minDiff = fabsf(raw - steps[0]);

    for (size_t i = 1; i < sizeof(steps) / sizeof(steps[0]); ++i) {
        float diff = fabsf(raw - steps[i]);
        if (diff < minDiff) {
            minDiff = diff;
            closest = steps[i];
        }
    }
    return (uint32_t)(closest * 96.0f);
}


/* =========================================================================
 * API
 * ========================================================================= */

PalResult x11EnumerateMonitors(
    uint32_t* count,
    PalMonitor** monitors)
{
    uint32_t found = 0;
    uint32_t maxCount = monitors ? *count : 0;

    XRRScreenResources* resources = g_X11.getScreenResources(g_X11.display, g_X11.root);
    if (!resources) {
        if (!monitors) {
            *count = 0;
        }
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    for (int i = 0; i < resources->noutput; ++i) {
        RROutput output = resources->outputs[i];
        XRROutputInfo* outputInfo = g_X11.getOutputInfo(g_X11.display, resources, output);
        if (!outputInfo) {
            continue;
        }

        if (outputInfo->connection == RR_Connected && outputInfo->crtc != None) {
            if (monitors && found < maxCount) {
                monitors[found] = TO_PAL_HANDLE(PalMonitor, output);
            }
            found++;
        }
        g_X11.freeOutputInfo(outputInfo);
    }

    g_X11.freeScreenResources(resources);

    if (!monitors) {
        *count = found;
    }

    return PAL_RESULT_SUCCESS;
}

void x11GetPrimaryMonitor(PalMonitor** monitor)
{
    RROutput primary = g_X11.getOutputPrimary(g_X11.display, g_X11.root);
    if (primary && monitor) {
        *monitor = TO_PAL_HANDLE(PalMonitor, primary);
    }
}

void x11GetMonitorInfo(
    PalMonitor* monitor,
    PalMonitorInfo* info)
{
    MonitorContext ctx;
    if (!info || !acquireMonitorContext(monitor, PAL_TRUE, &ctx)) {
        return;
    }

    strncpy(info->name, ctx.outputInfo->name, sizeof(info->name) - 1);
    info->name[sizeof(info->name) - 1] = '\0';

    RROutput primary = g_X11.getOutputPrimary(g_X11.display, g_X11.root);
    info->primary = (ctx.output == primary) ? PAL_TRUE : PAL_FALSE;

    info->x = ctx.crtc->x;
    info->y = ctx.crtc->y;
    info->width = ctx.crtc->width;
    info->height = ctx.crtc->height;

    const XRRModeInfo* mode = findModeById(ctx.resources, ctx.crtc->mode);
    info->refreshRate = mode ? calcRefreshRate(mode) : 0;

    switch (ctx.crtc->rotation) {
    case RR_Rotate_90:
        info->orientation = PAL_ORIENTATION_PORTRAIT;
        break;
    case RR_Rotate_180:
        info->orientation = PAL_ORIENTATION_LANDSCAPE_FLIPPED;
        break;
    case RR_Rotate_270:
        info->orientation = PAL_ORIENTATION_PORTRAIT_FLIPPED;
        break;
    case RR_Rotate_0:
    default:
        info->orientation = PAL_ORIENTATION_LANDSCAPE;
        break;
    }

    info->dpi = estimateDpi(ctx.crtc->width);
    releaseMonitorContext(&ctx);
}

void x11EnumerateMonitorModes(
    PalMonitor* monitor,
    uint32_t* count,
    PalMonitorMode* modes)
{
    MonitorContext ctx;
    if (!acquireMonitorContext(monitor, PAL_FALSE, &ctx)) {
        return;
    }

    uint32_t modeCount = 0;
    uint32_t maxModeCount = modes ? *count : 0;

    for (int i = 0; i < ctx.outputInfo->nmode; ++i) {
        const XRRModeInfo* info = findModeById(ctx.resources, ctx.outputInfo->modes[i]);
        if (info) {
            if (modes && modeCount < maxModeCount) {
                fillMonitorMode(info, &modes[modeCount]);
            }
            modeCount++;
        }
    }

    if (!modes) {
        *count = modeCount;
    }

    releaseMonitorContext(&ctx);
}

void x11GetCurrentMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    MonitorContext ctx;
    if (!mode || !acquireMonitorContext(monitor, PAL_TRUE, &ctx)) {
        return;
    }

    const XRRModeInfo* info = findModeById(ctx.resources, ctx.crtc->mode);
    if (info) {
        fillMonitorMode(info, mode);
    }

    releaseMonitorContext(&ctx);
}

PalResult x11SetMonitorMode(
    PalMonitor* monitor,
    const PalMonitorMode* mode)
{
    MonitorContext ctx;
    if (!acquireMonitorContext(monitor, PAL_TRUE, &ctx)) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    RRMode displayMode = x11FindMode(ctx.resources, mode);
    if (displayMode == None) {
        releaseMonitorContext(&ctx);
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    int ret = g_X11.setCrtcConfig(
        g_X11.display,
        ctx.resources,
        ctx.outputInfo->crtc,
        CurrentTime,
        ctx.crtc->x,
        ctx.crtc->y,
        displayMode,
        ctx.crtc->rotation,
        &ctx.output,
        1);

    releaseMonitorContext(&ctx);
    return (ret == Success) ? PAL_RESULT_SUCCESS : PAL_RESULT_CODE_PLATFORM_FAILURE;
}

PalResult x11ValidateMonitorMode(
    PalMonitor* monitor,
    const PalMonitorMode* mode)
{
    (void)monitor;
    (void)mode;
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult x11SetMonitorOrientation(
    PalMonitor* monitor,
    PalOrientation orientation)
{
    Rotation rotation = 0;
    switch (orientation) {
    case PAL_ORIENTATION_LANDSCAPE:
        rotation = RR_Rotate_0;
        break;
    case PAL_ORIENTATION_PORTRAIT:
        rotation = RR_Rotate_90;
        break;
    case PAL_ORIENTATION_LANDSCAPE_FLIPPED:
        rotation = RR_Rotate_180;
        break;
    case PAL_ORIENTATION_PORTRAIT_FLIPPED:
        rotation = RR_Rotate_270;
        break;
    default:
        return PAL_RESULT_CODE_INVALID_OPERATION;
    }

    MonitorContext ctx;
    if (!acquireMonitorContext(monitor, PAL_TRUE, &ctx)) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    if (!(ctx.crtc->rotations & rotation)) {
        releaseMonitorContext(&ctx);
        return PAL_RESULT_CODE_INVALID_OPERATION;
    }

    int ret = g_X11.setCrtcConfig(
        g_X11.display,
        ctx.resources,
        ctx.outputInfo->crtc,
        CurrentTime,
        ctx.crtc->x,
        ctx.crtc->y,
        ctx.crtc->mode,
        rotation,
        &ctx.output,
        1);

    releaseMonitorContext(&ctx);
    return (ret == Success) ? PAL_RESULT_SUCCESS : PAL_RESULT_CODE_PLATFORM_FAILURE;
}

#endif // PAL_HAS_X11_BACKEND