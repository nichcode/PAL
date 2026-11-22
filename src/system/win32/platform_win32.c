
/**
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

#include "system/platform.h"
#include "pal2/core/library.h"
#include "pal2/system/platform.h"
#include <stdio.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

typedef LONG(WINAPI* RtlGetVersionFn)(PRTL_OSVERSIONINFOW info);

static PalBool getPlatformVersion(PalVersion* version)
{
    RtlGetVersionFn getVersion = nullptr;
    OSVERSIONINFOEXW info = {0};
    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);

    /** We use the library API from the core system to silence GCC and Clang
     * warnings. 
     */
    PalLibrary* ntdll = palLoadLibrary("ntdll.dll");
    if (!ntdll) {
        return PAL_FALSE;
    }

    getVersion = (RtlGetVersionFn)palGetSymbol(ntdll, "RtlGetVersion");
    if (!getVersion) {
        palFreeLibrary(ntdll);
        return PAL_FALSE;
    }

    if (getVersion((PRTL_OSVERSIONINFOW)&info) != 0) {
        palFreeLibrary(ntdll);
        return PAL_FALSE;
    }

    version->major = info.dwMajorVersion;
    version->minor = info.dwMinorVersion;
    version->build = info.dwBuildNumber;

    palFreeLibrary(ntdll);

    return PAL_TRUE;
}

static PalBool isVersion(
    const PalVersion* osVersion,
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

void platformGetPlatformInfo(PalPlatformInfo* info)
{
    info->apiType = PAL_PLATFORM_API_TYPE_WIN32;
    info->type = PAL_PLATFORM_TYPE_WINDOWS;

    /** We follow the rules of system_platform.h to only get C drive size.*/
    ULARGE_INTEGER free, total, available;
    if (GetDiskFreeSpaceExW(L"C:\\", &available, &total, &free)) {
        info->totalMemory = (uint32_t)(total.QuadPart / (1024ULL * 1024ULL * 1024ULL));
    }

    MEMORYSTATUSEX status = {0};
    status.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&status)) {
        info->totalRAM = (uint32_t)(status.ullTotalPhys / (1024ULL * 1024ULL));
    }

    if (!getPlatformVersion(&info->version)) {
        snprintf(info->name, PAL_PLATFORM_NAME_SIZE, "Windows");
        return;
    }

    const char* name = "Windows";

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
    }

    snprintf(info->name, PAL_PLATFORM_NAME_SIZE, "%s", name);
}

#endif // _WIN32