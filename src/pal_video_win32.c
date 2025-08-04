
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
#define MAX_MODE_COUNT 128
#define NULL_ORIENTATION 5

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

static PalDisplayOrientation orientationFromWin32(DWORD orientation);
static DWORD orientationToin32(PalDisplayOrientation orientation);

static PalResult setDisplayMode(
    PalDisplay* display,
    PalDisplayMode* mode,
    bool test);

static void addMode(
    PalDisplayMode* modes, 
    const PalDisplayMode* mode, 
    int* count);

static bool compareMode(
    const PalDisplayMode* a, 
    const PalDisplayMode* b);

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

    if (count == 0) {
        PAL_RESULT_INSUFFICIENT_BUFFER;
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

    DEVMODE devMode = {};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    info->refreshRate = devMode.dmDisplayFrequency;   
    info->orientation = orientationFromWin32(devMode.dmDisplayOrientation);

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

PalResult _PCALL palEnumerateDisplayModes(
    PalDisplay* display,
    int* count,
    PalDisplayMode* modes) {

    if (!display || !count) {
        return PAL_RESULT_NULL_POINTER;
    }

    int modeCount = 0;
    int maxModes = 0;
    HMONITOR monitor = (HMONITOR)display;
    PalDisplayMode* displayModes = nullptr;

    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        return PAL_RESULT_INVALID_DISPLAY;
    }

    if (!modes) {
        // allocate and store tmp display modes
        // and check for the interested fields.
        displayModes = palAllocate(
            nullptr, 
            sizeof(PalDisplayMode) * MAX_MODE_COUNT,
            0
        );

        if (!displayModes) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        memset(
            displayModes, 
            0,
            sizeof(PalDisplayMode) * MAX_MODE_COUNT
        );
        maxModes = MAX_MODE_COUNT;

    } else {
        displayModes = modes;
        maxModes = *count;
    }

    DEVMODEW dm = {};
    dm.dmSize = sizeof(DEVMODE);
    for (int i = 0; EnumDisplaySettingsW(mi.szDevice, i, &dm); i++) {
        // Pal support up to 128 modes
        if (modeCount > maxModes) {
            break;
        }

        PalDisplayMode* mode = &displayModes[modeCount];
        mode->refreshRate = dm.dmDisplayFrequency;
        mode->width = dm.dmPelsWidth;
        mode->height = dm.dmPelsHeight;
        mode->bpp = dm.dmBitsPerPel;
        addMode(displayModes, mode, &modeCount);
    }

    if (!modes) {
        *count = modeCount;
        palFree(nullptr, displayModes);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult _PCALL palGetCurrentDisplayMode(
    PalDisplay* display,
    PalDisplayMode* mode) {

    if (!display || !mode) {
        return PAL_RESULT_NULL_POINTER;
    }

    HMONITOR monitor = (HMONITOR)display;
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        return PAL_RESULT_INVALID_DISPLAY; 
    }

    DEVMODE devMode = {};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    mode->width = devMode.dmPelsWidth;
    mode->height = devMode.dmPelsHeight;
    mode->refreshRate = devMode.dmDisplayFrequency;
    mode->bpp = devMode.dmBitsPerPel;
}

PalResult _PCALL palSetDisplayMode(
    PalDisplay* display,
    PalDisplayMode* mode) {

    return setDisplayMode(display, mode, FALSE);
}

PalResult _PCALL palValidateDisplayMode(
    PalDisplay* display,
    PalDisplayMode* mode) {

    return setDisplayMode(display, mode, TRUE);
}

PalResult _PCALL palSetDisplayOrientation(
    PalDisplay* display,
    PalDisplayOrientation orientation) {

    if (!display) {
        return PAL_RESULT_NULL_POINTER;
    }

    DWORD win32Orientation = orientationToin32(orientation);
    if (orientation == NULL_ORIENTATION) {
        return PAL_RESULT_INVALID_ORIENTATION;
    }

    HMONITOR monitor = (HMONITOR)display;
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        return PAL_RESULT_INVALID_DISPLAY; 
    }

    DEVMODE devMode = {};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    DWORD displayOrientation = devMode.dmDisplayOrientation;

    // only swap size if switching between landscape and portrait
    bool isDisplayLandscape = (displayOrientation == DMDO_DEFAULT || displayOrientation == DMDO_180);
    bool isLandscape = (win32Orientation == DMDO_DEFAULT || win32Orientation == DMDO_180);
    if (isDisplayLandscape != isLandscape) {
        DWORD tmp = devMode.dmPelsWidth;
        devMode.dmPelsWidth = devMode.dmPelsHeight;
        devMode.dmPelsHeight = tmp;
    }

    devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYORIENTATION;
    devMode.dmDisplayOrientation = win32Orientation;

    ULONG result = ChangeDisplaySettingsExW(
        mi.szDevice,
        &devMode,
        NULL,
        CDS_RESET,
        NULL
    );

    if (result == DISP_CHANGE_SUCCESSFUL) {
        return PAL_RESULT_SUCCESS;
    } else {
        return PAL_RESULT_INVALID_ORIENTATION;
    }    
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

static PalDisplayOrientation orientationFromWin32(DWORD orientation) {

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
}

static DWORD orientationToin32(PalDisplayOrientation orientation) {

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

static PalResult setDisplayMode(
    PalDisplay* display,
    PalDisplayMode* mode,
    bool test) {
    
    if (!display || !mode) {
        return PAL_RESULT_NULL_POINTER;
    } 

    HMONITOR monitor = (HMONITOR)display;
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        return PAL_RESULT_INVALID_DISPLAY; 
    }

    DWORD flags = DM_PELSWIDTH | DM_PELSHEIGHT;
    flags |= DM_DISPLAYFREQUENCY | DM_BITSPERPEL;

    DEVMODE devMode = {};
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

    ULONG result = ChangeDisplaySettingsExW(
        mi.szDevice,
        &devMode,
        NULL,
        settingsFlag,
        NULL
    );

    if (result == DISP_CHANGE_SUCCESSFUL) {
        return PAL_RESULT_SUCCESS;
    } else {
        return PAL_RESULT_INVALID_DISPLAY_MODE;
    }
}

static bool compareMode(
    const PalDisplayMode* a, 
    const PalDisplayMode* b) {

    return 
        a->bpp == b->bpp               &&
        a->width == b->width           &&
        a->height == b->height         &&
        a->refreshRate == b->refreshRate;
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