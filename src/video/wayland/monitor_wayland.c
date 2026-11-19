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

#if PAL_HAS_WAYLAND_BACKEND

#include "wayland_helper.h"
#include "pal2/video/monitor.h"
#include <string.h>

PalResult wlEnumerateMonitors(
    uint32_t* count,
    PalMonitor** monitors)
{
    if (!monitors) {
        *count = g_Wl.monitorCount;
        return PAL_RESULT_SUCCESS;
    }

    uint32_t index = 0;
    uint32_t maxCount = g_Wl.maxMonitorData;
    for (uint32_t i = 0; i < maxCount && index < *count; i++) {
        if (g_Wl.monitorData[i].used) {
            monitors[index++] = g_Wl.monitorData[i].monitor;
        }
    }

    *count = index;
    return PAL_RESULT_SUCCESS;
}

void wlGetPrimaryMonitor(PalMonitor** monitor)
{
    *monitor = nullptr;
    for (uint32_t i = 0; i < g_Wl.maxMonitorData; i++) {
        if (g_Wl.monitorData[i].used) {
            *monitor = g_Wl.monitorData[i].monitor;
            return;
        }
    }
}

void wlGetMonitorInfo(
    PalMonitor* monitor,
    PalMonitorInfo* info)
{
    MonitorData* monitorData = wlFindMonitorData(monitor);
    if (!monitorData) {
        return;
    }

    info->dpi = monitorData->dpi;
    info->x = monitorData->x;
    info->y = monitorData->y;
    info->width = monitorData->w;
    info->height = monitorData->h;
    info->refreshRate = monitorData->refreshRate;
    info->orientation = monitorData->orientation;

    info->primary = PAL_FALSE; // no way to query
    strcpy(info->name, monitorData->name);
}

void wlEnumerateMonitorModes(
    PalMonitor* monitor,
    uint32_t* count,
    PalMonitorMode* modes)
{
    MonitorData* monitorData = wlFindMonitorData(monitor);
    if (!monitorData) {
        *count = 0;
        return;
    }

    if (modes && *count > 0) {
        PalMonitorMode* mode = &modes[0];
        mode->bpp = monitorData->mode.bpp;
        mode->width = monitorData->mode.width;
        mode->height = monitorData->mode.height;
        mode->refreshRate = monitorData->mode.refreshRate;
        *count = 1;
    }

    if (!modes) {
        *count = 1; // wayland only gives the active mode
    }
}

void wlGetCurrentMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    MonitorData* monitorData = wlFindMonitorData(monitor);
    if (!monitorData) {
        return;
    }

    mode->bpp = monitorData->mode.bpp;
    mode->width = monitorData->mode.width;
    mode->height = monitorData->mode.height;
    mode->refreshRate = monitorData->mode.refreshRate;
}

PalResult wlSetMonitorMode(
    PalMonitor* monitor,
    const PalMonitorMode* mode)
{
    (void)monitor;
    (void)mode;
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult wlValidateMonitorMode(
    PalMonitor* monitor,
    const PalMonitorMode* mode)
{
    (void)monitor;
    (void)mode;
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult wlSetMonitorOrientation(
    PalMonitor* monitor,
    PalOrientation orientation)
{
    (void)monitor;
    (void)orientation;
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

#endif // PAL_HAS_WAYLAND_BACKEND