
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef __linux__
#if PAL_HAS_WAYLAND_BACKEND == 1

#include "pal_wayland.h"
#include "pal_shared.h"

PalResult wlEnumerateMonitors(
    int32_t* count,
    PalMonitor** outMonitors)
{
    if (outMonitors) {
        int index = 0;
        int maxCount = s_Video.maxMonitorData;
        for (int i = 0; i < maxCount && index < *count; i++) {
            if (s_Video.monitorData[i].used) {
                // found a monitor
                PalMonitor* monitor = s_Video.monitorData[index].monitor;
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
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalResult wlGetMonitorInfo(
    PalMonitor* monitor,
    PalMonitorInfo* info)
{
    MonitorData* monitorData = findMonitorData(monitor);
    if (!monitorData) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
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
    MonitorData* monitorData = findMonitorData(monitor);
    if (!monitorData) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
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
    MonitorData* monitorData = findMonitorData(monitor);
    if (!monitorData) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
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
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalResult wlValidateMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalResult wlSetMonitorOrientation(
    PalMonitor* monitor,
    PalOrientation orientation)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

#endif // PAL_HAS_WAYLAND_BACKEND
#endif // __linux__