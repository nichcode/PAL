
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

#ifdef __linux__
#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>

void platformGetPlatformInfo(PalPlatformInfo* info)
{
    info->type = PAL_PLATFORM_TYPE_LINUX;

    const char* session = getenv("XDG_SESSION_TYPE");
    if (session) {
        if (strcmp(session, "wayland") == 0) {
            info->apiType = PAL_PLATFORM_API_TYPE_WAYLAND;
        } else {
            info->apiType = PAL_PLATFORM_API_TYPE_X11;
        }
    } else {
        info->apiType = PAL_PLATFORM_API_TYPE_X11;
    }

    FILE* file = fopen("/etc/os-release", "r");
    if (!file) {
        return;
    }

    char line[256];
    char name[16];
    char version[16];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "NAME=", 5) == 0) {
            sscanf(line, "NAME=\"%15[^\"]", name);

        } else if (strncmp(line, "VERSION_ID=", 11) == 0) {
            sscanf(line, "VERSION_ID=\"%15[^\"]", version);
        }
    }

    snprintf(info->name, PAL_PLATFORM_NAME_SIZE, "%s %s", name, version);
    sscanf(version, "%u.%u", &info->version.major, &info->version.minor);

    fclose(file);

    struct sysinfo sysInfo;
    sysinfo(&sysInfo);
    info->totalRAM = (uint32_t)(sysInfo.totalram / (1024 * 1024));

    struct statvfs stats;
    if (statvfs("/", &stats) != 0) {
        return;
    }

    uint64_t size = (stats.f_blocks * stats.f_frsize) / (1024 * 1024 * 1024);
    info->totalMemory = (uint32_t)size;
}

#endif // __linux__