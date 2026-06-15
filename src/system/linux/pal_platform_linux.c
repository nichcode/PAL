/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef __linux__
#define _POSIX_C_SOURCE 200112L
#include "pal_shared.h"
#include "pal/pal_system.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>

PalResult PAL_CALL palGetPlatformInfo(PalPlatformInfo* info)
{
    if (!info) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    info->type = PAL_PLATFORM_LINUX;
    const char* session = getenv("XDG_SESSION_TYPE");
    if (session) {
        if (strcmp(session, "wayland") == 0) {
            info->apiType = PAL_PLATFORM_API_WAYLAND;
        } else {
            info->apiType = PAL_PLATFORM_API_X11;
        }
    } else {
        // default
        info->apiType = PAL_PLATFORM_API_X11;
    }

    FILE* file = fopen("/etc/os-release", "r");
    if (!file) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    char line[256];
    char name[15];
    char version[15];
    // parse version and name from the release file
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "NAME=", 5) == 0) {
            sscanf(line, "NAME=\"%15[^\"]", name);

        } else if (strncmp(line, "VERSION_ID=", 11) == 0) {
            sscanf(line, "VERSION_ID=\"%15[^\"]", version);
        }
    }

    // combine to get name
    snprintf(info->name, PAL_PLATFORM_NAME_SIZE, "%s %s", name, version);
    sscanf(version, "%d.%d", &info->version.major, &info->version.minor);
    fclose(file);

    // get total memory and disk space for the root drive
    struct sysinfo sysInfo;
    sysinfo(&sysInfo);
    info->totalRAM = (uint32_t)(sysInfo.totalram / (1024 * 1024));

    struct statvfs stats;
    if (statvfs("/", &stats) != 0) {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    uint64_t size = (stats.f_blocks * stats.f_frsize) / (1024 * 1024 * 1024);
    info->totalMemory = (uint32_t)size;
    return PAL_RESULT_SUCCESS;
}

#endif // __linux__