
/**
 * PAL - Prime Abstraction Layer (PAL)
 * A cross platform abstraction layer over graphics and windowing APIs
 * -------------------------------------------------------------------
 *
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "shared.h"
#include "system/system_platform.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

typedef LONG(WINAPI* RtlGetVersionFn)(PRTL_OSVERSIONINFOW info);

static bool getPlatformVersion(PalVersion* version)
{
    RtlGetVersionFn getVersion = nullptr;
    OSVERSIONINFOEXW info = {0};
    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);

    /** We use the library API from the core system to silence GCC and Clang
     * warnings. 
     */
    PalLibrary* ntdll = palLoadLibrary("ntdll.dll");
    if (!ntdll) {
        return false;
    }

    getVersion = (RtlGetVersionFn)palGetSymbol(ntdll, "RtlGetVersion");
    if (!getVersion) {
        palFreeLibrary(ntdll);
        return false;
    }

    if (getVersion((PRTL_OSVERSIONINFOW)&info)) {
        palFreeLibrary(ntdll);
        return false;
    }

    version->major = info.dwMajorVersion;
    version->minor = info.dwMinorVersion;
    version->build = info.dwBuildNumber;

    palFreeLibrary(ntdll);

    return true;
}

static bool isVersion(
    PalVersion* osVersion,
    uint32_t major,
    uint32_t minor,
    uint32_t build)
{
    if (osVersion->major > major) {
        return true;
    }

    if (osVersion->major < major) {
        return false;
    }

    if (osVersion->minor > minor) {
        return true;
    }

    if (osVersion->minor < minor) {
        return false;
    }

    return osVersion->build >= build;
}

void platformGetPlatformInfo(PalPlatformInfo* info)
{
    info->apiType = PAL_PLATFORM_API_TYPE_WIN32;
    info->type = PAL_PLATFORM_TYPE_WINDOWS;

    /** We follow the rules of system_platform.h to only get C drive size.*/
    ULARGE_INTEGER free, total, available;
    if (GetDiskFreeSpaceExW(L"C:\\", &available, &total, &free)) {
        info->totalMemory = (uint32_t)(total.QuadPart / (1024 * 1024 * 1024));
    }

    MEMORYSTATUSEX status = {0};
    status.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&status)) {
        info->totalRAM = (uint32_t)(status.ullTotalPhys / (1024 * 1024));
    }

    if (!getPlatformVersion(&info->version)) {
        return;
    }

    const char* name = nullptr;
    const char* build = nullptr;

    if (isVersion(&info->version, 5, 1, 0)) {
        name = "Windows XP";
    }

    if (isVersion(&info->version, 6, 0, 0)) {
        name = "Windows Vista";
    }

    if (isVersion(&info->version, 6, 1, 0)) {
        name = "Windows 7";
    }

    if (isVersion(&info->version, 6, 2, 0)) {
        name = "Windows 8";
    }

    if (isVersion(&info->version, 6, 3, 0)) {
        name = "Windows 8.1";
    }

    if (isVersion(&info->version, 10, 0, 0)) {
        name = "Windows 10";
    }

    if (isVersion(&info->version, 10, 0, 22000)) {
        name = "Windows 11";
        build = ".22000";
    }

    strcpy(info->name, name);
    if (build) {
        strcat(info->name, build);
    }
}

#endif // _WIN32