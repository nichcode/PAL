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
#include "pal2/system/platform.h"

#ifdef __linux__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>

static void parseOSReleaseValue(const char* src, char* dst, size_t dstSize)
{
    if (dstSize == 0) {
        return;
    }

    if (*src == '"') {
        src++;
    }

    size_t i = 0;
    while (src[i] != '\0' && src[i] != '"' && src[i] != '\n' && i < dstSize - 1) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

void platformGetPlatformInfo(PalPlatformInfo* info)
{
    info->type = PAL_PLATFORM_TYPE_LINUX;

    const char* session = getenv("XDG_SESSION_TYPE");
    if (session && strcmp(session, "wayland") == 0) {
        info->apiType = PAL_PLATFORM_API_TYPE_WAYLAND;
    } else if (!session && getenv("WAYLAND_DISPLAY") != nullptr) {
        info->apiType = PAL_PLATFORM_API_TYPE_WAYLAND;
    } else {
        info->apiType = PAL_PLATFORM_API_TYPE_X11;
    }

    char name[32] = {0};
    char version[16] = {0};

    FILE* file = fopen("/etc/os-release", "r");
    if (file) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            if (strncmp(line, "NAME=", 5) == 0) {
                parseOSReleaseValue(line + 5, name, sizeof(name));
            } else if (strncmp(line, "VERSION_ID=", 11) == 0) {
                parseOSReleaseValue(line + 11, version, sizeof(version));
            }
        }
        fclose(file);
    }

    if (name[0] != '\0' && version[0] != '\0') {
        snprintf(info->name, PAL_PLATFORM_NAME_SIZE, "%.15s %.15s", name, version);
    } else if (name[0] != '\0') {
        snprintf(info->name, PAL_PLATFORM_NAME_SIZE, "%s", name);
    } else {
        snprintf(info->name, PAL_PLATFORM_NAME_SIZE, "Linux");
    }

    if (version[0] != '\0') {
        sscanf(version, "%u.%u", &info->version.major, &info->version.minor);
    }

    struct sysinfo sysInfo;
    if (sysinfo(&sysInfo) == 0) {
        uint64_t totalBytes = (uint64_t)sysInfo.totalram * (uint64_t)sysInfo.mem_unit;
        info->totalRAM = (uint32_t)(totalBytes / (1024ULL * 1024ULL));
    }

    struct statvfs stats;
    if (statvfs("/", &stats) == 0) {
        uint64_t size = ((uint64_t)stats.f_blocks * (uint64_t)stats.f_frsize) / (1024ULL * 1024ULL * 1024ULL);
        info->totalMemory = (uint32_t)size;
    }
}

#endif // __linux__