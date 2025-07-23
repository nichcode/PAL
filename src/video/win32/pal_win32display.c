
#include "pal_pch.h"
#include "video/pal_video_internal.h"
#include "pal_win32video.h"

#define WIN32_DPI 0
#define WIN32_DPI_AWARE 2
#define MAX_MODE_COUNT 128

typedef HRESULT (WINAPI* GetDpiForMonitorFn)(HMONITOR, int, UINT*, UINT*);
typedef HRESULT (WINAPI* SetProcessAwarenessFn)(int);

static GetDpiForMonitorFn s_GetDpiForMonitor;
static SetProcessAwarenessFn s_SetProcessAwareness;

BOOL CALLBACK monitorProc(HMONITOR, HDC, LPRECT, LPARAM);

typedef struct DisplayData {
    PalVideoDataWin32* videoData;
    PalDisplay* displays;
    int count;
    int maxCount;
} DisplayData;

static void getMonitorDPI(HMONITOR monitor, int* dpi);

static void getModes(
    HMONITOR monitor, 
    const wchar_t* name, 
    PalDisplayMode* modes,
    int* count,
    int maxCount);

static bool compareMode(
    const PalDisplayMode* a, 
    const PalDisplayMode* b);

static void getColorBits(
    PalDisplayMode* mode, 
    int bpp);

static void addMode(
    PalDisplayMode* modes, 
    const PalDisplayMode* mode, 
    int* count);

PalResult _PCALL palEnumerateDisplays(
    PalVideo video,
    int* count,
    PalDisplay* displays) {

    if (!video || !count) {
        return PAL_ERROR_NULL_POINTER;
    }

    DisplayData data;
    data.count = 0;
    data.displays = displays;
    data.maxCount = displays ? *count : 0;
    data.videoData = video->platformData;

    // load function pointers
    s_GetDpiForMonitor = (GetDpiForMonitorFn)GetProcAddress(
        data.videoData->shcore,
        "GetDpiForMonitor"
    );

    s_SetProcessAwareness = (SetProcessAwarenessFn)GetProcAddress(
        data.videoData->shcore,
        "SetProcessDpiAwareness"
    );

    EnumDisplayMonitors(PAL_NULL, PAL_NULL, monitorProc, (LPARAM)&data);

    if (!displays) {
        *count = data.count;
    }
    return PAL_SUCCESS;
}

PalResult _PCALL palGetDisplayInfo(
    PalDisplay display, 
    PalDisplayInfo* info) {

    if (!display || !info) {
        return PAL_ERROR_NULL_POINTER;
    }

    HMONITOR monitor = (HMONITOR)display;
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        return PAL_ERROR_INVALID_DISPLAY; 
    }

    info->x = mi.rcMonitor.left;
    info->y = mi.rcMonitor.top;
    info->width = mi.rcMonitor.right - mi.rcMonitor.left;
    info->height = mi.rcMonitor.bottom - mi.rcWork.top;

    // get name
    WideCharToMultiByte(CP_UTF8, 0, mi.szDevice, -1, info->name, 32, NULL, NULL);

    // get refreshrate
    DEVMODE devMode = {};
    devMode.dmSize = sizeof(DEVMODE);
    bool success = EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    if (!success) {
        info->refreshRate = 60;
    } else {
        info->refreshRate = devMode.dmDisplayFrequency;
    }

    // get dpi scale
    getMonitorDPI(monitor, &info->dpi);
    return PAL_SUCCESS;
}

PalResult _PCALL palGetPrimaryDisplay(
    PalVideo video, 
    PalDisplay* outDisplay) {

    if (!video || !outDisplay) {
        return PAL_ERROR_NULL_POINTER;
    }

    HMONITOR monitor = MonitorFromPoint((POINT){0, 0}, MONITOR_DEFAULTTOPRIMARY);
    *outDisplay = (PalDisplay)monitor;
    return PAL_SUCCESS;
}

PalResult _PCALL palEnumerateDisplayModes(
    PalDisplay display,
    int* count,
    PalDisplayMode* modes) {

    if (!display || !count) {
        return PAL_ERROR_NULL_POINTER;
    }

    int modeCount = 0;
    int maxModes = 0;
    HMONITOR monitor = (HMONITOR)display;
    PalDisplayMode* displayModes = PAL_NULL;
    bool free = PAL_FALSE;

    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        return PAL_ERROR_INVALID_DISPLAY;
    }

    if (!modes) {
        // allocate and store tmp display modes
        // and check for the interested fields.
        displayModes = palAllocate(PAL_NULL, sizeof(PalDisplayMode) * MAX_MODE_COUNT);
        if (!displayModes) {
            return PAL_ERROR_OUT_OF_MEMORY;
        }

        palZeroMemory(displayModes, sizeof(PalDisplayMode) * MAX_MODE_COUNT);
        free = PAL_TRUE;
        maxModes = MAX_MODE_COUNT;

    } else {
        free = PAL_FALSE;
        displayModes = modes;
        maxModes = *count;
    }

    getModes(
        monitor, 
        mi.szDevice, 
        displayModes,
        &modeCount,
        maxModes);

    *count = modeCount;
    if (free) {
        palFree(PAL_NULL, displayModes);
    }
    return PAL_SUCCESS;
}

BOOL CALLBACK monitorProc(HMONITOR monitor, HDC, LPRECT, LPARAM lParam) {

    DisplayData* data = (DisplayData*)lParam;
    if (data->displays) {
        if (data->count < data->maxCount) {
            data->displays[data->count] = (PalDisplay)monitor;
        }
    }

    data->count++;
    return PAL_TRUE;
}

static void getMonitorDPI(HMONITOR monitor, int* dpi) {

    if (!s_GetDpiForMonitor || !s_SetProcessAwareness) {
        *dpi = 96;
        return;
    }

    int dpiX, dpiY;
    s_SetProcessAwareness(WIN32_DPI_AWARE);
    s_GetDpiForMonitor(monitor, WIN32_DPI, &dpiX, &dpiY);
    *dpi = dpiX;
}

static void getModes(
    HMONITOR monitor, 
    const wchar_t* name, 
    PalDisplayMode* modes,
    int* count,
    int maxCount) {

    DEVMODEW dm = {};
    dm.dmSize = sizeof(DEVMODE);
    for (int i = 0; EnumDisplaySettingsW(name, i, &dm); i++) {
        // Pal support up to 128 modes
        if (*count > maxCount) {
            break;
        }

        PalDisplayMode* mode = &modes[*count];
        mode->refreshRate = dm.dmDisplayFrequency;
        mode->width = dm.dmPelsWidth;
        mode->height = dm.dmPelsHeight;
        getColorBits(mode, dm.dmBitsPerPel);
        addMode(modes, mode, count);
    }
}

static bool compareMode(
    const PalDisplayMode* a, 
    const PalDisplayMode* b) {

    return 
        a->alphaBits == b->alphaBits   &&
        a->redBits == b->redBits       &&
        a->greenBits == b->greenBits   &&
        a->blueBits == b->blueBits     &&
        a->alphaBits == b->alphaBits   &&
        a->width == b->width           &&
        a->height == b->height         &&
        a->refreshRate == b->refreshRate;
}

static void getColorBits(
    PalDisplayMode* mode, 
    int bpp) {

    switch (bpp) {
        case 16: {
            mode->redBits = 5;
            mode->greenBits = 6;
            mode->blueBits = 5;
            mode->alphaBits = 0;
            return;
        }

        case 24: {
            mode->redBits = mode->greenBits = 8;
            mode->blueBits = 8;
            mode->alphaBits = 0;
            return;
        }

        case 32: {
            mode->redBits = mode->greenBits = 8;
            mode->blueBits = mode->alphaBits = 8;
            return;
        }
    }
    mode->redBits = mode->greenBits = 0;
    mode->blueBits = mode->alphaBits = 0;
}

static void addMode(
    PalDisplayMode* modes, 
    const PalDisplayMode* mode, 
    int* count) {

    // check if we have a duplicate mode
    for (int i = 0; i < *count; i++) {
        PalDisplayMode* oldMode = &modes[i];
        if (compareMode(oldMode, mode)) {
            return;
        }
    }

    // new mode
    modes[*count] = *mode;
    *count += 1;
}