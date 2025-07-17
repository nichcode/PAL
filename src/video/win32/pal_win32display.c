
#include "pal_pch.h"
#include "video/pal_video_internal.h"

#define WIN32_DPI 0
#define WIN32_DPI_AWARE 2

typedef HRESULT (WINAPI* GetDpiForMonitorFn)(HMONITOR, int, UINT*, UINT*);
typedef HRESULT (WINAPI* SetProcessAwarenessFn)(int);
BOOL CALLBACK monitorProc(HMONITOR, HDC, LPRECT, LPARAM);

typedef struct DisplayData
{
    PalDisplay** displays;
    int count;
    int maxCount;
} DisplayData;

static HINSTANCE s_Shcore;
static GetDpiForMonitorFn s_GetDpiForMonitor;
static SetProcessAwarenessFn s_SetProcessAwareness;

void getMonitorDPI(HMONITOR monitor, float* x, float* y);

PalResult _PCALL palEnumerateDisplays(
    PalVideo* video,
    int* count,
    PalDisplay** displays) {

    if (!video || !count) {
        return PAL_ERROR_NULL_POINTER;
    }

    DisplayData data;
    data.count = 0;
    data.displays = displays;
    data.maxCount = displays ? *count : 0;

    EnumDisplayMonitors(PAL_NULL, PAL_NULL, monitorProc, (LPARAM)&data);
    *count = data.count;
    return PAL_SUCCESS;
}

PalResult _PCALL palGetDisplayInfo(
    PalDisplay* display, 
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
    getMonitorDPI(monitor, &info->dpiScaleX, &info->dpiScaleY);
    return PAL_SUCCESS;
}

PalResult _PCALL palGetPrimaryDisplay(
    PalVideo* video, 
    PalDisplay** outDisplay) {

    if (!video || !outDisplay) {
        return PAL_ERROR_NULL_POINTER;
    }

    HMONITOR monitor = MonitorFromPoint((POINT){0, 0}, MONITOR_DEFAULTTOPRIMARY);
    *outDisplay = (PalDisplay*)monitor;
    return PAL_SUCCESS;
}

BOOL CALLBACK monitorProc(HMONITOR monitor, HDC, LPRECT, LPARAM lParam) {

    DisplayData* data = (DisplayData*)lParam;
    if (!data) {
        return PAL_FALSE;
    }

    if (data->displays) {
        if (data->count < data->maxCount) {
            data->displays[data->count] = (PalDisplay*)monitor;
        }
    }

    data->count++;
    return PAL_TRUE;
}

void getMonitorDPI(HMONITOR monitor, float* x, float* y) {

    if (!s_Shcore) {
        s_Shcore = LoadLibraryA("shcore.dll");
        if (!s_Shcore) {
            *x = 1.0f;
            *y = 1.0f;
            return;
        }

        if (!s_GetDpiForMonitor) {
            s_GetDpiForMonitor = (GetDpiForMonitorFn)GetProcAddress(
                s_Shcore, 
                "GetDpiForMonitor"
            );
        }

        if (!s_SetProcessAwareness) {
            s_SetProcessAwareness = (SetProcessAwarenessFn)GetProcAddress(
                s_Shcore, 
                "SetProcessDpiAwareness"
            );
        }

        int dpiX, dpiY;
        s_SetProcessAwareness(WIN32_DPI_AWARE);
        s_GetDpiForMonitor(monitor, WIN32_DPI, &dpiX, &dpiY);
        *x = (float)dpiX / 96.0f;
        *y = (float)dpiY / 96.0f;
    }
}