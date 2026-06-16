
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef _WIN32
#include "pal_video_common_win32.h"
#include "pal_shared.h"

#define MONITOR_DPI 0
#define MAX_MODE_COUNT 128
#define NULL_ORIENTATION 5

typedef struct {
    int32_t count;
    int32_t maxCount;
    PalMonitor** monitors;
} MonitorData;

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

static inline PalOrientation orientationFromWin32(DWORD orientation)
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

static inline DWORD orientationToin32(PalOrientation orientation)
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
    return NULL_ORIENTATION;
}

static inline PalResult setMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode,
    PalBool test)
{
    if (!monitor || !mode) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    MONITORINFOEXW mi = {0};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW((HMONITOR)monitor, (MONITORINFO*)&mi)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return palMakeResult(
                PAL_RESULT_INVALID_HANDLE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);

        } else {
            return palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);
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

    ULONG result = ChangeDisplaySettingsExW(mi.szDevice, &devMode, NULL, settingsFlag, NULL);
    if (result == DISP_CHANGE_SUCCESSFUL) {
        return PAL_RESULT_SUCCESS;

    } else {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }
}

static inline PalBool compareMonitorMode(
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

static inline void addMonitorMode(
    PalMonitorMode* modes,
    const PalMonitorMode* mode,
    int32_t* count)
{
    // check if we have a duplicate mode
    for (int32_t i = 0; i < *count; i++) {
        PalMonitorMode* oldMode = &modes[i];
        if (compareMonitorMode(oldMode, mode)) {
            return; // discard it
        }
    }

    // new mode
    modes[*count] = *mode;
    *count += 1;
}

PalResult PAL_CALL palEnumerateMonitors(
    int32_t* count,
    PalMonitor** outMonitors)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!count || *count == 0 && outMonitors) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    MonitorData data;
    data.count = 0;
    data.monitors = outMonitors;
    data.maxCount = outMonitors ? *count : 0;
    EnumDisplayMonitors(nullptr, nullptr, enumMonitors, (LPARAM)&data);

    if (!outMonitors) {
        *count = data.count;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetPrimaryMonitor(PalMonitor** outMonitor)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!outMonitor) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    HMONITOR monitor = nullptr;
    monitor = MonitorFromPoint((POINT){0, 0}, MONITOR_DEFAULTTOPRIMARY);
    if (!monitor) {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    *outMonitor = (PalMonitor*)monitor;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetMonitorInfo(
    PalMonitor* monitor,
    PalMonitorInfo* info)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!monitor || !info) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    MONITORINFOEXW mi = {0};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW((HMONITOR)monitor, (MONITORINFO*)&mi)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return palMakeResult(
                PAL_RESULT_INVALID_HANDLE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);

        } else {
            return palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);
        }
    }

    info->x = mi.rcMonitor.left;
    info->y = mi.rcMonitor.top;
    info->width = mi.rcMonitor.right - mi.rcMonitor.left;
    info->height = mi.rcMonitor.bottom - mi.rcWork.top;

    // get name
    WideCharToMultiByte(CP_UTF8, 0, mi.szDevice, -1, info->name, 32, NULL, NULL);

    DEVMODE devMode = {0};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    info->refreshRate = devMode.dmDisplayFrequency;
    info->orientation = orientationFromWin32(devMode.dmDisplayOrientation);

    // get dpi scale
    UINT dpiX, dpiY;
    if (s_Video.getDpiForMonitor) {
        s_Video.getDpiForMonitor((HMONITOR)monitor, MONITOR_DPI, &dpiX, &dpiY);

    } else {
        dpiX = 96;
    }
    info->dpi = dpiX;

    // check for primary monitor
    HMONITOR primary = nullptr;
    primary = MonitorFromPoint((POINT){0, 0}, MONITOR_DEFAULTTOPRIMARY);
    if (!primary) {
        info->primary = PAL_FALSE;
    }

    if (primary == (HMONITOR)monitor) {
        info->primary = PAL_TRUE;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palEnumerateMonitorModes(
    PalMonitor* monitor,
    int32_t* count,
    PalMonitorMode* modes)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!monitor || !count || *count == 0 && modes) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    int32_t modeCount = 0;
    int32_t maxModes = 0;
    PalMonitorMode* monitorModes = nullptr;

    MONITORINFOEXW mi = {0};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW((HMONITOR)monitor, (MONITORINFO*)&mi)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return palMakeResult(
                PAL_RESULT_INVALID_HANDLE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);

        } else {
            return palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);
        }
    }

    if (!modes) {
        // allocate and store tmp monitor modesand check for the interested
        // fields.
        monitorModes = palAllocate(s_Video.allocator, sizeof(PalMonitorMode) * MAX_MODE_COUNT, 0);
        if (!monitorModes) {
            return palMakeResult(
                PAL_RESULT_OUT_OF_MEMORY, 
                PAL_RESULT_SOURCE_WINDOWS, 
                GetLastError());
        }

        memset(monitorModes, 0, sizeof(PalMonitorMode) * MAX_MODE_COUNT);
        maxModes = MAX_MODE_COUNT;

    } else {
        monitorModes = modes;
        maxModes = *count;
    }

    DEVMODEW dm = {0};
    dm.dmSize = sizeof(DEVMODE);
    for (int32_t i = 0; EnumDisplaySettingsW(mi.szDevice, i, &dm); i++) {
        // Pal support up to 128 modes
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
        palFree(s_Video.allocator, monitorModes);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetCurrentMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!monitor || !mode) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    MONITORINFOEXW mi = {0};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW((HMONITOR)monitor, (MONITORINFO*)&mi)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return palMakeResult(
                PAL_RESULT_INVALID_HANDLE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);

        } else {
            return palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);
        }
    }

    DEVMODE devMode = {0};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    mode->width = devMode.dmPelsWidth;
    mode->height = devMode.dmPelsHeight;
    mode->refreshRate = devMode.dmDisplayFrequency;
    mode->bpp = devMode.dmBitsPerPel;

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    return setMonitorMode(monitor, mode, PAL_FALSE);
}

PalResult PAL_CALL palValidateMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    return setMonitorMode(monitor, mode, PAL_TRUE);
}

PalResult PAL_CALL palSetMonitorOrientation(
    PalMonitor* monitor,
    PalOrientation orientation)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!monitor) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    DWORD win32Orientation = orientationToin32(orientation);
    if (orientation == NULL_ORIENTATION) {
        return palMakeResult(
            PAL_RESULT_INVALID_OPERATION, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    MONITORINFOEXW mi = {0};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW((HMONITOR)monitor, (MONITORINFO*)&mi)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return palMakeResult(
                PAL_RESULT_INVALID_HANDLE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);

        } else {
            return palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);
        }
    }

    DEVMODE devMode = {0};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    DWORD monitorOrientation = devMode.dmDisplayOrientation;

    // clang-format off
    // only swap size if switching between landscape and portrait
    PalBool isMonitorLandscape = (monitorOrientation == DMDO_DEFAULT ||
                               monitorOrientation == DMDO_180);

    PalBool isLandscape = (win32Orientation == DMDO_DEFAULT ||
                        win32Orientation == DMDO_180);
    // clang-format on

    if (isMonitorLandscape != isLandscape) {
        DWORD tmp = devMode.dmPelsWidth;
        devMode.dmPelsWidth = devMode.dmPelsHeight;
        devMode.dmPelsHeight = tmp;
    }

    devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYORIENTATION;
    devMode.dmDisplayOrientation = win32Orientation;

    ULONG result = ChangeDisplaySettingsExW(mi.szDevice, &devMode, NULL, CDS_RESET, NULL);
    if (result == DISP_CHANGE_SUCCESSFUL) {
        return PAL_RESULT_SUCCESS;

    } else {
        return palMakeResult(
            PAL_RESULT_INVALID_OPERATION, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }
}

#endif // _WIN32