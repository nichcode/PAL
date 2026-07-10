
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

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

#include "pal/pal_system.h"
#include <string.h>
#include <windows.h>

typedef LONG(WINAPI* RtlGetVersionFn)(PRTL_OSVERSIONINFOW);

static inline PalBool getVersionWin32(PalVersion* version)
{
    OSVERSIONINFOEXW ver = {0};
    ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
    HINSTANCE ntdll = GetModuleHandleW(L"ntdll.dll");
    RtlGetVersionFn getVer = (RtlGetVersionFn)GetProcAddress(ntdll, "RtlGetVersion");
    if (!getVer) {
        return PAL_FALSE;
    }

    if (getVer((PRTL_OSVERSIONINFOW)&ver)) {
        return PAL_FALSE;
    }

    version->major = ver.dwMajorVersion;
    version->minor = ver.dwMinorVersion;
    version->build = ver.dwBuildNumber;
    return PAL_TRUE;
}

static inline PalBool isVersionWin32(
    PalVersion* osVersion,
    uint32_t major,
    uint32_t minor,
    uint32_t build)
{
    if (osVersion->major > major) {
        return PAL_TRUE;
    }

    if (osVersion->major < major) {
        return PAL_FALSE;
    }

    if (osVersion->minor > minor) {
        return PAL_TRUE;
    }

    if (osVersion->minor < minor) {
        return PAL_FALSE;
    }

    return osVersion->build >= build;
}

void PAL_CALL palGetPlatformInfo(PalPlatformInfo* info)
{
    info->apiType = PAL_PLATFORM_API_TYPE_WIN32;
    info->type = PAL_PLATFORM_TYPE_WINDOWS;

    // get total disk memory (size) in GB
    ULARGE_INTEGER free, total, available;
    if (GetDiskFreeSpaceExW(L"C:\\", &available, &total, &free)) {
        info->totalMemory = (uint32_t)(total.QuadPart / (1024 * 1024 * 1024));
    }

    // get ram (size) in MB
    MEMORYSTATUSEX status = {0};
    status.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&status)) {
        info->totalRAM = (uint32_t)(status.ullTotalPhys / (1024 * 1024));
    }

    // get windows build, version and combine them
    if (!getVersionWin32(&info->version)) {
        return;
    }

    const char* name = nullptr;
    const char* build = nullptr;
    // check the versions and set the appropriate name
    if (isVersionWin32(&info->version, 5, 1, 0)) {
        name = "Windows XP";
    }

    if (isVersionWin32(&info->version, 6, 0, 0)) {
        name = "Windows Vista";
    }

    if (isVersionWin32(&info->version, 6, 1, 0)) {
        name = "Windows 7";
    }

    if (isVersionWin32(&info->version, 6, 2, 0)) {
        name = "Windows 8";
    }

    if (isVersionWin32(&info->version, 6, 3, 0)) {
        name = "Windows 8.1";
    }

    if (isVersionWin32(&info->version, 10, 0, 0)) {
        name = "Windows 10";
    }

    if (isVersionWin32(&info->version, 10, 0, 22000)) {
        name = "Windows 11";
        build = ".22000";
    }

    // combine them into a single string
    strcpy(info->name, name);
    if (build) {
        strcat(info->name, build);
    }
}

#endif // _WIN32