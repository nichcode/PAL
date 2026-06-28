
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_WAYLAND_BACKEND == 1
#include "pal_wayland.h"

PalResult wlEnumerateMonitors(
    int32_t* count,
    PalMonitor** outMonitors)
{
    if (outMonitors) {
        int index = 0;
        int maxCount = s_Wl.maxMonitorData;
        for (int i = 0; i < maxCount && index < *count; i++) {
            if (s_Wl.monitorData[i].used) {
                // found a monitor
                PalMonitor* monitor = s_Wl.monitorData[index].monitor;
                outMonitors[index++] = monitor;
            }
        }
    }

    if (!outMonitors) {
        *count = s_Wl.monitorCount;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult wlGetPrimaryMonitor(PalMonitor** outMonitor)
{
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult wlGetMonitorInfo(
    PalMonitor* monitor,
    PalMonitorInfo* info)
{
    MonitorData* monitorData = wlFindMonitorData(monitor);
    if (!monitorData) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
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

    return PAL_RESULT_SUCCESS;
}

PalResult wlEnumerateMonitorModes(
    PalMonitor* monitor,
    int32_t* count,
    PalMonitorMode* modes)
{
    MonitorData* monitorData = wlFindMonitorData(monitor);
    if (!monitorData) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    if (modes && *count > 0) {
        PalMonitorMode* mode = &modes[0];
        mode->bpp = monitorData->mode.bpp;
        mode->width = monitorData->mode.width;
        mode->height = monitorData->mode.height;
        mode->refreshRate = monitorData->mode.refreshRate;
    }

    if (!modes) {
        *count = 1; // wayland only gives the active mode
    }

    return PAL_RESULT_SUCCESS;
}

PalResult wlGetCurrentMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    MonitorData* monitorData = wlFindMonitorData(monitor);
    if (!monitorData) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    // this is the same as the current mode
    mode->bpp = monitorData->mode.bpp;
    mode->width = monitorData->mode.width;
    mode->height = monitorData->mode.height;
    mode->refreshRate = monitorData->mode.refreshRate;

    return PAL_RESULT_SUCCESS;
}

PalResult wlSetMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult wlValidateMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult wlSetMonitorOrientation(
    PalMonitor* monitor,
    PalOrientation orientation)
{
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

#endif // PAL_HAS_WAYLAND_BACKEND