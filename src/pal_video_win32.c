
/**

Copyright (C) 2025 Nicholas Agbo
  
This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:
  
1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required. 
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

#include "pal/pal_video.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

// set unicode
#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include <windows.h>
#endif // _WIN32

#define PAL_WIN32_VIDEO_CLASS L"PALVideoClass"
#define PAL_WIN32_VIDEO_PROP L"PALVideo"
#define WIN32_DPI 0
#define WIN32_DPI_AWARE 2

typedef HRESULT (WINAPI* GetDpiForMonitorFn)(HMONITOR, int, UINT*, UINT*);
typedef HRESULT (WINAPI* SetProcessAwarenessFn)(int);

static GetDpiForMonitorFn s_GetDpiForMonitor;
static SetProcessAwarenessFn s_SetProcessAwareness;
static HINSTANCE s_Shcore;

typedef struct PalVideoSystem {
    PalAllocator* allocator;
    HINSTANCE instance;
} PalVideoSystem;

typedef struct DisplayData {
    PalDisplay** displays;
    int count;
    int maxCount;
} DisplayData;

static void getMonitorDPI(
    HMONITOR monitor, 
    int* dpi);

BOOL CALLBACK enumMonitors(
    HMONITOR monitor, 
    HDC hdc, 
    LPRECT lRect, 
    LPARAM lParam);

LRESULT CALLBACK videoProc(
    HWND hwnd, 
    UINT msg, 
    WPARAM wParam, 
    LPARAM lParam);

// ==================================================
// Public API
// ==================================================

PalResult _PCALL palCreateVideoSystem(
    const PalVideoSystemCreateInfo* info,
    PalVideoSystem** outSystem) {

    PalVideoSystem* system;
    if (!info || !outSystem) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (info->allocator && 
    (!info->allocator->allocate || !info->allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    system = palAllocate(info->allocator, sizeof(PalVideoSystem), 0);
    if (!system) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(system, 0, sizeof(PalVideoSystem));
    if (info->allocator) {
        system->allocator = info->allocator;
    }

    // register class
    system->instance = GetModuleHandleW(nullptr);
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    if (!GetClassInfoExW(system->instance, PAL_WIN32_VIDEO_CLASS, &wc)) {
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hbrBackground = NULL;
        wc.hCursor = LoadCursorW(system->instance, IDC_ARROW);
        wc.hIcon = LoadIconW(system->instance, IDI_APPLICATION);
        wc.hIconSm = LoadIconW(system->instance, IDI_APPLICATION);
        wc.hInstance = system->instance;
        wc.lpfnWndProc = videoProc;
        wc.lpszClassName = PAL_WIN32_VIDEO_CLASS;
        wc.lpszMenuName = NULL;
        wc.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

        if (!RegisterClassExW(&wc)) {
            return PAL_RESULT_ACCESS_DENIED;
        }
    }

    // load shared libraries
    s_Shcore = LoadLibraryA("shcore.dll");
    if (s_Shcore) {
        s_GetDpiForMonitor = (GetDpiForMonitorFn)GetProcAddress(
            s_Shcore,
            "GetDpiForMonitor"
        );

        s_SetProcessAwareness = (SetProcessAwarenessFn)GetProcAddress(
            s_Shcore,
            "SetProcessDpiAwareness"
        );
    }

    *outSystem = system;
    return PAL_RESULT_SUCCESS;
}

void _PCALL palDestroyVideoSystem(
    PalVideoSystem* system) {

    if (!system || (system && !system->instance)) {
        return;
    }

    UnregisterClassW(PAL_WIN32_VIDEO_CLASS, system->instance);
    palFree(system->allocator, system);
}

PalResult _PCALL palEnumerateDisplays(
    PalVideoSystem* system,
    int* count,
    PalDisplay** displays) {

    if (!system || !count) {
        return PAL_RESULT_NULL_POINTER;
    }

    DisplayData data;
    data.count = 0;
    data.displays = displays;
    data.maxCount = displays ? *count : 0;
    EnumDisplayMonitors(nullptr, nullptr, enumMonitors, (LPARAM)&data);

    if (!displays) {
        *count = data.count;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult _PCALL palGetPrimaryDisplay(
    PalVideoSystem* system,
    PalDisplay** outDisplay) {

    if (!system || !outDisplay) {
        return PAL_RESULT_NULL_POINTER;
    }

    HMONITOR monitor = MonitorFromPoint((POINT){0, 0}, MONITOR_DEFAULTTOPRIMARY);
    *outDisplay = (PalDisplay*)monitor;
    return PAL_RESULT_SUCCESS;
}

PalResult _PCALL palGetDisplayInfo(
    PalDisplay* display, 
    PalDisplayInfo* info) {

    if (!display || !info) {
        return PAL_RESULT_NULL_POINTER;
    }

    HMONITOR monitor = (HMONITOR)display;
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        return PAL_RESULT_INVALID_DISPLAY; 
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
    int dpi;
    getMonitorDPI(monitor, &dpi);
    info->dpi = dpi;

    // check for primary display
    HMONITOR primaryDisplay = MonitorFromPoint(
        (POINT){0, 0}, 
        MONITOR_DEFAULTTOPRIMARY
    );

    if (primaryDisplay == monitor) {
        info->primary = true;
    }

    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Internal API
// ==================================================

static void getMonitorDPI(
    HMONITOR monitor, 
    int* dpi) {

    if (!s_GetDpiForMonitor || !s_SetProcessAwareness) {
        *dpi = 96;
        return;
    }

    int dpiX, dpiY;
    s_SetProcessAwareness(WIN32_DPI_AWARE);
    s_GetDpiForMonitor(monitor, WIN32_DPI, &dpiX, &dpiY);
    *dpi = dpiX;
}

BOOL CALLBACK enumMonitors(
    HMONITOR monitor, 
    HDC hdc, 
    LPRECT lRect, 
    LPARAM lParam) {
    
    DisplayData* data = (DisplayData*)lParam;
    if (data->displays) {
        if (data->count < data->maxCount) {
            data->displays[data->count] = (PalDisplay*)monitor;
        }
    }

    data->count++;
    return TRUE;
}

LRESULT CALLBACK videoProc(
    HWND hwnd, 
    UINT msg, 
    WPARAM wParam, 
    LPARAM lParam) {

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}