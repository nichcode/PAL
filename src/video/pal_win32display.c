
#include "pal_pch.h"
#include "pal_video_internal.h"

#define WIN32_DPI 0
#define WIN32_DPI_AWARE 2

typedef HRESULT (WINAPI* GetDpiForMonitorFn)(HMONITOR, int, UINT*, UINT*);
typedef HRESULT (WINAPI* SetProcessAwarenessFn)(int);

typedef struct DisplayData
{
    Uint32 count;
    PalDisplay** displays;
} DisplayData;

BOOL CALLBACK palMonitorProc(HMONITOR monitor, HDC, LPRECT, LPARAM lParam)
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
        data->displays[data->count] = (PalDisplay*)monitor;
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

PalResult _PCALL palEnumerateDisplays(
    PalVideo* video, 
    Uint32* count, 
    PalDisplay** displays)
{
    if (!video) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return PAL_RESULT_FAIL;
    }

    DisplayData data;
    memset(&data, 0, sizeof(DisplayData));
    
    // if user just wants the display count
    if (displays) {
        data.displays = displays;
    }
    
    EnumDisplayMonitors(
        PAL_NULL, 
        PAL_NULL, 
        palMonitorProc, 
        (LPARAM)&data
    );

    if (count) {
        *count = data.count;
    }
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