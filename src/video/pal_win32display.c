
#include "pal_pch.h"
#include "pal_video_internal.h"

#define WIN32_DPI 0
#define WIN32_DPI_AWARE 2

typedef HRESULT (WINAPI* GetDpiForMonitorFn)(HMONITOR, int, UINT*, UINT*);
typedef HRESULT (WINAPI* SetProcessAwarenessFn)(int);

typedef struct DisplayData
{
    PalDisplay** displays;
    Uint32 count, maxCount;
    bool primaryOnly;
} DisplayData;

BOOL CALLBACK MonitorProc(HMONITOR monitor, HDC, LPRECT, LPARAM lParam)
{
    DisplayData* data = (DisplayData*)lParam;
    if (!data) {
        return PAL_FALSE;
    }

    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        return PAL_TRUE; 
    }

    if (data->displays) {
        if (data->count < data->maxCount) {
            data->displays[data->count] = (PalDisplay*)monitor;
        }
    }

    data->count++;
    return PAL_TRUE;
}

void getMonitorDPI(HMONITOR monitor, float* x, float* y)
{
    static HINSTANCE shcore;
    static GetDpiForMonitorFn getDpiForMonitor;
    static SetProcessAwarenessFn setProcessAwareness;

    if (!shcore) {
        shcore = LoadLibraryA("shcore.dll");
        if (!shcore) {
            *x = 1.0f;
            *y = 1.0f;
            return;
        }

        int dpiX, dpiY;
        getDpiForMonitor = (GetDpiForMonitorFn)GetProcAddress(
            shcore, 
            "GetDpiForMonitor"
        );

        setProcessAwareness = (SetProcessAwarenessFn)GetProcAddress(
            shcore, 
            "SetProcessDpiAwareness"
        );

        setProcessAwareness(WIN32_DPI_AWARE);
        getDpiForMonitor(monitor, WIN32_DPI, &dpiX, &dpiY);
        *x = (float)dpiX / 96.0f;
        *y = (float)dpiY / 96.0f;
    }
}

bool compareMode(const PalDisplayMode* a, const PalDisplayMode* b)
{
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

void getColorBits(PalDisplayMode* mode, int bpp)
{
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

void addMode(PalDisplayMode* modes, const PalDisplayMode* mode, int* count)
{
    // check if we have a duplicate mode
    for (int i = 0; i < *count; i++) {
        PalDisplayMode* oldMode = &modes[i];
        if (compareMode(oldMode, mode)) {
            // the same fields
            return;
        }
    }

    // new mode
    modes[*count] = *mode;
    *count += 1;
}

PalResult _PCALL palEnumerateDisplays(
    PalVideoInstance* videoInstance, 
    Uint32* count, 
    PalDisplay** displays)
{
    if (!videoInstance) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return PAL_RESULT_FAIL;
    }

    if (!count) {
        palSetError(PAL_ERROR_INVALID_ARGUMENT);
        return PAL_RESULT_FAIL;
    }

    DisplayData data;
    memset(&data, 0, sizeof(DisplayData));
    data.maxCount = *count;
    
    // if user just wants the display count
    if (displays) {
        data.displays = displays;
    }
    
    EnumDisplayMonitors(
        PAL_NULL, 
        PAL_NULL, 
        MonitorProc, 
        (LPARAM)&data
    );

    if (!displays) {
        *count = data.count;
    }
    return PAL_RESULT_OK;
}


PalResult _PCALL palEnumerateDisplayModes(
    PalDisplay* display, 
    Uint32* count, 
    PalDisplayMode* modes)
{
    if (!display) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return PAL_RESULT_FAIL;
    }

    if (!count) {
        palSetError(PAL_ERROR_INVALID_ARGUMENT);
        return PAL_RESULT_FAIL;
    }

    int modeCount = 0;
    HMONITOR monitor = (HMONITOR)display;
    PalDisplayMode* displayModes = PAL_NULL;
    bool free = PAL_FALSE;

    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        palSetError(PAL_ERROR_INVALID_DISPLAY);
        return PAL_RESULT_FAIL; 
    }

    if (!modes) {
        // allocate and store tmp display modes if the user passed NULL for modes
        // since Win32 DEVMODE fields are many, we will have duplicates PalDisplayMode.
        // this is because we are only interested in just a small number of fields
        displayModes = palAlloc(PAL_NULL, sizeof(PalDisplayMode) * 256);
        if (!displayModes) {
            palSetError(PAL_ERROR_ALLOCATION_FAILED);
            return PAL_RESULT_FAIL;
        }

        memset(displayModes, 0, sizeof(PalDisplayMode) * 256);
        free = PAL_TRUE;
    } else {
        free = PAL_FALSE;
        displayModes = modes;
    }

    DEVMODEW dm = {};
    dm.dmSize = sizeof(DEVMODE);
    for (int i = 0; EnumDisplaySettingsW(mi.szDevice, i, &dm); i++) {
        if (!free) {
            // user provided a modes array. we break so we dont go over
            if (modeCount > *count) {
                break;
            }
        }
        PalDisplayMode* mode = &displayModes[modeCount];
        mode->refreshRate = dm.dmDisplayFrequency;
        mode->width = dm.dmPelsWidth;
        mode->height = dm.dmPelsHeight;
        getColorBits(mode, dm.dmBitsPerPel);
        addMode(displayModes, mode, &modeCount);
    }

    if (!modes) {
        *count = modeCount;
    }

    if (free) {
        palFree(PAL_NULL, displayModes);
    }

    return PAL_RESULT_OK;
}

PalResult _PCALL palGetPrimaryDisplay(
    PalVideoInstance* videoInstance, 
    PalDisplay** outDisplay)
{
    if (!videoInstance || !outDisplay) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return PAL_RESULT_FAIL;
    }

    HMONITOR monitor = MonitorFromPoint((POINT){0, 0}, MONITOR_DEFAULTTOPRIMARY);
    *outDisplay = (PalDisplay*)monitor;
    return PAL_RESULT_OK;
}

PalResult _PCALL palGetDisplayInfo(PalDisplay* display, PalDisplayInfo* info)
{
    if (!display || !info) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return PAL_RESULT_FAIL;
    }

    HMONITOR monitor = (HMONITOR)display;
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        palSetError(PAL_ERROR_INVALID_DISPLAY);
        return PAL_RESULT_FAIL; 
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
    getMonitorDPI(monitor, &info->dpiScaleX, &info->dpiScaleY);
    return PAL_RESULT_OK;
}