
#include "pal_win32platform.h"
#include "pal/pal_thread.h"
#include "pal/pal_log.h"

//************************************************************
//********************Win32 platform API**********************
//************************************************************

bool compareMode(const PalDisplayMode* a, const PalDisplayMode* b);
void addMode(const PalDisplayMode* mode);
void getColorBits(PalDisplayMode* mode, int bpp);
void getDisplayDPI(PalDisplay* display);

void palToWstrUTF8Win32(wchar_t* buffer, const char* string)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, string, -1, PAL_NULL, 0);
    MultiByteToWideChar(CP_UTF8, 0, string, -1, buffer, len);
}

bool _palIsVersionWin32(int major, int minor, int servicePack)
{
    OSVERSIONINFOEXW osvi = { 0 };
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
    osvi.dwMajorVersion = major;
    osvi.dwMinorVersion = minor;
    osvi.wServicePackMajor = servicePack;

    DWORDLONG mask = 0;
    mask = VerSetConditionMask(mask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    mask = VerSetConditionMask(mask, VER_MINORVERSION, VER_GREATER_EQUAL);
    mask = VerSetConditionMask(mask, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
}

Uint32 _palGetBuildWin32()
{
    RtlGetVersionPtr RtlGetVersion = PAL_NULL;
    RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(s_Ntdll, "RtlGetVersion");
    if (!RtlGetVersion) return 0;

    RTL_OSVERSIONINFOW ver = { 0 };
    ver.dwOSVersionInfoSize = sizeof(ver);
    if (RtlGetVersion(&ver) == 0) {
        return ver.dwBuildNumber;
    }

    return 0;
}

// memory

PalAllocator palGetDefaultAllocator()
{
    PalAllocator allocator;
    allocator.alloc = malloc;
    allocator.free = free;
    allocator.alignedAlloc = _aligned_malloc;
    allocator.alignedFree = _aligned_free;
    return allocator;
}

void _PCALL palSetMemory(void* memory, int value, Uint64 size)
{
    if (memory) {
        memset(memory, value, size);
    }
}

// thread

PalTLSID _PCALL palCreateTLS()
{
    DWORD slot = TlsAlloc();
    if (slot == TLS_OUT_OF_INDEXES) {
        return 0;
    }

    return slot;
}

bool _PCALL palDestroyTLS(PalTLSID id)
{
    return TlsFree((DWORD)id);
}

void* _PCALL palGetTLS(PalTLSID id)
{
    return TlsGetValue((DWORD)id);
}

bool _PCALL palSetTLS(PalTLSID id, void* data, void (*destructor)(void*))
{
    return TlsSetValue((DWORD)id, data);
}

// system

const char* palGetPlatformString()
{
    if (PAL_WINDOWS_11) {
        return "Windows 11";

    } else if (PAL_WINDOWS_10) {
        return "Windows 10";

    } else if (PAL_WINDOWS_8_1) {
        return "Windows 8.1";

    } else if (PAL_WINDOWS_8) {
        return "Windows 8";

    } else if (PAL_WINDOWS_7) {
        return "Windows 7";

    } else if (PAL_WINDOWS_VISTA) {
        return "Windows Vista";

    } else if (PAL_WINDOWS_XP) {
        return "Windows XP";
        
    } else {
        return PAL_NULL;
    }
}

// platform

void _palPlatformWriteConsole(Uint32 level, const char* msg)
{
    wchar_t buffer[_PAL_MSG_SIZE] = {};
    palToWstrUTF8Win32(buffer, msg);

    bool error = level > PAL_LOG_LEVEL_WARN;
    HANDLE console = PAL_NULL;
    static int levels[4] = { 8, 2, 6, 4 };

    if (error) {
        console = GetStdHandle(STD_ERROR_HANDLE);
    } else {
        console = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    SetConsoleTextAttribute(console, levels[level]);
    Uint64 len = wcslen(buffer);
    DWORD written = 0;
    WriteConsoleW(console, buffer, (DWORD)len, &written, 0);
    OutputDebugStringW(buffer);
}

bool _palPlatformVideoInit()
{
    s_HInstance = GetModuleHandleW(PAL_NULL);
    WNDCLASSEXW wc = {};
    wc.cbClsExtra = 0;
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.cbWndExtra = 0;
    wc.hbrBackground = NULL;
    wc.hCursor = LoadCursorW(s_HInstance, IDC_ARROW);
    wc.hIcon = LoadIconW(s_HInstance, IDI_APPLICATION);
    wc.hIconSm = LoadIconW(s_HInstance, IDI_APPLICATION);
    wc.hInstance = s_HInstance;
    wc.lpfnWndProc = palProcWin32;
    wc.lpszClassName = WIN32_CLASS;
    wc.lpszMenuName = NULL;
    wc.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

    ATOM success = RegisterClassExW(&wc);
    if (!success) {
        palSetError(PAL_PLATFORM_ERROR);
        return PAL_FALSE;
    }

    s_Shcore = LoadLibraryA("shcore.dll");
    if (!s_Shcore) {
        palSetError(PAL_PLATFORM_ERROR);
        return PAL_FALSE;
    }

    s_Ntdll = GetModuleHandleA("ntdll.dll");

    // // load function pointers
    s_GetDpiForMonitor = (GetDpiForMonitorFn)GetProcAddress(
        s_Shcore, 
        "GetDpiForMonitor"
    );

    s_SetProcessAwareness = (SetProcessAwarenessFn)GetProcAddress(
        s_Shcore, 
        "SetProcessDpiAwareness"
    );

    s_Video.displayCount = 0;
    EnumDisplayMonitors(PAL_NULL, PAL_NULL, palMonitorProcWin32, 0);
    return PAL_TRUE;
}

void _palPlatformVideoShutdown()
{
    if (s_HInstance) {
        UnregisterClassW(WIN32_CLASS, s_HInstance);
    }
}

BOOL CALLBACK palMonitorProcWin32(HMONITOR monitor, HDC, LPRECT, LPARAM)
{
    if (s_Video.displayCount >= _PAL_MAX_DISPLAYS) { 
        return PAL_FALSE; 
    }

    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        return TRUE; 
    }

    PalDisplay* display = &s_Video.displays[s_Video.displayCount];
    display->index = s_Video.displayCount;
    display->handle = (void*)monitor;

    // size and pos
    display->x = mi.rcMonitor.left;
    display->y = mi.rcMonitor.top;
    display->width = mi.rcMonitor.right - mi.rcMonitor.left;
    display->height = mi.rcMonitor.bottom - mi.rcWork.top;

    // change into a UTF8 string
    WideCharToMultiByte(CP_UTF8, 0, mi.szDevice, -1, display->name, 32, NULL, NULL);
    getDisplayDPI(display);

    // get modes
    DEVMODEW dm = {};
    dm.dmSize = sizeof(DEVMODE);
    int i = 0;
    for (i = 0; EnumDisplaySettingsW(mi.szDevice, i, &dm); i++) {
        if (i >= PAL_MAX_MODES) {
            break;
        }

        PalDisplayMode mode;
        mode.refreshRate = dm.dmDisplayFrequency;
        mode.width = dm.dmPelsWidth;
        mode.height = dm.dmPelsHeight;
        getColorBits(&mode, dm.dmBitsPerPel);
        addMode(&mode);
    }

    display->modeCount--;
    display->refreshRate = display->modes[display->modeCount].refreshRate;
    s_Video.displayCount++;
    return PAL_TRUE;
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

void addMode(const PalDisplayMode* mode)
{
    PalDisplay* display = &s_Video.displays[s_Video.displayCount];
    for (int i = 0; i < display->modeCount; i++) {
        if (compareMode(&display->modes[i], mode) == PAL_TRUE) {
            return;
        }
    }

    display->modes[display->modeCount] = *mode;
    display->modeCount++;
}

bool compareMode(const PalDisplayMode* a, const PalDisplayMode* b)
{
    int same = a->alphaBits == b->alphaBits &&
           a->redBits == b->redBits         &&
           a->greenBits == b->greenBits     &&
           a->blueBits == b->blueBits       &&
           a->alphaBits == b->alphaBits     &&
           a->width == b->width             &&
           a->height == b->height           &&
           a->refreshRate == b->refreshRate;

    if (same) {
        return PAL_TRUE;
    }
    return PAL_FALSE;
}

void getDisplayDPI(PalDisplay* display)
{
    UINT dpiX, dpiY;
    if (s_SetProcessAwareness) {
        s_SetProcessAwareness(WIN32_DPI_AWARE);
    }

    if (s_GetDpiForMonitor) {
        s_GetDpiForMonitor(
            (HMONITOR)display->handle,
            WIN32_DPI,
            &dpiX,
            &dpiY
        );

        display->dpiScaleX = dpiX / 96.0f;
        display->dpiScaleY = dpiY / 96.0f;

    } else {
        display->dpiScaleX = 1.0f;
        display->dpiScaleY = 1.0f;
    }
}