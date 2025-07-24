
#include "pal_pch.h"
#include "platform/pal_platform.h"

// ==================================================
// Internal API
// ==================================================

palVersionsWin32 g_VersionsWin32;

typedef LONG (WINAPI *RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

bool palGetOsVersionWin32(PalVersion* version) {

    OSVERSIONINFOEXW ver = { 0 };
    ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);

    HINSTANCE ntdll = GetModuleHandleW(L"ntdll.dll");
    RtlGetVersionPtr getVer = (RtlGetVersionPtr)GetProcAddress(ntdll, "RtlGetVersion");
    if (!getVer) {
        return PAL_FALSE;
    }

    if (getVer((PRTL_OSVERSIONINFOW)&ver)) {
        return PAL_FALSE;
    }

    version->major = ver.dwMajorVersion;
    version->minor = ver.dwMinorVersion;
    version->patch = ver.dwBuildNumber;

    if (g_VersionsWin32.window10.major) {
        return PAL_TRUE;
    }

    // windows XP
    g_VersionsWin32.windowXP = (PalVersion) {
        .major = 5,
        .minor = 1,
        .patch = 0
    };

    // windows Vista
    g_VersionsWin32.windowVista = (PalVersion) {
        .major = 6,
        .minor = 0,
        .patch = 0
    };

    // windows 7
    g_VersionsWin32.window7 = (PalVersion) {
        .major = 6,
        .minor = 1,
        .patch = 0
    };

    // windows 8
    g_VersionsWin32.window8 = (PalVersion) {
        .major = 6,
        .minor = 2,
        .patch = 0
    };

    // windows 8_1
    g_VersionsWin32.window8_1 = (PalVersion) {
        .major = 6,
        .minor = 3,
        .patch = 0
    };

    // windows 10
    g_VersionsWin32.window10 = (PalVersion) {
        .major = 10,
        .minor = 0,
        .patch = 0
    };

    // windows 11
    g_VersionsWin32.window11 = (PalVersion) {
        .major = 10,
        .minor = 0,
        .patch = 22000
    };

    return PAL_TRUE;
}

bool palIsOsVersionWin32(PalVersion* osVersion, PalVersion* version) {

    if (osVersion->major > version->major) {
        return PAL_TRUE;
    }

    if (osVersion->major < version->major) {
        return PAL_FALSE;
    }

    if (osVersion->minor > version->minor) {
        return PAL_TRUE;
    }

    if (osVersion->minor < version->minor) {
        return PAL_FALSE;
    }

    return osVersion->patch >= version->patch;
}

int palStringToWideString(wchar_t* buffer, const char* string) {

    int len = MultiByteToWideChar(CP_UTF8, 0, string, -1, PAL_NULL, 0);
    MultiByteToWideChar(CP_UTF8, 0, string, -1, buffer, len);
    return len;
}

void palWriteConsole(PalLogLevel level, wchar_t* buffer, const char* msg) {

    HANDLE console = PAL_NULL;
    bool error = level > PAL_LOG_LEVEL_WARN;
    static int levels[4] = { 8, 2, 6, 4 };
    if (error) {
        console = GetStdHandle(STD_ERROR_HANDLE);
    } else {
        console = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    int len = palStringToWideString(buffer, msg);
    SetConsoleTextAttribute(console, levels[level]);
    DWORD written = 0;
    WriteConsoleW(console, buffer, (DWORD)len - 1, &written, 0);
    OutputDebugStringW(buffer);
}