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

#ifdef _WIN32

#include "win32_helper.h"
#include "pal2/video/monitor.h"
#include <string.h>

#define MONITOR_DPI 0
#define MAX_MODE_COUNT 128

typedef struct MonitorData {
    int32_t count;
    int32_t maxCount;
    PalMonitor** monitors;
} MonitorData;

/* =========================================================================
 * Helper
 * ========================================================================= */

BOOL CALLBACK enumMonitors(
    HMONITOR monitor,
    HDC hdc,
    LPRECT lRect,
    LPARAM lParam)
{
    MonitorData* data = (MonitorData*)lParam;
    if (data->monitors) {
        if (data->count < data->maxCount) {
            data->monitors[data->count] = (PalMonitor*)monitor;
        }
    }

    data->count++;
    return PAL_TRUE;
}

static PalOrientation orientationFromWin32(DWORD orientation)
{
    switch (orientation) {
    case DMDO_DEFAULT:
        return PAL_ORIENTATION_LANDSCAPE;

    case DMDO_90:
        return PAL_ORIENTATION_PORTRAIT;

    case DMDO_180:
        return PAL_ORIENTATION_LANDSCAPE_FLIPPED;

    case DMDO_270:
        return PAL_ORIENTATION_PORTRAIT_FLIPPED;
    }

    return PAL_ORIENTATION_LANDSCAPE;
}

static DWORD orientationToin32(PalOrientation orientation)
{
    switch (orientation) {
    case PAL_ORIENTATION_LANDSCAPE:
        return DMDO_DEFAULT;

    case PAL_ORIENTATION_PORTRAIT:
        return DMDO_90;

    case PAL_ORIENTATION_LANDSCAPE_FLIPPED:
        return DMDO_180;

    case PAL_ORIENTATION_PORTRAIT_FLIPPED:
        return DMDO_270;
    }

    return 0;
}

static PalResult setMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode,
    PalBool test)
{
    MONITORINFOEXW mi = {0};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW((HMONITOR)monitor, (MONITORINFO*)&mi)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return palMakeResult(PAL_RESULT_CODE_INVALID_HANDLE, PAL_RESULT_SOURCE_WIN32, error);
        } else {
            return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, error);
        }
    }

    DWORD flags = DM_PELSWIDTH | DM_PELSHEIGHT;
    flags |= DM_DISPLAYFREQUENCY | DM_BITSPERPEL;

    DEVMODE devMode = {0};
    devMode.dmSize = sizeof(DEVMODE);
    devMode.dmFields = flags;

    devMode.dmPelsWidth = mode->width;
    devMode.dmPelsHeight = mode->height;
    devMode.dmDisplayFrequency = mode->refreshRate;
    devMode.dmBitsPerPel = mode->bpp;

    DWORD settingsFlag = CDS_FULLSCREEN;
    if (test) {
        settingsFlag = CDS_TEST;
    }

    ULONG result = ChangeDisplaySettingsExW(mi.szDevice, &devMode, nullptr, settingsFlag, nullptr);
    if (result != DISP_CHANGE_SUCCESSFUL) {
        return palMakeResult(PAL_RESULT_CODE_INVALID_HANDLE, PAL_RESULT_SOURCE_WIN32, GetLastError());
    }

    return PAL_RESULT_SUCCESS;
}

static PalBool compareMonitorMode(
    const PalMonitorMode* a,
    const PalMonitorMode* b)
{
    // clang-format off
    return a->bpp == b->bpp            &&
           a->width == b->width        &&
           a->height == b->height      &&
           a->refreshRate == b->refreshRate;
    // clang-format on
}

static void addMonitorMode(
    PalMonitorMode* modes,
    const PalMonitorMode* mode,
    uint32_t* count)
{
    for (uint32_t i = 0; i < *count; i++) {
        PalMonitorMode* oldMode = &modes[i];
        if (compareMonitorMode(oldMode, mode)) {
            return; // discard it
        }
    }

    // new mode
    modes[*count] = *mode;
    *count += 1;
}


/* =========================================================================
 * API
 * ========================================================================= */

PalResult win32EnumerateMonitors(
    uint32_t* count,
    PalMonitor** monitors)
{
    MonitorData data;
    data.count = 0;
    data.monitors = monitors;
    data.maxCount = monitors ? *count : 0;
    EnumDisplayMonitors(nullptr, nullptr, enumMonitors, (LPARAM)&data);

    if (!monitors) {
        *count = data.count;
    }

    return PAL_RESULT_SUCCESS;
}

void win32GetPrimaryMonitor(PalMonitor** monitor)
{
    HMONITOR primary = MonitorFromPoint((POINT){0, 0}, MONITOR_DEFAULTTOPRIMARY);
    *monitor = (PalMonitor*)primary;
}

void win32GetMonitorInfo(
    PalMonitor* monitor,
    PalMonitorInfo* info)
{
    MONITORINFOEXW mi = {0};
    mi.cbSize = sizeof(MONITORINFOEXW);
    GetMonitorInfoW((HMONITOR)monitor, (MONITORINFO*)&mi);

    info->x = mi.rcMonitor.left;
    info->y = mi.rcMonitor.top;
    info->width = mi.rcMonitor.right - mi.rcMonitor.left;
    info->height = mi.rcMonitor.bottom - mi.rcWork.top;

    WideCharToMultiByte(CP_UTF8, 0, mi.szDevice, -1, info->name, 32, nullptr, nullptr);

    DEVMODE devMode = {0};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    info->refreshRate = devMode.dmDisplayFrequency;
    info->orientation = orientationFromWin32(devMode.dmDisplayOrientation);

    UINT dpiX, dpiY;
    if (g_Win32.getDpiForMonitor) {
        g_Win32.getDpiForMonitor((HMONITOR)monitor, MONITOR_DPI, &dpiX, &dpiY);

    } else {
        dpiX = 96;
    }
    info->dpi = dpiX;

    HMONITOR primary = nullptr;
    primary = MonitorFromPoint((POINT){0, 0}, MONITOR_DEFAULTTOPRIMARY);
    if (!primary) {
        info->primary = PAL_FALSE;
    }

    if (primary == (HMONITOR)monitor) {
        info->primary = PAL_TRUE;
    }
}

void win32EnumerateMonitorModes(
    PalMonitor* monitor,
    uint32_t* count,
    PalMonitorMode* modes)
{
    uint32_t modeCount = 0;
    uint32_t maxModes = 0;
    PalMonitorMode* monitorModes = nullptr;
    uint64_t modesBytes = sizeof(PalMonitorMode) * MAX_MODE_COUNT;

    MONITORINFOEXW mi = {0};
    mi.cbSize = sizeof(MONITORINFOEXW);
    GetMonitorInfoW((HMONITOR)monitor, (MONITORINFO*)&mi);

    if (!modes) {
        monitorModes = palAllocate(g_Win32.allocator, modesBytes, 0);
        if (!monitorModes) {
            return;
        }
        memset(monitorModes, 0, modesBytes);
        maxModes = MAX_MODE_COUNT;

    } else {
        monitorModes = modes;
        maxModes = *count;
    }

    DEVMODEW dm = {0};
    dm.dmSize = sizeof(DEVMODE);
    for (int32_t i = 0; EnumDisplaySettingsW(mi.szDevice, i, &dm); i++) {
        if (modeCount > maxModes) {
            break;
        }

        PalMonitorMode* mode = &monitorModes[modeCount];
        mode->refreshRate = dm.dmDisplayFrequency;
        mode->width = dm.dmPelsWidth;
        mode->height = dm.dmPelsHeight;
        mode->bpp = dm.dmBitsPerPel;
        addMonitorMode(monitorModes, mode, &modeCount);
    }

    if (!modes) {
        *count = modeCount;
        palFree(g_Win32.allocator, monitorModes);
    }
}

void win32GetCurrentMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    MONITORINFOEXW mi = {0};
    mi.cbSize = sizeof(MONITORINFOEXW);
    GetMonitorInfoW((HMONITOR)monitor, (MONITORINFO*)&mi);

    DEVMODE devMode = {0};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

    mode->width = devMode.dmPelsWidth;
    mode->height = devMode.dmPelsHeight;
    mode->refreshRate = devMode.dmDisplayFrequency;
    mode->bpp = devMode.dmBitsPerPel;
}

PalResult win32SetMonitorMode(
    PalMonitor* monitor,
    const PalMonitorMode* mode)
{
    return setMonitorMode(monitor, mode, PAL_FALSE);
}

PalResult win32ValidateMonitorMode(
    PalMonitor* monitor,
    const PalMonitorMode* mode)
{
    return setMonitorMode(monitor, mode, PAL_TRUE);
}

PalResult win32SetMonitorOrientation(
    PalMonitor* monitor,
    PalOrientation orientation)
{
    DWORD _orientation = orientationToin32(orientation);
    MONITORINFOEXW mi = {0};
    mi.cbSize = sizeof(MONITORINFOEXW);
    GetMonitorInfoW((HMONITOR)monitor, (MONITORINFO*)&mi);

    DEVMODE devMode = {0};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    DWORD monitorOrientation = devMode.dmDisplayOrientation;

    // clang-format off
    PalBool isMonitorLandscape = (monitorOrientation == DMDO_DEFAULT ||
                                  monitorOrientation == DMDO_180);

    PalBool isLandscape = (_orientation == DMDO_DEFAULT ||
                           _orientation == DMDO_180);
    // clang-format on

    if (isMonitorLandscape != isLandscape) {
        DWORD tmp = devMode.dmPelsWidth;
        devMode.dmPelsWidth = devMode.dmPelsHeight;
        devMode.dmPelsHeight = tmp;
    }

    devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYORIENTATION;
    devMode.dmDisplayOrientation = _orientation;

    ULONG result = ChangeDisplaySettingsExW(mi.szDevice, &devMode, nullptr, CDS_RESET, nullptr);
    if (result != DISP_CHANGE_SUCCESSFUL) {
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, GetLastError());
    }

    return PAL_RESULT_SUCCESS;
}

#endif // _WIN32