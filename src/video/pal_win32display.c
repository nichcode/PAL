
#include "pal_pch.h"
#include "pal_video_c.h"

#define MAX_MODES 64
#define NAME_SIZE 32
#define WIN32_DPI 0
#define WIN32_DPI_AWARE 2

typedef HRESULT (WINAPI* GetDpiForMonitorFn)(HMONITOR, int, UINT*, UINT*);
typedef HRESULT (WINAPI* SetProcessAwarenessFn)(int);
BOOL CALLBACK palMonitorProcWin32(HMONITOR, HDC, LPRECT, LPARAM);

static HINSTANCE s_Instance;
static HINSTANCE s_Shcore;
static GetDpiForMonitorFn s_GetDpiForMonitor;
static SetProcessAwarenessFn s_SetProcessAwareness;

int _PCALL palGetDisplayCount()
{
    return g_Video.displayCount;
}

const PalDisplay* _PCALL palGetPrimaryDisplay()
{
    if (!g_Video.displayCount) {
        return PAL_NULL;
    }
    return &g_Video.displays[0];
}

const PalDisplay* _PCALL palGetDisplay(int index)
{
    if (index < 0 || index > g_Video.displayCount) {
        return PAL_NULL;
    }
    return &g_Video.displays[index];
}

bool palInitDisplay(PalAllocator* allocator)
{
    s_Shcore = LoadLibraryA("shcore.dll");
    if (!s_Shcore) {
        return PAL_FALSE;
    }

    // load function pointers
    s_GetDpiForMonitor = (GetDpiForMonitorFn)GetProcAddress(
        s_Shcore, 
        "GetDpiForMonitor"
    );

    s_SetProcessAwareness = (SetProcessAwarenessFn)GetProcAddress(
        s_Shcore, 
        "SetProcessDpiAwareness"
    );

    g_Video.displayCount = 0;
    EnumDisplayMonitors(PAL_NULL, PAL_NULL, palMonitorProcWin32, 0);
    return PAL_TRUE;
}

void palShutdownDisplay()
{
    for (int i = 0; i < g_Video.displayCount; i++) {
        PalDisplay* display = &g_Video.displays[i];
        g_Video.allocator.free(display->name);
        g_Video.allocator.free(display->modes);
    }
}

bool compareDisplayMode(const PalDisplayMode* a, const PalDisplayMode* b)
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

void addDisplayMode(const PalDisplayMode* mode)
{
    PalDisplay* display = &g_Video.displays[g_Video.displayCount];
    for (int i = 0; i < display->modeCount; i++) {
        if (compareDisplayMode(&display->modes[i], mode) == PAL_TRUE) {
            return;
        }
    }

    display->modes[display->modeCount] = *mode;
    display->modeCount++;
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

void getDisplayDPI(PalDisplay* display)
{
    UINT dpiX, dpiY;
    if (s_GetDpiForMonitor && s_SetProcessAwareness) {
        s_SetProcessAwareness(WIN32_DPI_AWARE);
        s_GetDpiForMonitor((HMONITOR)display->handle, WIN32_DPI, &dpiX, &dpiY);

        display->dpiScaleX = dpiX / 96.0f;
        display->dpiScaleY = dpiY / 96.0f;
        return;
    }

    // fallback
    display->dpiScaleX = 1.0f;
    display->dpiScaleY = 1.0f;
}

BOOL CALLBACK palMonitorProcWin32(HMONITOR monitor, HDC, LPRECT, LPARAM)
{
    if (g_Video.displayCount >= PAL_MAX_DISPLAYS) { 
        return PAL_FALSE; 
    }

    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        return TRUE; 
    }

    PalDisplay* display = &g_Video.displays[g_Video.displayCount];
    display->index = g_Video.displayCount;
    display->handle = (void*)monitor;

    // size and pos
    display->x = mi.rcMonitor.left;
    display->y = mi.rcMonitor.top;
    display->width = mi.rcMonitor.right - mi.rcMonitor.left;
    display->height = mi.rcMonitor.bottom - mi.rcWork.top;

    display->name = g_Video.allocator.alloc(NAME_SIZE);
    display->modes = g_Video.allocator.alloc(
        MAX_MODES * sizeof(PalDisplayMode)
    );

    // change into a UTF8 string
    WideCharToMultiByte(CP_UTF8, 0, mi.szDevice, -1, display->name, 32, NULL, NULL);
    getDisplayDPI(display);

    // get modes
    DEVMODEW dm = {};
    dm.dmSize = sizeof(DEVMODE);
    int i = 0;
    for (i = 0; EnumDisplaySettingsW(mi.szDevice, i, &dm); i++) {
        if (i >= MAX_MODES) {
            break;
        }

        PalDisplayMode mode;
        mode.refreshRate = dm.dmDisplayFrequency;
        mode.width = dm.dmPelsWidth;
        mode.height = dm.dmPelsHeight;
        getColorBits(&mode, dm.dmBitsPerPel);
        addDisplayMode(&mode);
    }

    display->modeCount--;
    display->refreshRate = display->modes[display->modeCount].refreshRate;
    g_Video.displayCount++;
    return PAL_TRUE;
}